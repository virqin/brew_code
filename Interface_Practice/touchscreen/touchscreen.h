/******************************************************************************
FILE:  isms.h

SERVICES:  Header file

GENERAL DESCRIPTION:
  isms demo

PUBLIC CLASSES AND STATIC FUNCTIONS:
  Classes:

INITIALIZATION & SEQUENCING REQUIREMENTS:

  See Exported Routines

       Copyright © 2008 unicom-brew
               All Rights Reserved.
******************************************************************************/
#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/
// IFile app struct. This is the main struct for this applet. This will hold 
// all the data members that need to be remembered throughout the life of
// the applet. 
// THE FIRST DATA MEMBER OF THIS STRUCT MUST BE AN AEEApplet OBJECT. 
// This is critical requirement which is related to an applet being
// dynamically loaded. The app developers should add their data members
// following the AEEApplet data member.
typedef struct _TouchScreenApp {
   AEEApplet        a;
   IMenuCtl *  m_pIMenu;
   AEERect     m_rc;
   int m_cLineNum; // screen line to be written to next
   int m_cMaxLine; // max line number of screen on which output can be written
   int m_state;
} TouchScreenApp;

#define STATE_IDLE 0
#define STATE_SHOW_POINT 1
#define STATE_MENU 2

#endif // TOUCHSCREEN_H


