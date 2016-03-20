
#include "AEEModGen.h"         
#include "AEEAppGen.h" 
#include "aeestdlib.h"
#include "AEEShell.h"           
#include "AEEFile.h"

#include "MainApp.bid"

//#define AEECLSID_APPLOADER      0x88888881

typedef struct _MainApp {
	AEEApplet      a ;	       
    AEEDeviceInfo  DeviceInfo; 


} MainApp;


static  boolean MainApp_HandleEvent(MainApp* pMe,AEEEvent eCode, uint16 wParam,uint32 dwParam);
boolean MainApp_InitAppData(MainApp* pMe);
void    MainApp_FreeAppData(MainApp* pMe);
void    MainApp_DrawScreen(MainApp* pMe);
void    MainApp_StartApp(MainApp* pMe, const char* app_name);
boolean MainApp_KeyEvent(MainApp* pMe,uint16 wParam);
int MainApp_LoadMod(IShell *pIShell, void *ph, IModule **ppMod,const char* mod_name);
typedef int (*RunLoadMod)(IShell *ps, void * ph, IModule ** pMod);



int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;
	if( ClsId == AEECLSID_MAINAPP)
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
	AECHAR *szText1 = L"Press 1 Start APP1";
	AECHAR *szText2 = L"Press 2 Start APP2";
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


boolean MainApp_KeyEvent(MainApp* pMe,uint16 wParam)
{       
	char* app_name;
	switch(wParam)
	{
	case AVK_1:
		app_name = "hellobrew.mod";
		break;
	case AVK_2:
		app_name = "helloworld.mod";
		break;
	}
	MainApp_StartApp(pMe,app_name);
	return(TRUE);
}

void    MainApp_StartApp(MainApp* pMe, const char* app_name)
{
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
	}
}



void MainApp_FreeAppData(MainApp* pMe)
{
   
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
	nResult =  pFun(pIShell,ph,ppMod);
	DBGPRINTF("After RunLoadMod");
	return nResult;
TagFailModLoader:
	return EFAILED;
}