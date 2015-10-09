/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "SymbolMgr.h"
#include <windows.h>
#include <TLHELP32.H>
#include <imagehlp.h>

#pragma comment(lib, "imagehlp.lib")

#define PATH_SEPARATOR_CHAR '\\'
#define PATH_SEPARATOR      "\\"

typedef struct SymbolMgr
{
    DWORD  m_dwProcID;
    HANDLE m_hProcess;
} SymbolMgr;

SymbolMgr* SymbolMgr_New()
{
    SymbolMgr* pThis = malloc(sizeof(SymbolMgr));
    if (NULL == pThis) {
        return NULL;
    }

    memset(pThis, 0, sizeof(SymbolMgr));

    pThis->m_dwProcID = 0;
    pThis->m_hProcess = NULL;

    return pThis;
}

void SymbolMgr_Release(SymbolMgr* pThis)
{
    SymbolMgr_Reset(pThis);

    free(pThis);
}

void SymbolMgr_Init(SymbolMgr* pThis, DWORD dwProcID, HANDLE hProcess)
{
    SymbolMgr_Reset(pThis);

    if (SymInitialize(hProcess, NULL, FALSE)) {
        pThis->m_dwProcID = dwProcID;
        pThis->m_hProcess = hProcess;

        SymSetOptions(SymGetOptions() | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    }
}

void SymbolMgr_Reset(SymbolMgr* pThis)
{
    if (pThis->m_hProcess) {
        SymCleanup(pThis->m_hProcess);
    }

    pThis->m_hProcess = NULL;
}

typedef struct CheckLoadedCBContext
{
    void* pBaseAddr;
    BOOL bLoaded;
} CheckLoadedCBContext;

static BOOL CALLBACK SymbolMgr_CheckLoadedCB(LPSTR ModuleName, 
                                             ULONG BaseOfDll, 
                                             PVOID UserContext)
{
    CheckLoadedCBContext* pContext = UserContext;

    pContext->bLoaded = ((void*)BaseOfDll == pContext->pBaseAddr);

    return !pContext->bLoaded;
}

static void SymbolMgr_LoadModule(SymbolMgr* pThis, MODULEENTRY32* pModuleEntry)
{
    BOOL bLoaded = FALSE;
    char* pszPath = NULL;
    char* pszSep = NULL;
    IMAGEHLP_MODULE module_info = {0};

    pszPath = strdup(pModuleEntry->szExePath);
    if (pszPath)
    {
        pszSep = strrchr(pszPath, PATH_SEPARATOR_CHAR);
        if (pszSep) {
            *pszSep = '\0';
            pszSep++;
            SymSetSearchPath(pThis->m_hProcess, pszPath);
        }
        free(pszPath);
        pszPath = NULL;
    }

    bLoaded = SymLoadModule(pThis->m_hProcess, NULL, 
        pModuleEntry->szExePath, pModuleEntry->szModule, 
        (DWORD)pModuleEntry->modBaseAddr, pModuleEntry->modBaseSize);

    module_info.SizeOfStruct = sizeof(module_info);
    SymGetModuleInfo(pThis->m_hProcess, (DWORD)pModuleEntry->modBaseAddr, &module_info);

    OutputDebugString("############### Load module -- ");
    OutputDebugString(pModuleEntry->szExePath);
    if ((unsigned long)module_info.SymType >= sizeof(g_aszSymType) / sizeof(g_aszSymType[0])) {
        module_info.SymType = sizeof(g_aszSymType) / sizeof(g_aszSymType[0]);
    }

    if (bLoaded) {
        OutputDebugString("(Loaded: ");
        OutputDebugString(g_aszSymType[module_info.SymType]);
        OutputDebugString(")");
    } else {
        OutputDebugString("(Failed)");
    }
    OutputDebugString("\n");
}

void SymbolMgr_LoadModules(SymbolMgr* pThis)
{
    HANDLE hSnapshot = NULL;
    MODULEENTRY32 module_entry;
    BOOL bEnum = FALSE;

    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pThis->m_dwProcID);
    if (hSnapshot)
    {
        bEnum = Module32First(hSnapshot, &module_entry);
        while (bEnum)
        {
            CheckLoadedCBContext ctx;

            ctx.pBaseAddr = module_entry.modBaseAddr;
            ctx.bLoaded = FALSE;

            SymEnumerateModules(pThis->m_hProcess, 
                (PSYM_ENUMMODULES_CALLBACK)SymbolMgr_CheckLoadedCB, &ctx);

            if (!ctx.bLoaded) {
                SymbolMgr_LoadModule(pThis, &module_entry);
            }

            bEnum = Module32Next(hSnapshot, &module_entry);
        }
    }

    if (hSnapshot) {
        CloseHandle(hSnapshot);
    }
}
