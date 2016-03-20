/*===========================================================================

  FILE: imgpros.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEFile.h"			// File interface definitions
#include "AEE.h"
#include "AEEDB.h"
#include "imgpros.h"
#include "imgpros.brh"
#include "AEEIBitmapScale.h"
#include "AEEIBitmap.h"
#include "AEEIQI.h"
#include "AEEStdLib.h"
#include "AEEInterface.h"
#include "AEEBilinearScale.bid"


#define RELEASEIF(pi)      { if (pi) { IBASE_Release((IBase*)(pi)); (pi)=0; }}

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
//static boolean CImgProsApp_HandleEvent(IApplet * pi, AEEEvent eCode, 
//                                       uint16 wParam, uint32 dwParam);

/*===============================================================================

===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
{
	*ppObj = NULL;
	if(ClsId)
	{
		if(AEEApplet_New(sizeof(CImgProsApp), 
			ClsId, 
			pIShell,po,
			(IApplet**)ppObj,
			(AEEHANDLER)CImgProsApp_HandleEvent,
			(PFNFREEAPPDATA)CImgProsApp_FreeAppData))
			
			
		{
			if (CImgProsApp_InitAppData((IApplet*)*ppObj,ClsId,pIShell)) 
			{
				return(AEE_SUCCESS);
			}
			else
			{
				IAPPLET_Release((IApplet*)ppObj);
				*ppObj = NULL;
				return EFAILED;
			}
			
			
		}
	}
	return (AEE_SUCCESS);
}




/*===========================================================================

  
===========================================================================*/
static boolean CImgProsApp_HandleEvent(CImgProsApp * pApp, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	if (pApp->m_pMenu && IMENUCTL_IsActive(pApp->m_pMenu))
	{
		if(IMENUCTL_HandleEvent(pApp->m_pMenu, eCode, wParam, dwParam))
		{
				return TRUE;
		}
	}


	switch (eCode) 
	{
	case EVT_APP_START:                        
		
		CImgProsApp_DisplayMainMenu(pApp);
	
  		return(TRUE);

	case EVT_APP_STOP:
		
		// Add your code here .....
		
		return TRUE;
		
	case EVT_KEY:
		
		if (pApp->m_pMenu && IMENUCTL_IsActive(pApp->m_pMenu))
		{
			if (wParam == AVK_CLR)
			{
				IMENUCTL_Release(pApp->m_pMenu);
				pApp->m_pMenu = NULL;
				
				CImgProsApp_DisplayMainMenu(pApp);
		              return TRUE;
			}
		}
		
		return FALSE;
		
		
	case EVT_COMMAND:
		switch(wParam)
		{
		  case IDC_IMAGE_ONE:
			CImgProsApp_DisplayImage(pApp,IDC_IMAGE_ONE);
			return TRUE;
			
		  case IDC_IBITMAPSCALE:
			CImgProsApp_DisplayImage(pApp,IDC_IBITMAPSCALE);
			return TRUE;

		  default:
		      return TRUE;
		}
		
		return TRUE;

	default:
		break;
	}
	return FALSE;
}

static boolean CImgProsApp_InitAppData(IApplet * pApp,uint32 clsid,IShell * pIShell)
{
	int ret;
	CImgProsApp* pMe = (CImgProsApp*)pApp;
	ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &pMe->m_DeviceInfo);
	pMe->m_cxWidth = pMe->m_DeviceInfo.cxScreen;
	pMe->m_cyHeight = pMe->m_DeviceInfo.cyScreen;
	SETAEERECT(&pMe->m_rScreenRect,0,0,pMe->m_cxWidth,pMe->m_cyHeight);

	if(ret = ISHELL_CreateInstance(pIShell,AEECLSID_BilinearScale,(void**)&pMe->m_iBitmapScale)!=SUCCESS) 
	{
		DBGPRINTF("Create IBitmapFx instance failed:%d", ret);
		return FALSE;
	}
	return TRUE;
}

static void CImgProsApp_DisplayImage(CImgProsApp * pApp,int number)
{
	
	int ImageSizeX;
	int ImageSizeY;
	AEEImageInfo ii;
	
	IBitmap* pSrcBitmap = NULL;
	IBitmap* pDstBitmap = NULL;
	IBitmap* pDispBitmap = NULL;
	
	int nTransOp = AEEBITMAPSCALE_OPAQUE;                 //what is the range? 
	
	SETAEERECT(&pApp->m_prcSrc,0,0,128,160);    //source bitmap rectangle
	SETAEERECT(&pApp->m_prcDst,0,0,96,120);    //destination bitmap rectangle
	SETAEERECT(&pApp->m_prcClip,0,0,96,120);   //the clip rectangle 
	
	IDISPLAY_ClearScreen(pApp->a.m_pIDisplay);
       if(!pApp->m_pImage1)
       {
	    pApp->m_pImage1 = ISHELL_LoadResImage(pApp->a.m_pIShell, IMGPROS_RES_FILE, IDI_IMAGE_ONE);
       }
       else
       {
           return;
       }
	
	IIMAGE_GetInfo(pApp->m_pImage1, &ii);

	ImageSizeX = (pApp->m_cxWidth - ii.cx) / 2;
	ImageSizeY = (pApp->m_cyHeight - ii.cy) / 2;
	
	switch(number)
	{
	case IDC_IMAGE_ONE:
		IIMAGE_Draw(pApp->m_pImage1, ImageSizeX, ImageSizeY);			
		break;

	case IDC_IBITMAPSCALE:
		pDispBitmap = IDISPLAY_GetDestination(pApp->a.m_pIDisplay);
		IBITMAP_CreateCompatibleBitmap(pDispBitmap, &pSrcBitmap, (uint16)ii.cx, (uint16)ii.cy);
		IBITMAP_CreateCompatibleBitmap(pDispBitmap, &pDstBitmap, (uint16)ii.cx, (uint16)ii.cy); //creates pDstBitmap bitmap compatible with pDeviceBitmap

              // draw image1 on pSrcBitmap
		IDISPLAY_SetDestination(pApp->a.m_pIDisplay, pSrcBitmap);
		IIMAGE_Draw(pApp->m_pImage1, 0, 0);

              // set back Destination 
		IDISPLAY_SetDestination(pApp->a.m_pIDisplay, pDispBitmap);
		
		IBitmapScale_Scale(pApp->m_iBitmapScale,
			pSrcBitmap, &pApp->m_prcSrc,
			pDstBitmap, &pApp->m_prcDst,
			&pApp->m_prcClip, 
			nTransOp);
		
		IDISPLAY_BitBlt(pApp->a.m_pIDisplay,0,0,pApp->m_cxWidth,pApp->m_cyHeight, pDstBitmap, 0, 0, AEE_RO_COPY);

		RELEASEIF(pSrcBitmap);
		RELEASEIF(pDstBitmap);
		RELEASEIF(pDispBitmap);
		break;

	default:
	       break;
	}

	IDISPLAY_Update(pApp->a.m_pIDisplay);
	
	
   	RELEASEIF(pApp->m_pImage1);
}


static void CImgProsApp_DisplayMainMenu( CImgProsApp* pApp)
{
	IDISPLAY_ClearScreen(pApp->a.m_pIDisplay);
	
	if (!pApp->m_pMenu)
	{
		if (ISHELL_CreateInstance(pApp->a.m_pIShell, AEECLSID_MENUCTL, (void **)&(pApp->m_pMenu)) == SUCCESS)
		{
			IMENUCTL_SetRect(pApp->m_pMenu, &(pApp->m_rScreenRect));
			SetMenuStyle(pApp, pApp->m_pMenu);
			
			IMENUCTL_AddItem(pApp->m_pMenu, IMGPROS_RES_FILE, IDC_IMAGE_ONE, IDC_IMAGE_ONE, NULL, NULL);
			IMENUCTL_AddItem(pApp->m_pMenu, IMGPROS_RES_FILE, IDC_IBITMAPSCALE, IDC_IBITMAPSCALE, NULL, NULL);
            
			IMENUCTL_SetActive(pApp->m_pMenu, TRUE);
			
		}
	}
	
	
}


static void SetMenuStyle(CImgProsApp* pMe, IMenuCtl* pIMenu)
{
	if (pMe->m_nColorDepth == 1)
	{
		// Get rid of 3D effect on mono screens
		AEEItemStyle Sel,Normal;
		
		Sel.ft 		= AEE_FT_BOX;
		Sel.xOffset = 0;
		Sel.yOffset = 0;
		Sel.roImage = AEE_RO_NOT;
		
		Normal.ft 		= AEE_FT_NONE;
		Normal.xOffset = 0;
		Normal.yOffset = 0;
		Normal.roImage = AEE_RO_COPY;
		
		IMENUCTL_SetStyle(pIMenu, &Normal, &Sel);
	}
}

static void CImgProsApp_FreeAppData(CImgProsApp* pApp)
{
	CImgProsApp* pMe = (CImgProsApp*)pApp;

	RELEASEIF(pMe->m_iBitmapScale);
	RELEASEIF(pMe->m_pMenu);
}


