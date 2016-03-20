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
 * BREW Foundation Framework, タス�? * @endjapanese
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
     * タスク名用バッファのサイ�?     *
     * この値は、タスク名を格納するバッファのサイズとして�?     * Bff_Task_Driver.getName() に渡されます�?     * @endjapanese
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
 * BFF タス�? *
 * BFF タスクは、生存中のアプリケーションインスタンスを表現しています�? * 技術的には、BFF タスクは、アプリケーションヒストリ内の一つの<!--
 * -->エントリーにマップされま�?(IAppHistory インスタンス)�? *
 * (1) EVT_APP_START が配信されたとき�?2) もう一つのアプリケーション<!--
 * -->インスタンスが要求されていることを示�?EVT_APP_RESUME が配�?!--
 * -->されたとき、BFF は自動的�?BFF タスクのインスタンスを作成します�? * 正確には�?2) の場合、もうひとつのアプリケーションインスタンスが<!--
 * -->作成されるためには、もう一つ条件があります。その条件とは�? * Bff_Applet_Driver.isMultiTaskingEnabled() �?TRUE を返すことです�? * それ以外の場合は、EVT_APP_RESUME を受け取っても、BFF は新しい<!--
 * -->アプリケーションインスタンスを作成せず、かわりに、単に最初の<!--
 * -->アプリケーションインスタンスのレジュームをおこないます�? * @endjapanese
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
     * シーンの切り替えをブロックする�?     *
     * TRUE を指定してこのメソッドを呼び出し、シーンの切り替えを止め�?!--
     * -->場合、再�?FALSE を指定してこのメソッドを呼び出すまで、BFF
     * はシーンの切り替えをおこないません�?     *
     * @param[in] self   タスクインスタン�?     * @param[in] block  シーンの切り替えをブロックする場合は TRUE�?     *                   シーンの切り替えを再開する場合は FALSE�?     *
     * @retval SUCCESS   シーン切り替えのブロック／アンブロックに<!--
     *                   -->成功しました�?     * @retval EBADPARM  self がヌルポインタです�?     * @endjapanese
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
     * タスクドライバー固有の方法でタスクを制御します�?     *
     * このメソッドの動作は�?Bff_Task_Driver.control() に依存します�?     *
     * BFF シーンにも�?Bff_Scene.control() という同様のメソッドがあります�?     *
     * @param[in]     self     タスクインスタン�?     * @param[in]     command  このタスクに対するコマンド。値および<!--
     *                         -->その意味は、タスクドライバーの実装�?!--
     *                         -->より決定されます�?     * @param[in,out] args     command の引数の配列。サイズ、値�?!--
     *                         -->およびその意味は、タスクドライバ�?!--
     *                         -->の実装により決定されます�?     *
     * @retval SUCCESS       コマンドが正常に処理されました�?     * @retval EBADPARM      self がヌルポインタです�?     * @retval EUNSUPPORTED  コマンドがサポートされていません。こ�?!--
     *                       -->エラーコードは�?Bff_Task_Driver.control()
     *                       が定義されていない場合、もしくは、実装が<!--
     *                       -->このエラーコードを返す場合に、返されます�?     * @retval その�?       タスクドライバーの実装により、その他�?!--
     *                       -->エラーコードが返されることがあります�?     * @endjapanese
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
     * 現在のシーンを取得します�?     *
     * @param[in] self  タスクインスタン�?     *
     * @return  現在のシーン。このタスクが現在のシーンを保持していな�?!--
     *          -->場合、ヌルポインタが返されます。self
     *          がヌルポインタの場合、ヌルポインタが返されます�?     * @endjapanese
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
     * タスク固有のデータを取得します�?     *
     * 最後の Bff_Task.setData() の呼出しでセットされたデータが返されます�?     * BREW Foundation Framework は�?Bff_Task.setData() がどんな<!--
     * -->データをセットしていたかについては関知しません�?     *
     * 必須というわけではありませんが、想定される使用方法は�?     * Bff_Task_Driver.initialize() の実装内�?     * (もしくはアプリケーションにとって都合のよい他の場所�?
     * 独自のデータ構造体 (例え�?struct MyTaskData) 用のメモ�?!--
     * -->領域を確保し、そのメモリ領域へのポインタ�?Bff_Task.setData()
     * でセットすることです。そのポインタの値は、後から Bff_Task.getData()
     * により取り出すことができます。当該データ構造体は、タスク単位�?!--
     * -->データとして扱うことができます�?     *
     * Bff.getData(), Bff_Scene.getData() という似たメソッドがあります�?!--
     * -->、データのスコープがそれぞれ異なるので注意してください�?     *
     * @param[in] self  タスクインスタン�?     *
     * @return  タスク固有データ。最後の Bff_Task.setData() によ�?!--
     *          -->セットされた値が返されます�?Bff_Task.setData() �?!--
     *          -->それまでに呼び出されていない場合、ヌルポインタが<!--
     *          -->返されます。self がヌルポインタの場合、ヌルポイン�?!--
     *          -->が返されます�?     * @endjapanese
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
     * このタスクのタス�?ID を取得します�?     *
     * @param[in] self  タスクインスタン�?     *
     * @return  このタスクのタス�?ID。self がヌルポインタの場合�?     *          #Bff_Task_Id_INVALID が返されます�?     * @endjapanese
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
     * このタスクの名前を取得します�?     *
     * Bff_Task_Driver.getName() で、タスク名のデフォルト値を<!--
     * -->上書きすることができます�?     *
     * @param[in] self  タスクインスタン�?     *
     * @return  このタスクの名前。self がヌルポインタの場合�?!--
     *          -->ヌルポインタが返されます�?     * @endjapanese
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
     * スケジュールされたシーンを取得します�?     *
     * @param[in] self  タスクインスタン�?     *
     * @return  スケジュールされたシーン。このタスクがスケジュールされた<!--
     *          -->シーンを保持していない場合、ヌルポインタが返されます。self
     *          がヌルポインタの場合、ヌルポインタが返されます�?     * @endjapanese
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
     * このタスクが最初のタスクかどうか調べます�?     *
     * EVT_APP_START を契機として作成されたタスクは、最初のタス�?!--
     * -->であるとみなされます。他のタスクの生成は、EVT_APP_RESUME
     * が契機となります�?     *
     * @retval TRUE   このタスクは最初のタスクです�?     * @retval FALSE  このタスクは最初のタスクではありません。もしく�?     *                self がヌルポインタです�?     * @endjapanese
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
     * このタスクがサスペンドされているかどうかを調べます�?     *
     * @param[in] self  タスクインスタン�?     *
     * @retval TRUE   このタスクはサスペンドされています�?     * @retval FALSE  このタスクはサスペンドされていません。もしく�?     *                self がヌルポインタです�?     * @endjapanese
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
     * 新しいシーン作成してスケジュールします�?     *
     * このメソッドは、ショートカットメソッドで、「BFF シーンを作成<!--
     * -->する」、「そのシーンをスケジュールする」という作業をおこないます�?     * 別の表現をすると、このメソッドは、下記のメソッドの組み合わせ�?!--
     * -->同じことをおこないます�?     *
     *   - Bff.createScene()
     *   - Bff_Task.scheduleScene()
     *
     * @param[in]  self    タスクインスタン�?     * @param[in]  scene   新しく作成されたシーンのポインタが返されます�?     *                     ヌルポインタが渡された場合、新規作成された<!--
     *                     -->シーンのポインタは返されませんが、シーン<!--
     *                     -->の名前をもとに後で取得することは可能です
     *                     (Bff.getSceneByName())�?     * @param[in]  driver  新しいシーン用のシーンドライバー。driver
     *                     の内容はコピーされるので、このメソッドか�?!--
     *                     -->戻ってきたあとに、呼出し側が driver
     *                     の内容を保持しておく必要はありません�?     * @param[in]  arg     シーンに関連付ける引数。この引数は、そ�?!--
     *                     -->シーンの Bff_Scene_Driver.enter() もしくは
     *                     Bff_Scene_Driver.cancel() が後で呼ばれ�?!--
     *                     -->ときにシーンに渡されます�?     *
     * @retval SUCCESS    新しいシーンのスケジュールに成功しました�?     * @retval EBADPARM   self �?driver のどちらか、もしくは両方が<!--
     *                    -->ヌルポインタです�?     * @retval EBADSTATE  このタスクの内部状態により、シーン�?!--
     *                    -->スケジューリングが拒否されました�?     * @retval ENOMEMORY  メモリ不足です�?     * @retval その�?    指定されたシーンドライバーの返す値により、他�?!--
     *                    -->エラーコードが返されることがあります�?     * @endjapanese
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
     * 単発のシーンをスケジュールします�?     *
     * このメソッドは、ショートカットメソッドで、「BFF シーンを作成<!--
     * -->する」、「そのシーンをスケジュールする」、「仕事を終え�?!--
     * -->ときにそのシーンを解放する」、という作業をおこないます�?     * 別の表現をすると、このメソッドは、下記のメソッドの組み合わせ�?!--
     * -->同じことをおこないます�?     *
     *   - Bff.createScene()
     *   - Bff_Task.scheduleScene()
     *   - Bff.releaseScene()
     *
     * @param[in]  self    タスクインスタン�?     * @param[in]  scene   新しく作成されたシーンのポインタが返されます�?     *                     ヌルポインタが渡された場合、新規作成された<!--
     *                     -->シーンのポインタは返されませんが、シーン<!--
     *                     -->の名前をもとに後で取得することは可能です
     *                     (Bff.getSceneByName())�?     * @param[in]  driver  新しいシーン用のシーンドライバー。driver
     *                     の内容はコピーされるので、このメソッドか�?!--
     *                     -->戻ってきたあとに、呼出し側が driver
     *                     の内容を保持しておく必要はありません�?     * @param[in]  arg     シーンに関連付ける引数。この引数は、そ�?!--
     *                     -->シーンの Bff_Scene_Driver.enter() もしくは
     *                     Bff_Scene_Driver.cancel() が後で呼ばれ�?!--
     *                     -->ときにシーンに渡されます�?     *
     * @retval SUCCESS    単発シーンのスケジュールに成功しました�?     * @retval EBADPARM   self �?driver のどちらか、もしくは両方が<!--
     *                    -->ヌルポインタです�?     * @retval EBADSTATE  このタスクの内部状態により、シーン�?!--
     *                    -->スケジューリングが拒否されました�?     * @retval ENOMEMORY  メモリ不足です�?     * @retval その�?    指定されたシーンドライバーの返す値により、他�?!--
     *                    -->エラーコードが返されることがあります�?     * @endjapanese
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
     * シーンをスケジュールします�?     *
     * BFF タスクが既に他のシーンをスケジュールされたシーンとし�?!--
     * -->保持していた場合、既存のスケジュールされたシーン�?!--
     * -->キャンセルされます。これにより、キャンセルされるシーン�?!--
     * -->関連付けられた Bff_Scene_Driver.cancel() の実装が呼ばれます�?     *
     * @param[in] self   タスクインスタン�?     * @param[in] scene  スケジュールするシーンインスタンス�?!--
     *                   -->ヌルポインタも受け付けられます�?     * @param[in] arg    シーンに関連付ける引数。この引数は、そ�?!--
     *                   -->シーンの Bff_Scene_Driver.enter() もしくは
     *                   Bff_Scene_Driver.cancel() が後で呼ばれ�?!--
     *                   -->ときにシーンに渡されます�?     *
     * @retval SUCCESS    スケジュールに成功しました�?     * @retval EBADPARM   self がヌルポインタです�?     * @retval EBADSTATE  このタスクの内部状態により、シーン�?!--
     *                    -->スケジューリングが拒否されました�?     * @retval EALREADY   指定されたシーンは、このタスクに対して<!--
     *                    -->スケジュール済みか、もしくはこのタスク�?!--
     *                    現在のシーンとして動いています�?     * @retval EITEMBUSY  指定されたシーンは、スケジュールされた<!--
     *                    -->シーンとして、もしくは現在のシー�?!--
     *                    -->として、他のタスクにより保持されています�?     * @endjapanese
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
     * ID をもとにして、シーンをスケジュールします�?     *
     * 指定され�?ID を持�?BFF シーンを検索し、もし見つかれば�?     * Bff_Task.scheduleScene() と同じ手順を踏み、そのシーン�?!--
     * -->スケジュールします。もしも見つからなければ、エラー<!--
     * -->コードが返されます�?     *
     * @param[in] self     タスクインスタン�?     * @param[in] sceneId  スケジュールするシーンインスタンスのシー�?ID�?     * @param[in] arg      シーンに関連付ける引数。この引数は、そ�?!--
     *                     -->シーンの Bff_Scene_Driver.enter() もしくは
     *                     Bff_Scene_Driver.cancel() が後で呼ばれ�?!--
     *                     -->ときにシーンに渡されます�?     *
     * @retval SUCCESS    スケジュールに成功しました�?     * @retval EBADPARM   self がヌルポインタです�?     * @retval EBADITEM   指定されたシーン ID を持つシーンがありません�?     * @retval EBADSTATE  このタスクの内部状態により、シーン�?!--
     *                    -->スケジューリングが拒否されました�?     * @retval EALREADY   指定されたシーンは、このタスクに対して<!--
     *                    -->スケジュール済みか、もしくはこのタスク�?!--
     *                    現在のシーンとして動いています�?     * @retval EITEMBUSY  指定されたシーンは、スケジュールされた<!--
     *                    -->シーンとして、もしくは現在のシー�?!--
     *                    -->として、他のタスクにより保持されています�?     * @endjapanese
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
     * 名前をもとにして、シーンをスケジュールします�?     *
     * 指定された名前を持つ BFF シーンを検索し、もし見つかれば�?     * Bff_Task.scheduleScene() と同じ手順を踏み、そのシーン�?!--
     * -->スケジュールします。もしも見つからなければ、エラー<!--
     * -->コードが返されます�?     *
     * BFF �?BFF シーンの名前の重複を許しているので、もしも、指定された<!--
     * -->名前を持つシーンが複数存在する場合、どのシーンがスケジュー�?!--
     * -->されるかは、BFF の内部実装に依存します�?     *
     * @param[in] self  タスクインスタン�?     * @param[in] name  スケジュールするシーンインスタンスの名前
     * @param[in] arg   シーンに関連付ける引数。この引数は、そ�?!--
     *                  -->シーンの Bff_Scene_Driver.enter() もしくは
     *                  Bff_Scene_Driver.cancel() が後で呼ばれ�?!--
     *                  -->ときにシーンに渡されます�?     *
     * @retval SUCCESS    スケジュールに成功しました�?     * @retval EBADPARM   self �?name のどちらか、もしくは両方が<!--
     *                    -->ヌルポインタです�?     * @retval EBADITEM   指定された名前を持つシーンがありません�?     * @retval EBADSTATE  このタスクの内部状態により、シーン�?!--
     *                    -->スケジューリングが拒否されました�?     * @retval EALREADY   指定されたシーンは、このタスクに対して<!--
     *                    -->スケジュール済みか、もしくはこのタスク�?!--
     *                    現在のシーンとして動いています�?     * @retval EITEMBUSY  指定されたシーンは、スケジュールされた<!--
     *                    -->シーンとして、もしくは現在のシー�?!--
     *                    -->として、他のタスクにより保持されています�?     * @endjapanese
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
     * タスク固有のデータをセットします�?     *
     * このメソッドでセットしたデータは、あとか�?     * Bff_Task.getData() で取り出せます�?     *
     * Bff.setData(), Bff_Scene.setData() という似たメソッドがあります�?!--
     * -->、データのスコープがそれぞれ異なるので注意してください�?     * @endjapanese
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
 * @page multitasking マルチタスキング
 * <ol>
 * <li>@ref multitasking__standard_way
 * <li>@ref multitasking__problems_in_standard_way
 * <li>@ref multitasking__bff_way
 * </ol>
 *
 *
 * @section multitasking__standard_way マルチタスキングを有効にする標準的なやり�? * BFF はマルチタスキングをサポートします。ここでいうマルチタスキング<!--
 * -->とは、同一のアプレットのインスタンスが複数同時に生存可能である<!--
 * -->ということを意味しています。他の近代的なプラットフォームと�?!--
 * -->異なり、BREW では、このマルチタスキング機能を有効にするため�?!--
 * -->アプリケーションが明示的にあるコードを書かなければなりません�? * そうしないと、アプリケーションの新しいインスタンスを作ろうとしても�?!--
 * -->新しいインスタンスは作成されず、ただただ一つのインスタンス�?!--
 * -->サスペンドとレジュームを繰り返すだけになってしまいます�? *
 * マルチタスキング機能を有効にする標準的な方法は、直感的ではありませ�?!--
 * -->が、単純です。アプリケーション履歴のコンテキストに任意のデータを<!--
 * -->保存する <b>\c IAPPHISTORY_SetResumeData()</b> を呼んでやる�?!--
 * -->この機能が有効になります�? *
 * 二つ目以降のアプリケーションインスタンスは、起動時�?\c EVT_APP_START
 * イベントを受け取りません。代わり�?\c EVT_APP_RESUME を受け取ります�? * 最初のアプリケーションインスタンス�?AEEApplet_New() で登録し�?!--
 * -->イベントハンドラ�?(及び \c AEEApplet 構造体を拡張して作成し�?!--
 * -->唯一のグローバルデータ領�? は�?em>全てのアプリケーションインス<!--
 * -->タンスが共有している</em>ので、\c EVT_APP_RESUME イベントが次�?!--
 * -->どちらを示しているのかどうかを知る方法が、必ず必要です�? *
 *   -# 既存のアプリケーションインスタンスがレジュームされようとしている�? *   -# 新しいアプリケーションインスタンスが作成されようとしている�? *
 * その方法は、\c EVT_APP_RESUME イベントを受け取ったときに、何らか�?!--
 * -->データがそれまで�?\c IAPPHISTORY_SetResumeData() で保存されて<!--
 * -->いるかどうかをチェックすることです�?b>\c IAPPHISTORY_GetResumeData()
 * </b>でデータを取り出すことができます�? *
 * 要約すると、マルチタスキングに対応するアプリケーションは、次�?!--
 * -->概念例が示すようなコードを含むことになります�? *
 * @code
 * // EVT_APP_SUSPEND を受け取ったとき�?(もしくは EVT_APP_START から
 * // EVT_APP_SUSPEND の間の任意の時点�? 実行するコードブロッ�? *
 *     // このアプリケーションインスタンスを識別するデータ�? *     // アプリケーション履歴コンテキストに保存する�? *     IAPPHISTORY_SetResumeData(...);
 *
 *
 * // EVT_APP_RESUME を受け取ったときに実行するコードブロック
 *
 *     // アプリケーション履歴コンテキストからデータを取り出す�? *     ret = IAPPHISTORY_GetResumeData(...);
 *
 *     switch (ret)
 *     {
 *         case AEE_SUCCESS:
 *             // データが見つかった。既存のアプリケーションインスタンス�? *             // レジュームされようとしている�? *             ......
 *             break;
 *
 *         case AEE_EFAILED:
 *             // データが見つからなかった。新しいアプリケーション
 *             // インスタンスが作成されようとしている�? *             ......
 *             break;
 *
 *         default:
 *             // 期待していないエラー。IAPPHISTORY_GetResumeData() �? *             // 何らかのトラブルに遭った�? *             ......
 *             break;
 *     }
 * @endcode
 *
 *
 * @section multitasking__problems_in_standard_way 標準的なやり方の問題�? * 標準的なやり方で複数のアプリケーションインスタンスを扱うのは厄介<!--
 * -->だと、すぐに気づくことになるでしょう。あなたが指摘するであろ�?!--
 * -->問題点は次のようなものでしょう�? *
 *   -# 標準的なやり方自体が、使うのが厄介である�? *   -# \c AEEApplet_New() で登録したイベントハンドラーが全てのアプ�?!--
 *      -->ケーションインスタンスで共有されている。一たび、各アプ�?!--
 *      -->ケーションインスタンスに別々の動作をさせようとすると�?!--
 *      -->プログラミングが難しく、間違いやすくなってしまう�? *   -# \c AEEApplet 構造体を拡張する唯一のグローバルデータ領域が<!--
 *      -->全てのアプリケーションインスタンスで共有されている�? *      各アプリケーションインスタンスが、他からは切り離され�?!--
 *      -->それ専用のデータ領域を持てればいいのに�? *
 * BFF はこれらの問題を取り除きます�? *
 *
 * @section multitasking__bff_way マルチタスキングを有効にする BFF のやり方
 * BFF でマルチタスキングを有効にするためにしなければならないことは�? * TRUE を返�?Bff_Applet_Driver.isMultiTaskingEnabled() メソッド�?!--
 * -->実装することです。こ�?BFF の仕組みを使ってマルチタスキング�?!--
 * -->有効にする場合、\c IAPPHISTORY_SetResumeData() を呼ぶ必要は<!--
 * -->ありませんし、また、呼んではいけません�? *
 * @code
 * // Bff_Applet_create() を呼ぶコードブロック
 *
 *     Bff_Applet_Driver driver;
 *
 *     Bff_Applet_Driver_clear(&driver);
 *     driver.isMultiTaskingEnabled = applet_isMultiTaskingEnabled;
 *     ......
 *
 *
 * // Bff_Applet_Driver.isMultiTaskingEnabled() の実�? * static boolean
 * applet_isMultiTaskingEnabled(Bff *bff)
 * {
 *     return TRUE;
 * }
 * @endcode
 *
 * BFF は、アプリケーションインスタンス毎�?<b>BFF タス�?/b> (Bff_Task)
 * のインスタンスを作成します。BFF アプリケーションは�?b>BFF タス�?!--
 * -->ドライバ�?/b> (Bff_Task_Driver) を設定することにより、アプリ<!--
 * -->ケーションインスタンスの (= BFF タスクの) 動作をカスタマイズす�?!--
 * -->ことができます。タスク毎の初期化処理と終了処理を実行したり�?!--
 * -->タスク毎のイベントハンドラー群を設定したり、タスク毎のデー�?!--
 * -->領域を作成したりすることができます。これらは�? * Bff_Task_Driver.initialize() �?Bff_Task_Driver.finalize(),
 * Bff_Task_Driver.getPreHandlers() �?Bff_Task_Driver.getPostHandlers()
 * を実装し�?Bff_Task.getData() �?Bff_Task.setData() を利用す�?!--
 * -->ことにより設定することができます�? *
 * BFF �?BFF タスクのインスタンスを作成すると�?(= 新しいアプリ<!--
 * -->ケーションインスタンスが作られるとき)�?もし定義されていれば)
 * Bff_Applet_Driver.getTaskDriver() を呼びます。BFF タスクの動作�?!--
 * -->カスタマイズするためには、BFF アプリケーションはこのメソッドの<!--
 * -->実装を提供する必要があります�? *
 * Bff_Applet_Driver.getTaskDriver() の関数プロトタイプは次のとおりです�? *
 * @code
 *   int (*getTaskDriver)(
 *      Bff *bff, Bff_Task_Id taskId, Bff_Task_Driver *driver);
 * @endcode
 *
 * このメソッドの実装は、\c driver の幾つかもしくは全てのフィールド�?!--
 * -->関数ポインタを設定することが期待されています。もしも BFF
 * アプリケーションが全てのタスクに同じ動作をさせたいと望むならば�? * Bff_Applet_Driver.getTaskDriver() の実装は常に同じ関数ポインタ群を \c
 * driver に設定すべきです。そうではなく、もしも BFF アプリケーション<!--
 * -->がタスク毎に別々の動作をさせたいと望むならば�? * Bff_Applet_Driver.getTaskDriver() の実装は、呼ばれたときに異な�?!--
 * -->関数ポインタ群を設定すべきです�? *
 * Bff_Applet_Driver.getTaskDriver() には、BFF タス�?ID が渡されます�? * この数字はタスク毎の一意の ID で、その値は Bff_Task.getId() で取�?!--
 * -->することができます�?Bff_Applet_Driver.getTaskDriver() が最初の<!--
 * -->アプリケーションインスタンス用に呼ばれた場合 (= このメソッドの起動が
 * EVT_APP_START イベントを契機としている場合)、\c taskId とし�? * #Bff_Task_Id_FIRST が渡されます�? *
 * 現在動作中のタスクは�?Bff.getCurrentTask() で取得できます。また、ID
 * や名前からタスクを取得することもできます (Bff.getTaskById(),
 * Bff.getTaskByName())。次のコードは、現在のタスクの名前を表示します�? *
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
 * @ref sample_program__multitasking を参照してください�? * @endjapanese
 */
#endif /* BFF_TASK_H */
