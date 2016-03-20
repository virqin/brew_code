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
 * BREW Foundation Framework, Task Driver.
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, タスクドライバー
 * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 * @see @ref multitasking
 * @see Bff_Task
 */


#ifndef BFF_TASK_DRIVER_H
#define BFF_TASK_DRIVER_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEShell.h"
#include "bff/bff-types.h"


/*------------------------------------------------------------------------*
 * STRUCT DECLARATIONS
 *------------------------------------------------------------------------*/
/*
struct Bff_Task_Driver;
typedef struct Bff_Task_Driver Bff_Task_Driver;
*/


/*------------------------------------------------------------------------*
 * STRUCTS
 *------------------------------------------------------------------------*/
/**
 * @english
 * BFF Task Driver
 *
 * When BFF creates a new task, BFF calls Bff_Applet_Driver.getTaskDriver()
 * to get a task driver. Subsequently, BFF calls some methods of the
 * obtained task driver (if they are defined) in the following order
 * during the task creation process.
 *
 *     -# Bff_Task_Driver.getName()
 *     -# Bff_Task_Driver.getPreHandlers()
 *     -# Bff_Task_Driver.getPostHandlers()
 *     -# Bff_Task_Driver.initialize()
 *
 * If the calls of these methods have succeeded, it is assured that
 * Bff_Task_Driver.finalize() is called later during the finalization
 * process of the task. Otherwise, Bff_Task_Driver.finalize() is not
 * called.
 * @endenglish
 *
 * @japanese
 * BFF タスクドライバー
 *
 * 新しいタスクを作成するとき、BFF �?Bff_Applet_Driver.getTaskDriver()
 * をコールして、タスクドライバーを取得します。続けて、BFF は、タスク<!--
 * -->生成処理の過程で、取得したタスクドライバーの幾つかのメソッド�?!--
 * -->下記の順番で (もし定義されていれば) 呼び出します�? *
 *     -# Bff_Task_Driver.getName()
 *     -# Bff_Task_Driver.getPreHandlers()
 *     -# Bff_Task_Driver.getPostHandlers()
 *     -# Bff_Task_Driver.initialize()
 *
 * これらのメソッドが成功した場合、後のタスク終了処理の過程で�? * Bff_Task_Driver.finalize() が呼び出されることが保証されます�? * それ以外の場合は�?Bff_Task_Driver.finalize() は呼び出されません�? * @endjapanese
 *
 * @see @ref multitasking
 * @see Bff_Task
 */
typedef struct _Bff_Task_Driver
{
    /**
     * @english
     * Control the task in task-driver-specific way.
     *
     * BFF does not care about how the implementation of this method
     * behaves. All BFF does is just call this method when
     * Bff_Task.control() is called. This is a generic interface that
     * can do anything like ioctl().
     *
     * BFF Scene Driver also has a similar method,
     * Bff_Scene_Driver.control().
     *
     * @param[in]     bff   A BFF instance.
     * @param[in]     task  A task instance.
     * @param[in]     cmd   A command to this task. Its value and
     *                      meaning are determined by the task driver
     *                      implementation.
     * @param[in,out] args  An array of arguments for 'command'. Its
     *                      size, values and meaning are determined by
     *                      the task driver implementation.
     *
     * @retval SUCCESS       The command was handled successfully.
     * @retval EUNSUPPORTED  The command is not supported.
     * @retval Others        Up to the task driver.
     * @endenglish
     *
     * @japanese
     * タスクドライバー固有の方法でタスクを制御します�?     *
     * BFF は、このメソッドの実装がどのように動作するかについては<!--
     * -->関知しません。BFF がすることは�?Bff_Task.control()
     * が呼ばれたときにこのメソッドを呼ぶことだけです。これは�?ioctl()
     * のように、何でもできる汎用のインターフェースです�?     *
     * BFF シーンドライバーにも�?Bff_Scene_Driver.control()
     * という同様のメソッドがあります�?     *
     * @param[in]     bff   BFF インスタンス
     * @param[in]     task  タスクインスタン�?     * @param[in]     cmd   このタスクに対するコマンド。値および<!--
     *                      -->その意味は、タスクドライバーの実装�?!--
     *                      -->より決定されます�?     * @param[in,out] args  command の引数の配列。サイズ、値�?!--
     *                      -->およびその意味は、タスクドライバ�?!--
     *                      -->の実装により決定されます�?     *
     * @retval SUCCESS       コマンドが正常に処理されました�?     * @retval EUNSUPPORTED  コマンドがサポートされていません�?     * @retval その�?       タスクドライバー次第
     * @endjapanese
     *
     * @see Bff_Task.control()
     * @see Bff_Scene.control()
     * @see Bff_Scene_Driver.control()
     */
    int (*control)(Bff *bff, Bff_Task *task, uint32 cmd, Bff_Var *args);

    /**
     * @english
     * Perform task-wide finalization.
     *
     * This method is called only once per task. An implementation of this
     * method, however, may be called multiple times if the implementation
     * is shared among multiple tasks.
     *
     * @param[in] bff   A BFF instance.
     * @param[in] task  A task instance to finalize.
     * @endenglish
     *
     * @japanese
     * タスクの終了処理をおこないます�?     *
     * このメソッドは、タスク毎に一回だけ呼ばれます。しかし、こ�?!--
     * -->メソッドの実装は、もしも複数のタスクで共有されているなら�?!--
     * -->複数回呼ばれることになります�?     *
     * @param[in] bff   BFF インスタンス
     * @param[in] task  終了処理をおこなうタスク
     * @endjapanese
     *
     * @see Bff_Task_Driver.initialize()
     */
    void (*finalize)(Bff *bff, Bff_Task *task);

    /**
     * @english
     * Get a task name.
     *
     * The name set by this method can be obtained by Bff_Task.getName().
     *
     * This method is called only once per task. An implementation of this
     * method, however, may be called multiple times if the implementation
     * is shared among multiple tasks.
     *
     * @param[in]     bff       A BFF instance.
     * @param[in]     task      A task instance whose name is given by
     *                          this method.
     * @param[in]     flags     The first event parameter which has come
     *                          along with an EVT_APP_START or
     *                          EVT_APP_RESUME event.
     * @param[in]     appStart  The second event parameter which has come
     *                          along with an EVT_APP_START or
     *                          EVT_APP_RESUME event.
     * @param[in,out] name      A buffer to store a task name. The default
     *                          value of task name is stored there when
     *                          this method is invoked.
     * @param[in]     size      Size of the name buffer.
     *                          (= #Bff_Task_NAME_BUF_SIZE)
     *
     * @retval SUCCESS  Successfully obtained a task name.
     * @retval Others   Other values than SUCCESS are regarded as errors.
     * @endenglish
     *
     * @japanese
     * タスク名を取得します�?     *
     * このメソッドによりセットされた名前は Bff_Task.getName()
     * で取得できます�?     *
     * このメソッドは、タスク毎に一回だけよばれます。しかし、こ�?!--
     * -->メソッドの実装は、もしも複数のタスクで共有されているなら�?!--
     * -->複数回呼ばれることになります�?     *
     * @param[in]     bff       BFF インスタンス
     * @param[in]     task      このメソッドにより名前を与えられるタスク
     * @param[in]     flags     EVT_APP_START もしくは EVT_APP_RESUME
     *                          イベントの第一パラメー�?     * @param[in]     appStart  EVT_APP_START もしくは EVT_APP_RESUME
     *                          イベントの第二パラメータ
     * @param[in,out] name      タスク名を格納するバッファ。こ�?!--
     *                          -->メソッドが呼び出されたときには�?!--
     *                          -->タスク名のデフォルト値が格納され�?!--
     *                          -->います�?     * @param[in]     size      名前を格納するバッファのサイ�?     *                          (= #Bff_Task_NAME_BUF_SIZE)
     *
     * @retval SUCCESS  タスク名の取得に成功しました�?     * @retval その�?  SUCCESS 以外の値はエラーとみなされます�?     * @endjapanese
     *
     * @see Bff_Task.getName()
     */
    int (*getName)(
        Bff *bff, Bff_Task *task, uint16 flags, AEEAppStart *appStart,
        char *name, int size);

    /**
     * @english
     * Set up task-level post event handlers.
     *
     * A BFF Task can have two sets of task-level event handlers. One is
     * called "Task Pre Event Handlers" and the other is called "Task
     * Post Event Handlers". This method sets up the latter.
     *
     * Event handlers set by this method are used only while this task is
     * working as a current task.
     *
     * This method is called only once per task creation. An implementation
     * of this method, however, may be called multiple times if the
     * implementation is shared among multiple tasks.
     *
     * @param[in]  bff       A BFF instance.
     * @param[in]  task      A task instance that is set up by this driver.
     * @param[in]  flags     The first event parameter which has come along
     *                       with an EVT_APP_START or EVT_APP_RESUME event.
     * @param[in]  appStart  The second event parameter which has come along
     *                       with an EVT_APP_START or EVT_APP_RESUME event.
     * @param[out] handlers  Event handlers. All data fields of 'handlers'
     *                       are null pointers when this method is invoked.
     *                       An implementation of this method is expected
     *                       to set function pointers to some or all of
     *                       data fields of 'handlers'.
     *
     * @retval SUCCESS  Successfully set up event handlers.
     * @retval Others   Other values than SUCCESS are regarded as errors.
     * @endenglish
     *
     * @japanese
     * タスクレベルのポスト・イベントハンドラー群を設定します�?     *
     * BFF タスクは、タスクレベルのイベントハンドラー群のセットを二�?!--
     * -->持つことができます。一つは「タスク・プレ・イベントハンドラ�?!--
     * -->群」で、もう一方は「タスク・ポスト・イベントハンドラー群�?!--
     * -->です。このメソッドは、後者を設定します�?     *
     * このメソッドで設定されたイベントハンドラー群は、このタスクが現�?!--
     * -->のタスクとして動いている間だけ使用されます�?     *
     * このメソッドは、タスク生成時に一回だけ呼ばれます。しかし、こ�?!--
     * -->メソッドの実装は、もしも複数のタスクで共有されているなら�?!--
     * -->複数回呼ばれることになります�?     *
     * @param[in]  bff       BFF インスタンス
     * @param[in]  task      このドライバーで設定するタスクインスタン�?     * @param[in]  flags     EVT_APP_START もしくは EVT_APP_RESUME
     *                       イベントの第一パラメー�?     * @param[in]  appStart  EVT_APP_START もしくは EVT_APP_RESUME
     *                       イベントの第二パラメータ
     * @param[out] handlers  イベントハンドラー群。このメソッドが呼ば�?!--
     *                       -->た時点では、handlers の全てのデー�?!--
     *                       -->フィールドにはヌルポインタが設定され�?!--
     *                       -->います。このメソッドの実装で、handlers
     *                       のデータフィールドの幾つかもしくは全てに<!--
     *                       -->関数ポインタをセットします�?     *
     * @retval SUCCESS  イベントハンドラー群の設定に成功しました�?     * @retval その�?  SUCCESS 以外の他の値はエラーとみなされます�?     * @endjapanese
     *
     * @see Bff_Task_Driver.getPreHandlers()
     * @see @ref event_dispatching
     */
    int (*getPostHandlers)(
        Bff *bff, Bff_Task *task, uint16 flags, AEEAppStart *appStart,
        Bff_Handlers *handlers);

    /**
     * @english
     * Set up task-level pre event handlers.
     *
     * A BFF Task can have two sets of task-level event handlers. One is
     * called "Task Pre Event Handlers" and the other is called "Task
     * Post Event Handlers". This method sets up the former.
     *
     * Event handlers set by this method are used only while this task is
     * working as a current task.
     *
     * This method is called only once per task creation. An implementation
     * of this method, however, may be called multiple times if the
     * implementation is shared among multiple tasks.
     *
     * @param[in]  bff       A BFF instance.
     * @param[in]  task      A task instance that is set up by this driver.
     * @param[in]  flags     The first event parameter which has come along
     *                       with an EVT_APP_START or EVT_APP_RESUME event.
     * @param[in]  appStart  The second event parameter which has come along
     *                       with an EVT_APP_START or EVT_APP_RESUME event.
     * @param[out] handlers  Event handlers. All data fields of 'handlers'
     *                       are null pointers when this method is invoked.
     *                       An implementation of this method is expected
     *                       to set function pointers to some or all of
     *                       data fields of 'handlers'.
     *
     * @retval SUCCESS  Successfully set up event handlers.
     * @retval Others   Other values than SUCCESS are regarded as errors.
     * @endenglish
     *
     *
     * @japanese
     * タスクレベルのプレ・イベントハンドラー群を設定します�?     *
     * BFF タスクは、タスクレベルのイベントハンドラー群のセットを二�?!--
     * -->持つことができます。一つは「タスク・プレ・イベントハンドラ�?!--
     * -->群」で、もう一方は「タスク・ポスト・イベントハンドラー群�?!--
     * -->です。このメソッドは、前者を設定します�?     *
     * このメソッドで設定されたイベントハンドラー群は、このタスクが現�?!--
     * -->のタスクとして動いている間だけ使用されます�?     *
     * このメソッドは、タスク生成時に一回だけ呼ばれます。しかし、こ�?!--
     * -->メソッドの実装は、もしも複数のタスクで共有されているなら�?!--
     * -->複数回呼ばれることになります�?     *
     * @param[in]  bff       BFF インスタンス
     * @param[in]  task      このドライバーで設定するタスクインスタン�?     * @param[in]  flags     EVT_APP_START もしくは EVT_APP_RESUME
     *                       イベントの第一パラメー�?     * @param[in]  appStart  EVT_APP_START もしくは EVT_APP_RESUME
     *                       イベントの第二パラメータ
     * @param[out] handlers  イベントハンドラー群。このメソッドが呼ば�?!--
     *                       -->た時点では、handlers の全てのデー�?!--
     *                       -->フィールドにはヌルポインタが設定され�?!--
     *                       -->います。このメソッドの実装で、handlers
     *                       のデータフィールドの幾つかもしくは全てに<!--
     *                       -->関数ポインタをセットします�?     *
     * @retval SUCCESS  イベントハンドラー群の設定に成功しました�?     * @retval その�?  SUCCESS 以外の他の値はエラーとみなされます�?     * @endjapanese
     *
     * @see Bff_Task_Driver.getPostHandlers()
     * @see @ref event_dispatching
     */
    int (*getPreHandlers)(
        Bff *bff, Bff_Task *task, uint16 flags, AEEAppStart *appStart,
        Bff_Handlers *handlers);

    /**
     * @english
     * Perform task-wide initialization.
     *
     * This method is called only once per task creation. An implementation
     * of this method, however, may be called multiple times if the
     * implementation is shared among multiple tasks.
     *
     * @param[in] bff       A BFF instance.
     * @param[in] task      A task instance to initialize.
     * @param[in] flags     The first event parameter which has come along
     *                      with an EVT_APP_START or EVT_APP_RESUME event.
     * @param[in] appStart  The second event parameter which has come along
     *                      with an EVT_APP_START or EVT_APP_RESUME event.
     *
     * @retval SUCCESS  Successfully initialized.
     * @retval Others   Other values than SUCCESS are regarded as errors.
     * @endenglish
     *
     * @japanese
     * タスクの初期化処理をおこないます�?     *
     * このメソッドは、タスク生成時に一回だけ呼ばれます。しかし、こ�?!--
     * -->メソッドの実装は、もしも複数のタスクで共有されているなら�?!--
     * -->複数回呼ばれることになります�?     *
     * @param[in]  bff       BFF インスタンス
     * @param[in]  task      初期化するタスクインスタンス
     * @param[in]  flags     EVT_APP_START もしくは EVT_APP_RESUME
     *                       イベントの第一パラメー�?     * @param[in]  appStart  EVT_APP_START もしくは EVT_APP_RESUME
     *                       イベントの第二パラメータ
     *
     * @retval SUCCESS  初期化処理に成功しました�?     * @retval その�?  SUCCESS 以外の値はエラーとみなされます�?     * @endjapanese
     *
     * @see Bff_Task_Driver.finalize()
     */
    int (*initialize)(
        Bff *bff, Bff_Task *task, uint16 flags, AEEAppStart *appStart);
};


#endif /* BFF_TASK_DRIVER_H */
