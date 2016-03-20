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
 * BREW Foundation Framework, Memory utilities.
 *
 * @author Takahiko Kawasaki
 * @file
 */


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEStdLib.h"
#include "bff/bff.h"
#include "bff/bff-accessors.h"
#include "bff/bff-mem.h"


/*------------------------------------------------------------------------*
 * STATIC FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
static void *
bff_malloc(size_t size);


static void
bff_free(void *area);


/*------------------------------------------------------------------------*
 * STATIC FUNCTIONS
 *------------------------------------------------------------------------*/
static void *
bff_malloc(size_t size)
{
    Bff *bff = BFF();

    return bff->malloc(bff, size);
}


static void
bff_free(void *area)
{
    Bff *bff = BFF();

    bff->free(bff, area);
}


/*------------------------------------------------------------------------*
 * FUNCTIONS
 *------------------------------------------------------------------------*/
void *
Bff_Mem_malloc(size_t size)
{
    return bff_malloc(size);
}


void
Bff_Mem_free(void *area)
{
    bff_free(area);
}


char *
Bff_Mem_strdup(const char *str)
{
    char *newStr;
    size_t len;


    if (str == NULL)
    {
        return NULL;
    }

    len = STRLEN(str);

    newStr = bff_malloc(sizeof(char) * (len + 1));
    if (newStr == NULL)
    {
        return NULL;
    }

    MEMCPY(newStr, str, sizeof(char) * (len + 1));

    return newStr;
}


AECHAR *
Bff_Mem_wstrdup(const AECHAR *str)
{
    AECHAR *newStr;
    size_t len;


    if (str == NULL)
    {
        return NULL;
    }

    len = WSTRLEN(str);

    newStr = bff_malloc(sizeof(AECHAR) * (len + 1));
    if (newStr == NULL)
    {
        return NULL;
    }

    MEMCPY(newStr, str, sizeof(AECHAR) * (len + 1));

    return newStr;
}
