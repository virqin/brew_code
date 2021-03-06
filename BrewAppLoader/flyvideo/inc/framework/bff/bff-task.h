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
 * BREW Foundation Framework, Task.
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, ã¿ã¹ã? * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 * @see @ref multitasking
 * @see Bff_Task_Driver
 */


#ifndef BFF_TASK_H
#define BFF_TASK_H


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
     * Size of buffer for task name.
     *
     * This value is given to Bff_Task_Driver.getName() as the size
     * of the buffer where the task name should be set.
     * @endenglish
     *
     * @japanese
     * ã¿ã¹ã¯åç¨ãããã¡ã®ãµã¤ã?     *
     * ãã®å¤ã¯ãã¿ã¹ã¯åãæ ¼ç´ãããããã¡ã®ãµã¤ãºã¨ãã¦ã?     * Bff_Task_Driver.getName() ã«æ¸¡ããã¾ãã?     * @endjapanese
     *
     * @see Bff_Task_Driver.getName()
     */
    Bff_Task_NAME_BUF_SIZE = 32
};


/*------------------------------------------------------------------------*
 * STRUCT DECLARATIONS
 *------------------------------------------------------------------------*/
/*
struct Bff_Task;
typedef struct Bff_Task Bff_Task;*/



/*------------------------------------------------------------------------*
 * STRUCTS
 *------------------------------------------------------------------------*/
/**
 * @english
 * BFF Task
 *
 * A BFF Task represents a live application instance. Technically
 * speaking, a BFF Task is mapped to an entry in the application history
 * list (i.e. an IAppHistory instance).
 *
 * Instances of BFF Tasks are created automatically by BFF (1) when an
 * EVT_APP_START is delivered and (2) when an EVT_APP_RESUME is delivered
 * and the event indicates that another live application instance is
 * being required. To be exact, for the case of (2), there is one more
 * condition so that another application instance can be created. The
 * condition is that Bff_Applet_Driver.isMultiTaskingEnabled() returns
 * TRUE. Otherwise, BFF does not create a new application instance when
 * it receives an EVT_APP_RESUME, and instead simply resumes the first
 * application instance.
 * @endenglish
 *
 * @japanese
 * BFF ã¿ã¹ã? *
 * BFF ã¿ã¹ã¯ã¯ãçå­ä¸­ã®ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ãè¡¨ç¾ãã¦ãã¾ãã? * æè¡çã«ã¯ãBFF ã¿ã¹ã¯ã¯ãã¢ããªã±ã¼ã·ã§ã³ãã¹ããªåã®ä¸ã¤ã®<!--
 * -->ã¨ã³ããªã¼ã«ãããããã¾ã?(IAppHistory ã¤ã³ã¹ã¿ã³ã¹)ã? *
 * (1) EVT_APP_START ãéä¿¡ãããã¨ãã?2) ããä¸ã¤ã®ã¢ããªã±ã¼ã·ã§ã³<!--
 * -->ã¤ã³ã¹ã¿ã³ã¹ãè¦æ±ããã¦ãããã¨ãç¤ºã?EVT_APP_RESUME ãéä¿?!--
 * -->ãããã¨ããBFF ã¯èªåçã?BFF ã¿ã¹ã¯ã®ã¤ã³ã¹ã¿ã³ã¹ãä½æãã¾ãã? * æ­£ç¢ºã«ã¯ã?2) ã®å ´åãããã²ã¨ã¤ã®ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ã<!--
 * -->ä½æãããããã«ã¯ãããä¸ã¤æ¡ä»¶ãããã¾ãããã®æ¡ä»¶ã¨ã¯ã? * Bff_Applet_Driver.isMultiTaskingEnabled() ã?TRUE ãè¿ããã¨ã§ãã? * ããä»¥å¤ã®å ´åã¯ãEVT_APP_RESUME ãåãåã£ã¦ããBFF ã¯æ°ãã<!--
 * -->ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ãä½æããããããã«ãåã«æåã®<!--
 * -->ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ã®ã¬ã¸ã¥ã¼ã ããããªãã¾ãã? * @endjapanese
 *
 * @see @ref multitasking
 * @see Bff_Task_Driver
 */
typedef struct _Bff_Task
{
    /**
     * @english
     * Block scene switching.
     *
     * If scene switching is blocked by calling this method with TRUE,
     * the BFF task does not perform scene switching until this method
     * is called again with FALSE.
     *
     * @param[in] self   A task instance.
     * @param[in] block  TRUE to block scene switching. FALSE to unblock
     *                   scene switching.
     *
     * @retval SUCCESS   Successfully block/unblock scene switching.
     * @retval EBADPARM  'self' is NULL.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³ã®åãæ¿ãããã­ãã¯ããã?     *
     * TRUE ãæå®ãã¦ãã®ã¡ã½ãããå¼ã³åºããã·ã¼ã³ã®åãæ¿ããæ­¢ãã?!--
     * -->å ´åãååº?FALSE ãæå®ãã¦ãã®ã¡ã½ãããå¼ã³åºãã¾ã§ãBFF
     * ã¯ã·ã¼ã³ã®åãæ¿ãããããªãã¾ããã?     *
     * @param[in] self   ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     * @param[in] block  ã·ã¼ã³ã®åãæ¿ãããã­ãã¯ããå ´åã¯ TRUEã?     *                   ã·ã¼ã³ã®åãæ¿ããåéããå ´åã¯ FALSEã?     *
     * @retval SUCCESS   ã·ã¼ã³åãæ¿ãã®ãã­ãã¯ï¼ã¢ã³ãã­ãã¯ã«<!--
     *                   -->æåãã¾ããã?     * @retval EBADPARM  self ããã«ãã¤ã³ã¿ã§ãã?     * @endjapanese
     */
    int (*blockSceneSwitching)(Bff_Task *self, boolean block);

    /**
     * @english
     * Control the task in task-driver-specific way.
     *
     * Behaviors of this method depend on the implementation of
     * Bff_Task_Driver.control().
     *
     * BFF Scene also has a similar method, Bff_Scene.control().
     *
     * @param[in]     self     A task instance.
     * @param[in]     command  A command to this task. Its value and
     *                         meaning are determined by the task driver
     *                         implementation.
     * @param[in,out] args     An array of arguments for 'command'. Its
     *                         size, values and meaning are determined by
     *                         the task driver implementation.
     *
     * @retval SUCCESS       The command was handled successfully.
     * @retval EBADPARM      'self' is null.
     * @retval EUNSUPPORTED  The command is not supported. This error
     *                       code is returned when
     *                       Bff_Task_Driver.control() is not available
     *                       or the implementation of it returns this
     *                       error code.
     * @retval Others        Other error codes may be returned by the
     *                       task driver implementation.
     * @endenglish
     *
     * @japanese
     * ã¿ã¹ã¯ãã©ã¤ãã¼åºæã®æ¹æ³ã§ã¿ã¹ã¯ãå¶å¾¡ãã¾ãã?     *
     * ãã®ã¡ã½ããã®åä½ã¯ã?Bff_Task_Driver.control() ã«ä¾å­ãã¾ãã?     *
     * BFF ã·ã¼ã³ã«ãã?Bff_Scene.control() ã¨ããåæ§ã®ã¡ã½ãããããã¾ãã?     *
     * @param[in]     self     ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     * @param[in]     command  ãã®ã¿ã¹ã¯ã«å¯¾ããã³ãã³ããå¤ããã³<!--
     *                         -->ãã®æå³ã¯ãã¿ã¹ã¯ãã©ã¤ãã¼ã®å®è£ã?!--
     *                         -->ããæ±ºå®ããã¾ãã?     * @param[in,out] args     command ã®å¼æ°ã®éåããµã¤ãºãå¤ã?!--
     *                         -->ããã³ãã®æå³ã¯ãã¿ã¹ã¯ãã©ã¤ãã?!--
     *                         -->ã®å®è£ã«ããæ±ºå®ããã¾ãã?     *
     * @retval SUCCESS       ã³ãã³ããæ­£å¸¸ã«å¦çããã¾ããã?     * @retval EBADPARM      self ããã«ãã¤ã³ã¿ã§ãã?     * @retval EUNSUPPORTED  ã³ãã³ãããµãã¼ãããã¦ãã¾ããããã?!--
     *                       -->ã¨ã©ã¼ã³ã¼ãã¯ã?Bff_Task_Driver.control()
     *                       ãå®ç¾©ããã¦ããªãå ´åããããã¯ãå®è£ã<!--
     *                       -->ãã®ã¨ã©ã¼ã³ã¼ããè¿ãå ´åã«ãè¿ããã¾ãã?     * @retval ãã®ä»?       ã¿ã¹ã¯ãã©ã¤ãã¼ã®å®è£ã«ããããã®ä»ã?!--
     *                       -->ã¨ã©ã¼ã³ã¼ããè¿ããããã¨ãããã¾ãã?     * @endjapanese
     *
     * @see Bff_Task_Driver.control()
     * @see Bff_Scene.control()
     * @see Bff_Scene_Driver.control()
     */
    int (*control)(Bff_Task *self, uint32 command, Bff_Var *args);

    /**
     * @english
     * Get the current scene.
     *
     * @param[in] self  A task instance.
     *
     * @return  The current scene. If no scene is held by this task as a
     *          current scene, a null pointer is returned. If 'self' is
     *          null, a null pointer is returned.
     * @endenglish
     *
     * @japanese
     * ç¾å¨ã®ã·ã¼ã³ãåå¾ãã¾ãã?     *
     * @param[in] self  ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     *
     * @return  ç¾å¨ã®ã·ã¼ã³ããã®ã¿ã¹ã¯ãç¾å¨ã®ã·ã¼ã³ãä¿æãã¦ããªã?!--
     *          -->å ´åããã«ãã¤ã³ã¿ãè¿ããã¾ããself
     *          ããã«ãã¤ã³ã¿ã®å ´åããã«ãã¤ã³ã¿ãè¿ããã¾ãã?     * @endjapanese
     */
    Bff_Scene * (*getCurrentScene)(Bff_Task *self);

    /**
     * @english
     * Get the task-specific data.
     *
     * The data set by the last call of Bff_Task.setData() is returned.
     * Note that BREW Foundation Framework does not care about what data
     * has been stored by Bff_Task.setData().
     *
     * This is not a must, but it is expected that the application code
     * allocates a memory area for its own data structure (e.g. struct
     * MyTaskData) and sets the pointer to the memory area by
     * Bff_Task.setData() in the implementation of
     * Bff_Task_Driver.initialize() (or somewhere else appropriate
     * for the application). The value of the pointer can be retrieved
     * later by Bff_Task.getData(). Data stored in the data structure can
     * be treated as task-wide data.
     *
     * @param[in] self  A task instance.
     *
     * @return  The task-specific data. The data set by the last call of
     *          Bff_Task.setData() is returned. If Bff_Task.setData() has
     *          not been called before, NULL is returned. If 'self' is
     *          null, NULL is returned.
     * @endenglish
     *
     * @japanese
     * ã¿ã¹ã¯åºæã®ãã¼ã¿ãåå¾ãã¾ãã?     *
     * æå¾ã® Bff_Task.setData() ã®å¼åºãã§ã»ããããããã¼ã¿ãè¿ããã¾ãã?     * BREW Foundation Framework ã¯ã?Bff_Task.setData() ãã©ããª<!--
     * -->ãã¼ã¿ãã»ãããã¦ãããã«ã¤ãã¦ã¯é¢ç¥ãã¾ããã?     *
     * å¿é ã¨ããããã§ã¯ããã¾ããããæ³å®ãããä½¿ç¨æ¹æ³ã¯ã?     * Bff_Task_Driver.initialize() ã®å®è£åã?     * (ãããã¯ã¢ããªã±ã¼ã·ã§ã³ã«ã¨ã£ã¦é½åã®ããä»ã®å ´æã?
     * ç¬èªã®ãã¼ã¿æ§é ä½ (ä¾ãã?struct MyTaskData) ç¨ã®ã¡ã¢ã?!--
     * -->é åãç¢ºä¿ãããã®ã¡ã¢ãªé åã¸ã®ãã¤ã³ã¿ã?Bff_Task.setData()
     * ã§ã»ãããããã¨ã§ãããã®ãã¤ã³ã¿ã®å¤ã¯ãå¾ãã Bff_Task.getData()
     * ã«ããåãåºããã¨ãã§ãã¾ããå½è©²ãã¼ã¿æ§é ä½ã¯ãã¿ã¹ã¯åä½ã?!--
     * -->ãã¼ã¿ã¨ãã¦æ±ããã¨ãã§ãã¾ãã?     *
     * Bff.getData(), Bff_Scene.getData() ã¨ããä¼¼ãã¡ã½ãããããã¾ãã?!--
     * -->ããã¼ã¿ã®ã¹ã³ã¼ããããããç°ãªãã®ã§æ³¨æãã¦ãã ããã?     *
     * @param[in] self  ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     *
     * @return  ã¿ã¹ã¯åºæãã¼ã¿ãæå¾ã® Bff_Task.setData() ã«ãã?!--
     *          -->ã»ãããããå¤ãè¿ããã¾ãã?Bff_Task.setData() ã?!--
     *          -->ããã¾ã§ã«å¼ã³åºããã¦ããªãå ´åããã«ãã¤ã³ã¿ã<!--
     *          -->è¿ããã¾ããself ããã«ãã¤ã³ã¿ã®å ´åããã«ãã¤ã³ã?!--
     *          -->ãè¿ããã¾ãã?     * @endjapanese
     *
     * @see Bff_Task.setData()
     * @see Bff.getData()
     * @see Bff.setData()
     * @see Bff_Scene.getData()
     * @see Bff_Scene.setData()
     */
    void * (*getData)(Bff_Task *self);

    /**
     * @english
     * Get the task ID of this task.
     *
     * @param[in] self  A task instance.
     *
     * @return  The task ID of this task. #Bff_Task_Id_INVALID is returned
     *          if 'self' is null.
     * @endenglish
     *
     * @japanese
     * ãã®ã¿ã¹ã¯ã®ã¿ã¹ã?ID ãåå¾ãã¾ãã?     *
     * @param[in] self  ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     *
     * @return  ãã®ã¿ã¹ã¯ã®ã¿ã¹ã?IDãself ããã«ãã¤ã³ã¿ã®å ´åã?     *          #Bff_Task_Id_INVALID ãè¿ããã¾ãã?     * @endjapanese
     */
    Bff_Task_Id (*getId)(Bff_Task *self);

    /**
     * @english
     * Get the name of this task.
     *
     * Bff_Task_Driver.getName() method can overwrite the default value
     * of task name.
     *
     * @param[in] self  A task instance.
     *
     * @return  The name of this task. NULL is returned if 'self' is null.
     * @endenglish
     *
     * @japanese
     * ãã®ã¿ã¹ã¯ã®ååãåå¾ãã¾ãã?     *
     * Bff_Task_Driver.getName() ã§ãã¿ã¹ã¯åã®ããã©ã«ãå¤ã<!--
     * -->ä¸æ¸ããããã¨ãã§ãã¾ãã?     *
     * @param[in] self  ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     *
     * @return  ãã®ã¿ã¹ã¯ã®ååãself ããã«ãã¤ã³ã¿ã®å ´åã?!--
     *          -->ãã«ãã¤ã³ã¿ãè¿ããã¾ãã?     * @endjapanese
     *
     * @see Bff_Task_Driver.getName()
     */
    const char * (*getName)(Bff_Task *self);

    /**
     * @english
     * Get the scheduled scene.
     *
     * @param[in] self  A task instance.
     *
     * @return  The scheduled scene. If no scene is held by this task as a
     *          scheduled scene, a null pointer is returned. If 'self' is
     *          null, a null pointer is returned.
     * @endenglish
     *
     * @japanese
     * ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ãåå¾ãã¾ãã?     *
     * @param[in] self  ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     *
     * @return  ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ããã®ã¿ã¹ã¯ãã¹ã±ã¸ã¥ã¼ã«ããã<!--
     *          -->ã·ã¼ã³ãä¿æãã¦ããªãå ´åããã«ãã¤ã³ã¿ãè¿ããã¾ããself
     *          ããã«ãã¤ã³ã¿ã®å ´åããã«ãã¤ã³ã¿ãè¿ããã¾ãã?     * @endjapanese
     */
    Bff_Scene * (*getScheduledScene)(Bff_Task *self);

    /**
     * @english
     * Check if this task is the first one.
     *
     * The task whose creation has been triggered by EVT_APP_START is
     * regarded as the first task. Creation of other tasks are triggered
     * by EVT_APP_RESUME.
     *
     * @retval TRUE   This task is the first task.
     * @retval FALSE  This task is not the first task, or 'self' is null.
     * @endenglish
     *
     * @japanese
     * ãã®ã¿ã¹ã¯ãæåã®ã¿ã¹ã¯ãã©ããèª¿ã¹ã¾ãã?     *
     * EVT_APP_START ãå¥æ©ã¨ãã¦ä½æãããã¿ã¹ã¯ã¯ãæåã®ã¿ã¹ã?!--
     * -->ã§ããã¨ã¿ãªããã¾ããä»ã®ã¿ã¹ã¯ã®çæã¯ãEVT_APP_RESUME
     * ãå¥æ©ã¨ãªãã¾ãã?     *
     * @retval TRUE   ãã®ã¿ã¹ã¯ã¯æåã®ã¿ã¹ã¯ã§ãã?     * @retval FALSE  ãã®ã¿ã¹ã¯ã¯æåã®ã¿ã¹ã¯ã§ã¯ããã¾ããããããã?     *                self ããã«ãã¤ã³ã¿ã§ãã?     * @endjapanese
     */
    boolean (*isFirst)(Bff_Task *self);

    /**
     * @english
     * Check if this task is suspended.
     *
     * @param[in] self  A task instance.
     *
     * @retval TRUE   This task is suspended.
     * @retval FALSE  This task is not suspended, or 'self' is null.
     * @endenglish
     *
     * @japanese
     * ãã®ã¿ã¹ã¯ããµã¹ãã³ãããã¦ãããã©ãããèª¿ã¹ã¾ãã?     *
     * @param[in] self  ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     *
     * @retval TRUE   ãã®ã¿ã¹ã¯ã¯ãµã¹ãã³ãããã¦ãã¾ãã?     * @retval FALSE  ãã®ã¿ã¹ã¯ã¯ãµã¹ãã³ãããã¦ãã¾ããããããã?     *                self ããã«ãã¤ã³ã¿ã§ãã?     * @endjapanese
     */
    boolean (*isSuspended)(Bff_Task *self);

    /**
     * @english
     * Create and schedule a new scene.
     *
     * This method is a kind of a shortcut method which does "create
     * a BFF scene" and "schedule it". In other words, this method does
     * the same stuff as the combination of the following methods does.
     *
     *   - Bff.createScene()
     *   - Bff_Task.scheduleScene()
     *
     * @param[in]  self    A task instance.
     * @param[out] scene   A pointer to a newly created scene is returned
     *                     to the caller through this pointer. If NULL is
     *                     given, the pointer is not returned to the
     *                     caller, but it is still possible to get a
     *                     pointer of the new scene by scene name
     *                     (Bff.getSceneByName()).
     * @param[in]  driver  Scene driver for the new scene. The content
     *                     of 'driver' is copied, so the caller does not
     *                     have to keep the content of 'driver' after
     *                     this method returns.
     * @param[in]  arg     An argument associated with the scene. The
     *                     argument will be passed to the scene later when
     *                     a Bff_Scene_Driver.enter() implementation or
     *                     a Bff_Scene_Driver.cancel() implementation of
     *                     the scene is called.
     *
     * @retval SUCCESS    Successfully scheduled a one-shot scene.
     * @retval EBADPARM   Either or both of 'self' and 'driver' are null.
     * @retval EBADSTATE  The internal state of this task does not allow
     *                    scene scheduling.
     * @retval ENOMEMORY  Not enough memory.
     * @retval Others     Other error codes can be returned depending on
     *                    what the given scene driver return.
     * @endenglish
     *
     * @japanese
     * æ°ããã·ã¼ã³ä½æãã¦ã¹ã±ã¸ã¥ã¼ã«ãã¾ãã?     *
     * ãã®ã¡ã½ããã¯ãã·ã§ã¼ãã«ããã¡ã½ããã§ããBFF ã·ã¼ã³ãä½æ<!--
     * -->ããããããã®ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãããã¨ããä½æ¥­ããããªãã¾ãã?     * å¥ã®è¡¨ç¾ãããã¨ããã®ã¡ã½ããã¯ãä¸è¨ã®ã¡ã½ããã®çµã¿åããã?!--
     * -->åããã¨ããããªãã¾ãã?     *
     *   - Bff.createScene()
     *   - Bff_Task.scheduleScene()
     *
     * @param[in]  self    ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     * @param[in]  scene   æ°ããä½æãããã·ã¼ã³ã®ãã¤ã³ã¿ãè¿ããã¾ãã?     *                     ãã«ãã¤ã³ã¿ãæ¸¡ãããå ´åãæ°è¦ä½æããã<!--
     *                     -->ã·ã¼ã³ã®ãã¤ã³ã¿ã¯è¿ããã¾ããããã·ã¼ã³<!--
     *                     -->ã®ååããã¨ã«å¾ã§åå¾ãããã¨ã¯å¯è½ã§ã
     *                     (Bff.getSceneByName())ã?     * @param[in]  driver  æ°ããã·ã¼ã³ç¨ã®ã·ã¼ã³ãã©ã¤ãã¼ãdriver
     *                     ã®åå®¹ã¯ã³ãã¼ãããã®ã§ããã®ã¡ã½ãããã?!--
     *                     -->æ»ã£ã¦ãããã¨ã«ãå¼åºãå´ã driver
     *                     ã®åå®¹ãä¿æãã¦ããå¿è¦ã¯ããã¾ããã?     * @param[in]  arg     ã·ã¼ã³ã«é¢é£ä»ããå¼æ°ããã®å¼æ°ã¯ããã?!--
     *                     -->ã·ã¼ã³ã® Bff_Scene_Driver.enter() ãããã¯
     *                     Bff_Scene_Driver.cancel() ãå¾ã§å¼ã°ãã?!--
     *                     -->ã¨ãã«ã·ã¼ã³ã«æ¸¡ããã¾ãã?     *
     * @retval SUCCESS    æ°ããã·ã¼ã³ã®ã¹ã±ã¸ã¥ã¼ã«ã«æåãã¾ããã?     * @retval EBADPARM   self ã?driver ã®ã©ã¡ããããããã¯ä¸¡æ¹ã<!--
     *                    -->ãã«ãã¤ã³ã¿ã§ãã?     * @retval EBADSTATE  ãã®ã¿ã¹ã¯ã®åé¨ç¶æã«ãããã·ã¼ã³ã?!--
     *                    -->ã¹ã±ã¸ã¥ã¼ãªã³ã°ãæå¦ããã¾ããã?     * @retval ENOMEMORY  ã¡ã¢ãªä¸è¶³ã§ãã?     * @retval ãã®ä»?    æå®ãããã·ã¼ã³ãã©ã¤ãã¼ã®è¿ãå¤ã«ãããä»ã?!--
     *                    -->ã¨ã©ã¼ã³ã¼ããè¿ããããã¨ãããã¾ãã?     * @endjapanese
     *
     * @see Bff.createScene()
     * @see Bff_Task.scheduleScene()
     */
    int (*scheduleNewScene)(
        Bff_Task *self, Bff_Scene **scene, Bff_Scene_Driver *driver,
        void *arg);

    /**
     * @english
     * Schedule a one-shot scene.
     *
     * This method is a kind of a shortcut method which does "create
     * a BFF scene", "schedule it", and "release it" when the scene
     * finishes its job. In other words, this method does the same
     * stuff as the combination of the following methods does.
     *
     *   - Bff.createScene()
     *   - Bff_Task.scheduleScene()
     *   - Bff.releaseScene()
     *
     * @param[in]  self    A task instance.
     * @param[out] scene   A pointer to a newly created scene is returned
     *                     to the caller through this pointer. If NULL is
     *                     given, the pointer is not returned to the
     *                     caller, but it is still possible to get a
     *                     pointer of the new scene by scene name
     *                     (Bff.getSceneByName()).
     * @param[in]  driver  Scene driver for the new scene. The content
     *                     of 'driver' is copied, so the caller does not
     *                     have to keep the content of 'driver' after
     *                     this method returns.
     * @param[in]  arg     An argument associated with the scene. The
     *                     argument will be passed to the scene later when
     *                     a Bff_Scene_Driver.enter() implementation or
     *                     a Bff_Scene_Driver.cancel() implementation of
     *                     the scene is called.
     *
     * @retval SUCCESS    Successfully scheduled a one-shot scene.
     * @retval EBADPARM   Either or both of 'self' and 'driver' are null.
     * @retval EBADSTATE  The internal state of this task does not allow
     *                    scene scheduling.
     * @retval ENOMEMORY  Not enough memory.
     * @retval Others     Other error codes can be returned depending on
     *                    what the given scene driver return.
     * @endenglish
     *
     * @japanese
     * åçºã®ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãã¾ãã?     *
     * ãã®ã¡ã½ããã¯ãã·ã§ã¼ãã«ããã¡ã½ããã§ããBFF ã·ã¼ã³ãä½æ<!--
     * -->ããããããã®ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãããããä»äºãçµãã?!--
     * -->ã¨ãã«ãã®ã·ã¼ã³ãè§£æ¾ããããã¨ããä½æ¥­ããããªãã¾ãã?     * å¥ã®è¡¨ç¾ãããã¨ããã®ã¡ã½ããã¯ãä¸è¨ã®ã¡ã½ããã®çµã¿åããã?!--
     * -->åããã¨ããããªãã¾ãã?     *
     *   - Bff.createScene()
     *   - Bff_Task.scheduleScene()
     *   - Bff.releaseScene()
     *
     * @param[in]  self    ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     * @param[in]  scene   æ°ããä½æãããã·ã¼ã³ã®ãã¤ã³ã¿ãè¿ããã¾ãã?     *                     ãã«ãã¤ã³ã¿ãæ¸¡ãããå ´åãæ°è¦ä½æããã<!--
     *                     -->ã·ã¼ã³ã®ãã¤ã³ã¿ã¯è¿ããã¾ããããã·ã¼ã³<!--
     *                     -->ã®ååããã¨ã«å¾ã§åå¾ãããã¨ã¯å¯è½ã§ã
     *                     (Bff.getSceneByName())ã?     * @param[in]  driver  æ°ããã·ã¼ã³ç¨ã®ã·ã¼ã³ãã©ã¤ãã¼ãdriver
     *                     ã®åå®¹ã¯ã³ãã¼ãããã®ã§ããã®ã¡ã½ãããã?!--
     *                     -->æ»ã£ã¦ãããã¨ã«ãå¼åºãå´ã driver
     *                     ã®åå®¹ãä¿æãã¦ããå¿è¦ã¯ããã¾ããã?     * @param[in]  arg     ã·ã¼ã³ã«é¢é£ä»ããå¼æ°ããã®å¼æ°ã¯ããã?!--
     *                     -->ã·ã¼ã³ã® Bff_Scene_Driver.enter() ãããã¯
     *                     Bff_Scene_Driver.cancel() ãå¾ã§å¼ã°ãã?!--
     *                     -->ã¨ãã«ã·ã¼ã³ã«æ¸¡ããã¾ãã?     *
     * @retval SUCCESS    åçºã·ã¼ã³ã®ã¹ã±ã¸ã¥ã¼ã«ã«æåãã¾ããã?     * @retval EBADPARM   self ã?driver ã®ã©ã¡ããããããã¯ä¸¡æ¹ã<!--
     *                    -->ãã«ãã¤ã³ã¿ã§ãã?     * @retval EBADSTATE  ãã®ã¿ã¹ã¯ã®åé¨ç¶æã«ãããã·ã¼ã³ã?!--
     *                    -->ã¹ã±ã¸ã¥ã¼ãªã³ã°ãæå¦ããã¾ããã?     * @retval ENOMEMORY  ã¡ã¢ãªä¸è¶³ã§ãã?     * @retval ãã®ä»?    æå®ãããã·ã¼ã³ãã©ã¤ãã¼ã®è¿ãå¤ã«ãããä»ã?!--
     *                    -->ã¨ã©ã¼ã³ã¼ããè¿ããããã¨ãããã¾ãã?     * @endjapanese
     *
     * @see Bff.createScene()
     * @see Bff.releaseScene()
     * @see Bff_Task.scheduleScene()
     */
    int (*scheduleOneShotScene)(
        Bff_Task *self, Bff_Scene **scene, Bff_Scene_Driver *driver,
        void *arg);

    /**
     * @english
     * Schedule a scene.
     *
     * If another BFF Scene is already held by the BFF Task as a scheduled
     * scene, the existing scheduled scene is cancelled. This triggers a
     * call of the Bff_Scene_Driver.cancel() implementation associated
     * with the cancelled scene.
     *
     * @param[in] self   A task instance.
     * @param[in] scene  A scene instance to schedule. A null pointer also
     *                   can be accepted.
     * @param[in] arg    An argument associated with the scene. The
     *                   argument will be passed to the scene later when
     *                   a Bff_Scene_Driver.enter() implementation or
     *                   a Bff_Scene_Driver.cancel() implementation of
     *                   the scene is called.
     *
     * @retval SUCCESS    Successfully scheduled.
     * @retval EBADPARM   'self' is null.
     * @retval EBADSTATE  The internal state of this task does not allow
     *                    scene scheduling.
     * @retval EALREADY   The specified scene has already been scheduled
     *                    in this task or is the current running scene of
     *                    this task.
     * @retval EITEMBUSY  The specified scene is owned by other task as
     *                    a scheduled scene or a current scene.
     * @endenglish
     *
     * @japanese
     * ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãã¾ãã?     *
     * BFF ã¿ã¹ã¯ãæ¢ã«ä»ã®ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ã¨ãã?!--
     * -->ä¿æãã¦ããå ´åãæ¢å­ã®ã¹ã±ã¸ã¥ã¼ã«ãããã·ã¼ã³ã?!--
     * -->ã­ã£ã³ã»ã«ããã¾ããããã«ãããã­ã£ã³ã»ã«ãããã·ã¼ã³ã?!--
     * -->é¢é£ä»ãããã Bff_Scene_Driver.cancel() ã®å®è£ãå¼ã°ãã¾ãã?     *
     * @param[in] self   ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     * @param[in] scene  ã¹ã±ã¸ã¥ã¼ã«ããã·ã¼ã³ã¤ã³ã¹ã¿ã³ã¹ã?!--
     *                   -->ãã«ãã¤ã³ã¿ãåãä»ãããã¾ãã?     * @param[in] arg    ã·ã¼ã³ã«é¢é£ä»ããå¼æ°ããã®å¼æ°ã¯ããã?!--
     *                   -->ã·ã¼ã³ã® Bff_Scene_Driver.enter() ãããã¯
     *                   Bff_Scene_Driver.cancel() ãå¾ã§å¼ã°ãã?!--
     *                   -->ã¨ãã«ã·ã¼ã³ã«æ¸¡ããã¾ãã?     *
     * @retval SUCCESS    ã¹ã±ã¸ã¥ã¼ã«ã«æåãã¾ããã?     * @retval EBADPARM   self ããã«ãã¤ã³ã¿ã§ãã?     * @retval EBADSTATE  ãã®ã¿ã¹ã¯ã®åé¨ç¶æã«ãããã·ã¼ã³ã?!--
     *                    -->ã¹ã±ã¸ã¥ã¼ãªã³ã°ãæå¦ããã¾ããã?     * @retval EALREADY   æå®ãããã·ã¼ã³ã¯ããã®ã¿ã¹ã¯ã«å¯¾ãã¦<!--
     *                    -->ã¹ã±ã¸ã¥ã¼ã«æ¸ã¿ãããããã¯ãã®ã¿ã¹ã¯ã?!--
     *                    ç¾å¨ã®ã·ã¼ã³ã¨ãã¦åãã¦ãã¾ãã?     * @retval EITEMBUSY  æå®ãããã·ã¼ã³ã¯ãã¹ã±ã¸ã¥ã¼ã«ããã<!--
     *                    -->ã·ã¼ã³ã¨ãã¦ããããã¯ç¾å¨ã®ã·ã¼ã?!--
     *                    -->ã¨ãã¦ãä»ã®ã¿ã¹ã¯ã«ããä¿æããã¦ãã¾ãã?     * @endjapanese
     *
     * @see Bff_Task.scheduleNewScene()
     * @see Bff_Task.scheduleOneShotScene()
     * @see Bff_Task.scheduleSceneById()
     * @see Bff_Task.scheduleSceneByName()
     */
    int (*scheduleScene)(Bff_Task *self, Bff_Scene *scene, void *arg);

    /**
     * @english
     * Schedule a scene by ID.
     *
     * A BFF Scene whose ID is the same as the given one is looked up,
     * and if one is found, the same steps as those of
     * Bff_Task.scheduleScene() are performed and the scene is scheduled.
     * If no scene is found, an error code is returned.
     *
     * @param[in] self     A task instance.
     * @param[in] sceneId  A scene ID of a scene instance to schedule.
     * @param[in] arg      An argument associated with the scene. The
     *                     argument will be passed to the scene later when
     *                     a Bff_Scene_Driver.enter() implementation or
     *                     a Bff_Scene_Driver.cancel() implementation of
     *                     the scene is called.
     *
     * @retval SUCCESS    Successfully scheduled.
     * @retval EBADPARM   'self' is null.
     * @retval EBADITEM   No scene has the specified scene ID.
     * @retval EBADSTATE  The internal state of this task does not allow
     *                    scene scheduling.
     * @retval EALREADY   The specified scene has already been scheduled
     *                    in this task or is the current running scene of
     *                    this task.
     * @retval EITEMBUSY  The specified scene is owned by other task as
     *                    a scheduled scene or a current scene.
     * @endenglish
     *
     * @japanese
     * ID ããã¨ã«ãã¦ãã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãã¾ãã?     *
     * æå®ããã?ID ãæã?BFF ã·ã¼ã³ãæ¤ç´¢ããããè¦ã¤ããã°ã?     * Bff_Task.scheduleScene() ã¨åãæé ãè¸ã¿ããã®ã·ã¼ã³ã?!--
     * -->ã¹ã±ã¸ã¥ã¼ã«ãã¾ãããããè¦ã¤ãããªããã°ãã¨ã©ã¼<!--
     * -->ã³ã¼ããè¿ããã¾ãã?     *
     * @param[in] self     ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     * @param[in] sceneId  ã¹ã±ã¸ã¥ã¼ã«ããã·ã¼ã³ã¤ã³ã¹ã¿ã³ã¹ã®ã·ã¼ã?IDã?     * @param[in] arg      ã·ã¼ã³ã«é¢é£ä»ããå¼æ°ããã®å¼æ°ã¯ããã?!--
     *                     -->ã·ã¼ã³ã® Bff_Scene_Driver.enter() ãããã¯
     *                     Bff_Scene_Driver.cancel() ãå¾ã§å¼ã°ãã?!--
     *                     -->ã¨ãã«ã·ã¼ã³ã«æ¸¡ããã¾ãã?     *
     * @retval SUCCESS    ã¹ã±ã¸ã¥ã¼ã«ã«æåãã¾ããã?     * @retval EBADPARM   self ããã«ãã¤ã³ã¿ã§ãã?     * @retval EBADITEM   æå®ãããã·ã¼ã³ ID ãæã¤ã·ã¼ã³ãããã¾ããã?     * @retval EBADSTATE  ãã®ã¿ã¹ã¯ã®åé¨ç¶æã«ãããã·ã¼ã³ã?!--
     *                    -->ã¹ã±ã¸ã¥ã¼ãªã³ã°ãæå¦ããã¾ããã?     * @retval EALREADY   æå®ãããã·ã¼ã³ã¯ããã®ã¿ã¹ã¯ã«å¯¾ãã¦<!--
     *                    -->ã¹ã±ã¸ã¥ã¼ã«æ¸ã¿ãããããã¯ãã®ã¿ã¹ã¯ã?!--
     *                    ç¾å¨ã®ã·ã¼ã³ã¨ãã¦åãã¦ãã¾ãã?     * @retval EITEMBUSY  æå®ãããã·ã¼ã³ã¯ãã¹ã±ã¸ã¥ã¼ã«ããã<!--
     *                    -->ã·ã¼ã³ã¨ãã¦ããããã¯ç¾å¨ã®ã·ã¼ã?!--
     *                    -->ã¨ãã¦ãä»ã®ã¿ã¹ã¯ã«ããä¿æããã¦ãã¾ãã?     * @endjapanese
     *
     * @see Bff_Task.scheduleNewScene()
     * @see Bff_Task.scheduleOneShotScene()
     * @see Bff_Task.scheduleScene()
     * @see Bff_Task.scheduleSceneByName()
     */
    int (*scheduleSceneById)(
        Bff_Task *self, Bff_Scene_Id sceneId, void *arg);

    /**
     * @english
     * Schedule a scene by name.
     *
     * A BFF Scene whose name is the same as the given one is looked up,
     * and if one is found, the same steps as those of
     * Bff_Task.scheduleScene() are performed and the scene is scheduled.
     * If no scene is found, an error code is returned.
     *
     * Note that BFF allows name duplication of BFF Scenes, so if multiple
     * scenes have the same name as the specified one, which scene is
     * scheduled depends on BFF's internal implementation.
     *
     * @param[in] self  A task instance.
     * @param[in] name  A name of a scene instance to schedule.
     * @param[in] arg   An argument associated with the scene. The
     *                  argument will be passed to the scene later when
     *                  a Bff_Scene_Driver.enter() implementation or
     *                  a Bff_Scene_Driver.cancel() implementation of
     *                  the scene is called.
     *
     * @retval SUCCESS    Successfully scheduled.
     * @retval EBADPARM   Either or both of 'self' and 'name' are null.
     * @retval EBADITEM   No scene has the specified scene name.
     * @retval EBADSTATE  The internal state of this task does not allow
     *                    scene scheduling.
     * @retval EALREADY   The specified scene has already been scheduled
     *                    in this task or is the current running scene of
     *                    this task.
     * @retval EITEMBUSY  The specified scene is owned by other task as
     *                    a scheduled scene or a current scene.
     * @endenglish
     *
     * @japanese
     * ååããã¨ã«ãã¦ãã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã«ãã¾ãã?     *
     * æå®ãããååãæã¤ BFF ã·ã¼ã³ãæ¤ç´¢ããããè¦ã¤ããã°ã?     * Bff_Task.scheduleScene() ã¨åãæé ãè¸ã¿ããã®ã·ã¼ã³ã?!--
     * -->ã¹ã±ã¸ã¥ã¼ã«ãã¾ãããããè¦ã¤ãããªããã°ãã¨ã©ã¼<!--
     * -->ã³ã¼ããè¿ããã¾ãã?     *
     * BFF ã?BFF ã·ã¼ã³ã®ååã®éè¤ãè¨±ãã¦ããã®ã§ãããããæå®ããã<!--
     * -->ååãæã¤ã·ã¼ã³ãè¤æ°å­å¨ããå ´åãã©ã®ã·ã¼ã³ãã¹ã±ã¸ã¥ã¼ã?!--
     * -->ããããã¯ãBFF ã®åé¨å®è£ã«ä¾å­ãã¾ãã?     *
     * @param[in] self  ã¿ã¹ã¯ã¤ã³ã¹ã¿ã³ã?     * @param[in] name  ã¹ã±ã¸ã¥ã¼ã«ããã·ã¼ã³ã¤ã³ã¹ã¿ã³ã¹ã®åå
     * @param[in] arg   ã·ã¼ã³ã«é¢é£ä»ããå¼æ°ããã®å¼æ°ã¯ããã?!--
     *                  -->ã·ã¼ã³ã® Bff_Scene_Driver.enter() ãããã¯
     *                  Bff_Scene_Driver.cancel() ãå¾ã§å¼ã°ãã?!--
     *                  -->ã¨ãã«ã·ã¼ã³ã«æ¸¡ããã¾ãã?     *
     * @retval SUCCESS    ã¹ã±ã¸ã¥ã¼ã«ã«æåãã¾ããã?     * @retval EBADPARM   self ã?name ã®ã©ã¡ããããããã¯ä¸¡æ¹ã<!--
     *                    -->ãã«ãã¤ã³ã¿ã§ãã?     * @retval EBADITEM   æå®ãããååãæã¤ã·ã¼ã³ãããã¾ããã?     * @retval EBADSTATE  ãã®ã¿ã¹ã¯ã®åé¨ç¶æã«ãããã·ã¼ã³ã?!--
     *                    -->ã¹ã±ã¸ã¥ã¼ãªã³ã°ãæå¦ããã¾ããã?     * @retval EALREADY   æå®ãããã·ã¼ã³ã¯ããã®ã¿ã¹ã¯ã«å¯¾ãã¦<!--
     *                    -->ã¹ã±ã¸ã¥ã¼ã«æ¸ã¿ãããããã¯ãã®ã¿ã¹ã¯ã?!--
     *                    ç¾å¨ã®ã·ã¼ã³ã¨ãã¦åãã¦ãã¾ãã?     * @retval EITEMBUSY  æå®ãããã·ã¼ã³ã¯ãã¹ã±ã¸ã¥ã¼ã«ããã<!--
     *                    -->ã·ã¼ã³ã¨ãã¦ããããã¯ç¾å¨ã®ã·ã¼ã?!--
     *                    -->ã¨ãã¦ãä»ã®ã¿ã¹ã¯ã«ããä¿æããã¦ãã¾ãã?     * @endjapanese
     *
     * @see Bff_Task.scheduleNewScene()
     * @see Bff_Task.scheduleOneShotScene()
     * @see Bff_Task.scheduleScene()
     * @see Bff_Task.scheduleSceneById()
     */
    int (*scheduleSceneByName)(Bff_Task *self, const char *name, void *arg);

    /**
     * @english
     * Set task-specific data.
     *
     * Data set by this method can be retrieved later by
     * Bff_Task.getData().
     *
     * There are similar methods, Bff.setData() and Bff_Scene.getData(),
     * but note that their data scopes are different respectively.
     *
     * @param[in] self  A task instance.
     * @param[in] data  Task-specific data.
     * @endenglish
     *
     * @japanese
     * ã¿ã¹ã¯åºæã®ãã¼ã¿ãã»ãããã¾ãã?     *
     * ãã®ã¡ã½ããã§ã»ãããããã¼ã¿ã¯ããã¨ãã?     * Bff_Task.getData() ã§åãåºãã¾ãã?     *
     * Bff.setData(), Bff_Scene.setData() ã¨ããä¼¼ãã¡ã½ãããããã¾ãã?!--
     * -->ããã¼ã¿ã®ã¹ã³ã¼ããããããç°ãªãã®ã§æ³¨æãã¦ãã ããã?     * @endjapanese
     *
     * @see Bff_Task.getData()
     * @see Bff.getData()
     * @see Bff.setData()
     * @see Bff_Scene.getData()
     * @see Bff_Scene.setData()
     */
    void (*setData)(Bff_Task *self, void *data);
};


/**
 * @english
 * @page multitasking Multitasking
 * <ol>
 * <li>@ref multitasking__standard_way
 * <li>@ref multitasking__problems_in_standard_way
 * <li>@ref multitasking__bff_way
 * </ol>
 *
 *
 * @section multitasking__standard_way Standard way to enable multitasking
 * BFF supports multitasking. Multitasking here means a mechanism where
 * multiple instances of an idential applet can live simultaneously.
 * Unlike on other modern platforms, on BREW, an application has to
 * explicitly write some code to enable this multitasking feature.
 * Otherwise, no new instance is created and a single instance just
 * repeats to get suspended and resumed when an attempt to start a new
 * application instance is made.
 *
 * The standard way to enable the multitasking feature is not intuitive,
 * but simple. Calling <b>\c IAPPHISTORY_SetResumeData()</b> to store
 * arbitrary data in the context of application history will enable the
 * feature.
 *
 * The second and other application instances that follow will not
 * receive an \c EVT_APP_START event when it is started. Instead, they
 * receive an \c EVT_APP_RESUME event on their start-up. Because the
 * event handler that has been registered via \c AEEApplet_New() by the
 * first application instance (and also even the only global data area
 * created by extending \c AEEApplet structure) is <em>shared by all
 * application instances</em>, there must be a way to judge whether an
 * \c EVT_APP_RESUME event indicates:
 *
 *   -# that an existing application instance is getting resumed or
 *   -# that a new application instance is being created.
 *
 * The way is to check whether any data has been stored previously by
 * \c IAPPHISTORY_SetResumeData() when an \c EVT_APP_RESUME event is
 * received. <b>\c IAPPHISTORY_GetResumeData()</b> can be used to retrieve
 * the data.
 *
 * In summary, a multitasking aware application will contain some
 * code blocks like the following conceptual example.
 *
 * @code
 * // A code block to execute on EVT_APP_SUSPEND (or any time between
 * // from EVT_APP_START to EVT_APP_SUSPEND)
 *
 *     // Store data that identifies this application instance into
 *     // the application history context.
 *     IAPPHISTORY_SetResumeData(...);
 *
 *
 * // A code block to execute on EVT_APP_RESUME
 *
 *     // Try to retrieve data from the application history context.
 *     ret = IAPPHISTORY_GetResumeData(...);
 *
 *     switch (ret)
 *     {
 *         case AEE_SUCCESS:
 *             // Data was found. This means that an existing application
 *             // instance is getting resumed.
 *             ......
 *             break;
 *
 *         case AEE_EFAILED:
 *             // Data was not found. This means that a new application
 *             // instance is being created.
 *             ......
 *             break;
 *
 *         default:
 *             // An unexpected error. IAPPHISTORY_GetResumeData() must
 *             // have encountered some troubles.
 *             ......
 *             break;
 *     }
 * @endcode
 *
 *
 * @section multitasking__problems_in_standard_way Problems in standard way
 * You will soon find it cumbersome to handle multiple application
 * instances using the standard way. Problems you point out will be like
 * below.
 *
 *   -# The standard way itself is cumbersome to use.
 *   -# The single event handler registered by \c AEEApplet_New() is shared
 *      by all application instances. Programming becomes difficult and
 *      error-prone once each application instance wants to behave
 *      differently.
 *   -# The only global data area that extends \c AEEApplet structure
 *      is shared by all application instances. It would be better if
 *      each application instance can have its own data area that is
 *      separated from others'.
 *
 * BFF eliminates the problems.
 *
 *
 * @section multitasking__bff_way BFF way to enable multitasking
 * All you have to do to enable multitasking with BFF is to implement
 * Bff_Applet_Driver.isMultiTaskingEnabled() method that returns \c TRUE.
 * You do not have to and must not call \c IAPPHISTORY_SetResumeData()
 * if you enable multitasking by the BFF mechanism.
 *
 * @code
 * // A code block to call Bff_Applet_create().
 *
 *     Bff_Applet_Driver driver;
 *
 *     Bff_Applet_Driver_clear(&driver);
 *     driver.isMultiTaskingEnabled = applet_isMultiTaskingEnabled;
 *     ......
 *
 *
 * // The implementation of Bff_Applet_Driver.isMultiTaskingEnabled()
 * static boolean
 * applet_isMultiTaskingEnabled(Bff *bff)
 * {
 *     return TRUE;
 * }
 * @endcode
 *
 * BFF creates an instance of <b>BFF Task</b> (Bff_Task) per application
 * instance. A BFF application can customize behaviors of an application
 * instance (= a BFF Task) by setting up a <b>BFF Task Driver</b>
 * (Bff_Task_Driver). It is possible to execute per-task initialization
 * and finalzation, specify per-task event handlers and create per-task
 * data area. All of these can be configured by implementing
 * Bff_Task_Driver.initialize() and Bff_Task_Driver.finalize(),
 * Bff_Task_Driver.getPreHandlers() and Bff_Task_Driver.getPostHandlers(),
 * and using Bff_Task.getData() and Bff_Task.setData().
 *
 * BFF calls Bff_Applet_Driver.getTaskDriver() (if defined) when it creates
 * an instance of BFF Task (=when a new application instance is created).
 * A BFF application has to supply an implementation of the method in order
 * to customize behaviors of BFF Tasks.
 *
 * The function prototype of Bff_Applet_Driver.getTaskDriver() is as
 * follows.
 *
 * @code
 *   int (*getTaskDriver)(
 *      Bff *bff, Bff_Task_Id taskId, Bff_Task_Driver *driver);
 * @endcode
 *
 * An implementation of this method is expected to set function pointers
 * to some or all of the fields of '<code>driver</code>'. If a BFF
 * application wants to make all tasks behave the same, an implementation
 * of Bff_Applet_Driver.getTaskDriver() should always set the same function
 * pointers in '<code>driver</code>'. On the contrary, if a BFF application
 * wants to make each task behave in its own way, an implementation of
 * Bff_Applet_Driver.getTaskDriver() should set different function
 * pointers when it is called.
 *
 * A BFF Task ID is given to Bff_Applet_Driver.getTaskDriver(). The number
 * is a unique ID per task and the value can be obtained by
 * Bff_Task.getId(). #Bff_Task_Id_FIRST is given as \c taskId if
 * Bff_Applet_Driver.getTaskDriver() is called for the first application
 * instance (= if the invocation of the method has been triggerred by an
 * EVT_APP_START event).
 *
 * A task which is currently running can be obtained by
 * Bff.getCurrentTask(). Also, a task can be obtained by ID and name
 * (Bff.getTaskById(), Bff.getTaskByName()). The following code shows
 * the task name of the current task.
 *
 * @code
 * Bff *bff = BFF();
 * Bff_Task *task = bff->getCurrentTask(bff);
 *
 * if (task)
 * {
 *     BFF_INFO(("Task name is '%s'", task->getName(task)));
 * }
 * @endcode
 *
 * Please refer to @ref sample_program__multitasking.
 * @endenglish
 */


/**
 * @japanese
 * @page multitasking ãã«ãã¿ã¹ã­ã³ã°
 * <ol>
 * <li>@ref multitasking__standard_way
 * <li>@ref multitasking__problems_in_standard_way
 * <li>@ref multitasking__bff_way
 * </ol>
 *
 *
 * @section multitasking__standard_way ãã«ãã¿ã¹ã­ã³ã°ãæå¹ã«ããæ¨æºçãªããæ? * BFF ã¯ãã«ãã¿ã¹ã­ã³ã°ããµãã¼ããã¾ããããã§ãããã«ãã¿ã¹ã­ã³ã°<!--
 * -->ã¨ã¯ãåä¸ã®ã¢ãã¬ããã®ã¤ã³ã¹ã¿ã³ã¹ãè¤æ°åæã«çå­å¯è½ã§ãã<!--
 * -->ã¨ãããã¨ãæå³ãã¦ãã¾ããä»ã®è¿ä»£çãªãã©ãããã©ã¼ã ã¨ã?!--
 * -->ç°ãªããBREW ã§ã¯ããã®ãã«ãã¿ã¹ã­ã³ã°æ©è½ãæå¹ã«ããããã?!--
 * -->ã¢ããªã±ã¼ã·ã§ã³ãæç¤ºçã«ããã³ã¼ããæ¸ããªããã°ãªãã¾ããã? * ããããªãã¨ãã¢ããªã±ã¼ã·ã§ã³ã®æ°ããã¤ã³ã¹ã¿ã³ã¹ãä½ããã¨ãã¦ãã?!--
 * -->æ°ããã¤ã³ã¹ã¿ã³ã¹ã¯ä½æãããããã ãã ä¸ã¤ã®ã¤ã³ã¹ã¿ã³ã¹ã?!--
 * -->ãµã¹ãã³ãã¨ã¬ã¸ã¥ã¼ã ãç¹°ãè¿ãã ãã«ãªã£ã¦ãã¾ãã¾ãã? *
 * ãã«ãã¿ã¹ã­ã³ã°æ©è½ãæå¹ã«ããæ¨æºçãªæ¹æ³ã¯ãç´æçã§ã¯ããã¾ãã?!--
 * -->ããåç´ã§ããã¢ããªã±ã¼ã·ã§ã³å±¥æ­´ã®ã³ã³ãã­ã¹ãã«ä»»æã®ãã¼ã¿ã<!--
 * -->ä¿å­ãã <b>\c IAPPHISTORY_SetResumeData()</b> ãå¼ãã§ããã?!--
 * -->ãã®æ©è½ãæå¹ã«ãªãã¾ãã? *
 * äºã¤ç®ä»¥éã®ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ã¯ãèµ·åæã?\c EVT_APP_START
 * ã¤ãã³ããåãåãã¾ãããä»£ããã?\c EVT_APP_RESUME ãåãåãã¾ãã? * æåã®ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ã?AEEApplet_New() ã§ç»é²ãã?!--
 * -->ã¤ãã³ããã³ãã©ã?(åã³ \c AEEApplet æ§é ä½ãæ¡å¼µãã¦ä½æãã?!--
 * -->å¯ä¸ã®ã°ã­ã¼ãã«ãã¼ã¿é å? ã¯ã?em>å¨ã¦ã®ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹<!--
 * -->ã¿ã³ã¹ãå±æãã¦ãã</em>ã®ã§ã\c EVT_APP_RESUME ã¤ãã³ããæ¬¡ã?!--
 * -->ã©ã¡ããç¤ºãã¦ããã®ãã©ãããç¥ãæ¹æ³ããå¿ãå¿è¦ã§ãã? *
 *   -# æ¢å­ã®ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ãã¬ã¸ã¥ã¼ã ããããã¨ãã¦ããã? *   -# æ°ããã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ãä½æããããã¨ãã¦ããã? *
 * ãã®æ¹æ³ã¯ã\c EVT_APP_RESUME ã¤ãã³ããåãåã£ãã¨ãã«ãä½ããã?!--
 * -->ãã¼ã¿ãããã¾ã§ã?\c IAPPHISTORY_SetResumeData() ã§ä¿å­ããã¦<!--
 * -->ãããã©ããããã§ãã¯ãããã¨ã§ãã?b>\c IAPPHISTORY_GetResumeData()
 * </b>ã§ãã¼ã¿ãåãåºããã¨ãã§ãã¾ãã? *
 * è¦ç´ããã¨ããã«ãã¿ã¹ã­ã³ã°ã«å¯¾å¿ããã¢ããªã±ã¼ã·ã§ã³ã¯ãæ¬¡ã?!--
 * -->æ¦å¿µä¾ãç¤ºããããªã³ã¼ããå«ããã¨ã«ãªãã¾ãã? *
 * @code
 * // EVT_APP_SUSPEND ãåãåã£ãã¨ãã?(ãããã¯ EVT_APP_START ãã
 * // EVT_APP_SUSPEND ã®éã®ä»»æã®æç¹ã? å®è¡ããã³ã¼ããã­ãã? *
 *     // ãã®ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ãè­å¥ãããã¼ã¿ã? *     // ã¢ããªã±ã¼ã·ã§ã³å±¥æ­´ã³ã³ãã­ã¹ãã«ä¿å­ããã? *     IAPPHISTORY_SetResumeData(...);
 *
 *
 * // EVT_APP_RESUME ãåãåã£ãã¨ãã«å®è¡ããã³ã¼ããã­ãã¯
 *
 *     // ã¢ããªã±ã¼ã·ã§ã³å±¥æ­´ã³ã³ãã­ã¹ããããã¼ã¿ãåãåºãã? *     ret = IAPPHISTORY_GetResumeData(...);
 *
 *     switch (ret)
 *     {
 *         case AEE_SUCCESS:
 *             // ãã¼ã¿ãè¦ã¤ãã£ããæ¢å­ã®ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ã? *             // ã¬ã¸ã¥ã¼ã ããããã¨ãã¦ããã? *             ......
 *             break;
 *
 *         case AEE_EFAILED:
 *             // ãã¼ã¿ãè¦ã¤ãããªãã£ããæ°ããã¢ããªã±ã¼ã·ã§ã³
 *             // ã¤ã³ã¹ã¿ã³ã¹ãä½æããããã¨ãã¦ããã? *             ......
 *             break;
 *
 *         default:
 *             // æå¾ãã¦ããªãã¨ã©ã¼ãIAPPHISTORY_GetResumeData() ã? *             // ä½ããã®ãã©ãã«ã«é­ã£ãã? *             ......
 *             break;
 *     }
 * @endcode
 *
 *
 * @section multitasking__problems_in_standard_way æ¨æºçãªããæ¹ã®åé¡ç? * æ¨æºçãªããæ¹ã§è¤æ°ã®ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ãæ±ãã®ã¯åä»<!--
 * -->ã ã¨ãããã«æ°ã¥ããã¨ã«ãªãã§ããããããªããææããã§ããã?!--
 * -->åé¡ç¹ã¯æ¬¡ã®ãããªãã®ã§ãããã? *
 *   -# æ¨æºçãªããæ¹èªä½ããä½¿ãã®ãåä»ã§ããã? *   -# \c AEEApplet_New() ã§ç»é²ããã¤ãã³ããã³ãã©ã¼ãå¨ã¦ã®ã¢ãã?!--
 *      -->ã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ã§å±æããã¦ãããä¸ãã³ãåã¢ãã?!--
 *      -->ã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ã«å¥ãã®åä½ãããããã¨ããã¨ã?!--
 *      -->ãã­ã°ã©ãã³ã°ãé£ãããééãããããªã£ã¦ãã¾ãã? *   -# \c AEEApplet æ§é ä½ãæ¡å¼µããå¯ä¸ã®ã°ã­ã¼ãã«ãã¼ã¿é åã<!--
 *      -->å¨ã¦ã®ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ã§å±æããã¦ããã? *      åã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ããä»ããã¯åãé¢ããã?!--
 *      -->ããå°ç¨ã®ãã¼ã¿é åãæã¦ãã°ããã®ã«ã? *
 * BFF ã¯ãããã®åé¡ãåãé¤ãã¾ãã? *
 *
 * @section multitasking__bff_way ãã«ãã¿ã¹ã­ã³ã°ãæå¹ã«ãã BFF ã®ããæ¹
 * BFF ã§ãã«ãã¿ã¹ã­ã³ã°ãæå¹ã«ããããã«ããªããã°ãªããªããã¨ã¯ã? * TRUE ãè¿ã?Bff_Applet_Driver.isMultiTaskingEnabled() ã¡ã½ããã?!--
 * -->å®è£ãããã¨ã§ãããã?BFF ã®ä»çµã¿ãä½¿ã£ã¦ãã«ãã¿ã¹ã­ã³ã°ã?!--
 * -->æå¹ã«ããå ´åã\c IAPPHISTORY_SetResumeData() ãå¼ã¶å¿è¦ã¯<!--
 * -->ããã¾ããããã¾ããå¼ãã§ã¯ããã¾ããã? *
 * @code
 * // Bff_Applet_create() ãå¼ã¶ã³ã¼ããã­ãã¯
 *
 *     Bff_Applet_Driver driver;
 *
 *     Bff_Applet_Driver_clear(&driver);
 *     driver.isMultiTaskingEnabled = applet_isMultiTaskingEnabled;
 *     ......
 *
 *
 * // Bff_Applet_Driver.isMultiTaskingEnabled() ã®å®è£? * static boolean
 * applet_isMultiTaskingEnabled(Bff *bff)
 * {
 *     return TRUE;
 * }
 * @endcode
 *
 * BFF ã¯ãã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹æ¯ã?<b>BFF ã¿ã¹ã?/b> (Bff_Task)
 * ã®ã¤ã³ã¹ã¿ã³ã¹ãä½æãã¾ããBFF ã¢ããªã±ã¼ã·ã§ã³ã¯ã?b>BFF ã¿ã¹ã?!--
 * -->ãã©ã¤ãã?/b> (Bff_Task_Driver) ãè¨­å®ãããã¨ã«ãããã¢ããª<!--
 * -->ã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ã® (= BFF ã¿ã¹ã¯ã®) åä½ãã«ã¹ã¿ãã¤ãºãã?!--
 * -->ãã¨ãã§ãã¾ããã¿ã¹ã¯æ¯ã®åæåå¦çã¨çµäºå¦çãå®è¡ãããã?!--
 * -->ã¿ã¹ã¯æ¯ã®ã¤ãã³ããã³ãã©ã¼ç¾¤ãè¨­å®ããããã¿ã¹ã¯æ¯ã®ãã¼ã?!--
 * -->é åãä½æããããããã¨ãã§ãã¾ãããããã¯ã? * Bff_Task_Driver.initialize() ã?Bff_Task_Driver.finalize(),
 * Bff_Task_Driver.getPreHandlers() ã?Bff_Task_Driver.getPostHandlers()
 * ãå®è£ãã?Bff_Task.getData() ã?Bff_Task.setData() ãå©ç¨ãã?!--
 * -->ãã¨ã«ããè¨­å®ãããã¨ãã§ãã¾ãã? *
 * BFF ã?BFF ã¿ã¹ã¯ã®ã¤ã³ã¹ã¿ã³ã¹ãä½æããã¨ã?(= æ°ããã¢ããª<!--
 * -->ã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ãä½ãããã¨ã)ã?ããå®ç¾©ããã¦ããã°)
 * Bff_Applet_Driver.getTaskDriver() ãå¼ã³ã¾ããBFF ã¿ã¹ã¯ã®åä½ã?!--
 * -->ã«ã¹ã¿ãã¤ãºããããã«ã¯ãBFF ã¢ããªã±ã¼ã·ã§ã³ã¯ãã®ã¡ã½ããã®<!--
 * -->å®è£ãæä¾ããå¿è¦ãããã¾ãã? *
 * Bff_Applet_Driver.getTaskDriver() ã®é¢æ°ãã­ãã¿ã¤ãã¯æ¬¡ã®ã¨ããã§ãã? *
 * @code
 *   int (*getTaskDriver)(
 *      Bff *bff, Bff_Task_Id taskId, Bff_Task_Driver *driver);
 * @endcode
 *
 * ãã®ã¡ã½ããã®å®è£ã¯ã\c driver ã®å¹¾ã¤ããããã¯å¨ã¦ã®ãã£ã¼ã«ãã?!--
 * -->é¢æ°ãã¤ã³ã¿ãè¨­å®ãããã¨ãæå¾ããã¦ãã¾ããããã BFF
 * ã¢ããªã±ã¼ã·ã§ã³ãå¨ã¦ã®ã¿ã¹ã¯ã«åãåä½ãããããã¨æããªãã°ã? * Bff_Applet_Driver.getTaskDriver() ã®å®è£ã¯å¸¸ã«åãé¢æ°ãã¤ã³ã¿ç¾¤ã \c
 * driver ã«è¨­å®ãã¹ãã§ããããã§ã¯ãªããããã BFF ã¢ããªã±ã¼ã·ã§ã³<!--
 * -->ãã¿ã¹ã¯æ¯ã«å¥ãã®åä½ãããããã¨æããªãã°ã? * Bff_Applet_Driver.getTaskDriver() ã®å®è£ã¯ãå¼ã°ããã¨ãã«ç°ãªã?!--
 * -->é¢æ°ãã¤ã³ã¿ç¾¤ãè¨­å®ãã¹ãã§ãã? *
 * Bff_Applet_Driver.getTaskDriver() ã«ã¯ãBFF ã¿ã¹ã?ID ãæ¸¡ããã¾ãã? * ãã®æ°å­ã¯ã¿ã¹ã¯æ¯ã®ä¸æã® ID ã§ããã®å¤ã¯ Bff_Task.getId() ã§åå¾?!--
 * -->ãããã¨ãã§ãã¾ãã?Bff_Applet_Driver.getTaskDriver() ãæåã®<!--
 * -->ã¢ããªã±ã¼ã·ã§ã³ã¤ã³ã¹ã¿ã³ã¹ç¨ã«å¼ã°ããå ´å (= ãã®ã¡ã½ããã®èµ·åã
 * EVT_APP_START ã¤ãã³ããå¥æ©ã¨ãã¦ããå ´å)ã\c taskId ã¨ãã? * #Bff_Task_Id_FIRST ãæ¸¡ããã¾ãã? *
 * ç¾å¨åä½ä¸­ã®ã¿ã¹ã¯ã¯ã?Bff.getCurrentTask() ã§åå¾ã§ãã¾ããã¾ããID
 * ãååããã¿ã¹ã¯ãåå¾ãããã¨ãã§ãã¾ã (Bff.getTaskById(),
 * Bff.getTaskByName())ãæ¬¡ã®ã³ã¼ãã¯ãç¾å¨ã®ã¿ã¹ã¯ã®ååãè¡¨ç¤ºãã¾ãã? *
 * @code
 * Bff *bff = BFF();
 * Bff_Task *task = bff->getCurrentTask(bff);
 *
 * if (task)
 * {
 *     BFF_INFO(("Task name is '%s'", task->getName(task)));
 * }
 * @endcode
 *
 * @ref sample_program__multitasking ãåç§ãã¦ãã ããã? * @endjapanese
 */
#endif /* BFF_TASK_H */
