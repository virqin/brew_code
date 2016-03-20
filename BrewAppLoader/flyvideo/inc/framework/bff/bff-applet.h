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
 * BREW Foundation Framework, Applet.
 * @endenglish
 *
 *
 * @author Takahiko Kawasaki
 * @file
 * @see @ref entry_point
 */


#ifndef BFF_APPLET_H
#define BFF_APPLET_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEShell.h"
#include "bff/bff-applet-driver.h"


/*------------------------------------------------------------------------*
 * FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @english
 * Create a BFF-based IApplet instance.
 *
 * This function is the entry point of BREW Foundation Framework.
 * It is expected that a BREW application calls this function from
 * within the implementation of AEEClsCreateInstance() like below.
 *
 * @code
 * int
 * AEEClsCreateInstance(
 *     AEECLSID classId, IShell *shell, IModule *module, void **object)
 * {
 *     Bff_Applet_Driver driver;
 *
 *
 *     // Prepare BFF Applet driver.
 *     Bff_Applet_Driver_clear(&driver);
 *     driver.initialize = my_applet_driver_initialize;
 *     ......;
 *
 *     // Create a BFF-based IApplet instance.
 *     return Bff_Applet_create(classId, shell, module, object, &driver);
 * }
 * @endcode
 *
 * The implementation of this function calls AEEApplet_New().
 * This implies a BREW application which calls Bff_Applet_create()
 * needs to be linked with object files created from AEEAppGen.c
 * and AEEModGen.c both of which are contained in BREW SDK as
 * sample programs.
 *
 * @param[in]  classId  An application class ID.
 * @param[in]  shell    A shell instance.
 * @param[in]  module   A module instance.
 * @param[out] applet   A place to store a newly created IApplet instance.
 * @param[in]  driver   BFF Applet driver. The content of 'driver' is
 *                      copied, so the caller does not have to keep the
 *                      content of 'driver' after this function returns.
 *
 * @retval AEE_SUCCESS    An IApplet instance was successfully created.
 * @retval AEE_EBADPARM   One or more of 'shell', 'module', 'applet'
 *                        and 'driver' are null.
 * @retval AEE_ENOMEMORY  AEEApplet_New() that had been called from
 *                        within the implementation of this function
 *                        failed.
 * @retval Others         Other error codes may be returned by BFF.
 * @endenglish
 *
 *
 *
 *
 * @see @ref entry_point
 */
int
Bff_Applet_create(
    AEECLSID classId, IShell *shell, IModule *module, void **applet,
    Bff_Applet_Driver *driver);


#ifdef __cplusplus
}
#endif /* __cplusplus */


/**
 * @english
 * @page entry_point Entry Point
 * <ol>
 * <li>@ref entry_point__traditional_entry_point_impl
 * <li>@ref entry_point__problems_in_typical_impl
 * <li>@ref entry_point__entry_point_of_bff
 * </ol>
 *
 *
 * @section entry_point__traditional_entry_point_impl Traditional Entry Point Implementation
 * A BREW application needs to implement \c \b IModule interface and
 * \c \b IApplet interface, but usually, BREW application programmers
 * do not implement them from scratch. Instead, they use sample
 * implementations of the interfaces, \c AEEModGen.c and \c AEEAppGen.c,
 * which come along with BREW SDK.
 *
 * The sample \c IModule implementation calls \c \b AEEClsCreateInstance()
 * function which is supposed to be implemented by each BREW application.
 * This means that \c <span style="text-decoration: underline;"
 * >AEEClsCreateInstance() is the entry point for BREW applications</span>.
 * Its function prototype is defined in \c AEEModGen.h as below.
 *
 * @code
 * int
 * AEEClsCreateInstance(
 *     AEECLSID classId, IShell *shell, IModule *module, void **object);
 * @endcode
 *
 * An implementation of \c AEEClsCreateInstance() is expected to create
 * an object which implements \c IApplet interface and return the created
 * object to the caller via the \c object pointer. The sample \c IApplet
 * implementation supplies a function to do the job. Its name is
 * \c \b AEEApplet_New() and its function prototype is defined in
 * \c AEEModGen.h as below.
 *
 * @code
 * boolean
 * AEEApplet_New(
 *     int16 size, AEECLSID classId, IShell *shell, IModule *module,
 *     IApplet **applet, AEEHANDLER eventHandler, PFNFREEAPPDATA finalizer);
 * @endcode
 *
 * \c AEEApplet_New() creates an object which implements \c IApplet
 * interface and returns the created object to the caller via the \c
 * applet pointer. Among the arguments, \c classId, \c shell, \c module
 * and \c applet, are supposed to be the ones that have been passed as
 * arguments of \c AEEClsCreateInstance(). In other words, the arguments
 * should be passed to \c AEEApplet_New() without change like below.
 *
 * @code
 * int
 * AEEClsCreateInstance(
 *     AEECLSID classId, IShell *shell, IModule *module, void **object)
 * {
 *     ......
 *
 *     ret = AEEApplet_New(
 *               size, classId, shell, module, (IApplet **)object,
 *               eventHandler, finalizer);
 *
 *     ......
 * }
 * @endcode
 *
 * On the other hand, the arguments, \c size, \c eventHandler and \c
 * finalizer, are supposed to be application-specific ones. \c
 * eventHandler is used as an implementation of \c IApplet_HandleEvent(),
 * and \c finalizer is called later in the finalization process of the
 * applet. These functions need to be implemented by each BREW application
 * that uses \c AEEApplet_New(). Their types are defined in \c AEE.h and
 * \c AEEAppGen.h respectively as shown below.
 *
 * @code
 * typedef boolean
 * (*AEEHANDLER)(void *data, AEEEvent ev, uint16 w, uint32 dw);
 * @endcode
 *
 * @code
 * typedef void
 * (*PFNFREEAPPDATA)(IApplet *);
 * @endcode
 *
 * The first argument of \c AEEApplet_New(), \c size, is supposed to be
 * the size of an application-specific structure which extends \c
 * AEEApplet structure. Here, '\e extends' means that a derived structure
 * contains a base structure as its first data field. In an example code
 * shown below, a derived structure, \c MyApp, extends a base structure,
 * \c AEEApplet.
 *
 * @code
 * typedef struct
 * {
 *     AEEApplet applet;
 *     ......
 * }
 * MyApp;
 * @endcode
 *
 * In summary, a typical implementation of the entry point in the
 * traditional way will look like below.
 *
 * @code
 * int
 * AEEClsCreateInstance(
 *     AEECLSID classId, IShell *shell, IModule *module, void **object)
 * {
 *     boolean ret;
 *
 *     // Create an object which implements IApplet interface.
 *     ret = AEEApplet_New(
 *               sizeof(MyApp), classId, shell, module, (IApplet **)object,
 *               myHandleEvent, myFinalize);
 *     if (ret == FALSE)
 *     {
 *         return AEE_EFAILED;
 *     }
 *
 *     // Execute application-specific initialization.
 *     ret = myInitialize((MyApp *)*object);
 *     if (ret == FALSE)
 *     {
 *         IAPPLET_Release((IApplet *)*object);
 *         return AEE_EFAILED;
 *     }
 *
 *     // Successfully created an application instance.
 *     return AEE_SUCCESS;
 * }
 *
 * static boolean
 * myInitialize(MyApp *app)
 * {
 *     // Execute application-specific initialization.
 *     return TRUE;
 * }
 *
 * static void
 * myFinalize(IApplet *applet)
 * {
 *     // 'applet' can be cast to the application-specific structure.
 *     MyApp *app = (MyApp *)applet;
 *
 *     // Execute application-specific finalization.
 * }
 *
 * static boolean
 * myHandleEvent(void *applet, AEEEvent ev, uint16 w, uint32 dw)
 * {
 *     // 'applet' can be cast to the application-specific structure.
 *     MyApp *app = (MyApp *)applet;
 *
 *     // Implement application-specific event dispatching.
 *     switch (ev)
 *     {
 *         ......
 *     }
 *
 *     return FALSE;
 * }
 * @endcode
 *
 *
 * @section entry_point__problems_in_typical_impl Problems in Typical Implementation
 * The typical implementation may not seem so bad. However, once you start
 * to think of developing variants of the application or another new
 * application, you will soon find that the typical implementation lacks
 * reusability. A likely but stopgap measure against the problem would
 * be to introduce many per-variant or per-application \c \#ifdef's into
 * the shared single data strucutre, initializer, finalizer and event
 * handler (\c MyApp, \c myInitialize, \c myFinalize and \c myHandleEvent
 * in the above sample), or to create a separate file which has yet another
 * \c AEEClsCreateInstance() implementation.
 *
 * The most sinful point is that BREW AEE does not undertake but leaves
 * all event dispatching work to a single, application-defined event
 * handler. Other modern platforms than BREW provide sophisticated
 * mechanisms to deliver events to appropriate objects depending on
 * application states, but in BREW, application programmers must
 * implement all the event dispatching themselves. As a result, programmers
 * who simply follow Qualcomm's tutorials and/or BREW books available are
 * liable to write a huge \c switch block in their single event handler
 * (\c myHandleEvent in the above sample) and endlessly add many flags
 * each of which represents a various kind of application state into the
 * single data structure (\c MyApp in the above sample). This leads to
 * <b>state explosion</b> and causes BREW applications to become almost
 * impossible to maintain.
 *
 * If there are a mechanism to switch implementations of data structure,
 * initializer, finalizer and event handler easily per variant and/or
 * per application without changing shared time-proven code and a
 * mechanism to dispatch events depending on application states, they
 * will surely help BREW application programmers out of the cruel world.
 * And, yes, BREW Foundation Framework provides the mechanisms.
 *
 *
 * @section entry_point__entry_point_of_bff Entry Point of BFF
 * A BREW application which uses BREW Foundation Framework is supposed to
 * call Bff_Applet_create() instead of AEEApplet_New(). Its function
 * prototype is as follows.
 *
 * @code
 * int
 * Bff_Applet_create(
 *     AEECLSID classId, IShell *shell, IModule *module, void **applet,
 *     Bff_Applet_Driver *driver);
 * @endcode
 *
 * An instance of Bff_Applet_Driver must be passed to Bff_Applet_create().
 * Bff_Applet_Driver determines all behaviors of an application.
 *
 * Bff_Applet_Driver consists of function pointers. A BFF application
 * should set up some or all of the function pointers properly on an
 * as-needed basis. BFF does not report any error even if all the
 * function pointers are null pointers, but in such a case, the application
 * won't be launched successfully because no event handler which responds
 * to the EVT_APP_START event has been registered. Therefore, in practice,
 * at least one of the following function pointers needs to be set in order
 * to register an event handler to respond to EVT_APP_START.
 * (Bff_Applet_Driver.getTaskDriver() cannot register an event handler
 * directly but can do it indirectly.)
 *
 *     - Bff_Applet_Driver.getPreHandlers()
 *     - Bff_Applet_Driver.getPostHandlers()
 *     - Bff_Applet_Driver.getTaskDriver()
 *
 * <span style="text-decoration: underline;"><b>BFF provides a layered,
 * per-task and per-state event dispatching mechanism</b></span>, meaning
 * that there are multiple places where event handlers can be registered.
 * Details of the event dispatching mechanism are described in
 * @ref event_dispatching and so they are not mentioned any further here.
 *
 * Regarding all the other function pointers of Bff_Applet_Driver, please
 * refer to the description about Bff_Applet_Driver.
 *
 * An entry point using Bff_Applet_create() will be implemented like below.
 *
 * @code
 * int
 * AEEClsCreateInstance(
 *     AEECLSID classId, IShell *shell, IModule *module, void **object)
 * {
 *     Bff_Applet_Driver driver;
 *
 *
 *     // Prepare BFF Applet driver.
 *     Bff_Applet_Driver_clear(&driver);
 *     driver.initialize = my_applet_driver_initialize;
 *     ......;
 *
 *     // Create a BFF-based IApplet instance.
 *     return Bff_Applet_create(classId, shell, module, object, &driver);
 * }
 * @endcode
 *
 * For working examples, please refer to @ref sample_programs.
 * @endenglish
 */


#endif /* BFF_APPLET_H */
