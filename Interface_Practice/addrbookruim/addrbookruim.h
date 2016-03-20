/******************************************************************************
FILE:  addrbookruim.h

SERVICES:  Header file

GENERAL DESCRIPTION:
  addrbookruim is address book on ruim card

PUBLIC CLASSES AND STATIC FUNCTIONS:
  Classes:
    IMenuCtl
    ITextCtl

INITIALIZATION & SEQUENCING REQUIREMENTS:

  See Exported Routines

       Copyright © 2008 unicom-brew
               All Rights Reserved.
******************************************************************************/
#ifndef ADDRBOOKRUIM_H
#define ADDRBOOKRUIM_H

//*****************************************************************************
// INCLUDE FILES
//*****************************************************************************
#include "AEE.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEEShell.h"
#include "AEEText.h"

//*****************************************************************************
// DEFINITIONS
//*****************************************************************************

#define MAX_RES_STRING_BUF_SIZE 60
#define MAX_CONTACT_NAME_SIZE   30
#define MAX_CONTACT_NUM_SIZE    20

#define ABR_REC_LIST_ID 1000

#define RESULT_DISPLAY_TIMER 2000
//*****************************************************************************
// ENUMERATED TYPE DEFINITIONS
//*****************************************************************************

// Enumerate the various application states

typedef enum
{
  APP_STATE_MAIN,
  APP_STATE_ADD,
  APP_STATE_VIEW,
  APP_STATE_EDIT,
  APP_STATE_TOTAL_REC,
  APP_STATE_SWITCH_STORE_DEV,
  APP_STATE_MAX
} EAppStateType;

typedef enum
{
  IMT_NUMBER,
  IMT_LITTLE_CASE_LETTER,
  IMT_BIG_CASE_LETTER,
  IMT_RAPID,
  IMT_MAX
} EInputModeType;


/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _CAddrBookRuim {
    AEEApplet      a ;         // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface

    // add your own variables here...
  	EAppStateType           m_eAppState;    // Application State
  	AEERect                 m_rScreenRect;  // Screen Rect
  	int                     m_nFontHeight;  // Stores the height of the AEE_FONT_BOLD font.  This will

    IMenuCtl*               m_pMenu;        // Main Menu
    IMenuCtl*               m_pSK;          // Softkey Menu
    ITextCtl*               m_pText1;       // Text Control
    ITextCtl*               m_pText2;       // Text Control
    EInputModeType     m_nInputMode;       // input mode
    boolean isRuimContact;        // otherwise is handset address book
    uint16 m_wSelRecId;
    AEECLSID m_ContactCLS;
} CAddrBookRuim;

#endif // ADDRBOOKRUIM_H
