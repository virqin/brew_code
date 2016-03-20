#ifndef SCENECOMMON_H
#define SCENECOMMON_H

#include "framework.h"
#include "sceneUtils.h"
#include "sourceUtils.h"

#define MAIN_SCENE_NAME            "main_sn"
#define MAIN_TAB_SCENE_NAME        "main_tab_sn"
#define TEST_SCENE_NAME            "test_sn"

//TODO : add more scene names

typedef struct _commonSceneData
{
	Bff_Scene *prevScene;
	//TODO : add more later?
} CommonSceneData;

int activate_scene(Bff_Task *task, const char *name,void (*setupDriver)(Bff_Scene_Driver *));

int activate_prevScene(Bff_Task *task);




#endif /*SCENECOMMON_H*/