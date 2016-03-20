//#include "splashScene.h"
#include "mainApplet.h"
#include "mainScene.h"

/*------------------------------------------------------------------------*
* Declarations
*------------------------------------------------------------------------*/
/*创建程序实例*/
static int mainApplet_createInstance(AEECLSID classId, IShell *shell, IModule *module, void **object);

/*程序初始化*/
static int  mainApplet_initialize(Bff *bff);
/*程序析构*/
static void mainApplet_finalize(Bff *bff);
/*程序Task注册函数*/
static int  mainApplet_getTaskDriver(Bff *bff, Bff_Task_Id taskId, Bff_Task_Driver *driver);

/*程序系统事件注册函数*/
static int task_getPreHandlers(Bff *bff, Bff_Task *task, uint16 flags, AEEAppStart *appStart,Bff_Handlers *handlers);
/*程序开始事件*/
static boolean task_pre_onAppStart( Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed);
/*程序挂起事件*/
static boolean task_suspend(Bff *bff, AEESuspendReason reason, AEESuspendInfo *info,boolean *consumed);
/*程序唤醒事件*/
static boolean task_resume(Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed);
/*Task初始化*/
static int task_initialize(Bff *bff, Bff_Task *task, uint16 flags, AEEAppStart *appStart);
/*Task析构*/
static void task_finalize(Bff *bff, Bff_Task *task);

static void _suspend_backup_screen();

static void _ReleaseGlobalData(MainAppData* data);

/*------------------------------------------------------------------------*
* Implementations
*------------------------------------------------------------------------*/
static int mainApplet_createInstance(AEECLSID classId, IShell *shell, IModule *module, void **object)
{
	Bff_Applet_Driver driver;

	/* Set up BFF Applet driver. */
	Bff_Applet_Driver_clear(&driver);
	driver.initialize = mainApplet_initialize;
	driver.finalize = mainApplet_finalize;
	driver.getTaskDriver  = mainApplet_getTaskDriver;
	/* Create a BFF-based IApplet instance. */
	return Bff_Applet_create(classId, shell, module, object, &driver);
}


static int  mainApplet_initialize(Bff *bff)
{
	MainAppData* data;
	/* Allocate a memory area for applet global data. */
	data = MALLOC(sizeof(*data));
	if (data == NULL)
	{
		/* Failed to allocate a memory area. */
		return ENOMEMORY;
	}

	/* Register the memory area into the applet. */
	bff->setData(bff, data);
	se_dlg_init();
	return SUCCESS;
}
static void mainApplet_finalize(Bff *bff)
{
	MainAppData *data;
	data = (MainAppData *)bff->getData(bff);
	_ReleaseGlobalData(data);
	bff->setData(bff, NULL);

}
static int  mainApplet_getTaskDriver(Bff *bff, Bff_Task_Id taskId, Bff_Task_Driver *driver)
{
	driver->initialize     = task_initialize;
	driver->finalize       = task_finalize;
	driver->getPreHandlers = task_getPreHandlers;
	return SUCCESS;
}

static boolean task_suspend(Bff *bff, AEESuspendReason reason, AEESuspendInfo *info,boolean *consumed)
{
	_suspend_backup_screen();
	return (*consumed = TRUE);
}

static boolean task_resume(Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed)
{
	if(GetMainAppData()->pScreenBackup)
	{
		IDISPLAY_ClearScreen(BFF_DISPLAY());
		IDISPLAY_BitBlt(BFF_DISPLAY(),0,0,getDeviceWidth(),getDeviceHeight(),GetMainAppData()->pScreenBackup,0,0,AEE_RO_COPY);
		IDISPLAY_Update(BFF_DISPLAY());
		IBITMAP_Release(GetMainAppData()->pScreenBackup);
		GetMainAppData()->pScreenBackup = NULL;
	}
	return (*consumed = TRUE);
}

static void _suspend_backup_screen()
{
	IBitmap* pDeviceBitmap;
	IDISPLAY_GetDeviceBitmap(BFF_DISPLAY(),&pDeviceBitmap);
	IBITMAP_CreateCompatibleBitmap( pDeviceBitmap, &GetMainAppData()->pScreenBackup, getDeviceWidth(), getDeviceHeight());
	IBITMAP_BltIn( GetMainAppData()->pScreenBackup, 0, 0, getDeviceWidth(),getDeviceHeight(), pDeviceBitmap, 0, 0, AEE_RO_COPY );
	IBITMAP_Release( pDeviceBitmap );
}


static int task_getPreHandlers(Bff *bff, Bff_Task *task, uint16 flags, AEEAppStart *appStart,Bff_Handlers *handlers)
{
	handlers->onAppStart = task_pre_onAppStart;
	handlers->onAppSuspend = task_suspend;
	handlers->onAppResume = task_resume;
	return SUCCESS;
}

static int task_initialize(Bff *bff, Bff_Task *task, uint16 flags, AEEAppStart *appStart)
{
	bff->setLoggingLevel(bff, Bff_Logging_Level_NONE);
	return main_sn_scene_activateScene(bff,task);
}


static void task_finalize(Bff *bff, Bff_Task *task)
{
	bff->releaseSceneByName(bff, MAIN_SCENE_NAME);
	bff->releaseSceneByName(bff, MAIN_TAB_SCENE_NAME);
}

static boolean task_pre_onKey(Bff *bff, AVKType key, uint32 modifiers, boolean *consumed)
{
	return (*consumed = FALSE);
}

static boolean task_pre_onAppStart(Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed)
{
	return (*consumed = TRUE);
}


//程序入口
int AEEClsCreateInstance(AEECLSID classId, IShell *shell, IModule *module, void **object)
{
	return mainApplet_createInstance(classId, shell, module, object);
}


MainAppData* GetMainAppData()
{
	return (MainAppData*)BFF()->getData(BFF());
}

static void _ReleaseGlobalData(MainAppData* data)
{
	if(!data) return;
	se_dlg_finalize();
	FREE(data);
}