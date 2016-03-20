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
 * Utility macros to handle version information.
 * @endenglish
 *
 * @japanese
 * バージョン情報を扱うためのユーティリティマクロ
 * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_VERSION_COMMON_H
#define BFF_VERSION_COMMON_H


/*------------------------------------------------------------------------*
 * MACROS
 *------------------------------------------------------------------------*/
/**
 * @english
 * Build a packed representation of a version number.
 *
 * Build a packed representation of a version number from a given set of
 * a major, minor, micro and extra version numbers.
 *
 * A packed representation is easy to be compared to others.
 *
 * @param[in] major     Major version number
 * @param[in] minor     Minor version number
 * @param[in] micro     Micro version number
 * @param[in] extra     Extra version number
 *
 * @return  A packed representation of a version number.
 * @endenglish
 *
 * @japanese
 * バージョン番号のパックされた表現形式を作る
 *
 * 指定された、メジャー番号、マイナー番号、マイクロ番号、エクストラ番号<!--
 * -->の組から、パックされた表現形式を作る。
 *
 * パックされた形式は他と簡単に比較することができます。
 *
 * @param[in] major     メジャーバージョン番号
 * @param[in] minor     マイナーバージョン番号
 * @param[in] micro     マイクロバージョン番号
 * @param[in] extra     エクストラバージョン番号
 *
 * @return  バージョン番号のパックされた表現形式
 * @endjapanese
 */
#define BFF_VERSION_PACK(major,minor,micro,extra) \
    (((major) << 24) | ((minor) << 16) | ((micro) << 8) | (extra))


#define BFF__STR(x)  #x


/**
 * @english
 * Convert a version number into a string.
 *
 * Convert a given set of a major, minor, micro and exra version numbers
 * into one string.
 *
 * @param[in] major     Major version number
 * @param[in] minor     Minor version number
 * @param[in] micro     Micro version number
 * @param[in] extra     Extra version number
 *
 * @return  A string representation of a version number.
 * @endenglish
 *
 * @japanese
 * バージョン番号を文字列に変換する。
 *
 * 指定された、メジャー番号、マイナー番号、マイクロ番号、エクストラ番号<!--
 * -->の組を、文字列表現へと変換する。
 *
 * @param[in] major     メジャーバージョン番号
 * @param[in] minor     マイナーバージョン番号
 * @param[in] micro     マイクロバージョン番号
 * @param[in] extra     エクストラバージョン番号
 *
 * @return  バージョン番号の文字列表現
 * @endjapanese
 */
#define BFF_VERSION_STRING(major,minor,micro,extra) \
    BFF__STR(major)"."BFF__STR(minor)"."BFF__STR(micro)"."BFF__STR(extra)


#endif /* BFF_VERSION_COMMON_H */
