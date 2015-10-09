/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __SYMBOLMGR_H__
#define __SYMBOLMGR_H__

#include <windows.h>

typedef struct SymbolMgr SymbolMgr;

static const char *g_aszSymType[] = 
{
    "SymNone",
    "SymCoff",
    "SymCv",
    "SymPdb",
    "SymExport",
    "SymDeferred",
    "SymSym",
    "SymDia",
    "SymVirtual",
    "Unknown"
};

SymbolMgr* SymbolMgr_New();
void       SymbolMgr_Release(SymbolMgr* pThis);

void       SymbolMgr_Init(SymbolMgr* pThis, DWORD dwProcID, HANDLE hProcess);
void       SymbolMgr_Reset(SymbolMgr* pThis);

void       SymbolMgr_LoadModules(SymbolMgr* pThis);

#endif // __SYMBOLMGR_H__
