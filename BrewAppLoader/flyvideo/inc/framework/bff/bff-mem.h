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
 * BREW Foundation Framework, Memory utilities.
 *
 * The implementations of the functions listed in this header file
 * call Bff.malloc() and/or Bff.free().
 *
 * Bff.malloc() and Bff.free() call Bff_Applet_Driver.malloc() and
 * Bff_Applet_Driver.free() if they are defined.
 *
 * If a BREW application wants to control memory allocation and
 * deallocation, it can do it by supplying implementations of
 * Bff_Applet_Driver.malloc() and Bff_Applet_Driver.free().
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, メモリユーティリティ
 *
 * このヘッダファイルにリストされている関数群の実装では、
 * Bff.malloc() 及び Bff.free() が使用されます。
 *
 * Bff.malloc() と Bff.free() は、もしも定義されていれば、
 * Bff_Applet_Driver.malloc() と Bff_Applet_Driver.free()
 * を使用します。
 *
 * もしも BREW アプリケーションが、メモリの確保と解放を制御したい場合、
 * Bff_Applet_Driver.malloc() と Bff_Applet_Driver.free() の実装を<!--
 * -->用意することにより、それをおこなうことができます。
 * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_MEM_H
#define BFF_MEM_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEStdLib.h"


/*------------------------------------------------------------------------*
 * FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @english
 * Allocate a memory area.
 *
 * @param[in] size  Size of a newly allocated memory area.
 *
 * @return  A pointer to a newly allocated memory area. A null pointer is
 *          returned if memory allocation failed.
 * @endenglish
 *
 * @japanese
 * メモリ領域を確保します。
 *
 * @param[in] size  新しく確保するメモリ領域のサイズ
 *
 * @return  新しく確保されたメモリ領域のポインタ。メモリの確保に失敗<!--
 *          -->した場合は、ヌルポインタが返されます。
 * @endjapanese
 */
void *
Bff_Mem_malloc(size_t size);


/**
 * @english
 * Free a memory area.
 *
 * @param[in] area  Memory area to free.
 * @endenglish
 *
 * @japanese
 * メモリ領域を解放する。
 *
 * @param[in] area  解放するメモリ領域。
 * @endjapanese
 */
void
Bff_Mem_free(void *area);


/**
 * @english
 * Create a duplicate of a given string.
 *
 * @param[in] str  A source string to duplicate.
 *
 * @return  A pointer to a newly duplicated string. A null pointer is
 *          returned if memory allocation failed or 'str' is null.
 * @endenglish
 *
 * @japanese
 * 与えられた文字列の複製を作成します。
 *
 * @param[in] str  複製元の文字列
 *
 * @return  新しく複製された文字列へのポインタ。メモリ確保に失敗<!--
 *          -->した場合、もしくは str がヌルポインタだった場合、<!--
 *          -->ヌルポインタが返されます。
 * @endjapanese
 */
char *
Bff_Mem_strdup(const char *str);


/**
 * @english
 * Create a duplicate of a given string.
 *
 * @param[in] str  A source string to duplicate.
 *
 * @return  A pointer to a newly duplicated string. A null pointer is
 *          returned if memory allocation failed or 'str' is null.
 * @endenglish
 *
 * @japanese
 * 与えられた文字列の複製を作成します。
 *
 * @param[in] str  複製元の文字列
 *
 * @return  新しく複製された文字列へのポインタ。メモリ確保に失敗<!--
 *          -->した場合、もしくは str がヌルポインタだった場合、<!--
 *          -->ヌルポインタが返されます。
 * @endjapanese
 */
AECHAR *
Bff_Mem_wstrdup(const AECHAR *str);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BFF_MEM_H */
