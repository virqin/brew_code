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
 * BREW Foundation Framework, Types.
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, åž? * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_TYPES_H
#define BFF_TYPES_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "bff/bff-scene-id.h"
#include "bff/bff-task-id.h"


/*------------------------------------------------------------------------*
 * STRUCT/UNION DECLARATIONS
 *------------------------------------------------------------------------*/
struct _Bff;
typedef struct _Bff Bff;


struct _Bff_Applet_Driver;
typedef struct _Bff_Applet_Driver Bff_Applet_Driver;


struct _Bff_Dispatcher;
typedef struct _Bff_Dispatcher Bff_Dispatcher;


struct _Bff_Handlers;
typedef struct _Bff_Handlers Bff_Handlers;


struct _Bff_Scene;
typedef struct _Bff_Scene Bff_Scene;


struct _Bff_Scene_Driver;
typedef struct _Bff_Scene_Driver Bff_Scene_Driver;


struct _Bff_Task;
typedef struct _Bff_Task Bff_Task;


struct _Bff_Task_Driver;
typedef struct _Bff_Task_Driver Bff_Task_Driver;


union _Bff_Var;
typedef union _Bff_Var Bff_Var;


#endif /* BFF_TYPES_H */
