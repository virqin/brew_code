SAMPLEPOSDET
------------

This project contains a sample BREW(tm) application to demonstrate the use of the
IPosDet Interface.

SPDUserGuide.pdf : A brief user guide showing the different options available in this application.
SAMPLEPOSDET.BID : Contains the class ID associated with this application.
sampleposdet.mif : Is the MIF for the project.
sampleposdet.mod : Is the MOD for the project. Two MOD files are provided, one for BREW 2.0
                   devices and one for BREW 2.1 devices (ARM7).
sampleposdet.bri : Contains the resource data used by the application.
readme.txt       : Contains brief description of the project.
makefile.mak     : The make file to compile for the target platform.
SampleGPS.dsp    : Visual C++ project to build the application for BREW(tm) SDK.
Sampleposdet.c   : The entry functions to the project. This file also contain
                   few useful helper routines for managing the screens, drawing to the display.
Sampleposdet.h   : external functions of sampleposdet.c
SP_MainMenu.c    : The implementation of main menu screen used in the application.
SP_GetGPSInfo.c  : Displays the results of Position Determination.
SP_Track.c       : A sample implementation used in the application that repeatedly
                   invokes IPosDet when multiple position request is made.
SP_Track.h       : Exported routines of SP_Track.c
SP_ConfigMenu.c	 : The implementation of the config menu screens used in the application

NOTE: The BREWVersion.h file used in source code is only available in the BREW 2.1 SDK onwards. 
----  Developers compiling this application with the BREW2.0 SDK will have to create their own
      BREWVersion.h file (copy the 2.1 BREWVersion.h and make some minor version number changes).


NEW IN VERSION 1.1
------------------

SamplePosDet version 1.1 allows for user-configurable PDE server IP-address and port number, QoS
settings, and optimization mode.  It also fixes a bug which prevented the application for working
on BREW 2.1.3+ devices.