/*===========================================================================

FILE    : mybattery.c
SERVICES  : a battery inquiry sample application
DESCRIPTIONS: This is a sample application to use the following interfaces:
        IBattery, IBatteryNotifier
        It demonstrates how to query battery information.
        It demonstrates how to handle battery level change event.
Authoer   : Tech. Support, UniBrew
Date    : January, 2007
Conatct   : yqhuang@unicom-brew.com
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */

#include "AEEModel.h"
#include "mybattery.h"

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean mybattery_HandleEvent(mybattery* pMe, AEEEvent eCode,                                              uint16 wParam, uint32 dwParam);
static boolean mybattery_InitAppData(mybattery* pMe);
static void    mybattery_FreeAppData(mybattery* pMe);
static void     mybattery_EventProcess (void *pUserData, ModelEvent *pEvent);
// Display Functions
static void BuildMainMenu(mybattery *pMe);
static void DisplayText(mybattery *pMe);


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
   initialization section every time except for a call to IDisplay_InitAppData(). 
   This is done as follows: InitAppData() is called to initialize AppletData 
   instance. It is app developers responsibility to fill-in app data initialization 
   code of InitAppData(). App developer is also responsible to release memory 
   allocated for data contained in AppletData -- this can be done in 
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
  clsID: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell object. 

  pIModule: pin]: Contains pointer to the IModule object to the current module to which
  this app belongs

  ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
  of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
  *ppObj = NULL;

  if( ClsId == AEECLSID_MYBATTERY )
  {
    // Create the applet and make room for the applet structure
    if( AEEApplet_New(sizeof(mybattery),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)mybattery_HandleEvent,
                          (PFNFREEAPPDATA)mybattery_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
    {
      //Initialize applet data, this is called before sending EVT_APP_START
                     // to the HandleEvent function
      if(mybattery_InitAppData((mybattery*)*ppObj))
      {
        //Data initialized successfully
        return(AEE_SUCCESS);
      }
      else
      {
        //Release the applet. This will free the memory allocated for the applet when
        // AEEApplet_New was called.
        IAPPLET_Release((IApplet*)*ppObj);
        return EFAILED;
      }

        } // end AEEApplet_New

    }

  return(EFAILED);
}


/*===========================================================================
FUNCTION SampleAppWizard_HandleEvent

DESCRIPTION
  This is the EventHandler for this app. All events to this app are handled in this
  function. All APPs must supply an Event Handler.

PROTOTYPE:
  boolean SampleAppWizard_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
  pi: Pointer to the AEEApplet structure. This structure contains information specific
  to this applet. It was initialized during the AEEClsCreateInstance() function.

  ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean mybattery_HandleEvent(mybattery* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    int nErr = SUCCESS;

    switch (eCode) 
    {
        // App is told it is starting up
        case EVT_APP_START:                        
            BuildMainMenu(pMe);
            IDISPLAY_Update(pMe->m_pIDisplay);
            return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...
            mybattery_FreeAppData(pMe);
            return(TRUE);


        // App is being suspended 
        case EVT_APP_SUSPEND:
            // Add your code here...
            return(TRUE);


        // App is being resumed
        case EVT_APP_RESUME:
            // Add your code here...
            return(TRUE);


        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
            // Add your code here...
            return(TRUE);

            // A key was pressed. Look at the wParam above to see which key was pressed. The key
            // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
        case EVT_KEY:
            if(pMe->m_pIMainMenu && IMENUCTL_IsActive(pMe->m_pIMainMenu))
            {
                return IMENUCTL_HandleEvent(pMe->m_pIMainMenu, EVT_KEY, wParam, 0);
            }
      
            if(pMe->m_pISoftMenu && IMENUCTL_IsActive(pMe->m_pISoftMenu))
            {
                switch(wParam)
                {
                    case AVK_CLR:
                        // Go one level up (i.e. the MainMenu)
                        ISTATIC_Release(pMe->m_pIStatic);

                        IMENUCTL_SetActive(pMe->m_pISoftMenu, FALSE);
                        IMENUCTL_SetActive(pMe->m_pIMainMenu, TRUE);
                        IDISPLAY_ClearScreen(pMe->m_pIDisplay);
                        IMENUCTL_Redraw(pMe->m_pIMainMenu);
                        return TRUE;
             
                    default:
                        return IMENUCTL_HandleEvent(pMe->m_pISoftMenu, EVT_KEY, wParam, 0);

                }
            }

            return(TRUE);

        case EVT_COMMAND:
            if (pMe->m_pIBattery == NULL) 
            {
                DBGPRINTF("UNIBREW: pMe->m_pIBattery is NULL!");
                return (FALSE);
            }
            switch (wParam)
            {
                case RT_BATTERY_INFO:
                {
                    AEEBatteryStatus batteryStatus;
                    AEEBatteryChargerStatus chargerStatus;
                    uint32 dwData;    // for battery level data
                    uint16 nScale;    // for battery scale
                    uint16 nLevel;    // for battery level
                    boolean bExPower=FALSE;   // external power is presented or not 
                  
                    char szBuf[512];
                    AECHAR wszText[512];
        
                    const char battStatusList[5][10] = {"UNKOWN", "POWERDOWN", "LOW", "NORMAL", "MAX"};
                    const char chargerStatusList[12][14] = {"UNKOWN", "NO_BATT", "UNKNOWN_BATT", "DEAD_BATT", "NO_CHARGE", "NO_CHARGER", "CANNOT_CHARGE", "INIT", "CHARGING", "FULLY_CHARGE", "OVERVOLTAGE", "MAX"};
        
        
                    STRTOWSTR("Battery Info", pMe->m_szTitle,sizeof(pMe->m_szTitle));
                    if ( (nErr=IBATTERY_GetBatteryLevel(pMe->m_pIBattery, &dwData)) != SUCCESS)
                    {
                        DBGPRINTF("UNIBREW: ERR%d, failed to get battery level!", nErr);
                        SPRINTF(szBuf, "ERR %d, failed to get battery level!\n", nErr);
                        STRTOWSTR(szBuf, pMe->m_pszText, TEXT_BUF_SIZE);
                        DisplayText(pMe);
                        return FALSE;
                    } 
                    else  
                    {
                        nScale = GETBATTERYSCALE(dwData);
                        nLevel = GETBATTERYLEVEL(dwData);
                        DBGPRINTF("UNIBREW: scale = %d, level=%d", nScale, nLevel);
                        SPRINTF(szBuf, "Scale = %d, Level = %d\n", nScale, nLevel);
                        STRTOWSTR(szBuf, pMe->m_pszText, TEXT_BUF_SIZE);
                    }
        
                    if ( (nErr=IBATTERY_GetBatteryStatus(pMe->m_pIBattery, &batteryStatus)) != SUCCESS)
                    {
                        DBGPRINTF("UNIBREW:ERR%d, failed to get battery status!", nErr);
                        SPRINTF(szBuf,"ERR %d, failed to get battery status!\n", nErr);
                        STRTOWSTR(szBuf, wszText, sizeof(wszText));
                        WSTRCAT(pMe->m_pszText, wszText);
                        DisplayText(pMe);
                        return FALSE;
                    } 
                    else 
                    {
                        DBGPRINTF("UNIBREW: battery status = %d (%s)", batteryStatus, battStatusList[batteryStatus]);
                        SPRINTF(szBuf, "Battery Status = %d (%s)\n", batteryStatus, battStatusList[batteryStatus]);
                        STRTOWSTR(szBuf, wszText, sizeof(wszText));
                        WSTRCAT(pMe->m_pszText, wszText);
                    }
        
                    if ( (nErr=IBATTERY_IsExternalPowerPresent(pMe->m_pIBattery, &bExPower)) != SUCCESS)
                    {
                        DBGPRINTF("UNIBREW:ERR%d, failed to detect external power status!", nErr);
                        SPRINTF(szBuf, "Err %d, failed to detect external power status!\n", nErr);
                        STRTOWSTR(szBuf, wszText, sizeof(wszText));
                        WSTRCAT(pMe->m_pszText, wszText);
                        DisplayText(pMe);
                        return FALSE;
                    } 
                    else 
                    {
                  
                        if (bExPower)
                        {
                            DBGPRINTF("UNIBREW:External power is presented!");
                            SPRINTF(szBuf, "External power is presented!\n");
                        } 
                        else 
                        {
                            DBGPRINTF("UNIBREW:External power is not presented!");
                            SPRINTF(szBuf, "External power is not presented!\n");
                        }
                        STRTOWSTR(szBuf, wszText, sizeof(wszText));
                        WSTRCAT(pMe->m_pszText,  wszText);
                    }
        
                    if ( (nErr = IBATTERY_GetBatteryChargerStatus(pMe->m_pIBattery, &chargerStatus)) != SUCCESS)
                    {
                        DBGPRINTF("UNIBREW: ERR%d, failed to get battery charger status!", nErr);
                        SPRINTF(szBuf, "Err %d, failed to get battery charger status!\n", nErr);
                        STRTOWSTR(szBuf, wszText, sizeof(wszText));
                        WSTRCAT(pMe->m_pszText, wszText);
                        DisplayText(pMe);
                        return FALSE;
                    } 
                    else 
                    {
                        DBGPRINTF("UNIBREW: battery charger status = %d(%s)", chargerStatus, chargerStatusList[chargerStatus]);
                        SPRINTF(szBuf, "Battery charger status = %d(%s)\n", chargerStatus, chargerStatusList[chargerStatus]);
                        STRTOWSTR(szBuf, wszText, sizeof(wszText));
                        WSTRCAT(pMe->m_pszText, wszText);
                    }
                    DisplayText(pMe);
                    return TRUE;
                }
                    
                case RT_BATTERY_NOTIFY:
                {
                    char szBuf[512];
                    STRTOWSTR("Event Notifier", pMe->m_szTitle,sizeof(pMe->m_szTitle));
                    if ( (nErr = ISHELL_RegisterNotify(pMe->m_pIShell, AEECLSID_MYBATTERY, AEECLSID_BATTERYNOTIFIER, NMASK_BATTERY_EXTPWR_CHANGE )) != SUCCESS)
                    {
                        DBGPRINTF("UNIBREW: Err%d, failed to register NMASK_BATTERY_EXTPWR_CHANGE!");
                        SPRINTF(szBuf, "Err %d, failed to register NMASK_BATTERY_EXTPWR_CHANGE!\n", nErr);
                        STRTOWSTR(szBuf, pMe->m_pszText, TEXT_BUF_SIZE);
                        DisplayText(pMe);
                        return FALSE;
                    } 
                    SPRINTF(szBuf, "Registered NMASK_BATTERY_EXTPWR_CHANGE event!\n");
                    STRTOWSTR(szBuf, pMe->m_pszText, TEXT_BUF_SIZE);
                    DisplayText(pMe);
                    return TRUE;
                }
                
                case RT_BATTERY_NOTIFY_CANCEL:
                {
                    char szBuf[512];
                    STRTOWSTR("Cancle Notifier", pMe->m_szTitle,sizeof(pMe->m_szTitle));
                    if ( (nErr = ISHELL_RegisterNotify(pMe->m_pIShell, AEECLSID_MYBATTERY, AEECLSID_BATTERYNOTIFIER, 0 )) != SUCCESS)
                    {
                        DBGPRINTF("Cancle notify failure");
                        SPRINTF(szBuf, "Cancle notify failure:%d", nErr);
                        STRTOWSTR(szBuf, pMe->m_pszText, TEXT_BUF_SIZE);
                        DisplayText(pMe);
                        return(TRUE);
                    } 
                    SPRINTF(szBuf, "Cancle Noitfier ok");
                    STRTOWSTR(szBuf, pMe->m_pszText, TEXT_BUF_SIZE);
                    DisplayText(pMe);
                    return(TRUE);
                }

                case RT_QUERYINT_IMODEL:
                {
                    char szBuf[512];
                    DBGPRINTF("RT_QUERYINT_IMODEL");

                    STRTOWSTR("IMODEL command", pMe->m_szTitle,sizeof(pMe->m_szTitle));

                    nErr = IBATTERY_QueryInterface(pMe->m_pIBattery, AEEIID_MODEL, (void **) (&(pMe->m_pIModel)));
                    if ( nErr != SUCCESS)
                    {
                        DBGPRINTF("IBATTERY_QueryInterface failure");
                        return (TRUE);
                    }

                    LISTENER_Init(&pMe->myModelListerner, mybattery_EventProcess, pMe);

                    nErr = IModel_AddListener(pMe->m_pIModel, &pMe->myModelListerner); 
                    if(nErr != AEE_SUCCESS)
                    {
                        DBGPRINTF("IModel_AddListener failure");
                        SPRINTF(szBuf, "IModel_AddListener failure\n");
                        STRTOWSTR(szBuf, pMe->m_pszText, TEXT_BUF_SIZE);
                    }
                    else
                    {
                        DBGPRINTF("IModel_AddListener ok");
                        SPRINTF(szBuf, "IModel_AddListener ok\n");
                        STRTOWSTR(szBuf, pMe->m_pszText, TEXT_BUF_SIZE);

                    }
                    DisplayText(pMe);
                    return (TRUE);
                }

                case RT_QUERYINT_IMODEL_CANCEL:
                {
                    char szBuf[512];
                    boolean isRegisted;
                    STRTOWSTR("Cancle imodel", pMe->m_szTitle,sizeof(pMe->m_szTitle));

                    isRegisted = LISTENER_IsRegistered(&(pMe->myModelListerner));
                    DBGPRINTF("Imodel reg listener? %d", isRegisted);
                    
                    if ( isRegisted)
                    {
                        LISTENER_Cancel(&(pMe->myModelListerner));
                        SPRINTF(szBuf, "Cancel registed IModel ok\n");
                        STRTOWSTR(szBuf, pMe->m_pszText, TEXT_BUF_SIZE);
                        DisplayText(pMe);
                    }
                    else
                    {
                        SPRINTF(szBuf, "No IModel registed\n");
                        STRTOWSTR(szBuf, pMe->m_pszText, TEXT_BUF_SIZE);
                        DisplayText(pMe);
                    }
                    return(TRUE);
                }
    
                case SOFTMENU_BACK:
                {
                  DBGPRINTF("SOFTMENU_BACK");
                  if (pMe->m_pIStatic != NULL)
                  {
                    ISTATIC_Release(pMe->m_pIStatic);
                    pMe->m_pIStatic = NULL;
                  }
                  IMENUCTL_SetActive(pMe->m_pISoftMenu, FALSE);
                  IMENUCTL_SetActive(pMe->m_pIMainMenu, TRUE);
                  IDISPLAY_ClearScreen(pMe->m_pIDisplay);
                  IMENUCTL_Redraw(pMe->m_pIMainMenu);
                  return (TRUE);
                }
                      

                default:
                    return TRUE;
              }
              return(TRUE);


        case EVT_NOTIFY:
            {
                    uint32 uIsExtPowerExist;
                    AEENotify* pNotify = (AEENotify*) dwParam;
                    char szBuf[80];
                    AECHAR wszText[80];

                    // notifer monitor status of battery
                    if (pNotify && (pNotify->cls == AEECLSID_BATTERYNOTIFIER)) // event sender
                    {
                        DBGPRINTF("AEECLSID_BATTERYNOTIFIER sender");

                        if ((pNotify->dwMask & NMASK_BATTERY_EXTPWR_CHANGE)) 
                        {
                            DBGPRINTF("Notify mask ok");
                            uIsExtPowerExist = (uint32 )pNotify->pData;

                            SPRINTF(szBuf, "Notify ext pow: %d \n", uIsExtPowerExist);
                            STRTOWSTR(szBuf, wszText, sizeof(wszText));
                            WSTRCAT(pMe->m_pszText, wszText);
                            
                            DisplayText(pMe);
                        }
                    }
                    
                    return(TRUE);
            }

#if 0
            
            case EVT_NOTIFY: // example for battery level change event
            {
                char szBuf[512];
                uint16 nScale = 0;  // battery scale
                uint16 nLevel = 0;  // battery level
                AEENotify* pNotify = (AEENotify*)(dwParam);
                uint32 dwData = (uint32 ) (pNotify->pData); // for new battery level
                nScale = GETBATTERYSCALE(dwData);
                nLevel = GETBATTERYLEVEL(dwData);
        
                //if(pMe->m_pISoftMenu && IMENUCTL_IsActive(pMe->m_pISoftMenu))
                //{
                //  ISTATIC_Release(pMe->m_pIStatic);
                //}
                DBGPRINTF("UNIBREW: EVT_NOTIFY received for battery level change!");
                DBGPRINTF("Scale=%d, Level=%d", nScale, nLevel);
                
                STRTOWSTR("New Battery Level Info", pMe->m_szTitle, sizeof(pMe->m_szTitle));
                SPRINTF(szBuf, "Scale = %d, Level = %d\n", nScale, nLevel);
                STRTOWSTR(szBuf, pMe->m_pszText, TEXT_BUF_SIZE);
                DisplayText(pMe);
                IMENUCTL_Redraw(pMe->m_pISoftMenu);
                return TRUE;
              }
#endif            
              // If nothing fits up to this point then we'll just break out
              default:
                  break;
           }
        
           return FALSE;
}


// this function is called when your application is starting up
static boolean mybattery_InitAppData(mybattery* pMe)
{
    int nErr = SUCCESS;
    AECHAR szBuf[15];
  
    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

    // Insert your code here for initializing or allocating resources...
    pMe->m_pIDisplay = pMe->a.m_pIDisplay;
    pMe->m_pIShell = pMe->a.m_pIShell;
    if ( ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_MENUCTL, (void **) (&(pMe->m_pIMainMenu))) != SUCCESS)
    {
        return FALSE;
    }

    if ( ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_SOFTKEYCTL, (void **) (&(pMe->m_pISoftMenu))) != SUCCESS)
    {
        return FALSE;
    }

    STRTOWSTR("Back", szBuf, sizeof(szBuf));
    if (!IMENUCTL_AddItem(pMe->m_pISoftMenu, 0, 0, SOFTMENU_BACK, szBuf, 0))
    {
        return FALSE;
    }

    if ( (pMe->m_pszText = (AECHAR *) MALLOC(TEXT_BUF_SIZE)) == NULL)
    {
        return FALSE;
    }
  
    if ( (nErr = ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_BATTERY, (void **) (&(pMe->m_pIBattery)))) != SUCCESS)
    {
        DBGPRINTF("UNIBREW: ERR%d, failed to create IBattery obj!", nErr);
        return FALSE;
    }

    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
static void mybattery_FreeAppData(mybattery* pMe)
{
    // insert your code here for freeing any resources you have allocated...

    // example to use for releasing each interface:
    // if ( pMe->pIMenuCtl != NULL )         // check for NULL first
    // {
    //    IMENUCTL_Release(pMe->pIMenuCtl)   // release the interface
    //    pMe->pIMenuCtl = NULL;             // set to NULL so no problems trying to free later
    // }
    //

  if( pMe->m_pIStatic != NULL)
  {
    ISTATIC_Release(pMe->m_pIStatic);
    pMe->m_pIStatic = NULL;
  }

  if( pMe->m_pIMainMenu != NULL)
  {
    IMENUCTL_Release(pMe->m_pIMainMenu);
    pMe->m_pIMainMenu = NULL;
  }

  if( pMe->m_pISoftMenu != NULL)
  {
    IMENUCTL_Release(pMe->m_pISoftMenu);
    pMe->m_pISoftMenu = NULL;
  }
  
  if( pMe->m_pIBattery != NULL)
  {
    IBATTERY_Release(pMe->m_pIBattery);
    pMe->m_pIBattery = NULL;
  }

  FREEIF(pMe->m_pszText);
}

static void BuildMainMenu(mybattery *pMe) 
{

    AECHAR szBuf[40];
    AEERect rect;

    SETAEERECT(&rect, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen);

    IMENUCTL_SetRect(pMe->m_pIMainMenu, &rect);

    // Set Title
    STR_TO_WSTR("MyBattery charge demo", szBuf, sizeof(szBuf));
    IMENUCTL_SetTitle(pMe->m_pIMainMenu, NULL, 0, szBuf);

    //Add individual entries to the Menu

    STR_TO_WSTR("1. Battery Status Info", szBuf, sizeof(szBuf));
    IMENUCTL_AddItem(pMe->m_pIMainMenu, 0, 0, RT_BATTERY_INFO, szBuf, 0);

    STR_TO_WSTR("2. Add Notify Listener", szBuf, sizeof(szBuf));
    IMENUCTL_AddItem(pMe->m_pIMainMenu, 0, 0, RT_BATTERY_NOTIFY, szBuf, 0);

    STR_TO_WSTR("3. Cancle Notify Listener", szBuf, sizeof(szBuf));
    IMENUCTL_AddItem(pMe->m_pIMainMenu, 0, 0, RT_BATTERY_NOTIFY_CANCEL, szBuf, 0);

    STR_TO_WSTR("4. IModel Register", szBuf, sizeof(szBuf));
    IMENUCTL_AddItem(pMe->m_pIMainMenu, 0, 0, RT_QUERYINT_IMODEL, szBuf, 0);

    STR_TO_WSTR("5. Cancle IModel Reg", szBuf, sizeof(szBuf));
    IMENUCTL_AddItem(pMe->m_pIMainMenu, 0, 0, RT_QUERYINT_IMODEL_CANCEL, szBuf, 0);

    if(pMe->m_pISoftMenu)
    {
        IMENUCTL_SetActive(pMe->m_pISoftMenu,FALSE);
    }
    IMENUCTL_SetActive(pMe->m_pIMainMenu,TRUE);
}

static void DisplayText(mybattery *pMe)
{
  int nErr = SUCCESS;
  AEERect rect;

  if ( (nErr = ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_STATIC, (void **)(&(pMe->m_pIStatic)))) != SUCCESS)
  {
    DBGPRINTF("UNIBREW: ERR%d, failed to create IStatic instance!", nErr); 
    return;
  }
  
  //SETAEERECT(&rect, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen);
    SETAEERECT(&rect, 2, 2, pMe->DeviceInfo.cxScreen - 4, pMe->DeviceInfo.cyScreen - BOTTOM_TEXT_HEIGHT);
  
  IMENUCTL_SetActive(pMe->m_pIMainMenu, FALSE);
  IDISPLAY_ClearScreen(pMe->m_pIDisplay);
  
  
  ISTATIC_SetRect(pMe->m_pIStatic, &rect);
  if (ISTATIC_SetText(pMe->m_pIStatic, pMe->m_szTitle, pMe->m_pszText, AEE_FONT_BOLD, AEE_FONT_NORMAL) == FALSE)
    {
        ISTATIC_Release (pMe->m_pIStatic);
        pMe->m_pIStatic = NULL;
        return;
    }

    ISTATIC_Redraw(pMe->m_pIStatic);

    IMENUCTL_SetActive(pMe->m_pISoftMenu,TRUE);
    IDISPLAY_Update(pMe->m_pIDisplay);

}

static void     mybattery_EventProcess (void *pUserData, ModelEvent *pEvent)
{
    char szBuf[80];
    AECHAR wszText[80];
    mybattery *pMe=(mybattery *)pUserData;
    uint32 uIsExtPowerExist;

    if(pMe==NULL)
    {
        return;
    }
    
    switch (pEvent->evCode) 
    {
        case EVT_MDL_BATTERY_EXTPWR_CHANGE:
               DBGPRINTF("EVT_MDL_BATTERY_EXTPWR_CHANGE");
               uIsExtPowerExist = (uint32 )pEvent->dwParam;

               SPRINTF(szBuf, "IModel ext pow: %d \n", uIsExtPowerExist);
               STRTOWSTR(szBuf, wszText, sizeof(wszText));
               WSTRCAT(pMe->m_pszText, wszText);
               DisplayText(pMe);

            break;

        default:
            break;

    }
    
    return;
}



