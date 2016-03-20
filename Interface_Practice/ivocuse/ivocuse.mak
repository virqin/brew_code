#============================================================================
#  Name:
#    $(TARGET).MAK
#
#  Description:
#    Makefile to build the $(TARGET) downloadable module.
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#
#   The above targets can be made with the following command:
#
#     nmake /f $(TARGET).mak [target]
#
#  Assumptions:
#    1. The environment variable ADSHOME is set to the root directory of the
#       arm tools.
#    2. The version of ADS is 1.2 or above.
#
#  Notes:
#    None.
#
#
#        Copyright © 2000-2003 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================
BREW_HOME      =$(BREWDIR)
ARM_HOME       =$(ARMHOME)
TARGET         =D:\PROGRA~1\BREWSD~1.0\Examples\ivocuse\ivocuse
OBJS           =AEEAPP~1.o AEEMOD~1.o ivocuse.o 
APP_INCLUDES   =-I . -I ..\..\inc

#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

EXETYPE    = elf                # Target image file format
MODULE     = mod                # Downloadable module extension

#-------------------------------------------------------------------------------
# Target compile time symbol definitions
#
# Tells the SDK source stuffs that we're building a dynamic app.
#-------------------------------------------------------------------------------

DYNAPP          = -DDYNAMIC_APP


#-------------------------------------------------------------------------------
# Software tool and environment definitions
#-------------------------------------------------------------------------------

AEESRCPATH = $(BREW_HOME)\src
AEEINCPATH = $(BREW_HOME)\inc

ARMBIN = $(ARM_HOME)\bin        # ARM ADS application directory
ARMINC = $(ARM_HOME)\include    # ARM ADS include file directory
ARMLIB = $(ARM_HOME)\lib        # ARM ADS library directory

ARMCC   = $(ARMBIN)\armcc       # ARM ADS ARM 32-bit inst. set ANSI C compiler
LD      = $(ARMBIN)\armlink     # ARM ADS linker
HEXTOOL = $(ARMBIN)\fromelf     # ARM ADS utility to create hex file from image

OBJ_CMD    = -o                 # Command line option to specify output filename

#-------------------------------------------------------------------------------
# Processor architecture options
#-------------------------------------------------------------------------------

CPU = -cpu ARM7TDMI             # ARM7TDMI target processor

#-------------------------------------------------------------------------------
# ARM Procedure Call Standard (APCS) options
#-------------------------------------------------------------------------------

ROPI     = ropi                 # Read-Only(code) Position independence
INTERWRK = interwork            # Allow ARM-Thumb interworking

APCS = -apcs /$(ROPI)/$(INTERWRK)/norwpi

#-------------------------------------------------------------------------------
# Additional compile time error checking options
#-------------------------------------------------------------------------------

CHK = -fa                       # Check for data flow anomolies

#-------------------------------------------------------------------------------
# Compiler output options
#-------------------------------------------------------------------------------

OUT = -c                        # Object file output only

#-------------------------------------------------------------------------------
# Compiler/assembler debug options
#-------------------------------------------------------------------------------

DBG = -g                        # Enable debug

#-------------------------------------------------------------------------------
# Compiler optimization options
#-------------------------------------------------------------------------------

OPT = -Ospace -O2               # Full compiler optimization for space

#-------------------------------------------------------------------------------
# Compiler code generation options
#-------------------------------------------------------------------------------

END = -littleend                # Compile for little endian memory architecture
ZA  = -zo                       # LDR may only access 32-bit aligned addresses

CODE = $(END) $(ZA)


#-------------------------------------------------------------------------------
# Include file search path options
#-------------------------------------------------------------------------------

INC = -I. -I$(AEEINCPATH) $(APP_INCLUDES)


#-------------------------------------------------------------------------------
# Compiler pragma emulation options
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Linker options
#-------------------------------------------------------------------------------

LINK_CMD = -o                    #Command line option to specify output file
                                 #on linking

ROPILINK = -ropi                 #Link image as Read-Only Position Independent

LINK_ORDER = -first AEEMod_Load

#-------------------------------------------------------------------------------
# HEXTOOL options
#-------------------------------------------------------------------------------

BINFORMAT = -bin


#-------------------------------------------------------------------------------
# Compiler flag definitions
#-------------------------------------------------------------------------------

CFLAGS0 = $(OUT) $(DYNAPP) $(CPU) $(APCS) $(CODE) $(CHK) $(DBG)
CFLAGS  = $(CFLAGS0) $(OPT)

#-------------------------------------------------------------------------------
# Linker flag definitions
#-------------------------------------------------------------------------------

# the -entry flag is not really needed, but it keeps the linker from reporting
# warning L6305W (no entry point).  The address
LFLAGS = $(ROPILINK) -rwpi -entry 0x8000#

#----------------------------------------------------------------------------
# Default target
#----------------------------------------------------------------------------

all :  $(TARGET).$(MODULE)

#----------------------------------------------------------------------------
# Clean target
#----------------------------------------------------------------------------

# The object subdirectory, target image file, and target hex file are deleted.

clean :
        @echo ---------------------------------------------------------------
        @echo CLEAN
        -del /f $(OBJS)
        -del /f $(TARGET).$(EXETYPE)
        -del /f $(TARGET).$(MODULE)
        @echo ---------------------------------------------------------------

#============================================================================
#                           DEFAULT SUFFIX RULES
#============================================================================

# The following are the default suffix rules used to compile all objects that
# are not specifically included in one of the module specific rules defined
# in the next section.

# The following macros are used to specify the output object file, MSG_FILE
# symbol definition and input source file on the compile line in the rules
# defined below.

SRC_FILE = $(@F:.o=.c)                  # Input source file specification
OBJ_FILE = $(OBJ_CMD) $(@F)   # Output object file specification

.SUFFIXES :
.SUFFIXES : .o .dep .c

#--------------------------------------------------------------------------
# C code inference rules
#----------------------------------------------------------------------------

.c.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE) $(SRC_FILE)
        @echo ---------------------------------------------------------------

.c.mix:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(ARMCC) -S -fs $(CFLAGS) $(INC) $(OBJ_FILE) $<
        @echo ---------------------------------------------------------------


{$(AEESRCPATH)}.c.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE) $(AEESRCPATH)\$(SRC_FILE)
        @echo ---------------------------------------------------------------


#===============================================================================
#                           MODULE SPECIFIC RULES
#===============================================================================

APP_OBJS = $(OBJS)


#----------------------------------------------------------------------------
# Lib file targets
#----------------------------------------------------------------------------

$(TARGET).$(MODULE) : $(TARGET).$(EXETYPE)
        @echo ---------------------------------------------------------------
        @echo TARGET $@
        $(HEXTOOL)  $(TARGET).$(EXETYPE) $(BINFORMAT) $(TARGET).$(MODULE)

$(TARGET).$(EXETYPE) : $(APP_OBJS)
        @echo ---------------------------------------------------------------
        @echo TARGET $@
        $(LD) $(LINK_CMD) $(TARGET).$(EXETYPE) $(LFLAGS) $(APP_OBJS) $(LINK_ORDER)

#----------------------------------------------------------------------------
# Applet Specific Rules
#----------------------------------------------------------------------------


RULE1 = d:\PROGRA~1\BREWSD~1.0\src
{$(RULE1)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE1)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


RULE2 = d:\PROGRA~1\BREWSD~1.0\Examples\ivocuse
{$(RULE2)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE2)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


# --------------------------------------------
# DEPENDENCY LIST, DO NOT EDIT BELOW THIS LINE
# --------------------------------------------


AEEAPP~1.o:	d:\PROGRA~1\BREWSD~1.0\src\AEEAPP~1.C
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEClassIDs.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEError.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEComdef.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEVCodes.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEDisp.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEBitmap.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEFont.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEBitmap.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEImage.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEESoundPlayer.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEESound.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEControls_res.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEELngCode.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEStdLib.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEComdef.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEAppGen.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEModGen.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
AEEAPP~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEModGen.h
AEEMOD~1.o:	d:\PROGRA~1\BREWSD~1.0\src\AEEMOD~1.C
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEHeap.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEClassIDs.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEError.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEComdef.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEStdLib.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEComdef.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEVCodes.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEDisp.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEBitmap.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEFont.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEBitmap.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEImage.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEESoundPlayer.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEESound.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEControls_res.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEELngCode.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEStdLib.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEModGen.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEAppGen.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
AEEMOD~1.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEModGen.h
ivocuse.o:	d:\PROGRA~1\BREWSD~1.0\Examples\ivocuse\ivocuse.c
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEModGen.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEClassIDs.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEError.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEComdef.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEVCodes.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEDisp.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEBitmap.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEFont.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEBitmap.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEImage.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEESoundPlayer.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEESound.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEControls_res.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEELngCode.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEAppGen.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEModGen.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEMenu.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEStdLib.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEComdef.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEVocoder.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEE.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEEShell.h
ivocuse.o:	D:\PROGRA~1\BREWSD~1.0\inc\AEESound.h
ivocuse.o:	d:\PROGRA~1\BREWSD~1.0\Examples\ivocuse\ivocuse.bid
ivocuse.o:	d:\PROGRA~1\BREWSD~1.0\Examples\ivocuse\ivocuse_res.h
