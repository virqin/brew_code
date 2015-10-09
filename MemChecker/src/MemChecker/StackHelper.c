/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "StackHelper.h"
#include <imagehlp.h>

typedef struct StackHelper
{
    SymbolMgr* m_pSymbolMgr;
    HANDLE     m_hProcess;
} StackHelper;

StackHelper* StackHelper_New()
{
    StackHelper* pThis = malloc(sizeof(StackHelper));
    if (NULL == pThis) {
        return NULL;
    }
    
    memset(pThis, 0, sizeof(StackHelper));

    pThis->m_pSymbolMgr = SymbolMgr_New();
    if (NULL == pThis->m_pSymbolMgr) {
        free(pThis);
        pThis = NULL;
    }

    pThis->m_hProcess = NULL;

    return pThis;
}

void StackHelper_Release(StackHelper* pThis)
{
    StackHelper_Reset(pThis);

    if (pThis->m_pSymbolMgr) {
        SymbolMgr_Release(pThis->m_pSymbolMgr);
        pThis->m_pSymbolMgr = NULL;
    }

    free(pThis);
}

void StackHelper_Init(StackHelper* pThis, DWORD dwProcID)
{
    if (0 == dwProcID) {
        dwProcID = GetCurrentProcessId();
    }

    pThis->m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, dwProcID);

    if (NULL != pThis->m_pSymbolMgr) {
        SymbolMgr_Init(pThis->m_pSymbolMgr, dwProcID, pThis->m_hProcess);
    }
}

void StackHelper_Reset(StackHelper* pThis)
{
    if (pThis->m_pSymbolMgr) {
        SymbolMgr_Reset(pThis->m_pSymbolMgr);
    }

    if (pThis->m_hProcess)
    {
        CloseHandle(pThis->m_hProcess);
        pThis->m_hProcess = NULL;
    }
}

int StackHelper_StackWalk(StackHelper* pThis, HANDLE hThread, const CONTEXT* pContext, PFNONCALLSTACKENTRY pfnCS, void* pUserData)
{
    STACKFRAME sf = {0};
    CONTEXT context = {0};
    IMAGEHLP_MODULE module_info = {0};
    IMAGEHLP_SYMBOL *pSym = NULL;
    IMAGEHLP_LINE line = {0};
    CallStackEntry csEntry = {0};
    int nIndex = 0;

    if (NULL == pContext) {
        return FALSE;
    }

    if (NULL == hThread) {
        hThread = GetCurrentThread();
    }

    SymbolMgr_LoadModules(pThis->m_pSymbolMgr);

    context = *pContext;

    sf.AddrPC.Offset = context.Eip;
    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrFrame.Offset = context.Ebp;
    sf.AddrFrame.Mode = AddrModeFlat;
    sf.AddrStack.Offset = context.Esp;
    sf.AddrStack.Mode = AddrModeFlat;

    module_info.SizeOfStruct = sizeof(module_info);

    pSym = malloc(sizeof(IMAGEHLP_SYMBOL) + CALLSTACK_ENTRY_MAX_NAMELEN);
    if (NULL == pSym) {
        return FALSE;
    }
    memset(pSym, 0, sizeof(IMAGEHLP_SYMBOL) + CALLSTACK_ENTRY_MAX_NAMELEN);
    pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
    pSym->MaxNameLength = CALLSTACK_ENTRY_MAX_NAMELEN - 1;

    line.SizeOfStruct = sizeof(line);

    for (;;)
    {
        BOOL bRet = StackWalk(IMAGE_FILE_MACHINE_I386, 
            pThis->m_hProcess, hThread, 
            &sf, &context, 
            (PREAD_PROCESS_MEMORY_ROUTINE)ReadProcessMemory, 
            SymFunctionTableAccess, SymGetModuleBase, 
            NULL);
        if (!bRet) {
            break;
        }

        memset(&csEntry, 0, sizeof(csEntry));

        csEntry.offset = sf.AddrPC.Offset;

        if (0 == sf.AddrFrame.Offset) {
            break;
        }

        if (SymGetModuleInfo(pThis->m_hProcess, sf.AddrPC.Offset, &module_info))
        {
            if ((unsigned long)module_info.SymType >= sizeof(g_aszSymType) / sizeof(g_aszSymType[0])) {
                module_info.SymType = sizeof(g_aszSymType) / sizeof(g_aszSymType[0]);
            }
            csEntry.symTypeString = g_aszSymType[module_info.SymType];
            strcpy(csEntry.moduleName, module_info.ModuleName);
            csEntry.baseOfImage = module_info.BaseOfImage;
            strcpy(csEntry.loadedImageName, module_info.LoadedImageName);
        }

        if (SymGetSymFromAddr(pThis->m_hProcess, sf.AddrPC.Offset, &csEntry.offsetFromSmybol, pSym))
        {
            strcpy(csEntry.name, pSym->Name);
        }

        if (SymGetLineFromAddr(pThis->m_hProcess, sf.AddrPC.Offset, &csEntry.offsetFromLine, &line))
        {
            strcpy(csEntry.lineFileName, line.FileName);
            csEntry.lineNumber = line.LineNumber;
        }

        if (pfnCS) {
            pfnCS(pUserData, nIndex, &csEntry);
        }

        nIndex++;
    }

    return TRUE;
}
