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
 * BREW Foundation Framework, Variable.
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, 変数
 * @endjapanese
 *
 * @see Bff_Task.control()
 * @see Bff_Task_Driver.control()
 * @see Bff_Scene.control()
 * @see Bff_Scene_Driver.control()
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_VAR_H
#define BFF_VAR_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEComdef.h"
#include "bff/bff-types.h"


/*------------------------------------------------------------------------*
 * UNION DECLARATIONS
 *------------------------------------------------------------------------*/
/*
union Bff_Var;
typedef union Bff_Var Bff_Var;*/



/*------------------------------------------------------------------------*
 * UNIONS
 *------------------------------------------------------------------------*/
/**
 * @english
 * Generic Variable
 * @endenglish
 *
 * @japanese
 * 汎用変数
 * @endjapanese
 */
union Bff_Var
{
    char            c;
    unsigned char   uc;
    short           s;
    unsigned short  us;
    int             i;
    unsigned int    ui;
    long            l;
    unsigned long   ul;
    float           f;
    double          d;
    void *          p;

    boolean         b;
    int8            i8;
    uint8           ui8;
    int16           i16;
    uint16          ui16;
    int32           i32;
    uint32          ui32;
};


#endif /* BFF_VAR_H */
