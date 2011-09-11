/*
** ARM IR assembler (SSA IR -> machine code).
** Copyright (C) 2005-2011 Mike Pall. See Copyright Notice in luajit.h
*/

/* -- Register allocator extensions --------------------------------------- */

/* Allocate a register with a hint. */
static Reg ra_hintalloc(ASMState *as, IRRef ref, Reg hint, RegSet allow)
{
  Reg r = IR(ref)->r;
  if (ra_noreg(r)) {
    if (!ra_hashint(r) && !iscrossref(as, ref))
      ra_sethint(IR(ref)->r, hint);  /* Propagate register hint. */
    r = ra_allocref(as, ref, allow);
  }
  ra_noweak(as, r);
  return r;
}

/* Similar to ra_left, except we override any hints. */
static void ra_leftov(ASMState *as, Reg dest, IRRef lref)
{
  IRIns *ir = IR(lref);
  Reg left = ir->r;
  if (ra_noreg(left)) {
    ra_sethint(ir->r, dest);  /* Propagate register hint. */
    left = ra_allocref(as, lref, RSET_GPR);
  }
  ra_noweak(as, left);
  if (dest != left) {
    /* Use register renaming if dest is the PHI reg. */
    if (irt_isphi(ir->t) && as->phireg[dest] == lref) {
      ra_modified(as, left);
      ra_rename(as, left, dest);
    } else {
      emit_movrr(as, ir, dest, left);
    }
  }
}

/* Allocate a scratch register pair. */
static Reg ra_scratchpair(ASMState *as, RegSet allow)
{
  RegSet pick1 = as->freeset & allow;
  RegSet pick2 = pick1 & (pick1 >> 1) & RSET_GPREVEN;
  Reg r;
  if (pick2) {
    r = rset_picktop(pick2);
  } else {
    RegSet pick = pick1 & (allow >> 1) & RSET_GPREVEN;
    if (pick) {
      r = rset_picktop(pick);
      ra_restore(as, regcost_ref(as->cost[r+1]));
    } else {
      pick = pick1 & (allow << 1) & RSET_GPRODD;
      if (pick) {
	r = ra_restore(as, regcost_ref(as->cost[rset_picktop(pick)-1]));
      } else {
	r = ra_evict(as, allow & (allow >> 1) & RSET_GPREVEN);
	ra_restore(as, regcost_ref(as->cost[r+1]));
      }
    }
  }
  lua_assert(rset_test(RSET_GPREVEN, r));
  ra_modified(as, r);
  ra_modified(as, r+1);
  RA_DBGX((as, "scratchpair    $r $r", r, r+1));
  return r;
}

/* Force a RID_RET/RID_RETHI destination register pair (marked as free). */
static void ra_destpair(ASMState *as, IRIns *ir)
{
  Reg destlo = ir->r, desthi = (ir+1)->r;
  /* First spill unrelated refs blocking the destination registers. */
  if (!rset_test(as->freeset, RID_RET) &&
      destlo != RID_RET && desthi != RID_RET)
    ra_restore(as, regcost_ref(as->cost[RID_RET]));
  if (!rset_test(as->freeset, RID_RETHI) &&
      destlo != RID_RETHI && desthi != RID_RETHI)
    ra_restore(as, regcost_ref(as->cost[RID_RETHI]));
  /* Next free the destination registers (if any). */
  if (ra_hasreg(destlo)) {
    ra_free(as, destlo);
    ra_modified(as, destlo);
  } else {
    destlo = RID_RET;
  }
  if (ra_hasreg(desthi)) {
    ra_free(as, desthi);
    ra_modified(as, desthi);
  } else {
    desthi = RID_RETHI;
  }
  /* Check for conflicts and shuffle the registers as needed. */
  if (destlo == RID_RETHI) {
    if (desthi == RID_RET) {
      emit_movrr(as, ir, RID_RETHI, RID_TMP);
      emit_movrr(as, ir, RID_RET, RID_RETHI);
      emit_movrr(as, ir, RID_TMP, RID_RET);
    } else {
      emit_movrr(as, ir, RID_RETHI, RID_RET);
      if (desthi != RID_RETHI) emit_movrr(as, ir, desthi, RID_RETHI);
    }
  } else if (desthi == RID_RET) {
    emit_movrr(as, ir, RID_RET, RID_RETHI);
    if (destlo != RID_RET) emit_movrr(as, ir, destlo, RID_RET);
  } else {
    if (desthi != RID_RETHI) emit_movrr(as, ir, desthi, RID_RETHI);
    if (destlo != RID_RET) emit_movrr(as, ir, destlo, RID_RET);
  }
  /* Restore spill slots (if any). */
  if (ra_hasspill((ir+1)->s)) ra_save(as, ir+1, RID_RETHI);
  if (ra_hasspill(ir->s)) ra_save(as, ir, RID_RET);
}

/* -- Guard handling ------------------------------------------------------ */

/* Generate an exit stub group at the bottom of the reserved MCode memory. */
static MCode *asm_exitstub_gen(ASMState *as, ExitNo group)
{
  MCode *mxp = as->mcbot;
  int i;
  if (mxp + 4*4+4*EXITSTUBS_PER_GROUP >= as->mctop)
    asm_mclimit(as);
  /* str lr, [sp]; bl ->vm_exit_handler; .long DISPATCH_address, group. */
  *mxp++ = ARMI_STR|ARMI_LS_P|ARMI_LS_U|ARMF_D(RID_LR)|ARMF_N(RID_SP);
  *mxp = ARMI_BL|((((MCode *)(void *)lj_vm_exit_handler-mxp)-2)&0x00ffffffu);
  mxp++;
  *mxp++ = (MCode)i32ptr(J2GG(as->J)->dispatch);  /* DISPATCH address */
  *mxp++ = group*EXITSTUBS_PER_GROUP;
  for (i = 0; i < EXITSTUBS_PER_GROUP; i++)
    *mxp++ = ARMI_B|((-6-i)&0x00ffffffu);
  lj_mcode_commitbot(as->J, mxp);
  as->mcbot = mxp;
  as->mclim = as->mcbot + MCLIM_REDZONE;
  return mxp - EXITSTUBS_PER_GROUP;
}

/* Setup all needed exit stubs. */
static void asm_exitstub_setup(ASMState *as, ExitNo nexits)
{
  ExitNo i;
  if (nexits >= EXITSTUBS_PER_GROUP*LJ_MAX_EXITSTUBGR)
    lj_trace_err(as->J, LJ_TRERR_SNAPOV);
  for (i = 0; i < (nexits+EXITSTUBS_PER_GROUP-1)/EXITSTUBS_PER_GROUP; i++)
    if (as->J->exitstubgroup[i] == NULL)
      as->J->exitstubgroup[i] = asm_exitstub_gen(as, i);
}

/* Emit conditional branch to exit for guard. */
static void asm_guardcc(ASMState *as, ARMCC cc)
{
  MCode *target = exitstub_addr(as->J, as->snapno);
  MCode *p = as->mcp;
  if (LJ_UNLIKELY(p == as->invmcp)) {
    as->loopinv = 1;
    *p = ARMI_BL | ((target-p-2) & 0x00ffffffu);
    emit_branch(as, ARMF_CC(ARMI_B, cc^1), p+1);
    return;
  }
  emit_branch(as, ARMF_CC(ARMI_BL, cc), target);
}

/* -- Operand fusion ------------------------------------------------------ */

/* Limit linear search to this distance. Avoids O(n^2) behavior. */
#define CONFLICT_SEARCH_LIM	31

/* Check if there's no conflicting instruction between curins and ref. */
static int noconflict(ASMState *as, IRRef ref, IROp conflict)
{
  IRIns *ir = as->ir;
  IRRef i = as->curins;
  if (i > ref + CONFLICT_SEARCH_LIM)
    return 0;  /* Give up, ref is too far away. */
  while (--i > ref)
    if (ir[i].o == conflict)
      return 0;  /* Conflict found. */
  return 1;  /* Ok, no conflict. */
}

/* Fuse the array base of colocated arrays. */
static int32_t asm_fuseabase(ASMState *as, IRRef ref)
{
  IRIns *ir = IR(ref);
  if (ir->o == IR_TNEW && ir->op1 <= LJ_MAX_COLOSIZE &&
      noconflict(as, ref, IR_NEWREF))
    return (int32_t)sizeof(GCtab);
  return 0;
}

/* Fuse array/hash/upvalue reference into register+offset operand. */
static Reg asm_fuseahuref(ASMState *as, IRRef ref, int32_t *ofsp, RegSet allow)
{
  IRIns *ir = IR(ref);
  if (ra_noreg(ir->r)) {
    if (ir->o == IR_AREF) {
      if (mayfuse(as, ref)) {
	if (irref_isk(ir->op2)) {
	  IRRef tab = IR(ir->op1)->op1;
	  int32_t ofs = asm_fuseabase(as, tab);
	  IRRef refa = ofs ? tab : ir->op1;
	  ofs += 8*IR(ir->op2)->i;
	  if (ofs > -4096 && ofs < 4096) {
	    *ofsp = ofs;
	    return ra_alloc1(as, refa, allow);
	  }
	}
      }
    } else if (ir->o == IR_HREFK) {
      if (mayfuse(as, ref)) {
	int32_t ofs = (int32_t)(IR(ir->op2)->op2 * sizeof(Node));
	if (ofs < 4096) {
	  *ofsp = ofs;
	  return ra_alloc1(as, ir->op1, allow);
	}
      }
    } else if (ir->o == IR_UREFC) {
      if (irref_isk(ir->op1)) {
	GCfunc *fn = ir_kfunc(IR(ir->op1));
	int32_t ofs = i32ptr(&gcref(fn->l.uvptr[(ir->op2 >> 8)])->uv.tv);
	*ofsp = (ofs & 255);  /* Mask out less bits to allow LDRD. */
	return ra_allock(as, (ofs & ~255), allow);
      }
    }
  }
  *ofsp = 0;
  return ra_alloc1(as, ref, allow);
}

/* Fuse m operand into arithmetic/logic instructions. */
static uint32_t asm_fuseopm(ASMState *as, ARMIns ai, IRRef ref, RegSet allow)
{
  IRIns *ir = IR(ref);
  if (ra_hasreg(ir->r)) {
    ra_noweak(as, ir->r);
    return ARMF_M(ir->r);
  } else if (irref_isk(ref)) {
    uint32_t k = emit_isk12(ai, ir->i);
    if (k)
      return k;
  } else if (mayfuse(as, ref)) {
    if (ir->o >= IR_BSHL && ir->o <= IR_BROR) {
      Reg m = ra_alloc1(as, ir->op1, allow);
      ARMShift sh = ir->o == IR_BSHL ? ARMSH_LSL :
		    ir->o == IR_BSHR ? ARMSH_LSR :
		    ir->o == IR_BSAR ? ARMSH_ASR : ARMSH_ROR;
      if (irref_isk(ir->op2)) {
	return m | ARMF_SH(sh, (IR(ir->op2)->i & 31));
      } else {
	Reg s = ra_alloc1(as, ir->op2, rset_exclude(allow, m));
	return m | ARMF_RSH(sh, s);
      }
    } else if (ir->o == IR_ADD && ir->op1 == ir->op2) {
      Reg m = ra_alloc1(as, ir->op1, allow);
      return m | ARMF_SH(ARMSH_LSL, 1);
    }
  }
  return ra_allocref(as, ref, allow);
}

/* Fuse shifts into loads/stores. Only bother with BSHL 2 => lsl #2. */
static IRRef asm_fuselsl2(ASMState *as, IRRef ref)
{
  IRIns *ir = IR(ref);
  if (ra_noreg(ir->r) && mayfuse(as, ref) && ir->o == IR_BSHL &&
      irref_isk(ir->op2) && IR(ir->op2)->i == 2)
    return ir->op1;
  return 0;  /* No fusion. */
}

/* Fuse XLOAD/XSTORE reference into load/store operand. */
static void asm_fusexref(ASMState *as, ARMIns ai, Reg rd, IRRef ref,
			 RegSet allow)
{
  IRIns *ir = IR(ref);
  int32_t ofs = 0;
  Reg base;
  if (ra_noreg(ir->r) && mayfuse(as, ref)) {
    int32_t lim = (ai & 0x04000000) ? 4096 : 256;
    if (ir->o == IR_ADD) {
      if (irref_isk(ir->op2) && (ofs = IR(ir->op2)->i) > -lim && ofs < lim) {
	ref = ir->op1;
      } else {
	IRRef lref = ir->op1, rref = ir->op2;
	Reg rn, rm;
	if ((ai & 0x04000000)) {
	  IRRef sref = asm_fuselsl2(as, rref);
	  if (sref) {
	    rref = sref;
	    ai |= ARMF_SH(ARMSH_LSL, 2);
	  } else if ((sref = asm_fuselsl2(as, lref)) != 0) {
	    lref = rref;
	    rref = sref;
	    ai |= ARMF_SH(ARMSH_LSL, 2);
	  }
	}
	rn = ra_alloc1(as, lref, allow);
	rm = ra_alloc1(as, rref, rset_exclude(allow, rn));
	if ((ai & 0x04000000)) ai |= ARMI_LS_R;
	emit_dnm(as, ai|ARMI_LS_P|ARMI_LS_U, rd, rn, rm);
	return;
      }
    } else if (ir->o == IR_STRREF) {
      ofs = (int32_t)sizeof(GCstr);
      if (irref_isk(ir->op2)) {
	ofs += IR(ir->op2)->i;
	ref = ir->op1;
      } else if (irref_isk(ir->op1)) {
	ofs += IR(ir->op1)->i;
	ref = ir->op2;
      } else {
	/* NYI: Fuse ADD with constant. */
	Reg rn = ra_alloc1(as, ir->op1, allow);
	uint32_t m = asm_fuseopm(as, 0, ir->op2, rset_exclude(allow, rn));
	if ((ai & 0x04000000))
	  emit_lso(as, ai, rd, rd, ofs);
	else
	  emit_lsox(as, ai, rd, rd, ofs);
	emit_dn(as, ARMI_ADD^m, rd, rn);
	return;
      }
      if (ofs <= -lim || ofs >= lim) {
	Reg rn = ra_alloc1(as, ref, allow);
	Reg rm = ra_allock(as, ofs, rset_exclude(allow, rn));
	if ((ai & 0x04000000)) ai |= ARMI_LS_R;
	emit_dnm(as, ai|ARMI_LS_P|ARMI_LS_U, rd, rn, rm);
	return;
      }
    }
  }
  base = ra_alloc1(as, ref, allow);
  if ((ai & 0x04000000))
    emit_lso(as, ai, rd, base, ofs);
  else
    emit_lsox(as, ai, rd, base, ofs);
}

/* -- Calls --------------------------------------------------------------- */

/* Generate a call to a C function. */
static void asm_gencall(ASMState *as, const CCallInfo *ci, IRRef *args)
{
  uint32_t n, nargs = CCI_NARGS(ci);
  int32_t ofs = 0;
  Reg gpr = REGARG_FIRSTGPR;
  if ((void *)ci->func)
    emit_call(as, (void *)ci->func);
  for (n = 0; n < nargs; n++) {  /* Setup args. */
    IRRef ref = args[n];
    IRIns *ir = IR(ref);
    if (gpr <= REGARG_LASTGPR) {
      lua_assert(rset_test(as->freeset, gpr));  /* Must have been evicted. */
      if (ref) ra_leftov(as, gpr, ref);
      gpr++;
    } else {
      if (ref) {
	Reg r = ra_alloc1(as, ref, RSET_GPR);
	emit_spstore(as, ir, r, ofs);
      }
      ofs += 4;
    }
  }
}

/* Setup result reg/sp for call. Evict scratch regs. */
static void asm_setupresult(ASMState *as, IRIns *ir, const CCallInfo *ci)
{
  RegSet drop = RSET_SCRATCH;
  int hiop = ((ir+1)->o == IR_HIOP);
  if (ra_hasreg(ir->r))
    rset_clear(drop, ir->r);  /* Dest reg handled below. */
  if (hiop && ra_hasreg((ir+1)->r))
    rset_clear(drop, (ir+1)->r);  /* Dest reg handled below. */
  ra_evictset(as, drop);  /* Evictions must be performed first. */
  if (ra_used(ir)) {
    lua_assert(!irt_ispri(ir->t));
    if (hiop)
      ra_destpair(as, ir);
    else
      ra_destreg(as, ir, RID_RET);
  }
  UNUSED(ci);
}

static void asm_call(ASMState *as, IRIns *ir)
{
  IRRef args[CCI_NARGS_MAX];
  const CCallInfo *ci = &lj_ir_callinfo[ir->op2];
  asm_collectargs(as, ir, ci, args);
  asm_setupresult(as, ir, ci);
  asm_gencall(as, ci, args);
}

static void asm_callx(ASMState *as, IRIns *ir)
{
  IRRef args[CCI_NARGS_MAX];
  CCallInfo ci;
  ci.flags = asm_callx_flags(as, ir);
  asm_collectargs(as, ir, &ci, args);
  asm_setupresult(as, ir, &ci);
  if (irref_isk(ir->op2)) {  /* Call to constant address. */
    ci.func = (ASMFunction)(void *)(IR(ir->op2)->i);
  } else {  /* Need a non-argument register for indirect calls. */
    Reg freg = ra_alloc1(as, ir->op2, RSET_RANGE(RID_R4, RID_R12+1));
    emit_m(as, ARMI_BLXr, freg);
    ci.func = (ASMFunction)(void *)0;
  }
  asm_gencall(as, &ci, args);
}

/* -- Returns ------------------------------------------------------------- */

/* Return to lower frame. Guard that it goes to the right spot. */
static void asm_retf(ASMState *as, IRIns *ir)
{
  Reg base = ra_alloc1(as, REF_BASE, RSET_GPR);
  void *pc = ir_kptr(IR(ir->op2));
  int32_t delta = 1+bc_a(*((const BCIns *)pc - 1));
  as->topslot -= (BCReg)delta;
  if ((int32_t)as->topslot < 0) as->topslot = 0;
  /* Need to force a spill on REF_BASE now to update the stack slot. */
  emit_lso(as, ARMI_STR, base, RID_SP, ra_spill(as, IR(REF_BASE)));
  emit_setgl(as, base, jit_base);
  emit_addptr(as, base, -8*delta);
  asm_guardcc(as, CC_NE);
  emit_nm(as, ARMI_CMP, RID_TMP,
	  ra_allock(as, i32ptr(pc), rset_exclude(RSET_GPR, base)));
  emit_lso(as, ARMI_LDR, RID_TMP, base, -4);
}

/* -- Type conversions ---------------------------------------------------- */

static void asm_conv(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  IRType st = (IRType)(ir->op2 & IRCONV_SRCMASK);
  /* FP conversions and 64 bit integer conversions are handled by SPLIT. */
  lua_assert(!irt_isfp(ir->t) && !(st == IRT_NUM || st == IRT_FLOAT));
  lua_assert(!irt_isint64(ir->t) && !(st == IRT_I64 || st == IRT_U64));
  if (st >= IRT_I8 && st <= IRT_U16) {  /* Extend to 32 bit integer. */
    Reg left = ra_alloc1(as, ir->op1, RSET_GPR);
    lua_assert(irt_isint(ir->t) || irt_isu32(ir->t));
    if ((as->flags & JIT_F_ARMV6)) {
      ARMIns ai = st == IRT_I8 ? ARMI_SXTB :
		  st == IRT_U8 ? ARMI_UXTB :
		  st == IRT_I16 ? ARMI_SXTH : ARMI_UXTH;
      emit_dm(as, ai, dest, left);
    } else if (st == IRT_U8) {
      emit_dn(as, ARMI_AND|ARMI_K12|255, dest, left);
    } else {
      uint32_t shift = st == IRT_I8 ? 24 : 16;
      ARMShift sh = st == IRT_U16 ? ARMSH_LSR : ARMSH_ASR;
      emit_dm(as, ARMI_MOV|ARMF_SH(sh, shift), dest, RID_TMP);
      emit_dm(as, ARMI_MOV|ARMF_SH(ARMSH_LSL, shift), RID_TMP, left);
    }
  } else {  /* Handle 32/32 bit no-op (cast). */
    ra_leftov(as, dest, ir->op1);  /* Do nothing, but may need to move regs. */
  }
}

static void asm_strto(ASMState *as, IRIns *ir)
{
  const CCallInfo *ci = &lj_ir_callinfo[IRCALL_lj_str_tonum];
  IRRef args[2];
  Reg rlo = 0, rhi = 0, tmp;
  int destused = ra_used(ir);
  int32_t ofs = 0;
  ra_evictset(as, RSET_SCRATCH);
  if (destused) {
    if (ra_hasspill(ir->s) && ra_hasspill((ir+1)->s) &&
	(ir->s & 1) == 0 && ir->s + 1 == (ir+1)->s) {
      int i;
      for (i = 0; i < 2; i++) {
	Reg r = (ir+i)->r;
	if (ra_hasreg(r)) {
	  ra_free(as, r);
	  ra_modified(as, r);
	  emit_spload(as, ir+i, r, sps_scale((ir+i)->s));
	}
      }
      ofs = sps_scale(ir->s);
      destused = 0;
    } else {
      rhi = ra_dest(as, ir+1, RSET_GPR);
      rlo = ra_dest(as, ir, rset_exclude(RSET_GPR, rhi));
    }
  }
  asm_guardcc(as, CC_EQ);
  if (destused) {
    emit_lso(as, ARMI_LDR, rhi, RID_SP, 4);
    emit_lso(as, ARMI_LDR, rlo, RID_SP, 0);
  }
  emit_n(as, ARMI_CMP|ARMI_K12|0, RID_RET);  /* Test return status. */
  args[0] = ir->op1;      /* GCstr *str */
  args[1] = ASMREF_TMP1;  /* TValue *n  */
  asm_gencall(as, ci, args);
  tmp = ra_releasetmp(as, ASMREF_TMP1);
  if (ofs == 0)
    emit_dm(as, ARMI_MOV, tmp, RID_SP);
  else
    emit_opk(as, ARMI_ADD, tmp, RID_SP, ofs, RSET_GPR);
}

/* Get pointer to TValue. */
static void asm_tvptr(ASMState *as, Reg dest, IRRef ref)
{
  IRIns *ir = IR(ref);
  if (irt_isnum(ir->t)) {  /* Use the number constant itself as a TValue. */
    lua_assert(irref_isk(ref));
    ra_allockreg(as, i32ptr(ir_knum(ir)), dest);
  } else {
    /* Otherwise use [sp] and [sp+4] to hold the TValue. */
    RegSet allow = rset_exclude(RSET_GPR, dest);
    Reg type;
    emit_dm(as, ARMI_MOV, dest, RID_SP);
    if (!irt_ispri(ir->t)) {
      Reg src = ra_alloc1(as, ref, allow);
      emit_lso(as, ARMI_STR, src, RID_SP, 0);
    }
    if ((ir+1)->o == IR_HIOP)
      type = ra_alloc1(as, ref+1, allow);
    else
      type = ra_allock(as, irt_toitype(ir->t), allow);
    emit_lso(as, ARMI_STR, type, RID_SP, 4);
  }
}

static void asm_tostr(ASMState *as, IRIns *ir)
{
  IRRef args[2];
  args[0] = ASMREF_L;
  as->gcsteps++;
  if (irt_isnum(IR(ir->op1)->t) || (ir+1)->o == IR_HIOP) {
    const CCallInfo *ci = &lj_ir_callinfo[IRCALL_lj_str_fromnum];
    args[1] = ASMREF_TMP1;  /* const lua_Number * */
    asm_setupresult(as, ir, ci);  /* GCstr * */
    asm_gencall(as, ci, args);
    asm_tvptr(as, ra_releasetmp(as, ASMREF_TMP1), ir->op1);
  } else {
    const CCallInfo *ci = &lj_ir_callinfo[IRCALL_lj_str_fromint];
    args[1] = ir->op1;  /* int32_t k */
    asm_setupresult(as, ir, ci);  /* GCstr * */
    asm_gencall(as, ci, args);
  }
}

/* -- Memory references --------------------------------------------------- */

static void asm_aref(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg idx, base;
  if (irref_isk(ir->op2)) {
    IRRef tab = IR(ir->op1)->op1;
    int32_t ofs = asm_fuseabase(as, tab);
    IRRef refa = ofs ? tab : ir->op1;
    uint32_t k = emit_isk12(ARMI_ADD, ofs + 8*IR(ir->op2)->i);
    if (k) {
      base = ra_alloc1(as, refa, RSET_GPR);
      emit_dn(as, ARMI_ADD^k, dest, base);
      return;
    }
  }
  base = ra_alloc1(as, ir->op1, RSET_GPR);
  idx = ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, base));
  emit_dnm(as, ARMI_ADD|ARMF_SH(ARMSH_LSL, 3), dest, base, idx);
}

/* Inlined hash lookup. Specialized for key type and for const keys.
** The equivalent C code is:
**   Node *n = hashkey(t, key);
**   do {
**     if (lj_obj_equal(&n->key, key)) return &n->val;
**   } while ((n = nextnode(n)));
**   return niltv(L);
*/
static void asm_href(ASMState *as, IRIns *ir, IROp merge)
{
  RegSet allow = RSET_GPR;
  int destused = ra_used(ir);
  Reg dest = ra_dest(as, ir, allow);
  Reg tab = ra_alloc1(as, ir->op1, rset_clear(allow, dest));
  Reg key = 0, keyhi = 0, keynumhi = RID_NONE, tmp = RID_LR;
  IRRef refkey = ir->op2;
  IRIns *irkey = IR(refkey);
  IRType1 kt = irkey->t;
  int32_t k = 0, khi = emit_isk12(ARMI_CMP, irt_toitype(kt));
  uint32_t khash;
  MCLabel l_end, l_loop;
  rset_clear(allow, tab);
  if (!irref_isk(refkey) || irt_isstr(kt)) {
    key = ra_alloc1(as, refkey, allow);
    rset_clear(allow, key);
    if (irkey[1].o == IR_HIOP) {
      if (ra_hasreg((irkey+1)->r)) {
	keynumhi = (irkey+1)->r;
	keyhi = RID_TMP;
	ra_noweak(as, keynumhi);
      } else {
	keyhi = keynumhi = ra_allocref(as, refkey+1, allow);
      }
      rset_clear(allow, keynumhi);
      khi = 0;
    }
  } else if (irt_isnum(kt)) {
    int32_t val = (int32_t)ir_knum(irkey)->u32.lo;
    k = emit_isk12(ARMI_CMP, val);
    if (!k) {
      key = ra_allock(as, val, allow);
      rset_clear(allow, key);
    }
    val = (int32_t)ir_knum(irkey)->u32.hi;
    khi = emit_isk12(ARMI_CMP, val);
    if (!khi) {
      keyhi = ra_allock(as, val, allow);
      rset_clear(allow, keyhi);
    }
  } else if (!irt_ispri(kt)) {
    k = emit_isk12(ARMI_CMP, irkey->i);
    if (!k) {
      key = ra_alloc1(as, refkey, allow);
      rset_clear(allow, key);
    }
  }
  if (!irt_ispri(kt))
    tmp = ra_scratchpair(as, allow);

  /* Key not found in chain: jump to exit (if merged) or load niltv. */
  l_end = emit_label(as);
  as->invmcp = NULL;
  if (merge == IR_NE)
    asm_guardcc(as, CC_AL);
  else if (destused)
    emit_loada(as, dest, niltvg(J2G(as->J)));

  /* Follow hash chain until the end. */
  l_loop = --as->mcp;
  emit_n(as, ARMI_CMP|ARMI_K12|0, dest);
  emit_lso(as, ARMI_LDR, dest, dest, (int32_t)offsetof(Node, next));

  /* Type and value comparison. */
  if (merge == IR_EQ)
    asm_guardcc(as, CC_EQ);
  else
    emit_branch(as, ARMF_CC(ARMI_B, CC_EQ), l_end);
  if (!irt_ispri(kt)) {
    emit_nm(as, ARMF_CC(ARMI_CMP, CC_EQ)^khi, tmp+1, keyhi);
    emit_nm(as, ARMI_CMP^k, tmp, key);
    emit_lsox(as, ARMI_LDRD, tmp, dest, (int32_t)offsetof(Node, key));
  } else {
    emit_n(as, ARMI_CMP^khi, tmp);
    emit_lso(as, ARMI_LDR, tmp, dest, (int32_t)offsetof(Node, key.it));
  }
  *l_loop = ARMF_CC(ARMI_B, CC_NE) | ((as->mcp-l_loop-2) & 0x00ffffffu);

  /* Load main position relative to tab->node into dest. */
  khash = irref_isk(refkey) ? ir_khash(irkey) : 1;
  if (khash == 0) {
    emit_lso(as, ARMI_LDR, dest, tab, (int32_t)offsetof(GCtab, node));
  } else {
    emit_dnm(as, ARMI_ADD|ARMF_SH(ARMSH_LSL, 3), dest, dest, tmp);
    emit_dnm(as, ARMI_ADD|ARMF_SH(ARMSH_LSL, 1), tmp, tmp, tmp);
    if (irt_isstr(kt)) {  /* Fetch of str->hash is cheaper than ra_allock. */
      emit_dnm(as, ARMI_AND, tmp, tmp+1, RID_TMP);
      emit_lso(as, ARMI_LDR, dest, tab, (int32_t)offsetof(GCtab, node));
      emit_lso(as, ARMI_LDR, tmp+1, key, (int32_t)offsetof(GCstr, hash));
      emit_lso(as, ARMI_LDR, RID_TMP, tab, (int32_t)offsetof(GCtab, hmask));
    } else if (irref_isk(refkey)) {
      emit_opk(as, ARMI_AND, tmp, RID_TMP, (int32_t)khash,
	       rset_exclude(rset_exclude(RSET_GPR, tab), dest));
      emit_lso(as, ARMI_LDR, dest, tab, (int32_t)offsetof(GCtab, node));
      emit_lso(as, ARMI_LDR, RID_TMP, tab, (int32_t)offsetof(GCtab, hmask));
    } else {  /* Must match with hash*() in lj_tab.c. */
      if (ra_hasreg(keynumhi)) {  /* Canonicalize +-0.0 to 0.0. */
	if (keyhi == RID_TMP)
	  emit_dm(as, ARMF_CC(ARMI_MOV, CC_NE), keyhi, keynumhi);
	emit_d(as, ARMF_CC(ARMI_MOV, CC_EQ)|ARMI_K12|0, keyhi);
      }
      emit_dnm(as, ARMI_AND, tmp, tmp, RID_TMP);
      emit_dnm(as, ARMI_SUB|ARMF_SH(ARMSH_ROR, 32-HASH_ROT3), tmp, tmp, tmp+1);
      emit_lso(as, ARMI_LDR, dest, tab, (int32_t)offsetof(GCtab, node));
      emit_dnm(as, ARMI_EOR|ARMF_SH(ARMSH_ROR, 32-((HASH_ROT2+HASH_ROT1)&31)),
	       tmp, tmp+1, tmp);
      emit_lso(as, ARMI_LDR, RID_TMP, tab, (int32_t)offsetof(GCtab, hmask));
      emit_dnm(as, ARMI_SUB|ARMF_SH(ARMSH_ROR, 32-HASH_ROT1), tmp+1, tmp+1, tmp);
      if (ra_hasreg(keynumhi)) {
	emit_dnm(as, ARMI_EOR, tmp+1, tmp, key);
	emit_dnm(as, ARMI_ORR|ARMI_S, RID_TMP, tmp, key);  /* Test for +-0.0. */
	emit_dnm(as, ARMI_ADD, tmp, keynumhi, keynumhi);
      } else {
	emit_dnm(as, ARMI_EOR, tmp+1, tmp, key);
	emit_opk(as, ARMI_ADD, tmp, key, (int32_t)HASH_BIAS,
		 rset_exclude(rset_exclude(RSET_GPR, tab), key));
      }
    }
  }
}

static void asm_hrefk(ASMState *as, IRIns *ir)
{
  IRIns *kslot = IR(ir->op2);
  IRIns *irkey = IR(kslot->op1);
  int32_t ofs = (int32_t)(kslot->op2 * sizeof(Node));
  int32_t kofs = ofs + (int32_t)offsetof(Node, key);
  Reg dest = (ra_used(ir) || ofs > 4095) ? ra_dest(as, ir, RSET_GPR) : RID_NONE;
  Reg node = ra_alloc1(as, ir->op1, RSET_GPR);
  Reg key = RID_NONE, type = RID_TMP, idx = node;
  RegSet allow = rset_exclude(RSET_GPR, node);
  lua_assert(ofs % sizeof(Node) == 0);
  if (ofs > 4095) {
    idx = dest;
    rset_clear(allow, dest);
    kofs = (int32_t)offsetof(Node, key);
  } else if (ra_hasreg(dest)) {
    emit_opk(as, ARMI_ADD, dest, node, ofs, allow);
  }
  asm_guardcc(as, CC_NE);
  if (!irt_ispri(irkey->t)) {
    RegSet even = (as->freeset & (as->freeset >> 1) & allow & RSET_GPREVEN);
    if (even) {
      key = ra_scratch(as, even);
      if (rset_test(as->freeset, key+1)) {
	type = key+1;
	ra_modified(as, type);
      }
    } else {
      key = ra_scratch(as, allow);
    }
    rset_clear(allow, key);
  }
  rset_clear(allow, type);
  if (irt_isnum(irkey->t)) {
    emit_opk(as, ARMF_CC(ARMI_CMP, CC_EQ), 0, type,
	     (int32_t)ir_knum(irkey)->u32.hi, allow);
    emit_opk(as, ARMI_CMP, 0, key,
	     (int32_t)ir_knum(irkey)->u32.lo, allow);
  } if (ra_hasreg(key)) {
    emit_n(as, ARMF_CC(ARMI_CMN, CC_EQ)|ARMI_K12|-irt_toitype(irkey->t), type);
    emit_opk(as, ARMI_CMP, 0, key, irkey->i, allow);
  } else {
    emit_n(as, ARMI_CMN|ARMI_K12|-irt_toitype(irkey->t), type);
  }
  emit_lso(as, ARMI_LDR, type, idx, kofs+4);
  if (ra_hasreg(key)) emit_lso(as, ARMI_LDR, key, idx, kofs);
  if (ofs > 4095)
    emit_opk(as, ARMI_ADD, dest, node, ofs, RSET_GPR);
}

static void asm_newref(ASMState *as, IRIns *ir)
{
  const CCallInfo *ci = &lj_ir_callinfo[IRCALL_lj_tab_newkey];
  IRRef args[3];
  args[0] = ASMREF_L;     /* lua_State *L */
  args[1] = ir->op1;      /* GCtab *t     */
  args[2] = ASMREF_TMP1;  /* cTValue *key */
  asm_setupresult(as, ir, ci);  /* TValue * */
  asm_gencall(as, ci, args);
  asm_tvptr(as, ra_releasetmp(as, ASMREF_TMP1), ir->op2);
}

static void asm_uref(ASMState *as, IRIns *ir)
{
  /* NYI: Check that UREFO is still open and not aliasing a slot. */
  Reg dest = ra_dest(as, ir, RSET_GPR);
  if (irref_isk(ir->op1)) {
    GCfunc *fn = ir_kfunc(IR(ir->op1));
    MRef *v = &gcref(fn->l.uvptr[(ir->op2 >> 8)])->uv.v;
    emit_lsptr(as, ARMI_LDR, dest, v);
  } else {
    Reg uv = ra_scratch(as, RSET_GPR);
    Reg func = ra_alloc1(as, ir->op1, RSET_GPR);
    if (ir->o == IR_UREFC) {
      asm_guardcc(as, CC_NE);
      emit_n(as, ARMI_CMP|ARMI_K12|1, RID_TMP);
      emit_opk(as, ARMI_ADD, dest, uv,
	       (int32_t)offsetof(GCupval, tv), RSET_GPR);
      emit_lso(as, ARMI_LDRB, RID_TMP, uv, (int32_t)offsetof(GCupval, closed));
    } else {
      emit_lso(as, ARMI_LDR, dest, uv, (int32_t)offsetof(GCupval, v));
    }
    emit_lso(as, ARMI_LDR, uv, func,
	     (int32_t)offsetof(GCfuncL, uvptr) + 4*(int32_t)(ir->op2 >> 8));
  }
}

static void asm_fref(ASMState *as, IRIns *ir)
{
  UNUSED(as); UNUSED(ir);
  lua_assert(!ra_used(ir));
}

static void asm_strref(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  IRRef ref = ir->op2, refk = ir->op1;
  Reg r;
  if (irref_isk(ref)) {
    IRRef tmp = refk; refk = ref; ref = tmp;
  } else if (!irref_isk(refk)) {
    uint32_t k, m = ARMI_K12|sizeof(GCstr);
    Reg right, left = ra_alloc1(as, ir->op1, RSET_GPR);
    IRIns *irr = IR(ir->op2);
    if (ra_hasreg(irr->r)) {
      ra_noweak(as, irr->r);
      right = irr->r;
    } else if (mayfuse(as, irr->op2) &&
	       irr->o == IR_ADD && irref_isk(irr->op2) &&
	       (k = emit_isk12(ARMI_ADD,
			       (int32_t)sizeof(GCstr) + IR(irr->op2)->i))) {
      m = k;
      right = ra_alloc1(as, irr->op1, rset_exclude(RSET_GPR, left));
    } else {
      right = ra_allocref(as, ir->op2, rset_exclude(RSET_GPR, left));
    }
    emit_dn(as, ARMI_ADD^m, dest, dest);
    emit_dnm(as, ARMI_ADD, dest, left, right);
    return;
  }
  r = ra_alloc1(as, ref, RSET_GPR);
  emit_opk(as, ARMI_ADD, dest, r,
	   sizeof(GCstr) + IR(refk)->i, rset_exclude(RSET_GPR, r));
}

/* -- Loads and stores ---------------------------------------------------- */

static ARMIns asm_fxloadins(IRIns *ir)
{
  switch (irt_type(ir->t)) {
  case IRT_I8: return ARMI_LDRSB;
  case IRT_U8: return ARMI_LDRB;
  case IRT_I16: return ARMI_LDRSH;
  case IRT_U16: return ARMI_LDRH;
  case IRT_NUM: lua_assert(0);
  case IRT_FLOAT:
  default: return ARMI_LDR;
  }
}

static ARMIns asm_fxstoreins(IRIns *ir)
{
  switch (irt_type(ir->t)) {
  case IRT_I8: case IRT_U8: return ARMI_STRB;
  case IRT_I16: case IRT_U16: return ARMI_STRH;
  case IRT_NUM: lua_assert(0);
  case IRT_FLOAT:
  default: return ARMI_STR;
  }
}

static void asm_fload(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg idx = ra_alloc1(as, ir->op1, RSET_GPR);
  ARMIns ai = asm_fxloadins(ir);
  int32_t ofs;
  if (ir->op2 == IRFL_TAB_ARRAY) {
    ofs = asm_fuseabase(as, ir->op1);
    if (ofs) {  /* Turn the t->array load into an add for colocated arrays. */
      emit_dn(as, ARMI_ADD|ARMI_K12|ofs, dest, idx);
      return;
    }
  }
  ofs = field_ofs[ir->op2];
  if ((ai & 0x04000000))
    emit_lso(as, ai, dest, idx, ofs);
  else
    emit_lsox(as, ai, dest, idx, ofs);
}

static void asm_fstore(ASMState *as, IRIns *ir)
{
  Reg src = ra_alloc1(as, ir->op2, RSET_GPR);
  IRIns *irf = IR(ir->op1);
  Reg idx = ra_alloc1(as, irf->op1, rset_exclude(RSET_GPR, src));
  int32_t ofs = field_ofs[irf->op2];
  ARMIns ai = asm_fxstoreins(ir);
  if ((ai & 0x04000000))
    emit_lso(as, ai, src, idx, ofs);
  else
    emit_lsox(as, ai, src, idx, ofs);
}

static void asm_xload(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  lua_assert(!(ir->op2 & IRXLOAD_UNALIGNED));
  asm_fusexref(as, asm_fxloadins(ir), dest, ir->op1, RSET_GPR);
}

static void asm_xstore(ASMState *as, IRIns *ir)
{
  Reg src = ra_alloc1(as, ir->op2, RSET_GPR);
  asm_fusexref(as, asm_fxstoreins(ir), src, ir->op1,
	       rset_exclude(RSET_GPR, src));
}

static void asm_ahuvload(ASMState *as, IRIns *ir)
{
  int hiop = ((ir+1)->o == IR_HIOP);
  IRType t = hiop ? IRT_NUM : irt_type(ir->t);
  Reg dest = RID_NONE, type = RID_NONE, idx;
  RegSet allow = RSET_GPR;
  int32_t ofs = 0;
  if (hiop && ra_used(ir+1)) {
    type = ra_dest(as, ir+1, allow);
    rset_clear(allow, type);
  }
  if (ra_used(ir)) {
    lua_assert(irt_isint(ir->t) || irt_isaddr(ir->t));
    dest = ra_dest(as, ir, allow);
    rset_clear(allow, dest);
  }
  idx = asm_fuseahuref(as, ir->op1, &ofs, allow);
  if (!hiop) {
    rset_clear(allow, idx);
    if (ofs < 256 && ra_hasreg(dest) && (dest & 1) == 0 &&
	rset_test((as->freeset & allow), dest+1)) {
      type = dest+1;
      ra_modified(as, type);
    } else {
      type = RID_TMP;
    }
  }
  asm_guardcc(as, t == IRT_NUM ? CC_HS : CC_NE);
  emit_n(as, ARMI_CMN|ARMI_K12|-irt_toitype_(t), type);
  if (ra_hasreg(dest)) emit_lso(as, ARMI_LDR, dest, idx, ofs);
  emit_lso(as, ARMI_LDR, type, idx, ofs+4);
}

static void asm_ahustore(ASMState *as, IRIns *ir)
{
  RegSet allow = RSET_GPR;
  Reg idx, src = RID_NONE, type = RID_NONE;
  int32_t ofs = 0;
  int hiop = ((ir+1)->o == IR_HIOP);
  if (!irt_ispri(ir->t)) {
    src = ra_alloc1(as, ir->op2, allow);
    rset_clear(allow, src);
  }
  if (hiop)
    type = ra_alloc1(as, (ir+1)->op2, allow);
  else
    type = ra_allock(as, (int32_t)irt_toitype(ir->t), allow);
  idx = asm_fuseahuref(as, ir->op1, &ofs, rset_exclude(allow, type));
  if (ra_hasreg(src)) emit_lso(as, ARMI_STR, src, idx, ofs);
  emit_lso(as, ARMI_STR, type, idx, ofs+4);
}

static void asm_sload(ASMState *as, IRIns *ir)
{
  int32_t ofs = 8*((int32_t)ir->op1-1) + ((ir->op2 & IRSLOAD_FRAME) ? 4 : 0);
  int hiop = ((ir+1)->o == IR_HIOP);
  IRType t = hiop ? IRT_NUM : irt_type(ir->t);
  Reg dest = RID_NONE, type = RID_NONE, base;
  RegSet allow = RSET_GPR;
  lua_assert(!(ir->op2 & IRSLOAD_PARENT));  /* Handled by asm_head_side(). */
  lua_assert(irt_isguard(ir->t) || !(ir->op2 & IRSLOAD_TYPECHECK));
  lua_assert(!(ir->op2 & IRSLOAD_CONVERT));  /* Handled by LJ_SOFTFP SPLIT. */
  if (hiop && ra_used(ir+1)) {
    type = ra_dest(as, ir+1, allow);
    rset_clear(allow, type);
  }
  if (ra_used(ir)) {
    lua_assert(irt_isint(ir->t) || irt_isaddr(ir->t));
    dest = ra_dest(as, ir, allow);
    rset_clear(allow, dest);
  }
  base = ra_alloc1(as, REF_BASE, allow);
  if ((ir->op2 & IRSLOAD_TYPECHECK)) {
    if (ra_noreg(type)) {
      rset_clear(allow, base);
      if (ofs < 256 && ra_hasreg(dest) && (dest & 1) == 0 &&
	  rset_test((as->freeset & allow), dest+1)) {
	type = dest+1;
	ra_modified(as, type);
      } else {
	type = RID_TMP;
      }
    }
    asm_guardcc(as, t == IRT_NUM ? CC_HS : CC_NE);
    emit_n(as, ARMI_CMN|ARMI_K12|-irt_toitype_(t), type);
  }
  if (ra_hasreg(dest)) emit_lso(as, ARMI_LDR, dest, base, ofs);
  if (ra_hasreg(type)) emit_lso(as, ARMI_LDR, type, base, ofs+4);
}

/* -- Allocations --------------------------------------------------------- */

#if LJ_HASFFI
static void asm_cnew(ASMState *as, IRIns *ir)
{
  CTState *cts = ctype_ctsG(J2G(as->J));
  CTypeID typeid = (CTypeID)IR(ir->op1)->i;
  CTSize sz = (ir->o == IR_CNEWI || ir->op2 == REF_NIL) ?
	      lj_ctype_size(cts, typeid) : (CTSize)IR(ir->op2)->i;
  const CCallInfo *ci = &lj_ir_callinfo[IRCALL_lj_mem_newgco];
  IRRef args[2];
  RegSet allow = (RSET_GPR & ~RSET_SCRATCH);
  lua_assert(sz != CTSIZE_INVALID);

  args[0] = ASMREF_L;     /* lua_State *L */
  args[1] = ASMREF_TMP1;  /* MSize size   */
  as->gcsteps++;
  asm_setupresult(as, ir, ci);  /* GCcdata * */

  /* Initialize immutable cdata object. */
  if (ir->o == IR_CNEWI) {
    int32_t ofs = sizeof(GCcdata);
    lua_assert(sz == 4 || sz == 8);
    if (sz == 8) {
      ofs += 4; ir++;
      lua_assert(ir->o == IR_HIOP);
    }
    for (;;) {
      Reg r = ra_alloc1(as, ir->op2, allow);
      emit_lso(as, ARMI_STR, r, RID_RET, ofs);
      rset_clear(allow, r);
      if (ofs == sizeof(GCcdata)) break;
      ofs -= 4; ir--;
    }
  }
  /* Initialize gct and typeid. lj_mem_newgco() already sets marked. */
  {
    uint32_t k = emit_isk12(ARMI_MOV, typeid);
    Reg r = k ? RID_R1 : ra_allock(as, typeid, allow);
    emit_lso(as, ARMI_STRB, RID_TMP, RID_RET, offsetof(GCcdata, gct));
    emit_lsox(as, ARMI_STRH, r, RID_RET, offsetof(GCcdata, typeid));
    emit_d(as, ARMI_MOV|ARMI_K12|~LJ_TCDATA, RID_TMP);
    if (k) emit_d(as, ARMI_MOV^k, RID_R1);
  }
  asm_gencall(as, ci, args);
  ra_allockreg(as, (int32_t)(sz+sizeof(GCcdata)),
	       ra_releasetmp(as, ASMREF_TMP1));
}
#else
#define asm_cnew(as, ir)	((void)0)
#endif

/* -- Write barriers ------------------------------------------------------ */

static void asm_tbar(ASMState *as, IRIns *ir)
{
  Reg tab = ra_alloc1(as, ir->op1, RSET_GPR);
  Reg link = ra_scratch(as, rset_exclude(RSET_GPR, tab));
  Reg gr = ra_allock(as, i32ptr(J2G(as->J)),
		     rset_exclude(rset_exclude(RSET_GPR, tab), link));
  Reg mark = RID_TMP;
  MCLabel l_end = emit_label(as);
  emit_lso(as, ARMI_STR, link, tab, (int32_t)offsetof(GCtab, gclist));
  emit_lso(as, ARMI_STRB, mark, tab, (int32_t)offsetof(GCtab, marked));
  emit_lso(as, ARMI_STR, tab, gr,
	   (int32_t)offsetof(global_State, gc.grayagain));
  emit_dn(as, ARMI_BIC|ARMI_K12|LJ_GC_BLACK, mark, mark);
  emit_lso(as, ARMI_LDR, link, gr,
	   (int32_t)offsetof(global_State, gc.grayagain));
  emit_branch(as, ARMF_CC(ARMI_B, CC_EQ), l_end);
  emit_n(as, ARMI_TST|ARMI_K12|LJ_GC_BLACK, mark);
  emit_lso(as, ARMI_LDRB, mark, tab, (int32_t)offsetof(GCtab, marked));
}

static void asm_obar(ASMState *as, IRIns *ir)
{
  const CCallInfo *ci = &lj_ir_callinfo[IRCALL_lj_gc_barrieruv];
  IRRef args[2];
  MCLabel l_end;
  Reg obj, val, tmp;
  /* No need for other object barriers (yet). */
  lua_assert(IR(ir->op1)->o == IR_UREFC);
  ra_evictset(as, RSET_SCRATCH);
  l_end = emit_label(as);
  args[0] = ASMREF_TMP1;  /* global_State *g */
  args[1] = ir->op1;      /* TValue *tv      */
  asm_gencall(as, ci, args);
  if ((*as->mcp >> 28) == CC_AL)
    *as->mcp = ARMF_CC(*as->mcp, CC_NE);
  else
    emit_branch(as, ARMF_CC(ARMI_B, CC_EQ), l_end);
  ra_allockreg(as, i32ptr(J2G(as->J)), ra_releasetmp(as, ASMREF_TMP1));
  obj = IR(ir->op1)->r;
  tmp = ra_scratch(as, rset_exclude(RSET_GPR, obj));
  emit_n(as, ARMF_CC(ARMI_TST, CC_NE)|ARMI_K12|LJ_GC_BLACK, tmp);
  emit_n(as, ARMI_TST|ARMI_K12|LJ_GC_WHITES, RID_TMP);
  val = ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, obj));
  emit_lso(as, ARMI_LDRB, tmp, obj,
	   (int32_t)offsetof(GCupval, marked)-(int32_t)offsetof(GCupval, tv));
  emit_lso(as, ARMI_LDRB, RID_TMP, val, (int32_t)offsetof(GChead, marked));
}

/* -- Arithmetic and logic operations ------------------------------------- */

static int asm_swapops(ASMState *as, IRRef lref, IRRef rref)
{
  IRIns *ir;
  if (irref_isk(rref))
    return 0;  /* Don't swap constants to the left. */
  if (irref_isk(lref))
    return 1;  /* But swap constants to the right. */
  ir = IR(rref);
  if ((ir->o >= IR_BSHL && ir->o <= IR_BROR) ||
      (ir->o == IR_ADD && ir->op1 == ir->op2))
    return 0;  /* Don't swap fusable operands to the left. */
  ir = IR(lref);
  if ((ir->o >= IR_BSHL && ir->o <= IR_BROR) ||
      (ir->o == IR_ADD && ir->op1 == ir->op2))
    return 1;  /* But swap fusable operands to the right. */
  return 0;  /* Otherwise don't swap. */
}

static void asm_intop(ASMState *as, IRIns *ir, ARMIns ai)
{
  IRRef lref = ir->op1, rref = ir->op2;
  Reg left, dest = ra_dest(as, ir, RSET_GPR);
  uint32_t m;
  if (asm_swapops(as, lref, rref)) {
    IRRef tmp = lref; lref = rref; rref = tmp;
    if ((ai & ~ARMI_S) == ARMI_SUB || (ai & ~ARMI_S) == ARMI_SBC)
      ai ^= (ARMI_SUB^ARMI_RSB);
  }
  left = ra_hintalloc(as, lref, dest, RSET_GPR);
  m = asm_fuseopm(as, ai, rref, rset_exclude(RSET_GPR, left));
  if (irt_isguard(ir->t)) {  /* For IR_ADDOV etc. */
    asm_guardcc(as, CC_VS);
    ai |= ARMI_S;
  }
  emit_dn(as, ai^m, dest, left);
}

static void asm_bitop(ASMState *as, IRIns *ir, ARMIns ai)
{
  if (as->flagmcp == as->mcp) {  /* Try to drop cmp r, #0. */
    uint32_t cc = (as->mcp[1] >> 28);
    as->flagmcp = NULL;
    if (cc <= CC_NE) {
      as->mcp++;
      ai |= ARMI_S;
    } else if (cc == CC_GE) {
      *++as->mcp ^= ((CC_GE^CC_PL) << 28);
      ai |= ARMI_S;
    } else if (cc == CC_LT) {
      *++as->mcp ^= ((CC_LT^CC_MI) << 28);
      ai |= ARMI_S;
    }  /* else: other conds don't work with bit ops. */
  }
  if (ir->op2 == 0) {
    Reg dest = ra_dest(as, ir, RSET_GPR);
    uint32_t m = asm_fuseopm(as, ai, ir->op1, RSET_GPR);
    emit_d(as, ai^m, dest);
  } else {
    /* NYI: Turn BAND !k12 into uxtb, uxth or bfc or shl+shr. */
    asm_intop(as, ir, ai);
  }
}

static void asm_arithop(ASMState *as, IRIns *ir, ARMIns ai)
{
  if (as->flagmcp == as->mcp) {  /* Drop cmp r, #0. */
    as->flagmcp = NULL;
    as->mcp++;
    ai |= ARMI_S;
  }
  asm_intop(as, ir, ai);
}

static void asm_intneg(ASMState *as, IRIns *ir, ARMIns ai)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg left = ra_hintalloc(as, ir->op1, dest, RSET_GPR);
  emit_dn(as, ai|ARMI_K12|0, dest, left);
}

/* NYI: use add/shift for MUL(OV) with constants. FOLD only does 2^k. */
static void asm_intmul(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg left = ra_alloc1(as, ir->op1, rset_exclude(RSET_GPR, dest));
  Reg right = ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, left));
  Reg tmp = RID_NONE;
  /* ARMv5 restriction: dest != left and dest_hi != left. */
  if (dest == left && left != right) { left = right; right = dest; }
  if (irt_isguard(ir->t)) {  /* IR_MULOV */
    if (!(as->flags & JIT_F_ARMV6) && dest == left)
      tmp = left = ra_scratch(as, rset_exclude(RSET_FPR, left));
    asm_guardcc(as, CC_NE);
    emit_nm(as, ARMI_TEQ|ARMF_SH(ARMSH_ASR, 31), RID_TMP, dest);
    emit_dnm(as, ARMI_SMULL|ARMF_S(right), dest, RID_TMP, left);
  } else {
    if (!(as->flags & JIT_F_ARMV6) && dest == left) tmp = left = RID_TMP;
    emit_nm(as, ARMI_MUL|ARMF_S(right), dest, left);
  }
  /* Only need this for the dest == left == right case. */
  if (ra_hasreg(tmp)) emit_dm(as, ARMI_MOV, tmp, right);
}

static void asm_intmod(ASMState *as, IRIns *ir)
{
  const CCallInfo *ci = &lj_ir_callinfo[IRCALL_lj_vm_modi];
  IRRef args[2];
  args[0] = ir->op1;
  args[1] = ir->op2;
  asm_setupresult(as, ir, ci);
  asm_gencall(as, ci, args);
}

static void asm_bitswap(ASMState *as, IRIns *ir)
{
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg left = ra_alloc1(as, ir->op1, RSET_GPR);
  if ((as->flags & JIT_F_ARMV6)) {
    emit_dm(as, ARMI_REV, dest, left);
  } else {
    Reg tmp2 = dest;
    if (tmp2 == left)
      tmp2 = ra_scratch(as, rset_exclude(rset_exclude(RSET_GPR, dest), left));
    emit_dnm(as, ARMI_EOR|ARMF_SH(ARMSH_LSR, 8), dest, tmp2, RID_TMP);
    emit_dm(as, ARMI_MOV|ARMF_SH(ARMSH_ROR, 8), tmp2, left);
    emit_dn(as, ARMI_BIC|ARMI_K12|256*8|255, RID_TMP, RID_TMP);
    emit_dnm(as, ARMI_EOR|ARMF_SH(ARMSH_ROR, 16), RID_TMP, left, left);
  }
}

static void asm_bitshift(ASMState *as, IRIns *ir, ARMShift sh)
{
  if (irref_isk(ir->op2)) {  /* Constant shifts. */
    /* NYI: Turn SHL+SHR or BAND+SHR into uxtb, uxth or ubfx. */
    /* NYI: Turn SHL+ASR into sxtb, sxth or sbfx. */
    Reg dest = ra_dest(as, ir, RSET_GPR);
    Reg left = ra_alloc1(as, ir->op1, RSET_GPR);
    int32_t shift = (IR(ir->op2)->i & 31);
    emit_dm(as, ARMI_MOV|ARMF_SH(sh, shift), dest, left);
  } else {
    Reg dest = ra_dest(as, ir, RSET_GPR);
    Reg left = ra_alloc1(as, ir->op1, RSET_GPR);
    Reg right = ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, left));
    emit_dm(as, ARMI_MOV|ARMF_RSH(sh, right), dest, left);
  }
}

static void asm_intmin_max(ASMState *as, IRIns *ir, int cc)
{
  uint32_t kcmp = 0, kmov = 0;
  Reg dest = ra_dest(as, ir, RSET_GPR);
  Reg left = ra_hintalloc(as, ir->op1, dest, RSET_GPR);
  Reg right = 0;
  if (irref_isk(ir->op2)) {
    kcmp = emit_isk12(ARMI_CMP, IR(ir->op2)->i);
    if (kcmp) kmov = emit_isk12(ARMI_MOV, IR(ir->op2)->i);
  }
  if (!kmov) {
    kcmp = 0;
    right = ra_alloc1(as, ir->op2, rset_exclude(RSET_GPR, left));
  }
  if (dest != right) {
    emit_dm(as, ARMF_CC(ARMI_MOV, cc)^kmov, dest, right);
    cc ^= 1;  /* Must use opposite conditions for paired moves. */
  } else {
    cc ^= (CC_LT^CC_GT);  /* Otherwise may swap CC_LT <-> CC_GT. */
  }
  if (dest != left) emit_dm(as, ARMF_CC(ARMI_MOV, cc)^kmov, dest, left);
  emit_nm(as, ARMI_CMP^kcmp, left, right);
}

static void asm_fpmin_max(ASMState *as, IRIns *ir, int cc)
{
  const CCallInfo *ci = &lj_ir_callinfo[IRCALL_softfp_cmp];
  RegSet drop = RSET_SCRATCH;
  Reg r;
  IRRef args[4];
  args[0] = ir->op1; args[1] = (ir+1)->op1;
  args[2] = ir->op2; args[3] = (ir+1)->op2;
  /* __aeabi_cdcmple preserves r0-r3. */
  if (ra_hasreg(ir->r)) rset_clear(drop, ir->r);
  if (ra_hasreg((ir+1)->r)) rset_clear(drop, (ir+1)->r);
  if (!rset_test(as->freeset, RID_R2) &&
      regcost_ref(as->cost[RID_R2]) == args[2]) rset_clear(drop, RID_R2);
  if (!rset_test(as->freeset, RID_R3) &&
      regcost_ref(as->cost[RID_R3]) == args[3]) rset_clear(drop, RID_R3);
  ra_evictset(as, drop);
  ra_destpair(as, ir);
  emit_dm(as, ARMF_CC(ARMI_MOV, cc), RID_RETHI, RID_R3);
  emit_dm(as, ARMF_CC(ARMI_MOV, cc), RID_RET, RID_R2);
  emit_call(as, (void *)ci->func);
  for (r = RID_R0; r <= RID_R3; r++)
    ra_leftov(as, r, args[r-RID_R0]);
}

/* -- Comparisons --------------------------------------------------------- */

/* Map of comparisons to flags. ORDER IR. */
static const uint8_t asm_compmap[IR_ABC+1] = {
  /* op  FP swp  int cc   FP cc */
  /* LT       */ CC_GE + (CC_HS << 4),
  /* GE    x  */ CC_LT + (CC_HI << 4),
  /* LE       */ CC_GT + (CC_HI << 4),
  /* GT    x  */ CC_LE + (CC_HS << 4),
  /* ULT   x  */ CC_HS + (CC_LS << 4),
  /* UGE      */ CC_LO + (CC_LO << 4),
  /* ULE   x  */ CC_HI + (CC_LO << 4),
  /* UGT      */ CC_LS + (CC_LS << 4),
  /* EQ       */ CC_NE + (CC_NE << 4),
  /* NE       */ CC_EQ + (CC_EQ << 4),
  /* ABC      */ CC_LS + (CC_LS << 4)  /* Same as UGT. */
};

/* FP comparisons. */
static void asm_fpcomp(ASMState *as, IRIns *ir)
{
  const CCallInfo *ci = &lj_ir_callinfo[IRCALL_softfp_cmp];
  RegSet drop = RSET_SCRATCH;
  Reg r;
  IRRef args[4];
  int swp = (((ir->o ^ (ir->o >> 2)) & ~(ir->o >> 3) & 1) << 1);
  args[swp^0] = ir->op1; args[swp^1] = (ir+1)->op1;
  args[swp^2] = ir->op2; args[swp^3] = (ir+1)->op2;
  /* __aeabi_cdcmple preserves r0-r3. This helps to reduce spills. */
  for (r = RID_R0; r <= RID_R3; r++)
    if (!rset_test(as->freeset, r) &&
	regcost_ref(as->cost[r]) == args[r-RID_R0]) rset_clear(drop, r);
  ra_evictset(as, drop);
  asm_guardcc(as, (asm_compmap[ir->o] >> 4));
  emit_call(as, (void *)ci->func);
  for (r = RID_R0; r <= RID_R3; r++)
    ra_leftov(as, r, args[r-RID_R0]);
}

/* Integer comparisons. */
static void asm_intcomp(ASMState *as, IRIns *ir)
{
  ARMCC cc = (asm_compmap[ir->o] & 15);
  IRRef lref = ir->op1, rref = ir->op2;
  Reg left;
  uint32_t m;
  int cmpprev0 = 0;
  lua_assert(irt_isint(ir->t) || irt_isaddr(ir->t));
  if (asm_swapops(as, lref, rref)) {
    Reg tmp = lref; lref = rref; rref = tmp;
    if (cc >= CC_GE) cc ^= 7;  /* LT <-> GT, LE <-> GE */
    else if (cc > CC_NE) cc ^= 11;  /* LO <-> HI, LS <-> HS */
  }
  if (irref_isk(rref) && IR(rref)->i == 0) {
    IRIns *irl = IR(lref);
    cmpprev0 = (irl+1 == ir);
    /* Combine comp(BAND(left, right), 0) into tst left, right. */
    if (cmpprev0 && irl->o == IR_BAND && !ra_used(irl)) {
      IRRef blref = irl->op1, brref = irl->op2;
      uint32_t m2 = 0;
      Reg bleft;
      if (asm_swapops(as, blref, brref)) {
	Reg tmp = blref; blref = brref; brref = tmp;
      }
      if (irref_isk(brref)) {
	m2 = emit_isk12(ARMI_AND, IR(brref)->i);
	if ((m2 & (ARMI_AND^ARMI_BIC)))
	  goto notst;  /* Not beneficial if we miss a constant operand. */
      }
      if (cc == CC_GE) cc = CC_PL;
      else if (cc == CC_LT) cc = CC_MI;
      else if (cc > CC_NE) goto notst;  /* Other conds don't work with tst. */
      bleft = ra_alloc1(as, blref, RSET_GPR);
      if (!m2) m2 = asm_fuseopm(as, 0, brref, rset_exclude(RSET_GPR, bleft));
      asm_guardcc(as, cc);
      emit_n(as, ARMI_TST^m2, bleft);
      return;
    }
  }
notst:
  left = ra_alloc1(as, lref, RSET_GPR);
  m = asm_fuseopm(as, ARMI_CMP, rref, rset_exclude(RSET_GPR, left));
  asm_guardcc(as, cc);
  emit_n(as, ARMI_CMP^m, left);
  /* Signed comparison with zero and referencing previous ins? */
  if (cmpprev0 && (cc <= CC_NE || cc >= CC_GE))
    as->flagmcp = as->mcp;  /* Allow elimination of the compare. */
}

/* 64 bit integer comparisons. */
static void asm_int64comp(ASMState *as, IRIns *ir)
{
  int signedcomp = (ir->o <= IR_GT);
  ARMCC cclo, cchi;
  Reg leftlo, lefthi;
  uint32_t mlo, mhi;
  RegSet allow = RSET_GPR, oldfree;

  /* Always use unsigned comparison for loword. */
  cclo = asm_compmap[ir->o + (signedcomp ? 4 : 0)] & 15;
  leftlo = ra_alloc1(as, ir->op1, allow);
  oldfree = as->freeset;
  mlo = asm_fuseopm(as, ARMI_CMP, ir->op2, rset_clear(allow, leftlo));
  allow &= ~(oldfree & ~as->freeset);  /* Update for allocs of asm_fuseopm. */

  /* Use signed or unsigned comparison for hiword. */
  cchi = asm_compmap[ir->o] & 15;
  lefthi = ra_alloc1(as, (ir+1)->op1, allow);
  mhi = asm_fuseopm(as, ARMI_CMP, (ir+1)->op2, rset_clear(allow, lefthi));

  /* All register allocations must be performed _before_ this point. */
  if (signedcomp) {
    MCLabel l_around = emit_label(as);
    asm_guardcc(as, cclo);
    emit_n(as, ARMI_CMP^mlo, leftlo);
    emit_branch(as, ARMF_CC(ARMI_B, CC_NE), l_around);
    if (cchi == CC_GE || cchi == CC_LE) cchi ^= 6;  /* GE -> GT, LE -> LT */
    asm_guardcc(as, cchi);
  } else {
    asm_guardcc(as, cclo);
    emit_n(as, ARMF_CC(ARMI_CMP, CC_EQ)^mlo, leftlo);
  }
  emit_n(as, ARMI_CMP^mhi, lefthi);
}

/* -- Support for 64 bit ops in 32 bit mode ------------------------------- */

/* Hiword op of a split 64 bit op. Previous op must be the loword op. */
static void asm_hiop(ASMState *as, IRIns *ir)
{
  /* HIOP is marked as a store because it needs its own DCE logic. */
  int uselo = ra_used(ir-1), usehi = ra_used(ir);  /* Loword/hiword used? */
  if (LJ_UNLIKELY(!(as->flags & JIT_F_OPT_DCE))) uselo = usehi = 1;
  if ((ir-1)->o <= IR_NE) {  /* 64 bit integer or FP comparisons. ORDER IR. */
    as->curins--;  /* Always skip the loword comparison. */
    if (irt_isint(ir->t))
      asm_int64comp(as, ir-1);
    else
      asm_fpcomp(as, ir-1);
    return;
  } else if ((ir-1)->o == IR_MIN || (ir-1)->o == IR_MAX) {
    if (uselo || usehi || !(as->flags & JIT_F_OPT_DCE)) {
      as->curins--;  /* Always skip the loword min/max. */
      asm_fpmin_max(as, ir-1, (ir-1)->o == IR_MIN ? CC_HI : CC_LO);
    }
    return;
  }
  if (!usehi && (as->flags & JIT_F_OPT_DCE))
    return;  /* Skip unused hiword op for all remaining ops. */
  switch ((ir-1)->o) {
#if LJ_HASFFI
  case IR_ADD:
    if (uselo) {
      as->curins--;
      asm_intop(as, ir, ARMI_ADC);
      asm_intop(as, ir-1, ARMI_ADD|ARMI_S);
    } else {
      asm_intop(as, ir, ARMI_ADD);
    }
    break;
  case IR_SUB:
    if (uselo) {
      as->curins--;
      asm_intop(as, ir, ARMI_SBC);
      asm_intop(as, ir-1, ARMI_SUB|ARMI_S);
    } else {
      asm_intop(as, ir, ARMI_SUB);
    }
    break;
  case IR_NEG:
    if (uselo) {
      as->curins--;
      asm_intneg(as, ir, ARMI_RSC);
      asm_intneg(as, ir-1, ARMI_RSB|ARMI_S);
    } else {
      asm_intneg(as, ir, ARMI_RSB);
    }
    break;
#endif
  case IR_SLOAD: case IR_ALOAD: case IR_HLOAD: case IR_ULOAD: case IR_VLOAD:
  case IR_STRTO:
    if (!uselo)
      ra_allocref(as, ir->op1, RSET_GPR);  /* Mark lo op as used. */
    break;
  case IR_CALLN:
  case IR_CALLS:
  case IR_CALLXS:
    if (!uselo)
      ra_allocref(as, ir->op1, RID2RSET(RID_RET));  /* Mark lo op as used. */
    break;
  case IR_ASTORE: case IR_HSTORE: case IR_USTORE:
  case IR_TOSTR: case IR_CNEWI:
    /* Nothing to do here. Handled by lo op itself. */
    break;
  default: lua_assert(0); break;
  }
}

/* -- Stack handling ------------------------------------------------------ */

/* Check Lua stack size for overflow. Use exit handler as fallback. */
static void asm_stack_check(ASMState *as, BCReg topslot,
			    IRIns *irp, RegSet allow, ExitNo exitno)
{
  Reg pbase;
  uint32_t k;
  if (irp) {
    exitno = as->T->nsnap;  /* Highest exit + 1 indicates stack check. */
    if (ra_hasreg(irp->r)) {
      pbase = irp->r;
    } else if (allow) {
      pbase = rset_pickbot(allow);
    } else {
      pbase = RID_RET;
      emit_lso(as, ARMI_LDR, RID_RET, RID_SP, 0);  /* Restore temp. register. */
    }
  } else {
    pbase = RID_BASE;
  }
  emit_branch(as, ARMF_CC(ARMI_BL, CC_LS), exitstub_addr(as->J, exitno));
  k = emit_isk12(0, (int32_t)(8*topslot));
  lua_assert(k);
  emit_n(as, ARMI_CMP^k, RID_TMP);
  emit_dnm(as, ARMI_SUB, RID_TMP, RID_TMP, pbase);
  emit_lso(as, ARMI_LDR, RID_TMP, RID_TMP,
	   (int32_t)offsetof(lua_State, maxstack));
  if (irp) {  /* Must not spill arbitrary registers in head of side trace. */
    int32_t i = i32ptr(&J2G(as->J)->jit_L);
    if (ra_noreg(irp->r)) {
      lua_assert(ra_hasspill(irp->s));
      emit_lso(as, ARMI_LDR, RID_RET, RID_SP, sps_scale(irp->s));
    }
    emit_lso(as, ARMI_LDR, RID_TMP, RID_TMP, (i & 4095));
    if (ra_noreg(irp->r)) {
      emit_lso(as, ARMI_STR, RID_RET, RID_SP, 0);  /* Save temp. register. */
    }
    emit_loadi(as, RID_TMP, (i & ~4095));
  } else {
    emit_getgl(as, RID_TMP, jit_L);
  }
}

/* Restore Lua stack from on-trace state. */
static void asm_stack_restore(ASMState *as, SnapShot *snap)
{
  SnapEntry *map = &as->T->snapmap[snap->mapofs];
  MSize n, nent = snap->nent;
  SnapEntry *flinks = map + nent + snap->depth;
  /* Store the value of all modified slots to the Lua stack. */
  for (n = 0; n < nent; n++) {
    SnapEntry sn = map[n];
    BCReg s = snap_slot(sn);
    int32_t ofs = 8*((int32_t)s-1);
    IRRef ref = snap_ref(sn);
    IRIns *ir = IR(ref);
    if ((sn & SNAP_NORESTORE))
      continue;
    if (irt_isnum(ir->t)) {
      RegSet odd = rset_exclude(RSET_GPRODD, RID_BASE);
      Reg tmp;
      lua_assert(irref_isk(ref));  /* LJ_SOFTFP: must be a number constant. */
      tmp = ra_allock(as, (int32_t)ir_knum(ir)->u32.lo,
		      rset_exclude(RSET_GPREVEN, RID_BASE));
      emit_lso(as, ARMI_STR, tmp, RID_BASE, ofs);
      if (rset_test(as->freeset, tmp+1)) odd = RID2RSET(tmp+1);
      tmp = ra_allock(as, (int32_t)ir_knum(ir)->u32.hi, odd);
      emit_lso(as, ARMI_STR, tmp, RID_BASE, ofs+4);
    } else {
      RegSet odd = rset_exclude(RSET_GPRODD, RID_BASE);
      Reg type;
      lua_assert(irt_ispri(ir->t) || irt_isaddr(ir->t) || irt_isinteger(ir->t));
      if (!irt_ispri(ir->t)) {
	Reg src = ra_alloc1(as, ref, rset_exclude(RSET_GPREVEN, RID_BASE));
	emit_lso(as, ARMI_STR, src, RID_BASE, ofs);
	if (rset_test(as->freeset, src+1)) odd = RID2RSET(src+1);
      }
      if ((sn & (SNAP_CONT|SNAP_FRAME))) {
	if (s == 0) continue;  /* Do not overwrite link to previous frame. */
	type = ra_allock(as, (int32_t)(*flinks--), odd);
      } else if ((sn & SNAP_SOFTFPNUM)) {
	type = ra_alloc1(as, ref+1, rset_exclude(RSET_GPRODD, RID_BASE));
      } else {
	type = ra_allock(as, (int32_t)irt_toitype(ir->t), odd);
      }
      emit_lso(as, ARMI_STR, type, RID_BASE, ofs+4);
    }
    checkmclim(as);
  }
  lua_assert(map + nent == flinks);
}

/* -- GC handling --------------------------------------------------------- */

/* Check GC threshold and do one or more GC steps. */
static void asm_gc_check(ASMState *as)
{
  const CCallInfo *ci = &lj_ir_callinfo[IRCALL_lj_gc_step_jit];
  IRRef args[2];
  MCLabel l_end;
  Reg tmp1, tmp2;
  ra_evictset(as, RSET_SCRATCH);
  l_end = emit_label(as);
  /* Exit trace if in GCSatomic or GCSfinalize. Avoids syncing GC objects. */
  asm_guardcc(as, CC_NE);  /* Assumes asm_snap_prep() already done. */
  emit_n(as, ARMI_CMP|ARMI_K12|0, RID_RET);
  args[0] = ASMREF_TMP1;  /* global_State *g */
  args[1] = ASMREF_TMP2;  /* MSize steps     */
  asm_gencall(as, ci, args);
  tmp1 = ra_releasetmp(as, ASMREF_TMP1);
  tmp2 = ra_releasetmp(as, ASMREF_TMP2);
  emit_loadi(as, tmp2, (int32_t)as->gcsteps);
  /* Jump around GC step if GC total < GC threshold. */
  emit_branch(as, ARMF_CC(ARMI_B, CC_LS), l_end);
  emit_nm(as, ARMI_CMP, RID_TMP, tmp2);
  emit_lso(as, ARMI_LDR, tmp2, tmp1,
	   (int32_t)offsetof(global_State, gc.threshold));
  emit_lso(as, ARMI_LDR, RID_TMP, tmp1,
	   (int32_t)offsetof(global_State, gc.total));
  ra_allockreg(as, i32ptr(J2G(as->J)), tmp1);
  as->gcsteps = 0;
  checkmclim(as);
}

/* -- Loop handling ------------------------------------------------------- */

/* Fixup the loop branch. */
static void asm_loop_fixup(ASMState *as)
{
  MCode *p = as->mctop;
  MCode *target = as->mcp;
  if (as->loopinv) {  /* Inverted loop branch? */
    /* asm_guardcc already inverted the bcc and patched the final bl. */
    p[-2] |= ((uint32_t)(target-p) & 0x00ffffffu);
  } else {
    p[-1] = ARMI_B | ((uint32_t)((target-p)-1) & 0x00ffffffu);
  }
}

/* -- Head of trace ------------------------------------------------------- */

/* Reload L register from g->jit_L. */
static void asm_head_lreg(ASMState *as)
{
  IRIns *ir = IR(ASMREF_L);
  if (ra_used(ir)) {
    Reg r = ra_dest(as, ir, RSET_GPR);
    emit_getgl(as, r, jit_L);
    ra_evictk(as);
  }
}

/* Coalesce BASE register for a root trace. */
static void asm_head_root_base(ASMState *as)
{
  IRIns *ir;
  asm_head_lreg(as);
  ir = IR(REF_BASE);
  if (ra_hasreg(ir->r) && rset_test(as->modset, ir->r)) ra_spill(as, ir);
  ra_destreg(as, ir, RID_BASE);
}

/* Coalesce BASE register for a side trace. */
static RegSet asm_head_side_base(ASMState *as, IRIns *irp, RegSet allow)
{
  IRIns *ir;
  asm_head_lreg(as);
  ir = IR(REF_BASE);
  if (ra_hasreg(ir->r) && rset_test(as->modset, ir->r)) ra_spill(as, ir);
  if (ra_hasspill(irp->s)) {
    rset_clear(allow, ra_dest(as, ir, allow));
  } else {
    lua_assert(ra_hasreg(irp->r));
    rset_clear(allow, irp->r);
    ra_destreg(as, ir, irp->r);
  }
  return allow;
}

/* -- Tail of trace ------------------------------------------------------- */

/* Fixup the tail code. */
static void asm_tail_fixup(ASMState *as, TraceNo lnk)
{
  MCode *p = as->mctop;
  MCode *target;
  int32_t spadj = as->T->spadjust;
  if (spadj == 0) {
    as->mctop = --p;
  } else {
    /* Patch stack adjustment. */
    uint32_t k = emit_isk12(ARMI_ADD, spadj);
    lua_assert(k);
    p[-2] = (ARMI_ADD^k) | ARMF_D(RID_SP) | ARMF_N(RID_SP);
  }
  /* Patch exit branch. */
  target = lnk == TRACE_INTERP ? (MCode *)lj_vm_exit_interp :
				 traceref(as->J, lnk)->mcode;
  p[-1] = ARMI_B|(((target-p)-1)&0x00ffffffu);
}

/* Prepare tail of code. */
static void asm_tail_prep(ASMState *as)
{
  MCode *p = as->mctop - 1;  /* Leave room for exit branch. */
  if (as->loopref) {
    as->invmcp = as->mcp = p;
  } else {
    as->mcp = p-1;  /* Leave room for stack pointer adjustment. */
    as->invmcp = NULL;
  }
  *p = 0;  /* Prevent load/store merging. */
}

/* -- Instruction dispatch ------------------------------------------------ */

/* Assemble a single instruction. */
static void asm_ir(ASMState *as, IRIns *ir)
{
  switch ((IROp)ir->o) {
  /* Miscellaneous ops. */
  case IR_LOOP: asm_loop(as); break;
  case IR_NOP: case IR_XBAR: lua_assert(!ra_used(ir)); break;
  case IR_USE: ra_alloc1(as, ir->op1, RSET_GPR); break;
  case IR_PHI: asm_phi(as, ir); break;
  case IR_HIOP: asm_hiop(as, ir); break;

  /* Guarded assertions. */
  case IR_EQ: case IR_NE:
    if ((ir-1)->o == IR_HREF && ir->op1 == as->curins-1) {
      as->curins--;
      asm_href(as, ir-1, (IROp)ir->o);
      break;
    }
    /* fallthrough */
  case IR_LT: case IR_GE: case IR_LE: case IR_GT:
  case IR_ULT: case IR_UGE: case IR_ULE: case IR_UGT:
  case IR_ABC:
    asm_intcomp(as, ir);
    break;

  case IR_RETF: asm_retf(as, ir); break;

  /* Bit ops. */
  case IR_BNOT: asm_bitop(as, ir, ARMI_MVN); break;
  case IR_BSWAP: asm_bitswap(as, ir); break;

  case IR_BAND: asm_bitop(as, ir, ARMI_AND); break;
  case IR_BOR:  asm_bitop(as, ir, ARMI_ORR); break;
  case IR_BXOR: asm_bitop(as, ir, ARMI_EOR); break;

  case IR_BSHL: asm_bitshift(as, ir, ARMSH_LSL); break;
  case IR_BSHR: asm_bitshift(as, ir, ARMSH_LSR); break;
  case IR_BSAR: asm_bitshift(as, ir, ARMSH_ASR); break;
  case IR_BROR: asm_bitshift(as, ir, ARMSH_ROR); break;
  case IR_BROL: lua_assert(0); break;

  /* Arithmetic ops. */
  case IR_ADD: case IR_ADDOV: asm_arithop(as, ir, ARMI_ADD); break;
  case IR_SUB: case IR_SUBOV: asm_arithop(as, ir, ARMI_SUB); break;
  case IR_MUL: case IR_MULOV: asm_intmul(as, ir); break;
  case IR_MOD: asm_intmod(as, ir); break;

  case IR_NEG: asm_intneg(as, ir, ARMI_RSB); break;

  case IR_MIN: asm_intmin_max(as, ir, CC_GT); break;
  case IR_MAX: asm_intmin_max(as, ir, CC_LT); break;

  case IR_FPMATH: case IR_ATAN2: case IR_LDEXP:
  case IR_DIV: case IR_POW: case IR_ABS: case IR_TOBIT:
    lua_assert(0);  /* Unused for LJ_SOFTFP. */
    break;

  /* Memory references. */
  case IR_AREF: asm_aref(as, ir); break;
  case IR_HREF: asm_href(as, ir, 0); break;
  case IR_HREFK: asm_hrefk(as, ir); break;
  case IR_NEWREF: asm_newref(as, ir); break;
  case IR_UREFO: case IR_UREFC: asm_uref(as, ir); break;
  case IR_FREF: asm_fref(as, ir); break;
  case IR_STRREF: asm_strref(as, ir); break;

  /* Loads and stores. */
  case IR_ALOAD: case IR_HLOAD: case IR_ULOAD: case IR_VLOAD:
    asm_ahuvload(as, ir);
    break;
  case IR_FLOAD: asm_fload(as, ir); break;
  case IR_XLOAD: asm_xload(as, ir); break;
  case IR_SLOAD: asm_sload(as, ir); break;

  case IR_ASTORE: case IR_HSTORE: case IR_USTORE: asm_ahustore(as, ir); break;
  case IR_FSTORE: asm_fstore(as, ir); break;
  case IR_XSTORE: asm_xstore(as, ir); break;

  /* Allocations. */
  case IR_SNEW: case IR_XSNEW: asm_snew(as, ir); break;
  case IR_TNEW: asm_tnew(as, ir); break;
  case IR_TDUP: asm_tdup(as, ir); break;
  case IR_CNEW: case IR_CNEWI: asm_cnew(as, ir); break;

  /* Write barriers. */
  case IR_TBAR: asm_tbar(as, ir); break;
  case IR_OBAR: asm_obar(as, ir); break;

  /* Type conversions. */
  case IR_CONV: asm_conv(as, ir); break;
  case IR_TOSTR: asm_tostr(as, ir); break;
  case IR_STRTO: asm_strto(as, ir); break;

  /* Calls. */
  case IR_CALLN: case IR_CALLL: case IR_CALLS: asm_call(as, ir); break;
  case IR_CALLXS: asm_callx(as, ir); break;
  case IR_CARG: break;

  default:
    setintV(&as->J->errinfo, ir->o);
    lj_trace_err_info(as->J, LJ_TRERR_NYIIR);
    break;
  }
}

/* -- Trace setup --------------------------------------------------------- */

/* Ensure there are enough stack slots for call arguments. */
static Reg asm_setup_call_slots(ASMState *as, IRIns *ir, const CCallInfo *ci)
{
  IRRef args[CCI_NARGS_MAX];
  uint32_t i, nargs = (int)CCI_NARGS(ci);
  int nslots = 0, ngpr = REGARG_NUMGPR;
  asm_collectargs(as, ir, ci, args);
  for (i = 0; i < nargs; i++)
    if (!LJ_SOFTFP && irt_isfp(IR(args[i])->t)) {
      ngpr &= ~1;
      if (ngpr > 0) ngpr -= 2; else nslots += 2;
    } else {
      if (ngpr > 0) ngpr--; else nslots++;
    }
  if (nslots > as->evenspill)  /* Leave room for args in stack slots. */
    as->evenspill = nslots;
  return REGSP_HINT(RID_RET);
}

static void asm_setup_target(ASMState *as)
{
  /* May need extra exit for asm_stack_check on side traces. */
  asm_exitstub_setup(as, as->T->nsnap + (as->parent ? 1 : 0));
}

/* -- Trace patching ------------------------------------------------------ */

/* Patch exit jumps of existing machine code to a new target. */
void lj_asm_patchexit(jit_State *J, GCtrace *T, ExitNo exitno, MCode *target)
{
  MCode *p = T->mcode;
  MCode *pe = (MCode *)((char *)p + T->szmcode);
  MCode *cstart = NULL, *cend = p;
  MCode *mcarea = lj_mcode_patch(J, p, 0);
  MCode *px = exitstub_addr(J, exitno) - 2;
  for (; p < pe; p++) {
    /* Look for bl_cc exitstub, replace with b_cc target. */
    uint32_t ins = *p;
    if ((ins & 0x0f000000u) == 0x0b000000u && ins < 0xf0000000u &&
	((ins ^ (px-p)) & 0x00ffffffu) == 0) {
      *p = (ins & 0xfe000000u) | (((target-p)-2) & 0x00ffffffu);
      cend = p+1;
      if (!cstart) cstart = p;
    }
  }
  lua_assert(cstart != NULL);
  asm_cache_flush(cstart, cend);
  lj_mcode_patch(J, mcarea, 1);
}

