#ifndef APPMODULE_H
#define APPMODULE_H

#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEFile.h"

typedef int (*AEELOADMOD)(IShell *ps, void * ph, IModule ** pMod);


#ifndef MAX_PATH
#define MAX_PATH 256
#endif

typedef struct _AppModule 
{
	AEEApplet* mApplet;
	AEECLSID   mClsID;
	IModule*   mModule;
	IShell*    mShell;
	void*      mBackupPointer;
	byte*      mModBuff;
	AEELOADMOD mLoadModFun;
	AEEHANDLER mHandlerFun;
	char       mModPath[MAX_PATH];

	boolean    mMinimized;
} AppModule;

int AppModule_CreateInstance(IShell *pIShell,void* ph,char* path,AppModule** module);
int AppModule_StartApplet(AppModule* module,AEECLSID cls);
void AppModule_Release(AppModule* module);
int AppModule_CloseApplet(AppModule* module);

#endif