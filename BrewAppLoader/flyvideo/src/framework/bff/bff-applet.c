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
 * BREW Foundation Framework, Applet.
 *
 * @author Takahiko Kawasaki
 * @file
 */


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEAppGen.h"
#include "AEEShell.h"
#include "bff/bff-accessors.h"
#include "bff/bff-applet.h"
#include "bff/bff-applet-driver.h"
#include "bff/bff-logging.h"
#include "bff/bff-version-bff.h"
#include "bffim/bffim-kernel.h"


/*------------------------------------------------------------------------*
 * MACROS
 *------------------------------------------------------------------------*/
/*
 * Prefixes of log messages.
 *
 * Logging macros such as BFF_ERROR() cannot be used while BFF is
 * not alive (before BffIm_Kernel_initialize() returns and after
 * BffIm_Kernel.finalize() returns). Therefore, while BFF is not
 * alive, log messages need to be written by DBGPRINTF().
 *
 * Prefixes defined below mimic the prefixes of log messages by
 * the BFF logging macros. "BFF:" implies that a log message has
 * come from somewhere in the implementation of BFF.
 */
#define PREFIX_VERBOSE  BFF_LOGGING_PREFIX_VERBOSE "BFF:"
#define PREFIX_DEBUG    BFF_LOGGING_PREFIX_DEBUG   "BFF:"
#define PREFIX_INFO     BFF_LOGGING_PREFIX_INFO    "BFF:"
#define PREFIX_WARN     BFF_LOGGING_PREFIX_WARN    "BFF:"
#define PREFIX_ERROR    BFF_LOGGING_PREFIX_ERROR   "BFF:"
#define PREFIX_FATAL    BFF_LOGGING_PREFIX_FATAL   "BFF:"


/*------------------------------------------------------------------------*
 * STATIC FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
/**
 * Get the size of the memory area that this applet requires.
 *
 * @param[in]  classId  An application class ID.
 * @param[out] size     A place to store the required memory size.
 *
 * @retval SUCCESS   Successfully obtained the size.
 * @retval EBADPARM  'size' is null.
 */
static int
applet_getSize(AEECLSID classId, uint16 *size);


/**
 * Initialize a newly created IApplet instance with BFF driver.
 *
 * @param[in] applet   An IApplet instance to initialize.
 * @param[in] classId  The application class ID of the IApplet instance.
 * @param[in] size     Memory size reserved for the IApplet instance.
 * @param[in] driver   BFF Applet driver to initialize the IApplet
 *                     instance with.
 *
 * @retval SUCCESS   Successfully initialized.
 * @retval EBADPARM  Either or both of 'applet' and 'driver' are null.
 */
static int
applet_initialize(
    IApplet *applet, AEECLSID classId, uint16 size,
    Bff_Applet_Driver *driver);


/**
 * Handle all events delivered from BREW AEE.
 *
 * This function is given to AEEApplet_New() as its 6th argument.
 *
 * @param[in] applet  An IApplet instance.
 * @param[in] ev      An event from BREW AEE.
 * @param[in] w       The first parameter of the event.
 * @param[in] dw      The second parameter of the event.
 *
 * @return  A value that should be returned to BREW AEE. The meaning
 *          of the returned value varies depending on the type of the
 *          event, but basically, TRUE is returned when the event has
 *          been handled by the event handler.
 */
static boolean
applet_handleEvent(void *applet, AEEEvent ev, uint16 w, uint32 dw);


/**
 * Perform finalization for the given IApplet instance.
 *
 * This function is given to AEEApplet_New() as its 7th argument.
 *
 * @param[in] applet  An IApplet instance to finalize.
 */
static void applet_finalize(IApplet *applet);


/*------------------------------------------------------------------------*
 * STATIC FUNCTIONS
 *------------------------------------------------------------------------*/
static int
applet_getSize(AEECLSID classId, uint16 *size)
{
    /*
     * Delegate the calculation of the required memory size to a BFF Kernel
     * function (BffIm_Kernel_getSize()).
     */
    return BffIm_Kernel_getSize(classId, size);
}


static int
applet_initialize(
    IApplet *applet, AEECLSID classId, uint16 size,
    Bff_Applet_Driver *driver)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)applet;


    /*
     * Delegate the initialization of the applet to a BFF Kernel function
     * (BffIm_Kernel_initialize()).
     */
    return BffIm_Kernel_initialize(kernel, classId, size, driver);
}


static boolean
applet_handleEvent(void *applet, AEEEvent ev, uint16 w, uint32 dw)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)applet;


    /*
     * Delegate the event handling to a BFF Kernel method
     * (BffIm_Kernel.handleEvent()).
     */
    return kernel->handleEvent(kernel, ev, w, dw);
}


static void applet_finalize(IApplet *applet)
{
    BffIm_Kernel *kernel = (BffIm_Kernel *)applet;


    /*
     * Delegate the finalization to a BFF Kernel method
     * (BffIm_Kernel.finalize()).
     */
    kernel->finalize(kernel);

    /* Emit the last log message from this applet. */
    DBGPRINTF(PREFIX_INFO "BREW FOUNDATION FRAMEWORK, End.");
    DBGPRINTF(PREFIX_INFO "  class ID = 0x%08lX",
              (unsigned long)BFF_CLASS_ID_OF(kernel));
}


/*------------------------------------------------------------------------*
 * FUNCTIONS
 *------------------------------------------------------------------------*/
int
Bff_Applet_create(
    AEECLSID classId, IShell *shell, IModule *module, void **applet,
    Bff_Applet_Driver *driver)
{
    uint16 size = 0;
    int ret;


    /* Start of BREW Foundation Framework. */
    DBGPRINTF(PREFIX_INFO "BREW FOUNDATION FRAMEWORK, Start.");
    DBGPRINTF(PREFIX_INFO "  version = " BFF_THIS_VERSION_STRING);
    DBGPRINTF(PREFIX_INFO "  class ID = 0x%08lX", (unsigned long)classId);

    /* Check if given arguments are valid. */
    if (!shell || !module || !applet || !driver)
    {
        /* Invalid arguments. */
        DBGPRINTF(PREFIX_VERBOSE "Invalid argument(s).");
        return EBADPARM;
    }

    /* Get the size of the memory area that this applet requires. */
    ret = applet_getSize(classId, &size);
    if (ret != SUCCESS)
    {
        /* Failed to calculate the size for some reasons. */
        return ret;
    }

    /*
     * Assume that applet_getSize() does not return a value bigger
     * than SHRT_MAX (so the value of 'size' is not checked here).
     */

    /*
     * Create an IApplet instance.
     *
     * A newly created IApplet instance is stored at 'applet'.
     * applet_handleEvent() is used as an event handler that
     * accepts all events from BREW AEE. applet_finalize() is
     * used as a finalizer of the IApplet instance.
     */
    ret = (int)AEEApplet_New((int16)size, classId, shell, module,
                             (IApplet **)applet,
                             applet_handleEvent, applet_finalize);
    if (TRUE != (boolean)ret)
    {
        /* Failed to create an IApplet instance. */
        DBGPRINTF(PREFIX_ERROR "AEEApplet_New() failed.");
        DBGPRINTF(PREFIX_ERROR "  classId = 0x%08lX",
                  (unsigned long)classId);

        return ENOMEMORY;
    }


    /* Initialize the new IApplet instance with BFF driver. */
    ret = applet_initialize((IApplet *)*applet, classId, size, driver);
    if (SUCCESS != ret)
    {
        /* Failed to initialize the IApplet instance. */
        IAPPLET_Release((IApplet *)*applet);
        return ret;
    }

    /* Succeeded in creating a new IApplet instance. */
    BFF_DEBUG(("BFF:An IApplet instance has been created."));

    return AEE_SUCCESS;
}
