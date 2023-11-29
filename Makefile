all:
.SILENT:
PRECMD=echo "  $(@F)" ; mkdir -p $(@D) ;

MIDDIR:=mid
OUTDIR:=out

EHCFG:=../ra3/out/emuhost-config
EH_CFLAGS:=$(shell $(EHCFG) --cflags)
EH_LDFLAGS:=$(shell $(EHCFG) --ldflags)
EH_LIBS:=$(shell $(EHCFG) --libs)
EH_DEPS:=$(shell $(EHCFG) --deps)

CCWARN:=-Werror -Wno-deprecated-declarations -Wno-parentheses -Wno-constant-logical-operand -Wno-unused-result -Wno-format-truncation
CCOPT:=-c -MMD -O2 -Isrc -Isrc/lib -DUSE_linux=1 -DUSE_romassist=1
CC:=gcc $(CCOPT) $(CCWARN) $(EH_CFLAGS)
CXX:=g++ $(CCOPT) $(CCWARN) $(EH_CFLAGS)
OBJC:=gcc -xobjective-c $(CCOPT) $(CCWARN) $(EH_CFLAGS)
LD:=g++ $(EH_LDFLAGS)
LDPOST:=$(EH_LIBS)

EXE:=$(OUTDIR)/aksnes9x

RUNCMD:=$(EXE) /home/andy/rom/snes/s/super_mario_world.smc

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
$(EXE):$(COREOFILES) $(MAINOFILES) $(OPTOFILES) $(EH_DEPS);$(PRECMD) $(LD) -o $@ $(COREOFILES) $(MAINOFILES) $(OPTOFILES) $(LDPOST)

run:$(EXE);$(RUNCMD)

clean:;rm -rf mid out
