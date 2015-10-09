/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "MemCheckerUtil.h"
#include "AEEStdLib.h"
#include "AEEAppletCtl.h"
#include "XUtil.h"

AECHAR *GetAppName(IShell *pIShell, AEECLSID cls)
{
    AEEAppInfo ai = {0};
    AECHAR *pBuff = NULL;
    const AECHAR *cszSystem = L"{System}";

    if (AEECLSID_SHELL == cls) // System
    {
        pBuff = WSTRDUP(cszSystem);
        return pBuff;
    }

    ISHELL_QueryClass(pIShell, cls, &ai);
    ISHELL_LoadResStringEx(pIShell, ai.pszMIF, APPR_NAME(ai), &pBuff);
    return pBuff;
}

// Windows
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

int XMessageBox(const AECHAR* pszText, const AECHAR* pszCaption)
{
    return MessageBoxW(GetForegroundWindow(), pszText, pszCaption, MB_OK);
}

void _vDbgPrintf(const char *psz, va_list args)
{
#ifdef AEE_SIMULATOR
    char *pDest = NULL;
    const int BUFFER_SIZE = 1024;

    pDest = malloc(BUFFER_SIZE);
    if (NULL == pDest) {
        return;
    }

    memset(pDest, 0, BUFFER_SIZE);
    _vsnprintf(pDest, BUFFER_SIZE, psz, args);
    OutputDebugString(pDest);

    free(pDest);
    pDest = NULL;
#endif
}

void DbgPrintf(const char *psz, ...)
{
    va_list args;

    va_start(args, psz);
    _vDbgPrintf(psz, args);
    va_end(args);
}

void DbgPrintfLn(const char *psz, ...)
{
    va_list args;

    va_start(args, psz);
    _vDbgPrintf(psz, args);
    va_end(args);

    DbgPrintf("\n");
}
