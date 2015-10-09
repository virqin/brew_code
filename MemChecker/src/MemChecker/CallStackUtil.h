/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __CALLSTACKUTIL_H__
#define __CALLSTACKUTIL_H__

#include "StackHelper.h"

#define GET_CURRENT_CONTEXT(c, contextFlags) \
    do { \
        memset(&c, 0, sizeof(CONTEXT)); \
        c.ContextFlags = contextFlags; \
        __asm    call x \
        __asm x: pop eax \
        __asm    mov c.Eip, eax \
        __asm    mov c.Ebp, ebp \
        __asm    mov c.Esp, esp \
    } while(0);

#endif // __CALLSTACKUTIL_H__
