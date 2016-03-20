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
 * BREW Foundation Framework, Kernel.
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, カーネル
 * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFFIM_KERNEL_H
#define BFFIM_KERNEL_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEStdLib.h"
#include "AEEAppHist.h"
#include "bff/bff.h"
#include "bff/bff-applet-driver.h"
#include "bff/bff-logging.h"
#include "bff/bff-dispatcher.h"
#include "bff/bff-scene-id.h"
#include "bff/bff-task-id.h"
#include "bffim/bffim-types.h"


/*------------------------------------------------------------------------*
 * MACROS
 *------------------------------------------------------------------------*/
/**
 * @english
 * Get the BFF Kernel.
 * @endenglish
 *
 * @japanese
 * BFF カーネルを取得す�? * @endjapanese
 */
#define BFFIM_KERNEL()  ((BffIm_Kernel *)BFF())


/*------------------------------------------------------------------------*
 * STRUCT DECLARATIONS
 *------------------------------------------------------------------------*/
/*
struct _BffIm_Kernel;
typedef struct _BffIm_Kernel BffIm_Kernel;
*/

struct _BffIm_Kernel_ReentrantData;
typedef struct _BffIm_Kernel_ReentrantData BffIm_Kernel_ReentrantData;



/*------------------------------------------------------------------------*
 * STRUCTS
 *------------------------------------------------------------------------*/
/**
 * @english
 * BFF Kernel
 *
 * Bff extends AEEApplet, and BffIm_Kernel extends Bff. The figure below
 * shows the relationship among these three structures.
 *
 * @code
 *       AEEApplet                Bff               BffIm_Kernel
 *   +---------------+ - - +---------------+ - - +----------------+
 *   |               |     |               |     |                |
 *   |               |     |   AEEApplet   |     |                |
 *   |               |     |               |     |                |
 *   +---------------+ - - + - - - - - - - +     |      Bff       |
 *                         |               |     |                |
 *                         |               |     |                |
 *                         |               |     |                |
 *                         +---------------+ - - + - - - - - - - -+
 *                                               |                |
 *                                               |                |
 *                                               |                |
 *                                               +----------------+
 * @endcode
 *
 * This indicates that AEEApplet, Bff and BffIm_Kernel can be cast
 * to one another.
 *
 * Bff contains interfaces that are open to BFF applications. On
 * the other hand, BffIm_Kernel contains implementation specific
 * methods and data fields that should not be touched directly by
 * BFF applications.
 * @endenglish
 *
 * @japanese
 * BFF カーネル
 *
 * Bff �?AEEApplet を拡張します�?BffIm_Kernel �?Bff を拡張します�? * 下図は、これら三つの構造体の関係を表しています�? *
 * @code
 *       AEEApplet                Bff               BffIm_Kernel
 *   +---------------+ - - +---------------+ - - +----------------+
 *   |               |     |               |     |                |
 *   |               |     |   AEEApplet   |     |                |
 *   |               |     |               |     |                |
 *   +---------------+ - - + - - - - - - - +     |      Bff       |
 *                         |               |     |                |
 *                         |               |     |                |
 *                         |               |     |                |
 *                         +---------------+ - - + - - - - - - - -+
 *                                               |                |
 *                                               |                |
 *                                               |                |
 *                                               +----------------+
 * @endcode
 *
 * これは、AEEApplet, Bff, BffIm_Kernel がお互いにキャスト可能で<!--
 * -->あることを示しています�? *
 * Bff は、BFF アプリケーションに公開しているインターフェースを<!--
 * -->含んでいます。一方�?BffIm_Kernel は実装に特化したメソッド�?!--
 * -->データフィールドを含んでおり、それら�?BFF アプリケーション�?!--
 * -->直接操作すべきではありません�? * @endjapanese
 */
typedef struct _BffIm_Kernel
{
    /**
     * @english
     * Interfaces that are open to BFF applications.
     *
     * This data field must be located at the top of this structure
     * so that this structure can be cast by Bff.
     * @endenglish
     *
     * @japanese
     * BFF アプリケーションに公開しているインターフェー�?     *
     * この構造体�?Bff でキャストできるよう、このデータ<!--
     * -->フィールドはこの構造体の先頭になければいけません�?     * @endjapanese
     */
    Bff bff;


    /*-------------------------------------------*
     * Methods
     *-------------------------------------------*/

    /**
     * @english
     * Finalize BFF Kernel.
     *
     * @param[in] self  The BFF Kernel.
     * @endenglish
     *
     * @japanese
     * BFF カーネルの終了処理をおこなう�?     *
     * @param[in] self  BFF カーネル
     * @endjapanese
     */
    void (*finalize)(BffIm_Kernel *self);

    /**
     * @english
     * Handle a BREW event.
     *
     * An event given to this method is processed by multiple event
     * dispatchers (= multiple sets of event handlers) until one of
     * them consumes the event. The order of the event dispatchers
     * is as follows.
     *
     *   -# Kernel Pre  Event Dispatcher
     *   -# Applet Pre  Event Dispatcher
     *   -# Task   Pre  Event Dispatcher
     *   -# Scene       Event Dispatcher
     *   -# Task   Post Event Dispatcher
     *   -# Applet Post Event Dispatcher
     *   -# Kernel Post Event Dispatcher
     *
     * Each event dispatcher is an instance of Bff_Dispatcher.
     * See the description of Bff_Dispatcher.dispatch() as to how
     * an event dispatcher behaves.
     *
     * @param[in] self  The BFF Kernel.
     * @param[in] ev    An event from BREW AEE.
     * @param[in] w     The first parameter of the event.
     * @param[in] dw    The second parameter of the event.
     *
     * @return  A value that should be returned to BREW AEE.
     *          The meaning of the returned value varies
     *          depending on the type of the event, but
     *          basically, TRUE is returned when the event
     *          has been handled by the event handler.
     * @endenglish
     *
     * @japanese
     * BREW イベントを処理します�?     *
     * このメソッドに渡されたイベントは、複数のイベントディスパッチャー
     * (= 複数のイベントハンドラー群セット) により、そのいずれかがその<!--
     * -->イベントを消費するまで処理されます。イベントディスパッチャ�?!--
     * -->の順番は下記の通りです�?     *
     *   -# カーネル   プレ   イベントディスパッチャー
     *   -# アプレッ�?プレ   イベントディスパッチャー
     *   -# タス�?    プレ   イベントディスパッチャー
     *   -# シー�?           イベントディスパッチャー
     *   -# タス�?    ポス�?イベントディスパッチャー
     *   -# アプレッ�?ポス�?イベントディスパッチャー
     *   -# カーネル   ポス�?イベントディスパッチャー
     *
     * 各イベントディスパッチャーは�?Bff_Dispatcher のインスタンスです�?     * イベントディスパッチャーがどのように動作するかについては�?     * Bff_Dispatcher.dispatch() の記述を参照してください�?     *
     * @param[in] self  BFF カーネル
     * @param[in] ev    BREW AEE からのイベン�?     * @param[in] w     イベントの第一パラメー�?     * @param[in] dw    イベントの第二パラメータ
     *
     * @return  BREW AEE に返すべき値。戻り値の意味は、イベントの<!--
     *          -->種別により異なりますが、基本的には、イベン�?!--
     *          -->ハンドラーがイベントを処理した場合、TRUE
     *          が返されます�?     * @endjapanese
     *
     * @see Bff_Dispatcher.dispatch()
     */
    boolean (*handleEvent)(
        BffIm_Kernel *self, AEEEvent ev, uint16 w, uint32 dw);


    /*-------------------------------------------*
     * Data fields
     *-------------------------------------------*/

    /*--------------------*
     * Driver
     *--------------------*/

    /**
     * @english
     * BFF Applet driver.
     *
     * This data field holds a copy of BFF Applet driver that have
     * been given to Bff_Applet_create().
     * @endenglish
     *
     * @japanese
     * BFF アプレットドライバー
     *
     * このデータフィールドは�?Bff_Applet_create() に渡され�?BFF
     * アプレットドライバーのコピーを保持します�?     * @endjapanese
     *
     * @see Bff_Applet_create()
     */
    Bff_Applet_Driver appletDriver;


    /*--------------------*
     * Kernel
     *--------------------*/

    /**
     * @english
     * Pre Event Dispatcher of BFF Kernel.
     *
     * BREW events are passed to this event dispatcher before
     * they are passed to the pre event dispatcher of the applet.
     * @endenglish
     *
     * @japanese
     * BFF カーネルのプレ・イベントディスパッチャー
     *
     * BREW イベントは、アプレットのプレ・イベントディスパッチャー�?!--
     * -->渡される前に、このイベントディスパッチャーに渡されます�?     * @endjapanese
     */
    Bff_Dispatcher kernelPreDispatcher;

    /**
     * @english
     * Post Event Dispatcher of BFF Kernel.
     *
     * BREW events are passed to this event dispatcher before
     * their results (boolean values) are returned to BREW AEE.
     * @endenglish
     *
     * @japanese
     * BFF カーネルのポスト・イベントディスパッチャ�?     *
     * BREW イベントは、その処理結�?(ブーリアン�? �?BREW AEE
     * に返される前に、このイベントディスパッチャーに渡されます�?     * @endjapanese
     */
    Bff_Dispatcher kernelPostDispatcher;

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
         * 1 is set if an applet pre event dispatcher is defined.
         *
         * If Bff_Applet_Driver.getPreHandlers() is not empty and
         * its call has succeeded, 1 is set to this data field.
         * @endenglish
         *
         * @japanese
         * アプレットのプレ・イベントディスパッチャーが定義されている場合�?
         * がセットされます�?         *
         * Bff_Applet_Driver.getPreHandlers() が空でなく、その呼出し�?!--
         * -->成功した場合、このデータフィールドに 1 がセットされます�?         * @endjapanese
         *
         * @see Bff_Applet_Driver.getPreHandlers()
         */
        unsigned int appletHasPreDispatcher:1;

        /**
         * @english
         * 1 is set if an applet post event dispatcher is defined.
         *
         * If Bff_Applet_Driver.getPostHandlers() is not empty and
         * its call has succeeded, 1 is set to this data field.
         * @endenglish
         *
         * @japanese
         * アプレットのポスト・イベントディスパッチャーが定義されている場合�?
         * がセットされます�?         *
         * Bff_Applet_Driver.getPostHandlers() が空でなく、その呼出し�?!--
         * -->成功した場合、このデータフィールドに 1 がセットされます�?         * @endjapanese
         *
         * @see Bff_Applet_Driver.getPostHandlers()
         */
        unsigned int appletHasPostDispatcher:1;

        /**
         * @english
         * 1 is set if Bff_Applet_Driver.finalize() needs to be called.
         * @endenglish
         *
         * @japanese
         * Bff_Applet_Driver.finalize() をコールする必要がある場合�?
         * がセットされます�?         * @endjapanese
         *
         * @see Bff_Applet_Driver.finalize()
         */
        unsigned int appletNeedsFinalizerCall:1;

        /**
         * @english
         * 1 is set if multi-tasking support is enabled.
         *
         * If Bff_Applet_Driver.isMultiTaskingEnabled() is defined
         * and it has returned TRUE, 1 is set to this data field.
         * @endenglish
         *
         * @japanese
         * マルチタスキングサポートが有効の場合�? がセットされます�?         *
         * Bff_Applet_Driver.isMultiTaskingEnabled() が定義されており�?!--
         * -->かつ、それが TRUE を返した場合、このデータフィールドに
         * 1 がセットされます�?         * @endjapanese
         *
         * @see Bff_Applet_Driver.isMultiTaskingEnabled()
         */
        unsigned int multiTaskingEnabled:1;

        /**
         * @english
         * 1 is set if the pointer to the current task needs to be updated.
         * @endenglish
         *
         * @japanese
         * 現在のタスクへのポインタを更新する必要がある場合�?
         * がセットされます�?         * @endjapanese
         */
        unsigned int needsCurrentTaskUpdate:1;

        /**
         * @english
         * 1 is set if initialization on EVT_APP_START has been completed.
         * @endenglish
         *
         * @japanese
         * EVT_APP_START 時の初期化が完了した場合�? がセットされます�?         * @endjapanese
         */
        unsigned int setupOnAppStartCompleted:1;
    }
    flags;

    /**
     * @english
     * Reentrant data per BffIm_Kernel.handleEvent().
     * @endenglish
     *
     * @japanese
     * BffIm_Kernel.handleEvent() 毎のリエントラント・デー�?     * @endjapanese
     */
    BffIm_Kernel_ReentrantData *reentrantData;

    /**
     * @english
     * The current logging level.
     *
     * This data field is initialized by Bff_Logging_Level_DEFAULT.
     * Bff.setLoggingLevel() can change the value of this data field
     * and Bff.getLoggingLevel() can retrieve the value.
     *
     * The value affects behaviors of BFF logging macros defined in
     * bff-logging.h.
     * @endenglish
     *
     * @japanese
     * 現在のロギングレベル
     *
     * このデータフィールドは�?#Bff_Logging_Level_DEFAULT
     * で初期化されます�?Bff.setLoggingLevel() でこのデータフィールドの<!--
     * -->値を変更でき�?Bff.getLoggingLevel() でその値を取得できます�?     *
     * この値は、bff-logging.h で定義されている BFF ロギングマクロの<!--
     * -->動作に影響を与えます�?     * @endjapanese
     *
     * @see Bff.getLoggingLevel()
     * @see Bff.setLoggingLevel()
     */
    Bff_Logging_Level loggingLevel;


    /*--------------------*
     * Applet
     *--------------------*/

    /**
     * @english
     * Pre Event Dispatcher of applet.
     *
     * If Bff_Applet_Driver.getPreHandlers() is defined, the
     * method is called to set up the value of this data field. If
     * the method has succeeded, flags.appletHasPreDispatcher is
     * set to 1.
     * @endenglish
     *
     * @japanese
     * アプレットのプレ・イベントディスパッチャ�?     *
     * Bff_Applet_Driver.getPreHandlers() が定義されている場合�?!--
     * -->このデータフィールドをセットアップするためにそのメソッド�?!--
     * -->呼ばれます。そのメソッドが成功した場合�?     * flags.appletHasPreDispatcher �?1 がセットされます�?     * @endjapanese
     *
     * @see Bff_Applet_Driver.getPreHandlers()
     */
    Bff_Dispatcher appletPreDispatcher;

    /**
     * @english
     * Post Event Dispatcher of applet.
     *
     * If Bff_Applet_Driver.getPostHandlers() is defined, the
     * method is called to set up the value of this data field. If
     * the method has succeeded, flags.appletHasPostDispatcher is
     * set to 1.
     * @endenglish
     *
     * @japanese
     * アプレットのポスト・イベントディスパッチャー
     *
     * Bff_Applet_Driver.getPostHandlers() が定義されている場合�?!--
     * -->このデータフィールドをセットアップするためにそのメソッド�?!--
     * -->呼ばれます。そのメソッドが成功した場合�?     * flags.appletHasPostDispatcher �?1 がセットされます�?     * @endjapanese
     *
     * @see Bff_Applet_Driver.getPostHandlers()
     */
    Bff_Dispatcher appletPostDispatcher;

    /**
     * @english
     * Applet-specific data.
     *
     * This data field is initialized by NULL. Bff.setData() can set
     * any data to this data field and Bff.getData() can retrieve the
     * value
     *
     * BFF does not care about how this data field is used by BFF
     * applications.
     * @endenglish
     *
     * @japanese
     * アプレット固有データ
     *
     * このデータフィールド�?NULL で初期化されます�?Bff.setData()
     * で任意の値をこのデータフィールドにセットすることができ�?     * Bff.getData() でその値を取り出すことができます�?     *
     * BFF は、BFF アプリケーションがこのデータフィールドをどのよう�?!--
     * -->使うかについては、関知しません�?     * @endjapanese
     *
     * @see Bff.getData()
     * @see Bff.setData()
     */
    void *appletData;


    /*--------------------*
     * Task
     *--------------------*/

    /**
     * @english
     * Task list.
     * @endenglish
     *
     * @japanese
     * タスクリスト
     * @endjapanese
     */
    BffIm_Task *taskList;

    /**
     * @english
     * The last task ID assigned most recently.
     * @endenglish
     *
     * @japanese
     * 最後に割り当てたタスク ID
     * @endjapanese
     */
    Bff_Task_Id lastTaskId;

    /**
     * @english
     * The current task.
     * @endenglish
     *
     * @japanese
     * 現在のタスク
     * @endjapanese
     */
    BffIm_Task *currentTask;


    /*--------------------*
     * Scene
     *--------------------*/

    /**
     * @english
     * Scene list.
     * @endenglish
     *
     * @japanese
     * シーンリスト
     * @endjapanese
     */
    BffIm_Scene *sceneList;

    /**
     * @english
     * The last scene ID assigned most recently.
     * @endenglish
     *
     * @japanese
     * 最後に割り当てたシーン ID
     * @endjapanese
     */
    Bff_Scene_Id lastSceneId;
};


/**
 * @english
 * Reentrant Data
 * @endenglish
 *
 * @japanese
 * リエントラント・デー�? * @endjapanese
 */
typedef struct _BffIm_Kernel_ReentrantData
{
    /**
     * @english
     * A reentrant data that was previously active.
     *
     * The previous reentrant data becomes active again when the
     * reentrant data above in the stack is popped.
     * @endenglish
     *
     * @japanese
     * このデータの前にアクティブだったリエントラント・デー�?     *
     * 前のリエントラントデータは、スタック内で上にあるリエントラント<!--
     * -->・データがポップされたときに再度アクティブになります�?     * @endjapanese
     */
    BffIm_Kernel_ReentrantData *prev;

    /**
     * @english
     * A return value from the applet-level event handler.
     * @endenglish
     *
     * @japanese
     * アプレットレベルのイベントハンドラーからの戻り�?     * @endjapanese
     */
    boolean returnValueFromApplet;
};


/*------------------------------------------------------------------------*
 * FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @english
 * Get the size of memory area that BFF implementation requires.
 *
 * The size retured by this function is supposed to be used as
 * the first argument of AEEApplet_New().
 *
 * @param[in]  classId  A class ID passed from BREW AEE.
 * @param[out] size     The size of the necessary memory area.
 *
 * @retval SUCCESS   Successfully obtained the size.
 * @retval EBADPARM  'size' is null.
 * @endenglish
 *
 * @japanese
 * BFF 実装が要求するメモリ領域のサイズを取得する�? *
 * この関数が返すサイズは、AEEApplet_New() の第一引数に使われます�? *
 * @param[in]  classId  BREW AEE から渡されたクラ�?ID
 * @param[out] size     必要なメモリ領域のサイズ
 *
 * @retval SUCCESS   サイズの取得に成功しました�? * @retval EBADPARM  size がヌルポインタです�? * @endjapanese
 */
int
BffIm_Kernel_getSize(AEECLSID classId, uint16 *size);


/**
 * @english
 * Initialize BFF kernel.
 *
 * @param[out] instance  A BffIm_Kernel instance to initialize.
 * @param[in]  classId   A class ID passed from BREW AEE.
 * @param[in]  size      The size of the memory area given to BFF
 *                       Kernel. The same size as has been set by
 *                       BffIm_Kernel_getSize() is supposed to be
 *                       given.
 * @param[in]  driver    BFF Applet driver. The content of 'driver'
 *                       is copied, so the caller does not have to
 *                       keep the content of 'driver' after this
 *                       function returns.
 *
 * @retval SUCCESS   Successfully initialized.
 * @retval EBADPARM  Either or both of 'instance' and 'driver' are null.
 * @endenglish
 *
 * @japanese
 * BFF カーネルを初期化する�? *
 * @param[out] instance  初期化す�?BffIm_Kernel インスタンス
 * @param[in]  classId   BREW AEE から渡されたクラ�?ID
 * @param[in]  size      BFF カーネル用のメモリ領域のサイズ�? *                       BffIm_Kernel_getSize() で返されたのと同�?!--
 *                       -->サイズが渡されるものと期待しています�? * @param[in]  driver    BFF アプレットドライバー。driver の内容は<!--
 *                       -->コピーされるので、この関数から戻って<!--
 *                       -->きた後、呼出し側が driver の内容を<!--
 *                       -->保持しておく必要はありません�? *
 * @retval SUCCESS   初期化に成功しました�? * @retval EBADPARM  instance �?driver のどちらかもしくは両方が<!--
 *                   -->ヌルポインタです�? * @endjapanese
 */
int
BffIm_Kernel_initialize(
    BffIm_Kernel *instance, AEECLSID classId, uint16 size,
    Bff_Applet_Driver *driver);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BFFIM_KERNEL_H */
