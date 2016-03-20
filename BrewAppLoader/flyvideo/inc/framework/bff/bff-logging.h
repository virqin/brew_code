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
 * Macros to emit log messages.
 *
 * Macros to emit log messages are defined.
 *
 *   - BFF_VERBOSE()
 *   - BFF_DEBUG()
 *   - BFF_INFO()
 *   - BFF_WARN()
 *   - BFF_ERROR()
 *   - BFF_FATAL()
 *
 * Some points to note.
 *
 *  - Arguments given to these macros need to be enclosed with one more
 *    pair of parentheses like below.
 *
 *    @code
 *        BFF_DEBUG(("[x,y]=[%d,%d]",x, y));
 *    @endcode
 *
 *  - Bff.setLoggingLevel() changes the current logging level. For example,
 *    you can change the level to 'DEBUG' like below.
 *
 *    @code
 *        Bff *bff = BFF();
 *        bff->setLoggingLevel(bff, Bff_Logging_Level_DEBUG);
 *    @endcode
 *
 *  - BFF_DEBUG() won't output anything unless the current logging level
 *    is explicitly set to Bff_Logging_Level_DEBUG or lower, because the
 *    default logging level is #Bff_Logging_Level_INFO unless you set
 *    BFF_CFG_LOGGING_LEVEL_DEFAULT to BFF_LOGGING_LEVEL_DEBUG or lower
 *    like below when you build BFF library.
 *
 *    @code
 *        /D BFF_CFG_LOGGING_LEVEL_DEFAULT=BFF_LOGGING_LEVEL_DEBUG
 *    @endcode
 *
 *  - BFF_VERBOSE() won't be compiled in unless you set
 *    BFF_CFG_LOGGING_LEVEL_LOWEST to BFF_LOGGING_LEVEL_VERBOSE or lower
 *    like below when your source files process this header file.
 *
 *    @code
 *        /D BFF_CFG_LOGGING_LEVEL_LOWEST=BFF_LOGGING_LEVEL_VERBOSE
 *    @endcode
 *
 *  - These macros can be removed from your final product library by
 *    setting BFF_CFG_LOGGING_LEVEL_LOWEST properly. For example,
 *    setting a value like below will remove BFF_VERBOSE(),
 *    BFF_DEBUG(), BFF_INFO() and BFF_WARN().
 *
 *    @code
 *        /D BFF_CFG_LOGGING_LEVEL_LOWEST=BFF_LOGGING_LEVEL_ERROR
 *    @endcode
 *
 *  - Prefixes of log messages can be changed by setting
 *    BFF_CFG_LOGGING_PREFIX_xxx macros.
 * @endenglish
 *
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_LOGGING_H
#define BFF_LOGGING_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "bff/bff-configuration.h"


/*------------------------------------------------------------------------*
 * MACROS
 *------------------------------------------------------------------------*/
/**
 * @english
 * A value to indicate an invalid logging level.
 * @endenglish
 *
 * @japanese
 * 無効なロギングレベルを示す値
 * @endjapanese
 */
#define BFF_LOGGING_LEVEL_INVALID  0


/**
 * @english
 * A logging level to show all log messages.
 * @endenglish
 *
 * @japanese
 * 全てのログメッセージを表示するロギングレベル
 * @endjapanese
 */
#define BFF_LOGGING_LEVEL_ALL      1


/**
 * @english
 * A logging level to show verbose-level and higher log messages.
 * @endenglish
 *
 * @japanese
 * 冗長 (VERBOSE) レベル以上のログメッセージを表示するロギングレベル
 * @endjapanese
 */
#define BFF_LOGGING_LEVEL_VERBOSE  2


/**
 * @english
 * A logging level to show debug-level and higher log messages.
 * @endenglish
 *
 * @japanese
 * デバッグ (DEBUG) レベル以上のログメッセージを表示するロギングレベル
 * @endjapanese
 */
#define BFF_LOGGING_LEVEL_DEBUG    3


/**
 * @english
 * A logging level to show information-level and higher log messages.
 * @endenglish
 *
 * @japanese
 * 情報 (INFO) レベル以上のログメッセージを表示するロギングレベル
 * @endjapanese
 */
#define BFF_LOGGING_LEVEL_INFO     4


/**
 * @english
 * A logging level to show warning-level and higher log messages.
 * @endenglish
 *
 * @japanese
 * 警告 (WARN) レベル以上のログメッセージを表示するロギングレベル
 * @endjapanese
 */
#define BFF_LOGGING_LEVEL_WARN     5


/**
 * @english
 * A logging level to show error-level and higher log messages.
 * @endenglish
 *
 * @japanese
 * エラー (ERROR) レベル以上のログメッセージを表示するロギングレベル
 * @endjapanese
 */
#define BFF_LOGGING_LEVEL_ERROR    6


/**
 * @english
 * A logging level to show fatal-level and higher log messages.
 * @endenglish
 *
 * @japanese
 * 致命 (FATAL) レベル以上のログメッセージを表示するロギングレベル
 * @endjapanese
 */
#define BFF_LOGGING_LEVEL_FATAL    7


/**
 * @english
 * A logging level to suppress all log messages.
 * @endenglish
 *
 * @japanese
 * 全てのログメッセージを抑制するロギングレベル
 * @endjapanese
 */
#define BFF_LOGGING_LEVEL_NONE     8


/**
 * @english
 * A macro to check if a given logging level is out of the valid range.
 *
 * @param[in] level  A logging level.
 *
 * @return  True if the specified level is out of the valid range of
 *          the logging level. Otherwise, false.
 * @endenglish
 *
 * @japanese
 * 指定されたロギングレベルが範囲外かどうかをチェックするマクロ
 *
 * @param[in] level  ロギングレベル
 *
 * @return  指定されたレベルが有効なロギングレベルの範囲外であれば、真。
 *          それ以外であれば、偽。
 * @endjapanese
 */
#define BFF_LOGGING_LEVEL_OUT_OF_RANGE(level) \
    ((level) < BFF_LOGGING_LEVEL_ALL || BFF_LOGGING_LEVEL_NONE < (level))


/*
 * Determine the lowest logging level to compile in. The value is set to
 * BFF_LOGGING_LEVEL_LOWEST.
 *
 * BFF_CFG_LOGGING_LEVEL_LOWEST can change the value if it is set.
 */
#ifdef BFF_CFG_LOGGING_LEVEL_LOWEST
  #if BFF_LOGGING_LEVEL_OUT_OF_RANGE(BFF_CFG_LOGGING_LEVEL_LOWEST)
    #error "BFF_CFG_LOGGING_LEVEL_LOWEST is out of range"
  #endif /* BFF_LOGGING_LEVEL_OUT_OF_RANGE(BFF_CFG_LOGGING_LEVEL_LOWEST) */
  /**
   * @english
   * The lowest logging level whose log messages are compiled in.
   * @endenglish
   *
   * @japanese
   * ログメッセージがコンパイルで組み込まれることになるロギングレベル<!--
   * -->の最低値
   * @endjapanese
   */
  #define BFF_LOGGING_LEVEL_LOWEST  BFF_CFG_LOGGING_LEVEL_LOWEST
#else /* BFF_CFG_LOGGING_LEVEL_LOWEST */
  /**
   * @english
   * The lowest logging level whose log messages are compiled in.
   * @endenglish
   *
   * @japanese
   * ログメッセージがコンパイルで組み込まれることになるロギングレベル<!--
   * -->の最低値
   * @endjapanese
   */
  #define BFF_LOGGING_LEVEL_LOWEST  BFF_LOGGING_LEVEL_DEBUG
#endif /* BFF_CFG_LOGGING_LEVEL_LOWEST */


/*
 * Determine the default logging level at runtime. The value is set to
 * BFF_LOGGING_LEVEL_DEFAULT.
 *
 * BFF_CFG_LOGGING_LEVEL_DEFAULT can change the value if it is set.
 */
#ifdef BFF_CFG_LOGGING_LEVEL_DEFAULT
  #if BFF_LOGGING_LEVEL_OUT_OF_RANGE(BFF_CFG_LOGGING_LEVEL_DEFAULT)
    #error "BFF_CFG_LOGGING_LEVEL_DEFAULT is out of range"
  #endif /* BFF_LOGGING_LEVEL_OUT_OF_RANGE(BFF_CFG_LOGGING_LEVEL_DEFAULT) */
  /**
   * @english
   * The default logging level.
   * @endenglish
   *
   * @japanese
   * デフォルトのロギングレベル
   * @endjapanese
   */
  #define BFF_LOGGING_LEVEL_DEFAULT  BFF_CFG_LOGGING_LEVEL_DEFAULT
#else /* BFF_CFG_LOGGING_LEVEL_DEFAULT */
  /**
   * @english
   * The default logging level.
   * @endenglish
   *
   * @japanese
   * デフォルトのロギングレベル
   * @endjapanese
   */
  #define BFF_LOGGING_LEVEL_DEFAULT  BFF_LOGGING_LEVEL_INFO
#endif /* BFF_CFG_LOGGING_LEVEL_DEFAULT */


/*
 * Determine the prefix used by BFF_VERBOSE(). The value is set to
 * BFF_LOGGING_PREFIX_VERBOSE.
 *
 * BFF_CFG_LOGGING_PREFIX_VERBOSE can change the value.
 */
#ifdef BFF_CFG_LOGGING_PREFIX_VERBOSE
  /**
   * @english
   * The prefix used to emit verbose-level log messages.
   * @endenglish
   *
   * @japanese
   * 冗長 (VERBOSE) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_VERBOSE  BFF_CFG_LOGGING_PREFIX_VERBOSE
#else /* BFF_CFG_LOGGING_PREFIX_VERBOSE */
  /**
   * @english
   * The prefix used to emit verbose-level log messages.
   * @endenglish
   *
   * @japanese
   * 冗長 (VERBOSE) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_VERBOSE  ""
#endif /* BFF_CFG_LOGGING_PREFIX_VERBOSE */


/*
 * Determine the prefix used by BFF_DEBUG(). The value is set to
 * BFF_LOGGING_PREFIX_DEBUG.
 *
 * BFF_CFG_LOGGING_PREFIX_DEBUG can change the value.
 */
#ifdef BFF_CFG_LOGGING_PREFIX_DEBUG
  /**
   * @english
   * The prefix used to emit debug-level log messages.
   * @endenglish
   *
   * @japanese
   * デバッグ (DEBUG) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_DEBUG  BFF_CFG_LOGGING_PREFIX_DEBUG
#else /* BFF_CFG_LOGGING_PREFIX_DEBUG */
  /**
   * @english
   * The prefix used to emit debug-level log messages.
   * @endenglish
   *
   * @japanese
   * デバッグ (DEBUG) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_DEBUG  "DBG:"
#endif /* BFF_CFG_LOGGING_PREFIX_DEBUG */


/*
 * Determine the prefix used by BFF_INFO(). The value is set to
 * BFF_LOGGING_PREFIX_INFO.
 *
 * BFF_CFG_LOGGING_PREFIX_INFO can change the value.
 */
#ifdef BFF_CFG_LOGGING_PREFIX_INFO
  /**
   * @english
   * The prefix used to emit information-level log messages.
   * @endenglish
   *
   * @japanese
   * 情報 (INFO) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_INFO  BFF_CFG_LOGGING_PREFIX_INFO
#else /* BFF_CFG_LOGGING_PREFIX_INFO */
  /**
   * @english
   * The prefix used to emit information-level log messages.
   * @endenglish
   *
   * @japanese
   * 情報 (INFO) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_INFO  "INFO:"
#endif /* BFF_CFG_LOGGING_PREFIX_INFO */


/*
 * Determine the prefix used by BFF_WARN(). The value is set to
 * BFF_LOGGING_PREFIX_WARN.
 *
 * BFF_CFG_LOGGING_PREFIX_WARN can change the value.
 */
#ifdef BFF_CFG_LOGGING_PREFIX_WARN
  /**
   * @english
   * The prefix used to emit warning-level log messages.
   * @endenglish
   *
   * @japanese
   * 警告 (WARN) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_WARN  BFF_CFG_LOGGING_PREFIX_WARN
#else /* BFF_CFG_LOGGING_PREFIX_WARN */
  /**
   * @english
   * The prefix used to emit warning-level log messages.
   * @endenglish
   *
   * @japanese
   * 警告 (WARN) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_WARN  "WARN:"
#endif /* BFF_CFG_LOGGING_PREFIX_WARN */


/*
 * Determine the prefix used by BFF_ERROR(). The value is set to
 * BFF_LOGGING_PREFIX_ERROR.
 *
 * BFF_CFG_LOGGING_PREFIX_ERROR can change the value.
 */
#ifdef BFF_CFG_LOGGING_PREFIX_ERROR
  /**
   * @english
   * The prefix used to emit error-level log messages.
   * @endenglish
   *
   * @japanese
   * エラー (ERROR) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_ERROR  BFF_CFG_LOGGING_PREFIX_ERROR
#else /* BFF_CFG_LOGGING_PREFIX_ERROR */
  /**
   * @english
   * The prefix used to emit error-level log messages.
   * @endenglish
   *
   * @japanese
   * エラー (ERROR) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_ERROR  "ERR:"
#endif /* BFF_CFG_LOGGING_PREFIX_ERROR */


/*
 * Determine the prefix used by BFF_FATAL(). The value is set to
 * BFF_LOGGING_PREFIX_FATAL.
 *
 * BFF_CFG_LOGGING_PREFIX_FATAL can change the value.
 */
#ifdef BFF_CFG_LOGGING_PREFIX_FATAL
  /**
   * @english
   * The prefix used to emit fatal-level log messages.
   * @endenglish
   *
   * @japanese
   * 致命 (FATAL) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_FATAL  BFF_CFG_LOGGING_PREFIX_FATAL
#else /* BFF_CFG_LOGGING_PREFIX_FATAL */
  /**
   * @english
   * The prefix used to emit fatal-level log messages.
   * @endenglish
   *
   * @japanese
   * 致命 (FATAL) レベルのログメッセージのプレフィックス
   * @endjapanese
   */
  #define BFF_LOGGING_PREFIX_FATAL  "FATAL:"
#endif /* BFF_CFG_LOGGING_PREFIX_FATAL */


/*
 * Define BFF_VERBOSE().
 */
#if BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_VERBOSE
  /**
   * @english
   * Emit a verbose-level log message.
   * @endenglish
   *
   * @japanese
   * 冗長 (VERBOSE) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_VERBOSE(args)  Bff_Logging_verbose args
#else /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_VERBOSE */
  /**
   * @english
   * Emit a verbose-level log message.
   * @endenglish
   *
   * @japanese
   * 冗長 (VERBOSE) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_VERBOSE(args)  do { } while (0)
#endif /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_VERBOSE */


/*
 * Define BFF_DEBUG().
 */
#if BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_DEBUG
  /**
   * @english
   * Emit a debug-level log message.
   * @endenglish
   *
   * @japanese
   * デバッグ (DEBUG) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_DEBUG(args)  Bff_Logging_debug args
#else /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_DEBUG */
  /**
   * @english
   * Emit a debug-level log message.
   * @endenglish
   *
   * @japanese
   * デバッグ (DEBUG) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_DEBUG(args)  do { } while (0)
#endif /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_DEBUG */


/*
 * Define BFF_INFO().
 */
#if BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_INFO
  /**
   * @english
   * Emit an information-level log message.
   * @endenglish
   *
   * @japanese
   * 情報 (INFO) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_INFO(args)  Bff_Logging_info args
#else /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_INFO */
  /**
   * @english
   * Emit an information-level log message.
   * @endenglish
   *
   * @japanese
   * 情報 (INFO) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_INFO(args)  do { } while (0)
#endif /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_INFO */


/*
 * Define BFF_WARN().
 */
#if BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_WARN
  /**
   * @english
   * Emit a warning-level log message.
   * @endenglish
   *
   * @japanese
   * 警告 (WARN) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_WARN(args)  Bff_Logging_warn args
#else /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_WARN */
  /**
   * @english
   * Emit a warning-level log message.
   * @endenglish
   *
   * @japanese
   * 警告 (WARN) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_WARN(args)  do { } while (0)
#endif /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_WARN */


/*
 * Define BFF_ERROR().
 */
#if BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_ERROR
  /**
   * @english
   * Emit an error-level log message.
   * @endenglish
   *
   * @japanese
   * エラー (ERROR) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_ERROR(args)  Bff_Logging_error args
#else /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_ERROR */
  /**
   * @english
   * Emit an error-level log message.
   * @endenglish
   *
   * @japanese
   * エラー (ERROR) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_ERROR(args)  do { } while (0)
#endif /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_ERROR */


/*
 * Define BFF_FATAL().
 */
#if BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_FATAL
  /**
   * @english
   * Emit a fatal-level log message.
   * @endenglish
   *
   * @japanese
   * 致命 (FATAL) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_FATAL(args)  Bff_Logging_fatal args
#else /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_FATAL */
  /**
   * @english
   * Emit a fatal-level log message.
   * @endenglish
   *
   * @japanese
   * 致命 (FATAL) レベルのログメッセージを出力する
   * @endjapanese
   */
  #define BFF_FATAL(args)  do { } while (0)
#endif /* BFF_LOGGING_LEVEL_LOWEST <= BFF_LOGGING_LEVEL_FATAL */


/*------------------------------------------------------------------------*
 * TYPEDEFS
 *------------------------------------------------------------------------*/
/**
 * @english
 * Aliases of logging levels.
 * @endenglish
 *
 * @japanese
 * ロギングレベルの別名
 * @endjapanese
 */
typedef enum
{
    /**
     * @english
     * A logging level to show all log messages.
     * @endenglish
     *
     * @japanese
     * 全てのログメッセージを表示するロギングレベル
     * @endjapanese
     */
    Bff_Logging_Level_ALL     = BFF_LOGGING_LEVEL_ALL,

    /**
     * @english
     * A logging level to show verbose-level and higher log messages.
     * @endenglish
     *
     * @japanese
     * 冗長 (VERBOSE) レベル以上のログメッセージを表示するロギングレベル
     * @endjapanese
     */
    Bff_Logging_Level_VERBOSE = BFF_LOGGING_LEVEL_VERBOSE,

    /**
     * @english
     * A logging level to show debug-level and higher log messages.
     * @endenglish
     *
     * @japanese
     * デバッグ (DEBUG) レベル以上のログメッセージを表示するロギングレベル
     * @endjapanese
     */
    Bff_Logging_Level_DEBUG   = BFF_LOGGING_LEVEL_DEBUG,

    /**
     * @english
     * A logging level to show information-level and higher log messages.
     * @endenglish
     *
     * @japanese
     * 情報 (INFO) レベル以上のログメッセージを表示するロギングレベル
     * @endjapanese
     */
    Bff_Logging_Level_INFO    = BFF_LOGGING_LEVEL_INFO,

    /**
     * @english
     * A logging level to show warning-level and higher log messages.
     * @endenglish
     *
     * @japanese
     * 警告 (WARN) レベル以上のログメッセージを表示するロギングレベル
     * @endjapanese
     */
    Bff_Logging_Level_WARN    = BFF_LOGGING_LEVEL_WARN,

    /**
     * @english
     * A logging level to show error-level and higher log messages.
     * @endenglish
     *
     * @japanese
     * エラー (ERROR) レベル以上のログメッセージを表示するロギングレベル
     * @endjapanese
     */
    Bff_Logging_Level_ERROR   = BFF_LOGGING_LEVEL_ERROR,

    /**
     * @english
     * A logging level to show fatal-level and higher log messages.
     * @endenglish
     *
     * @japanese
     * 致命 (FATAL) レベル以上のログメッセージを表示するロギングレベル
     * @endjapanese
     */
    Bff_Logging_Level_FATAL   = BFF_LOGGING_LEVEL_FATAL,

    /**
     * @english
     * A logging level to suppress all log messages.
     * @endenglish
     *
     * @japanese
     * 全てのログメッセージを抑制するロギングレベル
     * @endjapanese
     */
    Bff_Logging_Level_NONE    = BFF_LOGGING_LEVEL_NONE,

    /**
     * @english
     * The default logging level.
     * @endenglish
     *
     * @japanese
     * デフォルトのロギングレベル
     * @endjapanese
     */
    Bff_Logging_Level_DEFAULT = BFF_LOGGING_LEVEL_DEFAULT,

    /**
     * @english
     * A value to indicate an invalid logging level.
     * @endenglish
     *
     * @japanese
     * 無効なロギングレベルを示す値
     * @endjapanese
     */
    Bff_Logging_Level_INVALID = BFF_LOGGING_LEVEL_INVALID
}
Bff_Logging_Level;


/*------------------------------------------------------------------------*
 * FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @english
 * Emit a verbose-level log message.
 * @endenglish
 *
 * @japanese
 * 冗長 (VERBOSE) レベルのログメッセージを出力する
 * @endjapanese
 */
void Bff_Logging_verbose(const char *fmt, ...);


/**
 * @english
 * Emit a debug-level log message.
 * @endenglish
 *
 * @japanese
 * デバッグ (DEBUG) レベルのログメッセージを出力する
 * @endjapanese
 */
void Bff_Logging_debug(const char *fmt, ...);


/**
 * @english
 * Emit an information-level log message.
 * @endenglish
 *
 * @japanese
 * 情報 (INFO) レベルのログメッセージを出力する
 * @endjapanese
 */
void Bff_Logging_info(const char *fmt, ...);


/**
 * @english
 * Emit a warning-level log message.
 * @endenglish
 *
 * @japanese
 * 警告 (WARN) レベルのログメッセージを出力する
 * @endjapanese
 */
void Bff_Logging_warn(const char *fmt, ...);


/**
 * @english
 * Emit an error-level log message.
 * @endenglish
 *
 * @japanese
 * エラー (ERROR) レベルのログメッセージを出力する
 * @endjapanese
 */
void Bff_Logging_error(const char *fmt, ...);


/**
 * @english
 * Emit a fatal-level log message.
 * @endenglish
 *
 * @japanese
 * 致命 (FATAL) レベルのログメッセージを出力する
 * @endjapanese
 */
void Bff_Logging_fatal(const char *fmt, ...);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BFF_LOGGING_H */
