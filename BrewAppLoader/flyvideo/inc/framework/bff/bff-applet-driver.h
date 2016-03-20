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
 * BREW Foundation Framework, Applet Driver.
 * @endenglish
 *
 *
 * @author Takahiko Kawasaki
 * @file
 * @see @ref entry_point
 */


#ifndef BFF_APPLET_DRIVER_H
#define BFF_APPLET_DRIVER_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEStdLib.h"
#include "bff/bff-types.h"


/*------------------------------------------------------------------------*
 * STRUCT DECLARATIONS
 *------------------------------------------------------------------------*/
/*
struct _Bff_Applet_Driver;
typedef struct _Bff_Applet_Driver Bff_Applet_Driver;*/



/*------------------------------------------------------------------------*
 * STRUCTS
 *------------------------------------------------------------------------*/
/**
 * @english
 * BFF Applet Driver
 *
 * A BREW application which uses BFF has to call Bff_Applet_create()
 * with an instance of Bff_Applet_Driver. BFF calls methods set in
 * the Bff_Applet_Driver instance as necessary.
 *
 * Methods of Bff_Applet_Driver are not called in the context of
 * AEEClsCreateInstance(), meaning it is guaranteed that they are
 * not called until the very first event is delivered from BREW AEE.
 *
 * When the first event is delivered from BREW AEE, BFF calls methods
 * listed below (if they are defined) in this order during its
 * initialization process.
 *
 *     -# Bff_Applet_Driver.getPreHandlers()
 *     -# Bff_Applet_Driver.getPostHandlers()
 *     -# Bff_Applet_Driver.initialize()
 *     -# Bff_Applet_Driver.isMultiTaskingEnabled()
 *
 * If the calls of the first three methods have succeeded, it is
 * assured that Bff_Applet_Driver.finalize() is called later during
 * BFF's finalization process. This means, for example, if
 * Bff_Applet_Driver.initialize() has failed,
 * Bff_Applet_Driver.finalize() will not be called.
 * @endenglish
 *
 *
 * @see @ref entry_point
 */
typedef struct _Bff_Applet_Driver
{
    /**
     * @english
     * Perform applet-wide finalization.
     *
     * This method is called in the context of the function that has been
     * given to AEEApplet_New() as the 7th argument. This means that BREW
     * event dispatching has already finished when this method is invoked.
     *
     * @param[in] bff  A BFF instance.
     * @endenglish
     *
     *
     * @see Bff_Applet_Driver.initialize()
     */
    void (*finalize)(Bff *bff);

    /**
     * @english
     * Free a memory area allocated by Bff_Applet_Driver.malloc().
     *
     * If implementations of Bff_Applet_Driver.free() and
     * Bff_Applet_Driver.malloc() are both given, they are used in the
     * implementations of Bff.free() and Bff.malloc(), respectively.
     * Otherwise, BFF's default implementations of %free() and %malloc()
     * are used.
     *
     * @param[in] bff   A BFF instance.
     * @param[in] area  A memory area to free.
     * @endenglish
     *
     *
     * @see Bff.free()
     * @see Bff.malloc()
     * @see Bff_Mem_free()
     * @see Bff_Mem_malloc()
     * @see Bff_Applet_Driver.malloc()
     */
    void (*free)(Bff *bff, void *area);

    /**
     * @english
     * Set up applet-level post event handlers.
     *
     * A BFF-based applet can have two sets of applet-level event handlers.
     * One is called "Applet Pre Event Handlers" and the other is called
     * "Applet Post Event Handlers". This method sets up the latter.
     *
     * This method is called only once when the first event is delivered
     * from BREW AEE.
     *
     * @param[in]  bff       A BFF instance.
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
     * @see Bff_Applet_Driver.getPreHandlers()
     * @see @ref event_dispatching
     */
    int (*getPostHandlers)(Bff *bff, Bff_Handlers *handlers);

    /**
     * @english
     * Set up applet-level pre event handlers.
     *
     * A BFF-based applet can have two sets of applet-level event handlers.
     * One is called "Applet Pre Event Handlers" and the other is called
     * "Applet Post Event Handlers". This method sets up the former.
     *
     * This method is called only once when the first event is delivered
     * from BREW AEE.
     *
     * @param[in]  bff       A BFF instance.
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
     *
     * @see Bff_Applet_Driver.getPostHandlers()
     * @see @ref event_dispatching
     */
    int (*getPreHandlers)(Bff *bff, Bff_Handlers *handlers);

    /**
     * @english
     * Set up a task driver.
     *
     * This method is called when BFF tries to create a new task.
     * If multi-tasking support is not enabled, this method is called
     * at most once. An implementation of this method can make all
     * tasks share the same task driver, but it should be noted that
     * it is also possible to set a different task driver per task.
     *
     * @param[in]  bff     A BFF instance.
     * @param[in]  taskId  A task ID that will be assigned to a new task.
     *                     #Bff_Task_Id_FIRST is given if the task creation
     *                     has been triggered by an EVT_APP_START event
     *                     (not by EVT_APP_RESUME events).
     * @param[out] driver  A task driver. All data fields of 'driver' are
     *                     null pointers when this method is invoked. An
     *                     implementation of this method is expected to
     *                     set function pointers to some or all of data
     *                     fields of 'driver'.
     *
     * @retval SUCCESS  Successfully set up a driver.
     * @retval Others   Other values than SUCCESS are regarded as errors.
     * @endenglish
     *
     *
     * @see Bff_Applet_Driver.isMultiTaskingEnabled()
     * @see @ref multitasking
     */
    int (*getTaskDriver)(
        Bff *bff, Bff_Task_Id taskId, Bff_Task_Driver *driver);

    /**
     * @english
     * Perform applet-wide initialization.
     *
     * This method is called only once when the very first BREW event
     * is delivered from BREW AEE. Note that EVT_APP_START is not always
     * the first event.
     *
     * @param[in] bff  A BFF instance.
     *
     * @retval SUCCESS  Successfully initialized.
     * @retval Others   Other values than SUCCESS are regarded as errors.
     * @endenglish
     *
     *
     * @see Bff_Applet_Driver.finalize()
     */
    int (*initialize)(Bff *bff);

    /**
     * @english
     * Query whether multi-tasking support is requested.
     *
     * If this method is implemented and TRUE is returned from the
     * implementation, multi-tasking support is enabled. In such a case,
     * multiple instances of one identical applet can run simultaneously.
     *
     * This method is called only once when the first event is delivered
     * from BREW AEE.
     *
     * @param[in] bff  A BFF instance.
     *
     * @retval TRUE   Multi-tasking support is requested.
     * @retval FALSE  Multi-tasking support is not needed.
     * @endenglish
     *
     *
     * @see Bff_Applet_Driver.getTaskDriver()
     * @see @ref multitasking
     */
    boolean (*isMultiTaskingEnabled)(Bff *bff);

    /**
     * @english
     * Allocate a memory area.
     *
     * If implementations of Bff_Applet_Driver.free() and
     * Bff_Applet_Driver.malloc() are both given, they are used in the
     * implementations of Bff.free() and Bff.malloc(), respectively.
     * Otherwise, BFF's default implementations of %free() and %malloc()
     * are used.
     *
     * @param[in] bff   A BFF instance.
     * @param[in] size  Size of a memory area to allocate.
     *
     * @return  A pointer to a newly allocated memory area. A null pointer
     *          should be returned if memory allocation failed.
     * @endenglish
     *
     * @see Bff.malloc()
     * @see Bff.free()
     * @see Bff_Mem_malloc()
     * @see Bff_Mem_free()
     * @see Bff_Applet_Driver.free()
     */
    void * (*malloc)(Bff *bff, size_t size);
};


/*------------------------------------------------------------------------*
 * FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @english
 * Clear all data fields in a Bff_Applet_Driver structure.
 *
 * Null pointers are set to all the data fields. It's not a must, but
 * this function is expected to be used as the first step to set up
 * a Bff_Applet_Driver instance before passing the instance to
 * Bff_Applet_create().
 *
 * @param[out] driver  A BFF applet driver to clear. If 'driver' is null,
 *                     nothing is done.
 * @endenglish
 *
 *
 * @see Bff_Applet_create()
 */
void
Bff_Applet_Driver_clear(Bff_Applet_Driver *driver);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BFF_APPLET_DRIVER_H */
