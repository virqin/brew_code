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
 * BREW Foundation Framework, Stringifier.
 * @endenglish
 *
 * @japanese
 * BREW Foundation Framework, 文字列表現取得
 * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_STRINGIFIER_H
#define BFF_STRINGIFIER_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEShell.h"


/*------------------------------------------------------------------------*
 * FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @english
 * Stringify AEEEvent.
 *
 * @param[in] event  A value of AEEEvent.
 *
 * @return  A string expression of the given value. A null pointer is
 *          returned if the given value is unknown to this function.
 * @endenglish
 *
 * @japanese
 * AEEEvent の文字列表現を得る。
 *
 * @param[in] event  AEEEvent の値
 *
 * @return  渡された値の文字列表現。未知の値が渡された場合、<!--
 *          -->ヌルポインタが返されます。
 * @endjapanese
 */
const char *
Bff_Stringifier_stringifyEvent(AEEEvent event);


/**
 * @english
 * Stringify AVKType.
 *
 * @param[in] key  A value of AVKType.
 *
 * @return  A string expression of the given value. A null pointer is
 *          returned if the given value is unknown to this function.
 * @endenglish
 *
 * @japanese
 * AVKType の文字列表現を得る。
 *
 * @param[in] key  AVKType の値
 *
 * @return  渡された値の文字列表現。未知の値が渡された場合、<!--
 *          -->ヌルポインタが返されます。
 * @endjapanese
 */
const char *
Bff_Stringifier_stringifyKey(AVKType key);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BFF_STRINGIFIER_H */
