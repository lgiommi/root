# Module.mk for reflex module
# Copyright (c) 2000 Rene Brun and Fons Rademakers
#
# Author: Fons Rademakers, 29/2/2000

MODNAME      := reflex
MODDIR       := cint/$(MODNAME)
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

REFLEXDIR    := $(MODDIR)
REFLEXDIRS   := $(REFLEXDIR)/src
REFLEXDIRI   := $(REFLEXDIR)/inc

REFLEXLIB    := $(LPATH)/libReflex.$(SOEXT)

# genreflex
ifeq ($(PLATFORM),win32)
RFLX_REFLEXLL   = lib/libReflex.lib
else
# test suite
RFLX_REFLEXLL   = -Llib -lReflex
ifneq ($(PLATFORM),fbsd)
ifneq ($(PLATFORM),obsd)
RFLX_REFLEXLL   += -ldl
endif
endif
endif

ifeq ($(PLATFORM),solaris)
RFLX_REFLEXLL   += -ldemangle
endif

RFLX_GENMAPX   = bin/genmap$(EXEEXT)
