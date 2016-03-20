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
 * BREW version information.
 * @endenglish
 *
 * @japanese
 * BREW バージョン情報
 * @endjapanese
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_VERSION_BREW_H
#define BFF_VERSION_BREW_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "BREWVersion.h"
#include "bff/bff-version-common.h"


/*------------------------------------------------------------------------*
 * MACROS
 *------------------------------------------------------------------------*/
/*
 * Make sure necessary constants to define BFF_THIS_BREW_VERSION are
 * defined.
 */
#ifndef BREW_MAJ_VER
#error "BREW_MAJ_VER is not defined."
#endif /* BREW_MAJ_VER */

#ifndef BREW_MIN_VER
#error "BREW_MIN_VER is not defined."
#endif /* BREW_MIN_VER */

#ifndef BREW_SUB_VER
#error "BREW_SUB_VER is not defined."
#endif /* BREW_SUB_VER */

#ifndef BREW_BUILD
#error "BREW_BUILD is not defined."
#endif /* BREW_BUILD */


/**
 * @english
 * A packed representation of the current BREW version number.
 * @endenglish
 *
 * @japanese
 * 現在の BREW バージョン番号のパックされた表現形式
 * @endjapanese
 */
#define BFF_THIS_BREW_VERSION  BFF_VERSION_PACK(BREW_MAJ_VER,\
                                                BREW_MIN_VER,\
                                                BREW_SUB_VER,\
                                                BREW_BUILD)


/**
 * @english
 * A string representation of the current BREW version number.
 * @endenglish
 *
 * @japanese
 * 現在の BREW バージョン番号の文字列表現
 * @endjapanese
 */
#define BFF_THIS_BREW_VERSION_STRING BFF_VERSION_STRING(BREW_MAJ_VER,\
                                                        BREW_MIN_VER,\
                                                        BREW_SUB_VER,\
                                                        BREW_BUILD)


/**
 * @english
 * A generic macro to compare the current BREW version number to a
 * given set of a major, minor, micro and extra version numbers with
 * the specified operator.
 *
 * @param[in] operator  Operator to use
 * @param[in] major     Major version number to compare
 * @param[in] minor     Minor version number to compare
 * @param[in] micro     Micro version number to compare
 * @param[in] extra     Extra version number to compare
 *
 * @return  Result of the comparison.
 * @endenglish
 *
 * @japanese
 * 現在の BREW バージョン番号と、指定されたメジャー番号・マイナー番号<!--
 * -->・マイクロ番号・エクストラ番号の組とを、指定された演算子により<!--
 * -->比較する、汎用マクロ
 *
 * @param[in] operator  使用する演算子
 * @param[in] major     比較対象のメジャーバージョン番号
 * @param[in] minor     比較対象のマイナーバージョン番号
 * @param[in] micro     比較対象のマイクロバージョン番号
 * @param[in] extra     比較対象のエクストラバージョン番号
 *
 * @return  比較の結果
 * @endjapanese
 */
#define BFF_BREW_VERSION_OP(operator,major,minor,micro,extra) \
    ((BFF_THIS_BREW_VERSION) operator \
     BFF_VERSION_PACK((major),(minor),(micro),(extra)))


/**
 * @english
 * Test whether the current BREW version is less than the specified
 * version.
 *
 * @param[in] major     Major version number to compare
 * @param[in] minor     Minor version number to compare
 * @param[in] micro     Micro version number to compare
 * @param[in] extra     Extra version number to compare
 *
 * @return  True if the current BREW version is less than the specified
 *          version. Otherwise, false.
 * @endenglish
 *
 * @japanese
 * 現在の BREW バージョンが、指定されたバージョンよりも小さいか<!--
 * -->どうかをテストする。
 *
 * @param[in] major     比較対象のメジャーバージョン番号
 * @param[in] minor     比較対象のマイナーバージョン番号
 * @param[in] micro     比較対象のマイクロバージョン番号
 * @param[in] extra     比較対象のエクストラバージョン番号
 *
 * @return  現在の BREW バージョンが指定されたバージョンよりも小さい<!--
 *          -->場合、真。そうでなければ、偽。
 * @endjapanese
 */
#define BFF_BREW_VERSION_LT(major,minor,micro,extra) \
    BFF_BREW_VERSION_OP(<,(major),(minor),(micro),(extra))


/**
 * @english
 * Test whether the current BREW version is less than or equal to
 * the specified version.
 *
 * @param[in] major     Major version number to compare
 * @param[in] minor     Minor version number to compare
 * @param[in] micro     Micro version number to compare
 * @param[in] extra     Extra version number to compare
 *
 * @return  True if the current BREW version is less than or equal to
 *          the specified version. Otherwise, false.
 * @endenglish
 *
 * @japanese
 * 現在の BREW バージョンが、指定されたバージョンよりも小さいか<!--
 * -->もしくは等しいかどうかをテストする。
 *
 * @param[in] major     比較対象のメジャーバージョン番号
 * @param[in] minor     比較対象のマイナーバージョン番号
 * @param[in] micro     比較対象のマイクロバージョン番号
 * @param[in] extra     比較対象のエクストラバージョン番号
 *
 * @return  現在の BREW バージョンが指定されたバージョンよりも小さいか<!--
 *          -->もしくは等しい場合、真。そうでなければ、偽。
 * @endjapanese
 */
#define BFF_BREW_VERSION_LE(major,minor,micro,extra) \
    BFF_BREW_VERSION_OP(<=,(major),(minor),(micro),(extra))


/**
 * @english
 * Test whether the current BREW version is greater than the specified
 * version.
 *
 * @param[in] major     Major version number to compare
 * @param[in] minor     Minor version number to compare
 * @param[in] micro     Micro version number to compare
 * @param[in] extra     Extra version number to compare
 *
 * @return  True if the current BREW version is greater than the specified
 *          version. Otherwise, false.
 * @endenglish
 *
 * @japanese
 * 現在の BREW バージョンが、指定されたバージョンよりも大きいか<!--
 * -->どうかをテストする。
 *
 * @param[in] major     比較対象のメジャーバージョン番号
 * @param[in] minor     比較対象のマイナーバージョン番号
 * @param[in] micro     比較対象のマイクロバージョン番号
 * @param[in] extra     比較対象のエクストラバージョン番号
 *
 * @return  現在の BREW バージョンが、指定されたバージョンよりも大きい<!--
 *          -->場合、真。そうでなければ、偽。
 * @endjapanese
 */
#define BFF_BREW_VERSION_GT(major,minor,micro,extra) \
    BFF_BREW_VERSION_OP(>,(major),(minor),(micro),(extra))


/**
 * @english
 * Test whether the current BREW version is greater than or equal to
 * the specified version.
 *
 * @param[in] major     Major version number to compare
 * @param[in] minor     Minor version number to compare
 * @param[in] micro     Micro version number to compare
 * @param[in] extra     Extra version number to compare
 *
 * @return  True if the current BREW version is greater than or equal to
 *          the specified version. Otherwise, false.
 * @endenglish
 *
 * @japanese
 * 現在の BREW バージョンが、指定されたバージョンよりも大きいか<!--
 * -->もしくは等しいかどうかをテストする。
 *
 * @param[in] major     比較対象のメジャーバージョン番号
 * @param[in] minor     比較対象のマイナーバージョン番号
 * @param[in] micro     比較対象のマイクロバージョン番号
 * @param[in] extra     比較対象のエクストラバージョン番号
 *
 * @return  現在の BREW バージョンが、指定されたバージョンよりも大きいか<!--
 *          -->もしくは等しい場合、真。そうでなければ、偽。
 * @endjapanese
 */
#define BFF_BREW_VERSION_GE(major,minor,micro,extra) \
    BFF_BREW_VERSION_OP(>=,(major),(minor),(micro),(extra))


/**
 * @english
 * Test whether the current BREW version is equal to the specified
 * version.
 *
 * @param[in] major     Major version number to compare
 * @param[in] minor     Minor version number to compare
 * @param[in] micro     Micro version number to compare
 * @param[in] extra     Extra version number to compare
 *
 * @return  True if the current BREW version is equal to the specified
 *          version. Otherwise, false.
 * @endenglish
 *
 * @japanese
 * 現在の BREW バージョンが、指定されたバージョンと等しいかどうかを<!--
 * -->テストする。
 *
 * @param[in] major     比較対象のメジャーバージョン番号
 * @param[in] minor     比較対象のマイナーバージョン番号
 * @param[in] micro     比較対象のマイクロバージョン番号
 * @param[in] extra     比較対象のエクストラバージョン番号
 *
 * @return  現在の BREW バージョンが、指定されたバージョンと等しければ真。
 *          そうでなければ、偽。
 * @endjapanese
 */
#define BFF_BREW_VERSION_EQ(major,minor,micro,extra) \
    BFF_BREW_VERSION_OP(==,(major),(minor),(micro),(extra))


/**
 * @english
 * Test whether the current BREW version is not equal to the specified
 * version.
 *
 * @param[in] major     Major version number to compare
 * @param[in] minor     Minor version number to compare
 * @param[in] micro     Micro version number to compare
 * @param[in] extra     Extra version number to compare
 *
 * @return  True if the current BREW version is not equal to the specified
 *          version. Otherwise, false.
 * @endenglish
 *
 * @japanese
 * 現在の BREW バージョンが、指定されたバージョンと異なるかどうかを<!--
 * -->テストする。
 *
 * @param[in] major     比較対象のメジャーバージョン番号
 * @param[in] minor     比較対象のマイナーバージョン番号
 * @param[in] micro     比較対象のマイクロバージョン番号
 * @param[in] extra     比較対象のエクストラバージョン番号
 *
 * @return  現在の BREW バージョンが、指定されたバージョンと異なれば真。
 *          そうでなければ、偽。
 * @endjapanese
 */
#define BFF_BREW_VERSION_NE(major,minor,micro,extra) \
    BFF_BREW_VERSION_OP(!=,(major),(minor),(micro),(extra))


#endif /* BFF_VERSION_BREW_H */
