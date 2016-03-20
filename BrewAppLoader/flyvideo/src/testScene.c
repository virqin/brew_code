#include "testScene.h"
#include "ui_dialog.h"

int init_test_sn_scene_data(Bff_Scene *scene);
void destory_test_sn_scene_data(Bff_Scene *scene);


int test_sn_scene_activateScene(Bff *bff,Bff_Task *task,int aa)
{
	Bff_Scene* self;
	int ret;
	ret = EFAILED;
	if(activate_scene(task,TEST_SCENE_NAME,test_sn_scene_setupDriver) == SUCCESS)
	{


		TestSceneData* data;
		self = bff->getSceneByName(bff,TEST_SCENE_NAME);
		data  = self->getData(self);
		if(!data)
		{
			init_test_sn_scene_data(self);
		}
		//save the input paras
		//NOTE: you cannot access the saved input paras in xxx_scene_initialize()
		//a good place to access the saved input paras is xxx_scene_enter()
		((TestSceneData*)self->getData(self))->aaa = aa;
	}
	return ret;

}

void test_sn_scene_setupDriver(Bff_Scene_Driver *driver)
{
	driver->initialize = test_sn_scene_initialize;
	driver->finalize = test_sn_scene_finalize;
	driver->getName = test_sn_scene_getName;
	driver->getHandlers = test_sn_scene_getHandlers;
	driver->enter = test_sn_scene_enter;
	driver->leave = test_sn_scene_leave;

}

int test_sn_scene_initialize(Bff *bff, Bff_Scene *scene)
{
	return init_test_sn_scene_data(scene);
}

void test_sn_scene_finalize(Bff *bff, Bff_Scene *scene)
{
	destory_test_sn_scene_data(scene);
}

int init_test_sn_scene_data(Bff_Scene *scene)
{
	TestSceneData *data;
	/* Allocate a memory area for scene-specific data. */
	data = MALLOC(sizeof(*data));
	if (data == NULL)
	{
		/* Failed to allocate a memory area. */
		return ENOMEMORY;
	}
	/* Initialize the scene-specific data. */
	data->commonSceneData.prevScene = NULL;

	/* Register the memory area into the scene. */
	scene->setData(scene, data);

	return SUCCESS;

}
void destory_test_sn_scene_data(Bff_Scene *scene)
{
	TestSceneData *data;

	/* Retrieve scene-specific data. */
	data = (TestSceneData *)scene->getData(scene);
	if (data)
	{
		/* Scene-specific data was found. Release it. */
		FREE(data);
		scene->setData(scene, NULL);
	}
}

int test_sn_scene_getName(Bff *bff, Bff_Scene *scene, char *name, int size)
{
	SNPRINTF(name, size, "%s", TEST_SCENE_NAME);
	name[size-1] = '\0';
	return SUCCESS;
}

int test_sn_scene_getHandlers(Bff *bff, Bff_Scene *scene, Bff_Handlers *handlers)
{
	handlers->onKey = test_sn_scene_pre_onKey;
	handlers->onPenDown = test_sn_scene_pre_onPenDown;
	handlers->onPointerDown = test_sn_scene_pre_onPointerDown;
	return SUCCESS;

}

boolean test_sn_scene_pre_onPenDown(Bff *bff, int16 x, int16 y, uint16 milliseconds,boolean *consumed, uint16 w, uint32 dw)
{
	return test_sn_scene_pre_onPointerDown(bff,x,y,milliseconds,consumed,w,dw);
}

boolean test_sn_scene_pre_onPointerDown(Bff *bff, int16 x, int16 y, uint16 milliseconds,boolean *consumed, uint16 w, uint32 dw)
{

	if (y < DEVICE_HEIGHT/2 && current_dlg_type() == ESEDlgNone)
	{
		AECHAR *queryMsg = NULL;
		initStringByID(&queryMsg,IDS_APP_NAME);
		query_dlg_run(queryMsg,NULL, NULL);
	}
	//TODO
	return (*consumed = TRUE);
}

boolean test_sn_scene_pre_onKey(Bff *bff, AVKType key, uint32 modifiers, boolean *consumed)
{
	//TODO
	return (*consumed = TRUE);
}

void test_sn_scene_enter(Bff *bff, Bff_Scene *scene, Bff_Scene *prev, void *arg)
{
	//TODO Other
	test_sn_scene_draw(bff,scene);
}

void test_sn_scene_leave(Bff *bff, Bff_Scene *scene, Bff_Scene *next)
{
	destory_test_sn_scene_data(scene);
	bff->releaseScene(bff,scene);
}

void test_sn_scene_draw(Bff *bff, Bff_Scene *scene)
{
	/*clear screen*/
	IDisplay *display = BFF_DISPLAY_OF(bff);
	AECHAR IMSI[50];
	ITAPI* tapi = NULL;
	
	TestSceneData* data;

	
	data = (TestSceneData*)scene->getData(scene);
	

	IDISPLAY_SetColor(display, CLR_USER_BACKGROUND, MAKE_RGB(0xdd,0xdd,0xdd));
	IDISPLAY_ClearScreen(display);

	if(ISHELL_CreateInstance(BFF_SHELL(), AEECLSID_TAPI, (void **)&tapi)==SUCCESS)
	{
		TAPIStatus status;
		if(ITAPI_GetStatus(tapi,&status)==SUCCESS)
		{
			STRTOWSTR(status.szMobileID,IMSI,sizeof(IMSI)*sizeof(AECHAR));
		}
		ITAPI_Release(tapi);
	}
	drawString(IMSI,20,20,MAKE_RGB(0xFF,0xFF,0xFF),MAKE_RGB(0x00,0x00,0x00),AEE_FONT_NORMAL,IDS_LEFT|IDS_TOP);
	//TODO

	IDISPLAY_Update(BFF_DISPLAY_OF(bff));	

}