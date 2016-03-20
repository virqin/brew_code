#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEText.h"

#include "AEENet.h"				// Socket interface definitions

#include "HelloWorld.bid"
#include "helloworld.brh"


typedef struct _HelloWorld {
	AEEApplet      a ;	       
    AEEDeviceInfo  DeviceInfo;
	IDisplay      *pDisplay;
	AECHAR         mScreenText[128];
} HelloWorld;


/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean HelloWorld_HandleEvent(HelloWorld* pMe, 
                                                   AEEEvent eCode, uint16 wParam, 
                                                   uint32 dwParam);
boolean HelloWorld_InitAppData(HelloWorld* pMe);
void    HelloWorld_FreeAppData(HelloWorld* pMe);
static void HelloBrew_DrawScreen(HelloWorld* pMe);


int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;
	if( ClsId == AEECLSID_HELLOWORLD )
	{
		if( AEEApplet_New(sizeof(HelloWorld),ClsId,pIShell,po,
                          (IApplet**)ppObj,(AEEHANDLER)HelloWorld_HandleEvent,(PFNFREEAPPDATA)HelloWorld_FreeAppData) ) 
                          
		{
			if(HelloWorld_InitAppData((HelloWorld*)*ppObj))
			{
				return(AEE_SUCCESS);
			}
			else
			{
				IAPPLET_Release((IApplet*)*ppObj);
				return EFAILED;
			}
        }

    }

	return(EFAILED);
}


static boolean HelloWorld_HandleEvent(HelloWorld* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	
    switch (eCode) 
	{
        case EVT_APP_START:                        
			HelloBrew_DrawScreen(pMe);
            return(TRUE);
        case EVT_APP_STOP:
      		return(TRUE);
        case EVT_APP_SUSPEND:
      		return(TRUE);
        case EVT_APP_RESUME:
		    HelloBrew_DrawScreen(pMe);
      		return(TRUE);
        case EVT_APP_MESSAGE:
      		return(TRUE);
        case EVT_KEY:
      		return(TRUE);
        default:
            break;
   }

   return FALSE;
}

static void HelloBrew_DrawScreen(HelloWorld* pMe)
{
	IDISPLAY_DrawText(pMe->pDisplay,AEE_FONT_BOLD,pMe->mScreenText,-1,0,0,NULL,IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update (pMe->pDisplay);

}


boolean HelloWorld_InitAppData(HelloWorld* pMe)
{
	pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);
	pMe->pDisplay = pMe->a.m_pIDisplay;
	ISHELL_LoadResString(pMe->a.m_pIShell,HELLOWORLD_RES_FILE,IDS_TEXT_01,pMe->mScreenText,sizeof(pMe->mScreenText));
    return TRUE;
}


void HelloWorld_FreeAppData(HelloWorld* pMe)
{

}
