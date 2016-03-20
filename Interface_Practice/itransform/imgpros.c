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
#include "AEEIBitmap.h"
#include "AEEIQI.h"
#include "AEEStdLib.h"
#include "AEEInterface.h"
/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
//static boolean CImgProsApp_HandleEvent(IApplet * pi, AEEEvent eCode, 
 //                                     uint16 wParam, uint32 dwParam);

static boolean CImgProsApp_HandleEvent(CImgProsApp* pApp,AEEEvent eCode, uint16 wParam, uint32 dwParam );
static void    CImgProsApp_FreeAppData(CImgProsApp* pApp);
static boolean CImgProsApp_InitAppData(IApplet*po,uint32 clsid,IShell* pIShell);
static void CImgProsApp_DisplaySplashScreen(CImgProsApp * pApp);
static void CImgProsApp_DisplayMainMenu( CImgProsApp* pApp);
static void SetMenuStyle(CImgProsApp* pMe, IMenuCtl* pIMenu);
static void CImgProsApp_TransImage(CImgProsApp * pApp,int number);

#define RELEASEIF(pi)      { if (pi) { IBASE_Release((IBase*)(pi)); (pi)=0; }}
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
			}
			else if (IMENUCTL_HandleEvent(pApp->m_pMenu, eCode, wParam, dwParam))
				return TRUE;
		}
		
		if (wParam == AVK_CLR)
		{
			if (pApp->m_pISpeedMenu)
			{
				
				IMENUCTL_Release(pApp->m_pISpeedMenu);
				pApp->m_pISpeedMenu = NULL;
				
				
				CImgProsApp_DisplayMainMenu(pApp);
			}
			return TRUE;
		}
		
		return FALSE;
		
		
		
	case EVT_COMMAND:
		switch(wParam)
		{
		case IDC_ORIGINAL:
			CImgProsApp_TransImage(pApp,IDC_ORIGINAL);
			return TRUE;
		case IDC_ITRANSEFORM:
			CImgProsApp_TransImage(pApp,IDC_ITRANSEFORM);
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
	CImgProsApp* pMe = (CImgProsApp*)pApp;
	ISHELL_GetDeviceInfo(pMe->a.m_pIShell, &pMe->m_DeviceInfo);
	pMe->m_cxWidth = pMe->m_DeviceInfo.cxScreen;
	pMe->m_cyHeight = pMe->m_DeviceInfo.cyScreen;
	SETAEERECT(&pMe->m_rScreenRect,0,0,pMe->m_cxWidth,pMe->m_cyHeight);
	return TRUE;
}


/*=============================================================================
FUNCTION: OATBITMAPFX_ConvertImageToBmp

DESCRIPTION:  
   Convert an IImage to an IBitmap

PARAMETERS:
   *pMe       [in]:  OATBITMAPFX object
   *pImage    [in]:  IImage to convert
   *pInfo     [in]:  Image information of pImage
   **ppBitmap [out]: return pointer to the new bitmap

RETURN VALUE:
   BREW result code

COMMENTS:
   
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OATBITMAPFX_ConvertImageToBmp(CImgProsApp * pMe,
                                    IImage *pImage,
                                    AEEImageInfo *pInfo,
                                    IBitmap **ppBitmap)
{
   int nResult = AEE_SUCCESS;
   IDisplay *pDisplay = NULL;
   IBitmap  *pDispBmp = NULL;
   IBitmap  *pImgBmp = NULL;

   if ( NULL == ppBitmap || NULL == pImage ) 
   {
      return EBADPARM;
   }

   do 
   {
      // We use IDisplay to draw the image onto the bitmap
      nResult = ISHELL_CreateInstance(pMe->a.m_pIShell, 
                                  AEECLSID_DISPLAY, 
                                  (void **) &pDisplay);   
      if ( nResult != AEE_SUCCESS ) 
      {
         break;
      }

      // Get Display's destination bitmap
      pDispBmp = IDISPLAY_GetDestination(pDisplay);

      if ( NULL == pDispBmp ) 
      {
         nResult = EFAILED;
         break;
      }

      // Create the result bitmap object
      nResult = IBITMAP_CreateCompatibleBitmap(pDispBmp, &pImgBmp,
                                           pInfo->cx, pInfo->cy);
      if ( nResult != AEE_SUCCESS ) 
      {
         break;
      }

      // Set the bitmap in display
      nResult = IDISPLAY_SetDestination(pDisplay, pImgBmp);

      if ( nResult != AEE_SUCCESS ) 
      {
         break;
      }

      // Swap out the display in IImage
      IIMAGE_SetDisplay(pImage, pDisplay);

      // Draw the image onto our bitmap
      IIMAGE_Draw(pImage, 0, 0);

      // Set back the original destination bitmap in display
      nResult = IDISPLAY_SetDestination(pDisplay, pDispBmp);

      if ( nResult != AEE_SUCCESS ) 
      {
         break;
      }

      // Now we got ourselves the image in bitmap format
      *ppBitmap = pImgBmp;
      pImgBmp = NULL;

   } while (0);

   RELEASEIF(pDispBmp);
   RELEASEIF(pImgBmp);
   RELEASEIF(pDisplay);

   return nResult;
}



static void CImgProsApp_TransImage(CImgProsApp * pApp,int number)
{
	IBitmap* pSourceBitmap;
	IBitmap* pCompatibleBitmap;
	IBitmap* pDstBitmapTransfered;
	AEEImageInfo info;
	AEETransformMatrix m_matrix;
	int width;    //the width of image will be displayed
	int height;   //the height of image will be displayed
	int x = 0;
	int y = 0;
/*
	m_matrix.A = 128;   //amplification factor=0.5
	m_matrix.B = 0;
	m_matrix.C = 0;
	m_matrix.D = 128;
*/

/*
	m_matrix.A = 0;   //amplification factor=0.25, 逆时针转90度
	m_matrix.B = 64;
	m_matrix.C = -64;
	m_matrix.D = 0;
*/

	m_matrix.A = 72;   // amplification factor=0.4, 顺时针转45度，0.4*256 * cos(-45度)
	m_matrix.B = -72;  //                                         0.4*256 * sin(-45度)
	m_matrix.C = 72;   //                                         0.4*256 * -sin(-45度)
	m_matrix.D = 72;   //                                         0.4*256 * cos(-45度)


	if(!pApp->m_pImage)
		pApp->m_pImage = ISHELL_LoadResImage(pApp->a.m_pIShell,IMGPROS_RES_FILE,IDI_IMAGE);
	
	if(pApp->m_pImage)
	{
		IIMAGE_GetInfo(pApp->m_pImage, &info);
		if((int)info.cx!=pApp->m_cxWidth || (int)info.cy!=pApp->m_cyHeight)
		{
			width = pApp->m_cxWidth>(int)info.cx?(int)info.cx:pApp->m_cxWidth;  //get the minimum width of the image and the screen 
			height = pApp->m_cyHeight>(int)info.cy?(int)info.cy:pApp->m_cyHeight; //get the minimum height of the image and the screen 
			x = x + (pApp->m_cxWidth-width)/2;      
			y = y + (pApp->m_cyHeight-height)/2;
		}
				

		switch(number)
		{
		case IDC_ORIGINAL:
			IDISPLAY_ClearScreen(pApp->a.m_pIDisplay );
			IIMAGE_Draw(pApp->m_pImage, 0, 0);    //draw original image
			IDISPLAY_Update(pApp->a.m_pIDisplay);
			break;

		case IDC_ITRANSEFORM:
			// 将 image 转为 bitmap
			OATBITMAPFX_ConvertImageToBmp(pApp, pApp->m_pImage, &info, &pSourceBitmap);
			
			// 转为 compatible bitmap
			IBITMAP_CreateCompatibleBitmap(pSourceBitmap,
				&pCompatibleBitmap,
				(uint16)info.cx, 
				(uint16)info.cy);
			
			IBITMAP_BltIn(pCompatibleBitmap, 0, 0, info.cx, info.cy, pSourceBitmap, 0, 0, AEE_RO_COPY);
			
			// 获得 destinate bitmap 对象
			IBITMAP_CreateCompatibleBitmap(pCompatibleBitmap,
				&pDstBitmapTransfered,
				(uint16)info.cx, 
				(uint16)info.cy);
			
			// get transform interface
			IBITMAP_QueryInterface(pDstBitmapTransfered, AEECLSID_TRANSFORM, (void **)&(pApp->m_iTransform));
			// transform bitmap
			ITRANSFORM_TransformBltComplex(pApp->m_iTransform,0,0,pCompatibleBitmap,0,0,info.cx,info.cy,&m_matrix, COMPOSITE_OPAQUE); 
			
			// show on display
			IDISPLAY_ClearScreen(pApp->a.m_pIDisplay );
			IDISPLAY_BitBlt(pApp->a.m_pIDisplay, 0, 0, info.cx, info.cy, pDstBitmapTransfered, 0, 0, AEE_RO_COPY);                        
			IDISPLAY_Update(pApp->a.m_pIDisplay);

			// release resource
			RELEASEIF(pApp->m_iTransform);
			RELEASEIF(pDstBitmapTransfered);
			RELEASEIF(pCompatibleBitmap);
			RELEASEIF(pSourceBitmap);
			break;

		default:
			break;
		}
	}
	
	RELEASEIF(pApp->m_pImage);

}


static void CImgProsApp_DisplayMainMenu( CImgProsApp* pApp)
{
	if(pApp->m_pSplash != NULL)
	{
		IIMAGE_Release(pApp->m_pSplash);
		pApp->m_pSplash = NULL;
	}
	IDISPLAY_ClearScreen(pApp->a.m_pIDisplay);
    
	
	if (!pApp->m_pMenu)
	{
		if (ISHELL_CreateInstance(pApp->a.m_pIShell, AEECLSID_MENUCTL, (void **)&(pApp->m_pMenu)) == SUCCESS)
		{
			IMENUCTL_SetRect(pApp->m_pMenu, &(pApp->m_rScreenRect));
			SetMenuStyle(pApp, pApp->m_pMenu);
			
			
			IMENUCTL_AddItem(pApp->m_pMenu, IMGPROS_RES_FILE, IDC_ORIGINAL, IDC_ORIGINAL, NULL, NULL);
			IMENUCTL_AddItem(pApp->m_pMenu, IMGPROS_RES_FILE, IDC_ITRANSEFORM, IDC_ITRANSEFORM, NULL, NULL);
			
			IMENUCTL_SetActive(pApp->m_pMenu, TRUE);
			
		}
	}
	
}


static void SetMenuStyle(CImgProsApp* pMe, IMenuCtl* pIMenu)
{
	if (pMe->m_nColorDepth == 1)
	{
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

static void CImgProsApp_FreeAppData(CImgProsApp* pMe)
{
	if (pMe->m_pMenu)
	{
		IMENUCTL_Release(pMe->m_pMenu);
		pMe->m_pMenu = NULL;
	}

	if (pMe->m_pISpeedMenu)
	{
		IMENUCTL_Release(pMe->m_pISpeedMenu);
		pMe->m_pISpeedMenu = NULL;
	}

}
