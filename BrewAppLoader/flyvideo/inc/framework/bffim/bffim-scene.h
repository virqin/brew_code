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
 * BREW Foundation Framework, Scene Implementation.
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, ã·ã¼ã³å®è£? * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFFIM_SCENE_H
#define BFFIM_SCENE_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEStdLib.h"
#include "bff/bff-dispatcher.h"
#include "bff/bff-scene.h"
#include "bff/bff-scene-driver.h"
#include "bffim/bffim-types.h"


/*------------------------------------------------------------------------*
 * MACROS
 *------------------------------------------------------------------------*/
/**
 * @english
 * Format to be used to generate the initial value of scene name.
 * @endenglish
 *
 * @japanese
 * ã·ã¼ã³åã®åæå¤ãçæããã®ã«ä½¿ããã©ã¼ãããæå­å? * @endjapanese
 */
#define BFFIM_SCENE_NAME_FORMAT  "Scene%ld"


/*------------------------------------------------------------------------*
 * STRUCT DECLARATIONS
 *------------------------------------------------------------------------*/
/*
struct BffIm_Scene;
typedef struct BffIm_Scene BffIm_Scene;*/



/*------------------------------------------------------------------------*
 * STRUCTS
 *------------------------------------------------------------------------*/
/**
 * @english
 * BFF Scene Implementation
 *
 * BffIm_Scene extends Bff_Scene as shown below.
 *
 * @code
 *       Bff_Scene            BffIm_Scene
 *   +---------------+ - - +---------------+
 *   |               |     |               |
 *   |               |     |   Bff_Scene   |
 *   |               |     |               |
 *   +---------------+ - - + - - - - - - - +
 *                         |               |
 *                         |               |
 *                         |               |
 *                         +---------------+
 * @endcode
 *
 * Bff_Scene contains interfaces that are open to BFF applications.
 * On the other hand, BffIm_Scene contains implementation specific
 * methods and data fields that should not be touched directly by
 * BFF applications.
 * @endenglish
 *
 * @japanese
 * BFF ã·ã¼ã³å®è£? *
 * BffIm_Scene ã¯ãä¸å³ãç¤ºãããã«ã?Bff_Scene ãæ¡å¼µãã¾ãã? *
 * @code
 *       Bff_Scene            BffIm_Scene
 *   +---------------+ - - +---------------+
 *   |               |     |               |
 *   |               |     |   Bff_Scene   |
 *   |               |     |               |
 *   +---------------+ - - + - - - - - - - +
 *                         |               |
 *                         |               |
 *                         |               |
 *                         +---------------+
 * @endcode
 *
 * Bff_Scene ã¯ãBFF ã¢ããªã±ã¼ã·ã§ã³ã«å¬éãã¦ããã¤ã³ã¿ã¼ãã§ã¼ã¹ã<!--
 * -->å«ãã§ãã¾ããä¸æ¹ã?BffIm_Scene ã¯å®è£ã«ç¹åããã¡ã½ããã?!--
 * -->ãã¼ã¿ãã£ã¼ã«ããå«ãã§ããããããã?BFF ã¢ããªã±ã¼ã·ã§ã³ã?!--
 * -->ç´æ¥æä½ãã¹ãã§ã¯ããã¾ããã? * @endjapanese
 */
typedef struct _BffIm_Scene
{
    /**
     * @english
     * Public interface of a scene.
     *
     * This data field must be located at the top of this structure
     * so that this structure can be cast by Bff_Scene.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³ã®å¬éã¤ã³ã¿ã¼ãã§ã¼ã¹
     *
     * ãã®æ§é ä½ã?Bff_Scene ã§ã­ã£ã¹ãã§ããããããã®ãã¼ã¿<!--
     * -->ãã£ã¼ã«ãã¯ãã®æ§é ä½ã®åé ­ã«ãªããã°ããã¾ããã?     * @endjapanese
     */
    Bff_Scene scene;


    /*-------------------------------------------*
     * Methods
     *-------------------------------------------*/

    /**
     * @english
     * Finalize this scene.
     *
     * @param[in] self  A scene implementation
     * @endenglish
     *
     * @japanese
     * ãã®ã·ã¼ã³ã®çµäºå¦çããããªãã?     *
     * @param[in] self  ã·ã¼ã³å®è£?     * @endjapanese
     */
    void (*finalize)(BffIm_Scene *self);

    /**
     * @english
     * Handle a BREW event.
     *
     * @param[in]  self      A scene implementation
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
     * @param[in]  self      ã·ã¼ã³å®è£?     * @param[in]  ev        ã¤ãã³ãã³ã¼ã?     * @param[in]  w         ã¤ãã³ãã®ç¬¬ä¸ãã©ã¡ã¼ã?     * @param[in]  dw        ã¤ãã³ãã®ç¬¬äºãã©ã¡ã¼ã¿
     * @param[out] consumed  ã¤ãã³ãããã®ã¡ã½ããã«ããæ¶è²»ãããå ´åã¯
     *                       TRUE ããããã§ãªãå ´åã?FALSE
     *                       ãæ¸ãæ»ããã¾ãã?     *
     * @return  BREW AEE ã«è¿ãã¹ãæ»ãå¤ãæ»ãå¤ã®å¤ã¨æå³ã?ev
     *          ã®å¤ã«ãã£ã¦ç°ãªãã¾ãã?     * @endjapanese
     */
    boolean (*handleEvent)(
        BffIm_Scene *self, AEEEvent *ev, uint16 *w, uint32 *dw,
        boolean *consumed);


    /*-------------------------------------------*
     * Data fields
     *-------------------------------------------*/

    /**
     * @english
     * Scene ID of this scene.
     * @endenglish
     *
     * @japanese
     * ãã®ã·ã¼ã³ã®ã·ã¼ã?ID
     * @endjapanese
     *
     * @see Bff_Scene.getId()
     */
    Bff_Scene_Id id;

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
         * 1 is set if a scene event dispatcher is defined.
         *
         * If Bff_Scene_Driver.getHandlers() is not empty and its call
         * has succeeded, 1 is set to this data field.
         * @endenglish
         *
         * @japanese
         * ã·ã¼ã³ã¤ãã³ããã£ã¹ãããã£ã¼ãå®ç¾©ããã¦ããã° 1
         * ãã»ããããã¾ãã?         *
         * Bff_Scene_Driver.getHandlers() ãç©ºã§ã¯ãªãããã¤ããã?!--
         * -->å¼åºããæåããå ´åããã®ãã¼ã¿ãã£ã¼ã«ãã« 1 ãã»ãã<!--
         * -->ããã¾ãã?         * @endjapanese
         *
         * @see Bff_Scene_Driver.getHandlers()
         */
        unsigned int hasDispatcher:1;

        /**
         * @english
         * 1 is set if Bff_Scene_Driver.finalize() needs to be called.
         * @endenglish
         *
         * @japanese
         * Bff_Scene_Driver.finalize() ãã³ã¼ã«ããå¿è¦ãããå ´åã?
         * ãã»ããããã¾ãã?         * @endjapanese
         *
         * @see Bff_Scene_Driver.finalize()
         */
        unsigned int needsFinalizerCall:1;

        /**
         * @english
         * 1 is set if this scene instance should be released when this
         * scene is demoted from the current scene (= when another
         * different scene becomes the current scene by kicking out this
         * scene) or when this scene is cancelled (= when another
         * different scene is scheduled while this scene is waiting as
         * a scheduled scene).
         * @endenglish
         *
         * @japanese
         * ãã®ã·ã¼ã³ãç¾å¨ã®ã·ã¼ã³ããéæ ¼ãããã¨ãã« (=ä»ã®ç°ãªã?!--
         * -->ã·ã¼ã³ããã®ã·ã¼ã³ãè¿½ãåºãã¦ç¾å¨ã®ã·ã¼ã³ã«ãªãã¨ãã?ã?!--
         * -->ãããã¯ããã®ã·ã¼ã³ãã­ã£ã³ã»ã«ãããã¨ãã« (=ãã®ã·ã¼ã³ã<!--
         * -->ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ã¨ãã¦å¾æ©ãã¦ããéã«ãä»ã®ç°ãªã<!--
         * -->ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãããã¨ãã«) ãã®ã·ã¼ã³ãè§£æ¾ããå¿è¦<!--
         * -->ãããã° 1 ãã»ããããã¾ãã?         * @endjapanese
         *
         * @see Bff_Task.scheduleOneShotScene()
         */
        unsigned int releaseOnLeave:1;

        /**
         * @english
         * 1 is set if this scene is a current scene of a task.
         * @endenglish
         *
         * @japanese
         * ãã®ã·ã¼ã³ãä½ããã®ã¿ã¹ã¯ã®ãç¾å¨ã®ã·ã¼ã³ãã§ããã?1
         * ãã»ããããã¾ãã?         * @endjapanese
         *
         * @see Bff_Scene.isCurrent()
         */
        unsigned int current:1;

        /**
         * @english
         * 1 is set if this scene is a scheduled scene of a task.
         * @endenglish
         *
         * @japanese
         * ãã®ã·ã¼ã³ãä½ããã®ã¿ã¹ã¯ã®ãã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ãã§ããã?         * 1 ãã»ããããã¾ãã?         * @endjapanese
         *
         * @see Bff_Scene.isScheduled()
         */
        unsigned int scheduled:1;
    }
    flags;

    /**
     * @english
     * Scene-specific data.
     *
     * This data field is initialized by NULL. Bff_Scene.setData()
     * can set any data to this data field and Bff_Scene.getData()
     * can retrieve the value.
     *
     * BFF does not care about how this data field is used by BFF
     * applications.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³åºæãã¼ã¿
     *
     * ãã®ãã¼ã¿ãã£ã¼ã«ãã?NULL ã§åæåããã¾ãã?Bff_Scene.setData()
     * ã§ãã®ãã¼ã¿ãã£ã¼ã«ãã«ä»»æã®å¤ãè¨­å®ã§ãã?Bff_Scene.getData()
     * ã§ãã®å¤ãåãåºããã¨ãã§ãã¾ãã?     *
     * BFF ã¯ãBFF ã¢ããªã±ã¼ã·ã§ã³ããã®ãã¼ã¿ãã£ã¼ã«ããã©ã®ããã?!--
     * -->ä½¿ç¨ãããã«ã¤ãã¦ã¯é¢ç¥ãã¾ããã?     * @endjapanese
     *
     * @see Bff_Scene.getData()
     * @see Bff_Scene.setData()
     */
    void *data;

    /**
     * @english
     * The previous scene in the scene list.
     *
     * BffIm_Scene implementation does not care about how this field
     * is used.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³ãªã¹ãåã«ããããåã®ã·ã¼ã³
     *
     * BffIm_Scene å®è£ã¯ããã®ãã£ã¼ã«ããã©ã®ããã«ä½¿ããããã«<!--
     * -->ã¤ãã¦ã¯é¢ç¥ãã¾ããã?     * @endjapanese
     */
    BffIm_Scene *prev;

    /**
     * @english
     * The next scene in the scene list.
     *
     * BffIm_Scene implementation does not care about how this field
     * is used.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³ãªã¹ãåã«ããããæ¬¡ã®ã·ã¼ã³
     *
     * BffIm_Scene å®è£ã¯ããã®ãã£ã¼ã«ããã©ã®ããã«ä½¿ããããã«<!--
     * -->ã¤ãã¦ã¯é¢ç¥ãã¾ããã?     * @endjapanese
     */
    BffIm_Scene *next;

    /**
     * @english
     * The task that this scene is currently associated with.
     *
     * If this scene is a current scene or a scheduled scene of a task,
     * this data field holds a pointer to the task. Otherwise, a null
     * pointer is set to this data field.
     * @endenglish
     *
     * @japanese
     * ãã®ã·ã¼ã³ãç¾å¨é¢é£ä»ãããã¦ããã¿ã¹ã?     *
     * ãã®ã·ã¼ã³ãä½ããã®ã¿ã¹ã¯ã®ãç¾å¨ã®ã·ã¼ã³ããããã?!--
     * -->ãã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ãã§ããå ´åããã®ãã¼ã¿<!--
     * -->ãã£ã¼ã«ãã¯ããã®ã¿ã¹ã¯ã¸ã®ãã¤ã³ã¿ãä¿æãã¾ãã?!--
     * -->ããä»¥å¤ã®å ´åã¯ããã®ãã¼ã¿ãã£ã¼ã«ãã«ã¯ãã«ãã¤ã³ã¿ã<!--
     * -->è¨­å®ããã¾ãã?     * @endjapanese
     */
    BffIm_Task *task;

    /**
     * @english
     * Scene name.
     *
     * This data field is initialized like below.
     *
     * @code
     * SNPRINTF(, , BFFIM_SCENE_NAME_FORMAT, sceneId)
     * @endcode
     *
     * Bff_Scene_Driver.getName() can overwrite the initial value.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³å
     *
     * ãã®ãã¼ã¿ãã£ã¼ã«ãã¯æ¬¡ã®ããã«åæåããã¾ãã?     *
     * @code
     * SNPRINTF(, , BFFIM_SCENE_NAME_FORMAT, sceneId)
     * @endcode
     *
     * Bff_Scene_Driver.getName() ã¯ãã®åæå¤ãä¸æ¸ãã§ãã¾ãã?     * @endjapanese
     *
     * @see Bff_Scene.getName()
     * @see Bff_Scene_Driver.getName()
     */
    char name[Bff_Scene_NAME_BUF_SIZE];

    /**
     * @english
     * Driver of this scene.
     * @endenglish
     *
     * @japanese
     * ãã®ã·ã¼ã³ã®ãã©ã¤ãã?     * @endjapanese
     */
    Bff_Scene_Driver driver;

    /**
     * @english
     * Event dispatcher of this scene.
     * @endenglish
     *
     * @japanese
     * ãã®ã·ã¼ã³ã®ã¤ãã³ããã£ã¹ãããã£ã¼
     * @endjapanese
     *
     * @see Bff_Scene_Driver.getHandlers()
     */
    Bff_Dispatcher dispatcher;
};


/*------------------------------------------------------------------------*
 * FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @english
 * Initialize a scene
 *
 * @param[out] instance  A scene to initialize.
 * @param[in]  kernel    The BFF Kernel.
 * @param[in]  sceneId   A scene ID to be assigned to the scene.
 * @param[in]  driver    A scene driver for the scene.
 *
 * @retval SUCCESS   Successfully initialized.
 * @retval EBADPARM  'instance', 'kernel' and/or 'driver' are null.
 * @endenglish
 *
 * @japanese
 * ã·ã¼ã³ãåæåãã¾ãã? *
 * @param[out] instance  åæåããã·ã¼ã³
 * @param[in]  kernel    BFF ã«ã¼ãã«
 * @param[in]  sceneId   åæåããã·ã¼ã³ã«å²ãå½ã¦ãã·ã¼ã?ID
 * @param[in]  driver    åæåããã·ã¼ã³ç¨ã®ã·ã¼ã³ãã©ã¤ãã¼
 *
 * @retval SUCCESS   åæåã«æåãã¾ããã? * @retval EBADPARM  instance, kernel, driver ã®ãããããããã?!--
 *                   -->è¤æ°ããã«ãã¤ã³ã¿ã§ãã? * @endjapanese
 */
int
BffIm_Scene_initialize(
    BffIm_Scene *instance, BffIm_Kernel *kernel, Bff_Scene_Id sceneId,
    Bff_Scene_Driver *driver);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BFFIM_SCENE_H */
