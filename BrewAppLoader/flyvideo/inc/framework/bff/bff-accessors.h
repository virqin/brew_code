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
 * Macros to refer to IShell and IDisplay instances and the class ID.
 * @endenglish
 *
 * 
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_ACCESSORS_H
#define BFF_ACCESSORS_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEAppGen.h"
#include "AEEStdLib.h"


/*------------------------------------------------------------------------*
 * MACROS
 *------------------------------------------------------------------------*/
/**
 * @english
 * Get the IShell instance of a given applet.
 *
 * If a pointer to AEEApplet is already available, BFF_SHELL_OF() can be
 * used with the pointer as the argument. BFF_SHELL_OF() is a little bit
 * faster than BFF_SHELL().
 *
 * @param[in] applet  A pointer to an AEEApplet instance.
 *
 * @return  IShell instance
 * @endenglish
 *
 *
 * @see BFF_SHELL()
 */
#define BFF_SHELL_OF(applet)  (((AEEApplet *)(applet))->m_pIShell)


/**
 * @english
 * Get the IShell instance.
 *
 * @return  IShell instance
 * @endenglish
 *
 *
 * @see BFF_SHELL_OF(applet)
 */
#define BFF_SHELL()  BFF_SHELL_OF(GETAPPINSTANCE())


/**
 * @english
 * Get the IDisplay instance of a given applet.
 *
 * If a pointer to AEEApplet is already available, BFF_DISPLAY_OF() can be
 * used with the pointer as the argument. BFF_DISPLAY_OF() is a little bit
 * faster than BFF_DISPLAY().
 *
 * @param[in] applet  A pointer to an AEEApplet instance.
 *
 * @return  IDisplay instance
 * @endenglish
 *
 *
 * @see BFF_DISPLAY()
 */
#define BFF_DISPLAY_OF(applet)  (((AEEApplet *)(applet))->m_pIDisplay)


/**
 * @english
 * Get the IDisplay instance.
 *
 * @return  IDisplay instance
 * @endenglish
 *
 *
 * @see BFF_DISPLAY_OF(applet)
 */
#define BFF_DISPLAY()  BFF_DISPLAY_OF(GETAPPINSTANCE())


/**
 * @english
 * Get the class ID of a given applet.
 *
 * If a pointer to AEEApplet is already available, BFF_CLASS_ID_OF() can
 * be used with the pointer as the argument. BFF_CLASS_ID_OF() is a little
 * bit faster than BFF_CLASS_ID().
 *
 * @param[in] applet  A pointer to an AEEApplet instance.
 *
 * @return  Class ID
 * @endenglish
 *
 *
 * @see BFF_CLASS_ID()
 */
#define BFF_CLASS_ID_OF(applet)  (((AEEApplet *)(applet))->clsID)


/**
 * @english
 * Get the class ID.
 *
 * @return  Class ID
 * @endenglish
 *
 *
 * @see BFF_CLASS_ID_OF(applet)
 */
#define BFF_CLASS_ID()  BFF_CLASS_ID_OF(GETAPPINSTANCE())


#endif /* BFF_ACCESSORS_H */
