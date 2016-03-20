#include "mainScene.h"
#include "mainTabScene.h"
#include "ui_dialog.h"

int init_main_sn_scene_data(Bff_Scene *scene);
void destory_main_sn_scene_data(Bff_Scene *scene);


int main_sn_scene_activateScene(Bff *bff,Bff_Task *task)
{
	Bff_Scene* self;
	int ret;
	ret = EFAILED;
	if(activate_scene(task,MAIN_SCENE_NAME,main_sn_scene_setupDriver) == SUCCESS)
	{
		self = bff->getSceneByName(bff,MAIN_SCENE_NAME);

		//save the input paras
		//NOTE: you cannot access the saved input paras in xxx_scene_initialize()
		//a good place to access the saved input paras is xxx_scene_enter()
		ret = SUCCESS;
	}
	return ret;

}

void main_sn_scene_setupDriver(Bff_Scene_Driver *driver)
{
	driver->initialize = main_sn_scene_initialize;
	driver->finalize = main_sn_scene_finalize;
	driver->getName = main_sn_scene_getName;
	driver->getHandlers = main_sn_scene_getHandlers;
	driver->enter = main_sn_scene_enter;
	driver->leave = main_sn_scene_leave;

}

int main_sn_scene_initialize(Bff *bff, Bff_Scene *scene)
{
	return init_main_sn_scene_data(scene);
}

void main_sn_scene_finalize(Bff *bff, Bff_Scene *scene)
{
	destory_main_sn_scene_data(scene);
}

int init_main_sn_scene_data(Bff_Scene *scene)
{
	MainSceneData *data;
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
void destory_main_sn_scene_data(Bff_Scene *scene)
{
	MainSceneData *data;

	/* Retrieve scene-specific data. */
	data = (MainSceneData *)scene->getData(scene);
	if (data)
	{
		/* Scene-specific data was found. Release it. */
		FREE(data);
		scene->setData(scene, NULL);
	}
}

int main_sn_scene_getName(Bff *bff, Bff_Scene *scene, char *name, int size)
{
	SNPRINTF(name, size, "%s", MAIN_SCENE_NAME);
	name[size-1] = '\0';
	return SUCCESS;
}

int main_sn_scene_getHandlers(Bff *bff, Bff_Scene *scene, Bff_Handlers *handlers)
{
	handlers->onKey = main_sn_scene_pre_onKey;
	handlers->onPenDown = main_sn_scene_pre_onPenDown;
	handlers->onPointerDown = main_sn_scene_pre_onPointerDown;
	return SUCCESS;

}

boolean main_sn_scene_pre_onPenDown(Bff *bff, int16 x, int16 y, uint16 milliseconds,boolean *consumed, uint16 w, uint32 dw)
{
	return main_sn_scene_pre_onPointerDown(bff,x,y,milliseconds,consumed,w,dw);
}

boolean main_sn_scene_pre_onPointerDown(Bff *bff, int16 x, int16 y, uint16 milliseconds,boolean *consumed, uint16 w, uint32 dw)
{

	if (y < DEVICE_HEIGHT/2 && current_dlg_type() == ESEDlgNone)
	{
		AECHAR *queryMsg = NULL;
		initStringByID(&queryMsg,IDS_APP_NAME);
		query_dlg_run(queryMsg,NULL, NULL);
	}
	else if (y > DEVICE_HEIGHT/2)
	{
		MainTabSceneInputParas paras;
		paras.index = 10;
		main_tab_sn_scene_activateScene(bff,bff->getCurrentTask(bff),&paras);
	}
	//TODO
	return (*consumed = TRUE);
}

boolean main_sn_scene_pre_onKey(Bff *bff, AVKType key, uint32 modifiers, boolean *consumed)
{
	//TODO
	return (*consumed = TRUE);
}

void main_sn_scene_enter(Bff *bff, Bff_Scene *scene, Bff_Scene *prev, void *arg)
{
	//TODO Other
	main_sn_scene_draw(bff,scene);
}

void main_sn_scene_leave(Bff *bff, Bff_Scene *scene, Bff_Scene *next)
{
	destory_main_sn_scene_data(scene);
	bff->releaseScene(bff,scene);
}

void main_sn_scene_draw(Bff *bff, Bff_Scene *scene)
{
	/*clear screen*/
	IDisplay *display = BFF_DISPLAY_OF(bff);
	AECHAR IMSI[50];
	ITAPI* tapi = NULL;

	IDISPLAY_SetColor(display, CLR_USER_BACKGROUND, MAKE_RGB(0xFF,0xFF,0xFF));
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