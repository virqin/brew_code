/******************************************************************************
FILE:  calldemo.h

SERVICES:  Header file

GENERAL DESCRIPTION:
  calldemo demo

PUBLIC CLASSES AND STATIC FUNCTIONS:
  Classes:

INITIALIZATION & SEQUENCING REQUIREMENTS:

  See Exported Routines

       Copyright © 2008 unicom-brew
               All Rights Reserved.
******************************************************************************/
#ifndef CALLDEMO_H
#define CALLDEMO_H

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
typedef struct _CallDemoApp {
   AEEApplet        a;
   IMenuCtl *  m_pIMenu;
   AEERect     m_rc;
   int m_cLineNum; // screen line to be written to next
   int m_cMaxLine; // max line number of screen on which output can be written

    ICallMgr *m_pCallMgr;
    ICall *m_pIncomingCall;
    ICall *m_pOutgoingCall;
    ICall *m_pOutgoingCallB;
    CallListener callListener;
    CallListener callListenerB;
    ITelephone *m_pTelephone;
    IModel     *m_pModel;
    PhoneListener   m_lisPhone;           // Listener on Telephone.
#ifdef EVENT_AUTO_GET
    AEEEvent m_UserAnswerEvent;
    AEEEvent m_UserRedirectEvent;
#endif
    boolean m_isDTMFItem;
    boolean m_isParty3Item;
    boolean m_isRedirectItem;
} CallDemoApp;

#endif // CALLDEMO_H


