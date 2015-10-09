/*===========================================================================

FILE: ExTAPI.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions

#include "AEETapi.h"			// TAPI Interface definitions
#include "AEEStdLib.h"

#include "ExTAPI.bid"
#include "mainapp.bid"

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
typedef struct _ExTAPI {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
	AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
	IDisplay      *pIDisplay;  // give a standard way to access the Display interface
	IShell        *pIShell;    // give a standard way to access the Shell interface
	ITAPI *pITAPI;
	// add your own variables here...



} ExTAPI;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean ExTAPI_HandleEvent(ExTAPI* pMe, 
																	 AEEEvent eCode, uint16 wParam, 
																	 uint32 dwParam);
boolean ExTAPI_InitAppData(ExTAPI* pMe);
void    ExTAPI_FreeAppData(ExTAPI* pMe);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;

	if( ClsId == AEECLSID_EXTAPI )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(ExTAPI),
			ClsId,
			pIShell,
			po,
			(IApplet**)ppObj,
			(AEEHANDLER)ExTAPI_HandleEvent,
			(PFNFREEAPPDATA)ExTAPI_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function

		{
			//Initialize applet data, this is called before sending EVT_APP_START
			// to the HandleEvent function
			if(ExTAPI_InitAppData((ExTAPI*)*ppObj))
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
===========================================================================*/
static void CBTAPI(void *pData)
{
	int a;
	a = 10;
	//ITAPI_MakeVoiceCall(((ExTAPI*)pData)->pITAPI,"18581234567",AEECLSID_EXTAPI);
}

static  void CBSMS(void  *pUser,int16 status)
{
}
static boolean ExTAPI_HandleEvent(ExTAPI* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	switch (eCode) 
	{
		// App is told it is starting up
	case EVT_APP_START:                        
		// Add your code here...
		{
			TAPIStatus status;
			int result;

			//					ITAPI_GetStatus(pMe->pITAPI,&status); 

			//OCS_IDLE  注册呼叫结束的通知，此通知有问题，不能注册，否则一旦注册就会调用回调函数
			ISHELL_CreateInstance(pMe->pIShell,AEECLSID_TAPI,(void**)&pMe->pITAPI);
			result = ITAPI_OnCallStatus(pMe->pITAPI,CBTAPI,pMe,2,OCS_IDLE);  //不能同时调用
			//result = ITAPI_MakeVoiceCall(pMe->pITAPI,"18581234567",AEECLSID_EXTAPI);//与上面的函数不能同时调用
	
		//	}			
			//ITAPI_GetStatus(pMe->pITAPI,&status);
		}
		return(TRUE);


		// App is told it is exiting
	case EVT_APP_STOP:
		// Add your code here...

		return(TRUE);


	case EVT_DIALOG_END:
		{
			int a;
			a = 10;
		}
		break;

	case EVT_APP_SUSPEND:
		{
			int a;
			a = 10;
		}
		return TRUE;
		// App is being resumed
	case EVT_APP_RESUME:
		// Add your code here...
		{

			int a;
			a = 10;

		}

		return(TRUE);

	case EVT_KEY:
		{
	

		}
		return(TRUE);

	case EVT_KEY_PRESS:
		{
			switch (wParam)
			{
			case AVK_1:
						ISHELL_CreateInstance(pMe->pIShell,AEECLSID_TAPI,(void**)&pMe->pITAPI);
			ITAPI_MakeVoiceCall(pMe->pITAPI,"18581234567",AEECLSID_EXTAPI);//与上面的函数不能同时调用
				break;
			}
		}
		return TRUE;

	default:
		break;
	}

	return FALSE;
}


// this function is called when your application is starting up
boolean ExTAPI_InitAppData(ExTAPI* pMe)
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
void ExTAPI_FreeAppData(ExTAPI* pMe)
{
	//ITAPI_Release(pMe->pITAPI);
	//pMe->pITAPI = NULL;
}
