/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __FUNCHOOKUTIL_H__
#define __FUNCHOOKUTIL_H__

// X86, Windows

#ifdef AEE_SIMULATOR
#include <windows.h>
#endif

static __inline long MemoryProtect(void *pAddress, DWORD dwSize, DWORD dwProtect, DWORD *pdwOldProtect)
{
#ifdef AEE_SIMULATOR
    return VirtualProtect(pAddress, dwSize, dwProtect, pdwOldProtect);
#else
    return FALSE;
#endif
}

typedef struct FuncHookCtx
{
    void       *pHookedFunc;

    const char *pHookBytes;
    int         nHookBytes;

    char        aOldBytes[10];
}FuncHookCtx;

static __inline FuncHookCtx *FuncHookCtx_New()
{
    FuncHookCtx *pObj = malloc(sizeof(FuncHookCtx));
    memset(pObj, 0, sizeof(FuncHookCtx));
    return pObj;
}

static __inline void FuncHookCtx_Release(FuncHookCtx *pObj)
{
    free(pObj);
}

#define LABEL_ADDRESS(label, pOut) do { __asm mov pOut, offset label } while (0)

#define GET_HOOK_BYTES(func, pHookBytes, nHookBytes) \
    do { \
        __asm   mov pHookBytes, offset func##_x \
        __asm   mov eax, offset func##_end \
        __asm   sub eax, offset func##_x \
        __asm   mov nHookBytes, eax \
        __asm   jmp func##_end \
        __asm func##_x: \
        __asm   mov eax, offset func \
        __asm   jmp eax \
        __asm func##_end: \
    } while (0)

#define HOOK_FUNC(func, hookfunc, ppContext) \
    do { \
        const char *pHookBytes__ = NULL; \
        int nHookBytes__ = 0; \
        FuncHookCtx *pContext = FuncHookCtx_New(); \
        GET_HOOK_BYTES(hookfunc, pHookBytes__, nHookBytes__); \
        pContext->pHookedFunc = (void*)(int)(func); \
        pContext->pHookBytes = pHookBytes__; \
        pContext->nHookBytes = nHookBytes__; \
        __hook_func((void*)(int)(func), pHookBytes__, nHookBytes__, pContext->aOldBytes); \
        *(ppContext) = pContext; \
    } while (0)

#define UNHOOK_FUNC(pContext) \
    HOOK_PAUSE(pContext); \
    FuncHookCtx_Release(pContext); \
    pContext = NULL

#define HOOK_PAUSE(pContext) \
    __hook_func(pContext->pHookedFunc, pContext->aOldBytes, pContext->nHookBytes, NULL)

#define HOOK_RESUME(pContext) \
    __hook_func(pContext->pHookedFunc, pContext->pHookBytes, pContext->nHookBytes, NULL)

void __hook_func(void *pFuncAddr, const char *pHookBytes, int nHookBytes, char *pOldBytes);

#endif // __FUNCHOOKUTIL_H__
