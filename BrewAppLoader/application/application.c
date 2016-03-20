
#include "AEEModGen.h"         
#include "AEEAppGen.h" 
#include "aeestdlib.h"
#include "AEEShell.h"           
#include "AEEFile.h"

#include "application.bid"

#define AEECLSID_APPLOADER      0x88888881


typedef int (*RunLoadMod)(IShell *ps, void * ph, IModule ** pMod);

typedef struct _MyApplet
{
	AEEApplet* mAppletData;
	byte*      mModData;
	IModule*   mModule;
	void*      mPH;
	RunLoadMod mLoadModFun;
}MyApplet;



typedef struct _MainApp {
	AEEApplet      a ;	       
	AEEDeviceInfo  DeviceInfo; 
	IModule * pIModule;
	MyApplet* pMyApplet;
} MainApp;


static  boolean MainApp_HandleEvent(MainApp* pMe,AEEEvent eCode, uint16 wParam,uint32 dwParam);
boolean MainApp_InitAppData(MainApp* pMe);
void    MainApp_FreeAppData(MainApp* pMe);
void    MainApp_DrawScreen(MainApp* pMe);
void    MainApp_StartApp(MainApp* pMe, const char* app_name);
boolean MainApp_KeyEvent(MainApp* pMe,uint16 wParam);

void*   MainApp_GetPH(IShell *pIShell);
RunLoadMod MainApp_GetLoadModFun(MainApp* pMe,MyApplet* applet,const char* mod_name);
int MainApp_LoadMod(IShell *pIShell, void *ph, IModule **ppMod,const char* mod_name);
void StartModAppliaction(IShell *pIShell,IModule* pMod);







int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;
	if( ClsId == AEECLSID_APPLICATION)
	{
		if( AEEApplet_New(sizeof(MainApp),ClsId,pIShell,po,(IApplet**)ppObj,(AEEHANDLER)MainApp_HandleEvent,(PFNFREEAPPDATA)MainApp_FreeAppData) ) 

		{
			if(MainApp_InitAppData((MainApp*)*ppObj))
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


static boolean MainApp_HandleEvent(MainApp* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	switch (eCode) 
	{
	case EVT_APP_START:
		MainApp_DrawScreen(pMe);
		return(TRUE);
	case EVT_APP_STOP:
		return(TRUE);
	case EVT_APP_SUSPEND:
		return(TRUE);
	case EVT_APP_RESUME:
		MainApp_DrawScreen(pMe);
		return(TRUE);
	case EVT_APP_MESSAGE:
		return(TRUE);
	case EVT_KEY:
		return MainApp_KeyEvent(pMe,wParam);
	default:
		break;
	}
	return FALSE;
}

void    MainApp_DrawScreen(MainApp* pMe)
{
	AECHAR *szText1 = L"1 Start FlyVideo";
	AECHAR *szText2 = L"2 Start Hello World";
	IDISPLAY_DrawText(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szText1,-1,0,40, NULL,IDF_ALIGN_CENTER);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szText2,-1,0,100, NULL,IDF_ALIGN_CENTER);
	IDISPLAY_Update (pMe->a.m_pIDisplay);
}

boolean MainApp_InitAppData(MainApp* pMe)
{
	pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
	ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);
	return TRUE;
}
void MainApp_FreeAppData(MainApp* pMe)
{
}


boolean MainApp_KeyEvent(MainApp* pMe,uint16 wParam)
{       
	char* app_name;
	switch(wParam)
	{
	case AVK_1:
		//app_name = "apps/flyvideo.bin";
		app_name = "flyvideo.mod";
		break;
	case AVK_2:
		//app_name = "apps/helloworld.bin";
		app_name = "helloworld.mod";
		break;
	}
	MainApp_StartApp(pMe,app_name);
	return(TRUE);
}



void    MainApp_StartApp(MainApp* pMe, const char* app_name)
{
/*
	if (app_name != NULL && STRLEN(app_name) > 0)
	{
		IFileMgr *pFm;
		IFile *pFile;
		ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,(void **)&pFm);
		if (IFILEMGR_Test(pFm, AEEFS_SHARED_DIR"run.app") == SUCCESS)
		{
			IFILEMGR_Remove(pFm, AEEFS_SHARED_DIR"run.app");
		}
		pFile = IFILEMGR_OpenFile(pFm, AEEFS_SHARED_DIR"run.app", _OFM_CREATE);

		IFILE_Write(pFile, app_name, STRLEN(app_name) * sizeof(char));
		IFILE_Release(pFile);
		IFILEMGR_Release(pFm);
		ISHELL_StartApplet(pMe->a.m_pIShell,0x88888881);
	}*/


	/*
	if (app_name != NULL && STRLEN(app_name) > 0)
		{
			int nResult;
			pMe->pMyApplet = (MyApplet*)MALLOC(sizeof(MyApplet));
			pMe->pIModule = (IModule*)MALLOC(sizeof(IModule));
			MEMCPY(pMe->pIModule,pMe->a.m_pIModule,sizeof(IModule));
			pMe->pMyApplet->mModule = pMe->a.m_pIModule;
			
			pMe->pMyApplet->mPH = MainApp_GetPH(pMe->a.m_pIShell);
			pMe->pMyApplet->mLoadModFun = MainApp_GetLoadModFun(pMe,pMe->pMyApplet,app_name);
			nResult = pMe->pMyApplet->mLoadModFun(pMe->a.m_pIShell,pMe->pMyApplet->mPH,&pMe->pMyApplet->mModule);
			DBGPRINTF("_____MainApp_StartApp:%d",nResult);
		}*/


	if (app_name != NULL || STRLEN(app_name) > 0)
	{
		IFileMgr      *pFileMgr      = NULL;
		IFile         *pFile         = NULL;
		IShell* pIShell;
		void* ph;
		IModule** ppMod;
		IModule* pMod = NULL;
		uint32 b;
		ppMod = &pMod; 
		if( !(SUCCESS == ISHELL_CreateInstance( pMe->a.m_pIShell , AEECLSID_FILEMGR, (void **) &pFileMgr)) )
		{
			return;
		}
		if (IFILEMGR_Test(pFileMgr, AEEFS_HOME_DIR"temp.data") != SUCCESS)
		{
			pFile = IFILEMGR_OpenFile(pFileMgr, AEEFS_HOME_DIR"temp.data", _OFM_CREATE);
		}
		else
		{
			pFile = IFILEMGR_OpenFile(pFileMgr, AEEFS_HOME_DIR"temp.data", _OFM_READWRITE);
		}

		IFILE_Read(pFile, &b, sizeof(uint32));
		pIShell = (IShell*)b;
		IFILE_Read(pFile, &b, sizeof(uint32));
		ph = (void*)b;
		//IFILE_Read(pFile, &b, sizeof(uint32));
		//ppMod = (void*)b;
		IFILE_Release(pFile);
		MainApp_LoadMod(pIShell,ph,ppMod,app_name);
		pMod = *ppMod;
		StartModAppliaction(pIShell,pMod);
	}
}

void StartModAppliaction(IShell *pIShell,IModule* pMod)
{
	void* data = NULL;
	int   nResult = 0;
	AEEApplet *pApplet;
	DBGPRINTF("************************************");
	IMODULE_AddRef(pMod);
	nResult = IMODULE_CreateInstance(pMod,pIShell,0x44444444,&data);
	DBGPRINTF("IMODULE_CreateInstance:%d",nResult);
	pApplet = (AEEApplet*)data;
	nResult = pApplet->pAppHandleEvent(pApplet,EVT_APP_START,0,0);
	DBGPRINTF("pAppHandleEvent:%d",nResult);
	DBGPRINTF("************************************");
}

void*   MainApp_GetPH(IShell *pIShell)
{
	IFileMgr      *pFileMgr      = NULL;
	IFile         *pFile         = NULL;
	void* ph;
	uint32 b;
	if( !(SUCCESS == ISHELL_CreateInstance( pIShell , AEECLSID_FILEMGR, (void **) &pFileMgr)) )
	{
		return NULL;
	}
	if (IFILEMGR_Test(pFileMgr, AEEFS_HOME_DIR"temp.data") != SUCCESS)
	{
		pFile = IFILEMGR_OpenFile(pFileMgr, AEEFS_HOME_DIR"temp.data", _OFM_CREATE);
	}
	else
	{
		pFile = IFILEMGR_OpenFile(pFileMgr, AEEFS_HOME_DIR"temp.data", _OFM_READWRITE);
	}
	IFILE_Read(pFile, &b, sizeof(uint32));
	IFILE_Read(pFile, &b, sizeof(uint32));
	ph = (void*)b;
	IFILE_Release(pFile);
	IFILEMGR_Release(pFileMgr);
	return ph;
}


RunLoadMod MainApp_GetLoadModFun(MainApp* pMe,MyApplet* applet,const char* mod_name)
{
	IFileMgr      *pFileMgr      = NULL;
	IFile         *pFile         = NULL;
	FileInfo      fileinfo             ;
	RunLoadMod    pFun           = NULL;
	int           nResult        = 0;

	if( !(SUCCESS == ISHELL_CreateInstance( pMe->a.m_pIShell , AEECLSID_FILEMGR, (void **) &pFileMgr)) )
		goto TagFailModLoader;
	pFile = IFILEMGR_OpenFile( pFileMgr , mod_name ,_OFM_READWRITE);//打开读 和写。
	if ( NULL == pFile )
		goto TagFailModLoader;
	IFILE_GetInfo(pFile, &fileinfo);
	applet->mModData = (byte *)MALLOC( 4 * fileinfo.dwSize + 5);//pData不用释放,由AEE环境释放
	MEMSET( applet->mModData , 0x00 ,  4 * fileinfo.dwSize + 5);
	MEMCPY( applet->mModData , &applet->mPH , 4 );
	IFILE_Read( pFile , applet->mModData + 4 , fileinfo.dwSize );
	IFILE_Release( pFile );
	IFILEMGR_Release( pFileMgr );
	pFun = (RunLoadMod)(applet->mModData + 4);
	return pFun;
TagFailModLoader:
	return NULL;
}

int MainApp_LoadMod(IShell *pIShell, void *ph, IModule **ppMod,const char* mod_name)
{
	IFileMgr      *pFileMgr      = NULL;
	IFile         *pFile         = NULL;
	byte          *pData         = NULL;
	FileInfo      fileinfo             ;
	RunLoadMod    pFun           = NULL;
	int           nResult        = 0;
	int           bytesread      = 0;

	if( !(SUCCESS == ISHELL_CreateInstance( pIShell , AEECLSID_FILEMGR, (void **) &pFileMgr)) )
		goto TagFailModLoader;
	DBGPRINTF("+++++++Create FileMgr OK");
	pFile = IFILEMGR_OpenFile( pFileMgr , mod_name ,_OFM_READWRITE);//打开读 和写。
	if ( NULL == pFile ){
		DBGPRINTF( "+++++++IFILEMGR_OpenFile ID:EFAILED");
		goto TagFailModLoader;}
	else{
		DBGPRINTF( "+++++++IFILEMGR_OpenFile ID:OK");
	}
	IFILE_GetInfo(pFile, &fileinfo);
	pData = (byte *)MALLOC( 4 * fileinfo.dwSize + 5);//pData不用释放,由AEE环境释放
	MEMSET( pData , 0x00 ,  4 * fileinfo.dwSize + 5);
	MEMCPY( pData , &ph , 4 );
	DBGPRINTF( "Open File OK:%d" , fileinfo.dwSize );
	bytesread = IFILE_Read( pFile , pData + 4 , fileinfo.dwSize );
	DBGPRINTF( "Read File OK" );

	IFILE_Release( pFile );
	IFILEMGR_Release( pFileMgr );

	DBGPRINTF("Before RunLoadMod");
	pFun = (RunLoadMod)(pData + 4);
	nResult =  pFun(pIShell,NULL,ppMod);
	DBGPRINTF("RunLoadMod:%d",nResult);

	DBGPRINTF("After RunLoadMod");
	return nResult;
TagFailModLoader:
	return EFAILED;
}