#============================================================================
#  Name:
#    brewser_arm.mak
#
#  Description:
#    Makefile to build the brewser downloadable module with ADS 1.01
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#
#   The above targets can be made with the following command:
#
#     make -f brewser_arm.mak [target]
#
# Assumptions:
#   1. The variable TARGET below has be set to the proper base name of the
#        module being built
#   2. ../xmod exists and contains the include makefile and xmodstub.c 
#       (the ARM mode AEEMod_Load entry stub)
#
#        Copyright © 2000-2002 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Set module name
#----------------------------------------------------------------------------

MODNAME = brewser

#----------------------------------------------------------------------------
# Set MODOBJS, these'll be compiled and pulled in
#----------------------------------------------------------------------------

MODOBJS = bookmarks.o brewser.o config.o headermaker.o lcgieng.o prefmgr.o \
          refresher.o slider.o statusbox.o tooltip.o webcache.o util.o 

MODLIBS = 

MODINC = -I../..

#----------------------------------------------------------------------------
# Overrideable from the command line
#----------------------------------------------------------------------------

ifndef XMODDIR
XMODDIR = ../xmod
endif

#----------------------------------------------------------------------------
# BREW SDK location
#
# Change this to location of the BREW sdk directory, or define it on 
#   the nmake command line
#  
#----------------------------------------------------------------------------

ifndef BREWDIR
BREWDIR = ../..
endif

BREWDIR := $(subst \,/,$(BREWDIR))

#----------------------------------------------------------------------------
# Pull in my little makie wakie, include last!
#----------------------------------------------------------------------------

include $(XMODDIR)/xmod_arm.min
