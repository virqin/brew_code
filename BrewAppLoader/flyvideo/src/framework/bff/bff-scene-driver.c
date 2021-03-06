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
 * BREW Foundation Framework, Scene Driver.
 *
 * @author Takahiko Kawasaki
 * @file
 */


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEStdLib.h"
#include "bff/bff-scene-driver.h"


/*------------------------------------------------------------------------*
 * FUNCTIONS
 *------------------------------------------------------------------------*/
void
Bff_Scene_Driver_clear(Bff_Scene_Driver *driver)
{
    if (driver == NULL)
    {
        return;
    }

    /*
     * Strictly speaking, this is not technically correct, but this
     * implementation assumes that internal representations of null
     * pointers are zero.
     */
    MEMSET(driver, 0, sizeof(Bff_Scene_Driver));
}
