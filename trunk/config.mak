PACKAGE=SDL_net++
VERSION=1.0.0

SDL_PREFIX=$(shell sdl-config --prefix)
SDL_CONFIG=$(SDL_PREFIX)/bin/sdl-config
SDL_LIBS=$(shell $(SDL_CONFIG) --libs)
SDL_CFLAGS=$(shell $(SDL_CONFIG) --cflags)

SDL_NET_PREFIX=$(SDL_PREFIX)
SDL_NET_CFLAGS=-I$(SDL_NET_PREFIX)/include
SDL_NET_LIBS=-L$(SDL_NET_PREFIX)/libs -lSDL_net

CPPFLAGS+=-DVERSION=\"$(VERSION)\" -DPACKAGE=\"$(PACKAGE)\" $(SDL_CFLAGS) $(SDL_NET_CFLAGS)
CXXFLAGS+=-O2 -Wall -Werror -ansi -pedantic -g
LDFLAGS=-O2 -g
LIBS+=$(SDL_LIBS) $(SDL_NET_LIBS) -lSDL_net

RANLIB=ranlib

#NODEPS=true
CXX_LD=true

#EXE=.exe

INSTALL_PREFIX=$(SDL_PREFIX)
INSTALL_INCDIR=$(INSTALL_PREFIX)/include/SDL/
INSTALL_LIBDIR=$(INSTALL_PREFIX)/lib
INSTALL_BINDIR=$(INSTALL_PREFIX)/bin

### RULES ######################################################################

ifdef CXX_LD
LD=$(CXX)
endif

ifeq ($(TOP),)
$(error TOP is not set)
endif

ifneq ($(AT_TOP),)
$(error AT_TOP is reserved)
endif

ifneq ($(EXE),)
PROGEXES=$(addsuffix $(EXE),$(PROGS))
endif
#$(shell echo PROGEXES=$(PROGEXES) >&2)

TOPDIR=$(shell cd "$(TOP)" ; pwd)
SUBDIR=$(subst $(TOPDIR),,$(shell pwd))
#$(shell echo TOPDIR=$(TOPDIR) >&2)
#$(shell echo SUBDIR=$(SUBDIR) >&2)

ifeq ($(TOPDIR),$(shell pwd))
AT_TOP=true
endif

CXXSRCS+=$(wildcard *.cc)
CXXOBJS+=$(patsubst %.cc,%.o,$(CXXSRCS))

HEADERS+=$(wildcard *.h)
SRCS+=$(CXXSRCS)
OBJS+=$(CXXOBJS)

.PHONY: $(DIRS) all clean new install dist deps rmdeps newdeps uninstall

DIST="$(TOP)/.dist/$(PACKAGE)-$(VERSION)"

all::

ifdef AT_TOP
dist::
	@echo "[DIST START]"
	@rm -rf .dist
	@mkdir -p "$(DIST)"
	@cp $(SRCS) $(HEADERS) Makefile config.mak $(EXTRA_DIST) $(DIST)
endif

deps::
# if deps are needed they will be made automagically now

newdeps:: rmdeps
	@$(MAKE) -s deps

clean rmdeps uninstall:: $(DIRS)
	@$(foreach subdir,$^,echo "[CHDIR] $(subdir)" && $(MAKE) -s -C "$(subdir)" NODEPS=true $@ &&) true
ifdef AT_TOP
ifdef DIRS
	@echo "[CHDIR] TOP"
endif
endif

all install dist deps:: $(DIRS)
	@$(foreach subdir,$^,echo "[CHDIR] $(subdir)" && $(MAKE) -s -C "$(subdir)" $@ &&) true
ifdef AT_TOP
ifdef DIRS
	@echo "[CHDIR] TOP"
endif
endif

all install:: all-in-dir

ifndef NO_INSTALL
 ifneq ($(HEADERS),)
 install::
	@echo "[INSTALL include] $(HEADERS)"
	@install -d $(INSTALL_INCDIR)
	@install -m 0644 $(HEADERS) $(INSTALL_INCDIR)
 endif

 ifneq ($(LIBRARY),)
 install:: all-in-dir
	@echo "[INSTALL lib] $(LIBRARY)"
	@install -d $(INSTALL_LIBDIR)
	@install -m 0644 $(LIBRARY) $(INSTALL_LIBDIR)
 endif

 ifneq ($(PROGEXES),)
 install:: all-in-dir
	@echo "[INSTALL bin] $(PROGEXES)"
	@install -d $(INSTALL_BINDIR)
	@install $(PROGEXES) $(INSTALL_BINDIR)
 else
  ifneq ($(PROGS),)
  install:: all-in-dir
	@echo "[INSTALL bin] $(PROGS)"
	@install -d $(INSTALL_BINDIR)
	@install $(PROGS) $(INSTALL_BINDIR)
  endif
 endif
endif

ifndef NO_INSTALL
 ifneq ($(HEADERS),)
 uninstall::
	@echo "[UNINSTALL include] $(HEADERS)"
	-@cd $(INSTALL_INCDIR) && rm -f $(HEADERS)
 endif

 ifneq ($(LIBRARY),)
 uninstall:: all-in-dir
	@echo "[UNINSTALL lib] $(LIBRARY)"
	-@cd $(INSTALL_LIBDIR) && rm -f $(LIBRARY)
 endif

 ifneq ($(PROGEXES),)
 uninstall:: all-in-dir
	@echo "[UNINSTALL bin] $(PROGEXES)"
	-@cd $(INSTALL_BINDIR) && rm -f $(PROGEXES)
 else
  ifneq ($(PROGS),)
  uninstall:: all-in-dir
	@echo "[UNINSTALL bin] $(PROGS)"
	-@cd $(INSTALL_BINDIR) && rm -f $(PROGS)
  endif
 endif
endif

rmdeps::
	@echo "[RMDEPS]"
	@rm -rf .depend

ifdef AT_TOP
dist::
	@echo "[DIST FINISH]"
	@tar -zcvf "$(PACKAGE)-$(VERSION).tar.gz" -C .dist "$(PACKAGE)-$(VERSION)"
	@rm -rf .dist
	@echo "[DIST READY] $(PACKAGE)-$(VERSION).tar.gz"
else
dist::
	@echo "[DIST]"
	@mkdir "$(DIST)/$(SUBDIR)"
	@cp $(SRCS) $(HEADERS) $(EXTRA_DIST) Makefile "$(DIST)$(SUBDIR)"
endif

clean::
	@echo "[CLEAN]"
	@rm -rf $(PROGS) $(PROGEXES) $(LIBRARY) $(OBJS) .dist .depend

new::
	@$(MAKE) -s NODEPS=true clean
	@$(MAKE) -s all

%$(EXE): %.cc
	@echo "[CXX->EXE$(EXE)] $@"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LIBS) $(DEPLIBS)

%$(EXE): %.o
	@echo "[LD->EXE$(EXE)] $@"
	@$(LD) $(LDFLAGS) -o $@ $^ $(LIBS) $(DEPLIBS)

%.o: %.cc
	@echo "[CXX->O] $<"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<

all-in-dir:: $(LIBRARY)

ifdef PROGEXES
all-in-dir:: $(PROGEXES)
$(PROGEXES): $(DEPLIBS) $(LIBRARY)
else
all-in-dir:: $(PROGS)
$(PROGS): $(DEPLIBS) $(LIBRARY)
endif

$(LIBRARY): $(OBJS)
	@echo "[AR->A] $@"
	@$(AR) crv $@ $?
	@echo "[RANLIB->A] $@"
	@$(RANLIB) $@

$(DEPLIBS):
	@$(MAKE) -C $(dir $@) $(notdir $@)

ifndef NODEPS
ifneq ($(PROGS)$(LIBRARY),)
.depend:: Makefile $(TOP)/config.mak $(SRCS) $(GEN_DEPS_ON) $(HEADERS)
	@echo "[DEPENDS]"
	@makedepend -f- -- $(CPPFLAGS) -- $(SRCS) > $@ 2>/dev/null
-include .depend
else
.depend::
endif
endif
