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
#        Copyright � 2000-2003 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================
BREW_HOME      =$(BREWDIR)
ARM_HOME       =$(ARMHOME)
TARGET         =ifile
OBJS           =AEEAppGen.o AEEModGen.o ifileusage.o 
APP_INCLUDES   = -I "$(BREWDIR)\inc"

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


# --------------------------------------------
# DEPENDENCY LIST, DO NOT EDIT BELOW THIS LINE
# --------------------------------------------


ifileusage.o:	ifileusage.c
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEAppGen.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEGroupIDs.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEClassIDs.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEECLSID_FILEMGR.bid
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEError.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEENetErr.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEPortErr.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEComdef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdDef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEEvent.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEECallback.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEDateTime.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdDef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEERect.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdDef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEInterface.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEInterface.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEComdef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBase.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdDef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEInterface.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIImage.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBase.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdErr.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEEvent.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIAStream.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBase.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIDisplay.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBase.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEECallback.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBitmap.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIQI.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdDef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEInterface.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEERGBVAL.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdDef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEERasterOp.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdDef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEPoint.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdDef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEERect.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIDIB.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBitmap.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIFont.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdDef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBitmap.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEImageInfo.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdDef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBase.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIApplet.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBase.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEEvent.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEQueryInterface.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEInterface.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEInterface.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEComdef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBase.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEComdef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIQI.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIAStream.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEShell.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEVCodes.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEStdDef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEDisp.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEBitmap.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBitmap.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBitmapDev.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIQI.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEECallback.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIDIB.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEFont.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEBitmap.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIFont.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIDisplay.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEERect.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEImage.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIImage.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEESoundPlayer.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEESound.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEISound.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIBase.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIResourceCtl.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIQI.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEECallback.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEResourceCtl.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEECLSID_RESPRIV_SOUND.BID
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEIResourceCtl.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEResCtlSound.bid
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEControls_res.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEELngCode.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEDeviceItems.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEAFlags.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\..\..\inc\AEEMIF.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEModGen.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEShell.h
ifileusage.o:	AEEUsageAppIDs.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEShell.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEDisp.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEFile.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEMenu.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEShell.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEStdLib.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEOldVaList.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEEComdef.h
ifileusage.o:	C:\PROGRA~1\BREW31~1.5\sdk\inc\AEE.h
