/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "FuncHookUtil.h"

void __hook_func(void *pFuncAddr, const char *pHookBytes, int nHookBytes, char *pOldBytes)
{
    DWORD dwOldProtect = 0;
    
    if (NULL != pOldBytes)
    {
        memcpy(pOldBytes, pFuncAddr, nHookBytes);
    }
    
    MemoryProtect(pFuncAddr, nHookBytes, PAGE_READWRITE, &dwOldProtect);
    memcpy(pFuncAddr, pHookBytes, nHookBytes);
    MemoryProtect(pFuncAddr, nHookBytes, dwOldProtect, &dwOldProtect);
}
