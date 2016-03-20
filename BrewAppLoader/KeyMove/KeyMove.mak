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
TARGET         =KeyMove
OBJS           =AEEAPP~1.o AEEMOD~2.o KeyMove.o 
APP_INCLUDES   =-I $(BREWDIR)\inc 

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


RULE1 = c:\PROGRA~1\BREW31~1.5\sdk\src
{$(RULE1)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE1)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


RULE2 = c:\BREWAP~1\KeyMove
{$(RULE2)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE2)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


# --------------------------------------------
# DEPENDENCY LIST, DO NOT EDIT BELOW THIS LINE
# --------------------------------------------


aeeapp~1.o:	$(BREWDIR)\src\aeeapp~1.c
aeeapp~1.o:	$(BREWDIR)\inc\aee.h
aeeapp~1.o:	$(BREWDIR)\inc\aeegroupids.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeclassids.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeclsid_filemgr.bid
aeeapp~1.o:	$(BREWDIR)\inc\aeeerror.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeneterr.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeporterr.h
aeeapp~1.o:	$(BREWDIR)\inc\aeecomdef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeedatetime.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeinterface.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
aeeapp~1.o:	$(BREWDIR)\inc\aeecomdef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeiimage.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestderr.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeiastream.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeidisplay.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeergbval.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeerasterop.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeepoint.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeidib.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeifont.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeimageinfo.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeiapplet.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
aeeapp~1.o:	$(BREWDIR)\inc\aeequeryinterface.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeinterface.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
aeeapp~1.o:	$(BREWDIR)\inc\aeecomdef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeeapp~1.o:	$(BREWDIR)\inc\aeecomdef.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeiastream.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeshell.h
aeeapp~1.o:	$(BREWDIR)\inc\aee.h
aeeapp~1.o:	$(BREWDIR)\inc\aeevcodes.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeeapp~1.o:	$(BREWDIR)\inc\aeedisp.h
aeeapp~1.o:	$(BREWDIR)\inc\aee.h
aeeapp~1.o:	$(BREWDIR)\inc\aeebitmap.h
aeeapp~1.o:	$(BREWDIR)\inc\aee.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmapdev.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeidib.h
aeeapp~1.o:	$(BREWDIR)\inc\aeefont.h
aeeapp~1.o:	$(BREWDIR)\inc\aeebitmap.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeifont.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeidisplay.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeimage.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeiimage.h
aeeapp~1.o:	$(BREWDIR)\inc\aee.h
aeeapp~1.o:	$(BREWDIR)\inc\aeesoundplayer.h
aeeapp~1.o:	$(BREWDIR)\inc\aee.h
aeeapp~1.o:	$(BREWDIR)\inc\aeesound.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeisound.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeiresourcectl.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
aeeapp~1.o:	$(BREWDIR)\inc\aee.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeresourcectl.h
aeeapp~1.o:	$(BREWDIR)\inc\aee.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeclsid_respriv_sound.bid
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeiresourcectl.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeresctlsound.bid
aeeapp~1.o:	$(BREWDIR)\inc\aeecontrols_res.h
aeeapp~1.o:	$(BREWDIR)\inc\aeelngcode.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeedeviceitems.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeeaflags.h
aeeapp~1.o:	$(BREWDIR)\inc\..\..\inc\aeemif.h
aeeapp~1.o:	$(BREWDIR)\inc\aeestdlib.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeoldvalist.h
aeeapp~1.o:	$(BREWDIR)\inc\aeecomdef.h
aeeapp~1.o:	$(BREWDIR)\inc\aee.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeappgen.h
aeeapp~1.o:	$(BREWDIR)\inc\aee.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeshell.h
aeeapp~1.o:	$(BREWDIR)\inc\aeemodgen.h
aeeapp~1.o:	$(BREWDIR)\inc\aee.h
aeeapp~1.o:	$(BREWDIR)\inc\aeeshell.h
aeeapp~1.o:	$(BREWDIR)\inc\aeemodgen.h
aeeapp~1.o:	$(BREWDIR)\inc\nmdef.h
aeemod~2.o:	$(BREWDIR)\src\aeemod~2.c
aeemod~2.o:	$(BREWDIR)\inc\aeeheap.h
aeemod~2.o:	$(BREWDIR)\inc\aee.h
aeemod~2.o:	$(BREWDIR)\inc\aeegroupids.h
aeemod~2.o:	$(BREWDIR)\inc\aeeclassids.h
aeemod~2.o:	$(BREWDIR)\inc\aeeclsid_filemgr.bid
aeemod~2.o:	$(BREWDIR)\inc\aeeerror.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeneterr.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeporterr.h
aeemod~2.o:	$(BREWDIR)\inc\aeecomdef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeedatetime.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeemod~2.o:	$(BREWDIR)\inc\aeeinterface.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
aeemod~2.o:	$(BREWDIR)\inc\aeecomdef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeiimage.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestderr.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeiastream.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeidisplay.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeergbval.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeerasterop.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeepoint.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeidib.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeifont.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeimageinfo.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeiapplet.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
aeemod~2.o:	$(BREWDIR)\inc\aeequeryinterface.h
aeemod~2.o:	$(BREWDIR)\inc\aeeinterface.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
aeemod~2.o:	$(BREWDIR)\inc\aeecomdef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeemod~2.o:	$(BREWDIR)\inc\aeecomdef.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeiastream.h
aeemod~2.o:	$(BREWDIR)\inc\aeestdlib.h
aeemod~2.o:	$(BREWDIR)\inc\aeeoldvalist.h
aeemod~2.o:	$(BREWDIR)\inc\aeecomdef.h
aeemod~2.o:	$(BREWDIR)\inc\aee.h
aeemod~2.o:	$(BREWDIR)\inc\aeestdlib.h
aeemod~2.o:	$(BREWDIR)\inc\aeemodgen.h
aeemod~2.o:	$(BREWDIR)\inc\aee.h
aeemod~2.o:	$(BREWDIR)\inc\aeeshell.h
aeemod~2.o:	$(BREWDIR)\inc\aee.h
aeemod~2.o:	$(BREWDIR)\inc\aeevcodes.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
aeemod~2.o:	$(BREWDIR)\inc\aeedisp.h
aeemod~2.o:	$(BREWDIR)\inc\aee.h
aeemod~2.o:	$(BREWDIR)\inc\aeebitmap.h
aeemod~2.o:	$(BREWDIR)\inc\aee.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmapdev.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeidib.h
aeemod~2.o:	$(BREWDIR)\inc\aeefont.h
aeemod~2.o:	$(BREWDIR)\inc\aeebitmap.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeifont.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeidisplay.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
aeemod~2.o:	$(BREWDIR)\inc\aeeimage.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeiimage.h
aeemod~2.o:	$(BREWDIR)\inc\aee.h
aeemod~2.o:	$(BREWDIR)\inc\aeesoundplayer.h
aeemod~2.o:	$(BREWDIR)\inc\aee.h
aeemod~2.o:	$(BREWDIR)\inc\aeesound.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeisound.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeiresourcectl.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
aeemod~2.o:	$(BREWDIR)\inc\aee.h
aeemod~2.o:	$(BREWDIR)\inc\aeeresourcectl.h
aeemod~2.o:	$(BREWDIR)\inc\aee.h
aeemod~2.o:	$(BREWDIR)\inc\aeeclsid_respriv_sound.bid
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeiresourcectl.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeresctlsound.bid
aeemod~2.o:	$(BREWDIR)\inc\aeecontrols_res.h
aeemod~2.o:	$(BREWDIR)\inc\aeelngcode.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeedeviceitems.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeeaflags.h
aeemod~2.o:	$(BREWDIR)\inc\..\..\inc\aeemif.h
aeemod~2.o:	$(BREWDIR)\inc\aeeappgen.h
aeemod~2.o:	$(BREWDIR)\inc\aee.h
aeemod~2.o:	$(BREWDIR)\inc\aeeshell.h
aeemod~2.o:	$(BREWDIR)\inc\aeemodgen.h
aeemod~2.o:	$(BREWDIR)\inc\nmdef.h
keymove.o:	c:\brewap~1\keymove\keymove.c
keymove.o:	$(BREWDIR)\inc\aeemodgen.h
keymove.o:	$(BREWDIR)\inc\aee.h
keymove.o:	$(BREWDIR)\inc\aeegroupids.h
keymove.o:	$(BREWDIR)\inc\aeeclassids.h
keymove.o:	$(BREWDIR)\inc\aeeclsid_filemgr.bid
keymove.o:	$(BREWDIR)\inc\aeeerror.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeneterr.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeporterr.h
keymove.o:	$(BREWDIR)\inc\aeecomdef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeedatetime.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
keymove.o:	$(BREWDIR)\inc\aeeinterface.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
keymove.o:	$(BREWDIR)\inc\aeecomdef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeiimage.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestderr.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeiastream.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeidisplay.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeergbval.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeerasterop.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeepoint.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeidib.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeifont.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeimageinfo.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeiapplet.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeevent.h
keymove.o:	$(BREWDIR)\inc\aeequeryinterface.h
keymove.o:	$(BREWDIR)\inc\aeeinterface.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeinterface.h
keymove.o:	$(BREWDIR)\inc\aeecomdef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
keymove.o:	$(BREWDIR)\inc\aeecomdef.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeiastream.h
keymove.o:	$(BREWDIR)\inc\aeeshell.h
keymove.o:	$(BREWDIR)\inc\aee.h
keymove.o:	$(BREWDIR)\inc\aeevcodes.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeestddef.h
keymove.o:	$(BREWDIR)\inc\aeedisp.h
keymove.o:	$(BREWDIR)\inc\aee.h
keymove.o:	$(BREWDIR)\inc\aeebitmap.h
keymove.o:	$(BREWDIR)\inc\aee.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmap.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibitmapdev.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeidib.h
keymove.o:	$(BREWDIR)\inc\aeefont.h
keymove.o:	$(BREWDIR)\inc\aeebitmap.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeifont.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeidisplay.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeerect.h
keymove.o:	$(BREWDIR)\inc\aeeimage.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeiimage.h
keymove.o:	$(BREWDIR)\inc\aee.h
keymove.o:	$(BREWDIR)\inc\aeesoundplayer.h
keymove.o:	$(BREWDIR)\inc\aee.h
keymove.o:	$(BREWDIR)\inc\aeesound.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeisound.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeibase.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeiresourcectl.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeiqi.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeecallback.h
keymove.o:	$(BREWDIR)\inc\aee.h
keymove.o:	$(BREWDIR)\inc\aeeresourcectl.h
keymove.o:	$(BREWDIR)\inc\aee.h
keymove.o:	$(BREWDIR)\inc\aeeclsid_respriv_sound.bid
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeiresourcectl.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeresctlsound.bid
keymove.o:	$(BREWDIR)\inc\aeecontrols_res.h
keymove.o:	$(BREWDIR)\inc\aeelngcode.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeedeviceitems.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeeaflags.h
keymove.o:	$(BREWDIR)\inc\..\..\inc\aeemif.h
keymove.o:	$(BREWDIR)\inc\aeeappgen.h
keymove.o:	$(BREWDIR)\inc\aee.h
keymove.o:	$(BREWDIR)\inc\aeeshell.h
keymove.o:	$(BREWDIR)\inc\aeemodgen.h
keymove.o:	$(BREWDIR)\inc\aeeshell.h
keymove.o:	c:\brewap~1\keymove\keymove.bid
keymove.o:	c:\brewap~1\keymove\keymove.brh
