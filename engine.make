# C Console Executable Makefile autogenerated by premake
# Don't edit this file! Instead edit `premake.lua` then rerun `make`

ifndef CONFIG
  CONFIG=Release
endif

# if multiple archs are defined turn off automated dependency generation
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifeq ($(CONFIG),Release)
  BINDIR := bin/Release
  LIBDIR := .
  OBJDIR := obj/Release
  OUTDIR := bin/Release
  CPPFLAGS := $(DEPFLAGS)
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -Os -fomit-frame-pointer `sdl-config --cflags` -s -march=prescott
  CXXFLAGS += $(CFLAGS)
  LDFLAGS += -L$(BINDIR) -L$(LIBDIR) `sdl-config --libs` -s -lGL -llua -lSDL_mixer -lphysfs
  LDDEPS :=
  RESFLAGS :=
  TARGET := engine
 BLDCMD = $(CC) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)

  define postbuildcmd
	@echo Running post-build commands...
	cp /home/rpg/engine/engine/bin/Release/engine /home/rpg/Игры/Monopoly/bin/linux
	cd /home/rpg/Игры/Monopoly; ./run_russian.sh
  endef
endif

OBJECTS := \
	$(OBJDIR)/Font.o \
	$(OBJDIR)/Mouse.o \
	$(OBJDIR)/FileIO.o \
	$(OBJDIR)/Main.o \
	$(OBJDIR)/Keyboard.o \
	$(OBJDIR)/physfsrwops.o \
	$(OBJDIR)/Sound.o \
	$(OBJDIR)/Thread.o \
	$(OBJDIR)/Macros.o \
	$(OBJDIR)/image_helper.o \
	$(OBJDIR)/image_DXT.o \
	$(OBJDIR)/stb_image_aug.o \
	$(OBJDIR)/SOIL.o \
	$(OBJDIR)/primitives.o \
	$(OBJDIR)/Graphics.o \
	$(OBJDIR)/init.o \
	$(OBJDIR)/image.o \

MKDIR_TYPE := msdos
CMD := $(subst \,\\,$(ComSpec)$(COMSPEC))
ifeq (,$(CMD))
  MKDIR_TYPE := posix
endif
ifeq (/bin,$(findstring /bin,$(SHELL)))
  MKDIR_TYPE := posix
endif
ifeq ($(MKDIR_TYPE),posix)
  CMD_MKBINDIR := mkdir -p $(BINDIR)
  CMD_MKLIBDIR := mkdir -p $(LIBDIR)
  CMD_MKOUTDIR := mkdir -p $(OUTDIR)
  CMD_MKOBJDIR := mkdir -p $(OBJDIR)
else
  CMD_MKBINDIR := $(CMD) /c if not exist $(subst /,\\,$(BINDIR)) mkdir $(subst /,\\,$(BINDIR))
  CMD_MKLIBDIR := $(CMD) /c if not exist $(subst /,\\,$(LIBDIR)) mkdir $(subst /,\\,$(LIBDIR))
  CMD_MKOUTDIR := $(CMD) /c if not exist $(subst /,\\,$(OUTDIR)) mkdir $(subst /,\\,$(OUTDIR))
  CMD_MKOBJDIR := $(CMD) /c if not exist $(subst /,\\,$(OBJDIR)) mkdir $(subst /,\\,$(OBJDIR))
endif

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(LDDEPS) $(RESOURCES)
	@echo Linking engine
	-@$(CMD_MKBINDIR)
	-@$(CMD_MKLIBDIR)
	-@$(CMD_MKOUTDIR)
	@$(BLDCMD)
	$(postbuildcmd)

clean:
	@echo Cleaning engine
ifeq ($(MKDIR_TYPE),posix)
	-@rm -f $(OUTDIR)/$(TARGET)
	-@rm -rf $(OBJDIR)
else
	-@if exist $(subst /,\,$(OUTDIR)/$(TARGET)) del /q $(subst /,\,$(OUTDIR)/$(TARGET))
	-@if exist $(subst /,\,$(OBJDIR)) del /q $(subst /,\,$(OBJDIR))
	-@if exist $(subst /,\,$(OBJDIR)) rmdir /s /q $(subst /,\,$(OBJDIR))
endif

$(OBJDIR)/Font.o: Font.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Mouse.o: Mouse.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FileIO.o: FileIO.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Main.o: Main.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Keyboard.o: Keyboard.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/physfsrwops.o: physfsrwops.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Sound.o: Sound.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Thread.o: Thread.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Macros.o: Macros.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/image_helper.o: SOIL/image_helper.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/image_DXT.o: SOIL/image_DXT.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/stb_image_aug.o: SOIL/stb_image_aug.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SOIL.o: SOIL/SOIL.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/primitives.o: Graphics/primitives.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Graphics.o: Graphics/Graphics.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/init.o: Graphics/init.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

$(OBJDIR)/image.o: Graphics/image.c
	-@$(CMD_MKOBJDIR)
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)

