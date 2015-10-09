/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

/*===========================================================================

FILE: helloformapp.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */

#include "AEE.h"              // Standard AEE Declarations
#include "AEEShell.h"         // AEE Shell Services
#include "AEEFont.h"          // AEE Font Services
#include "AEEDisp.h"          // AEE Display Services
#include "AEEStdLib.h"        // AEE StdLib Services
#include "XUtil.h"
#include "ClassList.h"
#include "IDesktopWindow.h"
#include "StaticCtl_IFace.h"
#include "ListBox_IFace.h"
#include "WidgetEvent.h"

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "me->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _helloformapp {

	IApplet              ia;   
	IAppletVtbl          iavt;  	
	int                  nRefs;
    IDisplay             *pIDisplay;  // give a standard way to access the Display interface
    IShell               *pIShell;    // give a standard way to access the Shell interface
	IModule				 *pIModule;

    IClassFactory        *m_pIClassFactory;
    IDesktopWindow       *m_pIDesktopWindow;
} helloformapp;

/*-------------------------------------------------------------------
Function prototypes
-------------------------------------------------------------------*/
static uint32  helloformapp_AddRef (IApplet *po);
static uint32  helloformapp_Release (IApplet *po);
static  boolean  helloformapp_HandleEvent (IApplet *pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);

static boolean helloformapp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
   helloformapp *me = (helloformapp *)pi;

   switch (eCode) {
      // App is told it is starting up
      case EVT_APP_START:                        
         // Add your code here...
	      return(TRUE);

      // App is told it is exiting
      case EVT_APP_STOP:
         // Add your code here...
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
         switch (wParam) {
	         // UP
	         case AVK_UP:
		         return TRUE;

	         // DOWN
	         case AVK_DOWN:
		         return TRUE;

	         // LEFT
	         case AVK_LEFT:
		         return TRUE;

	         // RIGHT
	         case AVK_RIGHT:
		         return TRUE;

	         // SELECT
	         case AVK_SELECT:
		         return TRUE;

	         // CLEAR
	         case AVK_CLR:
		         return TRUE;

            default:
               break;
	      }

	      return (TRUE);

      // If nothing fits up to this point then we'll just break out
      default:
         break;
   }

   return FALSE;
}

// Destroy samplehtmlwidgetapp object
static void helloformapp_Dtor(helloformapp *me)
{
	RELEASEIF(me->pIDisplay);
	RELEASEIF(me->pIModule);
	RELEASEIF(me->pIShell);
}

static void helloformapp_MainMenu_Listener(helloformapp *pMe, XEvent *pEvent)
{
    switch (pEvent->nCode)
    {
    case XEVT_FOCUS_SELECT:
        DBGPRINTF("#helloformapp_MainMenu_Listener#");
        break;
    }
}

static int helloformapp_CreateMainMenu(helloformapp *pMe)
{
    IWindow *piWindow = NULL;
    IContainer* piContainer = NULL;
    IWidget *piWidget = NULL;

    IClassFactory_CreateInstance(pMe->m_pIClassFactory, XCLSID_Window, (void **)&piWindow);
    IWidget_SetName((IWidget*)piWindow, "Window1");
    IWindow_SetPosition(piWindow, 20, 20);
    IWindow_SetSize(piWindow, 210, 298);
    IDesktopWindow_AddWindow(pMe->m_pIDesktopWindow, piWindow);
    IWidget_Release((IWidget*)piWindow);

    IClassFactory_CreateInstance(pMe->m_pIClassFactory, XCLSID_Container, (void **)&piContainer);
    IWidget_SetName((IWidget*)piContainer, "Container1");
    IWidget_SetPosition((IWidget*)piContainer, 80, 0);
    IWidget_SetSize((IWidget*)piContainer, 120, 200);
    IWindow_Insert(piWindow, (IWidget*)piContainer, -1, NULL);
    IWidget_Release(ICONTAINER_TO_IWIDGET(piContainer));

    IClassFactory_CreateInstance(pMe->m_pIClassFactory, XCLSID_StaticCtl, (void **)&piWidget);
    IWidget_SetName(piWidget, "StaticCtl1");
    IWidget_SetPosition(piWidget, 5, 200);
    IWidget_SetSize(piWidget, 80, 30);
    IWidget_SetText(piWidget, L"Hello, StaticCtl", FALSE);
    IWindow_Insert(piWindow, piWidget, -1, NULL);
    RELEASEIF(piWidget);

    IClassFactory_CreateInstance(pMe->m_pIClassFactory, XCLSID_ListBox, (void **)&piWidget);
    IWidget_SetName(piWidget, "ListBox1");
    IWidget_SetSize(piWidget, 100, 156);
    IWidget_ListData_SetPfnFree(piWidget, FREE);
    {
        IWidget_ListData_AddItem(piWidget, WSTRDUP(L"List Item0"));
        IWidget_ListData_AddItem(piWidget, WSTRDUP(L"List Item1"));
        IWidget_ListData_AddItem(piWidget, WSTRDUP(L"List Item2"));
        IWidget_ListData_AddItem(piWidget, WSTRDUP(L"List Item3"));
        IWidget_ListData_AddItem(piWidget, WSTRDUP(L"List Item4"));
    }
    IWindow_Insert(piWindow, piWidget, -1, NULL);
    {
        Listener *pListener = MALLOCREC(Listener);
        Listener_Init(pListener, helloformapp_MainMenu_Listener, pMe);
        IWidget_AddListener(piWidget, pListener);
    }
    RELEASEIF(piWidget);

    IClassFactory_CreateInstance(pMe->m_pIClassFactory, XCLSID_Button, (void **)&piWidget);
    IWidget_SetName(piWidget, "Button1");
    IWidget_SetRectEx(piWidget, 10, 10, 80, 40);
    IContainer_Insert(piContainer, piWidget, -1, NULL);
    RELEASEIF(piWidget);

    IClassFactory_CreateInstance(pMe->m_pIClassFactory, XCLSID_ScrollBar, (void **)&piWidget);
    IWidget_SetName(piWidget, "ScrollBar1");
    IWidget_SetRectEx(piWidget, 100, 5, 32, 204);
    IContainer_Insert(piContainer, piWidget, -1, NULL);
    RELEASEIF(piWidget);

    IContainer_Invalidate((IContainer *)pMe->m_pIDesktopWindow, NULL, NULL);

    return SUCCESS;
}

static int helloformapp_Construct(helloformapp *me, IModule * piModule, IShell * piShell)
{
   me->ia.pvt           = &me->iavt;
   me->iavt.AddRef      = helloformapp_AddRef;
   me->iavt.Release     = helloformapp_Release;
   me->iavt.HandleEvent = helloformapp_HandleEvent;

   me->nRefs = 1;

   me->pIShell = piShell;
   ISHELL_AddRef(piShell);

   me->pIModule = piModule;
   IMODULE_AddRef(piModule);
   
   return helloformapp_CreateMainMenu(me);   
}

static uint32 helloformapp_AddRef(IApplet *po)
{
   helloformapp *me = (helloformapp *)po;
   return (uint32)(++me->nRefs);
}


static uint32 helloformapp_Release(IApplet *po)
{
   helloformapp *me = (helloformapp *)po;
   uint32 nRefs = (uint32)(--me->nRefs);
   if (nRefs == 0) {
      helloformapp_Dtor(me);
      FREE(me);
   }
   return nRefs;
}


// #include "xmod.h"

int xModule_CreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp)
{
    helloformapp *me = MALLOCREC(helloformapp);

    if (me == 0) {
        return ENOMEMORY;
    } else {
        *pp = &me->ia;
        return helloformapp_Construct(me, module, shell);
    }

    *pp = 0;
    return ECLASSNOTSUPPORT;
}
