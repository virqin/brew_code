/******************************************************************************
FILE:  rscpooldemo.h

SERVICES:  Header file

GENERAL DESCRIPTION:
  rscpooldemo demo

PUBLIC CLASSES AND STATIC FUNCTIONS:
  Classes:

INITIALIZATION & SEQUENCING REQUIREMENTS:

  See Exported Routines

       Copyright © 2008 unicom-brew
               All Rights Reserved.
******************************************************************************/
#ifndef RSCPOOLDEMO_H
#define RSCPOOLDEMO_H

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
typedef struct _RscPoolDemoApp {
   AEEApplet        a;
   IMenuCtl *  m_pIMenu;
   AEERect     m_rc;
   int m_cLineNum; // screen line to be written to next
   int m_cMaxLine; // max line number of screen on which output can be written

   IRscPool *m_pIRscPool;
} RscPoolDemoApp;

#endif // RSCPOOLDEMO_H


