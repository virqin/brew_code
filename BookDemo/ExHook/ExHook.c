/*===========================================================================

FILE: ExHook.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEHtmlviewer.h"
#include "AEEFile.h"
#include "AEEStdLib.h"
#include "AEEBitmap.h"


#include "ExHook.bid"
typedef void (*PDisplayUpdate)(IDisplay *pDisplay,boolean bDefer);

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
typedef struct _ExHook {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface

    // add your own variables here...

	IHtmlViewer *pHtmlViewer;
	
	IDisplayVtbl* pIDisplayVtbl;
	IDisplayVtbl* pIDisplayOldVtbl;
	PDisplayUpdate systemUpdate;


} ExHook;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean ExHook_HandleEvent(ExHook* pMe, 
                                                   AEEEvent eCode, uint16 wParam, 
                                                   uint32 dwParam);
boolean ExHook_InitAppData(ExHook* pMe);
void    ExHook_FreeAppData(ExHook* pMe);
void ExHook_RestoreDisplayVtbl(ExHook *pMe);
void MyIDISPALY_UpdateEx(IDisplay *pDisplay,boolean bDefer);
void ExHook_ReplaceDisplayVtbl(ExHook *pMe);


/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;

	if( ClsId == AEECLSID_EXHOOK )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(ExHook),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)ExHook_HandleEvent,
                          (PFNFREEAPPDATA)ExHook_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(ExHook_InitAppData((ExHook*)*ppObj))
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
static boolean ExHook_HandleEvent(ExHook* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  

	//IHTMLVIEWER_HandleEvent(pMe->pHtmlViewer,eCode,wParam,dwParam);

    switch (eCode) 
	{
        // App is told it is starting up
        case EVT_APP_START:                        
					// Add your code here...
					//ExHook_ReplaceDisplayVtbl(pMe);
					//
					//
					//IDISPLAY_Update(pMe->pIDisplay);

					//ExHook_ReplaceDisplayVtbl(pMe);
					//IHTMLVIEWER_Redraw(pMe->pHtmlViewer);
					//IHTMLVIEWER_SetActive(pMe->pHtmlViewer,TRUE);
					//IDISPLAY_Update(pMe->pIDisplay);
						//ExHook_ReplaceDisplayVtbl(pMe);
						//IDISPLAY_Update(pMe->pIDisplay);
					ExHook_ReplaceDisplayVtbl(pMe);

					return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...

      		return(TRUE);


        // App is being suspended 
        case EVT_APP_SUSPEND:
		    // Add your code here...

      		return(TRUE);
				case EVT_KEY_PRESS:
					{
						switch (wParam)
						{
						case AVK_1:
							ExHook_ReplaceDisplayVtbl(pMe);
							IHTMLVIEWER_Redraw(pMe->pHtmlViewer);	
							IDISPLAY_Update(pMe->pIDisplay);
							return TRUE;
						case AVK_2:
							ExHook_RestoreDisplayVtbl(pMe);
							IHTMLVIEWER_Redraw(pMe->pHtmlViewer);
							IDISPLAY_Update(pMe->pIDisplay);
							return TRUE;
						}
					}
				break;
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
boolean ExHook_InitAppData(ExHook* pMe)
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
		{
			AEERect rect;
			IFileMgr *pFileMgr;
			IFile *pFile;
			FileInfo info;
			char *pbBuffer;


			ISHELL_CreateInstance(pMe->pIShell,AEECLSID_HTML,(void**)&pMe->pHtmlViewer);
			ISHELL_CreateInstance(pMe->pIShell,AEECLSID_FILEMGR,(void**)&pFileMgr);

			pFile = IFILEMGR_OpenFile(pFileMgr,"test.htm",_OFM_READ);
			IFILE_GetInfo(pFile,&info);
			pbBuffer = (char*)MALLOC(info.dwSize);

			IFILE_Read(pFile,pbBuffer,info.dwSize);
			IFILE_Release(pFile);
			IFILEMGR_Release(pFileMgr);
			IHTMLVIEWER_SetData(pMe->pHtmlViewer,pbBuffer,info.dwSize);
			FREE(pbBuffer);
			pbBuffer = NULL;
			rect.x = 0;
			rect.y = 0;
			rect.dx = pMe->DeviceInfo.cxScreen;
			rect.dy = pMe->DeviceInfo.cyScreen;
			IHTMLVIEWER_SetRect(pMe->pHtmlViewer, &rect ); 
			IHTMLVIEWER_SetProperties(pMe->pHtmlViewer,HVP_SCROLLBAR);

		}

    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void ExHook_FreeAppData(ExHook* pMe)
{
	IHTMLVIEWER_Release(pMe->pHtmlViewer);

	ExHook_RestoreDisplayVtbl(pMe);
	if(pMe->pIDisplayVtbl)
	{
		FREE(pMe->pIDisplayVtbl);
	}

}
void ExHook_ReplaceDisplayVtbl(ExHook *pMe)
{
	if(pMe->pIDisplayVtbl!=NULL)
	{
		*(( IDisplayVtbl**)pMe->pIDisplay) = pMe->pIDisplayVtbl;
	}
	else
	{
	pMe->pIDisplayVtbl = (IDisplayVtbl*)MALLOC(sizeof(IDisplayVtbl));
	MEMCPY(pMe->pIDisplayVtbl, *(( IDisplayVtbl**)pMe->pIDisplay), sizeof(IDisplayVtbl));
	pMe->systemUpdate = pMe->pIDisplayVtbl->Update;
	//Ìæ»»
	pMe->pIDisplayVtbl->Update = MyIDISPALY_UpdateEx;

	if(pMe->pIDisplayOldVtbl==NULL)
	{
		pMe->pIDisplayOldVtbl = *(( IDisplayVtbl**)pMe->pIDisplay);
	}
	*(( IDisplayVtbl**)pMe->pIDisplay) = pMe->pIDisplayVtbl;
	}
}
void ExHook_RestoreDisplayVtbl(ExHook *pMe)
{
	if(pMe->pIDisplayOldVtbl)
	{
		*(( IDisplayVtbl**)pMe->pIDisplay) = pMe->pIDisplayOldVtbl;
	}
}

static void MyIDISPALY_UpdateEx(IDisplay *po,boolean bDefer)
{
	
	IBitmap* pScrBitmap;
	IImage* pBgImage;
	IBitmap *pBgBitmap;
	NativeColor color=MAKE_RGB(255,255,255);
	NativeColor oldColor;
	AEEBitmapInfo info;
	int i;
	ExHook *pMe = (ExHook*)GETAPPINSTANCE();
	IDISPLAY_GetDeviceBitmap(pD,&pScrBitmap);
	IBITMAP_GetInfo(pScrBitmap,&info,sizeof(info));
	IBITMAP_CreateCompatibleBitmap(pScrBitmap, &pBgBitmap,info.cx,info.cy);
	IDISPLAY_SetDestination(pD, pBgBitmap);
	pBgImage = ISHELL_LoadImage(pMe->pIShell,"bg.png");
	IIMAGE_Draw(pBgImage,0,0);
	IIMAGE_Release(pBgImage);
	IBITMAP_GetTransparencyColor(pScrBitmap, &oldColor);
	color=color>>(32-info.nDepth);
	IBITMAP_SetTransparencyColor(pScrBitmap, color);
	IBITMAP_BltIn(pBgBitmap,0,0,info.cx,info.cy,pScrBitmap,0,0,AEE_RO_TRANSPARENT);
	IBITMAP_BltIn(pScrBitmap,0,0,info.cx,info.cy,pBgBitmap,0,0,AEE_RO_COPY);
	IBITMAP_SetTransparencyColor(pScrBitmap, oldColor);
	IDISPLAY_SetDestination(pD, pScrBitmap);
	IBITMAP_Release(pBgBitmap);
	IBITMAP_Release(pScrBitmap);	
	(pMe->systemUpdate)(pD, bDefer);	

}