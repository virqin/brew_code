#include "AEEModGen.h"         
#include "AEEAppGen.h"        
#include "AEEShell.h"  
#include "AppModule.h"
#include "Queue.h"
#include "AEEPointerHelpers.h"

#include "AppManager.bid"


#define  USER_EVT_STATR_APP (EVT_USER + 1000) 

typedef struct _AppManager {
	AEEApplet      a ;	      
    AEEDeviceInfo  DeviceInfo;
	void*      mBackupPointer;
	//void*      mMallocData;
	Queue*     mAppModuleQueue;
	AppModule* mCurrentModule;

} AppManager;


static  boolean AppManager_HandleEvent(AppManager* pMe,AEEEvent eCode, uint16 wParam,uint32 dwParam);
boolean AppManager_InitAppData(AppManager* pMe);
void    AppManager_FreeAppData(AppManager* pMe);
void    AppManager_LoadTempData(AppManager* pMe);

void    MainApp_DrawScreen(AppManager* pMe);
boolean MainApp_KeyEvent(AppManager* pMe,uint16 wParam);

int AppManager_StartApplet(AppManager* pMe,char* mod_name,AEECLSID clsId);
int AppManager_CloseApplet(AppManager* pMe,boolean bReturnToIdle);



int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{

	*ppObj = NULL;
	if( ClsId == AEECLSID_MY_APPMANAGER)
	{
		if( AEEApplet_New(sizeof(AppManager),ClsId,pIShell,po,(IApplet**)ppObj,(AEEHANDLER)AppManager_HandleEvent,(PFNFREEAPPDATA)AppManager_FreeAppData) ) 

		{
			if(AppManager_InitAppData((AppManager*)*ppObj))
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

static boolean AppManager_CheckModule(AppModule* module)
{
	//DBGPRINTF("*************AppManager_CheckModule***********************");
	if (module != NULL && module->mHandlerFun != NULL && module->mApplet != NULL && module->mApplet->pAppHandleEvent != NULL)
	{
		return TRUE;
	}
	return FALSE;
}

static boolean AppManager_HandleEvent(AppManager* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	//DBGPRINTF("******AppManager_HandleEvent:0X%X***********************",eCode);
	if (AppManager_CheckModule(pMe->mCurrentModule))
	{
		if(eCode == EVT_APP_STOP)
		{
			boolean bResult = pMe->mCurrentModule->mHandlerFun(pMe->mCurrentModule,eCode,wParam,dwParam);
			if (bResult == FALSE && ((*((boolean*) dwParam)) == FALSE)) //如果程序是最小化
			{
				pMe->mCurrentModule->mMinimized = TRUE;
				pMe->mCurrentModule = NULL;
				return(FALSE);
			}
			else
			{
				pMe->mAppModuleQueue->Remove(pMe->mAppModuleQueue,pMe->mCurrentModule);
				AppModule_Release(pMe->mCurrentModule);
				pMe->mCurrentModule = NULL;
				ISHELL_PostEvent(pMe->a.m_pIShell,AEECLSID_MY_APPMANAGER,USER_EVT_STATR_APP,0,0);
				*((boolean*) dwParam) = FALSE;
				return(FALSE);
			}
		}
		return pMe->mCurrentModule->mHandlerFun(pMe->mCurrentModule,eCode,wParam,dwParam);
	}
	switch (eCode) 
	{
	case EVT_APP_START:
		MainApp_DrawScreen(pMe);
		return(TRUE);
	case EVT_APP_STOP:
		{
			//*((boolean*) dwParam) = FALSE;
			//return(FALSE);
			return(TRUE);
		}
		//return(TRUE);
	case EVT_APP_SUSPEND:
		return(TRUE);
	case EVT_APP_RESUME:
		MainApp_DrawScreen(pMe);
		return(TRUE);
	case EVT_APP_MESSAGE:
		return(TRUE);
	//case EVT_POINTER_DOWN:
		//return MainApp_PointerEvent(pMe,AEE_POINTER_GET_X((const char*)dwParam),AEE_POINTER_GET_Y((const char*)dwParam));
	case EVT_KEY:
		return MainApp_KeyEvent(pMe,wParam);
	case USER_EVT_STATR_APP:
		ISHELL_StartApplet(pMe->a.m_pIShell,AEECLSID_MY_APPMANAGER);
		return(TRUE);
	default:
		break;
	}
	return FALSE;
}


boolean AppManager_InitAppData(AppManager* pMe)
{
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);
	pMe->mAppModuleQueue = Queue_CreateInstance();
	AppManager_LoadTempData(pMe);
    return TRUE;
}




void    AppManager_LoadTempData(AppManager* pMe)
{
	IFileMgr      *pFileMgr      = NULL;
	IFile         *pFile         = NULL;
	uint32 b;
	if( !(SUCCESS == ISHELL_CreateInstance( pMe->a.m_pIShell , AEECLSID_FILEMGR, (void **) &pFileMgr)) )
	{
		return;
	}
	pFile = IFILEMGR_OpenFile(pFileMgr, AEEFS_HOME_DIR"temp.data", _OFM_READWRITE);
	IFILE_Read(pFile, &b, sizeof(uint32));
	pMe->mBackupPointer = (void*)b;
	//IFILE_Read(pFile, &b, sizeof(uint32));
	//pMe->mMallocData = (void*)b;
	IFILE_Release(pFile);
	IFILEMGR_Release(pFileMgr);
}

void AppManager_FreeAppData(AppManager* pMe)
{
	if(pMe->mAppModuleQueue != NULL)
	{
		AppModule* module = NULL;
		//DBGPRINTF("****AppModuleQueue Size:%d********************",pMe->mAppModuleQueue->Size);
		while(pMe->mAppModuleQueue->Size > 0)
		{
			module = pMe->mAppModuleQueue->At(pMe->mAppModuleQueue,0);
			pMe->mAppModuleQueue->Remove(pMe->mAppModuleQueue,module);
			AppModule_Release(module);
		}
		pMe->mAppModuleQueue->Release(pMe->mAppModuleQueue);
	}
}

void    MainApp_DrawScreen(AppManager* pMe)
{
	AECHAR *szText1 = L"1 KeyMove";
	AECHAR *szText2 = L"2 MediaPlayer";
	AECHAR *szText3 = L"3 Game";
	AECHAR *szText4 = L"4 PFly";
	AECHAR *szText5 = L"5 FlyVideo";
	IDISPLAY_DrawText(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szText1,-1,0,20, NULL,IDF_ALIGN_CENTER);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szText2,-1,0,40, NULL,IDF_ALIGN_CENTER);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szText3,-1,0,60, NULL,IDF_ALIGN_CENTER);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szText4,-1,0,80, NULL,IDF_ALIGN_CENTER);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szText5,-1,0,100, NULL,IDF_ALIGN_CENTER);
	IDISPLAY_Update (pMe->a.m_pIDisplay);
}



boolean MainApp_KeyEvent(AppManager* pMe,uint16 wParam)
{       
	char* app_name = NULL;
	AEECLSID clsId;
	switch(wParam)
	{
	case AVK_1:
		app_name = "helloworld.mod";
		clsId = 0x88888886;
		break;
	case AVK_2:
		app_name = "mediaplayer.mod";
		clsId = 0x01010ef6;
		break;
	case AVK_3:
		app_name = "game.mod";
		clsId = 0x00000010;
		break;
	case AVK_4:
		app_name = "pfly.mod";
		clsId = 0x10120201;
		break;
	case AVK_5:
		app_name = "flyvideo.mod";
		clsId = 0x55555555;
		break;
	case AVK_7:
		{
			ISHELL_StartApplet(pMe->a.m_pIShell,0x88888881);
			break;
		}
	case AVK_CLR:
		AppManager_CloseApplet(pMe,FALSE);
		return(TRUE);
	}
	if (app_name != NULL)
	{
		AppManager_StartApplet(pMe,app_name,clsId);
	}
	return(TRUE);
}


int AppManager_CloseApplet(AppManager* pMe,boolean bReturnToIdle)
{
	return ISHELL_CloseApplet(pMe->a.m_pIShell,bReturnToIdle);
}



int AppManager_StartApplet(AppManager* pMe,char* mod_name,AEECLSID clsId)
{
	AppModule* module = NULL;
	int index;
	//DBGPRINTF("******************************************");
	//DBGPRINTF("**********AppManager_StartApple***********");
	//DBGPRINTF("******************************************");
	for (index = 0;index < pMe->mAppModuleQueue->Size;index ++)
	{
		//DBGPRINTF("AppManager Find App At:%d",index);
		module = pMe->mAppModuleQueue->At(pMe->mAppModuleQueue,index);
		if (module->mClsID == clsId)
		{
			//DBGPRINTF("AppManager App In Queue");
			pMe->mCurrentModule = module;
			module->mHandlerFun(module,EVT_APP_START,0,0);
			return SUCCESS;
		}
	}
	//DBGPRINTF("AppManager AppModule_CreateInstance");
	index = AppModule_CreateInstance(pMe->a.m_pIShell,pMe->mBackupPointer,mod_name,&module);
	//DBGPRINTF("AppModule_CreateInstance Result:%d",index);
	if (AppModule_StartApplet(module,clsId) == SUCCESS)
	{

		pMe->mAppModuleQueue->Add(pMe->mAppModuleQueue,module);
		pMe->mCurrentModule = module;
	}
	else
	{
		if (module != NULL)
		{
			AppModule_Release(module);
		}
		//DBGPRINTF("****AppModule_StartApplet EFAILED********");
	}
	
	return SUCCESS;
}
