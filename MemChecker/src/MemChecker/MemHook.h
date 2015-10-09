/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __MEMHOOK_H__
#define __MEMHOOK_H__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "AEEStdLib.h"

typedef enum
{
    OUTPUT_TO_DEBUGGER,
    OUTPUT_TO_SIMULATOR,
    OUTPUT_TO_FILE,
} EOutputMode;

typedef struct MemHookInitParam
{
    IShell     *pIShell;
    AEECLSID    clsApp;
    EOutputMode eOutputMode;
} MemHookInitParam;

boolean MemHook_Install(MemHookInitParam *pParam);
void    MemHook_Uninstall();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __MEMHOOK_H__
