#ifndef TESTSCENE_H
#define TESTSCENE_H


#include "sceneCommon.h"

typedef struct _testSceneData
{
	CommonSceneData commonSceneData;
	int aaa;
	//TODO: add more
}	TestSceneData;

int test_sn_scene_activateScene(Bff *bff,Bff_Task *task,int aa);

void test_sn_scene_setupDriver(Bff_Scene_Driver *driver);

int test_sn_scene_initialize(Bff *bff, Bff_Scene *scene);

void test_sn_scene_finalize(Bff *bff, Bff_Scene *scene);

int test_sn_scene_getName(Bff *bff, Bff_Scene *scene, char *name, int size);

int test_sn_scene_getHandlers(Bff *bff, Bff_Scene *scene, Bff_Handlers *handlers);

boolean test_sn_scene_pre_onPenDown(Bff *bff, int16 x, int16 y, uint16 milliseconds,boolean *consumed, uint16 w, uint32 dw);

boolean test_sn_scene_pre_onPointerDown(Bff *bff, int16 x, int16 y, uint16 milliseconds,boolean *consumed, uint16 w, uint32 dw);

boolean test_sn_scene_pre_onKey(Bff *bff, AVKType key, uint32 modifiers, boolean *consumed);

void test_sn_scene_enter(Bff *bff, Bff_Scene *scene, Bff_Scene *prev, void *arg);

void test_sn_scene_leave(Bff *bff, Bff_Scene *scene, Bff_Scene *next);

void test_sn_scene_draw(Bff *bff, Bff_Scene *scene);




#endif