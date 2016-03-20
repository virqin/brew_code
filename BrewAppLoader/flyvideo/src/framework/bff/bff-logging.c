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
 * BREW Foundation Framework, Logging.
 *
 * @author Takahiko Kawasaki
 * @file
 */


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEStdLib.h"
#include "bff/bff.h"
#include "bff/bff-logging.h"


/*------------------------------------------------------------------------*
 * ENUMS
 *------------------------------------------------------------------------*/
enum
{
    /**
     * BREW API manual says "Each call to DBGPRINTF() generates at most
     * 128 bytes of output".
     */
    Bff_Logging_MAX_NUM_OF_CHARS = 128
};


/*------------------------------------------------------------------------*
 * STATIC FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
static void
doLogging(
    Bff_Logging_Level level, const char *prefix, const char *fmt,
    va_list list);


static void
doPrint(Bff_Logging_Level level, const char *log);


/*------------------------------------------------------------------------*
 * STATIC FUNCTIONS
 *------------------------------------------------------------------------*/
static void
doLogging(
    Bff_Logging_Level level, const char *prefix, const char *fmt,
    va_list list)
{
    Bff *bff = BFF();
    char buf[Bff_Logging_MAX_NUM_OF_CHARS];
    uint32 bufLen;
    uint32 prefixLen;


    if (bff->getLoggingLevel(bff) > level)
    {
        return;
    }

    bufLen    = (uint32)sizeof(buf);
    prefixLen = (uint32)STRLEN(prefix);

    SNPRINTF(buf, bufLen, "%s", prefix);

    if (prefixLen < bufLen)
    {
        VSNPRINTF(buf + prefixLen, bufLen - prefixLen, fmt, list);
    }

    buf[bufLen - 1] = '\0';

    doPrint(level, buf);
}


static void
doPrint(Bff_Logging_Level level, const char *log)
{
#ifdef DBGPRINTF_FATAL
    if (Bff_Logging_Level_FATAL <= level)
    {
        DBGPRINTF_FATAL("%s", log);
    }
    else
    {
        DBGPRINTF("%s", log);
    }
#else /* DBGPRINTF_FATAL */
    DBGPRINTF("%s", log);
#endif /* DBGPRINTF_FATAL */
}


/*------------------------------------------------------------------------*
 * FUNCTIONS
 *------------------------------------------------------------------------*/
void
Bff_Logging_verbose(const char *fmt, ...)
{
    va_list list;


    va_start(list, fmt);

    doLogging(
        Bff_Logging_Level_VERBOSE, BFF_LOGGING_PREFIX_VERBOSE, fmt, list);

    va_end(list);
}


void
Bff_Logging_debug(const char *fmt, ...)
{
    va_list list;


    va_start(list, fmt);

    doLogging(Bff_Logging_Level_DEBUG, BFF_LOGGING_PREFIX_DEBUG, fmt, list);

    va_end(list);
}


void
Bff_Logging_info(const char *fmt, ...)
{
    va_list list;


    va_start(list, fmt);

    doLogging(Bff_Logging_Level_INFO, BFF_LOGGING_PREFIX_INFO, fmt, list);

    va_end(list);
}


void
Bff_Logging_warn(const char *fmt, ...)
{
    va_list list;


    va_start(list, fmt);

    doLogging(Bff_Logging_Level_WARN, BFF_LOGGING_PREFIX_WARN, fmt, list);

    va_end(list);
}


void
Bff_Logging_error(const char *fmt, ...)
{
    va_list list;


    va_start(list, fmt);

    doLogging(Bff_Logging_Level_ERROR, BFF_LOGGING_PREFIX_ERROR, fmt, list);

    va_end(list);
}


void
Bff_Logging_fatal(const char *fmt, ...)
{
    va_list list;


    va_start(list, fmt);

    doLogging(Bff_Logging_Level_FATAL, BFF_LOGGING_PREFIX_FATAL, fmt, list);

    va_end(list);
}
