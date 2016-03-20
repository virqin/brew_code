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
 * BREW Foundation Framework, Task ID.
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, タスク ID
 * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_TASK_ID_H
#define BFF_TASK_ID_H


/*------------------------------------------------------------------------*
 * TYPEDEFS
 *------------------------------------------------------------------------*/
/**
 * @english
 * BFF Task ID
 * @endenglish
 *
 * @japanese
 * BFF タスク ID
 * @endjapanese
 */
typedef int32 Bff_Task_Id;


/*------------------------------------------------------------------------*
 * ENUMS
 *------------------------------------------------------------------------*/
enum
{
    /**
     * @english
     * Invalid task ID.
     * @endenglish
     *
     * @japanese
     * 無効なタスク ID
     * @endjapanese
     */
    Bff_Task_Id_INVALID = -1,

    /**
     * @english
     * Task ID assigned to the first task.
     * @endenglish
     *
     * @japanese
     * 最初のタスクに割り当てられるタスク ID
     * @endjapanese
     *
     * @see Bff_Applet_Driver.getTaskDriver()
     */
    Bff_Task_Id_FIRST = 0
};


#endif /* BFF_TASK_ID_H */
