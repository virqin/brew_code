
#include "AEEHeap.h"    // Memory allocation Routines
#include "AEEStdLib.h"  // AEE Standard C library functions
#include "AEEModGen.h"  // Contains Sample AEEModule declarations
#include "AEEAppGen.h"
#include "AEEFile.h"
#include "nmdef.h"


static uint32     AEEMod_AddRef(IModule *po);
static uint32     AEEMod_Release(IModule *po);
static int        AEEMod_CreateInstance(IModule *po,IShell *pIShell,AEECLSID ClsId, void **ppObj);
static void       AEEMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm);
//char* AEEMod_LoadModName(IShell *pIShell,char* pName,int name_size);

typedef int (*RunLoadMod)(IShell *ps, void * ph, IModule ** pMod);




#ifdef AEE_SIMULATOR
AEEHelperFuncs *g_pvtAEEStdLibEntry;
#endif



#ifdef AEE_LOAD_DLL
__declspec(dllexport)
#endif
int AEEMod_Load(IShell *pIShell, void *ph, IModule **ppMod)
{
   return AEEStaticMod_New(sizeof(AEEMod),pIShell,ph,ppMod,NULL,NULL);
}


int AEEStaticMod_New(int16 nSize, IShell *pIShell, void *ph, IModule **ppMod,PFNMODCREATEINST pfnMC,PFNFREEMODDATA pfnMF)
{
	IFileMgr      *pFileMgr      = NULL;
	IFile         *pFile         = NULL;
	byte          *pData         = NULL;
	FileInfo      fileinfo             ;
	RunLoadMod    pFun           = NULL;
	int           nResult        = 0;
	char          name[128];
	char          path[128];
	DBGPRINTF( "+++++++++++++++++++++++++++++++++++++++++");
	MEMSET(name,0,128*sizeof(char));
	MEMSET(path,0,128*sizeof(char));
	if(!(SUCCESS == ISHELL_CreateInstance( pIShell , AEECLSID_FILEMGR, (void **) &pFileMgr)) )
		goto TagFailModLoader;

	/*
	pFile = IFILEMGR_OpenFile( pFileMgr , AEEFS_SHARED_DIR"run.app" ,_OFM_READWRITE);
		if ( NULL == pFile ){
			DBGPRINTF( "Open run.app EFAILED");
			IFILEMGR_Release( pFileMgr );
			goto TagFailModLoader;}
		else{
			DBGPRINTF( "Open run.app OK");
		}
		IFILE_Read(pFile, name, 128);
		IFILE_Release(pFile);
		STRCPY(path,AEEFS_SHARED_DIR);
		STRCAT(path,name);
	
		DBGPRINTF("Load Mod Path:%s",path);
		DBGPRINTF("Load Mod Name:%s",name);*/
	
	//pFile = IFILEMGR_OpenFile( pFileMgr , "helloworld.bin" ,_OFM_READWRITE);//打开读 和写。
	pFile = IFILEMGR_OpenFile( pFileMgr , "flyvideo.mod" ,_OFM_READWRITE);//打开读 和写。
	if ( NULL == pFile ){
		DBGPRINTF( "Open Mod File EFAILED");
		IFILEMGR_Release( pFileMgr );
		goto TagFailModLoader;}
	else{
		DBGPRINTF( "Open Mod File OK");
	}
	IFILE_GetInfo(pFile, &fileinfo);
	pData = (byte *)MALLOC( 4 * fileinfo.dwSize + 5);//pData不用释放,由AEE环境释放
	MEMSET( pData , 0x00 ,  4 * fileinfo.dwSize + 5);
	MEMCPY( pData , &ph , 4 );
	DBGPRINTF( "Open File OK:%d" , fileinfo.dwSize );
	IFILE_Read( pFile , pData + 4 , fileinfo.dwSize );
	DBGPRINTF( "Read File OK" );

	IFILE_Release( pFile );
	IFILEMGR_Release( pFileMgr );

	DBGPRINTF("Before RunLoadMod");
	pFun = (RunLoadMod)(pData + 4);
	nResult =  pFun(pIShell,ph,ppMod);
	(*ppMod)->pvt->FreeResources = AEEMod_FreeResources;
	DBGPRINTF("After RunLoadMod");
	DBGPRINTF( "+++++++++++++++++++++++++++++++++++++++++");
	return nResult;
TagFailModLoader:
	DBGPRINTF("AEEMod_Load EFAILED");
	DBGPRINTF( "+++++++++++++++++++++++++++++++++++++++++");
	return EFAILED;
}

/*
char* AEEMod_LoadModName(IShell *pIShell,char* pName,int name_size)
{
	IFileMgr      *pFileMgr      = NULL;
	IFile         *pFile         = NULL;
	if( !(SUCCESS == ISHELL_CreateInstance( pIShell , AEECLSID_FILEMGR, (void **) &pFileMgr)) )
	{
		return;
	}
	if (IFILEMGR_Test(pFileMgr, AEEFS_SHARED_DIR"run.app") != SUCCESS)
	{
		pFile = IFILEMGR_OpenFile(pFileMgr, AEEFS_SHARED_DIR"run.app", _OFM_CREATE);
	}
	else
	{
		pFile = IFILEMGR_OpenFile(pFileMgr, AEEFS_SHARED_DIR"run.app", _OFM_READWRITE);
	}
	IFILE_Read(pFile, pName, name_size);
	IFILE_Release(pFile);
	IFILEMGR_Release(pFileMgr);
	return pName;
}*/



/*========================For Default AEEModGen ================================================*/
static uint32 AEEMod_AddRef(IModule *po)
{
   return (++((AEEMod *)po)->m_nRefs);
}

static uint32 AEEMod_Release(IModule *po)
{
   AEEMod *pMe = (AEEMod *)po;

   if (--pMe->m_nRefs != 0) {
      return pMe->m_nRefs;
   }
   if (pMe->pfnModFreeData) {
      pMe->pfnModFreeData(po);
   }
   if (pMe->m_pIShell) {
      ISHELL_Release(pMe->m_pIShell);
      pMe->m_pIShell = NULL;
   }

   //Free the object itself
   FREE_VTBL(pMe, IModule);
   FREE(pMe);
  
   return 0;
}


static int AEEMod_CreateInstance(IModule *pIModule,IShell *pIShell,
                                 AEECLSID ClsId,void **ppObj)
{
   AEEMod    *pme = (AEEMod *)pIModule;
   int        nErr = EFAILED;

   if (pme->pfnModCrInst) {
      nErr = pme->pfnModCrInst(ClsId, pIShell, pIModule, ppObj);
#if !defined(AEE_STATIC)
   } else {
      nErr = AEEClsCreateInstance(ClsId, pIShell, pIModule, ppObj);
#endif
   }

   return nErr;
}

static void AEEMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm)
{
	DBGPRINTF( "+++++++++AEEMod_FreeResources++++++++++++");
	
	DBGPRINTF( "+++++++++AEEMod_FreeResources++++++++++++");
   (void)po,(void)ph,(void)pfm; /* all unused */
}

/*========================End of File ================================================*/
