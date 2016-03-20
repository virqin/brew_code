#ifndef MAINSCENE_H
#define MAINSCENE_H


#include "sceneCommon.h"

typedef struct _mainSceneData
{
	CommonSceneData commonSceneData;
	//TODO: add more
}	MainSceneData;

int main_sn_scene_activateScene(Bff *bff,Bff_Task *task);

void main_sn_scene_setupDriver(Bff_Scene_Driver *driver);

int main_sn_scene_initialize(Bff *bff, Bff_Scene *scene);

void main_sn_scene_finalize(Bff *bff, Bff_Scene *scene);

int main_sn_scene_getName(Bff *bff, Bff_Scene *scene, char *name, int size);

int main_sn_scene_getHandlers(Bff *bff, Bff_Scene *scene, Bff_Handlers *handlers);

boolean main_sn_scene_pre_onPenDown(Bff *bff, int16 x, int16 y, uint16 milliseconds,boolean *consumed, uint16 w, uint32 dw);

boolean main_sn_scene_pre_onPointerDown(Bff *bff, int16 x, int16 y, uint16 milliseconds,boolean *consumed, uint16 w, uint32 dw);

boolean main_sn_scene_pre_onKey(Bff *bff, AVKType key, uint32 modifiers, boolean *consumed);

void main_sn_scene_enter(Bff *bff, Bff_Scene *scene, Bff_Scene *prev, void *arg);

void main_sn_scene_leave(Bff *bff, Bff_Scene *scene, Bff_Scene *next);

void main_sn_scene_draw(Bff *bff, Bff_Scene *scene);



#endif