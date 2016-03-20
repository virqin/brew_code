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
 * BREW Foundation Framework, Event Dispatcher.
 * @endenglish
 *
 *
 * @author Takahiko Kawasaki
 * @file
 * @see @ref event_dispatching
 */


#ifndef BFF_DISPATCHER_H
#define BFF_DISPATCHER_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "bff/bff-handlers.h"
#include "bff/bff-types.h"


/*------------------------------------------------------------------------*
 * STRUCT DECLARATIONS
 *------------------------------------------------------------------------*/
/*
struct Bff_Dispatcher;
typedef struct Bff_Dispatcher Bff_Dispatcher;*/



/*------------------------------------------------------------------------*
 * STRUCTS
 *------------------------------------------------------------------------*/
/**
 * @english
 * BFF Event Dispatcher
 *
 * This generic event dispatcher was developed just for internal use by
 * BFF implementation. However, as it is generic enough, the interface
 * of this event dispatcher is exposed to BFF applications, too.
 *
 * Below is a sample code to dispatch a BREW event.
 *
 * @code
 * Bff *bff = BFF();
 * Bff_Handlers handlers;
 * Bff_Dispatcher dispatcher;
 * Bff_Dispatcher *d;
 * boolean consumed = FALSE;
 * boolean ret;
 *
 * // Set up event handlers.
 * Bff_Handlers_clear(&handlers);
 * handlers.onAppStart = myOnAppStart;
 * handlers.onAppStop  = myOnAppStop;
 *
 * // Set up an event dispatcher.
 * Bff_Dispatcher_initialize(&dispatcher, &handlers);
 *
 * // Dispatch a BREW event.
 * d   = &dispatcher;
 * ret = d->dispatch(d, bff, &event, &wParam, &dwParam, &consumed);
 * @endcode
 * @endenglish
 *
 *
 * @see @ref event_dispatching
 */
typedef struct _Bff_Dispatcher
{
    /**
     * @english
     * Dispatch a BREW event.
     *
     * Process a BREW event by the handlers that have been specified as
     * the second argument of Bff_Dispatcher_initialize().
     *
     * If there is no special reason, TRUE should be set to 'consumed'
     * before this method is called, because normal event handlers do
     * and should assume that the default value of 'consumed' is TRUE.
     *
     * Below is the flow of event dispatching.
     *
     *   -# If 'preFixup' is defined, execute it.
     *     -# If 'preFixup' has consumed the event, jump to [4].
     *        Otherwise, go to the next step.
     *   -# If 'preDispatch' is defined, execute it.
     *     -# If 'preDispatch' has consumed the event, jump to [4].
     *   -# If 'onXxx' corresponding to the event is defined, execute it.
     *   -# If 'postDispatch' is defined, execute it.
     *   -# If 'postFixup' is defined, execute it.
     *
     * Basically, a value return by an event handler that has consumed
     * the event is used as the return value of this method. Read the
     * implementation of this method to know the exact behavior of
     * this method.
     *
     * @param[in]     self      Dispatcher to dispatch the event.
     * @param[in]     bff       BREW Foundation Framework.
     * @param[in,out] ev        Event code of the event.
     * @param[in,out] w         The first parameter of the event.
     * @param[in,out] dw        The second parameter of the event.
     * @param[out]    consumed  TRUE if the event has been consumed by an
     *                          event handler of this event dispatcher.
     *                          Otherwise, FALSE.
     *
     * @return  A boolean value to be returned to BREW AEE. Its meaning
     *          varies depending on the value of 'ev'.
     * @endenglish
     *
     *
     * @see @ref event_dispatching
     */
    boolean (*dispatch)(
        Bff_Dispatcher *self, Bff *bff, AEEEvent *ev, uint16 *w, uint32 *dw,
        boolean *consumed);

    /**
     * @english
     * Event handlers used to dispatch a BREW event.
     *
     * The content of this variable is initialized by the second argument
     * of Bff_Dispatcher_initialize().
     * @endenglish
     *
     */
    Bff_Handlers handlers;
};


/*------------------------------------------------------------------------*
 * FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @english
 * Initialize an event dispatcher.
 *
 * @param[in] instance  Event dispatcher to initialize.
 * @param[in] handlers  Event handlers to use when events are dispatched.
 *
 * @retval SUCCESS   Successfully initialized.
 * @retval EBADPARM  Either or both of the arguments are null.
 * @endenglish
 *
 *
 * @see Bff_Handlers
 * @see Bff_Dispatcher.dispatch()
 * @see @ref event_dispatching
 */
int
Bff_Dispatcher_initialize(Bff_Dispatcher *instance, Bff_Handlers *handlers);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/**
 * @english
 * @page event_dispatching Event Dispatching
 * <ol>
 * <li>@ref event_dispatching__generic_event_dispatcher
 * <li>@ref event_dispatching__layered_event_dispatching
 * <li>@ref event_dispatching__use_cases
 * </ol>
 *
 *
 * @section event_dispatching__generic_event_dispatcher Generic Event Dispatcher
 * First of all, an explanation about a generic event dispatcher implemented
 * in BFF goes here.
 *
 * BREW AEE delivers various events to a BREW application. Each event has
 * an event code (AEEEvent), a word parameter (uint16) and a double-word
 * parameter (uint32).
 *
 * Meanings and actual data types of the two event parameters vary depending
 * on event codes. For example, if an event code is EVT_KEY, the first
 * parameter (the word parameter) is a key code (AVKType) and the second
 * one (the double-word parameter) is a collection of bit flags mainly about
 * modifier keys. Therefore, a normal event dispatcher will have a big \c
 * switch block which calls a different sub function per event code like
 * below.
 *
 * @code
 * static boolean
 * myHandleEvent(void *applet, AEEEvent ev, uint16 w, uint32)
 * {
 *     switch (ev)
 *     {
 *         case EVT_APP_START:
 *             return onAppStart(applet, w, (AEEAppStart *)dw);
 *
 *         case EVT_APP_STOP:
 *             return onAppStop(applet, (boolean *)dw);
 *
 *         ......
 *
 *         default:
 *             return FALSE;
 *     }
 * }
 * @endcode
 *
 * Although some BREW applications write detailed code directly right under
 * each \c case label in \c switch block without calling lower-level
 * functions, even in such terrible applications, overall flows of BREW
 * event dispatcher implementations are all alike.
 *
 * BFF has generalized this common flow into a reusable class named
 * Bff_Dispatcher. An instance of Bff_Dispatcher is initialized by
 * Bff_Dispatcher_initialize() with an instance of Bff_Handlers.
 * Bff_Handlers is a collection of function pointers each of which
 * corresponds to one BREW event code.
 *
 * As you expect, the Bff_Dispatcher implementation has a big \c switch
 * block like below (an excerpt from bff-dispatcher.c with some
 * modifications).
 *
 * @code
 * static boolean
 * dispatchPerEvent(
 *     Bff_Handlers *handlers, Bff *bff, AEEEvent ev, uint16 w, uint32 dw,
 *     boolean *consumed)
 * {
 *     switch (ev)
 *     {
 *         case EVT_APP_START:
 *             if (handlers->onAppStart)
 *                 return handlers->onAppStart(
 *                            bff, w, (AEEAppStart *)dw, consumed);
 *             break;
 *
 *         case EVT_APP_STOP:
 *             if (handlers->onAppStop)
 *                 return handlers->onAppStop(bff, (boolean *)dw, consumed);
 *             break;
 *
 *         ......
 *
 *         default:
 *             if (ev >= EVT_USER)
 *             {
 *                 if (handlers->onUser)
 *                 {
 *                     return handlers->onUser(bff, ev, w, dw, consumed);
 *                 }
 *             }
 *             else
 *             {
 *                 if (handlers->onOther)
 *                 {
 *                     return handlers->onOther(bff, ev, w, dw, consumed);
 *                 }
 *             }
 *             break;
 *     }
 *
 *     *consumed = FALSE;
 * 
 *     return FALSE;
 * }
 * @endcode
 *
 * What makes Bff_Dispatcher reusable is the point that implementations of
 * actual behaviors executed on each event are specified from \e outside
 * the Bff_Dispatcher implementation through a Bff_Handlers instance.
 * This means that the Bff_Dispatcher implementation does not have to be
 * statically linked with functions which implement actual behaviors and
 * also that actual behaviors can be replaced even at runtime. But, still,
 * Bff_Dispatcher can carry out its responsibility, event dispatching.
 *
 * Below is a sample code using Bff_Dispatcher.
 *
 * @code
 * Bff *bff = BFF();
 * Bff_Handlers handlers;
 * Bff_Dispatcher dispatcher;
 * Bff_Dispatcher *d;
 * boolean consumed = FALSE;
 * boolean ret;
 *
 * // Set up event handlers.
 * Bff_Handlers_clear(&handlers);
 * handlers.onAppStart = myOnAppStart;
 * handlers.onAppStop  = myOnAppStop;
 *
 * // Set up an event dispatcher.
 * Bff_Dispatcher_initialize(&dispatcher, &handlers);
 *
 * // Dispatch a BREW event.
 * d   = &dispatcher;
 * ret = d->dispatch(d, bff, &event, &wParam, &dwParam, &consumed);
 * @endcode
 *
 * In reality, Bff_Dispatcher does a bit more than guessed. The following
 * four methods in Bff_Handlers have special meanings and are treated
 * differently from other onXxx() methods.
 *
 *   - Bff_Handlers.preFixup()
 *   - Bff_Handlers.preDispatch()
 *   - Bff_Handlers.postDispatch()
 *   - Bff_Handlers.postFixup()
 *
 * Bff_Handlers.preFixup() can be used to change values of event parameters
 * (and even of event code) before an event is passed to an onXxx()
 * function. Bff_Handlers.preDispatch() cannot change values of event
 * parameters but can cut in at the beginning of every event dispatching
 * as Bff_Handlers.preFixup() can. Bff_Handlers.postFixup() and
 * Bff_Handlers.postDispatch() do the similar things at the last stage of
 * one event dispatching process.
 *
 * Please refer to the implementation of Bff_Dispatcher (bff-dispatcher.c)
 * in order to get the exact understanding about how these methods are
 * handled. Please make the most of the fact that BFF is open source
 * software <span style="font-family: monospace;">:-)</span>
 *
 *
 * @section event_dispatching__layered_event_dispatching Layered Event Dispatching
 * BFF has a <b>layered</b> event dispatching mechanism where a BREW event
 * is handled by multiple event dispatchers and continues to be handled
 * until one of the dispatchers consumes the event or until the chain
 * of event dispatchers reaches its end. There are four layers of event
 * dispatchers as listed below.
 *
 *   -# BFF Kernel layer
 *   -# Applet layer
 *   -# BFF Task layer
 *   -# BFF Scene layer
 *
 * All layers except BFF Scene layer have two event dispatchers,
 * respectively. One is <b>Pre Event Dispatcher</b> and the other is
 * <b>Post Event Dispatcher</b>. That is, 7 event dispatchers in total
 * listed below are involved in one event dispatching process.
 *
 * <div style="margin-left: 2em;">
 * <table border=0 cellpadding=5 cellspacing=0
 *        style="border-collapse: collapse; border: 1px solid black;">
 *   <tr style="border-bottom: 1px solid black; background: #E0FFFF;">
 *     <td style="border-right: 1px solid black;">1</td>
 *     <td><b>Pre</b></td>
 *     <td>Event Dispatcher of</td>
 *     <td>BFF</td>
 *     <td><b>Kernel</b></td>
 *     <td>layer</td>
 *   </tr>
 *   <tr style="border-bottom: 1px solid black; background: #87CEEB;">
 *     <td style="border-right: 1px solid black;">2</td>
 *     <td><b>Pre</b></td>
 *     <td>Event Dispatcher of</td>
 *     <td></td>
 *     <td><b>Applet</b></td>
 *     <td>layer</td>
 *   </tr>
 *   <tr style="border-bottom: 1px solid black; background: #00BFFF;">
 *     <td style="border-right: 1px solid black;">3</td>
 *     <td><b>Pre</b></td>
 *     <td>Event Dispatcher of</td>
 *     <td>BFF</td>
 *     <td><b>Task</b></td>
 *     <td>layer</td>
 *   </tr>
 *   <tr style="border-bottom: 1px solid black; background: #4169E1;">
 *     <td style="border-right: 1px solid black;">4</td>
 *     <td></td>
 *     <td>Event Dispatcher of</td>
 *     <td>BFF</td>
 *     <td><b>Scene</b></td>
 *     <td>layer</td>
 *   </tr>
 *   <tr style="border-bottom: 1px solid black; background: #00BFFF;">
 *     <td style="border-right: 1px solid black;">5</td>
 *     <td><b>Post</b></td>
 *     <td>Event Dispatcher of</td>
 *     <td>BFF</td>
 *     <td><b>Task</b></td>
 *     <td>layer</td>
 *   </tr>
 *   <tr style="border-bottom: 1px solid black; background: #87CEEB;">
 *     <td style="border-right: 1px solid black;">6</td>
 *     <td><b>Post</b></td>
 *     <td>Event Dispatcher of</td>
 *     <td></td>
 *     <td><b>Applet</b></td>
 *     <td>layer</td>
 *   </tr>
 *   <tr style="background: #E0FFFF;">
 *     <td style="border-right: 1px solid black;">7</td>
 *     <td><b>Post</b></td>
 *     <td>Event Dispatcher of</td>
 *     <td>BFF</td>
 *     <td><b>Kernel</b></td>
 *     <td>layer</td>
 *   </tr>
 * </table>
 * </div>
 *
 * The order of event dispatchers shown above has a meaning. A BREW event
 * is passed to event dispatchers in this order. However, there is a rule
 * to be noted about post event dispatchers. In general, a post event
 * dispatcher is always executed regardless of whether the target event
 * has already been consumed or not by its corresponding pre event
 * dispatcher unless a pre event dispatcher of the \e upper layer has
 * consumed the event and decided to skip lower layers.
 *
 * Details of the event dispatching flow are as follows.
 *
 *  -# <b>Pre Event Dispatcher of BFF Kernel</b> receives an BREW event.
 *     If the dispatcher consumes the event, jump to [7]. Otherwise,
 *     go to the next step.\n\n
 *  -# <b>Pre Event Dispatcher of Applet</b> receives the BREW event if the
 *     dispatcher is given (Bff_Applet_Driver.getPreHandlers() needs to
 *     be implemented to enable this event dispatcher). If the dispatcher
 *     consumes the event, jump to [6]. Otherwise, go to the next step.\n\n
 *  -# <b>Pre Event Dispatcher of BFF Task</b> receives the BREW event
 *     if the conditions below meet. If the dispatcher consumes the event,
 *     jump to [5]. Otherwise, go to the next step.\n\n
 *      -# the current task exists (this condition is true if the
 *       application has already received EVT_APP_START) and\n\n
 *      -# the dispatcher of the task is given
 *         (Bff_Task_Driver.getPreHandlers() needs to be implemented by
 *         the task driver (Bff_Task_Driver) dedicated to the task to
 *         enable this event dispatcher).\n\n
 *  -# <b>Event Dispatcher of BFF Scene</b> receives the BREW event if the
 *     conditions below meet. Regardless of whether the event dispatcher
 *     has consumed the event or not, go to the next step.\n\n
 *      -# the current scene exists (this condition is true if a non-null
 *         BFF scene was scheduled by Bff_Task.scheduleScene() or equivalent
 *         and the scene has been promoted to the current scene and it is
 *         running as the current scene of the BFF task) and\n\n
 *      -# the dispatcher of the current scene is given
 *         (Bff_Scene_Driver.getHandlers() needs to be implemented by the
 *         scene driver (Bff_Scene_Driver) dedicated to the scene to enable
 *         this event dispatcher).\n\n
 *  -# <b>Post Event Dispatcher of BFF Task</b> receives the BREW event
 *     if the dispatcher is given (Bff_Task_Driver.getPostHandlers() needs
 *     to be implemented by the task driver (Bff_Task_Driver) dedicated to
 *     the task to enable this event dispatcher). Regardless of whether the
 *     event dispatcher has consumed the event or not, go to to the next
 *     step.\n\n
 *  -# <b>Post Event Dispatcher of Applet</b> receives the BREW event if
 *     the dispatcher is given (Bff_Applet_Driver.getPostHandlers() needs
 *     to be implemented to enable this event dispatcher). Regardless of
 *     whether the event dispatcher has consumed the event or not, go to
 *     the next step.\n\n
 *  -# <b>Post Event Dispatcher of BFF Kernel</b> receives the BREW event.
 *
 * Basically, a return value from an event handler that has consumed
 * the event is used as the return value from this entire event
 * dispatching process. However, if it is necessary, post event
 * dispatchers can overwrite the return value by marking the event
 * as consumed and returning a desirable boolean value.
 *
 * Please refer to the implementation of BFF Kernel (bffim-kernel.c),
 * especially, the implementation of BffIm_Kernel.handleEvent() method
 * (= method_kernel_handleEvent() function), to get the exact understanding
 * about this event dispatching process.
 *
 *
 * @section event_dispatching__use_cases Use Cases
 * - If you want to turn on the backlight <em>every time</em> a key is
 *   pressed regardless of which task is running and which scene is
 *   running, the code should be implemented in the applet-level event
 *   dispatcher. Applet-level event dispatchers can be set up by
 *   Bff_Applet_Driver.getPreHandlers() and/or
 *   Bff_Applet_Driver.getPostHandlers().
 *
 * @code
 * int
 * AEEClsCreateInstance(
 *     AEECLSID classId, IShell *shell, IModule *module, void **object)
 * {
 *     Bff_Applet_Driver driver;
 *
 *     Bff_Applet_Driver_clear(&driver);
 *     driver.getPreHandlers = applet_getPreHandlers;
 *     ......
 *
 *     return Bff_Applet_create(classId, shell, module, object, &driver);
 * }
 *
 * static int
 * applet_getPreHandlers(Bff *bff, Bff_Handlers *handlers)
 * {
 *     handlers->onKeyPress = applet_onKeyPress;
 *     return SUCCESS;
 * }
 *
 * static boolean
 * applet_onKeyPress(
 *     Bff *bff, AVKType key, uint32 modifiers, boolean *consumed)
 * {
 *     IDISPLAY_Backlight(BFF_DISPLAY_OF(bff), TRUE);
 *     return (*consumed = FALSE);
 * }
 * @endcode
 *
 * - If the first task has to convert \c AVK_TXPGUP and \c AVK_TXPGDOWN
 *   to \c AVK_UP and \c AVK_DOWN unconditionally but other tasks should
 *   not do it, the code should be implemented in the task-level event
 *   dispatcher. Task-level event dispatchers can be set up by
 *   Bff_Task_Driver.getPreHandlers() and/or
 *   Bff_Task_Driver.getPostHandlers(). Note that
 *   Bff_Applet_Driver.getTaskDriver has to be set in order to set up
 *   Bff_Task_Driver.
 *
 * @code
 * int
 * AEEClsCreateInstance(
 *     AEECLSID classId, IShell *shell, IModule *module, void **object)
 * {
 *     Bff_Applet_Driver driver;
 *
 *     Bff_Applet_Driver_clear(&driver);
 *     driver.getTaskDriver = applet_getTaskDriver;
 *     ......
 *
 *     return Bff_Applet_create(classId, shell, module, object, &driver);
 * }
 *
 * static int
 * applet_getTaskDriver(
 *     Bff *bff, Bff_Task_Id taskId, Bff_Task_Driver *driver)
 * {
 *     if (taskId == Bff_Task_Id_FIRST)
 *     {
 *         driver->getPreHandlers = firstTask_getPreHandlers;
 *         ......
 *     }
 *     else
 *     {
 *         ......
 *     }
 *
 *     return SUCCESS;
 * }
 *
 * static int
 * firstTask_getPreHandlers(
 *     Bff *bff, Bff_Task *task, uint16 flags, AEEAppStart *appStart,
 *     Bff_Handlers *handlers)
 * {
 *     handlers->preFixup = firstTask_preFixup;
 *     ......
 *
 *     return SUCCESS;
 * }
 *
 * static boolean
 * firstTask_preFixup(
 *     Bff *bff, AEEEvent *ev, uint16 *w, uint32 *dw, boolean *consumed)
 * {
 *     switch (ev)
 *     {
 *         case EVT_KEY:
 *         case EVT_KEY_PRESS:
 *         case EVT_KEY_RELEASE:
 *             convertKeyCode((AVKType *)w);
 *             break;
 *
 *         default:
 *             break;
 *     }
 *
 *     return (*consumed = FALSE);
 * }
 *
 * static void
 * convertKeyCode(AVKType *key)
 * {
 *     switch (*key)
 *     {
 *         case AVK_TXPGUP:
 *             *key = AVK_UP;
 *             break;
 *
 *         case AVK_TXPGDOWN:
 *             *key = AVK_DOWN;
 *             break;
 *
 *         default:
 *             break;
 *     }
 * }
 * @endcode
 * @endenglish
 */




#endif /* BFF_DISPATCHER_H */
