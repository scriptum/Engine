/*
** $Id: Keyboard.c 27 2008-08-29 13:07:40Z basique $
** Library to use the keyboard with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"
#include "Keyboard.h"

/*
TODO
Проверить спецкнопки!!!
*/
static int Lua_Keyboard_isDown(lua_State *L)
{
    Uint8 *keystate = SDL_GetKeyState(NULL);
    Uint16 key = 0;
    const char * str = luaL_checkstring(L, 1);
    while (strcmp(lua_keys[key], str)) key++;

//	if (key>1000) {
//		key = key - 1000;
//		lua_pushboolean(L, keystate[key] + keystate[key+1]);
//	} else
    lua_pushboolean(L, keystate[key]);
    return 1;
}

static const struct luaL_Reg keyboardlib [] =
{
    {"keyIsDown", Lua_Keyboard_isDown},
    {NULL, NULL}
};

int luaopen_keyboard(lua_State *L, const char *parent)
{
    luaL_register(L, parent, keyboardlib);	/* leaves table on top of stack */
    lua_keys[0] = "unknown";
    lua_keys[8] = "backspace";
    lua_keys[9] = "tab";
    lua_keys[12] = "clear";
    lua_keys[13] = "return";
    lua_keys[19] = "pause";
    lua_keys[27] = "escape";
    lua_keys[32] = "space";
    lua_keys[33] = "!";
    lua_keys[34] = "\"";
    lua_keys[35] = "#";
    lua_keys[36] = "$";
    lua_keys[38] = "&";
    lua_keys[39] = "'";
    lua_keys[40] = "(";
    lua_keys[41] = ")";
    lua_keys[42] = "*";
    lua_keys[43] = "+";
    lua_keys[44] = ",";
    lua_keys[45] = "-";
    lua_keys[46] = ".";
    lua_keys[47] = "/";
    lua_keys[48] = "0";
    lua_keys[49] = "1";
    lua_keys[50] = "2";
    lua_keys[51] = "3";
    lua_keys[52] = "4";
    lua_keys[53] = "5";
    lua_keys[54] = "6";
    lua_keys[55] = "7";
    lua_keys[56] = "8";
    lua_keys[57] = "9";
    lua_keys[58] = ":";
    lua_keys[59] = ";";
    lua_keys[60] = "<";
    lua_keys[61] = "=";
    lua_keys[62] = ">";
    lua_keys[63] = "?";
    lua_keys[64] = "@";

    lua_keys[91] = "[";
    lua_keys[92] = "\\";
    lua_keys[93] = "]";
    lua_keys[94] = "^";
    lua_keys[95] = "_";
    lua_keys[96] = "`";
    lua_keys[97] = "a";
    lua_keys[98] = "b";
    lua_keys[99] = "c";
    lua_keys[100] = "d";
    lua_keys[101] = "e";
    lua_keys[102] = "f";
    lua_keys[103] = "g";
    lua_keys[104] = "h";
    lua_keys[105] = "i";
    lua_keys[106] = "j";
    lua_keys[107] = "k";
    lua_keys[108] = "l";
    lua_keys[109] = "m";
    lua_keys[110] = "n";
    lua_keys[111] = "o";
    lua_keys[112] = "p";
    lua_keys[113] = "q";
    lua_keys[114] = "r";
    lua_keys[115] = "s";
    lua_keys[116] = "t";
    lua_keys[117] = "u";
    lua_keys[118] = "v";
    lua_keys[119] = "w";
    lua_keys[120] = "x";
    lua_keys[121] = "y";
    lua_keys[122] = "z";
    lua_keys[127] = "delete";

    lua_keys[160] = "world_0";
    lua_keys[161] = "world_1";
    lua_keys[162] = "world_2";
    lua_keys[163] = "world_3";
    lua_keys[164] = "world_4";
    lua_keys[165] = "world_5";
    lua_keys[166] = "world_6";
    lua_keys[167] = "world_7";
    lua_keys[168] = "world_8";
    lua_keys[169] = "world_9";
    lua_keys[170] = "world_10";
    lua_keys[171] = "world_11";
    lua_keys[172] = "world_12";
    lua_keys[173] = "world_13";
    lua_keys[174] = "world_14";
    lua_keys[175] = "world_15";
    lua_keys[176] = "world_16";
    lua_keys[177] = "world_17";
    lua_keys[178] = "world_18";
    lua_keys[179] = "world_19";
    lua_keys[180] = "world_20";
    lua_keys[181] = "world_21";
    lua_keys[182] = "world_22";
    lua_keys[183] = "world_23";
    lua_keys[184] = "world_24";
    lua_keys[185] = "world_25";
    lua_keys[186] = "world_26";
    lua_keys[187] = "world_27";
    lua_keys[188] = "world_28";
    lua_keys[189] = "world_29";
    lua_keys[190] = "world_30";
    lua_keys[191] = "world_31";
    lua_keys[192] = "world_32";
    lua_keys[193] = "world_33";
    lua_keys[194] = "world_34";
    lua_keys[195] = "world_35";
    lua_keys[196] = "world_36";
    lua_keys[197] = "world_37";
    lua_keys[198] = "world_38";
    lua_keys[199] = "world_39";
    lua_keys[200] = "world_40";
    lua_keys[201] = "world_41";
    lua_keys[202] = "world_42";
    lua_keys[203] = "world_43";
    lua_keys[204] = "world_44";
    lua_keys[205] = "world_45";
    lua_keys[206] = "world_46";
    lua_keys[207] = "world_47";
    lua_keys[208] = "world_48";
    lua_keys[209] = "world_49";
    lua_keys[210] = "world_50";
    lua_keys[211] = "world_51";
    lua_keys[212] = "world_52";
    lua_keys[213] = "world_53";
    lua_keys[214] = "world_54";
    lua_keys[215] = "world_55";
    lua_keys[216] = "world_56";
    lua_keys[217] = "world_57";
    lua_keys[218] = "world_58";
    lua_keys[219] = "world_59";
    lua_keys[220] = "world_60";
    lua_keys[221] = "world_61";
    lua_keys[222] = "world_62";
    lua_keys[223] = "world_63";
    lua_keys[224] = "world_64";
    lua_keys[225] = "world_65";
    lua_keys[226] = "world_66";
    lua_keys[227] = "world_67";
    lua_keys[228] = "world_68";
    lua_keys[229] = "world_69";
    lua_keys[230] = "world_70";
    lua_keys[231] = "world_71";
    lua_keys[232] = "world_72";
    lua_keys[233] = "world_73";
    lua_keys[234] = "world_74";
    lua_keys[235] = "world_75";
    lua_keys[236] = "world_76";
    lua_keys[237] = "world_77";
    lua_keys[238] = "world_78";
    lua_keys[239] = "world_79";
    lua_keys[240] = "world_80";
    lua_keys[241] = "world_81";
    lua_keys[242] = "world_82";
    lua_keys[243] = "world_83";
    lua_keys[244] = "world_84";
    lua_keys[245] = "world_85";
    lua_keys[246] = "world_86";
    lua_keys[247] = "world_87";
    lua_keys[248] = "world_88";
    lua_keys[249] = "world_89";
    lua_keys[250] = "world_90";
    lua_keys[251] = "world_91";
    lua_keys[252] = "world_92";
    lua_keys[253] = "world_93";
    lua_keys[254] = "world_94";
    lua_keys[255] = "world_95";

    lua_keys[256] = "kp0";
    lua_keys[257] = "kp1";
    lua_keys[258] = "kp2";
    lua_keys[259] = "kp3";
    lua_keys[260] = "kp4";
    lua_keys[261] = "kp5";
    lua_keys[262] = "kp6";
    lua_keys[263] = "kp7";
    lua_keys[264] = "kp8";
    lua_keys[265] = "kp9";
    lua_keys[266] = "kp_period";
    lua_keys[267] = "kp_divide";
    lua_keys[268] = "kp_multiply";
    lua_keys[269] = "kp_minus";
    lua_keys[270] = "kp_plus";
    lua_keys[271] = "kp_enter";
    lua_keys[272] = "kp_equals";

    lua_keys[273] = "up";
    lua_keys[274] = "down";
    lua_keys[275] = "right";
    lua_keys[276] = "left";
    lua_keys[277] = "insert";
    lua_keys[278] = "home";
    lua_keys[279] = "end";
    lua_keys[280] = "pageup";
    lua_keys[281] = "pagedown";

    lua_keys[282] = "f1";
    lua_keys[283] = "f2";
    lua_keys[284] = "f3";
    lua_keys[285] = "f4";
    lua_keys[286] = "f5";
    lua_keys[287] = "f6";
    lua_keys[288] = "f7";
    lua_keys[289] = "f8";
    lua_keys[290] = "f9";
    lua_keys[291] = "f10";
    lua_keys[292] = "f11";
    lua_keys[293] = "f12";
    lua_keys[294] = "f13";
    lua_keys[295] = "f14";
    lua_keys[296] = "f15";

    lua_keys[300] = "numlock";
    lua_keys[301] = "capslock";
    lua_keys[302] = "scrollock";
    lua_keys[303] = "rshift";
    lua_keys[304] = "lshift";
    lua_keys[305] = "rctrl";
    lua_keys[306] = "lctrl";
    lua_keys[307] = "ralt";
    lua_keys[308] = "lalt";
    lua_keys[309] = "rmeta";
    lua_keys[310] = "lmeta";
    lua_keys[311] = "lsuper";
    lua_keys[312] = "rsuper";
    lua_keys[313] = "mode";
    lua_keys[314] = "compose";

    lua_keys[315] = "help";
    lua_keys[316] = "print";
    lua_keys[317] = "sysreq";
    lua_keys[318] = "break";
    lua_keys[319] = "menu";
    lua_keys[320] = "power";
    lua_keys[321] = "euro";
    lua_keys[322] = "undo";

    return 1;
}
