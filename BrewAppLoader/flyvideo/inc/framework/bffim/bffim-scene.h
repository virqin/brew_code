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
 * BREW Foundation Framework, シーン実�? * @endjapanese
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
 * シーン名の初期値を生成するのに使うフォーマット文字�? * @endjapanese
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
 * BFF シーン実�? *
 * BffIm_Scene は、下図が示すように�?Bff_Scene を拡張します�? *
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
 * Bff_Scene は、BFF アプリケーションに公開しているインターフェースを<!--
 * -->含んでいます。一方�?BffIm_Scene は実装に特化したメソッド�?!--
 * -->データフィールドを含んでおり、それら�?BFF アプリケーション�?!--
 * -->直接操作すべきではありません�? * @endjapanese
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
     * シーンの公開インターフェース
     *
     * この構造体�?Bff_Scene でキャストできるよう、このデータ<!--
     * -->フィールドはこの構造体の先頭になければいけません�?     * @endjapanese
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
     * このシーンの終了処理をおこなう�?     *
     * @param[in] self  シーン実�?     * @endjapanese
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
     * BREW イベントを処理する�?     *
     * @param[in]  self      シーン実�?     * @param[in]  ev        イベントコー�?     * @param[in]  w         イベントの第一パラメー�?     * @param[in]  dw        イベントの第二パラメータ
     * @param[out] consumed  イベントがこのメソッドにより消費された場合は
     *                       TRUE が、そうでない場合�?FALSE
     *                       が書き戻されます�?     *
     * @return  BREW AEE に返すべき戻り値。戻り値の値と意味�?ev
     *          の値によって異なります�?     * @endjapanese
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
     * このシーンのシー�?ID
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
     * 様々なフラ�?     * @endjapanese
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
         * シーンイベントディスパッチャーが定義されていれば 1
         * がセットされます�?         *
         * Bff_Scene_Driver.getHandlers() が空ではなく、かつ、そ�?!--
         * -->呼出しが成功した場合、このデータフィールドに 1 がセット<!--
         * -->されます�?         * @endjapanese
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
         * Bff_Scene_Driver.finalize() をコールする必要がある場合�?
         * がセットされます�?         * @endjapanese
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
         * このシーンが現在のシーンから降格されるときに (=他の異な�?!--
         * -->シーンがこのシーンを追い出して現在のシーンになるとき�?�?!--
         * -->もしくは、このシーンがキャンセルされるときに (=このシーンが<!--
         * -->スケジュールされたシーンとして待機している間に、他の異なる<!--
         * -->シーンがスケジュールされたときに) このシーンを解放する必要<!--
         * -->があれば 1 がセットされます�?         * @endjapanese
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
         * このシーンが何らかのタスクの「現在のシーン」であれ�?1
         * がセットされます�?         * @endjapanese
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
         * このシーンが何らかのタスクの「スケジュールされたシーン」であれ�?         * 1 がセットされます�?         * @endjapanese
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
     * シーン固有データ
     *
     * このデータフィールド�?NULL で初期化されます�?Bff_Scene.setData()
     * でこのデータフィールドに任意の値を設定でき�?Bff_Scene.getData()
     * でその値を取り出すことができます�?     *
     * BFF は、BFF アプリケーションがこのデータフィールドをどのよう�?!--
     * -->使用するかについては関知しません�?     * @endjapanese
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
     * シーンリスト内における、前のシーン
     *
     * BffIm_Scene 実装は、このフィールドがどのように使われるかに<!--
     * -->ついては関知しません�?     * @endjapanese
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
     * シーンリスト内における、次のシーン
     *
     * BffIm_Scene 実装は、このフィールドがどのように使われるかに<!--
     * -->ついては関知しません�?     * @endjapanese
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
     * このシーンが現在関連付けられているタス�?     *
     * このシーンが何らかのタスクの「現在のシーン」もしく�?!--
     * -->「スケジュールされたシーン」である場合、このデータ<!--
     * -->フィールドは、そのタスクへのポインタを保持します�?!--
     * -->それ以外の場合は、このデータフィールドにはヌルポインタが<!--
     * -->設定されます�?     * @endjapanese
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
     * シーン名
     *
     * このデータフィールドは次のように初期化されます�?     *
     * @code
     * SNPRINTF(, , BFFIM_SCENE_NAME_FORMAT, sceneId)
     * @endcode
     *
     * Bff_Scene_Driver.getName() はその初期値を上書きできます�?     * @endjapanese
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
     * このシーンのドライバ�?     * @endjapanese
     */
    Bff_Scene_Driver driver;

    /**
     * @english
     * Event dispatcher of this scene.
     * @endenglish
     *
     * @japanese
     * このシーンのイベントディスパッチャー
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
 * シーンを初期化します�? *
 * @param[out] instance  初期化するシーン
 * @param[in]  kernel    BFF カーネル
 * @param[in]  sceneId   初期化するシーンに割り当てるシー�?ID
 * @param[in]  driver    初期化するシーン用のシーンドライバー
 *
 * @retval SUCCESS   初期化に成功しました�? * @retval EBADPARM  instance, kernel, driver のいずれかもしく�?!--
 *                   -->複数がヌルポインタです�? * @endjapanese
 */
int
BffIm_Scene_initialize(
    BffIm_Scene *instance, BffIm_Kernel *kernel, Bff_Scene_Id sceneId,
    Bff_Scene_Driver *driver);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BFFIM_SCENE_H */
