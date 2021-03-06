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
 * BREW Foundation Framework, Scene.
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, ã·ã¼ã? * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 * @see @ref scene_switching
 * @see Bff_Scene_Driver
 */


#ifndef BFF_SCENE_H
#define BFF_SCENE_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEStdLib.h"
#include "bff/bff-types.h"
#include "bff/bff-var.h"


/*------------------------------------------------------------------------*
 * ENUMS
 *------------------------------------------------------------------------*/
enum
{
    /**
     * @english
     * Size of buffer for scene name.
     *
     * This value is given to Bff_Scene_Driver.getName() as the size
     * of the buffer where the scene name should be set.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³åç¨ãããã¡ã®ãµã¤ã?     *
     * ãã®å¤ã¯ãã·ã¼ã³åãæ ¼ç´ãããããã¡ã®ãµã¤ãºã¨ãã¦ã?     * Bff_Scene_Driver.getName() ã«æ¸¡ããã¾ãã?     * @endjapanese
     *
     * @see Bff_Scene_Driver.getName()
     */
    Bff_Scene_NAME_BUF_SIZE = 32
};


/*------------------------------------------------------------------------*
 * STRUCT DECLARATIONS
 *------------------------------------------------------------------------*/
/*
struct Bff_Scene;
typedef struct Bff_Scene Bff_Scene;*/



/*------------------------------------------------------------------------*
 * STRUCTS
 *------------------------------------------------------------------------*/
/**
 * @english
 * BFF Scene
 *
 * A BFF Scene intends to be mapped to an application state. A BFF Scene
 * can have one set of event handlers. This means that one application state
 * can have its own event handlers. This mechanism naturally encourages
 * BFF application programmers to adopt state-pattern programming style.
 *
 * A BFF Scene can be created by Bff.createScene(). An ID and a name are
 * automatically assigned to a new BFF Scene by BFF. The ID is unique and
 * cannot be changed, but the name can be overwritten by implementing
 * Bff_Scene_Driver.getName(). BFF Scenes are allowed to have the same
 * name. BFF does not check name duplication.
 *
 * To make a BFF Scene start working, the BFF Scene needs to be associated
 * with a BFF Task. To associate a BFF Scene with a BFF Task, the following
 * methods can be used.
 *
 *   - Bff_Task.scheduleScene()
 *   - Bff_Task.scheduleSceneById()
 *   - Bff_Task.scheduleSceneByName()
 *
 * These methods accept a BFF Scene as a "scheduled" scene. A scheduled
 * scene is promoted to a "current" scene later. It is when a BFF scene
 * becomes a current scene that the BFF scene starts working.
 *
 * BFF releases all existing BFF Scenes during its finalization process,
 * but it is recommended to release BFF Scenes by Bff.releaseScene() if
 * they are no longer used in order to save RAM.
 *
 * Bff_Task.scheduleNewScene() is a kind of a shortcut method which does
 * "create a BFF scene" and "schedule it". Bff_Task.scheduleOneShotScene()
 * also does the same thing as Bff_Task.scheduleNewScene() but additionally
 * does "release it when the scene finishes its job".
 *
 * When a BFF Scene is scheduled, if another BFF Scene is already held
 * by the BFF Task as a scheduled scene, the existing scheduled scene is
 * cancelled. If the scene driver of the cancelled scene has an
 * implementation of Bff_Scene_Driver.cancel(), the %cancel() method is
 * called. An argument specified when the scene was scheduled is passed
 * to Bff_Scene_Driver.cancel().
 *
 * Every time a BFF Scene is promoted to a current scene from a scheduled
 * scene, Bff_Scene_Driver.enter() is called if defined. An argument
 * specified when the scene was scheduled is passed to
 * Bff_Scene_Driver.enter().
 *
 * When another BFF scene is promoted to a current scene and kicks out
 * a running current scene, Bff_Scene_Driver.leave() of the kicked-out
 * scene is called. If the kicked-out scene has been scheduled by
 * Bff_Task.scheduleOneShotScene(), the scene is released after the
 * call of Bff_Scene_Driver.leave() is finished.
 *
 * If defined, Bff_Scene_Driver.finalize() is called when a BFF Scene
 * is released.
 * @endenglish
 *
 * @japanese
 * BFF ã·ã¼ã? *
 * BFF ã·ã¼ã³ã¯ãã¢ããªã±ã¼ã·ã§ã³ã®ç¶æã®ä¸ã¤ã«å¯¾å¿ä»ãããããã¨ã<!--
 * -->æå³ãã¦ãã¾ããBFF ã·ã¼ã³ã«ã¯ãã¤ãã³ããã³ãã©ã¼ç¾¤ã®ã»ãããä¸ã?!--
 * -->æããããã¨ãã§ãã¾ããããã¯ãä¸ã¤ã®ã¢ããªã±ã¼ã·ã§ã³ç¶ææ¯ã«<!--
 * -->ããåºæã®ã¤ãã³ããã³ãã©ã¼ç¾¤ãæããããã¨ãã§ããã¨ãããã¨ã<!--
 * -->æå³ãã¾ãããã®ä»çµã¿ã«ãããBFF ã¢ããªã±ã¼ã·ã§ã³ãã­ã°ã©ãã¯ã?!--
 * -->èªç¶ã¨ã¹ãã¼ããã¿ã¼ã³ã»ãã­ã°ã©ãã³ã°ãè¡ãããã«ãªãã¾ãã? *
 * BFF ã·ã¼ã³ã¯ã?Bff.createScene() ã«ããä½æã§ãã¾ããBFF ã«ããã?!--
 * -->ID ã¨ååããæ°ãã BFF ã·ã¼ã³ã«èªåçã«å²ãå½ã¦ããã¾ãã?!--
 * -->ID ã¯ä¸æã§ãããå¤æ´ã§ãã¾ããããååã¯ã? * Bff_Scene_Driver.getName() ãå®è£ãããã¨ã«ãããä¸æ¸ããããã¨ã?!--
 * -->ã§ãã¾ããè¤æ°ã® BFF ã·ã¼ã³ããåãååãæã¤ãã¨ã¯è¨±ããã?!--
 * -->ãã¾ããBFF ã¯ååã®éè¤ããã§ãã¯ãã¾ããã? *
 * BFF ã·ã¼ã³ãåããããã«ã¯ãã¿ã¹ã¯ã¨é¢é£ä»ããå¿è¦ãããã¾ããBFF
 * ã·ã¼ã³ã BFF ã¿ã¹ã¯ã¨é¢é£ä»ããã®ã«ã¯ãä¸è¨ã®ã¡ã½ãããä½¿ãã¾ãã? *
 *   - Bff_Task.scheduleScene()
 *   - Bff_Task.scheduleSceneById()
 *   - Bff_Task.scheduleSceneByName()
 *
 * ãããã®ã¡ã½ããã¯ãBFF ã·ã¼ã³ããã¹ã±ã¸ã¥ã¼ã«ããããã·ã¼ã³ã¨ãã?!--
 * -->åãä»ãã¾ããã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ã¯ãå¾ã»ã©ãç¾å¨ã®ãã·ã¼ã³<!--
 * -->ã¸ã¨ææ ¼ããããã¾ããBFF ã·ã¼ã³ãåãå§ããã®ã¯ããã?BFF
 * ã·ã¼ã³ãç¾å¨ã®ã·ã¼ã³ã«ãªã£ãã¨ãã§ãã? *
 * BFF ã¯ãçµäºå¦çã®éç¨ã§ãå­å¨ãã¦ããå¨ã¦ã® BFF ã·ã¼ã³ãè§£æ¾<!--
 * -->ãã¾ãããRAM ãç¯ç´ããããã«ããä½¿ç¨ããªããªã£ãæç¹ã§ã? * Bff.releaseScene() ã«ãã?BFF ã·ã¼ã³ãè§£æ¾ãããã¨ããå§ããã¾ãã? *
 * Bff_Task.scheduleNewScene() ã¯ãã·ã§ã¼ãã«ããã¡ã½ããã§ã?!--
 * -->ãBFF ã·ã¼ã³ãä½æããããããã®ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãããã¨ãã<!--
 * -->ä½æ¥­ããããªãã¾ãã?Bff_Task.scheduleOneShotScene() ãåããã¨ã<!--
 * -->ãããªãã¾ãããè¿½å ã§ãä»äºãçµããã¨ãã«ãã®ã·ã¼ã³ãè§£æ¾ããã?!--
 * -->ã¨ããä½æ¥­ããããªãã¾ãã? *
 * BFF ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãããã¨ãããããä»ã® BFF ã·ã¼ã³ã<!--
 * -->æ¢ã«ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ã¨ãã¦å¯¾è±¡ã® BFF ã¿ã¹ã¯ã«ãã<!--
 * -->ä¿æããã¦ããå ´åããã®æ¢å­ã®ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ã?!--
 * -->ã­ã£ã³ã»ã«ããã¾ããã­ã£ã³ã»ã«ãããã·ã¼ã³ã®ã·ã¼ã³ãã©ã¤ãã¼ã
 * Bff_Scene_Driver.cancel() ã®å®è£ãæã£ã¦ããå ´åããã?%cancel()
 * ã¡ã½ãããå¼ã°ãã¾ããã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãããã¨ãã«æå®ããã<!--
 * -->å¼æ°ã?Bff_Scene_Driver.cancel() ã«æ¸¡ããã¾ãã? *
 * ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ãç¾å¨ã®ã·ã¼ã³ã¸ã¨ææ ¼ãããéã¯å¸¸ã«ã? * ããå®ç¾©ããã¦ããã°ã?Bff_Scene_Driver.enter() ãå¼ã°ãã¾ãã? * ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãããã¨ãã«æå®ãããå¼æ°ãã? * Bff_Scene_Driver.enter() ã«æ¸¡ããã¾ãã? *
 * ä»ã® BFF ã·ã¼ã³ãç¾å¨ã®ã·ã¼ã³ã¸ã¨ææ ¼ãããåãã¦ããç¾å¨ã?!--
 * -->ã·ã¼ã³ãè¿½ãåºãã¨ããè¿½ãåºãããã·ã¼ã³ã?Bff_Scene_Driver.leave()
 * ãå¼ã°ãã¾ãããããè¿½ãåºãããã·ã¼ã³ã Bff_Task.scheduleOneShotScene()
 * ã«ããã¹ã±ã¸ã¥ã¼ã«ããããã®ã§ããã°ã?Bff_Scene_Driver.leave()
 * ã®å¼åºããçµãã£ããã¨ã§ããã®ã·ã¼ã³ã¯è§£æ¾ããã¾ãã? *
 * ããå®ç¾©ããã¦ããã°ãã·ã¼ã³ãè§£æ¾ãããã¨ãã? * Bff_Scene_Driver.finalize() ãå¼ã°ãã¾ãã? * @endjapanese
 *
 * @see @ref scene_switching
 * @see Bff_Scene_Driver
 */
typedef struct _Bff_Scene
{
    /**
     * @english
     * Control the scene in scene-driver-specific way.
     *
     * Behaviors of this method depend on the implementation of
     * Bff_Scene_Driver.control().
     *
     * BFF Task also has a similar method, Bff_Task.control().
     *
     * @param[in]     self     A scene instance.
     * @param[in]     command  A command to this scene. Its value and
     *                         meaning are determined by the scene driver
     *                         implementation.
     * @param[in,out] args     An array of arguments for 'command'. Its
     *                         size, values and meaning are determined by
     *                         the scene driver implementation.
     *
     * @retval SUCCESS       The command was handled successfully.
     * @retval EBADPARM      'self' is null.
     * @retval EUNSUPPORTED  The command is not supported. This error
     *                       code is returned when
     *                       Bff_Scene_Driver.control() is not available
     *                       or the implementation of it returns this
     *                       error code.
     * @retval Others        Other error codes may be returned by the
     *                       scene driver implementation.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³ãã©ã¤ãã¼åºæã®æ¹æ³ã§ã·ã¼ã³ãå¶å¾¡ãã¾ãã?     *
     * ãã®ã¡ã½ããã®åä½ã¯ã?Bff_Scene_Driver.control()
     * ã®å®è£ã«ä¾å­ãã¾ãã?     *
     * BFF ã¿ã¹ã¯ã«ãã?Bff_Task.control() ã¨ããåæ§ã®ã¡ã½ãããããã¾ãã?     *
     * @param[in]     self     ã·ã¼ã³ã¤ã³ã¹ã¿ã³ã?     * @param[in]     command  ãã®ã·ã¼ã³ã«å¯¾ããã³ãã³ããå¤ããã³<!--
     *                         -->ãã®æå³ã¯ãã·ã¼ã³ãã©ã¤ãã¼ã®å®è£ã?!--
     *                         -->ããæ±ºå®ããã¾ãã?     * @param[in,out] args     command ã®å¼æ°ã®éåããµã¤ãºãå¤ã?!--
     *                         -->ããã³ãã®æå³ã¯ãã·ã¼ã³ãã©ã¤ãã?!--
     *                         -->ã®å®è£ã«ããæ±ºå®ããã¾ãã?     *
     * @retval SUCCESS       ã³ãã³ããæ­£å¸¸ã«å¦çããã¾ããã?     * @retval EBADPARM      self ããã«ãã¤ã³ã¿ã§ãã?     * @retval EUNSUPPORTED  ã³ãã³ãããµãã¼ãããã¦ãã¾ããããã?!--
     *                       -->ã¨ã©ã¼ã³ã¼ãã¯ã?Bff_Scene_Driver.control()
     *                       ãå®ç¾©ããã¦ããªãå ´åããããã¯ãå®è£ã<!--
     *                       -->ãã®ã¨ã©ã¼ã³ã¼ããè¿ãå ´åã«ãè¿ããã¾ãã?     * @retval ãã®ä»?       ã·ã¼ã³ãã©ã¤ãã¼ã®å®è£ã«ããããã®ä»ã?!--
     *                       -->ã¨ã©ã¼ã³ã¼ããè¿ããããã¨ãããã¾ãã?     * @endjapanese
     *
     * @see Bff_Scene_Driver.control()
     * @see Bff_Task.control()
     * @see Bff_Task_Driver.control()
     */
    int (*control)(Bff_Scene *self, uint32 command, Bff_Var *args);

    /**
     * @english
     * Get the scene-specific data.
     *
     * The data set by the last call of Bff_Scene.setData() is returned.
     * Note that BREW Foundation Framework does not care about what data
     * has been stored by Bff_Scene.setData().
     *
     * This is not a must, but it is expected that the application code
     * allocates a memory area for its own data structure (e.g. struct
     * MySceneData) and sets the pointer to the memory area by
     * Bff_Scene.setData() in the implementation of
     * Bff_Scene_Driver.initialize() (or somewhere else appropriate
     * for the application). The value of the pointer can be retrieved
     * later by Bff_Scene.getData(). Data stored in the data structure
     * can be treated as scene-wide data.
     *
     * There are similar methods, Bff.getData() and Bff_Task.getData(),
     * but note that their data scopes are different respectively.
     *
     * @param[in] self  A scene instance.
     *
     * @return  The scene-specific data. The data set by the last call of
     *          Bff_Scene.setData() is returned. If Bff_Scene.setData()
     *          has not been called before, NULL is returned. If 'self'
     *          is null, NULL is returned.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³åºæã®ãã¼ã¿ãåå¾ãã¾ãã?     *
     * æå¾ã® Bff_Scene.setData() ã®å¼åºãã§ã»ããããããã¼ã¿ãè¿ããã¾ãã?     * BREW Foundation Framework ã¯ã?Bff_Scene.setData() ãã©ããª<!--
     * -->ãã¼ã¿ãã»ãããã¦ãããã«ã¤ãã¦ã¯é¢ç¥ãã¾ããã?     *
     * å¿é ã¨ããããã§ã¯ããã¾ããããæ³å®ãããä½¿ç¨æ¹æ³ã¯ã?     * Bff_Scene_Driver.initialize() ã®å®è£åã?     * (ãããã¯ã¢ããªã±ã¼ã·ã§ã³ã«ã¨ã£ã¦é½åã®ããä»ã®å ´æã?
     * ç¬èªã®ãã¼ã¿æ§é ä½ (ä¾ãã?struct MySceneData) ç¨ã®ã¡ã¢ã?!--
     * -->é åãç¢ºä¿ãããã®ã¡ã¢ãªé åã¸ã®ãã¤ã³ã¿ã?Bff_Scene.setData()
     * ã§ã»ãããããã¨ã§ãããã®ãã¤ã³ã¿ã®å¤ã¯ãå¾ãã Bff_Scene.getData()
     * ã«ããåãåºããã¨ãã§ãã¾ããå½è©²ãã¼ã¿æ§é ä½ã¯ãã·ã¼ã³åä½ã?!--
     * -->ãã¼ã¿ã¨ãã¦æ±ããã¨ãã§ãã¾ãã?     *
     * Bff.getData(), Bff_Task.getData() ã¨ããä¼¼ãã¡ã½ãããããã¾ãã?!--
     * -->ããã¼ã¿ã®ã¹ã³ã¼ããããããç°ãªãã®ã§æ³¨æãã¦ãã ããã?     *
     * @param[in] self  ã·ã¼ã³ã¤ã³ã¹ã¿ã³ã?     *
     * @return  ã·ã¼ã³åºæãã¼ã¿ãæå¾ã® Bff_Scene.setData() ã«ãã?!--
     *          -->ã»ãããããå¤ãè¿ããã¾ãã?Bff_Scene.setData() ã?!--
     *          -->ããã¾ã§ã«å¼ã³åºããã¦ããªãå ´åããã«ãã¤ã³ã¿ã<!--
     *          -->è¿ããã¾ããself ããã«ãã¤ã³ã¿ã®å ´åããã«ãã¤ã³ã?!--
     *          -->ãè¿ããã¾ãã?     * @endjapanese
     *
     * @see Bff_Scene.setData()
     * @see Bff.getData()
     * @see Bff.setData()
     * @see Bff_Task.getData()
     * @see Bff_Task.setData()
     */
    void * (*getData)(Bff_Scene *self);

    /**
     * @english
     * Get the scene ID of this scene.
     *
     * @param[in] self  A scene instance.
     *
     * @return  The scene ID of this scene. #Bff_Scene_Id_INVALID is
     *          returned if 'self' is null.
     * @endenglish
     *
     * @japanese
     * ãã®ã·ã¼ã³ã®ã·ã¼ã?ID ãåå¾ãã¾ãã?     *
     * @param[in] self  ã·ã¼ã³ã¤ã³ã¹ã¿ã³ã?     *
     * @return  ãã®ã·ã¼ã³ã®ã·ã¼ã?IDãself ããã«ãã¤ã³ã¿ã®å ´åã?     *          #Bff_Scene_Id_INVALID ãè¿ããã¾ãã?     * @endjapanese
     */
    Bff_Scene_Id (*getId)(Bff_Scene *self);

    /**
     * @english
     * Get the name of this scene.
     *
     * Bff_Scene_Driver.getName() method can overwrite the default value
     * of scene name.
     *
     * @param[in] self  A scene instance.
     *
     * @return  The name of this scene. NULL is returned if 'self' is null.
     * @endenglish
     *
     * @japanese
     * ãã®ã·ã¼ã³ã®ååãåå¾ãã¾ãã?     *
     * Bff_Scene_Driver.getName() ã§ãã·ã¼ã³åã®ããã©ã«ãå¤ã<!--
     * -->ä¸æ¸ããããã¨ãã§ãã¾ãã?     *
     * @param[in] self  ã·ã¼ã³ã¤ã³ã¹ã¿ã³ã?     *
     * @return  ãã®ã·ã¼ã³ã®ååãself ããã«ãã¤ã³ã¿ã®å ´åã?!--
     *          -->ãã«ãã¤ã³ã¿ãè¿ããã¾ãã?     * @endjapanese
     *
     * @see Bff_Scene_Driver.getName()
     */
    const char * (*getName)(Bff_Scene *self);

    /**
     * @english
     * Get the task hosting this scene.
     *
     * @param[in] self  A scene instance.
     *
     * @return  If this scene is a current task or a scheduled task of
     *          a certain task, a pointer to the task is returned.
     *          Otherwise, a null pointer is returned. If 'self' is
     *          null, a null pointer is returned.
     * @endenglish
     *
     * @japanese
     * ãã®ã·ã¼ã³ãä¿æãã¦ããã¿ã¹ã¯ãåå¾ãã¾ãã?     *
     * @param[in] self  ã·ã¼ã³ã¤ã³ã¹ã¿ã³ã?     *
     * @return  ãã®ã·ã¼ã³ããä½ããã®ã¿ã¹ã¯ã®ç¾å¨ã®ã·ã¼ã³ãããã?!--
     *          -->ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ã§ããå ´åããã®ã¿ã¹ã¯ã?!--
     *          -->ãã¤ã³ã¿ãè¿ããã¾ããããä»¥å¤ã®å ´åã¯ããã?!--
     *          -->ãã¤ã³ã¿ãè¿ããã¾ããself ããã«ãã¤ã³ã¿ã®<!--
     *          -->å ´åã¯ããã«ãã¤ã³ã¿ãè¿ããã¾ãã?     * @endjapanese
     */
    Bff_Task * (*getTask)(Bff_Scene *self);

    /**
     * @english
     * Check if this scene is a current scene of a task.
     *
     * A scene becomes "a current scene" after the two steps shown below.
     *
     *   -# A scene is scheduled by Bff_Task.scheduleScene() (or other
     *      methods which do the same thing). This makes the scene
     *      "a scheduled scene" of the task.
     *   -# The task performs scene switching, which promotes its
     *      scheduled scene to "a current scene".
     *
     * @param[in] self  A scene instance.
     *
     * @retval TRUE   This scene is working as a current scene of a task.
     * @retval FALSE  This scene is not working as a current scene of any
     *                task. If 'self' is null, FALSE is returned.
     * @endenglish
     *
     * @japanese
     * ãã®ã·ã¼ã³ãããã¿ã¹ã¯ã®ãç¾å¨ã®ã·ã¼ã³ãã§ãããã©ãããèª¿ã¹ã¾ãã?     *
     * ã·ã¼ã³ã¯ãä¸è¨ã®äºã¤ã®ã¹ãããã®ãã¨ã«ãç¾å¨ã®ã·ã¼ã³ãã«ãªãã¾ãã?     *
     *   -# Bff_Task.scheduleScene() (ãããã¯åæ§ã®ä»ã®ã¡ã½ãã?
     *      ã«ãããã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ããããããã«ãããã·ã¼ã³ã?!--
     *      -->ãã®ã¿ã¹ã¯ã®ãã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ãã«ãªãã?     *   -# ãã®ã¿ã¹ã¯ãã·ã¼ã³åãæ¿ããè¡ããã¹ã±ã¸ã¥ã¼ã«ããã<!--
     *      -->ã·ã¼ã³ããç¾å¨ã®ã·ã¼ã³ãã¸ã¨ææ ¼ãããã?     *
     * @param[in] self  ã·ã¼ã³ã¤ã³ã¹ã¿ã³ã?     *
     * @retval TRUE   ãã®ã·ã¼ã³ã¯ãããã¿ã¹ã¯ã®ç¾å¨ã®ã·ã¼ã³ã¨ãã¦<!--
     *                -->åãã¦ããã?     * @retval FALSE  ãã®ã·ã¼ã³ã¯ãã©ã®ã¿ã¹ã¯ã®ç¾å¨ã®ã·ã¼ã³ã§ã¯ãªãã?     *                self ããã«ãã¤ã³ã¿ã®å ´åãFALSE ãè¿ããã¾ãã?     * @endjapanese
     *
     * @see Bff_Task.scheduleScene()
     */
    boolean (*isCurrent)(Bff_Scene *self);

    /**
     * @english
     * Check if this scene is a scheduled scene of a task.
     *
     * A scene becomes "a scheduled scene" when it is scheduled by
     * Bff_Task.scheduleScene() (or other methods which do the same
     * thing).
     *
     * @param[in] self  A scene instance.
     *
     * @retval TRUE   This scene is associated with a task as a scheduled
     *                scene.
     * @retval FALSE  This scene is not associated with a task as a
     *                scheduled scene.
     * @endenglish
     *
     * @japanese
     * ãã®ã·ã¼ã³ãããã¿ã¹ã¯ã®ãã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ãã§ããã?!--
     * -->ã©ãããèª¿ã¹ã¾ãã?     *
     * ã·ã¼ã³ã¯ã?Bff_Task.scheduleScene() (ãããã¯åæ§ã®ä»ã®ã¡ã½ãã?
     * ã«ããã¹ã±ã¸ã¥ã¼ã«ãããã¨ãã«ããã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ã?!--
     * -->ã«ãªãã¾ãã?     *
     * @param[in] self  ã·ã¼ã³ã¤ã³ã¹ã¿ã³ã?     *
     * @retval TRUE   ãã®ã·ã¼ã³ã¯ãã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ã¨ãã¦ã?!--
     *                -->ãªãããã®ã¿ã¹ã¯ã«é¢é£ä»ãããã¦ããã?     * @retval FALSE  ãã®ã·ã¼ã³ã¯ãã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ã¨ãã¦ã¯ã?!--
     *                -->ãªãã®ã¿ã¹ã¯ã«ãé¢é£ä»ãããã¦ããªãã?     * @endjapanese
     *
     * @see Bff_Task.scheduleScene()
     */
    boolean (*isScheduled)(Bff_Scene *self);

    /**
     * @english
     * Set scene-specific data.
     *
     * Data set by this method can be retrieved later by
     * Bff_Scene.getData().
     *
     * There are similar methods, Bff.setData() and Bff_Task.getData(),
     * but note that their data scopes are different respectively.
     *
     * @param[in] self  A scene instance.
     * @param[in] data  Scene-specific data.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³åºæã®ãã¼ã¿ãã»ãããã¾ãã?     *
     * ãã®ã¡ã½ããã§ã»ãããããã¼ã¿ã¯ãå¾ãã Bff_Scene.getData()
     * ã«ããåãåºããã¨ãã§ãã¾ãã?     *
     * Bff.setData(), Bff_Task.setData() ã¨ããä¼¼ãã¡ã½ãããããã¾ãã?!--
     * -->ããã¼ã¿ã®ã¹ã³ã¼ããããããç°ãªãã®ã§æ³¨æãã¦ãã ããã?     *
     * @param[in] self  ã·ã¼ã³ã¤ã³ã¹ã¿ã³ã?     * @param[in] data  ã·ã¼ã³åºæãã¼ã¿
     * @endjapanese
     *
     * @see Bff_Scene.getData()
     * @see Bff.getData()
     * @see Bff.setData()
     * @see Bff_Task.getData()
     * @see Bff_Task.setData()
     */
    void (*setData)(Bff_Scene *self, void *data);
};


/**
 * @english
 * @page scene_switching Scene Switching
 * <ol>
 * <li>@ref scene_switching__state_pattern
 * <li>@ref scene_switching__bff_scene
 * <li>@ref scene_switching__lifecycle
 * </ol>
 *
 *
 * @section scene_switching__state_pattern State Pattern
 * Responses of a BREW application to events vary depending on
 * application states. For example, depending on the context,
 * a clear key event may trigger different actions such as (a)
 * close the application, (b) close a menu, (c) go back to the
 * previous screen, and so on.
 *
 * The most horrible implementation to change a response to a BREW
 * event per application state would look like below. First, the
 * code block dispatches an event based on the event code
 * (AEEEvent). Second, it writes another \c switch block again to
 * dispatch the event based on the key code (AVKType). Finally,
 * it checks the application state.
 *
 * @code
 * static boolean
 * myHandleEvent(MyApp *app, AEEEvent event, uint16 w, uint32 dw)
 * {
 *     // First, dispatch the event based on the event code (AEEEvent)
 *     switch (event)
 *     {
 *         case EVT_KEY:
 *             // Second, dispatch the event based on the key code (AVKType)
 *             switch (w)
 *             {
 *                 case AVK_CLR:
 *                     // Finally, check the application state.
 *                     switch (app->applicationState)
 *                     {
 *                         case APP_STATE_A:
 *                             ......
 *                             break;
 *
 *                         case APP_STATE_B:
 *                             ......
 *                             break;
 *
 *                         case APP_STATE_C:
 *                             ......
 *                             break;
 *
 *                         default:
 *                             break;
 *                     }
 *                     break;
 *
 *                 ......
 *             }
 *             break;
 *
 *         ......
 *     }
 * @endcode
 *
 * The root cause of this horrible code exists in that the only single
 * event handler (\c myHandleEvent() in the above example) handles all
 * events.
 *
 * &quot;<b>State pattern</b>&quot;, which is one of software design
 * patterns, is an appropriate programming style to solve this kind of
 * problems. Applying state pattern to the big \c switch block above
 * will result in that each application state has its own event handler,
 * respectively. Below is a pseudo code to show the concept.
 *
 * @code
 * typedef struct
 * {
 *     // Event handler per application state
 *     boolean (*handleEvent)(MyApp *app, AEEEvent ev, uint16 w, uint32 dw);
 * }
 * State;
 *
 * static boolean
 * myHandleEvent(MyApp *app, AEEEvent ev, uint16 w, uint32 dw)
 * {
 *     // Get the current state.
 *     State *state = getCurrentState(...);
 *
 *     // Invoke the state-specific event handler.
 *     return state->handleEvent(app, ev, w, dw);
 * }
 *
 * static boolean
 * stateA_handleEvent(MyApp *app, AEEEvent event, uint16 w, uint32 dw)
 * {
 *     // This function can focus on jobs dedicated to State A
 *     // without being bothered by other application states.
 *     ......
 * }
 *
 * static boolean
 * stateB_handleEvent(MyApp *app, AEEEvent ev, uint16 w, uint32 dw)
 * {
 *     ......
 * }
 *
 * static boolean
 * stateC_handleEvent(MyApp *app, AEEEvent ev, uint16 w, uint32 dw)
 * {
 *     ......
 * }
 * @endcode
 *
 *
 * @section scene_switching__bff_scene BFF Scene
 * To support state pattern programming, BFF provides <b>BFF Scene</b>
 * (Bff_Scene). BFF Scene intends to be mapped to one application state
 * that needs to respond to events in its own way. A BFF Scene can have
 * its own set of event handlers.
 *
 * There are some means to create a BFF Scene. The basic one is to call
 * Bff.createScene() method whose function prototype is as follows.
 *
 * @code
 * int (*createScene)(
 *     Bff *self, Bff_Scene **scene, Bff_Scene_Driver *driver);
 * @endcode
 *
 * As the function prototype indicates, a <b>BFF Scene Driver</b>
 * (Bff_Scene_Driver) needs to be given in order to create a BFF Scene.
 * BFF Scene Driver determines behaviors of a BFF Scene. Especially,
 * Bff_Scene_Driver.getHandlers() has to be implemented to set up
 * event handlers of a BFF scene.
 *
 *
 * @section scene_switching__lifecycle Life Cycle of BFF Scene
 * <div style="margin: 0 20px 20px 20px; float: right;">
 * @image html bff-scene-lifecycle.png "BFF Scene Lifecycle"
 * </div>
 *
 * A BFF Scene does not start to work only by creation. A BFF Scene
 * needs to be (1) scheduled and (2) promoted to a current scene to
 * start to work.
 *
 * There are some means to schedule a BFF Scene. The basic one is to call
 * Bff_Task.scheduleScene() method. A scheduled scene is promoted to a
 * current scene soon later (to be exact, when BFF Kernel's post event
 * dispatcher is executed) unless scene switching has been disabled by
 * Bff_Task.blockSceneSwitching().
 *
 * When a BFF Scene is promoted to a current scene, Bff_Scene_Driver.enter()
 * method of the scene driver dedicated to the scene is called if it is set.
 * Similarly, when a BFF Scene is demoted from a current scene,
 * Bff_Scene_Driver.leave() method is called if it is set. The figure right
 * illustrates the lifecycle of BFF Scene. Method names in the figure such
 * as %enter() are Bff_Scene_Driver methods that are called when their
 * corresponding operations are executed. For example, the figure says that
 * the following three methods are called when a BFF scene is created.
 *
 *   -# Bff_Scene_Driver.getName()
 *   -# Bff_Scene_Driver.getHandlers()
 *   -# Bff_Scene_Driver.initialize()
 *
 * For detailed description about Bff_Scene_Driver methods, please refer to
 * the description of Bff_Scene_Driver and Bff_Scene. For a working example,
 * please refer to @ref sample_program__scene_switching.
 * @endenglish
 */


/**
 * @japanese
 * @page scene_switching ã·ã¼ã³åãæ¿ã? * <ol>
 * <li>@ref scene_switching__state_pattern
 * <li>@ref scene_switching__bff_scene
 * <li>@ref scene_switching__lifecycle
 * </ol>
 *
 *
 * @section scene_switching__state_pattern ã¹ãã¼ããã¿ã¼ã³
 * ã¤ãã³ãã«å¯¾ãã BREW ã¢ããªã±ã¼ã·ã§ã³ã®åå¿ã¯ãã¢ããªã±ã¼ã·ã§ã³ã®<!--
 * -->ç¶æã«å¿ãã¦å¤ããã¾ããä¾ãã°ãã¯ãªã¢ã­ã¼ã¤ãã³ãã¯ãç¶æ³ã«<!--
 * -->å¿ãã¦ã?a)ã¢ããªã±ã¼ã·ã§ã³ãçµäºãããã?b)ã¡ãã¥ã¼ãéããã?!--
 * -->(c)åã®ç»é¢ã«æ»ãããªã©ã®ç°ãªãåä½ãå¼ãèµ·ããããããã¾ããã? *
 * ã¢ããªã±ã¼ã·ã§ã³ã®ç¶ææ¯ã?BREW ã¤ãã³ãã¸ã®åå¿ãå¤ããããã®ææªãª<!--
 * -->å®è£ã¯ãä¸è¨ã®ä¾ã®ããã«ãªãã§ãããããã®ã³ã¼ãã¯ãæåã«<!--
 * -->ã¤ãã³ãã³ã¼ã?(AEEEvent) ããã¨ã«ã¤ãã³ãã®æ¯ãåãããããªãã?!--
 * -->æ¬¡ã« \c switch ãã­ãã¯ãããä¸åº¦æ¸ãã¦ã­ã¼ã³ã¼ã?(AVKType)
 * ããã¨ã«ã¤ãã³ãã®æ¯ãåãããããªããæå¾ã«ã¢ããªã±ã¼ã·ã§ã³ã®ç¶æã<!--
 * -->ãã§ãã¯ãã¾ãã? *
 * @code
 * static boolean
 * myHandleEvent(MyApp *app, AEEEvent event, uint16 w, uint32 dw)
 * {
 *     // æåã«ãã¤ãã³ãã³ã¼ã (AEEEvent) ããã¨ã«ã¤ãã³ããæ¯ãåããã? *     switch (event)
 *     {
 *         case EVT_KEY:
 *             // æ¬¡ã«ãã­ã¼ã³ã¼ã (AVKType) ããã¨ã«ã¤ãã³ããæ¯ãåããã? *             switch (w)
 *             {
 *                 case AVK_CLR:
 *                     // æå¾ã«ãã¢ããªã±ã¼ã·ã§ã³ã®ç¶æããã§ãã¯ããã? *                     switch (app->applicationState)
 *                     {
 *                         case APP_STATE_A:
 *                             ......
 *                             break;
 *
 *                         case APP_STATE_B:
 *                             ......
 *                             break;
 *
 *                         case APP_STATE_C:
 *                             ......
 *                             break;
 *
 *                         default:
 *                             break;
 *                     }
 *                     break;
 *
 *                 ......
 *             }
 *             break;
 *
 *         ......
 *     }
 * @endcode
 *
 * ãã®ææªãªã³ã¼ãã®æ ¹æ¬åå ã¯ããã ä¸ã¤ã®ã¤ãã³ããã³ãã©ã?(ä¸è¨ã®ä¾<!--
 * -->ã«ããã \c myHandleEvent()) ã§å¨ã¦ã®ã¤ãã³ããå¦çãã¦ããã¨ãã<!--
 * -->ç¹ã«ããã¾ãã? *
 * ã½ããã¦ã§ã¢ãã¶ã¤ã³ãã¿ã¼ã³ã®ä¸ã¤ã§ããã?b>ã¹ãã¼ããã¿ã¼ã³</b>ã?!--
 * -->ã¯ããã®æã®åé¡ãè§£æ±ºããããã®é©åãªãã­ã°ã©ãã³ã°ææ³ã§ãã? * ä¸è¨ã®å¤§ããª \c switch ãã­ãã¯ã«ã¹ãã¼ããã¿ã¼ã³ãé©ç¨ããã¨ã?!--
 * -->åã¢ããªã±ã¼ã·ã§ã³ç¶æãããããã¤ãã³ããã³ãã©ã¼ãæã¤ãã¨ã?!--
 * -->ãªãã§ãããã? * ä¸è¨ã¯ããã®æ¦å¿µãç¤ºãæ¬ä¼¼ã³ã¼ãã§ãã? *
 * @code
 * typedef struct
 * {
 *     // ã¢ããªã±ã¼ã·ã§ã³ç¶ææ¯ã®ã¤ãã³ããã³ãã©ã? *     boolean (*handleEvent)(MyApp *app, AEEEvent ev, uint16 w, uint32 dw);
 * }
 * State;
 *
 * static boolean
 * myHandleEvent(MyApp *app, AEEEvent ev, uint16 w, uint32 dw)
 * {
 *     // ç¾å¨ã®ç¶æãåå¾ããã? *     State *state = getCurrentState(...);
 *
 *     // ç¶æã«ç¹åããã¤ãã³ããã³ãã©ã¼ãèµ·åããã? *     return state->handleEvent(app, ev, w, dw);
 * }
 *
 * static boolean
 * stateA_handleEvent(MyApp *app, AEEEvent ev, uint16 w, uint32 dw)
 * {
 *     // ãã®é¢æ°ã¯ãä»ã®ã¢ããªã±ã¼ã·ã§ã³ç¶æã«ç©ãããããã¨ãªãã? *     // ç¶æï¼¡ã«ç¹åããä½æ¥­ã«éä¸­ã§ããã? *     ......
 * }
 *
 * static boolean
 * stateB_handleEvent(MyApp *app, AEEEvent ev, uint16 w, uint32 dw)
 * {
 *     ......
 * }
 *
 * static boolean
 * stateC_handleEvent(MyApp *app, AEEEvent ev, uint16 w, uint32 dw)
 * {
 *     ......
 * }
 * @endcode
 *
 * @section scene_switching__bff_scene BFF ã·ã¼ã? * ã¹ãã¼ããã¿ã¼ã³ãã­ã°ã©ãã³ã°ããµãã¼ããããããBFF ã?<b>BFF ã·ã¼ã? * </b> (Bff_Scene) ãæä¾ãã¾ããBFF ã·ã¼ã³ã¯ãã¤ãã³ãã«å¯¾ãã¦ç¬èªã®<!--
 * -->æ¹æ³ã§åå¿ããªããã°ãªããªãã¢ããªã±ã¼ã·ã§ã³ç¶æã®ä¸ã¤ã«å¯¾å¿ãã<!--
 * -->ãã¨ãæå³ãã¦ãã¾ããBFF ã·ã¼ã³ã¯ãèªåç¨ã®ã¤ãã³ããã³ãã©ã¼ã?!--
 * -->ã»ãããæã¤ãã¨ãã§ãã¾ãã? *
 * BFF ã·ã¼ã³ãä½æããæ¹æ³ã¯å¹¾ã¤ãããã¾ããåºæ¬çãªããæ¹ã¯ãä¸è¨ã«<!--
 * -->ç¤ºãé¢æ°ãã­ãã¿ã¤ããæã?Bff.createScene() ã¡ã½ãããå¼ã¶ãã¨ã§ãã? *
 * @code
 * int (*createScene)(
 *     Bff *self, Bff_Scene **scene, Bff_Scene_Driver *driver);
 * @endcode
 *
 * é¢æ°ãã­ãã¿ã¤ããç¤ºãããã«ãBFF ã·ã¼ã³ãä½æããããã«ã¯ã?b>BFF
 * ã·ã¼ã³ãã©ã¤ãã¼</b> (Bff_Scene_Driver) ãæ¸¡ãå¿è¦ãããã¾ããBFF
 * ã·ã¼ã³ãã©ã¤ãã¼ã¯ãBFF ã·ã¼ã³ã®åä½ãæ±ºå®ãã¾ããç¹ã«ãBFF
 * ã·ã¼ã³ã®ã¤ãã³ããã³ãã©ã¼ç¾¤ãè¨­å®ããããã«ã¯ã? * Bff_Scene_Driver.getHandlers() ãå®è£ããå¿è¦ãããã¾ãã? *
 *
 * @section scene_switching__lifecycle BFF ã·ã¼ã³ã®ã©ã¤ããµã¤ã¯ã? * <div style="margin: 0 20px 20px 20px; float: right;">
 * @image html bff-scene-lifecycle.png "BFF ã·ã¼ã?ã©ã¤ããµã¤ã¯ã?
 * </div>
 *
 * ä½æããã ãã§ã¯ãBFF ã·ã¼ã³ã¯åãå§ãã¾ãããBFF ã·ã¼ã³ãåãå§ãã?!--
 * -->ã«ã¯ã?1)ã¹ã±ã¸ã¥ã¼ã«ããã?2)ç¾å¨ã®ã·ã¼ã³ã¸ã¨ææ ¼ãããå¿è¦ã<!--
 * -->ããã¾ãã? *
 * BFF ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ããæ¹æ³ã«ã¯å¹¾ã¤ãããã¾ããåºæ¬çãªããæ¹ã¯ã? * Bff_Task.scheduleScene() ã¡ã½ãããå¼ã¶ãã¨ã§ããã¹ã±ã¸ã¥ã¼ã«ããã<!--
 * -->ã·ã¼ã³ã¯ãã·ã¼ã³ã®åãæ¿ãã Bff_Task.blockSceneSwitching()
 * ã§ç¡å¹ã«ããã¦ããªãéããããã« (æ­£ç¢ºã«ã¯ BFF ã«ã¼ãã«ã®ãã¹ãã?!--
 * -->ã¤ãã³ããã£ã¹ãããã£ã¼ãå®è¡ãããã¨ãã? ç¾å¨ã®ã·ã¼ã³ã¸ã¨ææ ¼<!--
 * -->ããã¾ãã? *
 * BFF ã·ã¼ã³ãç¾å¨ã®ã·ã¼ã³ã¸ã¨ææ ¼ãããã¨ããããããã®ã·ã¼ã³ç¨ã?!--
 * -->ã·ã¼ã³ãã©ã¤ãã¼ã?Bff_Scene_Driver.enter() ãã»ãããã¦ããã°ã?!--
 * -->ãã®ã¡ã½ãããå®è¡ããã¾ããåæ§ã«ãBFF ã·ã¼ã³ãç¾å¨ã®ã·ã¼ã³ãã<!--
 * -->éæ ¼ãããã¨ãããããã»ããããã¦ããã?Bff_Scene_Driver.leave()
 * ã¡ã½ãããå¼ã°ãã¾ããå³ã®å³ã¯ãBFF ã·ã¼ã³ã®ã©ã¤ããµã¤ã¯ã«ãç¤ºãã?!--
 * -->ãã¾ãã?enter() ç­ã®å³ä¸­ã®ã¡ã½ããåã¯ãå¯¾å¿ããå¦çãå®è¡ããã?!--
 * -->ã¨ãã«å¼ã°ãã?Bff_Scene_Driver ã¡ã½ããã§ããä¾ãã°ãBFF ã·ã¼ã³ã<!--
 * -->ä½æãããã¨ãã«æ¬¡ã®ä¸ã¤ã®ã¡ã½ãããå¼ã°ãããã¨ããå³ã¯ç¤ºãã¦ãã¾ãã? *
 *   -# Bff_Scene_Driver.getName()
 *   -# Bff_Scene_Driver.getHandlers()
 *   -# Bff_Scene_Driver.initialize()
 *
 * Bff_Scene_Driver ã¡ã½ããã«ã¤ãã¦ã®è©³ç´°ãªè¨è¿°ã«ã¤ãã¦ã¯ã? * Bff_Scene_Driver ã?Bff_Scene ã®èª¬æãåç§ãã¦ãã ããã? * åä½ããä¾ã«ã¤ãã¦ã¯ã@ref sample_program__scene_switching
 * ãåç§ãã¦ãã ããã? * @endjapanese
 */


#endif /* BFF_SCENE_H */
