/*
 * Copyright (c) 2008 Takahiko Kawasaki
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


/**
 * @english
 * BREW Foundation Framework
 * @endenglish
 *
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_H
#define BFF_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEAppGen.h"
#include "AEEStdLib.h"
#include "bff/bff-logging.h"
#include "bff/bff-types.h"


/*------------------------------------------------------------------------*
 * MACROS
 *------------------------------------------------------------------------*/
/**
 * @english
 * Get the pointer to the Bff instance.
 * @endenglish
 *
 */

/*
struct _Bff;
typedef struct _Bff Bff;*/



#define BFF()  ((Bff *)GETAPPINSTANCE())


/*------------------------------------------------------------------------*
 * STRUCT DECLARATIONS
 *------------------------------------------------------------------------*/



/*------------------------------------------------------------------------*
 * STRUCTS
 *------------------------------------------------------------------------*/
/**
 * @english
 * BREW Foundation Framework
 * @endenglish
 *
 */
typedef struct _Bff
{
    /**
     * @english
     * Implementation of IApplet interface.
     *
     * This data field must be located at the top of this structure so
     * that this structure can be cast by AEEApplet.
     * @endenglish
     *
     */
    AEEApplet applet;


    /*-------------------------------------------*
     * Methods
     *-------------------------------------------*/

    /**
     * @english
     * Create a scene.
     *
     * During the process of scene creation, some of driver functions
     * given by 'driver' are called in the order shown below.
     *
     *   - Bff_Scene_Driver.getName()
     *   - Bff_Scene_Driver.getHandlers()
     *   - Bff_Scene_Driver.initialize()
     *
     * A scene should be released by Bff.releaseScene() after use.
     *
     * @param[in]  self    The BFF instance.
     * @param[out] scene   A pointer to a newly created scene is returned
     *                     to the caller through this pointer. If NULL is
     *                     given, the pointer is not returned to the
     *                     caller, but it is still possible to get a
     *                     pointer of the new scene by scene name later
     *                     (Bff.getSceneByName()).
     * @param[in]  driver  Scene driver for the new scene. The content
     *                     of 'driver' is copied, so the caller does not
     *                     have to keep the content of 'driver' after
     *                     this function returns.
     *
     * @retval SUCCESS    Successfully created.
     * @retval EBADPARM   Either or both of 'self' and 'driver' are null.
     * @retval ENOMEMORY  Not enough memory.
     * @retval Others     Other error codes can be returned depending on
     *                    what the given scene driver return.
     * @endenglish
     *
     * @see Bff.releaseScene()
     */
    int (*createScene)(
        Bff *self, Bff_Scene **scene, Bff_Scene_Driver *driver);

    /**
     * @english
     * Free a memory area which has been allocated by Bff.malloc().
     *
     * If Bff_Applet_Driver.malloc() and Bff_Applet_Driver.free() are
     * both defined, Bff_Applet_Driver.free() is called from within
     * this method. Otherwise, BFF's default %free() implementation is
     * used.
     *
     * Bff.free() is used by Bff_Mem_free().
     *
     * @param[in] self  The BFF instance.
     * @param[in] area  A memory area to free.
     * @endenglish
     *
     *
     * @see Bff_Mem_free()
     * @see Bff_Applet_Driver.free()
     */
    void (*free)(Bff *self, void *area);

    /**
     * @english
     * Get the current scene.
     *
     * @param[in] self  The BFF instance.
     *
     * @return  A scene that is the current scene of the current task.
     *          NULL is returned if the current task does not hold any
     *          scene or if there is no running task. NULL is returned
     *          also if 'self' is null.
     * @endenglish
     *
     */
    Bff_Scene * (*getCurrentScene)(Bff *self);

    /**
     * @english
     * Get the current task.
     *
     * @param[in] self  The Bff instance.
     *
     * @return  The current task. NULL is returned if no task is running
     *          or if 'self' is null.
     * @endenglish
     *
     */
    Bff_Task * (*getCurrentTask)(Bff *self);

    /**
     * @english
     * Get the applet-specific data.
     *
     * The data set by the last call of Bff.setData() is returned. Note
     * that BREW Foundation Framework does not care about what data has
     * been stored by Bff.setData().
     *
     * This is not a must, but it is expected that the application code
     * allocates a memory area for its own data structure (e.g. struct
     * MyAppletData) and sets the pointer to the memory area by
     * Bff.setData() in the implementation of
     * Bff_Applet_Driver.initialize() (or somewhere else appropriate
     * for the application). The value of the pointer can be retrieved
     * later by Bff.getData(). Data stored in the data structure can be
     * treated as application-wide global data.
     *
     * Below is a sample code of Bff_Applet_Driver.initialize() and
     * Bff_Applet_Driver.finalize(). The two example functions call
     * Bff.setData() and Bff.getData(), respectively.
     * @endenglish
     *
     *
     * @code
     *   static int my_applet_initialize(Bff *bff)
     *   {
     *       struct MyAppletData *data;
     *
     *       data = Bff_Mem_malloc(sizeof(MyAppletData));
     *       if (data == NULL)
     *       {
     *           return ENOMEMORY;
     *       }
     *
     *       data->dataX = 1;
     *       data->dataY = 2;
     *       data->dataZ = 3;
     *
     *       bff->setData(bff, data);
     *
     *       return SUCCESS;
     *   }
     *
     *   static void my_applet_finalize(Bff *bff)
     *   {
     *       struct MyAppletData *data;
     *
     *       data = bff->getData(bff);
     *       if (data != NULL)
     *       {
     *           Bff_Mem_free(data);
     *           bff->setData(bff, NULL);  // Just in case.
     *       }
     *   }
     * @endcode
     *
     * @english
     * There are similar methods, Bff_Task.getData() and
     * Bff_Scene.getData(), but note that their data scopes are
     * different respectively.
     *
     * @param[in] self  The Bff instance.
     *
     * @return  The applet-specific data. The data set by the last call
     *          of Bff.setData() is returned. If Bff.setData() has not
     *          been called before, NULL is returned. If 'self' is null,
     *          NULL is returned.
     * @endenglish
     *
     *
     * @see Bff.setData()
     * @see Bff_Task.getData()
     * @see Bff_Task.setData()
     * @see Bff_Scene.getData()
     * @see Bff_Scene.setData()
     */
    void * (*getData)(Bff *self);

    /**
     * @english
     * Get the current logging level.
     *
     * @param[in] self  The Bff instance.
     *
     * @return  The current logging level. #Bff_Logging_Level_INVALID is
     *          returned if 'self' is null.
     * @endenglish
     *
     *
     * @see Bff.setLoggingLevel()
     * @see BFF_VERBOSE()
     * @see BFF_DEBUG()
     * @see BFF_INFO()
     * @see BFF_WARN()
     * @see BFF_ERROR()
     * @see BFF_FATAL()
     */
    Bff_Logging_Level (*getLoggingLevel)(Bff *self);

    /**
     * @english
     * Get a scene which has the specified ID.
     *
     * @param[in] self     The BFF instance.
     * @param[in] sceneId  A scene ID to look up a scene by.
     *
     * @return  A scene. If there is no scene whose ID is the same as
     *          'sceneId' or if 'self' is null, a null pointer is
     *          returned.
     * @endenglish
     *
     *
     * @see Bff_Scene.getId()
     */
    Bff_Scene * (*getSceneById)(Bff *self, Bff_Scene_Id sceneId);

    /**
     * @english
     * Get a scene which has the specified name.
     *
     * @param[in] self  The BFF instance.
     * @param[in] name  A name to look up a scene by.
     *
     * @return  A scene. If there is no scene whose name is the same as
     *          'name', a null pointer is returned. If either or both of
     *          'self' and 'name' are null, a null pointer is returned.
     *          Note BFF allows multiple scenes have the same name, so
     *          if there are multiple scenes which have the same name
     *          as 'name', which scene is returned depends on BFF's
     *          internal implementation.
     * @endenglish
     *
     *
     * @see Bff_Scene.getName()
     * @see Bff_Scene_Driver.getName()
     */
    Bff_Scene * (*getSceneByName)(Bff *self, const char *name);

    /**
     * @english
     * Find a task by a task ID.
     *
     * @param[in] self    The Bff instance.
     * @param[in] taskId  The task ID of the task to search for.
     *
     * @return  A pointer to a task whose task ID is idential to the
     *          specified one. NULL is returned if no task has the
     *          specified task ID. If 'self' is null, NULL is returned.
     * @endenglish
     *
     *
     * @see Bff_Task.getId()
     */
    Bff_Task * (*getTaskById)(Bff *self, Bff_Task_Id taskId);

    /**
     * @english
     * Find a task by a task name.
     *
     * @param[in] self  The Bff instance.
     * @param[in] name  The task name of the task to search for.
     *
     * @return  A pointer to a task whose task name is identical to the
     *          specified one. NULL is returned if no task has the
     *          specified task name. If 'self' is null, NULL is returned.
     *
     *          Note that it is possible that multiple tasks have the
     *          same name if Bff_Task_Driver.getName() is implemented
     *          in such a way.
     * @endenglish
     *
     *
     * @see Bff_Task.getName()
     * @see Bff_Task_Driver.getName()
     */
    Bff_Task * (*getTaskByName)(Bff *self, const char *name);

    /**
     * @english
     * Allocate a memory area.
     *
     * If Bff_Applet_Driver.malloc() and Bff_Applet_Driver.free() are
     * both defined, Bff_Applet_Driver.malloc() is called from within
     * this method. Otherwise, BFF's default %malloc() implementation
     * is used.
     *
     * Bff.malloc() is used by Bff_Mem_malloc().
     *
     * @param[in] self  The BFF instance.
     * @param[in] size  The size of the memory area to allocate.
     *
     * @return  A pointer to a memory area newly allocated. NULL is
     *          returned if memory allocation failed. If 'self' is
     *          null, a null pointer is returned.
     * @endenglish
     *
     *
     * @see Bff_Mem_malloc()
     * @see Bff_Applet_Driver.malloc()
     */
    void * (*malloc)(Bff *self, size_t size);

    /**
     * @english
     * Release a scene created by Bff.createScene().
     *
     * @param[in] self   The BFF instance.
     * @param[in] scene  A scene to release.
     *
     * @retval SUCCESS    The given scene was successfully released.
     * @retval EBADPARM   Either or both of the arguments are null.
     * @retval EBADSTATE  The specified scene is a current scene or
     *                    a scheduled scene of a task.
     * @endenglish
     *
     *
     * @see Bff.createScene()
     * @see Bff.releaseSceneById()
     * @see Bff.releaseSceneByName()
     */
    int (*releaseScene)(Bff *self, Bff_Scene *scene);

    /**
     * @english
     * Release a scene created by Bff.createScene().
     *
     * @param[in] self     The BFF instance.
     * @param[in] sceneId  ID of a scene to release.
     *
     * @retval SUCCESS    The given scene was successfully released.
     * @retval EBADPARM   'self' is nul.
     * @retval EBADITEM   No scene has the specified ID.
     * @retval EBADSTATE  The specified scene is a current scene or
     *                    a scheduled scene of a task.
     * @endenglish
     *
     *
     * @see Bff.createScene()
     * @see Bff.releaseScene()
     * @see Bff.releaseSceneByName()
     */
    int (*releaseSceneById)(Bff *self, Bff_Scene_Id sceneId);

    /**
     * @english
     * Release a scene created by Bff.createScene().
     *
     * Note that BFF allows name duplication of BFF Scenes, so if multiple
     * scenes have the same name as the specified one, which scene is
     * released depends on BFF's internal implementation.
     *
     * @param[in] self  The BFF instance.
     * @param[in] name  Name of A scene to release.
     *
     * @retval SUCCESS    The given scene was successfully released.
     * @retval EBADPARM   Either or both of the arguments are null.
     * @retval EBATITEM   No scene has the specified name.
     * @retval EBADSTATE  The specified scene is a current scene or
     *                    a scheduled scene of a task.
     * @endenglish
     *
     *
     * @see Bff.createScene()
     * @see Bff.releaseScene()
     * @see Bff.releaseSceneById()
     */
    int (*releaseSceneByName)(Bff *self, const char *name);

    /**
     * @english
     * Set applet-specific data.
     *
     * Data set by this method can be retrieved later by Bff.getData().
     *
     * There are similar methods, Bff_Task.setData() and
     * Bff_Scene.setData(), but note that their data scopes are
     * different respectively.
     *
     * @param[in] self  The Bff instance.
     * @param[in] data  Applet-specific data.
     * @endenglish
     *
     *
     * @see Bff.getData()
     * @see Bff_Task.getData()
     * @see Bff_Task.setData()
     * @see Bff_Scene.getData()
     * @see Bff_Scene.setData()
     */
    void (*setData)(Bff *self, void *data);

    /**
     * @english
     * Set the current logging level.
     *
     * The given value changes behaviors of the logging macros listed
     * below. See bff-logging.h for details.
     *
     *   - BFF_VERBOSE()
     *   - BFF_DEBUG()
     *   - BFF_INFO()
     *   - BFF_WARN()
     *   - BFF_ERROR()
     *   - BFF_FATAL()
     *
     * @param[in] self   The Bff instance.
     * @param[in] level  A logging level to set.
     *
     * @retval SUCCESS   Successfully set.
     * @retval EBADPARM  'self' is null or 'level' is out of range.
     * @endenglish
     *
     *
     * @see Bff.getLoggingLevel()
     * @see BFF_VERBOSE()
     * @see BFF_DEBUG()
     * @see BFF_INFO()
     * @see BFF_WARN()
     * @see BFF_ERROR()
     * @see BFF_FATAL()
     */
    int (*setLoggingLevel)(Bff *self, Bff_Logging_Level level);
};


/**
 * @english
 * @mainpage
 *
 * <b>BREW Foundation Framework</b> provides a multitasking-aware kernel
 * which supports a layered, per-task and per-state event dispatching
 * mechanism to enable robust, state-pattern programming in BREW.
 *
 * <ol>
 * <li>@ref introduction
 * <li>@ref entry_point
 * <li>@ref event_dispatching
 * <li>@ref multitasking
 * <li>@ref scene_switching
 * <li>@ref sample_programs
 * <li><a href="http://sourceforge.net/project/showfiles.php?group_id=246449"
 *     ><b>Download</b></a>
 * </ol>
 *
 * <ul>
 * <li><a href="../../bff-ja/html/index.html">日本語ページ</a> (Japanese)
 * </ul>
 * @endenglish
 */


/**
 * @english
 * @page introduction Introduction
 *
 * <div style="margin: 0 20px 20px 20px; float: right;">
 * @image html bff-app-arch.png "Architecture of a BFF-based application"
 * </div>
 *
 * <b>BREW Foundation Framework</b> is a piece of software written using
 * standard BREW API. The purpose of this software is to make it easier
 * to develop BREW applications by introducing some programming concepts
 * to make up for the simplicity of BREW AEE (Application Execution
 * Environment) with.
 *
 * BREW AEE expects that a BREW application implements an event handler
 * and calls the event handler with some arguments every time it wants
 * to deliver an event to an application. No matter what type of event,
 * all events are delivered to an application through the event handler.
 * This mechanism may have succeeded in making it easy to implement BREW
 * AEE, but as a trade-off, it has put a heavy burden on BREW application
 * programmers.
 *
 * BREW AEE delivers various kind of events. Some are related to
 * application life cycle (start/stop/suspend/resume), others are related
 * to key events, menu events, dialogue events, and so on. Other modern
 * platforms than BREW provide sophisticated mechanisms to wrap such
 * uncategorized events, deliver them to appropriate objects and invoke
 * pre-registered callback functions. However, BREW application
 * programmers have to implement such mechanisms from scratch per
 * application.
 *
 * BREW Foundation Framework, BFF, provides a generic mechanism to escape
 * from such a hard world. BFF enables an implementation of a BREW
 * application to be divided into multiple small pieces each of which can
 * have their own event handlers and thus can focus on their own specific
 * jobs. Two major categories of the small pieces are <b>BFF Task</b>
 * (Bff_Task) and <b>BFF Scene</b> (Bff_Scene). The former corresponds to
 * an application instance (an entry of IAppHistory) and the latter to an
 * application state. These concepts naturally encourage BREW programmers
 * to implement their applications using "state" pattern, which is one of
 * programming design patterns. The state pattern programming style makes
 * applications easier to understand and maintain and prevents "state
 * explosion" problems where too many flags to represent various kinds of
 * application states are introduced and it becomes almost impossible to
 * tell what happens at a given timing.
 *
 * The figure in this page illustrates what the architecture of a BFF-based
 * application looks like.
 *
 * Please make use of BREW Foundation Framework to make your life easier
 * <span style="font-family: monospace;">;-)</span>
 * @endenglish
 */


/**
 * @english
 * @page sample_programs Sample Programs
 * <ol>
 * <li>@ref sample_program__getting_started
 * <li> @ref sample_program__hello_world_applet
 * <li> @ref sample_program__hello_world_task
 * <li> @ref sample_program__hello_world_scene
 * <li> @ref sample_program__multitasking
 * <li>@ref sample_program__scene_switching
 * </ol>
 * @endenglish
 */
/**
 * @english
 * @page sample_program__getting_started Sample Program: Getting Started
 * @include getting-started.c
 * @endenglish
 */
/**
 * @english
 * @page sample_program__hello_world_applet Sample Program: Hellow World Applet
 * @include hello-world-applet.c
 * @endenglish
 */
/**
 * @english
 * @page sample_program__hello_world_task Sample Program: Hello World Task
 * @include hello-world-task.c
 * @endenglish
 */
/**
 * @english
 * @page sample_program__hello_world_scene Sample Program: Hello World Scene
 * @include hello-world-scene.c
 * @endenglish
 */
/**
 * @english
 * @page sample_program__multitasking Sample Program: Multitasking
 * @include multi-tasking.c
 * @endenglish
 */
/**
 * @english
 * @page sample_program__scene_switching Sample Program: Scene Switching
 * @include scene-switching.c
 * @endenglish
 */




#endif /* BFF_H */
