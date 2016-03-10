#============================================================================
#  Name:
#    etclicore.MAK
#
#  Description:
#    Makefile to build the etclicore downloadable module.
#
#   The following nmake targets are available in this makefile:
#
#     all           - make .elf and .mod image files (default)
#     clean         - delete object directory and image files
#     filename.o    - make object file
#     filename.mix -  mixes assembly and source
#
#   The above targets can be made with the following command:
#
#     nmake /f etclicore.mak [target]
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
#        Copyright � 2000-2002 QUALCOMM Incorporated.
#               All Rights Reserved.
#            QUALCOMM Proprietary/GTDR
#
#----------------------------------------------------------------------------
#============================================================================

#-------------------------------------------------------------------------------
# Target file name and type definitions
#-------------------------------------------------------------------------------

SDKHOME	= $(BREWDIR)\sdk

TARGET     	= sampleposdet#           		# Target name for output files and object dir
OBJSDIR		= objs
TARGETDIR	= sampleposdet				# Target dir

#-------------------------------------------------------------------------------
# Local compile option
#-------------------------------------------------------------------------------
MYOPT = -Ec

#-------------------------------------------------------------------------------
# Local define option
#-------------------------------------------------------------------------------
MYDEFINES = #-DHAL_BREW  -DADS

#-------------------------------------------------------------------------------
# Local libs
#-------------------------------------------------------------------------------
MYLIBS = #.\echat\lib\echat.a

#------------------------------------------------------------------------------
# Application objects
#------------------------------------------------------------------------------

LOCAL_OBJS =$(OBJSDIR)\AEEAppGen.o\
			$(OBJSDIR)\AEEModGen.o\
			$(OBJSDIR)\sampleposdet.o\
			$(OBJSDIR)\SP_MainMenu.o\
			$(OBJSDIR)\SP_GetGPSInfo.o\
			$(OBJSDIR)\SP_Track.o\
			$(OBJSDIR)\SP_ConfigMenu.o\
			$(OBJSDIR)\e_asin.o\
			$(OBJSDIR)\transform.o

#-------------------------------------------------------------------------------
# Local include option
#-------------------------------------------------------------------------------
MYINC =		-I $(BREWDIR)\inc \
			-I $(BREWDIR)\sdk\inc \
			-I .
						
#------------------------------------------------------------------------------
# Common make file
#------------------------------------------------------------------------------

!INCLUDE "common.min"


#----------------------------------------------------------------------------
# Applet Specific Rules
#----------------------------------------------------------------------------

RULE1 = .
{$(RULE1)}.c.o:
	@echo ---------------------------------------------------------------
	@echo OBJECT $(@F)
	$(ARMCC) $(CFLAGS) $(INC) $(OBJ_FILE)  $(RULE1)\$(SRC_FILE)
	@echo ---------------------------------------------------------------

RULE2 = .
.brx.bar:
	@echo ---------------------------------------------------------------
	@echo OBJECT $@
	$(BREW_RES) -s -o $@ -h $(TARGET)_res.h $< 
	@echo ---------------------------------------------------------------
	
#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------
$(OBJSDIR)\$(TARGET).o : $(TARGET).c
$(OBJSDIR)\$(TARGET).o : $(TARGET).h


