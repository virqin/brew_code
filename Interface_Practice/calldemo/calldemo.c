/*===========================================================================

FILE: calldemo.c

SERVICES: Sample applet using AEE

DESCRIPTION
  This file contains usage examples of following interfaces. 
  AEECLSID_RANDOM
  ISOURCE

PUBLIC CLASSES:  
   N/A

       Copyright © 2000-2008 Unicom-brew.
                    All Rights Reserved.
                   Unicom-brew
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
// Mandatory includes
#include "AEEAppGen.h"  // AEEApplet declaration
#include "AEE.h"      // Standard AEE Declarations
#include "AEEShell.h" // AEE Shell Services
#include "AEEStdlib.h"    // AEE Display Services
#include "AEEDisp.h"    // AEE Display Services
#include "AEEMenu.h"    // AEE Display Services
#include "AEETelephone.h" 

// Services used by app
#include "calldemo.bid"
#include "calldemo.brh"
#include "calldemo.h"

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/
// Mandatory function:
// App Handle Event function
static boolean CallDemoApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// App specific data alloc-init/free functions
static boolean CallDemoApp_InitAppData(IApplet* pMe);
static void CallDemoApp_FreeAppData(IApplet* pMe);

static void BuildMainMenu(CallDemoApp *pMe);
static void CallDemoAppUsage (CallDemoApp *pMe, uint16 wParam);
static void DisplayEvent (CallDemoApp *pMe, uint16 wParam);
static int DisplayOutput(IShell *pIShell, IDisplay *pIDisplay, int nline, char *pszStr, AECHAR *pszwStr, boolean isWideChar);
static void WriteLine (CallDemoApp *pMe, char *pszStr, AECHAR *pszwStr, boolean isWideChar);
static void OriginateListener(CallDemoApp *pMe, ModelEvent *pEvent);
static void MyPhoneListener(CallDemoApp *pMe, ModelEvent *pEvent);
static int Telephone_EndCall(CallDemoApp* pMe);

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/

// Size of buffer used to hold text strings for display
#define TEXT_BUFFER_SIZE (100 * sizeof(AECHAR))

// App specific constants

#define USAGE_CALL_ORIG         100
#define USAGE_CALL_OUTGOING_END         101
#define USAGE_CALL_INCOMING_END         102
#define USAGE_CALL_DTMF         103
#define USAGE_CALL_PARTY3      104
#define USAGE_CALL_ANSWER       105
#define USAGE_CALL_REDIRECT      106

#define USAGE_ERASE_SCREEN      119   // Special case to erase screen

#ifndef EVENT_AUTO_GET
#define EVT_MY_ANSWER  (EVT_USER+1)
#define EVT_MY_REDIRECT  (EVT_USER+2)
#endif
/*===============================================================================
                     FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
  This function is invoked while the app is being loaded. All Modules must provide this 
  function. Ensure to retain the same name and parameters for this function.
  In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
  that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IFile_InitAppData(). This is done as follows:
   IFile_InitAppData() is called to initialize the applet's data. It is app developer's 
   responsibility to fill-in app data initialization code of IFile_InitAppData(). App developer
   is also responsible to release memory allocated for data -- this can be
   done in IFile_FreeAppData().

PROTOTYPE:
  boolean AEEClsCreateInstance(AEECLSID ClsId, IShell* pIShell, IModule* pIModule, 
                      void** ppObj)

PARAMETERS:
  ClsId: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell interface. 

  pIModule: [in]: Contains pointer to the IModule interface to the current module to which
  this app belongs

  ppObj: [out]: On return, *ppObj must point to a valid AEEApplet structure. Allocation
  of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app needs to be loaded and if AEEApplet_New() invocation was successful
  FALSE: If the app does not need to be loaded or if errors occurred in AEEApplet_New().
  If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell* pIShell, IModule* po, void** ppObj)
{
    *ppObj = NULL;
    
    // We want to load this App. So, invoke AEEApplet_New().To it, pass the
    // address of the app-specific handle event function.
    if(ClsId == AEECLSID_CALLDEMO)
    {
        if (AEEApplet_New(sizeof(CallDemoApp), ClsId, pIShell,po,(IApplet**)ppObj,
            (AEEHANDLER)CallDemoApp_HandleEvent,(PFNFREEAPPDATA)CallDemoApp_FreeAppData) == TRUE)
        {
            if (CallDemoApp_InitAppData((IApplet*)*ppObj))
            {
                return(AEE_SUCCESS);
            }
        }
    }
    return (EFAILED);
}

/*===========================================================================

FUNCTION CallDemoApp_HandleEvent

DESCRIPTION
  This is the EventHandler for this app. All events to this app are handled in this
  function.

PROTOTYPE:
  static boolean App_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
  pi: Pointer to the AEEApplet structure. This structure contains information specific
  to this applet.

  ecode: Specifies the Event sent to this applet

    wParam, dwParam: Event-specific data.

DEPENDENCIES
    none

RETURN VALUE
    TRUE: If the app has processed the event
    FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/

static boolean CallDemoApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    CallDemoApp * pMe = (CallDemoApp*)pi;

    switch (eCode) 
    {
        case EVT_APP_START:
            // When this applet is started, try to create the main menu
            // that allows the user to select a usage example to execute.
            // If we cannot create an instance of the menu class, exit and
            // indicate that we have handled the event by returning TRUE
            if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MENUCTL, 
                                                (void **)&pMe->m_pIMenu) != SUCCESS)
                return TRUE;

            // Succeeded in obtaining a menu instance pointer, so
            // construct the menu and display it on the screen
            BuildMainMenu(pMe);
            return(TRUE);

        case EVT_APP_STOP:
            return(TRUE);

        case EVT_KEY: 
            if(pMe->m_isDTMFItem)
            {
                if (wParam == AVK_2)
                {
                    AECHAR szwStr[32];
                    char szNumber[32];
                    int nRet;

                    // get telephone number (main) from resource file
                    ISHELL_LoadResString(pMe->a.m_pIShell, CALLDEMO_RES_FILE, IDS_STRING_TELEPHONE_NUMBER_SUB, szwStr, sizeof(szwStr));
                    WSTRTOSTR(szwStr, szNumber, sizeof(szNumber));

                    nRet = ICALL_BurstDTMF(pMe->m_pOutgoingCall,  AEET_DTMF_ON_95,AEET_DTMF_OFF_60,szNumber);
                    DBGPRINTF("ICALL_BurstDTMF ret %d", nRet);
                    WriteLine(pMe, "ICALL_BurstDTMF -->", NULL, FALSE);    
                }
            }


            if (pMe->m_isParty3Item)
            {
                if (wParam == AVK_3)
                {
                   AECHAR szwStr[32];
                   char szNumber[32];
                   boolean Result;

                   // get telephone number (main) from resource file
                   ISHELL_LoadResString(pMe->a.m_pIShell, CALLDEMO_RES_FILE, IDS_STRING_TELEPHONE_NUMBER_B, szwStr, sizeof(szwStr));
                   WSTRTOSTR(szwStr, szNumber, sizeof(szNumber));

                          
                  LISTENER_Init(&pMe->callListenerB, (PFNLISTENER) OriginateListener, pMe);
                  Result = ICALLMGR_Originate(pMe->m_pCallMgr, AEET_CALL_TYPE_VOICE, szNumber, NULL, &pMe->m_pOutgoingCallB, &pMe->callListenerB);
                  if (Result != SUCCESS)
                  {
                     DBGPRINTF("ICALLMGR_Originate failed with res=%d", Result);
                     break;
                  }
                  WriteLine(pMe, "Call Originate B-->", NULL, FALSE);  
                  WriteLine(pMe, "Press 1 to unhold A", NULL, FALSE);  
                  return TRUE;
                }

                if (wParam == AVK_1)
                {
                  int nRet;
                  if(pMe->m_pOutgoingCall)
                  {
                    nRet = ICALL_SwitchTo( pMe->m_pOutgoingCall );
                    DBGPRINTF("ICALL_SwitchTo A ret:%d", nRet);
                  }
#if 0
                  if(pMe->m_pOutgoingCallB)
                  {
                    nRet = ICALL_Hold( pMe->m_pOutgoingCallB );
                    DBGPRINTF("ICALL_Hold B ret:%d", nRet);
                  }
#endif                  
                  WriteLine(pMe, "switch issued", NULL, FALSE);  
                  return TRUE;
                }

#if 0
                if (wParam == AVK_2)
                {
                  int nRet;
                  if(pMe->m_pOutgoingCallB)
                  {
                    nRet = ICALL_SwitchTo( pMe->m_pOutgoingCallB );
                     DBGPRINTF("ICALL_SwitchTo B ret:%d", nRet);
                  }
                  
                  if(pMe->m_pOutgoingCall)
                  {
                    nRet = ICALL_Hold( pMe->m_pOutgoingCall );
                    DBGPRINTF("ICALL_Hold A ret:%d", nRet);
                  }
                  WriteLine(pMe, "switch issued", NULL, FALSE);  
                  return TRUE;
                }
#endif                  
            }

            // ¿ØÖÆÉÏÏÂ¼üÑ¡Ôñ²Ëµ¥
            if (pMe->m_pIMenu)
            {
                if ((IMENUCTL_IsActive(pMe->m_pIMenu) == FALSE) &&
                   ((wParam == AVK_UP) || (wParam == AVK_DOWN)))
                {
                    IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
                    IMENUCTL_Redraw(pMe->m_pIMenu);
                }

                return IMENUCTL_HandleEvent(pMe->m_pIMenu, EVT_KEY, wParam, 0);
            }
            else
                return FALSE;


        case EVT_NOTIFY:
            break;
            
        case EVT_COMMAND:
            switch(wParam)
            {
                // The following commands are generated by user selections
                // from the main usage app menu.
                
                case USAGE_CALL_ORIG:
                case USAGE_CALL_OUTGOING_END:
                case USAGE_CALL_INCOMING_END:
                case USAGE_CALL_DTMF:
                case USAGE_CALL_PARTY3:
                case USAGE_CALL_ANSWER:
                case USAGE_CALL_REDIRECT:
                    // Deactivate main menu to turn off horizontal scrolling of long menu items
                    IMENUCTL_SetActive(pMe->m_pIMenu, FALSE);

                    // Execute the usage example the user has selected
                    CallDemoAppUsage (pMe, wParam);
                    return TRUE;
             
                default:
                    return FALSE;
            } // switch(wPara)

        case EVT_MY_ANSWER:
        DBGPRINTF("handler of call incoming by answer");
        if (pMe->m_pIncomingCall)
        { 
            int nError;
            nError = ICALL_Answer(pMe->m_pIncomingCall);
            if (SUCCESS != nError)
            {
                DBGPRINTF("ICALL_Answer failed ret=%d", nError);
            }
            else
            {
                DBGPRINTF("ICALL_Answer -->");
                WriteLine(pMe, "ICALL_Answer -->", NULL, FALSE);          
            }
        }
                    return TRUE;

        case EVT_MY_REDIRECT:
        DBGPRINTF("handler of call incoming by redirect");
        if (pMe->m_pIncomingCall)
        { 
            int nError;
            AECHAR szwStr[32];
            char szNumber[32];

            // get redirect number
            ISHELL_LoadResString(pMe->a.m_pIShell, CALLDEMO_RES_FILE, IDS_STRING_TELEPHONE_NUMBER_REDIRECT, szwStr, sizeof(szwStr));
            WSTRTOSTR(szwStr, szNumber, sizeof(szNumber));

            nError = ICALL_Redirect(pMe->m_pIncomingCall, szNumber);
            if (SUCCESS != nError)
            {
                DBGPRINTF("ICALL_Redirect failed ret=%d", nError);
            }
            else
            {
                DBGPRINTF("ICALL_Redirect -->%s", szNumber);
                WriteLine(pMe, "ICALL_Redirect -->", NULL, FALSE);          
            }
        }
        return TRUE;

        default:
            break;
   } //  switch (eCode) 

#ifdef EVENT_AUTO_GET
   if(eCode == pMe->m_UserAnswerEvent)
   {
        DBGPRINTF("handler of call incoming by answer");
        if (pMe->m_pIncomingCall)
        { 
            int nError;
            nError = ICALL_Answer(pMe->m_pIncomingCall);
            if (SUCCESS != nError)
            {
                DBGPRINTF("ICALL_Answer failed ret=%d", nError);
            }
            else
            {
                DBGPRINTF("ICALL_Answer -->");
                WriteLine(pMe, "ICALL_Answer -->", NULL, FALSE);          
            }
        }
   }
   
   if(eCode == pMe->m_UserRedirectEvent)
   {
        DBGPRINTF("handler of call incoming by redirect");
        if (pMe->m_pIncomingCall)
        { 
            int nError;
            AECHAR szwStr[32];
            char szNumber[32];

            // get redirect number
            ISHELL_LoadResString(pMe->a.m_pIShell, CALLDEMO_RES_FILE, IDS_STRING_TELEPHONE_NUMBER_REDIRECT, szwStr, sizeof(szwStr));
            WSTRTOSTR(szwStr, szNumber, sizeof(szNumber));

            nError = ICALL_Redirect(pMe->m_pIncomingCall, szNumber);
            if (SUCCESS != nError)
            {
                DBGPRINTF("ICALL_Redirect failed ret=%d", nError);
            }
            else
            {
                DBGPRINTF("ICALL_Redirect -->%s", szNumber);
                WriteLine(pMe, "ICALL_Redirect -->", NULL, FALSE);          
            }
        }
   }
#endif    
   return FALSE;
}


/*===========================================================================

FUNCTION CallDemoApp_InitAppData

DESCRIPTION
   This function initializes app specific data.

PROTOTYPE:
  static boolean CallDemoApp_InitAppData(IApplet* pi);

PARAMETERS:
  pi [in]: Pointer to the IApplet structure.

DEPENDENCIES
    None.

RETURN VALUE
    TRUE: If the app has app data is allocated and initialized successfully
    FALSE: Either app data could not be allocated or initialized

SIDE EFFECTS
    None.
===========================================================================*/
static boolean CallDemoApp_InitAppData(IApplet* pi)
{
   CallDemoApp * pMe = (CallDemoApp*)pi;
   int charHeight, pnAscent, pnDescent;
   AEEDeviceInfo di;
   int nErr;

   // Make sure the pointers we'll be using are valid
   if (pMe == NULL || pMe->a.m_pIShell == NULL)
     return FALSE;

   pMe->m_pIMenu = NULL;

   // Determine the amount of available screen space
   ZEROAT(&di);
   di.wStructSize = sizeof(di);
   ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);

   pMe->m_rc.dx = di.cxScreen;
   pMe->m_rc.dy = di.cyScreen;
   // Determine the height of a line of text
   charHeight = IDISPLAY_GetFontMetrics (pMe->a.m_pIDisplay, AEE_FONT_NORMAL,
      &pnAscent, &pnDescent);

   // Number of available lines equals the available screen
   // space divided by the height of a line, minus 3 for the
   // lines we always print at the top and bottom of the screen
   pMe->m_cMaxLine = (di.cyScreen / charHeight) - 3;

   pMe->m_pCallMgr = NULL;
   pMe->m_pOutgoingCall = NULL;
   pMe->m_pIncomingCall = NULL;
   nErr =ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_CALLMGR, (void**) &pMe->m_pCallMgr);
   DBGPRINTF("CreateInst AEECLSID_CALLMGR ret %d", nErr);
   if(nErr != AEE_SUCCESS)
   {
       return FALSE;
   }

   if(SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_TELEPHONE, (void **) &pMe->m_pTelephone)) 
   {
       return FALSE;
   }

   if(SUCCESS != ITELEPHONE_QueryInterface(pMe->m_pTelephone, AEEIID_MODEL, (void **) &pMe->m_pModel)) 
   {
       return FALSE;
   }

   LISTENER_Init((ModelListener *)&pMe->m_lisPhone, MyPhoneListener, (void *) pMe);
   IMODEL_AddListener(pMe->m_pModel, (ModelListener *) &pMe->m_lisPhone);

#ifdef EVENT_AUTO_GET
   pMe->m_UserAnswerEvent = ISHELL_RegisterEvent(pMe->a.m_pIShell, "user anwser event", NULL );
   pMe->m_UserRedirectEvent = ISHELL_RegisterEvent(pMe->a.m_pIShell, "user redirect event", NULL );
#endif

   pMe->m_isDTMFItem = FALSE;
   pMe->m_isParty3Item= FALSE;
   pMe->m_isRedirectItem = FALSE;

   return TRUE;
}

/*===========================================================================
   Function
   static void ReleaseIf(IBase **ppif)
  
   Description
   -----------
   releases the interface pointer, sets it to null
  
   Parameters
   ----------
   IBase **ppif: interfacee pointer
  
   Returns
   -------
   nothing
  
===========================================================================*/
static void ReleaseIf(IBase **ppif)
{
    if ((IBase *)0 != *ppif) 
    {
        IBASE_Release(*ppif);
        *ppif = (IBase *)0;
    }
}


/*===========================================================================

FUNCTION CallDemoApp_FreeAppData

DESCRIPTION
    Frees any data and interfaces used by this app.

PROTOTYPE:
  static void CallDemoApp_FreeAppData(IApplet* pi);

PARAMETERS:
  pi [in]: Pointer to the IApplet structure.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
static void CallDemoApp_FreeAppData(IApplet* pi)
{
   CallDemoApp * pMe = (CallDemoApp*)pi;

   ReleaseIf((IBase **)&pMe->m_pIMenu);
   ReleaseIf((IBase **)&pMe->m_pCallMgr);
   ReleaseIf((IBase **)&pMe->m_pModel);
   ReleaseIf((IBase **)&pMe->m_pTelephone);

   if (pMe->m_pOutgoingCall != NULL)
   {
      ICALL_End(pMe->m_pOutgoingCall);
      ICALL_Release(pMe->m_pOutgoingCall);
      pMe->m_pOutgoingCall = NULL;
   }

   if (pMe->m_pOutgoingCallB != NULL)
   {
      ICALL_End(pMe->m_pOutgoingCallB);
      ICALL_Release(pMe->m_pOutgoingCallB);
      pMe->m_pOutgoingCallB = NULL;
   }
}

/*===============================================================================
                     APP HELPER FUNCTION DEFINITIONS
=============================================================================== */
/*===========================================================================

FUNCTION: CallDemoAppUsage

DESCRIPTION
    This function executes the usage example selected from the main menu by the
  user.  It takes as input the app data pointer and the ID of the example the
  user selected.  After clearing the screen, the selected example is executed.
  Each example is a self-contained code block that contains any local variables
  needed to execute the example.  Each code block also creates and releases any
  BREW interfaces that are required, and will exit if any errors are detected.  
  Once the example has been executed, this function prints a heading at the top 
  of the screen to indicate which example was selected, 
  and a message at the bottom of the screen instructing the user how to return to
  the main menu to execute another example.
    
PROTOTYPE:
   static void IFileUsage(CallDemoApp * pMe, uint16 wParam)

PARAMETERS:
   pMe:   [in]: Contains a pointer to the usage app data structure.
   wParam: [in]: Identifier of the selected example (example IDs are specified when the
                 main usage app menu is created).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void CallDemoAppUsage (CallDemoApp * pMe, uint16 wParam)
{

    
   // Make sure the pointers we'll be using are valid
   if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->a.m_pIDisplay == NULL)
     return;

   // Erase screen first for display output purposes.
   DisplayEvent (pMe, USAGE_ERASE_SCREEN);
  IMENUCTL_SetProperties(pMe->m_pIMenu, MP_NO_REDRAW);
  IMENUCTL_SetActive(pMe->m_pIMenu, FALSE);

   // Initialize line number where we will start the output 
   // of each test
   pMe->m_cLineNum = 1;

   switch (wParam)
   {
        case USAGE_CALL_ORIG:
        {
           AECHAR szwStr[32];
           char szNumber[32];
           boolean Result;

           // get telephone number from resource file
           ISHELL_LoadResString(pMe->a.m_pIShell, CALLDEMO_RES_FILE, IDS_STRING_TELEPHONE_NUMBER, szwStr, sizeof(szwStr));
           WSTRTOSTR(szwStr, szNumber, sizeof(szNumber));

                  
          LISTENER_Init(&pMe->callListener, (PFNLISTENER) OriginateListener, pMe);
          Result = ICALLMGR_Originate(pMe->m_pCallMgr, AEET_CALL_TYPE_VOICE, szNumber, NULL, &pMe->m_pOutgoingCall, &pMe->callListener);
          if (Result != SUCCESS)
          {
             DBGPRINTF("ICALLMGR_Originate failed with res=%d", Result);
             break;
          }
          WriteLine(pMe, "Call Originate -->", NULL, FALSE);          
        }
        break;

        case USAGE_CALL_OUTGOING_END:
            {
                if (Telephone_EndCall(pMe) != SUCCESS) 
                {
                    if (pMe->m_pOutgoingCall) 
                    {
                        ICALL_Release(pMe->m_pOutgoingCall);
                        pMe->m_pOutgoingCall = NULL;
                    }
                }
                WriteLine(pMe, "Call End -->", NULL, FALSE);                
            }
            break;
            
        case USAGE_CALL_INCOMING_END:
            {
                int iResult = EFAILED;

                if (pMe->m_pIncomingCall != NULL)
                {
                    iResult = ICALL_End(pMe->m_pIncomingCall);
                    if (iResult == SUCCESS)
                    {
                        ICALL_Release(pMe->m_pIncomingCall);
                        pMe->m_pIncomingCall = NULL;
                    }
                }
                
                WriteLine(pMe, "Call End -->", NULL, FALSE);                
            }
            break;

        case USAGE_CALL_DTMF:
        {
           AECHAR szwStr[32];
           char szNumber[32];
           boolean Result;

           // get telephone number (main) from resource file
           ISHELL_LoadResString(pMe->a.m_pIShell, CALLDEMO_RES_FILE, IDS_STRING_TELEPHONE_NUMBER_MAIN, szwStr, sizeof(szwStr));
           WSTRTOSTR(szwStr, szNumber, sizeof(szNumber));

                  
          LISTENER_Init(&pMe->callListener, (PFNLISTENER) OriginateListener, pMe);
          Result = ICALLMGR_Originate(pMe->m_pCallMgr, AEET_CALL_TYPE_VOICE, szNumber, NULL, &pMe->m_pOutgoingCall, &pMe->callListener);
          if (Result != SUCCESS)
          {
             DBGPRINTF("ICALLMGR_Originate failed with res=%d", Result);
             break;
          }
          WriteLine(pMe, "Call Originate -->", NULL, FALSE);  
          WriteLine(pMe, "Press 2 to DTMF", NULL, FALSE);  
          pMe->m_isDTMFItem = TRUE;
        }
        break;

        case USAGE_CALL_PARTY3:
        {
           AECHAR szwStr[32];
           char szNumber[32];
           boolean Result;

           // get telephone number (main) from resource file
           ISHELL_LoadResString(pMe->a.m_pIShell, CALLDEMO_RES_FILE, IDS_STRING_TELEPHONE_NUMBER_A, szwStr, sizeof(szwStr));
           WSTRTOSTR(szwStr, szNumber, sizeof(szNumber));

                  
          LISTENER_Init(&pMe->callListener, (PFNLISTENER) OriginateListener, pMe);
          Result = ICALLMGR_Originate(pMe->m_pCallMgr, AEET_CALL_TYPE_VOICE, szNumber, NULL, &pMe->m_pOutgoingCall, &pMe->callListener);
          if (Result != SUCCESS)
          {
             DBGPRINTF("ICALLMGR_Originate failed with res=%d", Result);
             break;
          }
          WriteLine(pMe, "Call Originate A-->", NULL, FALSE);  
          WriteLine(pMe, "Press 3 to call B", NULL, FALSE);  
          pMe->m_isParty3Item = TRUE;
        }
        break;

        case USAGE_CALL_ANSWER:
            pMe->m_isRedirectItem = FALSE;
            break;
        
        case USAGE_CALL_REDIRECT:
            pMe->m_isRedirectItem = TRUE;
            break;
        
        default:
             return;
    }
            
    // Display above event. 
    DisplayEvent (pMe, wParam);

    return;
}

/*===========================================================================

FUNCTION: DisplayEvent

DESCRIPTION
    This function prints a heading at the top 
  of the screen to indicate which usage example was selected by the user, 
  It also prints a message on the bottom two lines of the screen instructing 
  the user to press the UP or DOWN key to return to
  the main menu to execute another example.
    
PROTOTYPE:
   static void DisplayEvent(CallDemoApp * pMe, uint16 wParam)

PARAMETERS:
   pMe:   [in]: Contains a pointer to the usage app data structure.
   wParam: [in]: Identifier of the selected example (example IDs are specified when the
                 main usage app menu is created).  If the value USAGE_ERASE_SCREEN is supplied
         instead, this function clears the screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void DisplayEvent (CallDemoApp *pMe, uint16 wParam)
{
  AEERect qrc;
  AEEDeviceInfo di; // Device Info
  AECHAR * szBuf;
  AEERect rc; 

  // Make sure the pointers we'll be using are valid
  if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->a.m_pIDisplay == NULL)
  {
    return;
  }

  if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)
  {
    return;
  }
  
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);
  qrc.x = 0;
  qrc.y = 0;
  qrc.dx = di.cxScreen;
  qrc.dy = di.cyScreen;
  rc.x = 5;
  rc.y = 5;
  rc.dx = di.cxScreen - 5;
  rc.dy = di.cyScreen - 5;

  // Specify text for heading based on example the user has selected
  switch (wParam)
  {
    case USAGE_ERASE_SCREEN:
       IDISPLAY_EraseRect(pMe->a.m_pIDisplay,&qrc);  //Clear the display
       FREE(szBuf);
       return;

    case USAGE_CALL_ORIG:
       STR_TO_WSTR("CALL ORIG", szBuf, TEXT_BUFFER_SIZE);
       break; 

    case USAGE_CALL_OUTGOING_END:
       STR_TO_WSTR("End OutGoing Call", szBuf, TEXT_BUFFER_SIZE);
       break; 

    case USAGE_CALL_INCOMING_END:
       STR_TO_WSTR("End Incoming Call", szBuf, TEXT_BUFFER_SIZE);
       break; 

    case USAGE_CALL_DTMF:
       STR_TO_WSTR("Call and DTMF", szBuf, TEXT_BUFFER_SIZE);
       break; 

    case USAGE_CALL_PARTY3:
       STR_TO_WSTR("Call Party 3", szBuf, TEXT_BUFFER_SIZE);
       break; 

    case USAGE_CALL_ANSWER:
       STR_TO_WSTR("Answer incoming call", szBuf, TEXT_BUFFER_SIZE);
       break; 

    case USAGE_CALL_REDIRECT:
       STR_TO_WSTR("Redirect incoming call", szBuf, TEXT_BUFFER_SIZE);
       break; 

    default:
       STR_TO_WSTR("Unsupported fn", szBuf, TEXT_BUFFER_SIZE);
  }

  // Print the heading at the top of the screen
  IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szBuf, -1,10, 5, &rc,
  IDF_ALIGN_NONE|  IDF_TEXT_TRANSPARENT);

  // Print a message at the bottom of the screen telling the user
  // how to return to the main app menu
  STR_TO_WSTR("Press Up/Down", szBuf, TEXT_BUFFER_SIZE);
  IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szBuf, -1,10, qrc.dy - 25, &rc,
    IDF_ALIGN_NONE | IDF_TEXT_TRANSPARENT);
  STR_TO_WSTR("keys to continue..", szBuf, TEXT_BUFFER_SIZE);
  IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szBuf, -1,10, qrc.dy - 15, &rc,
    IDF_ALIGN_NONE | IDF_TEXT_TRANSPARENT);
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,TRUE);

  FREE(szBuf);
}
/*===========================================================================

FUNCTION: BuildMainMenu

DESCRIPTION
    This function constructs the menu control that allows the user to
  select a usage example to execute.  The command ID for each menu item
  is passed to the usage example function above when the user selects the
  menu item for that example.
    
PROTOTYPE:
   static void BuildMainMenu(CallDemoApp * pMe)

PARAMETERS:
   pMe:   [in]: Contains a pointer to the usage app data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void BuildMainMenu(CallDemoApp *pMe)
{
  AEERect qrc;
  AEEDeviceInfo di;
  AECHAR * szBuf;

  // Make sure the pointers we'll be using are valid
  if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->m_pIMenu == NULL)
  {
    return;
  }
  
  if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)
  {
     return;
  }
  
  // Set Title
  STR_TO_WSTR("CALLDEMO & CALLDEMOMsg Example", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_SetTitle(pMe->m_pIMenu, NULL, 0, szBuf);

  //Set the Rectangle for the Menu
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);
  qrc.x = 0;
  qrc.y = 0;
  qrc.dx = di.cxScreen;
  qrc.dy = di.cyScreen;
  IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);  

  //Add individual entries to the Menu

  // Add originate call to menu
  STR_TO_WSTR("1. Originate Call", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_CALL_ORIG, szBuf, 0);

  STR_TO_WSTR("2. End OutGoing Call", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_CALL_OUTGOING_END, szBuf, 0);

  STR_TO_WSTR("3. End Incoming Call", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_CALL_INCOMING_END, szBuf, 0);

  STR_TO_WSTR("4. Call and DTMF", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_CALL_DTMF, szBuf, 0);

  STR_TO_WSTR("5. Call Party", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_CALL_PARTY3, szBuf, 0);

  STR_TO_WSTR("6. Call answer", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_CALL_ANSWER, szBuf, 0);

  STR_TO_WSTR("7. Call redirect", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_CALL_REDIRECT, szBuf, 0);

  // Activate menu
  IMENUCTL_SetActive(pMe->m_pIMenu,TRUE);

  FREE(szBuf);
}
/*===========================================================================

FUNCTION: DisplayOutput

DESCRIPTION
    This function displays an output string at a given line number on the
    screen. If the nline parameter is a negative value (-1) the string
    is displayed in the middle of the screen. If the "nline" value is larger
    than or equal to zero the "nline" value is multiplied by 15 and the 
    resulting value in pixels is set to the y-coordinate of the start of 
    the string display on the screen. If the string does not fit on one line
    the string wraps around to the next line (spaced rougly 10-15 pixels apart).
    By default 5 is used as the starting the x-coordinate of a displayed 
    string.

    How many characters that fit on one line is calculated for each line 
    that is wrapped around to the next line.

    Note: depending on the phone screen size and the fonts used for characters 
          the output might differ on different handsets (devices). Where some 
          handsets will have a smaller screen and large default fonts which will 
          cause partial overlapping of lines. This function does not try to address
          these issues (this is meant as a simple display function).
    
PROTOTYPE:
   int DisplayOutput(IShell *pIShell, IDisplay *pDisplay, int nline, char *pszStr, AECHAR *pszwStr, boolean isWideChar)

PARAMETERS:
   pIShell:   [in]: Contains a pointer to the IShell interface.
   pIDisplay: [in]: Contains a pointer to the IDisplay interface.
   nline:     [in]: Contains the line number to start displaying the text. The line
        numbers are by default spaced 15 pixels apart along the y-axis.
   pszStr:    [in]: The character string to be displayed on the screen.

DEPENDENCIES
  None

RETURN VALUE
  Number of lines written to the screen.

SIDE EFFECTS
  None

===========================================================================*/
static int DisplayOutput(IShell *pIShell, IDisplay *pIDisplay, int nline, char *pszStr, AECHAR *pszwStr, boolean isWideChar)
{
  AEEDeviceInfo di; // Device Info
  AECHAR * szBuf;     // a buffer that supports 200 char string
  AECHAR * psz = NULL;
  int charHeight = 0;      // Stores the char height in pixels for given font
  int pnAscent = 0;        // Stores the ascent in number of pixels
  int pnDescent = 0;       // Stores the descent in number of pixels
  int pixelWidth;
  AEEFont font = AEE_FONT_NORMAL;
  int pnFits = 0, dy;
  int totalCh = 0;
  int numLinesPrinted = 0;

  // Make sure the pointers we'll be using are valid
  if (pIShell == NULL || pIDisplay == NULL)
  {
    return 0;
  }
  
  if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)
  {
    return 0;
  }
  // Get device information
  ISHELL_GetDeviceInfo(pIShell,&di);

  // Get the font metrics info
  charHeight = IDISPLAY_GetFontMetrics (pIDisplay, AEE_FONT_NORMAL,
     &pnAscent, &pnDescent);
  if(isWideChar)
  {
       WSTRCPY(szBuf,pszwStr);
  }
  else
  {
   // Convert to wide string (unicode)
   STR_TO_WSTR ((char *)pszStr, szBuf, TEXT_BUFFER_SIZE);
  }
  // If nlines is zero then print this string starting around the middle of 
  // the screen. Or else multiply nlines by charheight to decide the y coordinate of
  // the start of the string.
  if (nline < 0) {
     dy = di.cyScreen*2/5;
  }
  else{
     dy = nline * charHeight + 5;
  }

  // psz keeps track of the point from which to write from the string buffer
  // in case the string does not fit one line and needs to wrap around in the
  // next line.
  psz = szBuf;
     
  // Need to calculate the lotal string length to decide if any wrapping
  // around is needed.
  if(isWideChar)
  {
       totalCh = 2*WSTRLEN (pszwStr);
  }
  else
  {
       totalCh = STRLEN ((char *)pszStr);
  }

  // Keep displaying text string on multiple lines if the string can't be displayed
  // on one single line. Lines are spaced 15 pixels apart.
  while ((totalCh > 0) && (*psz != NULL))
  { 
     // Get information on how many characters will fit in a line.
     // Give the pointer to the buffer to be displayed, and the number of
     // pixels along the x axis you want to display the string in (max number)
     // pnFits will have the max number of chars that will fit in the maxWidth
     // number of pixels (given string can't fit in one line), or the number of 
     // chars in the string (if it does fit in one line). pnWidth gives the
     // number of pixels that will be used to display pnFits number of chars.
     pixelWidth = IDISPLAY_MeasureTextEx(pIDisplay,
                     font, 
                     (AECHAR *) psz,  // Start of the buffer to display,
                     -1,
                     di.cxScreen - 5, // maxWidth
                     &pnFits);         // Number of chars that will fit a line

     // If pnFits is zero there is something wrong in the input to above function. 
     // Normally this scenario should not occur. But, have the check anyway.
     if (pnFits == 0)
     {
       FREE(szBuf);
       return 0;
     }

     IDISPLAY_DrawText(pIDisplay, AEE_FONT_NORMAL, psz, pnFits, 5 /*start dx*/, 
        dy, 0 /* use default rectangle coordinates */, 0);

     psz += pnFits;      // move pointer to the next segment to be displayed
     totalCh -= pnFits;  // reduce the total number of characters to still display
     dy += charHeight;   // Place next line charHeight pixels below the 
                         // previous line.
     ++numLinesPrinted;

     IDISPLAY_Update(pIDisplay); //, TRUE);
     if (totalCh < pnFits)
        pnFits = totalCh;  // if total number is less than pnFits, adjust pnFits
  }

  FREE(szBuf);

  return numLinesPrinted;   
} // End of DisplayOutput

/*===========================================================================

FUNCTION: WriteLine

DESCRIPTION
    This function displays lines of text on the screen, taking into account
  the number of text lines printed so far and the number of lines available
  for output.

    If all available lines are exhausted, this function returns without doing
  anything.

    If the last available line of the screen is reached, this function prints
  "..." to indicate that some lines may not have been printed due to lack
  of space.

    Otherwise, this function prints the line on the screen by calling DisplayOutput,
  and updates the count of lines printed based on how many lines DisplayOutput
  used to print the text.
 
    
PROTOTYPE:
   static void WriteLine(CallDemoApp * pMe,  char *pszStr, AECHAR *pszwStr, boolean isWideChar)

PARAMETERS:
   pMe:   [in]: Contains a pointer to this app's data structure.
   pszStr:    [in]: The character string to be displayed on the screen.

DEPENDENCIES
  Assumes the pMe->m_cLineNum gets initialized to the starting line for text display
  on the screen, and that pMe->m_cMaxLine contains the last available line for 
  output on the screen.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void WriteLine(CallDemoApp * pMe, char *pszStr, AECHAR *pszwStr, boolean isWideChar)
{
   
  // If all available lines used, return
  if (pMe->m_cLineNum > pMe->m_cMaxLine)
    return;

  // If we've reached last available line, print an
  // ellipsis indicator
  if (pMe->m_cLineNum == pMe->m_cMaxLine)
  {
    DisplayOutput(pMe->a.m_pIShell, pMe->a.m_pIDisplay, pMe->m_cLineNum++, "...", NULL, FALSE);
    return;
  }

  // There are lines available for printing.  Print the string passed as
  // input and update the count of lines available
  pMe->m_cLineNum += DisplayOutput(pMe->a.m_pIShell, pMe->a.m_pIDisplay, pMe->m_cLineNum, pszStr, pszwStr, isWideChar);
  return;

} // End of WriteLine

static void OriginateListener(CallDemoApp *pMe, ModelEvent *pEvent)
{
   AEETCallEvent* pCallEvent = (AEETCallEvent*) pEvent;

   switch (pCallEvent->evCode)
   {
      case AEET_EVENT_CALL_CONNECT:
      {
        DBGPRINTF("Rx:   AEET_EVENT_CALL_CONNECT cd=%d", pCallEvent->call.cd);
        WriteLine(pMe, "Rx:   AEET_EVENT_CALL_CONNECT", NULL, FALSE);    

         break;
      }

      case AEET_EVENT_CALL_ERROR:
      {
         DBGPRINTF("AEET_EVENT_CALL_ERROR");
         WriteLine(pMe, "Rx:   AEET_EVENT_CALL_ERROR", NULL, FALSE);                
         LISTENER_Cancel(&pMe->callListener);

         break;
      }

      case AEET_EVENT_INBAND_FWD_BURST_DTMF:
      {
         DBGPRINTF("AEET_EVENT_INBAND_FWD_BURST_DTMF");
         WriteLine(pMe, "Rx:   AEET_EVENT_INBAND_FWD_BURST_DTMF", NULL, FALSE);                
         break;
      }

      case AEET_EVENT_CALL_END:
      {

         DBGPRINTF("Rx:   AEET_EVENT_CALL_END");
         WriteLine(pMe, "Rx:   AEET_EVENT_CALL_END", NULL, FALSE);                
         LISTENER_Cancel(&pMe->callListener);
         if (Telephone_EndCall(pMe) != SUCCESS) 
         {
           if (pMe->m_pOutgoingCall) 
           {
               ICALL_Release(pMe->m_pOutgoingCall);
               pMe->m_pOutgoingCall = NULL;
           }
         }

         if(pMe->m_isDTMFItem)
         {
           pMe->m_isDTMFItem = FALSE;
         }         

         if(pMe->m_isParty3Item)
         {
           pMe->m_isParty3Item = FALSE;
           LISTENER_Cancel(&pMe->callListenerB);
         } 

         if(pMe->m_isRedirectItem)
         {
           pMe->m_isRedirectItem = FALSE;
         } 
         
         break;
      }

      default:
      {
         DBGPRINTF("OriginateListener Rx:  %d", pCallEvent->evCode);
         break;
      }
   }
}

static void MyPhoneListener(CallDemoApp *pMe, ModelEvent *pEvent)
{
    char incom_info[100];
    AEETCallEvent * pCallEvent = (AEETCallEvent*) pEvent;

    if (pCallEvent && (AEET_EVENT_CALL_INCOM == pCallEvent->evCode))
    {
        AEETCallEventData * pTCallEventData = (AEETCallEventData *)&(pCallEvent->call);
	 AEETCalls calls = {0};
	 int iResult = EFAILED;
        iResult = ITELEPHONE_GetCalls(pMe->m_pTelephone, &calls, sizeof(AEETCalls));
        if ((iResult != SUCCESS) || (calls.dwCount == 0))
        {
              DBGPRINTF("ITELEPHONE_GetCalls ret: %d, count=%d", iResult, calls.dwCount);
		return;
        }

        DBGPRINTF("ITELEPHONE_GetCalls AEECallDesc=%d", calls.calls[0]);


        ICALLMGR_GetCall(pMe->m_pCallMgr, calls.calls[0], &pMe->m_pIncomingCall);

        SPRINTF(incom_info, "Rx: CALL INCOM EVENT with num: %s", pTCallEventData->number.buf);
        WriteLine(pMe, incom_info, NULL, FALSE);        
        DBGPRINTF("Rx: CALL INCOM EVENT with num: %s", pTCallEventData->number.buf);

        if(pMe->m_isRedirectItem)
        {
#ifdef EVENT_AUTO_GET
          ISHELL_PostEvent(pMe->a.m_pIShell, 
                                 AEECLSID_CALLDEMO, 
                                 pMe->m_UserRedirectEvent,
                                 0,
                                 0);
#else
          ISHELL_PostEvent(pMe->a.m_pIShell, 
                                 AEECLSID_CALLDEMO, 
                                 EVT_MY_REDIRECT,
                                 0,
                                 0);
#endif
        }
        else
        {
            DBGPRINTF("handler of call incoming by answer");
            if (pMe->m_pIncomingCall)
            { 
                int nError;
                nError = ICALL_Answer(pMe->m_pIncomingCall);
                if (SUCCESS != nError)
                {
                    DBGPRINTF("ICALL_Answer failed ret=%d", nError);
                }
                else
                {
                    DBGPRINTF("ICALL_Answer -->");
                    WriteLine(pMe, "ICALL_Answer -->", NULL, FALSE);          
                }
            }
      
#if 0
#ifdef EVENT_AUTO_GET
          ISHELL_PostEvent(pMe->a.m_pIShell, 
                                 AEECLSID_CALLDEMO, 
                                 pMe->m_UserAnswerEvent,
                                 0,
                                 0);
#else
          ISHELL_PostEvent(pMe->a.m_pIShell, 
                                 AEECLSID_CALLDEMO, 
                                 EVT_MY_ANSWER,
                                 0,
                                 0);
#endif
#endif
        }
    }

    if (pCallEvent && (AEET_EVENT_CALL_CALLER_ID == pCallEvent->evCode))
    {

      AEETCallEventData * pTCallEventData = (AEETCallEventData *)&(pCallEvent->call);

      SPRINTF(incom_info, "Rx: CALL INCOM EVENT with num: %s", pTCallEventData->number.buf);
      WriteLine(pMe, incom_info, NULL, FALSE);        

      DBGPRINTF("Rx: CALLER ID : %s", pTCallEventData->number.buf);
    }

    if (pCallEvent && (AEET_EVENT_CALL_ANSWER == pCallEvent->evCode))
    {

      DBGPRINTF("Rx: AEET_EVENT_CALL_ANSWER");
    }

    if (pCallEvent && (AEET_EVENT_CALL_CONNECT == pCallEvent->evCode))
    {

      DBGPRINTF("Rx: AEET_EVENT_CALL_CONNECT");
    }

    if (pCallEvent && (AEET_EVENT_CALL_END == pCallEvent->evCode))
    {
         if(pMe->m_isRedirectItem)
         {
           pMe->m_isRedirectItem = FALSE;
         } 

      DBGPRINTF("Rx: AEET_EVENT_CALL_END");
    }

}

/*===========================================================================
Function: Telephone_EndCall()

Description:
   Ends an outgoing call.

Prototype:
   static int Telephone_EndCall(Telephone* pMe)

Parameters:
   pMe: Pointer to CallDemo phone instance.

Return Value:
   SUCCESS on successful termination, EFAILED otherwise.
===========================================================================*/
static int Telephone_EndCall(CallDemoApp* pMe)
{
   int iResult = EFAILED;

   if (pMe->m_pOutgoingCall != NULL)
   {
      iResult = ICALL_End(pMe->m_pOutgoingCall);
      if (iResult == SUCCESS)
      {
         ICALL_Release(pMe->m_pOutgoingCall);
         pMe->m_pOutgoingCall = NULL;
      }
   }

   return iResult;
}



