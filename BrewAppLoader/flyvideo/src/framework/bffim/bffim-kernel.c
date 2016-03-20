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
 * BREW Foundation Framework, Kernel.
 *
 * @author Takahiko Kawasaki
 * @file
 */


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEStdLib.h"
#include "AEEAppHist.h"
#include "bff/bff.h"
#include "bff/bff-accessors.h"
#include "bff/bff-dispatcher.h"
#include "bff/bff-handlers.h"
#include "bff/bff-logging.h"
#include "bffim/bffim-kernel.h"
#include "bffim/bffim-scene.h"
#include "bffim/bffim-task.h"


/*------------------------------------------------------------------------*
 * STATIC FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/

/*---------------------------------------------*
 * Initializers in the first phase
 *---------------------------------------------*/
/**
 * Initialize BFF Kernel (1st phase).
 *
 * Initialization of BFF Kernel is divided into three phases. This function
 * is in charge of the first one and supposed to be called in the context
 * of AEEClsCreateInstance() or equivalent.
 *
 * @param[in] instance  A BFF Kernel instance to initialize.
 * @param[in] classId   An application class ID.
 * @param[in] size      Size of memory area that has been allocated for
 *                      the applet.
 * @param[in] driver    BFF Applet driver that determine behaviors of BFF.
 *
 * @retval SUCCESS  Successfully initialized.
 *                  (The current implementation always returns SUCCESS.)
 *
 * @see setupKernelOnFirstEvent()
 * @see setupKernelOnAppStart()
 */
static int
setupKernelOnCreation(
    BffIm_Kernel *instance, AEECLSID classId, uint16 size,
    Bff_Applet_Driver *driver);


/**
 * Set up BFF methods.
 *
 * Assign function pointers to each BFF method.
 *
 * @param[in] instance  A BFF Kernel to set up.
 */
static void
setupBffMethods(BffIm_Kernel *instance);


/**
 * Set up BFF Kernel methods.
 *
 * Assign function pointers to each BFF Kernel method.
 *
 * @param[in] instance  A BFF Kernel to set up.
 */
static void
setupKernelMethods(BffIm_Kernel *instance);


/**
 * Set up pre event-dispatcher of BFF Kernel.
 *
 * Set up event handlers that catch BREW events and handle them
 * before the events are passed to application-defined event
 * handlers.
 *
 * @param[in] instance  A BFF Kernel to set up.
 */
static void
setupKernelPreDispatcher(BffIm_Kernel *instance);


/**
 * Set up post event-dispatcher of BFF Kernel.
 *
 * Set up event handlers that are performed before BFF Kernel
 * returns a result to BREW AEE.
 *
 * @param[in] instance  A BFF Kernel to set up.
 */
static void
setupKernelPostDispatcher(BffIm_Kernel *instance);


/*---------------------------------------------*
 * Initializers in the second phase
 *---------------------------------------------*/
/**
 * Initialize BFF Kernel (2nd phase).
 *
 * Initialization of BFF Kernel is divided into three phases. This function
 * is in charge of the second one and supposed to be called when the very
 * first event is delivered from BREW AEE.
 *
 * Some driver functions that have been given to Bff_Applet_create() are
 * called from within this function.
 *
 * @param[in] kernel  A BFF Kernel instance.
 *
 * @retval SUCCESS  Successfully initialized.
 * @retval Others   Initialization failed.
 *
 * @see setupKernelOnCreation()
 * @see setupKernelOnAppStart()
 */
static int
setupKernelOnFirstEvent(BffIm_Kernel *kernel);


/**
 * Set up memory allocation and deallocation functions.
 *
 * BFF provides a mechanism to allow BFF applications to define their
 * own memory allocation and deallocation functions.
 *
 * If Bff_Applet_Driver.malloc() and Bff_Applet_Driver.free() are both
 * specified, they are used as memory allocation and deallocation
 * functions, respectively. Otherwise, the default implementations are
 * used.
 *
 * @param[in] kernel  A BFF Kernel to set up.
 */
static void
setupMemoryDriver(BffIm_Kernel *kernel);


/**
 * Perform applet-specific initialization.
 *
 * Some applet driver are called in the order shown below, if they are
 * defined.
 *
 *   -# Bff_Applet_Driver.getPreHandlers()
 *   -# Bff_Applet_Driver.getPostHandlers()
 *   -# Bff_Applet_Driver.initialize()
 *
 * BFF applications can specify their own event handlers by properly
 * implementing Bff_Applet_Driver.getPreHandlers() and
 * Bff_Applet_Driver.getPostHandlers().
 *
 * @param[in] kernel  A BFF Kernel to set up.
 *
 * @retval SUCCESS  A BFF Kernel to set up.
 * @retval Others   An applet driver has failed.
 */
static int
setupApplet(BffIm_Kernel *kernel);


/**
 * Set up applet-wide pre event-dispatcher.
 *
 * Call Bff_Applet_Driver.getPreHandlers() to get application-
 * defined event handlers and initialize applet-wide pre event-dispatcher
 * with the event handlers.
 *
 * Nothing is done if Bff_Applet_Driver.getPreHandlers() is not specified.
 *
 * @param[in] kernel   A BFF Kernel to set up.
 * @param[in] driver  Applet driver to use.
 *
 * @retval SUCCESS  Successfully initialized.
 * @retval Others   Bff_Applet_Driver.getPreHandlers() has failed.
 */
static int
setupAppletPreDispatcher(BffIm_Kernel *kernel, Bff_Applet_Driver *driver);


/**
 * Set up applet-wide post event-dispatcher.
 *
 * Call Bff_Applet_Driver.getPostHandlers() to get application-
 * defined event handlers and initialize applet-wide post event-dispatcher
 * with the event handlers.
 *
 * Nothing is done if Bff_Applet_Driver.getPostHandlers() is not specified.
 *
 * @param[in] kernel   A BFF Kernel to set up.
 * @param[in] driver  Applet driver to use.
 *
 * @retval SUCCESS  Successfully initialized.
 * @retval Others   Bff_Applet_Driver.getPostHandlers() has failed.
 */
static int
setupAppletPostDispatcher(BffIm_Kernel *kernel, Bff_Applet_Driver *driver);


/**
 * Execute application-defined applet initialization.
 *
 * Call Bff_Applet_Driver.initialize() where a BFF application
 * can execute its own applet-wide initialization.
 *
 * Nothing is done if Bff_Applet_Driver.initialize() is not specified.
 *
 * @param[in] kernel   A BFF Kernel to set up.
 * @param[in] driver  Applet driver to use.
 *
 * @retval SUCCESS  Successfully initialized.
 * @retval Others   Bff_Applet_Driver.initialize() has failed.
 */
static int
setupAppletInitialize(BffIm_Kernel *kernel, Bff_Applet_Driver *driver);


/**
 * Perform initialization related to task management.
 *
 * @param[in] kernel  A BFF Kernel to set up.
 *
 * @retval SUCCESS  Successfully initialized.
 */
static int
setupTaskManagement(BffIm_Kernel *kernel);


/**
 * Query whether multi-tasking support is requested.
 *
 * Call Bff_Applet_Driver.isMultiTaskingEnabled() to query whether
 * multi-tasking support is requested. If the driver function is
 * specified and it returns TRUE, multi-tasking support is enabled.
 * Otherwise, multi-tasking support is not enabled.
 *
 * @param[in] kernel  A BFF Kernel to set up.
 */
static void
setupMultiTasking(BffIm_Kernel *kernel);


/*---------------------------------------------*
 * Initializers in the third phase
 *---------------------------------------------*/
/**
 * Initialize BFF Kernel (3rd phase).
 *
 * Initialization of BFF Kernel is divided into three phases.
 * This function is in charge of the third one and supposed
 * to be called when EVT_APP_START event arrives.
 *
 * @param[in] kernel  A BFF Kernel instance.
 *
 * @retval SUCCESS  Successfully initialized.
 * @retval Others   Initialization failed.
 *
 * @see setupKernelOnCreation()
 * @see setupKernelOnFirstEvent()
 */
static int
setupKernelOnAppStart(BffIm_Kernel *kernel);


/*---------------------------------------------*
 * Bff methods
 *---------------------------------------------*/
/**
 * Implementation of Bff.createScene().
 *
 * @see Bff.createScene()
 */
static int
method_bff_createScene(
    Bff *self, Bff_Scene **scene, Bff_Scene_Driver *driver);


/**
 * Implementation of Bff.free().
 *
 * @see Bff.free()
 */
static void
method_bff_free(Bff *self, void *area);


/**
 * Implementation of Bff.getCurrentScene().
 *
 * @see Bff.getCurrentScene()
 */
static Bff_Scene *
method_bff_getCurrentScene(Bff *self);


/**
 * Implementation of Bff.getCurrentTask().
 *
 * @see Bff.getCurrentTask()
 */
static Bff_Task *
method_bff_getCurrentTask(Bff *self);


/**
 * Implementation of Bff.getData().
 *
 * @see Bff.getData()
 */
static void *
method_bff_getData(Bff *self);


/**
 * Implementation of Bff.getLoggingLevel().
 *
 * @see Bff.getLoggingLevel()
 */
static Bff_Logging_Level
method_bff_getLoggingLevel(Bff *self);


/**
 * Implementation of Bff.getSceneById().
 *
 * @see Bff.getSceneById()
 */
static Bff_Scene *
method_bff_getSceneById(Bff *self, Bff_Scene_Id sceneId);


/**
 * Implementation of Bff.getSceneByName().
 *
 * @see Bff.getSceneByName()
 */
static Bff_Scene *
method_bff_getSceneByName(Bff *self, const char *name);


/**
 * Implementation of Bff.getTaskById().
 *
 * @see Bff.getTaskById()
 */
static Bff_Task *
method_bff_getTaskById(Bff *self, Bff_Task_Id taskId);


/**
 * Implementation of Bff.getTaskByName().
 *
 * @see Bff.getTaskByName()
 */
static Bff_Task *
method_bff_getTaskByName(Bff *self, const char *name);


/**
 * Implementation of Bff.malloc().
 *
 * @see Bff.malloc()
 */
static void *
method_bff_malloc(Bff *self, size_t size);


/**
 * Implementation of Bff.releaseScene().
 *
 * @see Bff.releaseScene()
 */
static int
method_bff_releaseScene(Bff *self, Bff_Scene *scene);


/**
 * Implementation of Bff.releaseSceneById().
 *
 * @see Bff.releaseSceneById()
 */
static int
method_bff_releaseSceneById(Bff *self, Bff_Scene_Id sceneId);


/**
 * Implementation of Bff.releaseSceneByName().
 *
 * @see Bff.releaseSceneByName()
 */
static int
method_bff_releaseSceneByName(Bff *self, const char *name);


/**
 * Implementation of Bff.setData().
 *
 * @see Bff.setData()
 */
static void
method_bff_setData(Bff *self, void *data);


/**
 * Implementation of Bff.setLoggingLevel().
 *
 * @see Bff.setLoggingLevel()
 */
static int
method_bff_setLoggingLevel(Bff *self, Bff_Logging_Level level);


/*---------------------------------------------*
 * BffIm_Kernel methods
 *---------------------------------------------*/
/**
 * Implementation of BffIm_Kernel.finalize().
 *
 * Some application driver related to finalization are called.
 *
 * @see BffIm_Kernel.finalize()
 */
static void
method_kernel_finalize(BffIm_Kernel *self);


/**
 * Implementation of BffIm_Kernel.handleEvent().
 *
 * @see BffIm_Kernel.handleEvent()
 */
static boolean
method_kernel_handleEvent(
    BffIm_Kernel *self, AEEEvent ev, uint16 w, uint32 dw);


/*---------------------------------------------*
 * Finalizers
 *---------------------------------------------*/
/**
 * Finalize applet.
 *
 * Call Bff_Applet_Driver.finalize() if it is defined. The driver
 * function is not called if setupApplet() has failed.
 *
 * @param[in] kernel  A BFF Kernel to finalize.
 *
 * @see Bff_Applet_Driver.finalize()
 * @see setupApplet()
 */
static void
finalizeApplet(BffIm_Kernel *kernel);


/**
 * Destroy all remaining tasks.
 *
 * BffIm_Task.finalize() is called on each remaining task and
 * the memory area of each task is released.
 *
 * @param[in] kernel  A BFF Kernel to finalize.
 */
static void
finalizeTaskManagement(BffIm_Kernel *kernel);


/**
 * Destroy all remaining scenes.
 *
 * BffIm_Scene.finalize() is called on each remaining scene and
 * the memory area of each scene is released.
 *
 * @param[in] kernel  A BFF Kernel to finalize.
 */
static void
finalizeSceneManagement(BffIm_Kernel *kernel);


/*---------------------------------------------*
 * Event dispatching
 *---------------------------------------------*/
static boolean
handleEventByKernel(BffIm_Kernel *kernel, AEEEvent ev, uint16 w, uint32 dw);


static void
pushKernelReentrantData(
    BffIm_Kernel *kernel, BffIm_Kernel_ReentrantData *rd);


static void
popKernelReentrantData(BffIm_Kernel *kernel);


static boolean
handleEventByApplet(
    BffIm_Kernel *kernel, AEEEvent ev, uint16 w, uint32 dw,
    boolean *consumed);


static boolean
handleEventByTask(
    BffIm_Kernel *kernel, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed);


/*---------------------------------------------*
 * Kernel event handlers
 *---------------------------------------------*/
static boolean 
kernel_pre_hookFirstEvent(
    Bff *bff, AEEEvent *ev, uint16 *w, uint32 *dw, boolean *consumed);


static boolean
kernel_pre_handlerOnFailure(
    Bff *bff, AEEEvent ev, uint16 w, uint32 dw, boolean *consumed);


static boolean
kernel_pre_preDispatch(
    Bff *bff, AEEEvent ev, uint16 w, uint32 dw, boolean *consumed);


static boolean
kernel_pre_onAppStart(
    Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed);


static boolean
kernel_pre_onAppResume(
    Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed);


static boolean
kernel_post_postDispatch(
    Bff *bff, AEEEvent ev, uint16 w, uint32 dw, boolean *consumed);


static boolean
kernel_post_onAppSuspend(
    Bff *bff, AEESuspendReason reason, AEESuspendInfo *info,
    boolean *consumed);


static boolean
kernel_post_onAppStop(Bff *bff, boolean *close, boolean *consumed);


/*---------------------------------------------*
 * Task management
 *---------------------------------------------*/
static int
createCurrentTask(
    BffIm_Kernel *kernel, uint16 flags, AEEAppStart *appStart);


static int
createTask(
    BffIm_Kernel *kernel, BffIm_Task **task, uint16 flags,
    AEEAppStart *appStart);


static void
destroyTask(BffIm_Kernel *kernel, BffIm_Task *task);


static void
onCurrentTaskUpdateFailure(BffIm_Kernel *kernel);


static int
stopCurrentAppHistoryInstance(void);


static int
getTaskDriver(
    BffIm_Kernel *kernel, Bff_Task_Id taskId, Bff_Task_Driver *driver);


static void
clearTaskDriver(Bff_Task_Driver *driver);


static int
manageTask(BffIm_Kernel *kernel, BffIm_Task *task);


static void
unmanageTask(BffIm_Kernel *kernel, BffIm_Task *task);


static void
addTaskToList(BffIm_Kernel *kernel, BffIm_Task *task);


static void
removeTaskFromList(BffIm_Kernel *kernel, BffIm_Task *task);


static void
cleanupTaskList(BffIm_Kernel *kernel);


static int
getAppHistory(IAppHistory **appHistory);


static int
getTaskInHistoryData(BffIm_Task **task);


static int
setTaskInHistoryData(BffIm_Task *task);


static int
getHistoryData(const char *name, void *buf, int *size);


static int
setHistoryData(const char *name, void *buf, int size);


static BffIm_Task *
getTaskById(BffIm_Kernel *kernel, Bff_Task_Id taskId);


static BffIm_Task *
getTaskByName(BffIm_Kernel *kernel, const char *name);


/*---------------------------------------------*
 * Scene management
 *---------------------------------------------*/
static BffIm_Scene *
getCurrentScene(BffIm_Kernel *kernel);


static int
createScene(
    BffIm_Kernel *kernel, BffIm_Scene **scene, Bff_Scene_Driver *driver);


static int
releaseScene(BffIm_Kernel *kernel, BffIm_Scene *scene);


static void
destroyScene(BffIm_Kernel *kernel, BffIm_Scene *scene);


static int
manageScene(BffIm_Kernel *kernel, BffIm_Scene *scene);


static void
unmanageScene(BffIm_Kernel *kernel, BffIm_Scene *scene);


static void
addSceneToList(BffIm_Kernel *kernel, BffIm_Scene *scene);


static void
removeSceneFromList(BffIm_Kernel *kernel, BffIm_Scene *scene);


static void
cleanupSceneList(BffIm_Kernel *kernel);


static BffIm_Scene *
getSceneById(BffIm_Kernel *kernel, Bff_Scene_Id sceneId);


static BffIm_Scene *
getSceneByName(BffIm_Kernel *kernel, const char *name);


/*---------------------------------------------*
 * Memory management
 *---------------------------------------------*/
static void *
kernel_malloc(BffIm_Kernel *kernel, size_t size);


static void
kernel_free(BffIm_Kernel *kernel, void *area);


static void *
default_driver_malloc(Bff *self, size_t size);


static void
default_driver_free(Bff *self, void *area);


/*------------------------------------------------------------------------*
 * STATIC FUNCTIONS
 *------------------------------------------------------------------------*/

/*---------------------------------------------*
 * Initializers in the first phase
 *---------------------------------------------*/
static int
setupKernelOnCreation(
    BffIm_Kernel *instance, AEECLSID classId, uint16 size,
    Bff_Applet_Driver *driver)
{
    /* Set up methods of Bff structure. */
    setupBffMethods(instance);

    /* Set up methods of BffIm_Kernel structure. */
    setupKernelMethods(instance);

    /*--------------------*
     * Driver
     *--------------------*/

    /* Copy BFF Applet driver. */
    instance->appletDriver = *driver;

    /*--------------------*
     * Kernel
     *--------------------*/

    /* Set up event handlers of kernel pre-processing. */
    setupKernelPreDispatcher(instance);

    /* Set up event handlers of kernel post-processing. */
    setupKernelPostDispatcher(instance);

    /* Clear all flags. */
    MEMSET(&instance->flags, 0, sizeof(instance->flags));

    /* No reentrant is set yet. */
    instance->reentrantData = NULL;

    /* Set the default logging level. */
    instance->loggingLevel = Bff_Logging_Level_DEFAULT;

    /*--------------------*
     * Applet
     *--------------------*/

    /* 'appletPreDispatcher' is initialized in the second phase. */

    /* 'appletPostDispatcher' is initialized in the second phase. */

    /* Set the default value of applet-specific data. */
    instance->appletData = NULL;

    /*--------------------*
     * Task
     *--------------------*/

    /* No task is under management. */
    instance->taskList = NULL;

    /* No task has been started so far. */
    instance->lastTaskId = Bff_Task_Id_INVALID;

    /* No task is running at this moment. */
    instance->currentTask = NULL;

    /*--------------------*
     * Scene
     *--------------------*/

    /* No scene is under management. */
    instance->sceneList = NULL;

    /* No scene has been started so far. */
    instance->lastSceneId = Bff_Scene_Id_INVALID;

    return SUCCESS;
}


static void
setupBffMethods(BffIm_Kernel *instance)
{
    Bff *bff = (Bff *)instance;

    /* Set up methods in Bff structure. */
    bff->createScene        = method_bff_createScene;
    bff->free               = method_bff_free;
    bff->getCurrentScene    = method_bff_getCurrentScene;
    bff->getCurrentTask     = method_bff_getCurrentTask;
    bff->getData            = method_bff_getData;
    bff->getLoggingLevel    = method_bff_getLoggingLevel;
    bff->getSceneById       = method_bff_getSceneById;
    bff->getSceneByName     = method_bff_getSceneByName;
    bff->getTaskById        = method_bff_getTaskById;
    bff->getTaskByName      = method_bff_getTaskByName;
    bff->malloc             = method_bff_malloc;
    bff->releaseScene       = method_bff_releaseScene;
    bff->releaseSceneById   = method_bff_releaseSceneById;
    bff->releaseSceneByName = method_bff_releaseSceneByName;
    bff->setData            = method_bff_setData;
    bff->setLoggingLevel    = method_bff_setLoggingLevel;
}


static void
setupKernelMethods(BffIm_Kernel *instance)
{
    /* Set up methods in BffIm_Kernel structure. */
    instance->finalize    = method_kernel_finalize;
    instance->handleEvent = method_kernel_handleEvent;
}


static void
setupKernelPreDispatcher(BffIm_Kernel *instance)
{
    Bff_Handlers handlers;


    Bff_Handlers_clear(&handlers);

    /* List of event handlers of kernel pre-processing. */
    handlers.preFixup    = kernel_pre_hookFirstEvent;
    handlers.preDispatch = kernel_pre_preDispatch;
    handlers.onAppStart  = kernel_pre_onAppStart;
    handlers.onAppResume = kernel_pre_onAppResume;

    /*
     * Initialize the kernel pre event dispatcher with the event handlers.
     */
    Bff_Dispatcher_initialize(&instance->kernelPreDispatcher, &handlers);
}


static void
setupKernelPostDispatcher(BffIm_Kernel *instance)
{
    Bff_Handlers handlers;


    Bff_Handlers_clear(&handlers);

    /* List of event handlers of kernel post-processing. */
    handlers.postDispatch = kernel_post_postDispatch;
    handlers.onAppSuspend = kernel_post_onAppSuspend;
    handlers.onAppStop    = kernel_post_onAppStop;

    /*
     * Initialize the kernel post event dispatcher with the event handlers.
     */
    Bff_Dispatcher_initialize(&instance->kernelPostDispatcher, &handlers);
}


/*---------------------------------------------*
 * Initializers in the second phase
 *---------------------------------------------*/
static int
setupKernelOnFirstEvent(BffIm_Kernel *kernel)
{
    int ret;


    /* Set up memory driver. */
    setupMemoryDriver(kernel);

    /* Set up the applet by applet driver. */
    ret = setupApplet(kernel);
    if (ret != SUCCESS)
    {
        /* Failed to set up applet. */
        return ret;
    }

    /* Set up task management. */
    ret = setupTaskManagement(kernel);
    if (ret != SUCCESS)
    {
        /* Failed to set up task management. */
        return ret;
    }

    return SUCCESS;
}


static void
setupMemoryDriver(BffIm_Kernel *kernel)
{
    Bff_Applet_Driver *driver = &kernel->appletDriver;


    /*
     * Bff_Applet_Driver.malloc() and Bff_Applet_Driver.free() must be paired.
     */
    if (driver->malloc == NULL || driver->free == NULL)
    {
        /* Overwrite malloc() and free(). */
        driver->malloc = default_driver_malloc;
        driver->free   = default_driver_free;
    }
}


static int
setupApplet(BffIm_Kernel *kernel)
{
    Bff_Applet_Driver *driver = &kernel->appletDriver;
    int ret;


    /* Call Bff_Applet_Driver.getPreHandlers() if it is defined. */
    ret = setupAppletPreDispatcher(kernel, driver);
    if (ret != SUCCESS)
    {
        /* Failed to set up pre-dispatcher. */
        return ret;
    }

    /* Call Bff_Applet_Driver.getPostHandlers() if it is defined. */
    ret = setupAppletPostDispatcher(kernel, driver);
    if (ret != SUCCESS)
    {
        /* Failed to set up post-dispatcher. */
        return ret;
    }

    /* Call Bff_Applet_Driver.initialize() if it is defined. */
    ret = setupAppletInitialize(kernel, driver);
    if (ret != SUCCESS)
    {
        /* Failed to initialize the applet. */
        return ret;
    }

    /* Initialization by applet driver has been done successfully. */

    /* If a function to set up applet pre event dispatcher is defined. */
    if (driver->getPreHandlers)
    {
        /*
         * Applet pre event dispatcher has been set up by the application.
         * The pre-dispatcher is used in the process of event dispatching.
         */
        kernel->flags.appletHasPreDispatcher = 1;
    }

    /* If a function to set up applet post event dispatcher is defined. */
    if (driver->getPostHandlers)
    {
        /*
         * Applet post event dispatcher has been set up by the application.
         * The post-dispatcher is used in the process of event dispatching.
         */
        kernel->flags.appletHasPostDispatcher = 1;
    }

    /*
     * Bff_Applet_Driver.finalize() needs to be called in the finalization
     * process.
     */
    kernel->flags.appletNeedsFinalizerCall = 1;

    return SUCCESS;
}


static int
setupAppletPreDispatcher(BffIm_Kernel *kernel, Bff_Applet_Driver *driver)
{
    Bff *bff = (Bff *)kernel;
    Bff_Handlers handlers;
    int ret;


    /*
     * Check if a function to set up applet pre event handlers is defined.
     */
    if (driver->getPreHandlers == NULL)
    {
        /*
         * A function to set up applet pre event handlers is not defined.
         */
        return SUCCESS;
    }

    Bff_Handlers_clear(&handlers);

    /* We are about to call Bff_Applet_Driver.getPreHandlers(). */
    BFF_DEBUG(("BFF:Call getPreHandlers() of Bff_Applet_Driver"));

    /*
     * Call Bff_Applet_Driver.getPreHandlers() to set up event handlers
     * of applet event pre-processing.
     */
    ret = driver->getPreHandlers(bff, &handlers);
    if (ret != SUCCESS)
    {
        /* Failed to set up the pre event handlers of the applet. */
        BFF_ERROR(("BFF:getPreHandlers() of Bff_Applet_Driver"));
        BFF_ERROR(("BFF:  ret = %d", ret));

        return ret;
    }

    /*
     * Initialize the applet pre event dispatcher with the event handlers.
     */
    Bff_Dispatcher_initialize(&kernel->appletPreDispatcher, &handlers);

    return SUCCESS;
}


static int
setupAppletPostDispatcher(BffIm_Kernel *kernel, Bff_Applet_Driver *driver)
{
    Bff *bff = (Bff *)kernel;
    Bff_Handlers handlers;
    int ret;


    /*
     * Check if a function to set up applet post event handlers is defined.
     */
    if (driver->getPostHandlers == NULL)
    {
        /*
         * A function to set up applet post event handlers is not defined.
         */
        return SUCCESS;
    }

    Bff_Handlers_clear(&handlers);

    /* We are about to call Bff_Applet_Driver.getPostHandlers(). */
    BFF_DEBUG(("BFF:Call getPostHandlers() of Bff_Applet_Driver"));

    /*
     * Call Bff_Applet_Driver.getPostHandlers() to set up event handlers
     * of applet post-processing.
     */
    ret = driver->getPostHandlers(bff, &handlers);
    if (ret != SUCCESS)
    {
        /* Failed to set up the post event handlers of the applet. */
        BFF_ERROR(("BFF:getPostHandlers() of Bff_Applet_Driver"));
        BFF_ERROR(("BFF:  ret = %d", ret));

        return ret;
    }

    /*
     * Initialize the applet post event dispatcher with the event handlers.
     */
    Bff_Dispatcher_initialize(&kernel->appletPostDispatcher, &handlers);

    return SUCCESS;
}


static int
setupAppletInitialize(BffIm_Kernel *kernel, Bff_Applet_Driver *driver)
{
    int ret;


    /* Check if a function to initialize an applet is defined. */
    if (driver->initialize == NULL)
    {
        /* A function to initialize an applet is not defined. */
        return SUCCESS;
    }

    /* We are about to call Bff_Applet_Driver.initialize(). */
    BFF_DEBUG(("BFF:Call initialize() of Bff_Applet_Driver"));

    /* Call the initializer for the applet. */
    ret = driver->initialize((Bff *)kernel);
    if (ret != SUCCESS)
    {
        /* Failed to initialize the applet. */
        BFF_ERROR(("BFF:initialize() of Bff_Applet_Driver"));
        BFF_ERROR(("BFF:  ret = %d", ret));

        return ret;
    }

    return SUCCESS;
}


static int
setupTaskManagement(BffIm_Kernel *kernel)
{
    /* Set up multi-tasking. */
    setupMultiTasking(kernel);

    return SUCCESS;
}


static void
setupMultiTasking(BffIm_Kernel *kernel)
{
    Bff_Applet_Driver *driver = &kernel->appletDriver;
    boolean multiTasking;


    /*
     * Check if a function to check whether the application wants to
     * enable multi-tasking is defined.
     */
    if (driver->isMultiTaskingEnabled == NULL)
    {
        /*
         * No function is defined. Assume that multi-tasking is not
         * required.
         */
        return;
    }

    /* We are about to call Bff_Applet_Driver.isMultiTaskingEnabled(). */
    BFF_DEBUG(("BFF:Call isMultiTaskingEnabled() of Bff_Applet_Driver"));

    /* Check whether the application is requesting multi-tasking. */
    multiTasking = driver->isMultiTaskingEnabled((Bff *)kernel);
    if (multiTasking)
    {
        /* Multi-tasking is requested. */
        kernel->flags.multiTaskingEnabled = 1;
    }
}


/*---------------------------------------------*
 * Initializers in the third phase
 *---------------------------------------------*/
static int
setupKernelOnAppStart(BffIm_Kernel *kernel)
{
    /* In the current implementation, there is nothing to do. */

    /* Setup on EVT_APP_START has been completed. */
    kernel->flags.setupOnAppStartCompleted = 1;

    return SUCCESS;
}


/*---------------------------------------------*
 * Bff methods
 *---------------------------------------------*/
static int
method_bff_createScene(
    Bff *self, Bff_Scene **scene, Bff_Scene_Driver *driver)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)self;
    BffIm_Scene *newScene = NULL;
    int ret;


    if (self == NULL || driver == NULL)
    {
        return EBADPARM;
    }

    /* Create a scene. */
    ret = createScene(kernel, &newScene, driver);
    if (ret != SUCCESS)
    {
        /* Failed to create a scene. */
        return ret;
    }

    /* Start to manage the new scene. */
    ret = manageScene(kernel, newScene);
    if (ret != SUCCESS)
    {
        /* Failed to start to manage the new scene. */
        destroyScene(kernel, newScene);

        return ret;
    }

    /* If the caller wants to have a pointer to the new scene. */
    if (scene)
    {
        *scene = (Bff_Scene *)newScene;
    }

    /* Successfully created a scene. */
    return SUCCESS;
}


static void
method_bff_free(Bff *self, void *area)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)self;


    if (self == NULL)
    {
        return;
    }

    kernel_free(kernel, area);
}


static Bff_Scene *
method_bff_getCurrentScene(Bff *self)
{
    if (self == NULL)
    {
        return NULL;
    }

    return (Bff_Scene *)getCurrentScene((BffIm_Kernel *)self);
}


static Bff_Task *
method_bff_getCurrentTask(Bff *self)
{
    if (self == NULL)
    {
        return NULL;
    }

    /* Return the current task. */
    return (Bff_Task *)((BffIm_Kernel *)self)->currentTask;
}


static void *
method_bff_getData(Bff *self)
{
    if (self == NULL)
    {
        return NULL;
    }

    /* Return the current value of applet data. */
    return ((BffIm_Kernel *)self)->appletData;
}


static Bff_Logging_Level
method_bff_getLoggingLevel(Bff *self)
{
    if (self == NULL)
    {
        return Bff_Logging_Level_INVALID;
    }

    /* Return the current logging level. */
    return ((BffIm_Kernel *)self)->loggingLevel;
}


static Bff_Scene *
method_bff_getSceneById(Bff *self, Bff_Scene_Id sceneId)
{
    if (self == NULL)
    {
        return NULL;
    }

    return (Bff_Scene *)getSceneById((BffIm_Kernel *)self, sceneId);
}


static Bff_Scene *
method_bff_getSceneByName(Bff *self, const char *name)
{
    if (self == NULL || name == NULL)
    {
        return NULL;
    }

    return (Bff_Scene *)getSceneByName((BffIm_Kernel *)self, name);
}


static Bff_Task *
method_bff_getTaskById(Bff *self, Bff_Task_Id taskId)
{
    if (self == NULL)
    {
        return NULL;
    }

    return (Bff_Task *)getTaskById((BffIm_Kernel *)self, taskId);
}


static Bff_Task *
method_bff_getTaskByName(Bff *self, const char *name)
{
    if (self == NULL || name == NULL)
    {
        return NULL;
    }

    return (Bff_Task *)getTaskByName((BffIm_Kernel *)self, name);
}


static void *
method_bff_malloc(Bff *self, size_t size)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)self;


    if (self == NULL)
    {
        return NULL;
    }

    return kernel_malloc(kernel, size);
}


static int
method_bff_releaseScene(Bff *self, Bff_Scene *scene)
{
    if (self == NULL || scene == NULL)
    {
        return EBADPARM;
    }

    return releaseScene((BffIm_Kernel *)self, (BffIm_Scene *)scene);
}


static int
method_bff_releaseSceneById(Bff *self, Bff_Scene_Id sceneId)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)self;
    BffIm_Scene *scene;


    if (self == NULL)
    {
        return EBADPARM;
    }

    scene = getSceneById(kernel, sceneId);
    if (scene == NULL)
    {
        return EBADITEM;
    }

    return releaseScene(kernel, scene);
}


static int
method_bff_releaseSceneByName(Bff *self, const char *name)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)self;
    BffIm_Scene *scene;


    if (self == NULL || name == NULL)
    {
        return EBADPARM;
    }

    scene = getSceneByName(kernel, name);
    if (scene == NULL)
    {
        return EBADITEM;
    }

    return releaseScene(kernel, scene);
}


static void
method_bff_setData(Bff *self, void *data)
{
    if (self == NULL)
    {
        return;
    }

    /* Set the applet data. */
    ((BffIm_Kernel *)self)->appletData = data;
}


static int
method_bff_setLoggingLevel(Bff *self, Bff_Logging_Level level)
{
    if (self == NULL)
    {
        return EBADPARM;
    }

    if (BFF_LOGGING_LEVEL_OUT_OF_RANGE(level))
    {
        /* The given value of 'level' is out of range. */
        return EBADPARM;
    }

    /* Set the current logging level. */
    ((BffIm_Kernel *)self)->loggingLevel = level;

    return SUCCESS;
}


/*---------------------------------------------*
 * BffIm_Kernel methods
 *---------------------------------------------*/
static void
method_kernel_finalize(BffIm_Kernel *self)
{
    if (self == NULL)
    {
        return;
    }

    /* Finalize the applet. */
    finalizeApplet(self);

    /* Finalize task management. */
    finalizeTaskManagement(self);

    /* Finalize scene management. */
    finalizeSceneManagement(self);

    BFF_INFO(("BFF:APPLET BEING STOPPED"));
    BFF_INFO(("BFF:  class ID = 0x%X", BFF_CLASS_ID_OF(self)));
}


static boolean
method_kernel_handleEvent(
    BffIm_Kernel *self, AEEEvent ev, uint16 w, uint32 dw)
{
    if (self == NULL)
    {
        return FALSE;
    }

    return handleEventByKernel(self, ev, w, dw);
}


/*---------------------------------------------*
 * Finalizers
 *---------------------------------------------*/
static void
finalizeApplet(BffIm_Kernel *kernel)
{
    Bff_Applet_Driver *driver = &kernel->appletDriver;


    /* If applet finalizer needs to be called and one is defined. */
    if (kernel->flags.appletNeedsFinalizerCall && driver->finalize)
    {
        /* Applet finalizer is defined. We are about to call it. */
        BFF_DEBUG(("BFF:Call finalize() of Bff_Applet_Driver"));

        /* Invoke the applet finalizer. */
        driver->finalize((Bff *)kernel);
    }
}


static void
finalizeTaskManagement(BffIm_Kernel *kernel)
{
    cleanupTaskList(kernel);
}


static void
finalizeSceneManagement(BffIm_Kernel *kernel)
{
    cleanupSceneList(kernel);
}


/*---------------------------------------------*
 * Event dispatching
 *---------------------------------------------*/
static boolean
handleEventByKernel(BffIm_Kernel *kernel, AEEEvent ev, uint16 w, uint32 dw)
{
    BffIm_Kernel_ReentrantData rd;
    Bff_Dispatcher *dsp;
    boolean consumed;
    boolean ret;
    boolean ret2;


    pushKernelReentrantData(kernel, &rd);

    /*
     * Initialize the work variable that holds a return value from
     * handleEventByApplet().
     */
    rd.returnValueFromApplet = FALSE;

    /*
     * Process the event by the pre-dispatcher of BFF Kernel before
     * letting the applet handle the event.
     */
    consumed = TRUE;
    dsp      = &kernel->kernelPreDispatcher;
    ret      = dsp->dispatch(dsp, (Bff *)kernel, &ev, &w, &dw, &consumed);
    if (consumed)
    {
        /*
         * The pre-dispatcher of BFF Kernel has consumed the event.
         * Skip executing the applet's event handlers.
         */
        goto post_process;
    }

    /*
     * Let the applet handle the event. Do not pass pointers of event
     * parameters to handleEventByApplet() in order to prevent the
     * applet from modifying values of the event parameters before
     * the parameters are passed to the post-dispatcher of BFF Kernel.
     */
    ret = handleEventByApplet(kernel, ev, w, dw, &consumed);

    /*
     * Copy the return value to the work variable because some event
     * handlers in the kernel post-dispatcher may want to refer to
     * the value.
     */
    rd.returnValueFromApplet = ret;

post_process:
    /*
     * Process the event by the post-dispatcher of BFF Kernel.
     *
     * The post-dispatcher is always executed regardless of whether
     * the event has already been consumed or not.
     */
    consumed = TRUE;
    dsp      = &kernel->kernelPostDispatcher;
    ret2     = dsp->dispatch(dsp, (Bff *)kernel, &ev, &w, &dw, &consumed);
    if (consumed)
    {
        /*
         * Only when the event has been consumed by the post-dispatcher
         * of BFF Kernel, the value of 'ret' is overwritten.
         */
        ret = ret2;
    }

    popKernelReentrantData(kernel);

    return ret;
}


static void
pushKernelReentrantData(
    BffIm_Kernel *kernel, BffIm_Kernel_ReentrantData *rd)
{
    rd->prev = kernel->reentrantData;
    kernel->reentrantData = rd;
}


static void
popKernelReentrantData(BffIm_Kernel *kernel)
{
    if (kernel->reentrantData)
    {
        kernel->reentrantData = kernel->reentrantData->prev;
    }
}


static boolean
handleEventByApplet(
    BffIm_Kernel *kernel, AEEEvent ev, uint16 w, uint32 dw,
    boolean *consumed)
{
    Bff_Dispatcher *dsp;
    boolean ret = FALSE;
    boolean ret2;


    /* If the pre-dispatcher of the applet has been set up. */
    if (kernel->flags.appletHasPreDispatcher)
    {
        /* Let the pre-dispatcher of the applet handle the event. */
        *consumed = TRUE;
        dsp = &kernel->appletPreDispatcher;
        ret = dsp->dispatch(dsp, (Bff *)kernel, &ev, &w, &dw, consumed);
        if (*consumed)
        {
            /*
             * The pre-dispatcher of the applet has consumed the event.
             * Skip executing event handlers of the current task.
             */
            goto post_process;
        }
    }

    /* Let the current task handle the event.*/
    ret = handleEventByTask(kernel, &ev, &w, &dw, consumed);

post_process:
    /* If the post-dispatcher of the applet has been set up. */
    if (kernel->flags.appletHasPostDispatcher)
    {
        /*
         * Let the post-dispatcher of the applet handle the event.
         *
         * The post-dispatcher is always executed (if defined) regardless
         * of whether the event has already been consumed or not.
         */
        *consumed = TRUE;
        dsp  = &kernel->appletPostDispatcher;
        ret2 = dsp->dispatch(dsp, (Bff *)kernel, &ev, &w, &dw, consumed);
        if (*consumed)
        {
            /*
             * Only when the post-dispatcher of the applet has consumed
             * the event, 'ret' is overwritten.
             */
            ret = ret2;
        }
    }

    return ret;
}


static boolean
handleEventByTask(
    BffIm_Kernel *kernel, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed)
{
    BffIm_Task *task = kernel->currentTask;
    boolean ret;


    if (task == NULL)
    {
        /* No task is running. */
        return (*consumed = FALSE);
    }

    /* Increment the event nest level. */
    task->eventNestLevel++;

    /* Let the task handle the event. */
    ret = task->handleEvent(task, ev, w, dw, consumed);

    /* Decrement the event nest level. */
    task->eventNestLevel--;

    return ret;
}


/*---------------------------------------------*
 * Kernel event handlers
 *---------------------------------------------*/
static boolean 
kernel_pre_hookFirstEvent(
    Bff *bff, AEEEvent *ev, uint16 *w, uint32 *dw, boolean *consumed)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)bff;
    int ret;


    /*
     * This function (kernel_pre_hookFirstEvent()) has been registered
     * as preFixup() function. Detach this function from the list of
     * event handlers to ensure that this function is executed only once.
     */
    kernel->kernelPreDispatcher.handlers.preFixup = NULL;

    /* Start the second phase of the initialization. */
    ret = setupKernelOnFirstEvent(kernel);
    if (ret != SUCCESS)
    {
        /* Setup failed. */
        BFF_ERROR(("BFF:Set up on the first event failed."));

        /* Register a special handler for setup failure. */
        kernel->kernelPreDispatcher.handlers.preDispatch =
            kernel_pre_handlerOnFailure;

        /* Warn that this application is about to stop. */
        BFF_WARN(("BFF:Stopping due to initialization failure."));

        /*
         * Stop this application. An EVT_APP_STOP event will be
         * sent to this application soon and then handled by
         * kernel_pre_handlerOnFailure().
         */
        ISHELL_CloseApplet(BFF_SHELL_OF(bff), FALSE);

        /* Do not pass this event to other event handlers. */
        *consumed = TRUE;

        /* The return value below will be returned to BREW AEE. */
        return FALSE;
    }

    /* Continue to process this event. */
    *consumed = FALSE;

    /* BFF initialization has been done successfully. */
    BFF_DEBUG(("BFF:Setup on the first event completed."));

    /*
     * The return value below does not have any meaning because the event
     * has not been consumed in this function.
     */
    return FALSE;
}


static boolean
kernel_pre_handlerOnFailure(
    Bff *bff, AEEEvent ev, uint16 w, uint32 dw, boolean *consumed)
{
    /*
     * All events are consumed here, so any other event handler won't
     * see this event.
     */
    *consumed = TRUE;

    return FALSE;
}


static boolean
kernel_pre_preDispatch(
    Bff *bff, AEEEvent ev, uint16 w, uint32 dw, boolean *consumed)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)bff;
    BffIm_Task *task = NULL;
    int ret;


    /* Check if multi-tasking support is enabled. */
    if (!kernel->flags.multiTaskingEnabled)
    {
        /*
         * There is nothing additional to do here if multi-tasking
         * support is not enabled. Pass this event to subsequent
         * event handlers.
         */
        return (*consumed = FALSE);
    }

    /* Check if initialization on EVT_APP_START has been completed. */
    if (!kernel->flags.setupOnAppStartCompleted)
    {
        /*
         * This applet has not received EVT_APP_START event yet,
         * meaning that task-specific processing below does not
         * have to be performed.
         */
        return (*consumed = FALSE);
    }

    if (!kernel->flags.needsCurrentTaskUpdate)
    {
        /* The pointer to the current task does not need to be updated. */
        return (*consumed = FALSE);
    }

    /*
     * Get a pointer of BffIm_Task from the history context.
     *
     * BREW AEE does not provide a mechanism to allow each history
     * entry of the same applet to have each own event handler.
     * In other words, all running instances of the same applet
     * share the identical global event handler. This makes it
     * difficult to write a multi-tasking aware BREW application.
     *
     * BFF provides the mechanism on behalf of BREW. BFF tries to
     * get the stored task from the history context and set it as
     * the current task.
     */
    ret = getTaskInHistoryData(&task);
    if (ret == SUCCESS)
    {
        /* Succeeded in getting task information. */

        /*
         * 'task' holds information about the current running applet
         * instance. If 'task' is null, it is highly likely that a
         * new task is created in a subsequent event handler,
         * "kernel_pre_onAppStart" or "kernel_pre_onAppResume" (but
         * this is not always true).
         */
        kernel->currentTask = task;

        /* Pass this event to subsequent event handlers. */
        *consumed = FALSE;
    }
    else
    {
        /* Failed to get task information from the history context. */
        onCurrentTaskUpdateFailure(kernel);

        /* Do not allow application-defined event handlers to run. */
        *consumed = TRUE;
    }

    /* The current task has been updated. */
    kernel->flags.needsCurrentTaskUpdate = 0;

    return FALSE;
}


static boolean
kernel_pre_onAppStart(
    Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)bff;
    int ret;


    /* An EVT_APP_START event has been delivered. */
    BFF_INFO(("BFF:APPLET STARTED"));
    BFF_INFO(("BFF:  class ID = 0x%X", BFF_CLASS_ID_OF(bff)));

    /* Execute the third initialization. */
    ret = setupKernelOnAppStart(kernel);
    if (ret != SUCCESS)
    {
        /* Failed to set up the kernel. */
        goto error;
    }

    /* Create and set the current task. */
    ret = createCurrentTask(kernel, flags, appStart);
    if (ret != SUCCESS)
    {
        /* Failed to create the first task. */
        goto error;
    }

    /*
     * The first task has been created successfully.
     *
     * We do not consume this EVT_APP_START event here so that
     * it can be passed to subsequent event handlers.
     */
    *consumed = FALSE;

    /*
     * Since the event has not been consumed here, the return value
     * below does not have any meaning, but let's return TRUE :-)
     */
    return TRUE;

error:
    /* Do not allow application-defined event handlers to run. */
    *consumed = TRUE;

    /* Tell BREW AEE that the application cannot start. */
    return FALSE;
}


static boolean
kernel_pre_onAppResume(
    Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)bff;
    BffIm_Task *task = NULL;
    int ret;


    /* Check if multi-tasking support is enabled. */
    if (!kernel->flags.multiTaskingEnabled)
    {
        /* Pass this event to subsequent event handlers. */
        return (*consumed = FALSE);
    }

    /* Try to get a task pointer from the history context. */
    ret = getTaskInHistoryData(&task);
    if (ret != SUCCESS)
    {
        /* Failed to retrieve a task pointer from the history context. */
        goto error;
    }

    if (task)
    {
        /* "task!=NULL" means that a suspended task was resumed just now. */
        BFF_INFO(("BFF:TASK RESUMED"));
        BFF_INFO(("BFF:  task ID = %ld", (long)task->id));
        BFF_INFO(("BFF:  task name = %s", task->name));

        /* The task is no longer suspended. */
        task->flags.suspended = 0;

        /* The resumed task restarts working as the current task. */
        kernel->currentTask = task;

        /* Pass this event to subsequent event handlers. */
        return (*consumed = FALSE);
    }

    /* This running instance is a new entry in the application history. */
    ret = createCurrentTask(kernel, flags, appStart);
    if (ret != SUCCESS)
    {
        /* Failed to create the current task. */
        goto error;
    }

    /* Pass this event to subsequent event handlers. */
    return (*consumed = FALSE);

error:
    onCurrentTaskUpdateFailure(kernel);

    /* Do not allow application-defined event handlers to run. */
    *consumed = TRUE;

    return FALSE;
}


static boolean
kernel_post_postDispatch(
    Bff *bff, AEEEvent ev, uint16 w, uint32 dw, boolean *consumed)
{
    BffIm_Task *task = ((BffIm_Kernel *)bff)->currentTask;


    if ((task != NULL) &&
        (task->eventNestLevel < task->scheduledSceneEventNestLevel))
    {
        task->switchScenes(task);
    }

    return (*consumed = FALSE);
}


static boolean
kernel_post_onAppSuspend(
    Bff *bff, AEESuspendReason reason, AEESuspendInfo *info,
    boolean *consumed)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)bff;
    BffIm_Kernel_ReentrantData *rd = kernel->reentrantData;


    /* If the application has chosen to get suspended. */
    if (rd && rd->returnValueFromApplet == TRUE)
    {
        /* Mark the current task as 'suspended'. */
        BffIm_Task *task = kernel->currentTask;

        if (task)
        {
            task->flags.suspended = 1;

            /* The task is about to be suspended. */
            BFF_INFO(("BFF:TASK BEING SUSPENDED"));
            BFF_INFO(("  task ID = %ld", (long)task->id));
            BFF_INFO(("  task name = %s", task->name));
        }
    }

    return (*consumed = FALSE);
}


static boolean
kernel_post_onAppStop(Bff *bff, boolean *close, boolean *consumed)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)bff;
    BffIm_Task *task = kernel->currentTask;


    if (task)
    {
        /* The task is about to stop. */
        BFF_INFO(("BFF:TASK BEING STOPPED"));
        BFF_INFO(("BFF:  task ID = %ld", (long)task->id));
        BFF_INFO(("BFF:  task name = %s", task->name));

        /* Stop managing the task. */
        unmanageTask(kernel, task);

        /* Destroy the task. */
        destroyTask(kernel, task);
    }

    /* The current task needs to be updated. */
    kernel->flags.needsCurrentTaskUpdate = 1;

    return (*consumed = FALSE);
}


/*---------------------------------------------*
 * Task management
 *---------------------------------------------*/
static int
createCurrentTask(BffIm_Kernel *kernel, uint16 flags, AEEAppStart *appStart)
{
    BffIm_Task *newTask = NULL;
    int ret;


    /* Create a task. */
    ret = createTask(kernel, &newTask, flags, appStart);
    if (ret != SUCCESS)
    {
        /* Failed to create a task. */
        return ret;
    }

    /* Start to manage the new task. */
    ret = manageTask(kernel, newTask);
    if (ret != SUCCESS)
    {
        /* Failed to start to manage the new task. */
        destroyTask(kernel, newTask);

        return ret;
    }

    /* Set the new task as the current task. */
    kernel->currentTask = newTask;

    /* The new task has started working. */
    BFF_INFO(("BFF:TASK STARTED"));
    BFF_INFO(("  task ID = %ld", (long)newTask->id));
    BFF_INFO(("  task name = %s", newTask->name));

    /*
     * Try scene switching because Bff_Task_Driver.initialize()
     * may have scheduled the initial scene.
     */
    newTask->switchScenes(newTask);

    return SUCCESS;
}


static int
createTask(
    BffIm_Kernel *kernel, BffIm_Task **task, uint16 flags,
    AEEAppStart *appStart)
{
    BffIm_Task *newTask;
    Bff_Task_Driver driver;
    Bff_Task_Id taskId;
    int ret;


    /*
     * Get a task ID for the new task.
     *
     * We assume that the task ID will never reach the maximum number
     * (because the maximum number is so big).
     *
     * Note that (Bff_Task_Id_INVALID + 1) equals Bff_Task_Id_FIRST.
     */
    taskId = kernel->lastTaskId + 1;

    /* Get a task driver. */
    ret = getTaskDriver(kernel, taskId, &driver);
    if (ret != SUCCESS)
    {
        /* Failed to get a task driver. */
        BFF_ERROR(("Failed to get a task driver."));

        return ret;
    }

    /* Allocate a memory area for a new task. */
    newTask = kernel_malloc(kernel, sizeof(BffIm_Task));
    if (newTask == NULL)
    {
        /* Failed to allocate a memory area. */
        BFF_ERROR(("Task not created due to memory shortage."));

        return ENOMEMORY;
    }

    /* Initialize the memory area as a task. */
    ret = BffIm_Task_initialize(newTask, kernel, taskId, &driver, flags,
                                appStart);
    if (ret != SUCCESS)
    {
        /* Failed to initialize a task. */
        BFF_ERROR(("Task not created. Initialization failed."));

        /* Release the memory area allocated just above. */
        kernel_free(kernel, newTask);

        return ret;
    }

    /* The assigned task ID is used by the new task. */
    kernel->lastTaskId = taskId;

    /* Return the new task to the caller. */
    *task = newTask;

    /* A task was created successfully. */
    return SUCCESS;
}


static void
destroyTask(BffIm_Kernel *kernel, BffIm_Task *task)
{
    task->finalize(task);

    kernel_free(kernel, task);
}


static void
onCurrentTaskUpdateFailure(BffIm_Kernel *kernel)
{
    int ret;


    /*
     * Could not update kernel->currentTask properly. This means
     * that the current running IAppHistory instance and the task
     * pointed to by kernel->currentTask do not match. In this
     * case, we should avoid calling BffIm_Task.handleEvent() of
     * kernel->currentTask.
     *
     */
    kernel->currentTask = NULL;

    /* Remove the current IAppHistory instance from the history context. */
    ret = stopCurrentAppHistoryInstance();
    if (ret != SUCCESS)
    {
        /* Last resort. Stop the entire applet. */
        BFF_WARN(("BFF:Stopping this applet."));
        ISHELL_CloseApplet(BFF_SHELL_OF(kernel), FALSE);
    }
}


static int
stopCurrentAppHistoryInstance(void)
{
    IAppHistory *appHistory = NULL;
    int ret;


    /* Get an IAppHistory instance to stop the current entry. */
    ret = getAppHistory(&appHistory);
    if (ret != SUCCESS)
    {
        /*
         * Failed to get an IAppHistory instance. Cannot remove
         * the current running instance.
         */
        return ret;
    }

    /* About to issue a 'remove the current entry' command. */
    BFF_WARN(("BFF:Stopping the current IAppHistory instance."));

    /* Stop the current running IAppHistory entry. */
    ret = IAPPHISTORY_Remove(appHistory);

    IAPPHISTORY_Release(appHistory);

    if (ret != SUCCESS)
    {
        /* Failed to remove the current entry. */
        BFF_ERROR(("BFF:IAPPHISTORY_Remove()"));

        return ret;
    }

    /* Succeeded in removing the current IAppHistory instance. */
    return SUCCESS;
}


static int
getTaskDriver(
    BffIm_Kernel *kernel, Bff_Task_Id taskId, Bff_Task_Driver *driver)
{
    int ret;


    /* By default, there is no function in a task driver. */
    clearTaskDriver(driver);

    /* Check if a function to set up a task driver is defined. */
    if (kernel->appletDriver.getTaskDriver == NULL)
    {
        /* A function to set up a task driver is not defined. */
        return SUCCESS;
    }

    /* We are about to call Bff_Applet_Driver.getTaskDriver(). */
    BFF_DEBUG(("BFF:Call getTaskDriver() of Bff_Task_Driver"));

    ret = kernel->appletDriver.getTaskDriver((Bff *)kernel, taskId, driver);
    if (ret != SUCCESS)
    {
        /* Failed to get a task driver. */
        BFF_ERROR(("BFF:getTaskDriver() of Bff_Applet_Driver"));
        BFF_ERROR(("  ret = %d", ret));

        return ret;
    }

    /* Succeeded in getting a task driver. */
    return SUCCESS;
}


static void
clearTaskDriver(Bff_Task_Driver *driver)
{
    /*
     * Strictly speaking, this is not technically correct, but this
     * implementation assumes that internal representations of null
     * pointers are zero.
     */
    MEMSET(driver, 0, sizeof(Bff_Task_Driver));
}


static int
manageTask(BffIm_Kernel *kernel, BffIm_Task *task)
{
    int ret;


    if (kernel->flags.multiTaskingEnabled)
    {
        /*
         * Store the pointer of the task in the history context.
         * As a side effect, multi-tasking gets enabled if it
         * has not been yet.
         */
        ret = setTaskInHistoryData(task);
        if (ret != SUCCESS)
        {
            return ret;
        }
    }

    addTaskToList(kernel, task);

    return SUCCESS;
}


static void
unmanageTask(BffIm_Kernel *kernel, BffIm_Task *task)
{
    removeTaskFromList(kernel, task);

    if (kernel->currentTask == task)
    {
        kernel->currentTask = NULL;
    }
}


static void
addTaskToList(BffIm_Kernel *kernel, BffIm_Task *task)
{
    if (kernel->taskList)
    {
        kernel->taskList->prev = task;
    }

    /* Set up links of the new task. */
    task->prev = NULL;
    task->next = kernel->taskList;

    /* Replace the top of the task list with the new task. */
    kernel->taskList = task;
}


static void
removeTaskFromList(BffIm_Kernel *kernel, BffIm_Task *task)
{
    if (task->prev)
    {
        task->prev->next = task->next;
    }

    if (task->next)
    {
        task->next->prev = task->prev;
    }

    if (kernel->taskList == task)
    {
        kernel->taskList = task->next;
    }

    task->prev = NULL;
    task->next = NULL;
}


static void
cleanupTaskList(BffIm_Kernel *kernel)
{
    BffIm_Task *task;
    BffIm_Task *next;


    for (task = kernel->taskList; task; task = next)
    {
        next = task->next;

        unmanageTask(kernel, task);

        destroyTask(kernel, task);
    }
}


static int
getAppHistory(IAppHistory **appHistory)
{
    IAppHistory *ah = NULL;
    int ret;


    /* Create an IAppHistory instance. */
    ret = ISHELL_CreateInstance(BFF_SHELL(),
                                AEECLSID_APPHISTORY,
                                (void **)&ah);
    if (ret != SUCCESS)
    {
        /* Failed to create an IAppHistory instance. */
        BFF_ERROR(("BFF:IAppHistory instance not created."));

        return ret;
    }

    /* An IAppHistory instance has been created successfully. */
    *appHistory = ah;

    return SUCCESS;
}


static int
getTaskInHistoryData(BffIm_Task **task)
{
    int size = sizeof(BffIm_Task *);
    int ret;


    ret = getHistoryData(NULL, task, &size);
    switch (ret)
    {
        case SUCCESS:
            /* Could get the stored task data. */
            return SUCCESS;

        case EFAILED:
            /* No stored task data, but this is not an error. */
            *task = NULL;
            return SUCCESS;

        default:
            /*
             * Unexpected error.
             * Task management cannot remain consistent any more.
             */
            BFF_FATAL(("BFF:Cannot get task data."));

            return ret;
    }
}


static int
setTaskInHistoryData(BffIm_Task *task)
{
    int ret;


    /*
     * Store a task pointer in the history context. The name associated
     * with the data is intentionally NULL. As a side effect of this,
     * multi-tasking gets enabled. See the following excerpt from the
     * BREW API document about IAppHistory.
     *
     *   --------------------------------------------------------------
     *   Rules on creating history entry for an app: Every application
     *   that is either top-visible or is suspended has an associated
     *   history entry. When there is an attempt to start an app and
     *   make it top-visible (i.e. when ISHELL_StartApplet() or one of
     *   the macros based on this function) is invoked, BREW will
     *   associate an apphistory entry with this app. If the app does
     *   not already have a history entry with it (i.e if it is not
     *   already in the list of suspended apps), a new history entry
     *   will be created for this app. If this app is already in the
     *   history list, the following rules will be used to decide
     *   whether to create a new entry for this app OR to move the
     *   already existing history entry to the top of the list:
     *   [a] If the app has set resume data previously (when it got
     *   suspended) by invoking IAppHistory_SetResumeData(), then a
     *   new history entry will be created for the app when
     *   ISHELL_StartApplet() is invoked on it again [b] If the app
     *   has not set resume data previously by calling
     *   IAppHistory_SetResumeData(), then a new history entry will
     *   not be created for this app when it is started again. Instead,
     *   the already existing history entry will be moved to the top
     *   of the list (to make this app top-visible). 
     *   --------------------------------------------------------------
     */
    ret = setHistoryData(NULL, &task, sizeof(BffIm_Task *));
    if (ret != SUCCESS)
    {
        /*
         * Failed to set task data in the history context.
         * Task management cannot remain consistent any more.
         */
        BFF_FATAL(("BFF:Cannot set task data."));

        return ret;
    }

    return SUCCESS;
}


static int
getHistoryData(const char *name, void *buf, int *size)
{
    IAppHistory *appHistory = NULL;
    int ret;


    ret = getAppHistory(&appHistory);
    if (ret != SUCCESS)
    {
        /*
         * Failed to get an IAppHistory instance. Return an error code
         * that is not equal to AEE_EFAILED because AEE_EFAILED indicates
         * "Date was not found".
         */
        return EBADSTATE;
    }

    ret = IAPPHISTORY_GetData(appHistory, name, buf, size);

    IAPPHISTORY_Release(appHistory);

    switch (ret)
    {
        case AEE_SUCCESS:
            /* Data was found. */
            return SUCCESS;

        case EFAILED:
            /* Data was not found. */
            return ret;

        default:
            /* Unexpected error. */
            BFF_ERROR(("BFF:IAPPHISTORY_GetData()"));
            BFF_ERROR(("BFF:  ret = %d", ret));
            BFF_ERROR(("BFF:  name = %s", name ? name : "(null)"));

            return ret;
    }
}


static int
setHistoryData(const char *name, void *buf, int size)
{
    IAppHistory *appHistory = NULL;
    int ret;


    ret = getAppHistory(&appHistory);
    if (ret != SUCCESS)
    {
        /* Failed to get an IAppHistory instance. */
        return ret;
    }

    ret = IAPPHISTORY_SetData(appHistory, name, buf, size);

    IAPPHISTORY_Release(appHistory);

    if (ret != AEE_SUCCESS)
    {
        /* IAPPHISTORY_SetData() failed. */
        BFF_ERROR(("BFF:IAPPHISTORY_SetData()"));
        BFF_ERROR(("BFF:  ret = %d", ret));
        BFF_ERROR(("BFF:  name = %s", name ? name : "(null)"));

        return ret;
    }

    return SUCCESS;
}


static BffIm_Task *
getTaskById(BffIm_Kernel *kernel, Bff_Task_Id taskId)
{
    BffIm_Task *task;


    /* This implementation assumes the total number of tasks is not big. */
    for (task = kernel->taskList; task; task = task->next)
    {
        if (task->id == taskId)
        {
            return task;
        }
    }

    return NULL;
}


static BffIm_Task *
getTaskByName(BffIm_Kernel *kernel, const char *name)
{
    BffIm_Task *task;


    /* This implementation assumes the total number of tasks is not big. */
    for (task = kernel->taskList; task; task = task->next)
    {
        if (STRCMP(task->name, name) == 0)
        {
            return task;
        }
    }

    return NULL;
}


/*---------------------------------------------*
 * Scene management
 *---------------------------------------------*/
static BffIm_Scene *
getCurrentScene(BffIm_Kernel *kernel)
{
    if (kernel->currentTask == NULL)
    {
        return NULL;
    }

    return kernel->currentTask->currentScene;
}


static int
createScene(
    BffIm_Kernel *kernel, BffIm_Scene **scene, Bff_Scene_Driver *driver)
{
    BffIm_Scene *newScene;
    Bff_Scene_Id sceneId;
    int ret;


    /*
     * Get a scene ID for the new scene.
     *
     * We assuem that the scene ID will never reach the maximum number
     * (because the maximum number is so big).
     */
    sceneId = kernel->lastSceneId + 1;

    /* Allocate a memory area for a new scene. */
    newScene = kernel_malloc(kernel, sizeof(BffIm_Scene));
    if (newScene == NULL)
    {
        /* Failed to allocate a memory area. */
        BFF_ERROR(("Scene not created due to memory shortage."));

        return ENOMEMORY;
    }

    /* Initialize the memory area as a scene. */
    ret = BffIm_Scene_initialize(newScene, kernel, sceneId, driver);
    if (ret != SUCCESS)
    {
        /* Failed to initialize a scene. */
        BFF_ERROR(("Scene not created. Initialization failed."));

        /* Release the memory area allocated just above. */
        kernel_free(kernel, newScene);

        return ret;
    }

    /* The assigned scene ID is used by the new scene. */
    kernel->lastSceneId = sceneId;

    /* Return the new scene to the caller. */
    *scene = newScene;

    /* A scene was created successfully. */
    return SUCCESS;
}


static int
releaseScene(BffIm_Kernel *kernel, BffIm_Scene *scene)
{
    if (scene->flags.current || scene->flags.scheduled)
    {
        return EBADSTATE;
    }

    unmanageScene(kernel, scene);

    destroyScene(kernel, scene);

    return SUCCESS;
}


static void
destroyScene(BffIm_Kernel *kernel, BffIm_Scene *scene)
{
    scene->finalize(scene);

    kernel_free(kernel, scene);
}


static int
manageScene(BffIm_Kernel *kernel, BffIm_Scene *scene)
{
    addSceneToList(kernel, scene);

    return SUCCESS;
}


static void
unmanageScene(BffIm_Kernel *kernel, BffIm_Scene *scene)
{
    removeSceneFromList(kernel, scene);
}


static void
addSceneToList(BffIm_Kernel *kernel, BffIm_Scene *scene)
{
    if (kernel->sceneList)
    {
        kernel->sceneList->prev = scene;
    }

    /* Set up links of the new scene. */
    scene->prev = NULL;
    scene->next = kernel->sceneList;

    /* Replace the top of the scene list with the new task. */
    kernel->sceneList = scene;
}


static void
removeSceneFromList(BffIm_Kernel *kernel, BffIm_Scene *scene)
{
    if (scene->prev)
    {
        scene->prev->next = scene->next;
    }

    if (scene->next)
    {
        scene->next->prev = scene->prev;
    }

    if (kernel->sceneList == scene)
    {
        kernel->sceneList = scene->next;
    }

    scene->prev = NULL;
    scene->next = NULL;
}


static void
cleanupSceneList(BffIm_Kernel *kernel)
{
    BffIm_Scene *scene;
    BffIm_Scene *next;


    for (scene = kernel->sceneList; scene; scene = next)
    {
        next = scene->next;

        unmanageScene(kernel, scene);

        destroyScene(kernel, scene);
    }
}


static BffIm_Scene *
getSceneById(BffIm_Kernel *kernel, Bff_Scene_Id sceneId)
{
    BffIm_Scene *scene;


    /*
     * TODO:
     *
     * The implementation below (linear search) assumes the total number
     * of scenes is not big. However, it is possible that many scenes are
     * created. Therefore, a hash table or something similar to look up
     * a scene by ID should be implemented.
     */

    for (scene = kernel->sceneList; scene; scene = scene->next)
    {
        if (scene->id == sceneId)
        {
            return scene;
        }
    }

    return NULL;
}


static BffIm_Scene *
getSceneByName(BffIm_Kernel *kernel, const char *name)
{
    BffIm_Scene *scene;


    /*
     * TODO:
     *
     * The implementation below (linear search) assumes the total number
     * of scenes is not big. However, it is possible that many scenes are
     * created. Therefore, a hash table or something similar to look up
     * a scene by name should be implemented.
     */

    for (scene = kernel->sceneList; scene; scene = scene->next)
    {
        if (STRCMP(scene->name, name) == 0)
        {
            return scene;
        }
    }

    return NULL;
}


/*---------------------------------------------*
 * Memory management
 *---------------------------------------------*/
static void *
kernel_malloc(BffIm_Kernel *kernel, size_t size)
{
    /* TODO; memory usage statistics. memory leak failsafe. */
    return kernel->appletDriver.malloc((Bff *)kernel, size);
}


static void
kernel_free(BffIm_Kernel *kernel, void *area)
{
    kernel->appletDriver.free((Bff *)kernel, area);
}


static void *
default_driver_malloc(Bff *self, size_t size)
{
    uint32 maxMemNodeSize = 0;


    if (self == NULL)
    {
        return NULL;
    }

    /*
     * MALLOC() should return a NULL pointer if it cannot allocate
     * a memory area of the required size. However, there are some
     * devices which simply crash if a very large value is given to
     * MALLOC(). Therefore, check the amount of available memory
     * before calling MALLOC().
     */
    GETRAMFREE(NULL, &maxMemNodeSize);
    if (maxMemNodeSize < size)
    {
        return NULL;
    }

    return MALLOC(size);
}


static void
default_driver_free(Bff *self, void *area)
{
    if (self == NULL)
    {
        return;
    }

    FREE(area);
}


/*------------------------------------------------------------------------*
 * FUNCTIONS
 *------------------------------------------------------------------------*/
int
BffIm_Kernel_getSize(AEECLSID classId, uint16 *size)
{
    if (size == NULL)
    {
        return EBADPARM;
    }

    /*
     * Return the size of the memory area required by BFF Kernel.
     *
     * An application class ID is passed to this function, but it is
     * not used in the current implementation.
     */
    *size = sizeof(BffIm_Kernel);

    return SUCCESS;
}


int
BffIm_Kernel_initialize(
    BffIm_Kernel *instance, AEECLSID classId, uint16 size,
    Bff_Applet_Driver *driver)
{
    int ret;


    if (instance == NULL || driver == NULL)
    {
        return EBADPARM;
    }

    /*
     * Start the first phase of the initialization and return the result.
     *
     * Note that the event loop has not started yet at this moment.
     * The second phase of the initialization will be performed later
     * by setupKernelOnFirstEvent().
     */
    ret = setupKernelOnCreation(instance, classId, size, driver);
    if (ret != SUCCESS)
    {
        /* Failed to set up BFF Kernel. */
        return ret;
    }

    return SUCCESS;
}
