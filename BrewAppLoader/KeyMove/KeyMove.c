/*===========================================================================

FILE: KeyMove.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions


#include "KeyMove.bid"
#include "keymove.brh" //KEYMOVE_RES_FILE
#include "AEEPointerHelpers.h"



#ifndef Max
#define Max( x, y ) ( ((x) > (y)) ? (x) : (y) )
#endif
#ifndef Min
#define Min( x, y ) ( ((x) < (y)) ? (x) : (y) )
#endif

#define RECT_SIZE 15


typedef struct _KeyMove {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information

	IDisplay *m_pIDisplay;
	AEERect m_rect;
	AEERect m_FullScreenRect;
	AECHAR  mScreenText[128];
	boolean mSureExit;
} KeyMove;




static  boolean KeyMove_HandleEvent(KeyMove* pMe,AEEEvent eCode, uint16 wParam,uint32 dwParam);
boolean KeyMove_InitAppData(KeyMove* pMe);
void    KeyMove_FreeAppData(KeyMove* pMe);
boolean KeyMove_KeyEvent(KeyMove* pMe,uint16 wParam);
boolean MainApp_PointerEvent(KeyMove* pMe,int16 x,int16 y);
boolean print(KeyMove* pMe);


int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;

	if( ClsId == AEECLSID_KEYMOVE )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(KeyMove),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)KeyMove_HandleEvent,
                          (PFNFREEAPPDATA)KeyMove_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(KeyMove_InitAppData((KeyMove*)*ppObj))
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


static boolean KeyMove_HandleEvent(KeyMove* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    switch (eCode) 
	{
        case EVT_APP_START:   
			{
				print(pMe);
				return(TRUE);
			}
        case EVT_APP_STOP:
			{
				if (pMe->mSureExit)
				{
					return(TRUE);
				}
				else
				{
					*((boolean*) dwParam) = FALSE;
					//ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_KEYMOVE,EVT_APP_MESSAGE,0,0);
					//ISHELL_StartApplet(pMe->a.m_pIShell,AEECLSID_KEYMOVE);
					return(FALSE);
				}
			}
        case EVT_APP_SUSPEND:
		    return(TRUE);
		case EVT_POINTER_DOWN:
			return MainApp_PointerEvent(pMe,AEE_POINTER_GET_X((const char*)dwParam),AEE_POINTER_GET_Y((const char*)dwParam));
        case EVT_APP_RESUME:
			{
				print(pMe);
				return(TRUE);
			}
        case EVT_APP_MESSAGE:
			//ISHELL_StartApplet(pMe->a.m_pIShell,AEECLSID_KEYMOVE);
		    return(TRUE);
        case EVT_KEY:
		    return KeyMove_KeyEvent(pMe,wParam);
        default:
            break;
   }

   return FALSE;
}



boolean KeyMove_InitAppData(KeyMove* pMe)
{
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

	pMe->m_pIDisplay = pMe->a.m_pIDisplay;
	pMe->m_rect.x=20;
	pMe->m_rect.y=20;

	pMe->m_FullScreenRect.x = 0;
	pMe->m_FullScreenRect.y = 0;
	pMe->m_FullScreenRect.dx = pMe->DeviceInfo.cxScreen;
	pMe->m_FullScreenRect.dy = pMe->DeviceInfo.cyScreen;
	SETAEERECT(&pMe->m_rect,pMe->m_rect.x,pMe->m_rect.y,RECT_SIZE,RECT_SIZE);

	ISHELL_LoadResString(pMe->a.m_pIShell,KEYMOVE_RES_FILE,IDS_TEXT_01,pMe->mScreenText,sizeof(pMe->mScreenText));
	pMe->mSureExit = FALSE;
    return TRUE;
}

void KeyMove_FreeAppData(KeyMove* pMe)
{

}

boolean MainApp_PointerEvent(KeyMove* pMe,int16 x,int16 y)
{
	pMe->m_rect.y = y;
	pMe->m_rect.x = x;
	pMe->m_rect.x = Max(0, Min(pMe->m_rect.x, pMe->DeviceInfo.cxScreen - RECT_SIZE));
	pMe->m_rect.y = Max(0, Min(pMe->m_rect.y, pMe->DeviceInfo.cyScreen - RECT_SIZE));
	SETAEERECT(&pMe->m_rect,pMe->m_rect.x,pMe->m_rect.y,RECT_SIZE,RECT_SIZE);


	if (y < pMe->DeviceInfo.cyScreen / 2 && x < pMe->DeviceInfo.cxScreen / 2)
	{
		ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE);
		return TRUE;
	}
	else if (y > pMe->DeviceInfo.cyScreen / 2 && x < pMe->DeviceInfo.cxScreen / 2)
	{
		pMe->mSureExit = TRUE;
		ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE);
		return TRUE;
	}
	return print(pMe);
}



boolean KeyMove_KeyEvent(KeyMove* pMe,uint16 wParam)
{
	switch(wParam)
	{
	case AVK_UP:
		pMe->m_rect.y -=4;
		break;
	case AVK_DOWN:
		pMe->m_rect.y +=4;
		break;
	case AVK_LEFT:
		pMe->m_rect.x -=4;
		break;
	case AVK_RIGHT:
		pMe->m_rect.x +=4;
		break;
	case AVK_CLR:
		{
			pMe->mSureExit = TRUE;
			ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE);
			break;
		}
	default:
		return FALSE;
	}
	pMe->m_rect.x = Max(0, Min(pMe->m_rect.x, pMe->DeviceInfo.cxScreen - RECT_SIZE));
	pMe->m_rect.y = Max(0, Min(pMe->m_rect.y, pMe->DeviceInfo.cyScreen - RECT_SIZE));
	SETAEERECT(&pMe->m_rect,pMe->m_rect.x,pMe->m_rect.y,RECT_SIZE,RECT_SIZE);

	return print(pMe);
}


boolean print(KeyMove* pMe)
{
	SETAEERECT(&pMe->m_FullScreenRect,pMe->m_FullScreenRect.x,pMe->m_FullScreenRect.y,pMe->m_FullScreenRect.dx,pMe->m_FullScreenRect.dy);
	
	//IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_TEXT, 0xFFFFFFFF);
	//IDISPLAY_SetColor(pMe->a.m_pIDisplay, CLR_USER_BACKGROUND, 0x00000000);
	IDISPLAY_DrawRect(pMe->a.m_pIDisplay,&pMe->m_FullScreenRect,0xffffff00,0xffffff00,IDF_RECT_FRAME|IDF_RECT_FILL);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay,AEE_FONT_BOLD,pMe->mScreenText,-1,0,0,NULL,IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	
	
	IDISPLAY_DrawRect(pMe->a.m_pIDisplay,&pMe->m_rect,0xff000000,0x00ff1200,IDF_RECT_FRAME|IDF_RECT_FILL);


	IDISPLAY_Update (pMe->m_pIDisplay);
	return TRUE;
}
