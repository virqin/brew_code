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
 * BREW Foundation Framework, Scene.
 *
 * @author Takahiko Kawasaki
 * @file
 */


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "bff/bff.h"
#include "bff/bff-accessors.h"
#include "bff/bff-dispatcher.h"
#include "bff/bff-handlers.h"
#include "bff/bff-logging.h"
#include "bff/bff-var.h"
#include "bffim/bffim-scene.h"
#include "bffim/bffim-task.h"


/*------------------------------------------------------------------------*
 * STATIC FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/

/*---------------------------------------------*
 * Initializers
 *---------------------------------------------*/
static int
setupScene(
    BffIm_Scene *instance, BffIm_Kernel *kernel, Bff_Scene_Id sceneId,
    Bff_Scene_Driver *driver);


static void
setupSceneMethods(BffIm_Scene *instance);


static void
setupSceneImMethods(BffIm_Scene *instance);


static int
setupSceneByDriver(
    BffIm_Scene *instance, BffIm_Kernel *kernel, Bff_Scene_Driver *driver);


static int
setupSceneName(BffIm_Scene *instance, Bff_Scene_Driver *driver, Bff *bff);


static int
setupSceneDispatcher(
    BffIm_Scene *instance, Bff_Scene_Driver *driver, Bff *bff);


static int
setupSceneInitialize(
    BffIm_Scene *instance, Bff_Scene_Driver *driver, Bff *bff);


/*---------------------------------------------*
 * Bff_Scene methods
 *---------------------------------------------*/
static int
method_scene_control(Bff_Scene *self, uint32 command, Bff_Var *args);


static void *
method_scene_getData(Bff_Scene *self);


static Bff_Scene_Id
method_scene_getId(Bff_Scene *self);


static const char *
method_scene_getName(Bff_Scene *self);


static Bff_Task *
method_scene_getTask(Bff_Scene *self);


static boolean
method_scene_isCurrent(Bff_Scene *self);


static boolean
method_scene_isScheduled(Bff_Scene *self);


static void
method_scene_setData(Bff_Scene *self, void *data);


/*---------------------------------------------*
 * BffIm_Scene methods
 *---------------------------------------------*/
static void
method_sceneim_finalize(BffIm_Scene *self);


static boolean
method_sceneim_handleEvent(
    BffIm_Scene *self, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed);


/*------------------------------------------------------------------------*
 * STATIC FUNCTIONS
 *------------------------------------------------------------------------*/


/*---------------------------------------------*
 * Initializers
 *---------------------------------------------*/
static int
setupScene(
    BffIm_Scene *instance, BffIm_Kernel *kernel, Bff_Scene_Id sceneId,
    Bff_Scene_Driver *driver)
{
    int ret;


    /* Set up methods of Bff_Scene structure. */
    setupSceneMethods(instance);

    /* Set up methods of BffIm_Scene structure. */
    setupSceneImMethods(instance);

    /* Assign a scene ID. */
    instance->id = sceneId;

    /* Clear all flags. */
    MEMSET(&instance->flags, 0, sizeof(instance->flags));

    /* Set the default value of scene-specific data. */
    instance->data = NULL;

    /* The previous scene in the scene list. */
    instance->prev = NULL;

    /* The next scene in the scene list. */
    instance->next = NULL;

    /* No task is associated yet. */
    instance->task = NULL;

    /*
     * Set the default value of scene name. SNPRINTF() guarantees
     * null-termination.
     */
    SNPRINTF(instance->name, sizeof(instance->name),
             BFFIM_SCENE_NAME_FORMAT, (long)sceneId);

    /* Copy the given driver to the internal data field. */
    instance->driver = *driver;

    /* Set up this scene using the scene driver. */
    ret = setupSceneByDriver(instance, kernel, driver);
    if (ret != SUCCESS)
    {
        /* Failed to set up the scene by driver. */
        return ret;
    }

    return SUCCESS;
}


static void
setupSceneMethods(BffIm_Scene *instance)
{
    Bff_Scene *scene = (Bff_Scene *)instance;


    scene->control     = method_scene_control;
    scene->getData     = method_scene_getData;
    scene->getId       = method_scene_getId;
    scene->getName     = method_scene_getName;
    scene->getTask     = method_scene_getTask;
    scene->isCurrent   = method_scene_isCurrent;
    scene->isScheduled = method_scene_isScheduled;
    scene->setData     = method_scene_setData;
}


static void
setupSceneImMethods(BffIm_Scene *instance)
{
    instance->finalize    = method_sceneim_finalize;
    instance->handleEvent = method_sceneim_handleEvent;
}


static int
setupSceneByDriver(
    BffIm_Scene *instance, BffIm_Kernel *kernel, Bff_Scene_Driver *driver)
{
    Bff *bff = (Bff *)kernel;
    int ret;


    /* Set up scene name. */
    ret = setupSceneName(instance, driver, bff);
    if (ret != SUCCESS)
    {
        /* Failed to set up scene name. */
        return ret;
    }

    /* Set up event handlers. */
    ret = setupSceneDispatcher(instance, driver, bff);
    if (ret != SUCCESS)
    {
        /* Failed to set up event handlers. */
        return ret;
    }

    /* Call initializer of the scene. */
    ret = setupSceneInitialize(instance, driver, bff);
    if (ret != SUCCESS)
    {
        /* Failed to initialize the scene by the driver. */
        return ret;
    }

    /* If a function to set up event handlers is defined. */
    if (driver->getHandlers)
    {
        /*
         * Scene dispatcher has been set up by the application.
         * The scene dispatcher is used in the process of event dispatching.
         */
        instance->flags.hasDispatcher = 1;
    }

    /*
     * Bff_Scene_Driver.finalize() needs to be called in the
     * finalization process.
     */
    instance->flags.needsFinalizerCall = 1;

    return SUCCESS;
}


static int
setupSceneName(BffIm_Scene *instance, Bff_Scene_Driver *driver, Bff *bff)
{
    /* Check if a function to setup scene name is defined. */
    if (driver->getName == NULL)
    {
        /* getName() is not defined. The default scene name is used. */
        return SUCCESS;
    }

    /* We are about to call getName(). */
    BFF_DEBUG(("BFF:Call getName() of Bff_Scene_Driver"));
    BFF_DEBUG(("BFF:  scene ID = %ld", (long)instance->id));

    /* Get the scene name. */
    driver->getName(bff, (Bff_Scene *)instance,
                    instance->name, sizeof(instance->name));

    /* Make sure the scene name is null-terminated. */
    instance->name[ sizeof(instance->name) - 1 ] = '\0';

    return SUCCESS;
}


static int
setupSceneDispatcher(
    BffIm_Scene *instance, Bff_Scene_Driver *driver, Bff *bff)
{
    Bff_Handlers handlers;
    int ret;


    /* Check if getHandlers() is defined. */
    if (driver->getHandlers == NULL)
    {
        /* getHandlers() is not defined. */
        return SUCCESS;
    }

    Bff_Handlers_clear(&handlers);

    /* We are about to call getHandlers(). */
    BFF_DEBUG(("BFF:Call getHandlers() of Bff_Scene_Driver"));
    BFF_DEBUG(("BFF:  scene ID = %ld", (long)instance->id));
    BFF_DEBUG(("BFF:  scene name = %s", instance->name));

    /* Make the application set up event handlers. */
    ret = driver->getHandlers(bff, (Bff_Scene *)instance, &handlers);
    if (ret != SUCCESS)
    {
        /* Failed to set up event handlers. */
        BFF_ERROR(("BFF:getHandlers() of Bff_Scene_Driver"));
        BFF_ERROR(("BFF:  scene ID = %ld", (long)instance->id));
        BFF_ERROR(("BFF:  scene name = %s", instance->name));

        return ret;
    }

    /* Initialize the dispatcher with the event handlers. */
    Bff_Dispatcher_initialize(&instance->dispatcher, &handlers);

    return SUCCESS;
}


static int
setupSceneInitialize(
    BffIm_Scene *instance, Bff_Scene_Driver *driver, Bff *bff)
{
    int ret;


    /* Check if initialize() is defined. */
    if (driver->initialize == NULL)
    {
        /* initialize() is not defined. */
        return SUCCESS;
    }

    /* We are about to call initialize(). */
    BFF_DEBUG(("BFF:Call initialize() of Bff_Scene_Driver"));
    BFF_DEBUG(("BFF:  scene ID = %ld", (long)instance->id));
    BFF_DEBUG(("BFF:  scene name = %s", instance->name));

    /* Call the initializer of the scene. */
    ret = driver->initialize(bff, (Bff_Scene *)instance);
    if (ret != SUCCESS)
    {
        /* The initializer failed. */
        BFF_ERROR(("BFF:initialize() of Bff_Scene_Driver"));
        BFF_ERROR(("BFF:  scene ID = %ld", (long)instance->id));
        BFF_ERROR(("BFF:  scene name = %s", instance->name));

        return ret;
    }

    return SUCCESS;
}


/*---------------------------------------------*
 * Bff_Scene methods
 *---------------------------------------------*/
static int
method_scene_control(Bff_Scene *self, uint32 command, Bff_Var *args)
{
    Bff_Scene_Driver *driver;


    if (self == NULL)
    {
        return EBADPARM;
    }

    driver = &((BffIm_Scene *)self)->driver;

    if (driver->control == NULL)
    {
        return EUNSUPPORTED;
    }

    /* We are about to call Bff_Scene_Driver.control(). */
    BFF_DEBUG(("BFF:Call control() of Bff_Scene_Driver"));
    BFF_DEBUG(("BFF:  scene ID = %ld", (long)self->getId(self)));
    BFF_DEBUG(("BFF:  scene name = %s", self->getName(self)));
    BFF_DEBUG(("BFF:  command = 0x%08lX", (unsigned long)command));

    return driver->control(BFF(), self, command, args);
}


static void *
method_scene_getData(Bff_Scene *self)
{
    if (self == NULL)
    {
        return NULL;
    }

    return ((BffIm_Scene *)self)->data;
}


static Bff_Scene_Id
method_scene_getId(Bff_Scene *self)
{
    if (self == NULL)
    {
        return Bff_Scene_Id_INVALID;
    }

    return ((BffIm_Scene *)self)->id;
}


static const char *
method_scene_getName(Bff_Scene *self)
{
    if (self == NULL)
    {
        return NULL;
    }

    return ((BffIm_Scene *)self)->name;
}


static Bff_Task *
method_scene_getTask(Bff_Scene *self)
{
    if (self == NULL)
    {
        return NULL;
    }

    return (Bff_Task *)((BffIm_Scene *)self)->task;
}


static boolean
method_scene_isCurrent(Bff_Scene *self)
{
    if (self == NULL)
    {
        return FALSE;
    }

    if (((BffIm_Scene *)self)->flags.current)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


static boolean
method_scene_isScheduled(Bff_Scene *self)
{
    if (self == NULL)
    {
        return FALSE;
    }

    if (((BffIm_Scene *)self)->flags.scheduled)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


static void
method_scene_setData(Bff_Scene *self, void *data)
{
    if (self == NULL)
    {
        return;
    }

    ((BffIm_Scene *)self)->data = data;
}


/*---------------------------------------------*
 * BffIm_Scene methods
 *---------------------------------------------*/
static void
method_sceneim_finalize(BffIm_Scene *self)
{
    if (self == NULL)
    {
        return;
    }

    if (self->flags.needsFinalizerCall && self->driver.finalize)
    {
        /* We are about to call Bff_Scene_Driver.finalize(). */
        BFF_DEBUG(("BFF:Call finalize() of Bff_Scene_Driver"));

        /* Call the finalizer of thies scene. */
        self->driver.finalize(BFF(), (Bff_Scene *)self);
    }
}


static boolean
method_sceneim_handleEvent(
    BffIm_Scene *self, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed)
{
    Bff_Dispatcher *dispatcher;


    if (!self || !ev || !w || !dw || !consumed)
    {
        if (consumed)
        {
            *consumed = FALSE;
        }

        return FALSE;
    }

    if (!self->flags.hasDispatcher)
    {
        return (*consumed = FALSE);
    }

    dispatcher = &self->dispatcher;

    return dispatcher->dispatch(dispatcher, BFF(), ev, w,  dw, consumed);
}


/*------------------------------------------------------------------------*
 * FUNCTIONS
 *------------------------------------------------------------------------*/
int
BffIm_Scene_initialize(
    BffIm_Scene *instance, BffIm_Kernel *kernel, Bff_Scene_Id sceneId,
    Bff_Scene_Driver *driver)
{
    int ret;


    /* Check if the given arguments are valid. */
    if (instance == NULL || kernel == NULL || driver == NULL)
    {
        /* The arguments are invalid. */
        return EBADPARM;
    }

    /* Set up scene. */
    ret = setupScene(instance, kernel, sceneId, driver);
    if (ret != SUCCESS)
    {
        /* Failed to set up a scene. */
        return ret;
    }

    return SUCCESS;
}
