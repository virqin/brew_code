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
 * BREW Foundation Framework, Scene Driver.
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, シーンドライバー
 * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 * @see @ref scene_switching
 * @see Bff_Scene
 */


#ifndef BFF_SCENE_DRIVER_H
#define BFF_SCENE_DRIVER_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEComdef.h"
#include "bff/bff-types.h"


/*------------------------------------------------------------------------*
 * STRUCT DECLARATIONS
 *------------------------------------------------------------------------*/
/*
struct Bff_Scene_Driver;
typedef struct Bff_Scene_Driver Bff_Scene_Driver;*/



/*------------------------------------------------------------------------*
 * STRUCTS
 *------------------------------------------------------------------------*/
/**
 * @english
 * BFF Scene Driver
 *
 * When BFF creates a new scene, some methods of Bff_Scene_Driver
 * are called (if they are defined) in the following order during
 * the scene creation process.
 *
 *     -# Bff_Scene_Driver.getName()
 *     -# Bff_Scene_Driver.getHandlers()
 *     -# Bff_Scene_Driver.initialize()
 *
 * If the calls of these methods have succeeded, it is assured that
 * Bff_Scene_Driver.finalize() is called later during the finalization
 * process of the scene. Otherwise, Bff_Scene_Driver.finalize() is not
 * called.
 *
 * A scene becomes a 'scheduled' scene when it is scheduled, for
 * example, by Bff_Task.scheduleScene(). In normal cases, a scheduled
 * scene becomes the 'current' scene soon. When a scene is promoted
 * to the current scene, Bff_Scene_Driver.enter() is called if defined.
 * Similarly, when a scene is demoted from the current scene,
 * Bff_Scene_Driver.leave() is called if defined. In some cases, a
 * scheduled scene is cancelled without being promoted to the current
 * scene. In such a case, Bff_Scene_Driver.cancel() is called if defined.
 * @endenglish
 *
 * @japanese
 * BFF シーンドライバー
 *
 * BFF が新しいシーンを作成するとき、シーン作成過程において�?!--
 * -->(もし定義されていれば) Bff_Scene_Driver の幾つかのメソッドが�?!--
 * -->下記の順番で呼び出されます�? *
 *     -# Bff_Scene_Driver.getName()
 *     -# Bff_Scene_Driver.getHandlers()
 *     -# Bff_Scene_Driver.initialize()
 *
 * これらのメソッドの呼び出しが成功した場合、シーンの終了処理の過程�? * Bff_Scene_Driver.finalize() が呼び出されることが保証されます�? * それ以外の場合は�?Bff_Scene_Driver.finalize() は呼ばれません�? *
 * シーンは�?例え�?Bff_Task.scheduleScene() によ�?
 * スケジュールされたときに「スケジュールされた」シーンになります�? * 通常、スケジュールされたシーンはすぐに「現在の」シーンになります�? * シーンが現在のシーンへと昇格されるとき、もしも定義されていれば�? * Bff_Scene_Driver.enter() が呼ばれます。同様にして、シーン�?!--
 * -->現在のシーンから降格させられるときは、もしも定義されていれば�? * Bff_Scene_Driver.leave() が呼ばれます。場合によっては�?!--
 * -->現在のシーンへと昇格されることなく、スケジュールされたシーンが<!--
 * -->キャンセルされることがあります。このような場合、もしも定義<!--
 * -->されていれば�?Bff_Scene_Driver.cancel() が呼ばれます�? * @endjapanese
 *
 * @see @ref scene_switching
 * @see Bff_Scene
 */
typedef struct _Bff_Scene_Driver
{
    /**
     * @english
     * A callback function invoked when the scene is cancelled.
     *
     * A scene becomes a 'scheduled' scene when it is scheduled, for
     * example, by Bff_Task.scheduleScene(). Usually, a scheduled scene
     * is promoted to the 'current' scene soon, but it is possible that
     * another scene is scheduled before the existing scheduled scene
     * becomes the current scene. In such a case, the existing scheduled
     * scene is cancelled and this method is called if defined.
     *
     * @param[in] bff    A BFF instance.
     * @param[in] scene  A scene to be cancelled.
     * @param[in] arg    An arbitrary parameter which was specified when
     *                   this scene was scheduled, for example, by
     *                   Bff_Task.scheduleScene().
     * @param[in] task   A task in which the scene was scheduled.
     * @endenglish
     *
     * @japanese
     * シーンがキャンセルされるときに呼ばれるコールバック関�?     *
     * シーンは�?例え�?Bff_Task.scheduleScene() によ�?
     * スケジュールされたときに「スケジュールされた」シーンになります�?     * 通常、スケジュールされたシーンはすぐに「現在の」シーンへと昇格<!--
     * -->されますが、既存のスケジュールされたシーンが現在のシーンに<!--
     * -->なる前に、他のシーンがスケジュールされることがありえます�?     * この場合、既存のスケジュールされたシーンはキャンセルされ�?!--
     * -->もしも定義されていれば、このメソッドが呼ばれます�?     *
     * @param[in] bff    BFF インスタンス
     * @param[in] scene  キャンセルされるシー�?     * @param[in] arg    このシーンが (例え�?Bff_Task.scheduleScene()
     *                   によ�? スケジュールされたときに指定され�?!--
     *                   -->任意のパラメータ�?     * @param[in] task   シーンがスケジュールされていたタスク
     * @endjapanese
     */
    void (*cancel)(Bff *bff, Bff_Scene *scene, void *arg, Bff_Task *task);

    /**
     * @english
     * Control the scene in scene-driver-specific way.
     *
     * BFF does not care about how the implementation of this method
     * behaves. All BFF does is just call this method when
     * Bff_Scene.control() is called. This is a generic interface that
     * can do anything like %ioctl().
     *
     * BFF Task Driver also has a similar method,
     * Bff_Task_Driver.control().
     *
     * @param[in]     bff    A BFF instance.
     * @param[in]     scene  A scene instance.
     * @param[in]     cmd    A command to this scene. Its value and
     *                       meaning are determined by the scene driver
     *                       implementation.
     * @param[in,out] args   An array of arguments for 'command'. Its
     *                       size, values and meaning are determined by
     *                       the scene driver implementation.
     *
     * @retval SUCCESS       The command was handled successfully.
     * @retval EUNSUPPORTED  The command is not supported.
     * @retval Others        Up to the scene driver.
     * @endenglish
     *
     * @japanese
     * シーンドライバー固有の方法でシーンを制御します�?     *
     * BFF は、このメソッドの実装がどのように動作するかについては<!--
     * -->関知しません。BFF がすることは�?Bff_Scene.control()
     * が呼ばれたときにこのメソッドを呼ぶことだけです。これは�?ioctl()
     * のように、何でもできる汎用のインターフェースです�?     *
     * BFF タスクドライバーにも�?Bff_Task_Driver.control()
     * という同様のメソッドがあります�?     *
     * @param[in]     bff    BFF インスタンス
     * @param[in]     scene  シーンインスタン�?     * @param[in]     cmd    このシーンに対するコマンド。値および<!--
     *                       -->その意味は、シーンドライバーの実装�?!--
     *                       -->より決定されます�?     * @param[in,out] args   command の引数の配列。サイズ、値�?!--
     *                       -->およびその意味は、シーンドライバ�?!--
     *                       -->の実装により決定されます�?     *
     * @retval SUCCESS       コマンドが正常に処理されました�?     * @retval EUNSUPPORTED  コマンドがサポートされていません�?     * @retval その�?       シーンドライバー次第
     * @endjapanese
     *
     * @see Bff_Scene.control()
     * @see Bff_Task.control()
     * @see Bff_Task_Driver.control()
     */
    int (*control)(Bff *bff, Bff_Scene *scene, uint32 cmd, Bff_Var *args);

    /**
     * @english
     * A callback function invoked when the scene is promoted to the
     * current scene and starts working.
     *
     * @param[in] bff    A BFF instance.
     * @param[in] scene  A scene promoted to the current scene.
     * @param[in] prev   A scene that was working previously. This can be
     *                   null.
     * @param[in] arg    An arbitrary parameter which was specified when
     *                   this scene was scheduled, for example, by
     *                   Bff_Task.scheduleScene().
     * @endenglish
     *
     * @japanese
     * シーンが現在のシーンへと昇格され、働き始めるときに呼ばれ�?!--
     * -->コールバック関数
     *
     * @param[in] bff    BFF インスタンス
     * @param[in] scene  現在のシーンへと昇格されるシーン
     * @param[in] prev   その前に動いていたシーン。ヌルポインタという<!--
     *                   -->こともありえます�?     * @param[in] arg    このシーンが (例え�?Bff_Task.scheduleScene()
     *                   によ�? スケジュールされたときに指定され�?!--
     *                   -->任意のパラメータ�?     * @endjapanese
     */
    void (*enter)(Bff *bff, Bff_Scene *scene, Bff_Scene *prev, void *arg);

    /**
     * @english
     * Finalize a scene.
     *
     * This method is called only once per scene. An implementation of this
     * method, however, may be called multiple times if the implementation
     * is shared among multiple scenes.
     *
     * @param[in] bff    A BFF instance.
     * @param[in] scene  A scene to finalize.
     * @endenglish
     *
     * @japanese
     * シーンの終了処理をおこないます�?     *
     * このメソッドは、シーン毎に一回だけ呼ばれます。しかし、こ�?!--
     * -->メソッドの実装は、もしも複数のシーンで共有されているなら�?!--
     * -->複数回呼ばれることになります�?     *
     * @param[in] bff    BFF インスタンス
     * @param[in] scene  終了処理をおこなうシーン
     * @endjapanese
     *
     * @see Bff_Scene_Driver.initialize()
     */
    void (*finalize)(Bff *bff, Bff_Scene *scene);

    /**
     * @english
     * Get a scene name.
     *
     * The name set by this method can be obtained by Bff_Scene.getName().
     *
     * This method is called only once per scene. An implementation of this
     * method, however, may be called multiple times if the implementation
     * is shared among multiple scenes.
     *
     * @param[in]     bff    A BFF instance.
     * @param[in]     scene  A scene instance whose name is given by this
     *                       method.
     * @param[in,out] name   A buffer to store a scene name. The default
     *                       value of scene name is stored there when
     *                       this method is invoked.
     * @param[in]     size   Size of the name buffer.
     *                       (= #Bff_Scene_NAME_BUF_SIZE)
     *
     * @retval SUCCESS  Successfully obtained a scene name.
     * @retval Others   Other values than SUCCESS are regarded as errors.
     * @endenglish
     *
     * @japanese
     * シーンの名前を取得する�?     *
     * このメソッドによりセットされた名前は Bff_Scene.getName()
     * で取得できます�?     *
     * このメソッドは、シーン毎に一回だけ呼ばれます。しかし、こ�?!--
     * -->メソッドの実装は、もしも複数のシーンで共有されているなら�?!--
     * -->複数回呼ばれることになります�?     *
     * @param[in]     bff    BFF インスタンス
     * @param[in]     scene  このメソッドにより名前を与えられるシーン
     * @param[in,out] name   シーン名を格納するバッファ。このメソッドが<!--
     *                       -->呼び出されたときには、シーン名の<!--
     *                       -->デフォルト値が格納されています�?     * @param[in]     size   名前を格納するバッファのサイ�?     *                       (= #Bff_Scene_NAME_BUF_SIZE)
     *
     * @retval SUCCESS  名前の取得に成功しました�?     * @retval その�?  SUCCESS 以外の値はエラーとみなされます�?     * @endjapanese
     *
     * @see Bff_Scene.getName()
     * @see #Bff_Scene_NAME_BUF_SIZE
     */
    int (*getName)(Bff *bff, Bff_Scene *scene, char *name, int size);

    /**
     * @english
     * Set up event handlers.
     *
     * Event handlers set by this method are used only while this scene is
     * working as a current scene of a current task.
     *
     * This method is called only once per scene. An implementation of this
     * method, however, may be called multiple times if the implementation
     * is shared among multiple scenes.
     *
     * @param[in]  bff       A BFF instance.
     * @param[in]  scene     A scene to set up.
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
     * イベントハンドラー群を設定する�?     *
     * このメソッドで設定されるイベントハンドラー群は、このシーンが�?!--
     * -->現在のタスクの現在のシーンとして動いている間だけ、使�?!--
     * -->されます�?     *
     * このメソッドは、シーン毎に一回だけ呼ばれます。しかし、こ�?!--
     * -->メソッドの実装は、もしも複数のシーンで共有されているなら�?!--
     * -->複数回呼ばれることになります�?     *
     * @param[in]  bff       BFF インスタンス
     * @param[in]  scene     設定するシー�?     * @param[out] handlers  イベントハンドラー群。このメソッドが呼ば�?!--
     *                       -->た時点では、handlers の全てのデー�?!--
     *                       -->フィールドにはヌルポインタが設定され�?!--
     *                       -->います。このメソッドの実装で、handlers
     *                       のデータフィールドの幾つかもしくは全てに<!--
     *                       -->関数ポインタをセットします�?     *
     * @retval SUCCESS  イベントハンドラー群の設定に成功しました�?     * @retval その�?  SUCCESS 以外の値はエラーとみなされます�?     * @endjapanese
     */
    int (*getHandlers)(Bff *bff, Bff_Scene *scene, Bff_Handlers *handlers);

    /**
     * @english
     * Initialize a scene.
     *
     * This method is called only once per scene. An implementation of this
     * method, however, may be called multiple times if the implementation
     * is shared among multiple scenes.
     *
     * @param[in] bff    A BFF instance.
     * @param[in] scene  A scene to initialize.
     *
     * @retval SUCCESS  Successfully initialized.
     * @retval Others   Other values than SUCCESS are regarded as errors.
     * @endenglish
     *
     * @japanese
     * シーンの初期化処理をおこないます�?     *
     * このメソッドは、シーン毎に一回だけ呼ばれます。しかし、こ�?!--
     * -->メソッドの実装は、もしも複数のシーンで共有されているなら�?!--
     * -->複数回呼ばれることになります�?     *
     * @param[in] bff    BFF インスタンス
     * @param[in] scene  初期化するシーン
     *
     * @retval SUCCESS  初期化処理に成功しました�?     * @retval その�?  SUCCESS 以外の値はエラーとみなされます�?     * @endjapanese
     *
     * @see Bff_Scene_Driver.finalize()
     */
    int (*initialize)(Bff *bff, Bff_Scene *scene);

    /**
     * @english
     * A callback function invoked when the scene is demoted from the
     * current scene and stops working.
     *
     * Note that methods to schedule a scene (e.g. Bff_Task.scheduleScene())
     * fail unconditionally when they are called from with an implementation
     * of this method.
     *
     * @param[in] bff    A BFF instance.
     * @param[in] scene  A scene demoted from the current scene.
     * @param[in] next   A scene that will be promoted to the current
     *                   scene next. This can be null.
     * @endenglish
     *
     * @japanese
     * シーンが現在のシーンから降格され、働くのをやめるときに呼ばれ�?!--
     * -->コールバック関数
     *
     * シーンをスケジュールするメソッド (例え�?Bff_Task.scheduleScene())
     * は、このメソッドの実装内から呼ばれたときは無条件に失敗します�?     *
     * @param[in] bff    BFF インスタンス
     * @param[in] scene  現在のシーンから降格されたシーン
     * @param[in] next   次に現在のシーンに昇格されるシーン�?     *                   ヌルポインタのこともありえます�?     * @endjapanese
     */
    void (*leave)(Bff *bff, Bff_Scene *scene, Bff_Scene *next);
};


/*------------------------------------------------------------------------*
 * FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @english
 * Clear all data fields in a Bff_Scene_Driver structure.
 *
 * Null pointers are set to all the data fields. It's not a must, but
 * this function is expected to be used as the first step to set up
 * a Bff_Scene_Driver instance before passing the instance to
 * Bff.createScene(), Bff_Task.scheduleNewScene() or
 * Bff_Task.scheduleOneShotScene().
 *
 * @param[out] driver  A BFF scene driver to clear. If 'driver' is null,
 *                     nothing is done.
 * @endenglish
 *
 * @japanese
 * Bff_Scene_Driver 構造体の全データフィールドをクリアします�? *
 * 全てのデータフィールドにヌルポインタがセットされます。必須で�?!--
 * -->ありませんが�?Bff.createScene(), Bff_Task.scheduleNewScene()
 * もしくは Bff_Task.scheduleOneShotScene() に渡す前に�?Bff_Scene_Driver
 * インスタンスを設定する際の最初のステップとして、この関数が<!--
 * -->使用されることが想定されています�? *
 * @param[out] driver  クリアす�?BFF シーンドライバー。driver
 *                     がヌルポインタの場合、何もしません�? * @endjapanese
 *
 * @see Bff.createScene()
 * @see Bff_Task.scheduleNewScene()
 * @see Bff_Task.scheduleOneShotScene()
 */
void
Bff_Scene_Driver_clear(Bff_Scene_Driver *driver);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BFF_SCENE_DRIVER_H */
