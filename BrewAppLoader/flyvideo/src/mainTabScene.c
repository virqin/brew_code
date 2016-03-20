#include "mainTabScene.h"
#include "testScene.h"
#include "mainScene.h"

int init_main_tab_sn_scene_data(Bff_Scene *scene);
void destory_main_tab_sn_scene_data(Bff_Scene *scene);

int main_tab_sn_scene_activateScene(Bff *bff,Bff_Task *task,const MainTabSceneInputParas* inputParas)
{
	Bff_Scene* self;
	int ret;
	ret = EFAILED;
	if(activate_scene(task,MAIN_TAB_SCENE_NAME,main_tab_sn_scene_setupDriver) == SUCCESS)
	{
		MainTabSceneData* data;
		self = bff->getSceneByName(bff,MAIN_TAB_SCENE_NAME);
		data  = self->getData(self);
		if(!data)
		{
			init_main_tab_sn_scene_data(self);
		}
		//save the input paras
		//NOTE: you cannot access the saved input paras in xxx_scene_initialize()
		//a good place to access the saved input paras is xxx_scene_enter()
		if(inputParas != NULL)
		{
			((MainTabSceneInputParas*)self->getData(self))->index = inputParas->index;
		}
		ret = SUCCESS;

	}
	return ret;

}

void main_tab_sn_scene_setupDriver(Bff_Scene_Driver *driver)
{
	driver->initialize = main_tab_sn_scene_initialize;
	driver->finalize = main_tab_sn_scene_finalize;
	driver->getName = main_tab_sn_scene_getName;
	driver->getHandlers = main_tab_sn_scene_getHandlers;
	driver->enter = main_tab_sn_scene_enter;
	driver->leave = main_tab_sn_scene_leave;
}

int main_tab_sn_scene_initialize(Bff *bff, Bff_Scene *scene)
{
	return init_main_tab_sn_scene_data(scene);
}

void main_tab_sn_scene_finalize(Bff *bff, Bff_Scene *scene)
{
	destory_main_tab_sn_scene_data(scene);
}

int init_main_tab_sn_scene_data(Bff_Scene *scene)
{
	MainTabSceneData *data;
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
void destory_main_tab_sn_scene_data(Bff_Scene *scene)
{
	MainTabSceneData *data;

	/* Retrieve scene-specific data. */
	data = (MainTabSceneData *)scene->getData(scene);
	if (data)
	{
		/* Scene-specific data was found. Release it. */
		FREE(data);
		scene->setData(scene, NULL);
	}
}


int main_tab_sn_scene_getName(Bff *bff, Bff_Scene *scene, char *name, int size)
{
	SNPRINTF(name, size, "%s", MAIN_TAB_SCENE_NAME);
	name[size-1] = '\0';
	return SUCCESS;
}

int main_tab_sn_scene_getHandlers(Bff *bff, Bff_Scene *scene, Bff_Handlers *handlers)
{
	handlers->onKey = main_tab_sn_scene_pre_onKey;
	handlers->onPenDown = main_tab_sn_scene_pre_onPenDown;
	handlers->onPointerDown = main_tab_sn_scene_pre_onPointerDown;
	return SUCCESS;

}

boolean main_tab_sn_scene_pre_onPenDown(Bff *bff, int16 x, int16 y, uint16 milliseconds,boolean *consumed, uint16 w, uint32 dw)
{
	return main_tab_sn_scene_pre_onPointerDown(bff,x,y,milliseconds,consumed,w,dw);
}

boolean main_tab_sn_scene_pre_onPointerDown(Bff *bff, int16 x, int16 y, uint16 milliseconds,boolean *consumed, uint16 w, uint32 dw)
{
	if (x < DEVICE_WIDTH/2)
	{
		main_sn_scene_activateScene(bff,bff->getCurrentTask(bff));
	}
	else if (x > DEVICE_WIDTH/2)
	{
		test_sn_scene_activateScene(bff,bff->getCurrentTask(bff),12);
	}
	//TODO
	return (*consumed = TRUE);
}

boolean main_tab_sn_scene_pre_onKey(Bff *bff, AVKType key, uint32 modifiers, boolean *consumed)
{
	return (*consumed = TRUE);
}

void main_tab_sn_scene_enter(Bff *bff, Bff_Scene *scene, Bff_Scene *prev, void *arg)
{
	//TODO Other
	main_tab_sn_scene_draw(bff,scene);
}

void main_tab_sn_scene_leave(Bff *bff, Bff_Scene *scene, Bff_Scene *next)
{
	

}

void main_tab_sn_scene_draw(Bff *bff, Bff_Scene *scene)
{
	/*clear screen*/
	IDisplay *display = BFF_DISPLAY_OF(bff);
	IDISPLAY_SetColor(display, CLR_USER_BACKGROUND, MAKE_RGB(0x00,0x00,0x00));
	IDISPLAY_ClearScreen(display);

	//TODO

	IDISPLAY_Update(BFF_DISPLAY_OF(bff));	

}