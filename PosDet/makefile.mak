#============================================================================
#  Name:
#    MAKEFILE.MAK
#
#  Description:
#    Makefile to build the downloadable module.
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#
#   The above targets can be made with the following command:
#
#     nmake -f makefile.mak [target]
#
# Assumptions:
#   1. The ARM ADS tools are installed in $(ARMHOME)
#   2. The BREW sdk source files AEEAppGen.c and AEEModGen.c are installed
#      in $(BREWDIR)/src
#   3. The BREW sdk include files are installed in $(BREWDIR)/inc
#   4. The variable TARGET below has been set to the proper base name of the
#      module being built
#
# Copyright © 2003 QUALCOMM Incorporated.  All Rights Reserved.
#----------------------------------------------------------------------------
#============================================================================

#-------------------------------------------------------------------------------
# Target file name and type and path definitions
#-------------------------------------------------------------------------------

TARGET     = sampleposdet      # Target name for output files and object dir
MODULE     = mod               # Downloadable module extension

!IFNDEF BREWDIR
!ERROR BREWDIR must be defined as the directory containing src and inc subdirs
!ENDIF

SUPPORT_DIR    = $(BREWDIR)\src
SUPPORT_INCDIR = $(BREWDIR)\inc
BREW_RES       = $(BREWDIR)\bin\brew_res.exe

!IFNDEF ARMHOME
!ERROR ARMHOME must be defined as the ARM installation dir with Bin, Include & Lib subdirs
!ENDIF

#-------------------------------------------------------------------------------
# Target compile time symbol definitions
#-------------------------------------------------------------------------------
DEBUG=NODEBUG

DEFINES = -DDYNAMIC_APP -D$(DEBUG)

#-------------------------------------------------------------------------------
# Software tool and environment definitions
#-------------------------------------------------------------------------------

ARMBIN = $(ARMHOME)\Bin  			# ARM ADS application directory
ARMINC = $(ARMHOME)\Include			# ARM ADS include file directory
ARMLIB = $(ARMHOME)\Lib				# ARM ADS library directory

ARMCC   = "$(ARMBIN)\armcc"                     # ARM ADS ARM 32-bit ANSI C compiler
ARMCPP  = "$(ARMBIN)\armcpp"                    # ARM ADS ARM 32-bit ANSI CPP compiler
LD      = "$(ARMBIN)\armlink"                   # ARM ADS linker
FROMELF = "$(ARMBIN)\fromelf"                   # ARM ADS utility to create bin file from image

#-------------------------------------------------------------------------------
# Processor architecture options
#-------------------------------------------------------------------------------

CPU = -littleend -cpu ARM7TDMI -fpu softvfp 	# ARM7TDMI targets

#-------------------------------------------------------------------------------
# ARM Procedure Call Standard (APCS) options
#-------------------------------------------------------------------------------

APCS = -apcs /noswst/inter/ropi/rwpi

#-------------------------------------------------------------------------------
# C Compiler options
#-------------------------------------------------------------------------------

COPT = -zo -zas4 -fa

#-------------------------------------------------------------------------------
# Compiler/assembler debug options
#-------------------------------------------------------------------------------

#DBG = -g                         # Enable debug
DBG =                       	# No debug

#-------------------------------------------------------------------------------
# Compiler optimization options
#-------------------------------------------------------------------------------

OPT = -Ospace -O2   # Full compiler optimization for space
#OPT = -Otime        # Optimize for time
#OPT = -O0           # No optimization at all

#-------------------------------------------------------------------------------
# Include file search path options
#-------------------------------------------------------------------------------

!IFDEF BREWAEEDIR
INC = -I$(BREWAEEDIR)
!ELSE
INC =
!ENDIF

INC = $(INC) -I$(SUPPORT_INCDIR)

#-------------------------------------------------------------------------------
# Compiler flag definitions
#-------------------------------------------------------------------------------

CFLAGS = $(CPU) $(APCS) $(COPT) $(DBG) $(OPT) $(DEFINES) $(INC) -Wy

#-------------------------------------------------------------------------------
# Linker options
#-------------------------------------------------------------------------------
LFLAGS = -ropi -rwpi -remove -first AEEMod_Load -strict

#----------------------------------------------------------------------------
# Default target
#----------------------------------------------------------------------------
all :  $(TARGET).$(MODULE)

#============================================================================
#                           DEFAULT SUFFIX RULES
#============================================================================

# The following are the default suffix rules used to compile all objects that
# are not specifically included in one of the module specific rules defined
# in the next section.

# The following macros are used to specify the output object file, MSG_FILE
# symbol definition and input source file on the compile line in the rules
# defined below.

.SUFFIXES :
.SUFFIXES : .o .c .cpp .bar .bri

#--------------------------------------------------------------------------
# C code inference rules
#----------------------------------------------------------------------------

.c.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $@
        $(ARMCC) -ansic -c $(CFLAGS) -o $@ $<
        @echo ---------------------------------------------------------------

.cpp.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $@
        $(ARMCPP) -cpp -c $(CFLAGS) -o $@ $<
        @echo ---------------------------------------------------------------

{$(SUPPORT_DIR)}.c.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $@
        $(ARMCC) -ansic -c $(CFLAGS) -o $@ $<
        @echo ---------------------------------------------------------------

{$(SUPPORT_DIR)}.cpp.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $@
        $(ARMCPP) -cpp -c $(CFLAGS) -o $@ $<
        @echo ---------------------------------------------------------------

#--------------------------------------------------------------------------
# BAR code inference rules
#----------------------------------------------------------------------------
.bri.bar:
	@echo ---------------------------------------------------------------
	@echo OBJECT $@
	$(BREW_RES) -c -f $< -h -o $@
	@echo ---------------------------------------------------------------

#===============================================================================
#                           MODULE SPECIFIC RULES
#===============================================================================

APP_OBJS =  AEEAppGen.o AEEModGen.o sampleposdet.o SP_MainMenu.o SP_GetGPSInfo.o SP_Track.o SP_ConfigMenu.o

#----------------------------------------------------------------------------
# Target rules
#----------------------------------------------------------------------------

$(TARGET).$(MODULE) : $(TARGET).elf
        @echo ---------------------------------------------------------------
        @echo TARGET $@
        $(FROMELF) $(TARGET).elf -bin -o $@ 

#	perl zeropadbin.pl $@

$(TARGET).elf : $(APP_OBJS)
        @echo ---------------------------------------------------------------
        @echo TARGET $@
	-$(LD) $(LFLAGS) $(APP_OBJS) -o $@

clean :
	@echo ---------------------------------------------------------------
	@echo CLEAN
	-del /f AEEAppGen.o
	-del /f AEEModGen.o
	-del /f $(TARGET).bar
	-del /f $(TARGET)_res.h
	-del /f $(APP_OBJS)
	-del /f $(TARGET).elf
	-del /f $(TARGET).map
	-del /f $(TARGET).$(MODULE)
	@echo ---------------------------------------------------------------

# ------------
# DEPENDENCIES

AEEAppGen.o : $(SUPPORT_DIR)\AEEAppGen.c
AEEAppGen.o : $(SUPPORT_INCDIR)\AEEAppGen.h

AEEModGen.o : $(SUPPORT_DIR)\AEEModGen.c
AEEModGen.o : $(SUPPORT_INCDIR)\AEEModGen.h

sampleposdet.o : sampleposdet.c sampleposdet.bid sampleposdet_res.h 
sampleposdet_res.h : sampleposdet.bar

$(TARGET).o : $(SUPPORT_INCDIR)\AEEAppGen.h
$(TARGET).o : $(SUPPORT_INCDIR)\AEEModGen.h


