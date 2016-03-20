#include "AppModule.h"
#include "AEEModGen.h"         
#include "AEEAppGen.h" 
#include "aeestdlib.h"
#include "AEEShell.h"           
#include "AEEFile.h"

static int AppModule_LoadMod(AppModule* module);
boolean AppModule_Handler(void * pData, AEEEvent evt, uint16 wParam, uint32 lParam);

int AppModule_CreateInstance(IShell *pIShell,void* ph,char* path,AppModule** module)
{
	AppModule* pModule    ;
	int        nResult = 0;
	*module = NULL;
	pModule = (AppModule*)MALLOC(sizeof(AppModule));
	MEMSET(pModule,0,sizeof(AppModule));
	pModule->mShell = pIShell;
	pModule->mBackupPointer = ph;
	STRCPY(pModule->mModPath,path);
	nResult = AppModule_LoadMod(pModule);
	pModule->mHandlerFun = AppModule_Handler;
	pModule->mMinimized = FALSE;
	*module = pModule;
	return nResult;
}

int AppModule_CloseApplet(AppModule* module)
{
	return SUCCESS;
}

void AppModule_Release(AppModule* module)
{
	//DBGPRINTF("****AppModule_Release********************");
	if (module != NULL)
	{
		AEEMod *pAEEMod;
		int count = 0;
		int index = 0;
		IAPPLET_AddRef((IApplet*)module->mApplet);
		IMODULE_AddRef(module->mModule);
		if (module->mApplet->pFreeAppData != NULL)
			module->mApplet->pFreeAppData((IApplet*)(module->mApplet));
		//DBGPRINTF("AppModule_Release IApplet Count********************");
		count = module->mApplet->m_nRefs;
		//DBGPRINTF("**************Count:%d********************",count);
		for (index = 0;index < count;index++)
		{
			IAPPLET_Release((IApplet*)(module->mApplet));
		}
		pAEEMod = (AEEMod *)module->mModule;
		//DBGPRINTF("AppModule_Release Module Count********************");
		count = pAEEMod->m_nRefs;
		//DBGPRINTF("**************Count:%d********************",count);
		for (index = 0;index < count;index++)
		{
			IMODULE_Release(module->mModule);
		}
		FREEIF(module->mModBuff);
		FREEIF(module);
		//DBGPRINTF("****AppModule_Release Success********************");
	}
	else
	{
		//DBGPRINTF("****AppModule_Release EFAILED********************");
	}
}

int AppModule_StartApplet(AppModule* module,AEECLSID cls)
{
	int        nResult = 0;
	module->mClsID = cls;
	//DBGPRINTF("****StartApplet:0X%X************************",cls);
	//DBGPRINTF("****StartApplet AEELOADMOD************************");
	nResult = module->mLoadModFun(module->mShell,module->mBackupPointer,&(module->mModule));
	//DBGPRINTF("AEELoadMod Result:%d",nResult);
	if (nResult != SUCCESS)
	{
		//DBGPRINTF("**************ERROR***************************");
		return nResult;
	}
	IMODULE_AddRef(module->mModule);
	//DBGPRINTF("****StartApplet IMODULE_CreateInstance************************");
	nResult = IMODULE_CreateInstance(module->mModule,module->mShell,module->mClsID,(void**)(&(module->mApplet)));
	IMODULE_Release(module->mModule);
	//DBGPRINTF("IMODULE_CreateInstance:%d",nResult);
	if (nResult != SUCCESS)
	{
		//DBGPRINTF("**************ERROR***************************");
		return nResult;
	}
	nResult = AppModule_Handler(module,EVT_APP_START,0,0);
	//DBGPRINTF("AEEHandler:%d",nResult);
	if (nResult != TRUE)
	{
		//DBGPRINTF("**************ERROR***************************");
		return nResult;
	}
	//DBGPRINTF("*****AppModule_StartApplet OK***************************");
	return SUCCESS;
}

boolean AppModule_Handler(void * pData, AEEEvent evt, uint16 wParam, uint32 lParam)
{
	AppModule* module = (AppModule*)pData;
	if (module!= NULL && module->mApplet != NULL && module->mApplet->pAppHandleEvent != NULL)
	{
		return module->mApplet->pAppHandleEvent(module->mApplet,evt,wParam,lParam);
	}
	return FALSE;
}


static int AppModule_LoadMod(AppModule* module)
{
	IFileMgr      *pFileMgr      = NULL;
	IFile         *pFile         = NULL;
	FileInfo      fileinfo             ;
	int           nResult        = 0;
	//DBGPRINTF("****************************************");
	if( !(SUCCESS == ISHELL_CreateInstance( module->mShell , AEECLSID_FILEMGR, (void **) &pFileMgr)) )
		goto TagFailModLoader;
	//DBGPRINTF("AppModule Create FileMgr OK");
	pFile = IFILEMGR_OpenFile( pFileMgr , module->mModPath ,_OFM_READWRITE);//´ò¿ª¶Á ºÍÐ´¡£
	if ( NULL == pFile ){
		//DBGPRINTF( "AppModule OpenFile EFAILED");
		IFILEMGR_Release( pFileMgr );
		goto TagFailModLoader;}
	else{
		//DBGPRINTF( "AppModule OpenFile OK");
	}
	IFILE_GetInfo(pFile, &fileinfo);
	module->mModBuff = (byte *)MALLOC( 41000/*fileinfo.dwSize + 4*/);
	MEMSET( module->mModBuff , 0x00 ,  41000/*fileinfo.dwSize + 4*/);
	MEMCPY( module->mModBuff , &module->mBackupPointer , 4 );
	//DBGPRINTF( "AppModule OpenFile Size:%d" , fileinfo.dwSize );
	nResult = IFILE_Read( pFile , module->mModBuff + 4 , fileinfo.dwSize );
	//DBGPRINTF( "AppModule Read File OK" );
	IFILE_Release( pFile );
	IFILEMGR_Release( pFileMgr );
	module->mLoadModFun = (AEELOADMOD)(module->mModBuff + 4);
	//DBGPRINTF("Get Function Pointer OK");
	//DBGPRINTF("****************************************");
	return SUCCESS;
TagFailModLoader:
	return EFAILED;
}