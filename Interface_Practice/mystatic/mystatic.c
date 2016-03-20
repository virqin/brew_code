/*===========================================================================

FILE: mystatic.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions

#include "mystatic.bid"
#include "AEEMenu.h"
#include "mystatic_res.h"

#define ID_TEXT            1
#define ID_PIC             2
#define ID_NO_SCROLL       3

#define STATUS_MAIN_MENU      7
#define STATUS_TEXT           8
#define STATUS_PICTURE        9
#define STATUS_NO_SCROLL      10
/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _mystatic {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface

    // add your own variables here...
	IMenuCtl       *pMenu;
	IStatic        *pText;
	IStatic        *pPic;
	IStatic        *pNoscroll;
	uint16         status;
} mystatic;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean mystatic_HandleEvent(mystatic* pMe, AEEEvent eCode, 
                                             uint16 wParam, uint32 dwParam);
boolean mystatic_InitAppData(mystatic* pMe);
void    mystatic_FreeAppData(mystatic* pMe);
static void BuildMainMenu(mystatic * pMe);


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

	if( ClsId == AEECLSID_MYSTATIC )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(mystatic),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)mystatic_HandleEvent,
                          (PFNFREEAPPDATA)mystatic_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(mystatic_InitAppData((mystatic*)*ppObj))
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
static boolean mystatic_HandleEvent(mystatic* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	uint16  item;

    if(pMe->pMenu && IMENUCTL_HandleEvent(pMe->pMenu,eCode,wParam,dwParam) )
	    return TRUE;
	if(pMe->pNoscroll && ISTATIC_HandleEvent(pMe->pNoscroll,eCode,wParam,dwParam))
		return TRUE;

    switch (eCode) 
	{
        // App is told it is starting up
        case EVT_APP_START:                        
		    // Add your code here...
			BuildMainMenu(pMe);
            return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...

      		return(TRUE);


        // App is being suspended 
        case EVT_APP_SUSPEND:
		    // Add your code here...
			switch(pMe->status)
			{
			case STATUS_MAIN_MENU:
				IMENUCTL_Release(pMe->pMenu);
				pMe->pMenu = NULL;
				break;
			case STATUS_TEXT:
				ISTATIC_Release(pMe->pText);
				pMe->pText = NULL;
				break;
			case STATUS_PICTURE:
				ISTATIC_Release(pMe->pPic);
				pMe->pPic = NULL;
				break;
			case STATUS_NO_SCROLL:
				ISTATIC_Release(pMe->pNoscroll);
				pMe->pNoscroll = NULL;
				break;
			}

      		return(TRUE);


        // App is being resumed
        case EVT_APP_RESUME:
		    // Add your code here...
			switch(pMe->status)
			{
			case STATUS_MAIN_MENU:
				BuildMainMenu(pMe);
				break;

			}
      		return(TRUE);


        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
		    // Add your code here...

      		return(TRUE);

        // A key was pressed. Look at the wParam above to see which key was pressed. The key
        // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
        case EVT_KEY:
		    // Add your code here...
		  if((wParam == AVK_CLR) && (pMe->status != STATUS_MAIN_MENU)) 
		  {
			  if(pMe->pText)
			  {
				  ISTATIC_Release(pMe->pText);
				  pMe->pText = NULL;
			  }
			  if(pMe->pPic)
			  {
				  ISTATIC_Release(pMe->pPic);
				  pMe->pPic = NULL;
			  }
			  if(pMe->pNoscroll)
			  {
				  ISTATIC_Release(pMe->pNoscroll);
				  pMe->pNoscroll = NULL;
			  }
			  IMENUCTL_Redraw(pMe->pMenu);
			  IMENUCTL_SetActive(pMe->pMenu,TRUE);
			  pMe->status = STATUS_MAIN_MENU;
			  return TRUE;
		  }
		  else //When the user press clear key and the current page is main menu, the application should exit.
			  return FALSE;

      		return(TRUE);

		 case EVT_COMMAND:
			 item = IMENUCTL_GetSel (pMe->pMenu);
			 switch(item)
			  {
			  case ID_TEXT:

				  break;
			  case ID_PIC:

				  break;
			  case ID_NO_SCROLL:

				  break;
			  default:
				  break;
			  }
			  return TRUE;

        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}


// this function is called when your application is starting up
boolean mystatic_InitAppData(mystatic* pMe)
{
    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

    // The display and shell interfaces are always created by
    // default, so we'll asign them so that you can access
    // them via the standard "pMe->" without the "a."
    pMe->pIDisplay = pMe->a.m_pIDisplay;
    pMe->pIShell   = pMe->a.m_pIShell;

    // Insert your code here for initializing or allocating resources...
	pMe->pMenu = NULL;
	pMe->pText = NULL;
	pMe->pPic = NULL;
	pMe->pNoscroll = NULL;

    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void mystatic_FreeAppData(mystatic* pMe)
{
    // insert your code here for freeing any resources you have allocated...

    // example to use for releasing each interface:
    // if ( pMe->pIMenuCtl != NULL )         // check for NULL first
    // {
    //    IMENUCTL_Release(pMe->pIMenuCtl)   // release the interface
    //    pMe->pIMenuCtl = NULL;             // set to NULL so no problems trying to free later
    // }
    //
	if(pMe->pMenu)
	{
		IMENUCTL_Release(pMe->pMenu);
		pMe->pMenu = NULL;
	}
	if(pMe->pPic)
	{
		ISTATIC_Release(pMe->pPic);
		pMe->pPic = NULL;
	}
	if(pMe->pText)
	{
		ISTATIC_Release(pMe->pText);
		pMe->pText = NULL;
	}
	if(pMe->pNoscroll)
	{
		ISTATIC_Release(pMe->pNoscroll);
		pMe->pNoscroll = NULL;
	}
}

static void BuildMainMenu(mystatic * pMe)
{
	if(ISHELL_CreateInstance(pMe->pIShell,AEECLSID_MENUCTL,(void**)&pMe->pMenu) != SUCCESS)
		return ;

	IMENUCTL_SetTitle(pMe->pMenu,MYSTATIC_RES_FILE,IDS_TITLE,NULL);

	IMENUCTL_AddItem(pMe->pMenu,MYSTATIC_RES_FILE,IDS_TEXT,ID_TEXT,NULL,0);
	IMENUCTL_AddItem(pMe->pMenu,MYSTATIC_RES_FILE,IDS_PIC,ID_PIC,NULL,0);
	IMENUCTL_AddItem(pMe->pMenu,MYSTATIC_RES_FILE,IDS_NO_SCROLL,ID_NO_SCROLL,NULL,0);

	IMENUCTL_SetActive(pMe->pMenu,TRUE);
	pMe->status = STATUS_MAIN_MENU;
}

