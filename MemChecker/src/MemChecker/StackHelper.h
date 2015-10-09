/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __STACKHELPER_H__
#define __STACKHELPER_H__

#include "SymbolMgr.h"

typedef struct StackHelper StackHelper;

#define CALLSTACK_ENTRY_MAX_NAMELEN 1024

typedef struct CallStackEntry
{
    DWORD  offset;
    CHAR   name[CALLSTACK_ENTRY_MAX_NAMELEN];
    DWORD  offsetFromSmybol;
    DWORD  offsetFromLine;
    DWORD  lineNumber;
    CHAR   lineFileName[CALLSTACK_ENTRY_MAX_NAMELEN];
    LPCSTR symTypeString;
    CHAR   moduleName[CALLSTACK_ENTRY_MAX_NAMELEN];
    DWORD  baseOfImage;
    CHAR   loadedImageName[CALLSTACK_ENTRY_MAX_NAMELEN];
} CallStackEntry;

typedef void (*PFNONCALLSTACKENTRY)(void *pUser, int nIndex, CallStackEntry *pCSEntry);

StackHelper* StackHelper_New();
void         StackHelper_Release(StackHelper* pThis);

void         StackHelper_Init(StackHelper* pThis, DWORD dwProcID);
void         StackHelper_Reset(StackHelper* pThis);

int          StackHelper_StackWalk(StackHelper* pThis, HANDLE hThread, const CONTEXT* pContext, PFNONCALLSTACKENTRY pfnCS, void* pUserData);

#endif // __STACKHELPER_H__
