TOP=..

CPPFLAGS=-I$(TOP)/src
DEPLIBS=$(TOP)/src/libSDL_net++.a
GEN_DEPS_ON=$(wildcard $(TOP)/src/*.h)
PROGS=$(patsubst %.cc,%,$(wildcard *.cc))
NO_INSTALL=true

include $(TOP)/config.mak
