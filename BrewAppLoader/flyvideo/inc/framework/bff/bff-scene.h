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
 * BREW Foundation Framework, シー�? * @endjapanese
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
     * シーン名用バッファのサイ�?     *
     * この値は、シーン名を格納するバッファのサイズとして�?     * Bff_Scene_Driver.getName() に渡されます�?     * @endjapanese
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
 * BFF シー�? *
 * BFF シーンは、アプリケーションの状態の一つに対応付けられることを<!--
 * -->意図しています。BFF シーンには、イベントハンドラー群のセットを一�?!--
 * -->持たせることができます。これは、一つのアプリケーション状態毎に<!--
 * -->それ固有のイベントハンドラー群を持たせることができるということを<!--
 * -->意味します。この仕組みにより、BFF アプリケーションプログラマは�?!--
 * -->自然とステートパターン・プログラミングを行うようになります�? *
 * BFF シーンは�?Bff.createScene() により作成できます。BFF により�?!--
 * -->ID と名前が、新しい BFF シーンに自動的に割り当てられます�?!--
 * -->ID は一意であり、変更できませんが、名前は�? * Bff_Scene_Driver.getName() を実装することにより、上書きすること�?!--
 * -->できます。複数の BFF シーンが、同じ名前を持つことは許され�?!--
 * -->います。BFF は名前の重複をチェックしません�? *
 * BFF シーンを動かすためには、タスクと関連付ける必要があります。BFF
 * シーンを BFF タスクと関連付けるのには、下記のメソッドを使います�? *
 *   - Bff_Task.scheduleScene()
 *   - Bff_Task.scheduleSceneById()
 *   - Bff_Task.scheduleSceneByName()
 *
 * これらのメソッドは、BFF シーンを「スケジュールされた」シーンとし�?!--
 * -->受け付けます。スケジュールされたシーンは、後ほど「現在の」シーン<!--
 * -->へと昇格させられます。BFF シーンが働き始めるのは、そ�?BFF
 * シーンが現在のシーンになったときです�? *
 * BFF は、終了処理の過程で、存在している全ての BFF シーンを解放<!--
 * -->しますが、RAM を節約するためにも、使用しなくなった時点で�? * Bff.releaseScene() によ�?BFF シーンを解放することをお勧めします�? *
 * Bff_Task.scheduleNewScene() は、ショートカットメソッドで�?!--
 * -->「BFF シーンを作成する」、「そのシーンをスケジュールする」という<!--
 * -->作業をおこないます�?Bff_Task.scheduleOneShotScene() も同じことを<!--
 * -->おこないますが、追加で「仕事を終えたときにそのシーンを解放する�?!--
 * -->という作業もおこないます�? *
 * BFF シーンがスケジュールされるとき、もしも他の BFF シーンが<!--
 * -->既にスケジュールされたシーンとして対象の BFF タスクにより<!--
 * -->保持されていた場合、その既存のスケジュールされたシーン�?!--
 * -->キャンセルされます。キャンセルされるシーンのシーンドライバーが
 * Bff_Scene_Driver.cancel() の実装を持っている場合、そ�?%cancel()
 * メソッドが呼ばれます。シーンがスケジュールされたときに指定された<!--
 * -->引数�?Bff_Scene_Driver.cancel() に渡されます�? *
 * スケジュールされたシーンが現在のシーンへと昇格される際は常に�? * もし定義されていれば�?Bff_Scene_Driver.enter() が呼ばれます�? * シーンがスケジュールされたときに指定された引数が�? * Bff_Scene_Driver.enter() に渡されます�? *
 * 他の BFF シーンが現在のシーンへと昇格され、動いている現在�?!--
 * -->シーンを追い出すとき、追い出されるシーン�?Bff_Scene_Driver.leave()
 * が呼ばれます。もしも追い出されるシーンが Bff_Task.scheduleOneShotScene()
 * によりスケジュールされたものであれば�?Bff_Scene_Driver.leave()
 * の呼出しが終わったあとで、そのシーンは解放されます�? *
 * もし定義されていれば、シーンが解放されるとき�? * Bff_Scene_Driver.finalize() が呼ばれます�? * @endjapanese
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
     * シーンドライバー固有の方法でシーンを制御します�?     *
     * このメソッドの動作は�?Bff_Scene_Driver.control()
     * の実装に依存します�?     *
     * BFF タスクにも�?Bff_Task.control() という同様のメソッドがあります�?     *
     * @param[in]     self     シーンインスタン�?     * @param[in]     command  このシーンに対するコマンド。値および<!--
     *                         -->その意味は、シーンドライバーの実装�?!--
     *                         -->より決定されます�?     * @param[in,out] args     command の引数の配列。サイズ、値�?!--
     *                         -->およびその意味は、シーンドライバ�?!--
     *                         -->の実装により決定されます�?     *
     * @retval SUCCESS       コマンドが正常に処理されました�?     * @retval EBADPARM      self がヌルポインタです�?     * @retval EUNSUPPORTED  コマンドがサポートされていません。こ�?!--
     *                       -->エラーコードは�?Bff_Scene_Driver.control()
     *                       が定義されていない場合、もしくは、実装が<!--
     *                       -->このエラーコードを返す場合に、返されます�?     * @retval その�?       シーンドライバーの実装により、その他�?!--
     *                       -->エラーコードが返されることがあります�?     * @endjapanese
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
     * シーン固有のデータを取得します�?     *
     * 最後の Bff_Scene.setData() の呼出しでセットされたデータが返されます�?     * BREW Foundation Framework は�?Bff_Scene.setData() がどんな<!--
     * -->データをセットしていたかについては関知しません�?     *
     * 必須というわけではありませんが、想定される使用方法は�?     * Bff_Scene_Driver.initialize() の実装内�?     * (もしくはアプリケーションにとって都合のよい他の場所�?
     * 独自のデータ構造体 (例え�?struct MySceneData) 用のメモ�?!--
     * -->領域を確保し、そのメモリ領域へのポインタ�?Bff_Scene.setData()
     * でセットすることです。そのポインタの値は、後から Bff_Scene.getData()
     * により取り出すことができます。当該データ構造体は、シーン単位�?!--
     * -->データとして扱うことができます�?     *
     * Bff.getData(), Bff_Task.getData() という似たメソッドがあります�?!--
     * -->、データのスコープがそれぞれ異なるので注意してください�?     *
     * @param[in] self  シーンインスタン�?     *
     * @return  シーン固有データ。最後の Bff_Scene.setData() によ�?!--
     *          -->セットされた値が返されます�?Bff_Scene.setData() �?!--
     *          -->それまでに呼び出されていない場合、ヌルポインタが<!--
     *          -->返されます。self がヌルポインタの場合、ヌルポイン�?!--
     *          -->が返されます�?     * @endjapanese
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
     * このシーンのシー�?ID を取得します�?     *
     * @param[in] self  シーンインスタン�?     *
     * @return  このシーンのシー�?ID。self がヌルポインタの場合�?     *          #Bff_Scene_Id_INVALID が返されます�?     * @endjapanese
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
     * このシーンの名前を取得します�?     *
     * Bff_Scene_Driver.getName() で、シーン名のデフォルト値を<!--
     * -->上書きすることができます�?     *
     * @param[in] self  シーンインスタン�?     *
     * @return  このシーンの名前。self がヌルポインタの場合�?!--
     *          -->ヌルポインタが返されます�?     * @endjapanese
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
     * このシーンを保持しているタスクを取得します�?     *
     * @param[in] self  シーンインスタン�?     *
     * @return  このシーンが、何らかのタスクの現在のシーンもしく�?!--
     *          -->スケジュールされたシーンである場合、そのタスク�?!--
     *          -->ポインタが返されます。それ以外の場合は、ヌ�?!--
     *          -->ポインタが返されます。self がヌルポインタの<!--
     *          -->場合は、ヌルポインタが返されます�?     * @endjapanese
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
     * このシーンがあるタスクの「現在のシーン」であるかどうかを調べます�?     *
     * シーンは、下記の二つのステップのあとに「現在のシーン」になります�?     *
     *   -# Bff_Task.scheduleScene() (もしくは同様の他のメソッ�?
     *      により、シーンがスケジュールされる。これにより、シーン�?!--
     *      -->そのタスクの「スケジュールされたシーン」になる�?     *   -# そのタスクがシーン切り替えを行い、スケジュールされた<!--
     *      -->シーンを「現在のシーン」へと昇格させる�?     *
     * @param[in] self  シーンインスタン�?     *
     * @retval TRUE   このシーンは、あるタスクの現在のシーンとして<!--
     *                -->動いている�?     * @retval FALSE  このシーンは、どのタスクの現在のシーンではない�?     *                self がヌルポインタの場合、FALSE が返されます�?     * @endjapanese
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
     * このシーンがあるタスクの「スケジュールされたシーン」である�?!--
     * -->どうかを調べます�?     *
     * シーンは�?Bff_Task.scheduleScene() (もしくは同様の他のメソッ�?
     * によりスケジュールされたときに、「スケジュールされたシーン�?!--
     * -->になります�?     *
     * @param[in] self  シーンインスタン�?     *
     * @retval TRUE   このシーンは、スケジュールされたシーンとして�?!--
     *                -->なんらかのタスクに関連付けられている�?     * @retval FALSE  このシーンは、スケジュールされたシーンとしては�?!--
     *                -->なんのタスクにも関連付けられていない�?     * @endjapanese
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
     * シーン固有のデータをセットします�?     *
     * このメソッドでセットしたデータは、後から Bff_Scene.getData()
     * により取り出すことができます�?     *
     * Bff.setData(), Bff_Task.setData() という似たメソッドがあります�?!--
     * -->、データのスコープがそれぞれ異なるので注意してください�?     *
     * @param[in] self  シーンインスタン�?     * @param[in] data  シーン固有データ
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
 * @page scene_switching シーン切り替�? * <ol>
 * <li>@ref scene_switching__state_pattern
 * <li>@ref scene_switching__bff_scene
 * <li>@ref scene_switching__lifecycle
 * </ol>
 *
 *
 * @section scene_switching__state_pattern ステートパターン
 * イベントに対する BREW アプリケーションの反応は、アプリケーションの<!--
 * -->状態に応じて変わります。例えば、クリアキーイベントは、状況に<!--
 * -->応じて�?a)アプリケーションを終了させる�?b)メニューを閉じる�?!--
 * -->(c)前の画面に戻る、などの異なる動作を引き起こすかもしれません�? *
 * アプリケーションの状態毎�?BREW イベントへの反応を変えるための最悪な<!--
 * -->実装は、下記の例のようになるでしょう。このコードは、最初に<!--
 * -->イベントコー�?(AEEEvent) をもとにイベントの振り分けをおこない�?!--
 * -->次に \c switch ブロックをもう一度書いてキーコー�?(AVKType)
 * をもとにイベントの振り分けをおこない、最後にアプリケーションの状態を<!--
 * -->チェックします�? *
 * @code
 * static boolean
 * myHandleEvent(MyApp *app, AEEEvent event, uint16 w, uint32 dw)
 * {
 *     // 最初に、イベントコード (AEEEvent) をもとにイベントを振り分ける�? *     switch (event)
 *     {
 *         case EVT_KEY:
 *             // 次に、キーコード (AVKType) をもとにイベントを振り分ける�? *             switch (w)
 *             {
 *                 case AVK_CLR:
 *                     // 最後に、アプリケーションの状態をチェックする�? *                     switch (app->applicationState)
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
 * この最悪なコードの根本原因は、ただ一つのイベントハンドラ�?(上記の例<!--
 * -->における \c myHandleEvent()) で全てのイベントを処理しているという<!--
 * -->点にあります�? *
 * ソフトウェアデザインパターンの一つである�?b>ステートパターン</b>�?!--
 * -->は、この手の問題を解決するための適切なプログラミング手法です�? * 上記の大きな \c switch ブロックにステートパターンを適用すると�?!--
 * -->各アプリケーション状態がそれぞれイベントハンドラーを持つこと�?!--
 * -->なるでしょう�? * 下記は、この概念を示す擬似コードです�? *
 * @code
 * typedef struct
 * {
 *     // アプリケーション状態毎のイベントハンドラ�? *     boolean (*handleEvent)(MyApp *app, AEEEvent ev, uint16 w, uint32 dw);
 * }
 * State;
 *
 * static boolean
 * myHandleEvent(MyApp *app, AEEEvent ev, uint16 w, uint32 dw)
 * {
 *     // 現在の状態を取得する�? *     State *state = getCurrentState(...);
 *
 *     // 状態に特化したイベントハンドラーを起動する�? *     return state->handleEvent(app, ev, w, dw);
 * }
 *
 * static boolean
 * stateA_handleEvent(MyApp *app, AEEEvent ev, uint16 w, uint32 dw)
 * {
 *     // この関数は、他のアプリケーション状態に煩わされることなく�? *     // 状態Ａに特化した作業に集中できる�? *     ......
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
 * @section scene_switching__bff_scene BFF シー�? * ステートパターンプログラミングをサポートするため、BFF �?<b>BFF シー�? * </b> (Bff_Scene) を提供します。BFF シーンは、イベントに対して独自の<!--
 * -->方法で反応しなければならないアプリケーション状態の一つに対応する<!--
 * -->ことを意図しています。BFF シーンは、自分用のイベントハンドラー�?!--
 * -->セットを持つことができます�? *
 * BFF シーンを作成する方法は幾つかあります。基本的なやり方は、下記に<!--
 * -->示す関数プロトタイプを持�?Bff.createScene() メソッドを呼ぶことです�? *
 * @code
 * int (*createScene)(
 *     Bff *self, Bff_Scene **scene, Bff_Scene_Driver *driver);
 * @endcode
 *
 * 関数プロトタイプが示すように、BFF シーンを作成するためには�?b>BFF
 * シーンドライバー</b> (Bff_Scene_Driver) を渡す必要があります。BFF
 * シーンドライバーは、BFF シーンの動作を決定します。特に、BFF
 * シーンのイベントハンドラー群を設定するためには�? * Bff_Scene_Driver.getHandlers() を実装する必要があります�? *
 *
 * @section scene_switching__lifecycle BFF シーンのライフサイク�? * <div style="margin: 0 20px 20px 20px; float: right;">
 * @image html bff-scene-lifecycle.png "BFF シー�?ライフサイク�?
 * </div>
 *
 * 作成しただけでは、BFF シーンは動き始めません。BFF シーンが動き始め�?!--
 * -->には�?1)スケジュールされ�?2)現在のシーンへと昇格される必要が<!--
 * -->あります�? *
 * BFF シーンをスケジュールする方法には幾つかあります。基本的なやり方は�? * Bff_Task.scheduleScene() メソッドを呼ぶことです。スケジュールされた<!--
 * -->シーンは、シーンの切り替えが Bff_Task.blockSceneSwitching()
 * で無効にされていない限り、すぐに (正確には BFF カーネルのポスト�?!--
 * -->イベントディスパッチャーが実行されるとき�? 現在のシーンへと昇格<!--
 * -->されます�? *
 * BFF シーンが現在のシーンへと昇格されるとき、もしもそのシーン用�?!--
 * -->シーンドライバー�?Bff_Scene_Driver.enter() をセットしていれば�?!--
 * -->そのメソッドが実行されます。同様に、BFF シーンが現在のシーンから<!--
 * -->降格されるとき、もしもセットされていれ�?Bff_Scene_Driver.leave()
 * メソッドが呼ばれます。右の図は、BFF シーンのライフサイクルを示し�?!--
 * -->います�?enter() 等の図中のメソッド名は、対応する処理が実行され�?!--
 * -->ときに呼ばれ�?Bff_Scene_Driver メソッドです。例えば、BFF シーンが<!--
 * -->作成されるときに次の三つのメソッドが呼ばれることを、図は示しています�? *
 *   -# Bff_Scene_Driver.getName()
 *   -# Bff_Scene_Driver.getHandlers()
 *   -# Bff_Scene_Driver.initialize()
 *
 * Bff_Scene_Driver メソッドについての詳細な記述については�? * Bff_Scene_Driver �?Bff_Scene の説明を参照してください�? * 動作する例については、@ref sample_program__scene_switching
 * を参照してください�? * @endjapanese
 */


#endif /* BFF_SCENE_H */
