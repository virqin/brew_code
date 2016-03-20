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
 * BREW Foundation Framework, タスク実�? * @endjapanese
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
 * タスク名の初期値を生成するのに使うフォーマット文字�? * @endjapanese
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
 * BFF タスク実�? *
 * BffIm_Task は、下図が示すように�?Bff_Task を拡張します�? *
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
 * Bff_Task は、BFF アプリケーションに公開しているインターフェースを<!--
 * -->含んでいます。一方�?BffIm_Task は実装に特化したメソッド�?!--
 * -->データフィールドを含んでおり、それら�?BFF アプリケーション�?!--
 * -->直接操作すべきではありません�? * @endjapanese
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
     * タスクの公開インターフェース
     *
     * この構造体�?Bff_Task でキャストできるよう、このデータ<!--
     * -->フィールドはこの構造体の先頭になければいけません�?     * @endjapanese
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
     * このタスクの終了処理をおこなう�?     *
     * @param[in] self  タスク実�?     * @endjapanese
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
     * BREW イベントを処理する�?     *
     * @param[in]  self      タスク実�?     * @param[in]  ev        イベントコー�?     * @param[in]  w         イベントの第一パラメー�?     * @param[in]  dw        イベントの第二パラメータ
     * @param[out] consumed  イベントがこのメソッドにより消費された場合は
     *                       TRUE が、そうでない場合�?FALSE
     *                       が書き戻されます�?     *
     * @return  BREW AEE に返すべき戻り値。戻り値の値と意味�?ev
     *          の値によって異なります�?     * @endjapanese
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
     * スケジュールされたシーンがあれば、シーンの切り替えをおこなう�?     *
     * @param[in] self  タスク実�?     *
     * @retval SUCCESS    シーンの切り替えに成功しました。もしくは�?!--
     *                    -->スケジュールされたシーンがありません�?     * @retval EBADSTATE  シーン切り替えがブロックされています�?     * @endjapanese
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
     * このタスクのタス�?ID
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
     * 様々なフラ�?     * @endjapanese
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
         * タスク・プレ・イベントディスパッチャーが定義されていれば 1
         * がセットされます�?         *
         * Bff_Task_Driver.getPreHandlers() が空ではなく、かつ、そ�?!--
         * -->呼出しが成功した場合、このデータフィールドに 1 がセット<!--
         * -->されます�?         * @endjapanese
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
         * タスク・ポスト・イベントディスパッチャーが定義されていれ�?1
         * がセットされます�?         *
         * Bff_Task_Driver.getPostHandlers() が空ではなく、かつ、そ�?!--
         * -->呼出しが成功した場合、このデータフィールドに 1 がセット<!--
         * -->されます�?         * @endjapanese
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
         * Bff_Task_Driver.finalize() をコールする必要がある場合�?
         * がセットされます�?         * @endjapanese
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
         * このタスクがサスペンドされている場合�? がセットされます�?         * @endjapanese
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
         * このタスクが「スケジュールされたシーン」を保持していれ�?         * 1 がセットされます�?         * @endjapanese
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
         * このタスクが、シーンのスケジューリングを受け付けない場合�?         * 1 がセットされます�?         * このデータフィールド�?1 の間は、下記のメソッド呼出しは<!--
         * -->失敗します�?         *
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
         * このタスクがシーン切り替えを行わない場合�? がセットされます�?         * このデータフィールドの値は�?Bff_Task.blockSceneSwitching()
         * で変更することができます�?         * @endjapanese
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
     * タスク固有データ
     *
     * このデータフィールド�?NULL で初期化されます�?Bff_Task.setData()
     * でこのデータフィールドに任意の値を設定でき�?Bff_Task.getData()
     * でその値を取り出すことができます�?     *
     * BFF は、BFF アプリケーションがこのデータフィールドをどのよう�?!--
     * -->使用するかについては関知しません�?     * @endjapanese
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
     * 前のタス�?     *
     * BffIm_Task 実装は、このフィールドがどのように使われるかに<!--
     * -->ついては関知しません�?     * @endjapanese
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
     * 次のタス�?     *
     * BffIm_Task 実装は、このフィールドがどのように使われるかに<!--
     * -->ついては関知しません�?     * @endjapanese
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
     * イベントネストレベル
     *
     * この値は�?handleEvent() がコールされる前�?BFF カーネル<!--
     * -->によりインクリメントされ�?handleEvent() 終了後に<!--
     * -->デクリメントされます�?     * @endjapanese
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
     * タスク名
     *
     * このデータフィールドは次のように初期化されます�?     *
     * @code
     * SNPRINTF(, , #BFFIM_TASK_NAME_FORMAT, taskId)
     * @endcode
     *
     * Bff_Task_Driver.getName() はその初期値を上書きできます�?     * @endjapanese
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
     * タスクドライバー
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
     * このタスクのプレ・イベントディスパッチャ�?     *
     * BREW イベントは、現在のシーンのイベントディスパッチャーに渡され�?!--
     * -->前にこのプレ・ディスパッチャーに渡されます�?     * @endjapanese
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
     * このタスクのポスト・イベントディスパッチャー
     *
     * このタスクのイベントハンドラ�?(BffIm_Task.handleEvent())
     * の最後の処理として、このポスト・ディスパッチャーに BREW
     * イベントが渡されます�?     * @endjapanese
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
     * 現在のシーン
     * @endjapanese
     */
    BffIm_Scene *currentScene;

    /**
     * @english
     * A scheduled scene.
     * @endenglish
     *
     * @japanese
     * スケジュールされたシーン
     * @endjapanese
     */
    BffIm_Scene *scheduledScene;

    /**
     * @english
     * An argument for the scheduled scene.
     * @endenglish
     *
     * @japanese
     * スケジュールされたシーンの引�?     * @endjapanese
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
     * シーンがスケジュールされたときのイベントネストレベル
     *
     * 現在のイベントネストレベルがこのデータフィールドによって保持<!--
     * -->されている値よりも深い (大き�? 場合、シーン切り替え�?!--
     * -->実行されません。これにより、同期イベントがシーン切り替えを<!--
     * -->引き起こすことを防止します�?     * @endjapanese
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
 * タスクを初期化します�? *
 * @param[out] instance  初期化するタスク
 * @param[in]  kernel    BFF カーネル
 * @param[in]  taskId    タスクに割り当てるタスク ID
 * @param[in]  driver    初期化するタスク用のタスクドライバー
 * @param[in]  flags     EVT_APP_START また�?EVT_APP_RESUME イベント�?!--
 *                       -->第一パラメー�? * @param[in]  appStart  EVT_APP_START また�?EVT_APP_RESUME イベント�?!--
 *                       -->第二パラメー�? *
 * @retval SUCCESS   初期化に成功しました�? * @retval EBADPARM  instance, kernel, driver のいずれかもしくは複数が<!--
 *                   -->ヌルポインタです�? * @endjapanese
 */
int
BffIm_Task_initialize(
    BffIm_Task *instance, BffIm_Kernel *kernel, Bff_Task_Id taskId,
    Bff_Task_Driver *driver, uint16 flags, AEEAppStart *appStart);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BFFIM_TASK_H */
