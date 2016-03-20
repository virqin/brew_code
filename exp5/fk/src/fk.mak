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
#    1. The ARM ADS 1.0.1 tools are installed in the c:\ads directory.
#    2. This Makefile can be used with the ARM ADS 1.0.1 Compiler only. 
#
#  Notes:
#    1. While building the application using this make file, the following warnings may be 
#       received. They can be safely ignored:
#       "Warning: C2067I: option -zas will not be supported in future releases of the compiler
#       "Warning: C2071W: option -za will not be supported in future releases of the compiler 
#       "Warning: L6305W: Image does not have an entry point. (Not specified or not set due to 
#          multiple choices).
#  
#
#        Copyright © 2000-2001 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================
ARM_HOME       = C:\PROGRA~1\ARM\ADSv1_2
BREW_HOME      = C:\PROGRA~1\BREWSD~1.1
TARGET         = fk
OBJS	       = AEEAppGen.o AEEModGen.o FirstWindow.o fk.o GAME.o helpfunc.o STATE.o 
APP_INCLUDES   = -I "$(BREWDIR)\inc"

#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

EXETYPE    = elf                # Target image file format
MODULE     = mod                # Downloadable module extension

#-------------------------------------------------------------------------------
# Target compile time symbol definitions
#-------------------------------------------------------------------------------

DYNAPP          = -DDYNAMIC_APP


#-------------------------------------------------------------------------------
# Software tool and environment definitions
#-------------------------------------------------------------------------------

SUPPORT_DIR    = $(BREW_HOME)\src
SUPPORT_INCDIR = $(BREW_HOME)\inc

ARMBIN = $(ARM_HOME)\bin        # ARM ADS application directory
ARMINC = $(ARM_HOME)\include    # ARM ADS include file directory
ARMLIB = $(ARM_HOME)\lib        # ARM ADS library directory

ARMCC    = $(ARMBIN)\armcc      # ARM ADS ARM 32-bit inst. set ANSI CPP compiler
ARMCPP   = $(ARMBIN)\armcpp     # ARM ADS ARM 32-bit inst. set ANSI CPP compiler
LD       = $(ARMBIN)\armlink     # ARM ADS linker
HEXTOOL  = $(ARMBIN)\fromelf     # ARM ADS utility to create hex file from image

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
ZAS = -zas4                     # Min byte alignment for structures

CODE = $(END) $(ZA) $(ZAS)


#-------------------------------------------------------------------------------
# Include file search path options
#-------------------------------------------------------------------------------

INC = -I. -I$(SUPPORT_INCDIR) $(APP_INCLUDES)


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

CFLAGS0 = $(OUT) $(DYNAPP) $(INC) $(CPU) $(APCS) $(CODE) $(CHK) $(DBG)
CFLAGS  = $(CFLAGS0) $(OPT)

#-------------------------------------------------------------------------------
# Linker flag definitions
#-------------------------------------------------------------------------------

LFLAGS = $(ROPILINK) -rwpi

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

SRC_CPP_FILE = $(@F:.o=.cpp)      # Input source file specification
SRC_C_FILE   = $(@F:.o=.c)      # Input source file specification
OBJ_FILE = $(OBJ_CMD) $(@F)   # Output object file specification

.SUFFIXES :
.SUFFIXES : .o .dep .c .cpp

#--------------------------------------------------------------------------
# C code inference rules
#----------------------------------------------------------------------------

.cpp.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(ARMCPP) $(CFLAGS) $(INC) $(OBJ_FILE) $(SRC_CPP_FILE)
        @echo ---------------------------------------------------------------


{$(SUPPORT_DIR)}.c.o:
        @echo ---------------------------------------------------------------
        @echo OBJECT $(@F)
        $(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE) $(SUPPORT_DIR)\$(SRC_C_FILE)
        @echo ---------------------------------------------------------------


#===============================================================================
#                           MODULE SPECIFIC RULES
#===============================================================================

APP_OBJS =  $(OBJS)

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


FirstWindow.o:	FirstWindow.cpp
FirstWindow.o:	fk.h
FirstWindow.o:	Commondef.h
FirstWindow.o:	fk_res.h
FirstWindow.o:	fk.bid
FirstWindow.o:	GAME.h
FirstWindow.o:	STATE.h
FirstWindow.o:	helpfunc.h
FirstWindow.o:	FirstWindow.h
fk.o:	fk.cpp
fk.o:	fk.h
fk.o:	Commondef.h
fk.o:	fk_res.h
fk.o:	fk.bid
fk.o:	GAME.h
fk.o:	STATE.h
fk.o:	helpfunc.h
fk.o:	FirstWindow.h
GAME.o:	GAME.cpp
GAME.o:	fk.h
GAME.o:	Commondef.h
GAME.o:	fk_res.h
GAME.o:	fk.bid
GAME.o:	GAME.h
GAME.o:	STATE.h
GAME.o:	helpfunc.h
GAME.o:	FirstWindow.h
helpfunc.o:	helpfunc.cpp
helpfunc.o:	fk.h
helpfunc.o:	Commondef.h
helpfunc.o:	fk_res.h
helpfunc.o:	fk.bid
helpfunc.o:	GAME.h
helpfunc.o:	STATE.h
helpfunc.o:	helpfunc.h
helpfunc.o:	FirstWindow.h
STATE.o:	STATE.cpp
STATE.o:	fk.h
STATE.o:	Commondef.h
STATE.o:	fk_res.h
STATE.o:	fk.bid
STATE.o:	GAME.h
STATE.o:	STATE.h
STATE.o:	helpfunc.h
STATE.o:	FirstWindow.h
