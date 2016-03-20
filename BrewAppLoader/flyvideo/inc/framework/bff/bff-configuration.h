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
 * Compilation time configuration support.
 *
 * Support build systems which automatically generate header files at
 * compilation time.
 *
 * - BFF_CFG_HAVE_CONFIGURATION_FILE
 *   - Defining this macro means that there is another configuration file
 *     which should be read.
 *
 * - BFF_CFG_CONFIGURATION_FILE
 *   - This macro is expected to hold a name of a configuration file which
 *     should be read.
 *
 * These two macros are not defined in BFF's header files. Instead, it is
 * expected that these macros are given as command line arguments of a
 * compiler. (e.g. /D BFF_CFG_CONFIGURATION_FILE="my-configuration-file.h",
 * /D BFF_CFG_HAVE_CONFIGURATION_FILE)
 *
 * If BFF_CFG_HAVE_CONFIGURATION_FILE is defined but
 * BFF_CFG_CONFIGURATION_FILE is not defined, the default value of a
 * configuration file name is assigned to BFF_CFG_CONFIGURATION_FILE.
 *
 * If BFF_CFG_CONFIGURATION_FILE is defined but
 * BFF_CFG_HAVE_CONFIGURATION_FILE is not defined,
 * BFF_CFG_HAVE_CONFIGURATION_FILE is defined.
 *
 * If neither of BFF_CFG_HAVE_CONFIGURATION_FILE nor
 * BFF_CFG_CONFIGURATION_FILE is defined, no header file is read by this
 * header file.
 * @endenglish
 *
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_CONFIGURATION_H
#define BFF_CONFIGURATION_H


/*------------------------------------------------------------------------*
 * MACROS
 *------------------------------------------------------------------------*/
/**
 * The default value of the configuration file name.
 */
#define BFF_CONFIGURATION_FILE_DEFAULT  "configuration-bff.h"

#ifdef BFF_CFG_HAVE_CONFIGURATION_FILE
  /* If a name of a configuration file is not specified, */
  #ifndef BFF_CFG_CONFIGURATION_FILE
    /* Assign the default value of a configuration file name. */
    #define BFF_CFG_CONFIGURATION_FILE  BFF_CONFIGURATION_FILE_DEFAULT
  #endif /* ! BFF_CFG_CONFIGURATION_FILE */
#else /* BFF_CFG_HAVE_CONFIGURATION_FILE */
  /* If a name of a configuration file is specified, */
  #ifdef BFF_CFG_CONFIGURATION_FILE
    /* Assume that a configuration file should be read. */
    #define BFF_CFG_HAVE_CONFIGURATION_FILE
  #endif /* BFF_CFG_CONFIGURATION_FILE */
#endif /* BFF_CFG_HAVE_CONFIGURATION_FILE */


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
/*
 * Read a configuration file if BFF_CFG_HAVE_CONFIGURATION_FILE is defined.
 */
#ifdef BFF_CFG_HAVE_CONFIGURATION_FILE
  #include BFF_CFG_CONFIGURATION_FILE
#endif /* BFF_CFG_HAVE_CONFIGURATION_FILE */


#endif /* BFF_CONFIGURATION_H */
