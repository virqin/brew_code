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
 * BREW Foundation Framework, Task Implementation.
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, ã¿ã¹ã¯å®è£? * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFFIM_TASK_H
#define BFFIM_TASK_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEStdLib.h"
#include "bff/bff-dispatcher.h"
#include "bff/bff-task.h"
#include "bff/bff-task-driver.h"
#include "bffim/bffim-types.h"


/*------------------------------------------------------------------------*
 * MACROS
 *------------------------------------------------------------------------*/
/**
 * @english
 * Format to be used to generate the initial value of task name.
 * @endenglish
 *
 * @japanese
 * ã¿ã¹ã¯åã®åæå¤ãçæããã®ã«ä½¿ããã©ã¼ãããæå­å? * @endjapanese
 */
#define BFFIM_TASK_NAME_FORMAT  "Task%ld"


/*------------------------------------------------------------------------*
 * STRUCT DECLARATIONS
 *------------------------------------------------------------------------*/
/*
struct BffIm_Task;
typedef struct BffIm_Task BffIm_Task;*/



/*------------------------------------------------------------------------*
 * STRUCTS
 *------------------------------------------------------------------------*/
/**
 * @english
 * BFF Task Implementation
 *
 * BffIm_Task extends Bff_Task as shown below.
 *
 * @code
 *       Bff_Task             BffIm_Task
 *   +---------------+ - - +---------------+
 *   |               |     |               |
 *   |               |     |   Bff_Task    |
 *   |               |     |               |
 *   +---------------+ - - + - - - - - - - +
 *                         |               |
 *                         |               |
 *                         |               |
 *                         +---------------+
 * @endcode
 *
 * Bff_Task contains interfaces that are open to BFF applications.
 * On the other hand, BffIm_Task contains implementation specific
 * methods and data fields that should not be touched directly by
 * BFF applications.
 * @endenglish
 *
 * @japanese
 * BFF ã¿ã¹ã¯å®è£? *
 * BffIm_Task ã¯ãä¸å³ãç¤ºãããã«ã?Bff_Task ãæ¡å¼µãã¾ãã? *
 * @code
 *       Bff_Task            BffIm_Task
 *   +--------------+ - - +--------------+
 *   |              |     |              |
 *   |              |     |   Bff_Task   |
 *   |              |     |              |
 *   +--------------+ - - + - - - - - -  +
 *                        |              |
 *                        |              |
 *                        |              |
 *                        +--------------+
 * @endcode
 *
 * Bff_Task ã¯ãBFF ã¢ããªã±ã¼ã·ã§ã³ã«å¬éãã¦ããã¤ã³ã¿ã¼ãã§ã¼ã¹ã<!--
 * -->å«ãã§ãã¾ããä¸æ¹ã?BffIm_Task ã¯å®è£ã«ç¹åããã¡ã½ããã?!--
 * -->ãã¼ã¿ãã£ã¼ã«ããå«ãã§ããããããã?BFF ã¢ããªã±ã¼ã·ã§ã³ã?!--
 * -->ç´æ¥æä½ãã¹ãã§ã¯ããã¾ããã? * @endjapanese
 */
typedef struct _BffIm_Task
{
    /**
     * @english
     * Public interface of a task.
     *
     * This data field must be located at the top of this structure
     * so that this structure can be cast by Bff_Task.
     * @endenglish
     *
     * @japanese
     * ã¿ã¹ã¯ã®å¬éã¤ã³ã¿ã¼ãã§ã¼ã¹
     *
     * ãã®æ§é ä½ã?Bff_Task ã§ã­ã£ã¹ãã§ããããããã®ãã¼ã¿<!--
     * -->ãã£ã¼ã«ãã¯ãã®æ§é ä½ã®åé ­ã«ãªããã°ããã¾ããã?     * @endjapanese
     */
    Bff_Task task;


    /*-------------------------------------------*
     * Methods
     *-------------------------------------------*/

    /**
     * @english
     * Finalize this task.
     *
     * @param[in] self  A task implementation
     * @endenglish
     *
     * @japanese
     * ãã®ã¿ã¹ã¯ã®çµäºå¦çããããªãã?     *
     * @param[in] self  ã¿ã¹ã¯å®è£?     * @endjapanese
     */
    void (*finalize)(BffIm_Task *self);

    /**
     * @english
     * Handle a BREW event.
     *
     * @param[in]  self      A task implementation
     * @param[in]  ev        An event code
     * @param[in]  w         The first parameter of the event
     * @param[in]  dw        The second parameter of the event
     * @param[out] consumed  TRUE is stored back if the event has been
     *                       consumed by this method. Otherwise, FALSE
     *                       is stored back.
     *
     * @return  A boolean value that should be returned back to BREW
     *          AEE. The value and the meaning of the returned value
     *          varies depending on the value of 'ev'.
     * @endenglish
     *
     * @japanese
     * BREW ã¤ãã³ããå¦çããã?     *
     * @param[in]  self      ã¿ã¹ã¯å®è£?     * @param[in]  ev        ã¤ãã³ãã³ã¼ã?     * @param[in]  w         ã¤ãã³ãã®ç¬¬ä¸ãã©ã¡ã¼ã?     * @param[in]  dw        ã¤ãã³ãã®ç¬¬äºãã©ã¡ã¼ã¿
     * @param[out] consumed  ã¤ãã³ãããã®ã¡ã½ããã«ããæ¶è²»ãããå ´åã¯
     *                       TRUE ããããã§ãªãå ´åã?FALSE
     *                       ãæ¸ãæ»ããã¾ãã?     *
     * @return  BREW AEE ã«è¿ãã¹ãæ»ãå¤ãæ»ãå¤ã®å¤ã¨æå³ã?ev
     *          ã®å¤ã«ãã£ã¦ç°ãªãã¾ãã?     * @endjapanese
     */
    boolean (*handleEvent)(
        BffIm_Task *self, AEEEvent *ev, uint16 *w, uint32 *dw,
        boolean *consumed);

    /**
     * @english
     * Switch scenes if there is a scheduled scene.
     *
     * @param[in] self  A task implementation
     *
     * @retval SUCCESS    Successfully performed scene switching, or
     *                    there is no scheduled scene.
     * @retval EBADSTATE  Scene switching is being blocked.
     * @endenglish
     *
     * @japanese
     * ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ãããã°ãã·ã¼ã³ã®åãæ¿ãããããªãã?     *
     * @param[in] self  ã¿ã¹ã¯å®è£?     *
     * @retval SUCCESS    ã·ã¼ã³ã®åãæ¿ãã«æåãã¾ããããããã¯ã?!--
     *                    -->ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ãããã¾ããã?     * @retval EBADSTATE  ã·ã¼ã³åãæ¿ãããã­ãã¯ããã¦ãã¾ãã?     * @endjapanese
     *
     * @see Bff_Task.blockSceneSwitching()
     */
    int (*switchScenes)(BffIm_Task *self);


    /*-------------------------------------------*
     * Data fields
     *-------------------------------------------*/

    /**
     * @english
     * Task ID of this task.
     * @endenglish
     *
     * @japanese
     * ãã®ã¿ã¹ã¯ã®ã¿ã¹ã?ID
     * @endjapanese
     *
     * @see Bff_Task.getId()
     */
    Bff_Task_Id id;

    /**
     * @english
     * Various flags.
     * @endenglish
     *
     * @japanese
     * æ§ããªãã©ã?     * @endjapanese
     */
    struct
    {
        /**
         * @english
         * 1 is set if a task pre event dispatcher is defined.
         *
         * If Bff_Task_Driver.getPreHandlers() is not empty and
         * its call has succeeded, 1 is set to this data field.
         * @endenglish
         *
         * @japanese
         * ã¿ã¹ã¯ã»ãã¬ã»ã¤ãã³ããã£ã¹ãããã£ã¼ãå®ç¾©ããã¦ããã° 1
         * ãã»ããããã¾ãã?         *
         * Bff_Task_Driver.getPreHandlers() ãç©ºã§ã¯ãªãããã¤ããã?!--
         * -->å¼åºããæåããå ´åããã®ãã¼ã¿ãã£ã¼ã«ãã« 1 ãã»ãã<!--
         * -->ããã¾ãã?         * @endjapanese
         *
         * @see Bff_Task_Driver.getPreHandlers()
         */
        unsigned int hasPreDispatcher:1;

        /**
         * @english
         * 1 is set if a task post event dispatcher is defined.
         *
         * If Bff_Task_Driver.getPostHandlers() is not empty and
         * its call has succeeded, 1 is set to this data field.
         * @endenglish
         *
         * @japanese
         * ã¿ã¹ã¯ã»ãã¹ãã»ã¤ãã³ããã£ã¹ãããã£ã¼ãå®ç¾©ããã¦ããã?1
         * ãã»ããããã¾ãã?         *
         * Bff_Task_Driver.getPostHandlers() ãç©ºã§ã¯ãªãããã¤ããã?!--
         * -->å¼åºããæåããå ´åããã®ãã¼ã¿ãã£ã¼ã«ãã« 1 ãã»ãã<!--
         * -->ããã¾ãã?         * @endjapanese
         *
         * @see Bff_Task_Driver.getPostHandlers()
         */
        unsigned int hasPostDispatcher:1;

        /**
         * @english
         * 1 is set if Bff_Task_Driver.finalize() needs to be called.
         * @endenglish
         *
         * @japanese
         * Bff_Task_Driver.finalize() ãã³ã¼ã«ããå¿è¦ãããå ´åã?
         * ãã»ããããã¾ãã?         * @endjapanese
         *
         * @see Bff_Task_Driver.finalize()
         */
        unsigned int needsFinalizerCall:1;

        /**
         * @english
         * 1 is set if this task is suspended.
         * @endenglish
         *
         * @japanese
         * ãã®ã¿ã¹ã¯ããµã¹ãã³ãããã¦ããå ´åã? ãã»ããããã¾ãã?         * @endjapanese
         *
         * @see Bff_Task.isSuspended()
         */
        unsigned int suspended:1;

        /**
         * @english
         * 1 is set if this task has a scheduled scene.
         * @endenglish
         *
         * @japanese
         * ãã®ã¿ã¹ã¯ããã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ããä¿æãã¦ããã?         * 1 ãã»ããããã¾ãã?         * @endjapanese
         */
        unsigned int hasScheduledScene:1;

        /**
         * @english
         * 1 is set while this task does not accept a request to schedule
         * a scene. The following methods will fail while this data field
         * is 1.
         *
         *   - Bff_Task.scheduleNewScene()
         *   - Bff_Task.scheduleOneShotScene()
         *   - Bff_Task.scheduleScene()
         *   - Bff_Task.scheduleSceneById()
         *   - Bff_Task.scheduleSceneByName()
         * @endenglish
         *
         * @japanese
         * ãã®ã¿ã¹ã¯ããã·ã¼ã³ã®ã¹ã±ã¸ã¥ã¼ãªã³ã°ãåãä»ããªãå ´åã?         * 1 ãã»ããããã¾ãã?         * ãã®ãã¼ã¿ãã£ã¼ã«ãã?1 ã®éã¯ãä¸è¨ã®ã¡ã½ããå¼åºãã¯<!--
         * -->å¤±æãã¾ãã?         *
         *   - Bff_Task.scheduleNewScene()
         *   - Bff_Task.scheduleOneShotScene()
         *   - Bff_Task.scheduleScene()
         *   - Bff_Task.scheduleSceneById()
         *   - Bff_Task.scheduleSceneByName()
         * @endjapanese
         */
        unsigned int schedulingBlocked:1;

        /**
         * @english
         * 1 is set while this task does not perform scene switching.
         * The value of this data field can be changed by
         * Bff_Task.blockSceneSwitching().
         * @endenglish
         *
         * @japanese
         * ãã®ã¿ã¹ã¯ãã·ã¼ã³åãæ¿ããè¡ããªãå ´åã? ãã»ããããã¾ãã?         * ãã®ãã¼ã¿ãã£ã¼ã«ãã®å¤ã¯ã?Bff_Task.blockSceneSwitching()
         * ã§å¤æ´ãããã¨ãã§ãã¾ãã?         * @endjapanese
         *
         * @see Bff_Task.blockSceneSwitching()
         */
        unsigned int switchingBlocked:1;
    }
    flags;

    /**
     * @english
     * Task-specific data.
     *
     * This data field is initialized by NULL. Bff_Task.setData()
     * can set any data to this data field and Bff_Task.getData()
     * can retrieve the value.
     *
     * BFF does not care about how this data field is used by BFF
     * applications.
     * @endenglish
     *
     * @japanese
     * ã¿ã¹ã¯åºæãã¼ã¿
     *
     * ãã®ãã¼ã¿ãã£ã¼ã«ãã?NULL ã§åæåããã¾ãã?Bff_Task.setData()
     * ã§ãã®ãã¼ã¿ãã£ã¼ã«ãã«ä»»æã®å¤ãè¨­å®ã§ãã?Bff_Task.getData()
     * ã§ãã®å¤ãåãåºããã¨ãã§ãã¾ãã?     *
     * BFF ã¯ãBFF ã¢ããªã±ã¼ã·ã§ã³ããã®ãã¼ã¿ãã£ã¼ã«ããã©ã®ããã?!--
     * -->ä½¿ç¨ãããã«ã¤ãã¦ã¯é¢ç¥ãã¾ããã?     * @endjapanese
     *
     * @see Bff_Task.getData()
     * @see Bff_Task.setData()
     */
    void *data;

    /**
     * @english
     * The previous task.
     *
     * BffIm_Task implementation does not care about how this field
     * is used.
     * @endenglish
     *
     * @japanese
     * åã®ã¿ã¹ã?     *
     * BffIm_Task å®è£ã¯ããã®ãã£ã¼ã«ããã©ã®ããã«ä½¿ããããã«<!--
     * -->ã¤ãã¦ã¯é¢ç¥ãã¾ããã?     * @endjapanese
     */
    BffIm_Task *prev;

    /**
     * @english
     * The next task.
     *
     * BffIm_Task implementation does not care about how this field
     * is used.
     * @endenglish
     *
     * @japanese
     * æ¬¡ã®ã¿ã¹ã?     *
     * BffIm_Task å®è£ã¯ããã®ãã£ã¼ã«ããã©ã®ããã«ä½¿ããããã«<!--
     * -->ã¤ãã¦ã¯é¢ç¥ãã¾ããã?     * @endjapanese
     */
    BffIm_Task *next;

    /**
     * @english
     * The event nest level.
     *
     * This value is incremented by the BFF Kernel before handleEvent()
     * is called and decremented after handleEvent() returns.
     * @endenglish
     *
     * @japanese
     * ã¤ãã³ããã¹ãã¬ãã«
     *
     * ãã®å¤ã¯ã?handleEvent() ãã³ã¼ã«ãããåã?BFF ã«ã¼ãã«<!--
     * -->ã«ããã¤ã³ã¯ãªã¡ã³ãããã?handleEvent() çµäºå¾ã«<!--
     * -->ãã¯ãªã¡ã³ãããã¾ãã?     * @endjapanese
     */
    uint16 eventNestLevel;

    /**
     * @english
     * Task name.
     *
     * This data field is initialized like below.
     *
     * @code
     * SNPRINTF(, , #BFFIM_TASK_NAME_FORMAT, taskId)
     * @endcode
     *
     * Bff_Task_Driver.getName() can overwrite the initial value.
     * @endenglish
     *
     * @japanese
     * ã¿ã¹ã¯å
     *
     * ãã®ãã¼ã¿ãã£ã¼ã«ãã¯æ¬¡ã®ããã«åæåããã¾ãã?     *
     * @code
     * SNPRINTF(, , #BFFIM_TASK_NAME_FORMAT, taskId)
     * @endcode
     *
     * Bff_Task_Driver.getName() ã¯ãã®åæå¤ãä¸æ¸ãã§ãã¾ãã?     * @endjapanese
     *
     * @see Bff_Task.getName()
     * @see Bff_Task_Driver.getName()
     */
    char name[Bff_Task_NAME_BUF_SIZE];

    /**
     * @english
     * Task driver.
     * @endenglish
     *
     * @japanese
     * ã¿ã¹ã¯ãã©ã¤ãã¼
     * @endjapanese
     *
     * @see Bff_Applet_Driver.getTaskDriver()
     */
    Bff_Task_Driver driver;

    /**
     * @english
     * Pre Event Dispatcher of this task.
     *
     * BREW events are passed to this pre dispatcher before they are
     * passed to a event dispatcher of the current scene.
     * @endenglish
     *
     * @japanese
     * ãã®ã¿ã¹ã¯ã®ãã¬ã»ã¤ãã³ããã£ã¹ãããã£ã?     *
     * BREW ã¤ãã³ãã¯ãç¾å¨ã®ã·ã¼ã³ã®ã¤ãã³ããã£ã¹ãããã£ã¼ã«æ¸¡ããã?!--
     * -->åã«ãã®ãã¬ã»ãã£ã¹ãããã£ã¼ã«æ¸¡ããã¾ãã?     * @endjapanese
     *
     * @see Bff_Task_Driver.getPreHandlers()
     */
    Bff_Dispatcher preDispatcher;

    /**
     * @english
     * Post Event Dispatcher of this task.
     *
     * BREW events are passed this post dispatcher as the last step in
     * the event handler of this task (BffIm_Task.handleEvent()).
     * @endenglish
     *
     * @japanese
     * ãã®ã¿ã¹ã¯ã®ãã¹ãã»ã¤ãã³ããã£ã¹ãããã£ã¼
     *
     * ãã®ã¿ã¹ã¯ã®ã¤ãã³ããã³ãã©ã?(BffIm_Task.handleEvent())
     * ã®æå¾ã®å¦çã¨ãã¦ããã®ãã¹ãã»ãã£ã¹ãããã£ã¼ã« BREW
     * ã¤ãã³ããæ¸¡ããã¾ãã?     * @endjapanese
     *
     * @see Bff_Task_Driver.getPostHandlers()
     */
    Bff_Dispatcher postDispatcher;

    /*--------------------*
     * Scene
     *--------------------*/

    /**
     * @english
     * The current scene.
     * @endenglish
     *
     * @japanese
     * ç¾å¨ã®ã·ã¼ã³
     * @endjapanese
     */
    BffIm_Scene *currentScene;

    /**
     * @english
     * A scheduled scene.
     * @endenglish
     *
     * @japanese
     * ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³
     * @endjapanese
     */
    BffIm_Scene *scheduledScene;

    /**
     * @english
     * An argument for the scheduled scene.
     * @endenglish
     *
     * @japanese
     * ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ã®å¼æ?     * @endjapanese
     */
    void *scheduledSceneArg;

    /**
     * @english
     * The event nest level when a scene was scheduled.
     *
     * Switching scenes is not performed if the current event nest level
     * is deeper (bigger) than the value held by this data field. This
     * prevents synchronous events from triggering scene switching.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãããã¨ãã®ã¤ãã³ããã¹ãã¬ãã«
     *
     * ç¾å¨ã®ã¤ãã³ããã¹ãã¬ãã«ããã®ãã¼ã¿ãã£ã¼ã«ãã«ãã£ã¦ä¿æ<!--
     * -->ããã¦ããå¤ãããæ·±ã (å¤§ãã? å ´åãã·ã¼ã³åãæ¿ãã?!--
     * -->å®è¡ããã¾ãããããã«ãããåæã¤ãã³ããã·ã¼ã³åãæ¿ãã<!--
     * -->å¼ãèµ·ãããã¨ãé²æ­¢ãã¾ãã?     * @endjapanese
     */
    uint16 scheduledSceneEventNestLevel;
};


/*------------------------------------------------------------------------*
 * FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @english
 * Initialize a task.
 *
 * @param[out] instance  A task to initialize.
 * @param[in]  kernel    The BFF Kernel.
 * @param[in]  taskId    A task ID to be assigned to the task.
 * @param[in]  driver    A task driver for the task.
 * @param[in]  flags     The first parameter of an EVT_APP_START event or
 *                       an EVT_APP_RESUME event.
 * @param[in]  appStart  The second parameter of an EVT_APP_START event or
 *                       an EVT_APP_RESUME event.
 *
 * @retval SUCCESS   Successfully initialized.
 * @retval EBADPARM  'instance', 'kernel', and/or 'driver' are null.
 * @endenglish
 *
 * @japanese
 * ã¿ã¹ã¯ãåæåãã¾ãã? *
 * @param[out] instance  åæåããã¿ã¹ã¯
 * @param[in]  kernel    BFF ã«ã¼ãã«
 * @param[in]  taskId    ã¿ã¹ã¯ã«å²ãå½ã¦ãã¿ã¹ã¯ ID
 * @param[in]  driver    åæåããã¿ã¹ã¯ç¨ã®ã¿ã¹ã¯ãã©ã¤ãã¼
 * @param[in]  flags     EVT_APP_START ã¾ãã?EVT_APP_RESUME ã¤ãã³ãã?!--
 *                       -->ç¬¬ä¸ãã©ã¡ã¼ã? * @param[in]  appStart  EVT_APP_START ã¾ãã?EVT_APP_RESUME ã¤ãã³ãã?!--
 *                       -->ç¬¬äºãã©ã¡ã¼ã? *
 * @retval SUCCESS   åæåã«æåãã¾ããã? * @retval EBADPARM  instance, kernel, driver ã®ãããããããã¯è¤æ°ã<!--
 *                   -->ãã«ãã¤ã³ã¿ã§ãã? * @endjapanese
 */
int
BffIm_Task_initialize(
    BffIm_Task *instance, BffIm_Kernel *kernel, Bff_Task_Id taskId,
    Bff_Task_Driver *driver, uint16 flags, AEEAppStart *appStart);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BFFIM_TASK_H */
