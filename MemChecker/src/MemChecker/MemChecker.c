/*===========================================================================

FILE: MemChecker.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEFile.h"			// File interface definitions
#include "AEEStdLib.h"
#include "AEEMenu.h"
#include "AEESource.h"
#include "AEEPointerHelpers.h"

#include "ClassList.h"
#include "IDesktopWindow.h"
#include "ListBox_IFace.h"
#include "StaticCtl_IFace.h"
#include "WidgetEvent.h"

#include "INIFileParser.h"
#include "MemCheckerUtil.h"
#include "MemHook.h"

#include "MemChecker.bid"
#include "MemChecker.brh"

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _MemChecker {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information

    IClassFactory *m_pIClassFactory;
    IDesktopWindow*m_pIDesktopWindow;

    AEECLSID       m_clsCheckedApp;
    AEECallback    m_cbWaitForAppStartup;
    AEECallback    m_cbModUnload;

    EOutputMode    m_eOutputMode;

    boolean        m_bBackground; // background app

    IAppletCtl    *m_pIAppletCtl;

} MemChecker;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean MemChecker_HandleEvent(MemChecker* pMe, AEEEvent eCode, 
                                             uint16 wParam, uint32 dwParam);
boolean MemChecker_InitAppData(MemChecker* pMe);
void    MemChecker_FreeAppData(MemChecker* pMe);
static int MemChecker_CreateMainMenu(MemChecker* pMe);

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

	if( ClsId != 0/*ClsId == AEECLSID_MEMCHECKER*/ )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(MemChecker),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)MemChecker_HandleEvent,
                          (PFNFREEAPPDATA)MemChecker_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(MemChecker_InitAppData((MemChecker*)*ppObj))
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

void MemChecker_ParseLineCB(void *pUser, const char *pszKey, int nKeyLen, const char *pszValue, int nValueLen)
{
    MemChecker *pMe = (MemChecker *)pUser;
    char *pszValueCopy = NULL;

    pszValueCopy = MALLOC(nValueLen + 1);
    if (NULL == pszValueCopy) {
        return;
    }
    MEMCPY(pszValueCopy, pszValue, nValueLen);

    if (0 == STRNCMP("ClassID", pszKey, nKeyLen))
    {
        if (STRBEGINS("0x", pszValueCopy)) {
            pMe->m_clsCheckedApp = STRTOUL(pszValueCopy, NULL, 16);
        } else {
            pMe->m_clsCheckedApp = ATOI(pszValueCopy);
        }
    }
    else if (0 == STRNCMP("OutputTo", pszKey, nKeyLen))
    {
        pMe->m_eOutputMode = ATOI(pszValueCopy);
    }

    FREEIF(pszValueCopy);
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
static boolean MemChecker_HandleEvent(MemChecker* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
#if 1
    // for debugging
    if (EVT_POINTER_DOWN == eCode)
    {
        AEERect rc;
        int x, y;
        AEE_POINTER_GET_XY((char *)dwParam, &x, &y);
        SETAEERECT(&rc, x, y, 2, 2);
        IDisplay_FillRect(pMe->a.m_pIDisplay, &rc, RGB_BLACK);
        IDisplay_Update(pMe->a.m_pIDisplay);
    }
#endif

    if (pMe->m_pIDesktopWindow 
        && IDesktopWindow_HandleEvent(pMe->m_pIDesktopWindow, eCode, wParam, dwParam))
    {
        return TRUE;
    }

    switch (eCode) 
    {
        // App is told it is starting up
    case EVT_APP_START:
        DBGPRINTF("##MemChecker_HandleEvent: EVT_APP_START ##############");

        if (pMe->m_bBackground)
        {
            pMe->m_bBackground = FALSE;
            IContainer_Invalidate((IContainer *)pMe->m_pIDesktopWindow, NULL, NULL);
            return TRUE;
        }

        ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_APPLETCTL, (void **)&pMe->m_pIAppletCtl);
        IniFileParser_LoadFile(pMe->a.m_pIShell, MemChecker_ParseLineCB, pMe);

        // ClassFactory & DestopWindow
        pMe->m_pIClassFactory = XClassFactory_New();
        IClassFactory_CreateInstance(pMe->m_pIClassFactory, XCLSID_DesktopWindow, (void **)&pMe->m_pIDesktopWindow);
        IWidget_SetName((IWidget*)pMe->m_pIDesktopWindow, "DesktopWindow");
        IDesktopWindow_SetDisplay(pMe->m_pIDesktopWindow, pMe->a.m_pIDisplay);
        IWidget_SetRectEx((IWidget*)pMe->m_pIDesktopWindow, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen);

        MemChecker_CreateMainMenu(pMe);

        return(TRUE);


    // App is told it is exiting
    case EVT_APP_STOP:
        DBGPRINTF("##MemChecker_HandleEvent: EVT_APP_STOP ###################");

        if (pMe->m_bBackground)
        {
            *(boolean *)dwParam = FALSE;
            return TRUE;
        }

        CALLBACK_Cancel(&pMe->m_cbModUnload);
        CALLBACK_Cancel(&pMe->m_cbWaitForAppStartup);

        MemHook_Uninstall();

        RELEASEIF(pMe->m_pIClassFactory);
        RELEASEIF(pMe->m_pIDesktopWindow);

        RELEASEIF(pMe->m_pIAppletCtl);

        return(TRUE);


        // App is being suspended 
    case EVT_APP_SUSPEND:
        DBGPRINTF("##MemChecker_HandleEvent: EVT_APP_SUSPEND ################");

        return(TRUE);


        // App is being resumed
    case EVT_APP_RESUME:
        DBGPRINTF("##MemChecker_HandleEvent: EVT_APP_RESUME #################");
        IContainer_Invalidate((IContainer *)pMe->m_pIDesktopWindow, NULL, NULL);

        return(TRUE);


        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
    case EVT_APP_MESSAGE:
        // Add your code here...

        return(TRUE);


        // If nothing fits up to this point then we'll just break out
    default:
        break;
    }

    return FALSE;
}

// this function is called when your application is starting up
boolean MemChecker_InitAppData(MemChecker* pMe)
{
    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void MemChecker_FreeAppData(MemChecker* pMe)
{
    // insert your code here for freeing any resources you have allocated...
    DBGPRINTF("##MemChecker_FreeAppData######################################");
}

void MemChecker_WaitForAppStop(MemChecker* pMe)
{
    int nErr = SUCCESS;
    uint32 dwAppState = (uint32)-1;

    nErr = GetAppRunningState(pMe->m_pIAppletCtl, pMe->m_clsCheckedApp, &dwAppState);

    if (SUCCESS != nErr)
    {
        MemHook_Uninstall();
        pMe->m_bBackground = FALSE; // Close really
        ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
    }
    else
    {
        CALLBACK_Init(&pMe->m_cbModUnload, MemChecker_WaitForAppStop, pMe);
        ISHELL_OnModUnload(pMe->a.m_pIShell, &pMe->m_cbModUnload);
    }
}

static void MemChecker_WaitForAppStartup(MemChecker* pMe)
{
    int nErr = SUCCESS;
    uint32 dwAppState = (uint32)-1;

    nErr = GetAppRunningState(pMe->m_pIAppletCtl, pMe->m_clsCheckedApp, &dwAppState);
    if (APPSTATE_TOP_VISIBLE == dwAppState || APPSTATE_BACKGROUND == dwAppState)
    {
        MemChecker_WaitForAppStop(pMe);
    }
    else
    {
        CALLBACK_Init(&pMe->m_cbWaitForAppStartup, MemChecker_WaitForAppStartup, pMe);
        ISHELL_Resume(pMe->a.m_pIShell, &pMe->m_cbWaitForAppStartup);
    }
}

static void MemChecker_Button_Listener(MemChecker *pMe, XEvent *pEvent)
{
    if (XEVT_COMMAND == pEvent->nCode)
    {
        switch(pEvent->dwParam)
        {
        case IDS_BEGIN_WATCH:
        case IDS_RUN_AND_WATCH:
            {
                int nErr = SUCCESS;
                MemHookInitParam sParam = {0};
                AEEAppInfo ai = {0};

                if (ISHELL_QueryClass(pMe->a.m_pIShell, pMe->m_clsCheckedApp, &ai)) {
                    sParam.pIShell = pMe->a.m_pIShell;
                    sParam.clsApp = pMe->m_clsCheckedApp;
                    sParam.eOutputMode = pMe->m_eOutputMode;
                    MemHook_Install(&sParam);

                    if (IDS_RUN_AND_WATCH == pEvent->dwParam) {
                        nErr = ISHELL_StartApplet(pMe->a.m_pIShell, pMe->m_clsCheckedApp);
                    }
                } else {
                    nErr = ECLASSNOTSUPPORT;
                }

                if (SUCCESS == nErr)
                {
                    pMe->m_bBackground = TRUE; // Go background
                    ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
                    MemChecker_WaitForAppStartup(pMe);
                }
                else // Failed
                {
                    MemHook_Uninstall();

                    if (IDS_BEGIN_WATCH == pEvent->dwParam) {
                        XMessageBox(
                            L"Please set a valid applet class id in \"config.ini\".", 
                            L"Error");
                    } else {
                        XMessageBox(
                            L"Can not start the applet. "
                            L"Please set a valid applet class id in \"config.ini\".", 
                            L"Error");
                    }
                }
            }

            break;
        case IDS_ABOUT:
            XMessageBox(
                L"BREW Memory Leak Checker\n\n"
                L"Version: 1.0.1\n\n"
                L"Copyright 2010-2012 by DXJ. All Rights Reserved.\n\n"
                L"Email: kindlywolf@gmail.com", 
                L"About me");
            break;

        default:
            break;
        }
    }
}

static IWidget* MemChecker_CreateButton(MemChecker* pMe, const char* pszName, int x, int y, int dx, int dy, const AECHAR* pszText)
{
    IWidget *piwStatic = NULL;
    IWidget *piButton = NULL;
    IImage *piImage = NULL;
    IBitmap *piMaskBitmap = NULL;
    NativeColor ncTrans;

    IClassFactory_CreateInstance(pMe->m_pIClassFactory, XCLSID_Button, (void**)&piButton);
    IWidget_SetName(piButton, pszName);
    IWidget_SetPosition(piButton, x, y);
    IWidget_SetSize(piButton, dx, dy);
    IWidget_SetBorderWidth(piButton, 0);
    IWidget_SetBGColor(piButton, RGB_NONE);
    piImage = ISHELL_LoadResImage(pMe->a.m_pIShell, MEMCHECKER_RES_FILE, IDI_BUTTON_BG);
    IWidget_SetBGImage(piButton, piImage);
    RELEASEIF(piImage);
    piImage = ISHELL_LoadResImage(pMe->a.m_pIShell, MEMCHECKER_RES_FILE, IDI_BUTTON_BG_A);
    IWidget_SetProperty(piButton, PROP_ACTIVE_BGIMAGE, piImage);
    RELEASEIF(piImage);
    piImage = ISHELL_LoadResImage(pMe->a.m_pIShell, MEMCHECKER_RES_FILE, IDI_BUTTON_BG_S);
    IWidget_SetProperty(piButton, PROP_SELECTED_ACTIVE_BGIMAGE, piImage);
    RELEASEIF(piImage);
    piMaskBitmap = ISHELL_LoadResBitmap(pMe->a.m_pIShell, MEMCHECKER_RES_FILE, IDI_BUTTON_MASK);
    ncTrans = IBitmap_RGBToNative(piMaskBitmap, RGB_BLACK);
    IBitmap_SetTransparencyColor(piMaskBitmap, ncTrans);
    IWidget_SetMaskBitmap(piButton, piMaskBitmap);
    RELEASEIF(piMaskBitmap);
    {
        IClassFactory_CreateInstance(pMe->m_pIClassFactory, XCLSID_StaticCtl, (void **)&piwStatic);
        IWidget_SetSize(piwStatic, dx, dy);
        IWidget_SetBorderWidth(piwStatic, 0);
        IWidget_SetBGColor(piwStatic, RGB_NONE);
        IWidget_SetFGColor(piwStatic, RGB_WHITE);
        IWidget_SetText(piwStatic, pszText, TRUE);
        IContainer_Insert((IContainer*)piButton, piwStatic, -1, NULL);
        RELEASEIF(piwStatic);
    }

    return piButton;
}

static int MemChecker_CreateMainMenu(MemChecker* pMe)
{
    IWindow *piWindow = NULL;
    IWidget *piWidget = NULL;
    AECHAR *pszText = NULL;

    IClassFactory_CreateInstance(pMe->m_pIClassFactory, XCLSID_Window, (void **)&piWindow);
    IWidget_SetName((IWidget*)piWindow, "Window1");
    IWindow_SetSize(piWindow, 240, 320);
    IDesktopWindow_AddWindow(pMe->m_pIDesktopWindow, piWindow);
    IWidget_Release((IWidget*)piWindow);

    ISHELL_LoadResStringEx(pMe->a.m_pIShell, MEMCHECKER_RES_FILE, IDS_BEGIN_WATCH, &pszText);
    piWidget = MemChecker_CreateButton(pMe, "Button1", 0, 0, 96, 84, pszText);
    if (NULL != piWidget)
    {
        Listener* pListener = MALLOCREC(Listener);
        Listener_Init(pListener, MemChecker_Button_Listener, pMe);
        IWidget_AddListener(piWidget, pListener);

        IWidget_SetID(piWidget, IDS_BEGIN_WATCH);
        IWindow_Insert(piWindow, piWidget, -1, NULL);
        RELEASEIF(piWidget);
    }

    ISHELL_LoadResStringEx(pMe->a.m_pIShell, MEMCHECKER_RES_FILE, IDS_RUN_AND_WATCH, &pszText);
    piWidget = MemChecker_CreateButton(pMe, "Button2", 72, 42, 96, 84, pszText);
    if (NULL != piWidget)
    {
        Listener* pListener = MALLOCREC(Listener);
        Listener_Init(pListener, MemChecker_Button_Listener, pMe);
        IWidget_AddListener(piWidget, pListener);

        IWidget_SetID(piWidget, IDS_RUN_AND_WATCH);
        IWindow_Insert(piWindow, piWidget, -1, NULL);
        RELEASEIF(piWidget);
    }

    ISHELL_LoadResStringEx(pMe->a.m_pIShell, MEMCHECKER_RES_FILE, IDS_ABOUT, &pszText);
    piWidget = MemChecker_CreateButton(pMe, "Button3", 144, 0, 96, 84, pszText);
    if (NULL != piWidget)
    {
        Listener* pListener = MALLOCREC(Listener);
        Listener_Init(pListener, MemChecker_Button_Listener, pMe);
        IWidget_AddListener(piWidget, pListener);

        IWidget_SetID(piWidget, IDS_ABOUT);
        IWindow_Insert(piWindow, piWidget, -1, NULL);
        RELEASEIF(piWidget);
    }

    piWidget = MemChecker_CreateButton(pMe, NULL, 0, 84, 96, 84, WSTRDUP(L"Unimplemented"));
    if (NULL != piWidget)
    {
        IWindow_Insert(piWindow, piWidget, -1, NULL);
        RELEASEIF(piWidget);
    }
    piWidget = MemChecker_CreateButton(pMe, NULL, 72, 126, 96, 84, WSTRDUP(L"Unimplemented"));
    if (NULL != piWidget)
    {
        IWindow_Insert(piWindow, piWidget, -1, NULL);
        RELEASEIF(piWidget);
    }
    piWidget = MemChecker_CreateButton(pMe, NULL, 144, 84, 96, 84, WSTRDUP(L"Unimplemented"));
    if (NULL != piWidget)
    {
        IWindow_Insert(piWindow, piWidget, -1, NULL);
        RELEASEIF(piWidget);
    }
    piWidget = MemChecker_CreateButton(pMe, NULL, 0, 168, 96, 84, WSTRDUP(L"Unimplemented"));
    if (NULL != piWidget)
    {
        IWindow_Insert(piWindow, piWidget, -1, NULL);
        RELEASEIF(piWidget);
    }
    piWidget = MemChecker_CreateButton(pMe, NULL, 72, 210, 96, 84, WSTRDUP(L"Unimplemented"));
    if (NULL != piWidget)
    {
        IWindow_Insert(piWindow, piWidget, -1, NULL);
        RELEASEIF(piWidget);
    }
    piWidget = MemChecker_CreateButton(pMe, NULL, 144, 168, 96, 84, WSTRDUP(L"Unimplemented"));
    if (NULL != piWidget)
    {
        IWindow_Insert(piWindow, piWidget, -1, NULL);
        RELEASEIF(piWidget);
    }

    IContainer_Invalidate((IContainer *)pMe->m_pIDesktopWindow, NULL, NULL);

    return SUCCESS;
}
