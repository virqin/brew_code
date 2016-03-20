#include "sceneCommon.h"

int activate_scene(Bff_Task *task, const char *name,void (*setupDriver)(Bff_Scene_Driver *))
{
	Bff_Scene_Driver driver;
	int ret;

	/* Try to find a scene with the specified sceneId and schedule it. */
	ret = task->scheduleSceneByName(task, name, NULL);
	if (ret == EBADITEM)
	{
		/*
		* No scene with the name was found. This means that the
		* scene has not been created yet.
		*/

		/* Set up a scene driver. */
		Bff_Scene_Driver_clear(&driver);
		setupDriver(&driver);

		/* Create a new scene and schedule it. */
		ret = task->scheduleNewScene(task, NULL, &driver, NULL);
	}

	return ret;
}

int activate_prevScene(Bff_Task *task)
{
	Bff_Scene *scene = task->getCurrentScene(task);
	CommonSceneData *data  = (CommonSceneData *)scene->getData(scene);
	if (data->prevScene)
	{
		return task->scheduleScene(task, data->prevScene, NULL);
	}

	return SUCCESS;
}