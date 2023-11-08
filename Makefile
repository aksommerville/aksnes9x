all:
.SILENT:
PRECMD=echo "  $(@F)" ; mkdir -p $(@D) ;

MIDDIR:=mid
OUTDIR:=out

LIBROMASSIST:=../ra3/out/libemuhost.a
HDRROMASSIST:=../ra3/out/include

UNAMES:=$(shell uname -s)
ifeq ($(UNAMES),Darwin)

  CCWARN:=-Werror -Wno-deprecated-declarations -Wno-parentheses -Wno-constant-logical-operand
  CCOPT:=-c -MMD -O2 -Isrc -Isrc/lib -DUSE_macos=1 -DUSE_romassist=1 -I$(HDRROMASSIST)
  CC:=gcc $(CCOPT) $(CCWARN)
  CXX:=g++ $(CCOPT) $(CCWARN)
  OBJC:=gcc -xobjective-c $(CCOPT) $(CCWARN)
  LD:=g++
  LDPOST:=-framework Cocoa -framework OpenGL -framework IOKit -framework AudioUnit $(LIBROMASSIST)
  OPT:=macos romassist

  BUNDLE_MAIN:=$(OUTDIR)/aksnes9x.app
  EXE:=$(BUNDLE_MAIN)/Contents/MacOS/aksnes9x
  PLIST_MAIN:=$(BUNDLE_MAIN)/Contents/Info.plist
  NIB_MAIN:=$(BUNDLE_MAIN)/Contents/Resources/Main.nib
  ICON_MAIN:=$(BUNDLE_MAIN)/Contents/Resources/appicon.icns

  $(EXE):$(PLIST_MAIN) $(NIB_MAIN)
  $(PLIST_MAIN):src/opt/macos/Info.plist;$(PRECMD) cp $< $@
  $(NIB_MAIN):src/opt/macos/Main.xib;$(PRECMD) ibtool --compile $@ $<

  INPUT_ICONS:=$(wildcard src/opt/macos/appicon.iconset/*)
  $(ICON_MAIN):$(INPUT_ICONS);$(PRECMD) iconutil -c icns -o $@ src/opt/macos/appicon.iconset

  RUNCMD:=open -W $(BUNDLE_MAIN) --args --reopen-tty=$$(tty) --chdir=$$(pwd) /Users/andy/rom/snes/m/mickey_and_donald_3.smc

else ifeq ($(UNAMES),Linux)

  CCWARN:=-Werror -Wno-deprecated-declarations -Wno-parentheses -Wno-constant-logical-operand -Wno-unused-result -Wno-format-truncation
  CCOPT:=-c -MMD -O2 -Isrc -Isrc/lib -DUSE_linux=1 -DUSE_romassist=1 -I$(HDRROMASSIST)
  CC:=gcc $(CCOPT) $(CCWARN)
  CXX:=g++ $(CCOPT) $(CCWARN)
  OBJC:=gcc -xobjective-c $(CCOPT) $(CCWARN)
  LD:=g++
  LDPOST:=$(LIBROMASSIST) -lz -lX11 -lGLX -lGLESv2 -lasound -lpthread -lm -lpulse -lpulse-simple -ldrm -lgbm -lEGL
  OPT:=

  EXE:=$(OUTDIR)/aksnes9x

  RUNCMD:=$(EXE) /home/andy/rom/snes/s/super_mario_world.smc

endif

$(MIDDIR)/%.o:src/%.c;$(PRECMD) $(CC) -o $@ $<
$(MIDDIR)/%.o:src/%.cpp;$(PRECMD) $(CXX) -o $@ $<
$(MIDDIR)/%.o:src/%.m;$(PRECMD) $(OBJC) -o $@ $<

OPTAVAILABLE:=$(notdir $(wildcard src/opt/*))
OPTIGNORE:=$(filter-out $(OPT),$(OPTAVAILABLE))
MIDOPTIGNORE:=$(addprefix $(MIDDIR)/opt/,$(addsuffix /%,$(OPTIGNORE)))
SRCOPTIGNORE:=$(addprefix src/opt/,$(addsuffix /%,$(OPTIGNORE)))

SRCCFILES:=$(filter-out $(SRCOPTIGNORE),$(shell find src -name '*.[cm]' -or -name '*.cpp'))
OPTCFILES:=$(filter src/opt/%,$(SRCCFILES))
MAINCFILES:=$(filter src/main/%,$(SRCCFILES))
CORECFILES:=$(filter-out $(OPTCFILES) $(MAINCFILES),$(SRCCFILES))

GENERATED_FILES:=$(filter-out $(MIDOPTIGNORE),$(GENERATED_FILES))
GENHFILES:=$(filter %.h,$(GENERATED_FILES))
GENCFILES:=$(filter %.c,$(GENERATED_FILES))
GENOPTCFILES:=$(filter $(MIDDIR)/opt/%,$(GENCFILES))
GENMAINCFILES:=$(filter $(MIDDIR)/main/%,$(GENCFILES))
GENCORECFILES:=$(filter-out $(GENOPTCFILES) $(GENMAINCFILES),$(GENCFILES))

COREOFILES:=$(patsubst src/%,$(MIDDIR)/%.o,$(basename $(CORECFILES))) $(GENCOREOFILES:.c=.o)
MAINOFILES:=$(patsubst src/%,$(MIDDIR)/%.o,$(basename $(MAINCFILES))) $(GENMAINCFILES:.c=.o)
OPTOFILES:=$(patsubst src/%,$(MIDDIR)/%.o,$(basename $(OPTCFILES))) $(GENOPTCFILES:.c=.o)
ALLOFILES:=$(COREOFILES) $(MAINOFILES) $(OPTOFILES)
-include $(ALLOFILES:.o=.d)

$(MIDDIR)/%.o:src/%.c|$(GENHFILES);$(PRECMD) $(CC) -o $@ $<
$(MIDDIR)/%.o:$(MIDDIR)/%.c|$(GENHFILES);$(PRECMD) $(CC) -o $@ $<
$(MIDDIR)/%.o:src/%.m|$(GENHFILES);$(PRECMD) $(OBJC) -o $@ $<
$(MIDDIR)/%.o:$(MIDDIR)/%.m|$(GENHFILES);$(PRECMD) $(OBJC) -o $@ $<
$(MIDDIR)/%.o:src/%.cpp|$(GENHFILES);$(PRECMD) $(CXX) -o $@ $<
$(MIDDIR)/%.o:$(MIDDIR)/%.cpp|$(GENHFILES);$(PRECMD) $(CXX) -o $@ $<

all:$(EXE) $(TEST)
$(EXE):$(COREOFILES) $(MAINOFILES) $(OPTOFILES);$(PRECMD) $(LD) -o $@ $(COREOFILES) $(MAINOFILES) $(OPTOFILES) $(LDPOST)

run:$(EXE);$(RUNCMD)

clean:;rm -rf mid out
