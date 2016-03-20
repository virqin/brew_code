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
#    1. nmake requires you to have GCC cross compiler installed on your machine.
#    2. cygwin(to be precise cygwin1.dll) is installed on your machine and 
#       is in the path.  
#    3. GCCHOME environment variable points to location of GCC cross compiler
#    4. GCCLIBPATH environment variable points to location of libgcc.a
#    5. BREWADDINS environment variable points to location of BREW Addins
#    6. BREWDIR environment variable points to location of BREW SDK
#    7. The PATH environment variable contains location of GCC (GCCHOME), (GCCHOME)\arm-elf\bin,
#       and (GCCHOME)\lib\gcc-lib\arm-elf\2.95.3 or whichever version of GCC is installed.
#
#  Notes:
#    1. While building the application using this make file, the following warnings may be 
#       received. They can be safely ignored:
#		"Warning: _divsi3.o does not support interworking, whereas <Your App>.elf does not"
#		"Warning: _dvmd_tls.o does not support interworking, whereas <Your App>.elf does not"
#		"Warning: cannot find entry symbol _start; defaulting to 00008000"
#  
#============================================================================
#============================================================================
BREW_HOME      = $(BREWDIR)
BREW_ADDINS    = $(BREWADDINS)
GCC_HOME       = $(GCCHOME)
GCC_LIBPATH    = $(GCCLIBPATH)
TARGET         = HelloWorld
OBJS           = AEEMOD~1.o AEEAPP~1.o HELLOW~1.o GCCResolver.o 
APP_INCLUDES   = -I "$(BREWDIR)\inc"-I $(BREWDIR)\inc 

#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

EXETYPE	=	elf                # Target image file format
BINARY  =	bin
MODULE	=	mod                # Downloadable module extension

#-----------------------------------------------------------------------
# Target compile time symbol definitions
#
# Tells the SDK source stuffs that we're building a dynamic app.
#-----------------------------------------------------------------------

DYNAPP = 	-DDYNAMIC_APP

#-----------------------------------------------------------------------
# Software tool and environment definitions
#   (PREFIX needs to be set to the location of your gcc cross-compiler
#    and binutils.  STUBTOOL needs to be the path and filename of your
#    copy of makestub)
#-----------------------------------------------------------------------

AEESRCPATH = $(BREW_HOME)\src
AEEINCPATH = $(BREW_HOME)\inc
GCC		   = $(GCC_HOME)\bin\arm-elf-gcc
LD		   = $(GCC_HOME)\bin\arm-elf-ld
DUMPTOOL   = $(GCC_HOME)\bin\arm-elf-objdump
ELF2MODTOOLPATH = $(BREW_ADDINS)\common\bin
ELF2MODTOOL	=	$(ELF2MODTOOLPATH)\BREWelf2mod.exe
GCCRESOLVEPATH = $(BREW_ADDINS)\common\templates\src
#-----------------------------------------------------------------------
# Compiler optimization options
#   -O0 disables compiler optimizations.  Other options probably work as
#     well.  Set to taste.
#-----------------------------------------------------------------------
OPT	=	-O2

#-----------------------------------------------------------------------
# Compiler code generation options
#   Add $(TARG) to the CODE line if you're building a Thumb binary (at
#   the moment, this doesn't work).
#-----------------------------------------------------------------------
END		=	-mlittle-endian
TARG	=	-mthumb
CODE	=	$(END) -fshort-enums -fno-builtin

#-----------------------------------------------------------------------
# Include file search path options
#   (change this to point to where the BREW SDK headers are located)
#-----------------------------------------------------------------------
INC	=	-I$(AEEINCPATH) -I$(GCC_LIBPATH)\include -I$(GCC_HOME)\arm-elf\include 

#-----------------------------------------------------------------------
# Library search path options
#   (You can change this to point to where your interworking version of 
#    libgcc.a resides)
#-----------------------------------------------------------------------
LIBDIRS = -L$(GCC_LIBPATH)

#-----------------------------------------------------------------------
# Nothing below here (except for the dependencies at the bottom of the
#  file) should need to be changed for a reasonably normal compilation.
#-----------------------------------------------------------------------

#-----------------------------------------------------------------------
# Processor architecture options
# Sets the designated target processor for this applet.
# Currently, all BREW phones use the ARM 7t chip
#-----------------------------------------------------------------------
CPU	=	-mcpu=arm7tdmi

#-----------------------------------------------------------------------
# ARM Procedure Call Standard (APCS) options
# -fPIC             sets posititon independent code.  Other option: -fpic
# -mthumb-interwork enables switching between ARM and Thumb code
# -mapcs-frame      runs on systems with the frame ptr. specified in the
# 			APCS
#-----------------------------------------------------------------------
ROPI	=	
INTRWK	=	-mthumb-interwork
APCS	=	-mapcs-frame $(ROPI) $(INTRWK)

#-----------------------------------------------------------------------
# Compiler output options
# -c sets object file output only
#-----------------------------------------------------------------------
OUT	=	-c

#-----------------------------------------------------------------------
# Compiler/assembler debug Options
#   -g is the standard flag to leave debugging information in the
#   object files.
#-----------------------------------------------------------------------
DBG	=	-g

#-----------------------------------------------------------------------
# Linker Options
# -o sets the output filename
#-----------------------------------------------------------------------
LINK_CMD  = -Ttext 0 --emit-relocs -entry AEEMod_Load -o
#LIBS		=	-lgcc

#-----------------------------------------------------------------------
# HEXTOOL Options
#-----------------------------------------------------------------------
BINFORMAT = -O binary

#-----------------------------------------------------------------------
# Compiler flag definitions
#-----------------------------------------------------------------------
CFLAGS0 =	$(OUT) $(DYNAPP) $(CPU) $(APCS) $(CODE) $(DBG)
CFLAGS =	$(CFLAGS0) $(INC) $(OPT) 

#-----------------------------------------------------------------------
# Linker flag definitions
#-----------------------------------------------------------------------
LDFLAGS =	$(LIBDIRS)

#-----------------------------------------------------------------------
# Default target
#-----------------------------------------------------------------------
default: $(TARGET).$(MODULE)

#-----------------------------------------------------------------------
# All target
#-----------------------------------------------------------------------
all: $(TARGET).$(MODULE)


#-----------------------------------------------------------------------
# C Code inference rules (plus a few others, for debugging purposes)
#   "make foo.s" will compile foo.c into assembly code.
#   "make foo.sp" will produce a foo.s file with the information on
#     which pattern in the RTL was matched to produce each assembly
#     instruction 
#   "make foo.dump" will make foo.o if necessary and then use objdump -D
#     on it, piping the results into foo.dump.  This is slightly
#     different from the output of "make foo.s", in general.
#-----------------------------------------------------------------------
%.o: %.c
	$(GCC) $(CFLAGS) -o $@ $< 
%.s: %.c
	$(GCC) $(CFLAGS) -S $< 
%.sp: %.c
	$(GCC) $(CFLAGS) -S -dp $<
%.dump: %.o
	$(DUMPTOOL) -D $< > $@

OBJ_CMD    = -o               # Command line option to specify output filename
SRC_FILE = $(@F:.o=.c)        # Input source file specification
OBJ_FILE = $(OBJ_CMD) $(@F)   # Output object file specification

.SUFFIXES :
.SUFFIXES : .o .dep .c

.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(GCC) $(CFLAGS) $(OBJ_FILE) $(SRC_FILE)
	@echo ---------------------------------------------------------------

.c.mix:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(GCC) -S -fs $(CFLAGS) $(INC) $(OBJ_FILE) $<
	@echo ---------------------------------------------------------------

{$(AEESRCPATH)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(GCC) $(CFLAGS) $(INC) $(OBJ_FILE) $(AEESRCPATH)\$(SRC_FILE)
	@echo ---------------------------------------------------------------

{$(GCCRESOLVEPATH)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(GCC) $(CFLAGS) $(INC) $(OBJ_FILE) $(GCCRESOLVEPATH)\$(SRC_FILE)
	@echo ---------------------------------------------------------------

#===============================================================================
#                           MODULE SPECIFIC RULES
#===============================================================================

APP_OBJS = $(OBJS) 

#-----------------------------------------------------------------------
# Targets for making the actual binary
#-----------------------------------------------------------------------
$(TARGET).$(MODULE) :   $(TARGET).$(EXETYPE)
	$(ELF2MODTOOL) $(TARGET).$(EXETYPE) $(TARGET).$(MODULE)

$(TARGET).$(EXETYPE) :  $(APP_OBJS)
	$(LD) $(LINK_CMD) $(TARGET).$(EXETYPE) $(LDFLAGS) \
	$(APP_OBJS) $(LIBS) $(LINK_ORDER)

#-----------------------------------------------------------------------
# Cleanup 
#-----------------------------------------------------------------------
clean:
	rm -f $(APP_OBJS) $(TARGET).$(EXETYPE) $(TARGET).$(MODULE)		

#-----------------------------------------------------------------------
# Object File Dependencies
#   You may well want to add more dependencies here.
#-----------------------------------------------------------------------

#----------------------------------------------------------------------------
# Applet Specific Rules
#----------------------------------------------------------------------------


RULE1 = c:\PROGRA~1\BREW31~1.5\sdk\src
{$(RULE1)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(GCC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE1)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


RULE2 = c:\BREWAP~1\HELLOW~1
{$(RULE2)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(GCC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE2)\$(SRC_FILE)
	@echo ---------------------------------------------------------------


# --------------------------------------------
# DEPENDENCY LIST, DO NOT EDIT BELOW THIS LINE
# --------------------------------------------



GCCResolver.o:	$(BREWADDINS)\common\templates\src\GCCResolver.c 
