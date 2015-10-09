#######################################################################
# File: Makefile
#
# This is a gnu-make makefile for brewser.  It builds (by default) a
#   1bit, English-language version of BREWser and its resources.
#
#######################################################################
.PHONY: all clean

MSDEVDIR := $(subst \,/,$(MSDEVDIR))
MSDEV = "$(MSDEVDIR)"/bin/msdev

ifndef RES_BITDEPTH
  export RES_BITDEPTH=1
endif

ifndef RES_LANGUAGE
  export RES_LANGUAGE=en
endif

ifndef CFG
  export CFG=brewser - Win32 Debug
endif

all: brewser.mod brewser.dll ../brewser.mif brewser.bar

clean:
	$(MSDEV) brewser.dsp /MAKE /CLEAN "$(CFG)"
	$(MAKE) -f brewser_arm.mak clean
	(cd res && $(MAKE) clean)
	rm -f brewser.bar brewser_res.h
	(cd mif && $(MAKE) clean)
	rm -f ../brewser.mif

../brewser.mif: mif/$(RES_LANGUAGE)/brewser_mif.brc mif/$(RES_BITDEPTH)bit/*
	(cd mif && $(MAKE))
	cp mif/brewser.mif ../brewser.mif

brewser.dll: *.c *.h brewser_res.h
	$(MSDEV) brewser.dsp /MAKE "$(CFG)"

brewser.mod: *.c *.h brewser_res.h
	$(MAKE) -f brewser_arm.mak

brewser_res.h brewser.bar: res/$(RES_LANGUAGE)/*.html res/$(RES_LANGUAGE)/brewser.brc res/$(RES_BITDEPTH)bit/*
	(cd res && $(MAKE))
	cp res/brewser.bar res/brewser_res.h .

#============================================================================
# Preparing archives for BREW Distribution System 
#============================================================================
.PHONY: archive cleanarchive

archive: all
	$(MAKE) -s -f brewser_archive.mak

cleanarchive: clean
	$(MAKE) -s -f brewser_archive.mak clean

#============================================================================
# All builds and archives for all bitdepths and languages 
#============================================================================
.PHONY: allclean allarchives

allclean:
	$(MAKE) -s RES_BITDEPTH=1 RES_LANGUAGE=en CFG="brewser - Win32 Release" cleanarchive
	$(MAKE) -s RES_BITDEPTH=1 RES_LANGUAGE=pt CFG="brewser - Win32 Release" cleanarchive
	$(MAKE) -s RES_BITDEPTH=1 RES_LANGUAGE=ru CFG="brewser - Win32 Release" cleanarchive
	$(MAKE) -s RES_BITDEPTH=8 RES_LANGUAGE=en CFG="brewser - Win32 Release" cleanarchive
	$(MAKE) -s RES_BITDEPTH=8 RES_LANGUAGE=pt CFG="brewser - Win32 Release" cleanarchive
	$(MAKE) -s RES_BITDEPTH=8 RES_LANGUAGE=ru CFG="brewser - Win32 Release" cleanarchive

allarchives:
	$(MAKE) -s RES_BITDEPTH=1 RES_LANGUAGE=en CFG="brewser - Win32 Release" cleanarchive archive
	$(MAKE) -s RES_BITDEPTH=1 RES_LANGUAGE=pt CFG="brewser - Win32 Release" cleanarchive archive
	$(MAKE) -s RES_BITDEPTH=1 RES_LANGUAGE=ru CFG="brewser - Win32 Release" cleanarchive archive
	$(MAKE) -s RES_BITDEPTH=8 RES_LANGUAGE=en CFG="brewser - Win32 Release" cleanarchive archive
	$(MAKE) -s RES_BITDEPTH=8 RES_LANGUAGE=pt CFG="brewser - Win32 Release" cleanarchive archive
	$(MAKE) -s RES_BITDEPTH=8 RES_LANGUAGE=ru CFG="brewser - Win32 Release" cleanarchive archive
