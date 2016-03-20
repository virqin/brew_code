/*===========================================================================

FILE: mydisplay.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEStdLib.h"

#include "mydisplay.bid"
#include "mydisplay_res.h"

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _mydisplay {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface

    // add your own variables here...



} mydisplay;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean mydisplay_HandleEvent(mydisplay* pMe, AEEEvent eCode, 
                                             uint16 wParam, uint32 dwParam);
boolean mydisplay_InitAppData(mydisplay* pMe);
void    mydisplay_FreeAppData(mydisplay* pMe);
static  void DisplayText(mydisplay *pMe);

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

	if( ClsId == AEECLSID_MYDISPLAY )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(mydisplay),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)mydisplay_HandleEvent,
                          (PFNFREEAPPDATA)mydisplay_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(mydisplay_InitAppData((mydisplay*)*ppObj))
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
static boolean mydisplay_HandleEvent(mydisplay* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  

    switch (eCode) 
	{
        // App is told it is starting up
        case EVT_APP_START:                        
		    // Add your code here...
			DisplayText(pMe);

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
		    // Add your code here...

      		return(TRUE);


        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}


// this function is called when your application is starting up
boolean mydisplay_InitAppData(mydisplay* pMe)
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



    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void mydisplay_FreeAppData(mydisplay* pMe)
{
    // insert your code here for freeing any resources you have allocated...

    // example to use for releasing each interface:
    // if ( pMe->pIMenuCtl != NULL )         // check for NULL first
    // {
    //    IMENUCTL_Release(pMe->pIMenuCtl)   // release the interface
    //    pMe->pIMenuCtl = NULL;             // set to NULL so no problems trying to free later
    // }
    //

}

static void DisplayText(mydisplay *pMe)
{
	AECHAR  szText[100];
	AECHAR  *pCh;
	int     height;
	int     len,y;
	int     str_len;
	int     ascent,descent;

	ISHELL_LoadResString(pMe->pIShell,MYDISPLAY_RES_FILE,IDS_TEST,szText,sizeof(szText));
	IDISPLAY_MeasureTextEx(pMe->pIDisplay,AEE_FONT_BOLD,szText,-1,pMe->DeviceInfo.cxScreen,&len);
	IDISPLAY_GetFontMetrics(pMe->pIDisplay,AEE_FONT_BOLD,&ascent,&descent);
	height = ascent + descent;
	y = 0;
	str_len = WSTRLEN(szText);
	pCh = szText;
	while(str_len > 0)
	{
                if(str_len<len){
                	len=str_len;
                }
		IDISPLAY_DrawText(pMe->pIDisplay,    
					   AEE_FONT_BOLD,       
					   pCh,              
					   len,                  
					   0,                  
					   y,                   
					   NULL,              
					   0);
		IDISPLAY_Update (pMe->pIDisplay);
		y = y + height;
		str_len = str_len - len;
		pCh = pCh + len;
   	}
}