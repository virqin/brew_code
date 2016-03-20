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
 * BREW Foundation Framework, Task.
 *
 * @author Takahiko Kawasaki
 * @file
 */


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEStdLib.h"
#include "bff/bff-accessors.h"
#include "bff/bff-dispatcher.h"
#include "bff/bff-handlers.h"
#include "bff/bff-logging.h"
#include "bff/bff-task.h"
#include "bff/bff-var.h"
#include "bffim/bffim-kernel.h"
#include "bffim/bffim-scene.h"
#include "bffim/bffim-task.h"


/*------------------------------------------------------------------------*
 * STATIC FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/

/*---------------------------------------------*
 * Initializers
 *---------------------------------------------*/
static int
setupTask(
    BffIm_Task *instance, BffIm_Kernel *kernel, Bff_Task_Id taskId,
    Bff_Task_Driver *driver, uint16 flags, AEEAppStart *appStart);


static void
setupTaskMethods(BffIm_Task *instance);


static void
setupTaskImMethods(BffIm_Task *instance);


static int
setupTaskByDriver(
    BffIm_Task *instance, BffIm_Kernel *kernel, Bff_Task_Driver *driver,
    uint16 flags, AEEAppStart *appStart);


static int
setupTaskName(
    BffIm_Task *instance, Bff_Task_Driver *driver, Bff *bff, uint16 flags,
    AEEAppStart *appStart);


static int
setupTaskPreDispatcher(
    BffIm_Task *instance, Bff_Task_Driver *driver, Bff *bff, uint16 flags,
    AEEAppStart *appStart);


static int
setupTaskPostDispatcher(
    BffIm_Task *instance, Bff_Task_Driver *driver, Bff *bff, uint16 flags,
    AEEAppStart *appStart);


static int
setupTaskInitialize(
    BffIm_Task *instance, Bff_Task_Driver *driver, Bff *bff, uint16 flags,
    AEEAppStart *appStart);


/*---------------------------------------------*
 * Bff_Task methods
 *---------------------------------------------*/
static int
method_task_blockSceneSwitching(Bff_Task *self, boolean block);


static int
method_task_control(Bff_Task *self, uint32 command, Bff_Var *args);


static Bff_Scene *
method_task_getCurrentScene(Bff_Task *self);


static void *
method_task_getData(Bff_Task *self);


static Bff_Task_Id
method_task_getId(Bff_Task *self);


static const char *
method_task_getName(Bff_Task *self);


static Bff_Scene *
method_task_getScheduledScene(Bff_Task *self);


static boolean
method_task_isFirst(Bff_Task *self);


static boolean
method_task_isSuspended(Bff_Task *self);


static int
method_task_scheduleNewScene(
    Bff_Task *self, Bff_Scene **scene, Bff_Scene_Driver *driver, void *arg);


static int
method_task_scheduleOneShotScene(
    Bff_Task *self, Bff_Scene **scene, Bff_Scene_Driver *driver, void *arg);


static int
method_task_scheduleScene(Bff_Task *self, Bff_Scene *scene, void *arg);


static int
method_task_scheduleSceneById(
    Bff_Task *self, Bff_Scene_Id sceneId, void *arg);


static int
method_task_scheduleSceneByName(
    Bff_Task *self, const char *name, void *arg);


static void
method_task_setData(Bff_Task *self, void *data);


/*---------------------------------------------*
 * BffIm_Task methods
 *---------------------------------------------*/
static void
method_taskim_finalize(BffIm_Task *self);


static boolean
method_taskim_handleEvent(
    BffIm_Task *self, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed);

static int
method_taskim_switchScenes(BffIm_Task *self);


/*---------------------------------------------*
 * Event dispatching
 *---------------------------------------------*/
static boolean
handleEventByScene(
    BffIm_Task *task, Bff *bff, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed);


/*---------------------------------------------*
 * Scene management
 *---------------------------------------------*/
static int
checkAndScheduleScene(Bff_Task *task, Bff_Scene *scene, void *arg);


static int
scheduleNewScene(
    BffIm_Task *task, BffIm_Scene **scene, Bff_Scene_Driver *driver,
    void *arg);

static int
scheduleScene(BffIm_Task *task, BffIm_Scene *scene, void *arg);


static void
cancelScheduledScene(BffIm_Task *task);


static int
switchScenes(BffIm_Task *task);


static void
logAboutSceneSwitching(
    BffIm_Task *task, BffIm_Scene *curr, BffIm_Scene *next);


static void
leaveCurrentScene(
    Bff *bff, BffIm_Task *task, BffIm_Scene *curr, BffIm_Scene *next);


static void
enterScheduledScene(
    Bff *bff, BffIm_Task *task, BffIm_Scene *curr, BffIm_Scene *next,
    void *arg);


static long
getSceneIdForLogging(BffIm_Scene *scene);


static const char *
getSceneNameForLogging(BffIm_Scene *scene);


/*------------------------------------------------------------------------*
 * STATIC FUNCTIONS
 *------------------------------------------------------------------------*/


/*---------------------------------------------*
 * Initializers
 *---------------------------------------------*/
static int
setupTask(
    BffIm_Task *instance, BffIm_Kernel *kernel, Bff_Task_Id taskId,
    Bff_Task_Driver *driver, uint16 flags, AEEAppStart *appStart)
{
    int ret;


    /* Set up methods of Bff_Task structure. */
    setupTaskMethods(instance);

    /* Set up methods of BffIm_Task structure. */
    setupTaskImMethods(instance);

    /* Assign a task ID. */
    instance->id = taskId;

    /* Clear all flags. */
    MEMSET(&instance->flags, 0, sizeof(instance->flags));

    /* Set the default value of task-specific data. */
    instance->data = NULL;

    /* The previous task. */
    instance->prev = NULL;

    /* The next task. */
    instance->next = NULL;

    /* Initialize the event nest level. */
    instance->eventNestLevel = 0;

    /* The current scene. No scene is running now. */
    instance->currentScene = NULL;

    /* No scene is scheduled yet. */
    instance->scheduledScene               = NULL;
    instance->scheduledSceneArg            = NULL;
    instance->scheduledSceneEventNestLevel = 0;

    /*
     * Set the default value of task name. SNPRINTF() guarantees
     * null-termination.
     */
    SNPRINTF(instance->name, sizeof(instance->name),
             BFFIM_TASK_NAME_FORMAT, (long)taskId);

    /* Copy the given driver to the internal data field. */
    instance->driver = *driver;

    /* Setup this task using task driver. */
    ret = setupTaskByDriver(instance, kernel, driver, flags, appStart);
    if (ret != SUCCESS)
    {
        /* Failed to set up the task by driver. */
        return ret;
    }

    /* Successfully set up. */
    return SUCCESS;
}


static void
setupTaskMethods(BffIm_Task *instance)
{
    Bff_Task *task = (Bff_Task *)instance;


    /* Set up methods of Bff_Task structure. */
    task->blockSceneSwitching  = method_task_blockSceneSwitching;
    task->control              = method_task_control;
    task->getCurrentScene      = method_task_getCurrentScene;
    task->getData              = method_task_getData;
    task->getId                = method_task_getId;
    task->getName              = method_task_getName;
    task->getScheduledScene    = method_task_getScheduledScene;
    task->isFirst              = method_task_isFirst;
    task->isSuspended          = method_task_isSuspended;
    task->scheduleNewScene     = method_task_scheduleNewScene;
    task->scheduleOneShotScene = method_task_scheduleOneShotScene;
    task->scheduleScene        = method_task_scheduleScene;
    task->scheduleSceneById    = method_task_scheduleSceneById;
    task->scheduleSceneByName  = method_task_scheduleSceneByName;
    task->setData              = method_task_setData;
}


static void
setupTaskImMethods(BffIm_Task *instance)
{
    /* Set up methods of BffIm_Task structure. */
    instance->finalize     = method_taskim_finalize;
    instance->handleEvent  = method_taskim_handleEvent;
    instance->switchScenes = method_taskim_switchScenes;
}


static int
setupTaskByDriver(
    BffIm_Task *instance, BffIm_Kernel *kernel, Bff_Task_Driver *driver,
    uint16 flags, AEEAppStart *appStart)
{
    Bff *bff = (Bff *)kernel;
    int ret;


    /* Set up task name. */
    ret = setupTaskName(instance, driver, bff, flags, appStart);
    if (ret != SUCCESS)
    {
        /* Failed to set up task name. */
        return ret;
    }

    /* Set up event handlers for pre-processing. */
    ret = setupTaskPreDispatcher(instance, driver, bff, flags, appStart);
    if (ret != SUCCESS)
    {
        /* Failed to set up event handlers for pre-processing. */
        return ret;
    }

    /* Set up event handlers for post-processing. */
    ret = setupTaskPostDispatcher(instance, driver, bff, flags, appStart);
    if (ret != SUCCESS)
    {
        /* Failed to set up event handlers for post-processing. */
        return ret;
    }

    /* Initialize the task by an application-defined initializer. */
    ret = setupTaskInitialize(instance, driver, bff, flags, appStart);
    if (ret != SUCCESS)
    {
        /* Failed to initialize the task. */
        return ret;
    }

    /* Initialization by task driver has been done successfully. */

    /* If a function to set up task pre event handlers is defined, */
    if (driver->getPreHandlers)
    {
        /*
         * Task pre-dispatcher has been set up by the application.
         * The pre-dispatcher is used in the process of event dispatching.
         */
        instance->flags.hasPreDispatcher = 1;
    }

    /* If a function to set up applet event post handlers is defined, */
    if (driver->getPostHandlers)
    {
        /*
         * Task post-dispatcher has been set up by the application.
         * The post-dispatcher is used in the process of event dispatching.
         */
        instance->flags.hasPostDispatcher = 1;
    }

    /*
     * Bff_Task_Driver.finalize() needs to be called
     * in the finalization process.
     */
    instance->flags.needsFinalizerCall = 1;

    return SUCCESS;
}


static int
setupTaskName(
    BffIm_Task *instance, Bff_Task_Driver *driver, Bff *bff, uint16 flags,
    AEEAppStart *appStart)
{
    /* Check if a function to set up task name is defined. */
    if (driver->getName == NULL)
    {
        /* getName() is not defined. The default task name is used. */
        return SUCCESS;
    }

    /* We are about to call getName(). */
    BFF_DEBUG(("BFF:Call getName() of Bff_Task_Driver"));
    BFF_DEBUG(("BFF:  task ID = %ld", (long)instance->id));

    /* Get the task name. */
    driver->getName(bff, (Bff_Task *)instance, flags, appStart,
                    instance->name, sizeof(instance->name));

    /* Make sure the name is null-terminated. */
    instance->name[ sizeof(instance->name) - 1 ] = '\0';

    return SUCCESS;
}


static int
setupTaskPreDispatcher(
    BffIm_Task *instance, Bff_Task_Driver *driver, Bff *bff, uint16 flags,
    AEEAppStart *appStart)
{
    Bff_Handlers handlers;
    int ret;


    /* Check if getPreHandlers() is defined. */
    if (driver->getPreHandlers == NULL)
    {
        /* getPreHandlers() is not defined. */
        return SUCCESS;
    }

    Bff_Handlers_clear(&handlers);

    /* We are about to call getPreHandlers(). */
    BFF_DEBUG(("BFF:Call getPreHandlers() of Bff_Task_Driver"));
    BFF_DEBUG(("BFF:  task ID = %ld", (long)instance->id));
    BFF_DEBUG(("BFF:  task name = %s", instance->name));

    /* Make the application set up event handlers for pre-processing. */
    ret = driver->getPreHandlers(bff, (Bff_Task *)instance,
                                 flags, appStart, &handlers);
    if (ret != SUCCESS)
    {
        /* Failed to set up event handlers for pre-processing. */
        BFF_ERROR(("BFF:getPreHandlers() of Bff_Task_Driver"));
        BFF_ERROR(("BFF:  task ID = %ld", (long)instance->id));
        BFF_ERROR(("BFF:  task name = %s", instance->name));

        return ret;
    }

    /* Initialize the pre-dispatcher with the event handlers. */
    Bff_Dispatcher_initialize(&instance->preDispatcher, &handlers);

    return SUCCESS;
}


static int
setupTaskPostDispatcher(
    BffIm_Task *instance, Bff_Task_Driver *driver, Bff *bff, uint16 flags,
    AEEAppStart *appStart)
{
    Bff_Handlers handlers;
    int ret;


    /* Check if getPostHandlers() is defined. */
    if (driver->getPostHandlers == NULL)
    {
        /* getPostHandlers() is not defined. */
        return SUCCESS;
    }

    Bff_Handlers_clear(&handlers);

    /* We are about to call getPostHandlers(). */
    BFF_DEBUG(("BFF:Call getPostHandlers() of Bff_Task_Driver"));
    BFF_DEBUG(("BFF:  task ID = %ld", (long)instance->id));
    BFF_DEBUG(("BFF:  task name = %s", instance->name));

    /* Make the application set up event handlers for post-processing. */
    ret = driver->getPostHandlers(bff, (Bff_Task *)instance,
                                  flags, appStart, &handlers);
    if (ret != SUCCESS)
    {
        /* Failed to set up event handlers for post-processing. */
        BFF_ERROR(("BFF:getPostHandlers() of Bff_Task_Driver"));
        BFF_ERROR(("BFF:  task ID = %ld", (long)instance->id));
        BFF_ERROR(("BFF:  task name = %s", instance->name));

        return ret;
    }

    /* Initialize the post-dispatcher with the event handlers. */
    Bff_Dispatcher_initialize(&instance->postDispatcher, &handlers);

    return SUCCESS;
}


static int
setupTaskInitialize(
    BffIm_Task *instance, Bff_Task_Driver *driver, Bff *bff, uint16 flags,
    AEEAppStart *appStart)
{
    int ret;


    /* Check if initialize() is defined. */
    if (driver->initialize == NULL)
    {
        /* initialize() is not defined. */
        return SUCCESS;
    }

    /* We are about to call initialize(). */
    BFF_DEBUG(("BFF:Call initialize() of Bff_Task_Driver"));
    BFF_DEBUG(("BFF:  task ID = %ld", (long)instance->id));
    BFF_DEBUG(("BFF:  task name = %s", instance->name));

    /* Call the initializer for the task. */
    ret = driver->initialize(bff, (Bff_Task *)instance, flags, appStart);
    if (ret != SUCCESS)
    {
        /* Failed to initialize the task. */
        BFF_ERROR(("BFF:initialize() of Bff_Task_Driver"));
        BFF_ERROR(("BFF:  task ID = %ld", (long)instance->id));
        BFF_ERROR(("BFF:  task name = %s", instance->name));

        return ret;
    }

    return SUCCESS;
}


/*---------------------------------------------*
 * Bff_Task methods
 *---------------------------------------------*/
static int
method_task_blockSceneSwitching(Bff_Task *self, boolean block)
{
    int newValue;


    if (self == NULL)
    {
        return EBADPARM;
    }

    newValue = (block ? 1 : 0);

    ((BffIm_Task *)self)->flags.switchingBlocked = newValue;

    return SUCCESS;
}


static int
method_task_control(Bff_Task *self, uint32 command, Bff_Var *args)
{
    Bff_Task_Driver *driver;


    if (self == NULL)
    {
        return EBADPARM;
    }

    driver = &((BffIm_Task *)self)->driver;

    if (driver->control == NULL)
    {
        return EUNSUPPORTED;
    }

    /* We are about to call Bff_Task_Driver.control(). */
    BFF_DEBUG(("BFF:Call control() of Bff_Task_Driver"));
    BFF_DEBUG(("BFF:  task ID = %ld", (long)self->getId(self)));
    BFF_DEBUG(("BFF:  task name = %s", self->getName(self)));
    BFF_DEBUG(("BFF:  command = 0x%08lX", (unsigned long)command));

    return driver->control(BFF(), self, command, args);
}


static Bff_Scene *
method_task_getCurrentScene(Bff_Task *self)
{
    if (self == NULL)
    {
        return NULL;
    }

    /* Get the current scene. */
    return (Bff_Scene *)((BffIm_Task *)self)->currentScene;
}


static void *
method_task_getData(Bff_Task *self)
{
    if (self == NULL)
    {
        return NULL;
    }

    /* Get task-specific data. */
    return ((BffIm_Task *)self)->data;
}


static Bff_Task_Id
method_task_getId(Bff_Task *self)
{
    if (self == NULL)
    {
        return Bff_Task_Id_INVALID;
    }

    /* Get the task ID. */
    return ((BffIm_Task *)self)->id;
}


static const char *
method_task_getName(Bff_Task *self)
{
    if (self == NULL)
    {
        return NULL;
    }

    /* Get the task name. */
    return ((BffIm_Task *)self)->name;
}


static Bff_Scene *
method_task_getScheduledScene(Bff_Task *self)
{
    if (self == NULL)
    {
        return NULL;
    }

    /* Get the scheduled scene. */
    return (Bff_Scene *)((BffIm_Task *)self)->scheduledScene;
}


static boolean
method_task_isFirst(Bff_Task *self)
{
    if (self == NULL)
    {
        return FALSE;
    }

    return ((BffIm_Task *)self)->id == Bff_Task_Id_FIRST ? TRUE : FALSE;
}


static boolean
method_task_isSuspended(Bff_Task *self)
{
    if (self == NULL)
    {
        return FALSE;
    }

    return ((BffIm_Task *)self)->flags.suspended ? TRUE : FALSE;
}


static int
method_task_scheduleNewScene(
    Bff_Task *self, Bff_Scene **scene, Bff_Scene_Driver *driver, void *arg)
{
    BffIm_Scene *newScene = NULL;
    int ret;


    if (self == NULL || driver == NULL)
    {
        return EBADPARM;
    }

    ret = scheduleNewScene((BffIm_Task *)self, &newScene, driver, arg);
    if (ret != SUCCESS)
    {
        return ret;
    }

    if (scene)
    {
        *scene = (Bff_Scene *)newScene;
    }

    return SUCCESS;
}


static int
method_task_scheduleOneShotScene(
    Bff_Task *self, Bff_Scene **scene, Bff_Scene_Driver *driver, void *arg)
{
    BffIm_Scene *newScene = NULL;
    int ret;


    if (self == NULL || driver == NULL)
    {
        return EBADPARM;
    }

    ret = scheduleNewScene((BffIm_Task *)self, &newScene, driver, arg);
    if (ret != SUCCESS)
    {
        return ret;
    }

    /* Mark the scene as "one shot". */
    newScene->flags.releaseOnLeave = 1;

    if (scene)
    {
        *scene = (Bff_Scene *)newScene;
    }

    return SUCCESS;
}


static int
method_task_scheduleScene(Bff_Task *self, Bff_Scene *scene, void *arg)
{
    if (self == NULL)
    {
        return EBADPARM;
    }

    return checkAndScheduleScene(self, scene, arg);
}


static int
method_task_scheduleSceneById(
    Bff_Task *self, Bff_Scene_Id sceneId, void *arg)
{
    Bff_Scene *scene;
    Bff *bff = BFF();


    if (self == NULL)
    {
        return EBADPARM;
    }

    scene = bff->getSceneById(bff, sceneId);
    if (scene == NULL)
    {
        return EBADITEM;
    }

    return checkAndScheduleScene(self, scene, arg);
}


static int
method_task_scheduleSceneByName(Bff_Task *self, const char *name, void *arg)
{
    Bff_Scene *scene;
    Bff *bff = BFF();


    if (self == NULL || name == NULL)
    {
        return EBADPARM;
    }

    scene = bff->getSceneByName(bff, name);
    if (scene == NULL)
    {
        return EBADITEM;
    }

    return checkAndScheduleScene(self, scene, arg);
}


static void
method_task_setData(Bff_Task *self, void *data)
{
    if (self == NULL)
    {
        return;
    }

    /* Set task-specific data. */
    ((BffIm_Task *)self)->data = data;
}


/*---------------------------------------------*
 * BffIm_Task methods
 *---------------------------------------------*/
static void
method_taskim_finalize(BffIm_Task *self)
{
    if (self == NULL)
    {
        return;
    }

    /* Do not allow any scene to be scheduled in driver implementations. */
    self->flags.schedulingBlocked = 1;

    /* Kick out a scheduled scene and/or a current scene if any. */
    if (self->flags.hasScheduledScene || self->currentScene)
    {
        /* Kick out the scheduled scene. */
        scheduleScene(self, NULL, NULL);

        if (self->currentScene)
        {
            /* Kick out the current scene. */
            switchScenes(self);
        }
    }

    /* If task finalizer needs to be called and one is available. */
    if (self->flags.needsFinalizerCall && self->driver.finalize)
    {
        /* We are about to call finalize(). */
        BFF_DEBUG(("BFF:Call finalize() of Bff_Task_Driver"));

        /* Invoke the finalizer. */
        self->driver.finalize(BFF(), (Bff_Task *)self);
    }
}


static boolean
method_taskim_handleEvent(
    BffIm_Task *self, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed)
{
    Bff *bff = BFF();
    Bff_Dispatcher *dsp;
    boolean ret = FALSE;
    boolean ret2;


    if (!self || !ev || !w || !dw || !consumed)
    {
        if (consumed)
        {
            *consumed = FALSE;
        }

        return FALSE;
    }

    /* If the pre-dispatcher has been set up, */
    if (self->flags.hasPreDispatcher)
    {
        /* Let the pre-dispatcher of handle the event. */
        *consumed = TRUE;
        dsp = &self->preDispatcher;
        ret = dsp->dispatch(dsp, bff, ev, w, dw, consumed);
        if (*consumed)
        {
            /*
             * The pre-dispatcher has consumed the event.
             * Skip executing the current scene's event handlers.
             */
            goto post_process;
        }
    }

    /* Let the current scene handle the event.*/
    ret = handleEventByScene(self, bff, ev, w, dw, consumed);

post_process:
    /* If the post-dispatcher has been set up, */
    if (self->flags.hasPostDispatcher)
    {
        /* Let the post-dispatcher handle the event. */
        *consumed = TRUE;
        dsp  = &self->postDispatcher;
        ret2 = dsp->dispatch(dsp, bff, ev, w, dw, consumed);
        if (*consumed)
        {
            /*
             * Only when the post-dispatcher has consumed
             * the event, 'ret' is overwritten.
             */
            ret = ret2;
        }
    }

    return ret;
}


static int
method_taskim_switchScenes(BffIm_Task *self)
{
    if (self == NULL)
    {
        return EBADPARM;
    }

    if (self->flags.switchingBlocked)
    {
        BFF_WARN(("BFF:Scene switching is being blocked."));
        return EBADSTATE;
    }

    return switchScenes(self);
}


/*---------------------------------------------*
 * Event dispatching
 *---------------------------------------------*/
static boolean
handleEventByScene(
    BffIm_Task *task, Bff *bff, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed)
{
    BffIm_Scene *scene = task->currentScene;


    if (scene == NULL)
    {
        /* No scene is running at this moment. */
        return (*consumed = FALSE);
    }

    return scene->handleEvent(scene, ev, w, dw, consumed);
}


/*---------------------------------------------*
 * Scene management
 *---------------------------------------------*/
static int
checkAndScheduleScene(Bff_Task *task, Bff_Scene *scene, void *arg)
{
    BffIm_Task *t = (BffIm_Task *)task;
    BffIm_Scene *s = (BffIm_Scene *)scene;
    int ret = SUCCESS;


    if (t->flags.schedulingBlocked)
    {
        BFF_WARN(("BFF:Scene scheduling is blocked."));
        return EBADSTATE;
    }

    if (s && s->flags.scheduled)
    {
        if (s->task == t)
        {
            /* The scene is recognized as a scheduled scene by this task. */
            BFF_WARN(("BFF:The scene has already been scheduled."));
            ret = EALREADY;
        }
        else
        {
            /* The scene is owned by other task as a scheduled scene. */
            BFF_WARN(("BFF:Other task owns the scene as a scheduled one."));
            ret = EITEMBUSY;
        }
    }
    else if (s && s->flags.current)
    {
        if (s->task == t)
        {
            /* The scene is the current scene of this task. */
            BFF_WARN(("BFF:The scene is the current scene of this task."));
            ret = EALREADY;
        }
        else
        {
            /* The scene is owned by other task as a current scene. */
            BFF_WARN(("BFF:Other task owns the scene as a current one."));
            ret = EITEMBUSY;
        }
    }

    if (ret != SUCCESS)
    {
        BFF_WARN(("BFF:  scene ID = %ld",  getSceneIdForLogging(s)));
        BFF_WARN(("BFF:  scene name = %s", getSceneNameForLogging(s)));
        return ret;
    }

    return scheduleScene(t, s, arg);
}


static int
scheduleNewScene(
    BffIm_Task *task, BffIm_Scene **scene, Bff_Scene_Driver *driver,
    void *arg)
{
    Bff *bff= BFF();
    int ret;


    if (task->flags.schedulingBlocked)
    {
        return EBADSTATE;
    }

    /* Create a new scene. */
    ret = bff->createScene(bff, (Bff_Scene **)scene, driver);
    if (ret != SUCCESS)
    {
        /* Failed to create a new scene. */
        return ret;
    }

    /* Schedule the new scene. */
    ret = scheduleScene(task, *scene, arg);
    if (ret != SUCCESS)
    {
        /* Failed to scheduled the scene. */
        bff->releaseScene(bff, (Bff_Scene *)*scene);

        return ret;
    }

    /* Scheduled a scene. */
    return SUCCESS;
}


static int
scheduleScene(BffIm_Task *task, BffIm_Scene *scene, void *arg)
{
    /* If there is already a scheduled scene, cancel it. */
    cancelScheduledScene(task);

    /* Accept the scene. */
    task->scheduledScene               = scene;
    task->scheduledSceneArg            = arg;
    task->scheduledSceneEventNestLevel = task->eventNestLevel;

    if (scene)
    {
        /* Mark the scene as 'scheduled'. */
        scene->flags.scheduled = 1;

        /* Set the associated task. */
        scene->task = task;
    }

    task->flags.hasScheduledScene = 1;

    BFF_INFO(("BFF:SCENE SCHEDULING"));
    if (scene)
    {
        BFF_INFO(("BFF:  scene ID = %ld",  getSceneIdForLogging(scene)));
        BFF_INFO(("BFF:  scene name = %s", getSceneNameForLogging(scene)));
    }
    else
    {
        BFF_INFO(("BFF:  scene = (null)"));
    }

    return SUCCESS;
}


static void
cancelScheduledScene(BffIm_Task *task)
{
    Bff *bff = BFF();
    BffIm_Scene *scene = task->scheduledScene;
    void *arg = task->scheduledSceneArg;


    /*
     * Make sure that the internal state remains consistent
     * before calling a scene driver method ("cancel()" here).
     */
    task->scheduledScene               = NULL;
    task->scheduledSceneArg            = NULL;
    task->scheduledSceneEventNestLevel = task->eventNestLevel;

    if (scene == NULL)
    {
        /* There is no scheduled scene to cancel. */
        return;
    }

    /* The scene is no longer a scheduled one. */
    scene->flags.scheduled = 0;

    /* The scene is no longer associated with this task. */
    scene->task = NULL;

    /* We are about to cancel the scene. */
    BFF_WARN(("BFF:A scheduled scene is cancelled."));
    BFF_WARN(("BFF:  scene ID = %ld",  getSceneIdForLogging(scene)));
    BFF_WARN(("BFF:  scene name = %s", getSceneNameForLogging(scene)));

    if (scene->driver.cancel)
    {
        /*
         * Call "cancel" if defined.
         *
         * If the memory area which is pointed to by 'arg' was allocated
         * dynamically when the scene was scheduled, it should be freed
         * in the implementation of "cancel" unless it is freed somewhere
         * else.
         */
        scene->driver.cancel(
            bff, (Bff_Scene *)scene, arg, (Bff_Task *)task);
    }

    if (scene->flags.releaseOnLeave)
    {
        bff->releaseScene(bff, (Bff_Scene *)scene);
    }
}


static int
switchScenes(BffIm_Task *task)
{
    Bff *bff = BFF();
    BffIm_Scene *curr = task->currentScene;
    BffIm_Scene *next = task->scheduledScene;
    void *arg = task->scheduledSceneArg;


    /* Check if there is a scheduled scene. */
    if (!task->flags.hasScheduledScene)
    {
        /*
         * There is no scene to replace the current scene.
         * This function might have been called accidentally.
         */
        return SUCCESS;
    }

    /* We are about to switch scenes. */
    logAboutSceneSwitching(task, curr, next);

    /* Leave the current scene. */
    leaveCurrentScene(bff, task, curr, next);

    /*
     * Update the status of this scene to "no scheduled scene"
     * before invoking Bff_Scene_Driver.enter().
     */
    task->flags.hasScheduledScene      = 0;
    task->scheduledScene               = NULL;
    task->scheduledSceneArg            = NULL;
    task->scheduledSceneEventNestLevel = 0;

    /*
     * Update the current scene before invoking
     * Bff_Scene_Driver.enter().
     */
    task->currentScene = next;

    /* Enter the next scene. */
    enterScheduledScene(bff, task, curr, next, arg);

    /* If the previous scene is a one-shot scene. */
    if (curr && curr->flags.releaseOnLeave)
    {
        /* Release the previous scene. */
        bff->releaseScene(bff, (Bff_Scene *)curr);
    }

    return SUCCESS;
}


static void
logAboutSceneSwitching(
    BffIm_Task *task, BffIm_Scene *curr, BffIm_Scene *next)
{
    BFF_INFO(("BFF:SCENE SWITCHING"));

    BFF_INFO(("BFF:  Task = [%ld] %s", (long)task->id, task->name));

    if (curr)
    {
        BFF_INFO(("BFF:  Curr = [%ld] %s", getSceneIdForLogging(curr),
                                           getSceneNameForLogging(curr)));
    }
    else
    {
        BFF_INFO(("BFF:  Curr = (null)"));
    }

    if (next)
    {
        BFF_INFO(("BFF:  Next = [%ld] %s", getSceneIdForLogging(next),
                                           getSceneNameForLogging(next)));
    }
    else
    {
        BFF_INFO(("BFF:  Next = (null)"));
    }
}


static void
leaveCurrentScene(
    Bff *bff, BffIm_Task *task, BffIm_Scene *curr, BffIm_Scene *next)
{
    if (curr == NULL)
    {
        return;
    }

    if (curr->driver.leave)
    {
        boolean blocked = task->flags.schedulingBlocked ? TRUE : FALSE;

        /* We are about to call Bff_Scene_Driver.leave() */
        BFF_DEBUG(("BFF:Call leave() of Bff_Scene_Driver"));

        /* Do not allow Bff_Scene_Driver.leave() to schedule a scene. */
        task->flags.schedulingBlocked = 1;

        /* Call the function to be performed when leaving the scene. */
        curr->driver.leave(bff, (Bff_Scene *)curr, (Bff_Scene *)next);

        /* Restore the previous state. */
        task->flags.schedulingBlocked = blocked ? 1 : 0;
    }

    /* The scene is no longer a current one. */
    curr->flags.current = 0;

    /* The scene is no longer associated with this task. */
    curr->task = NULL;
}


static void
enterScheduledScene(
    Bff *bff, BffIm_Task *task, BffIm_Scene *curr, BffIm_Scene *next,
    void *arg)
{
    if (next == NULL)
    {
        return;
    }

    next->flags.scheduled = 0;
    next->flags.current   = 1;

    if (next->driver.enter)
    {
        /* We are about to call Bff_Scene_Driver.enter() */
        BFF_DEBUG(("BFF:Call enter() of Bff_Scene_Driver"));

        next->driver.enter(bff, (Bff_Scene *)next, (Bff_Scene *)curr, arg);
    }
}


static long
getSceneIdForLogging(BffIm_Scene *scene)
{
    return (long)(scene ? scene->id : Bff_Scene_Id_INVALID);
}


static const char *
getSceneNameForLogging(BffIm_Scene *scene)
{
    return (const char *)(scene ? scene->name : "(null)");
}


/*------------------------------------------------------------------------*
 * FUNCTIONS
 *------------------------------------------------------------------------*/
int
BffIm_Task_initialize(
    BffIm_Task *instance, BffIm_Kernel *kernel, Bff_Task_Id taskId,
    Bff_Task_Driver *driver, uint16 flags, AEEAppStart *appStart)
{
    int ret;


    /* Check if the given arguments are valid. */
    if (instance == NULL || kernel == NULL || driver == NULL)
    {
        /* The arguments are invalid. */
        return EBADPARM;
    }

    /* Set up task. */
    ret = setupTask(instance, kernel, taskId, driver, flags, appStart);
    if (ret != SUCCESS)
    {
        /* Failed to set up a task. */
        return ret;
    }

    return SUCCESS;
}
