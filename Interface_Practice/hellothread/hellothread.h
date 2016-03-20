/******************************************************************************
FILE:  hellothread.h

SERVICES:  Header file

GENERAL DESCRIPTION:
  hellothread demo

PUBLIC CLASSES AND STATIC FUNCTIONS:
  Classes:

INITIALIZATION & SEQUENCING REQUIREMENTS:

  See Exported Routines

       Copyright © 2008 unicom-brew
               All Rights Reserved.
******************************************************************************/
#ifndef HELLOTHREAD_H
#define HELLOTHREAD_H

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

typedef struct _SThreadCtx
{
  IThread  *pit;
  int n;
  IDisplay *pid;
  IShell *pis;
  IGraphics *pig;
} SThreadCtx;

typedef struct _HelloThreadApp
{
  AEEApplet a;
  SThreadCtx stc1, stc2;
  AEECallback cbThread2Done;
  int nThread2Result;

} HelloThreadApp;



#endif // HELLOTHREAD_H


