/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __MEMCHECKERUTIL_H__
#define __MEMCHECKERUTIL_H__

#include "AEEShell.h"
#include "AEEAppletCtl.h"
#include "XUtil.h"

AECHAR *GetAppName(IShell *pIShell, AEECLSID cls);

static __inline int GetAppRunningState(IAppletCtl *pIAppletCtl, AEECLSID cls, uint32 *pdwState)
{
    int nSize = sizeof(*pdwState);
    return IAPPLETCTL_GetRunningInfo(pIAppletCtl, cls, AEE_APPITEM_STATE, pdwState, &nSize);
}

int     XMessageBox(const AECHAR* pszText, const AECHAR* pszCaption);

void    DbgPrintf(const char *psz, ...);
void    DbgPrintfLn(const char *psz, ...);

#endif // __MEMCHECKERUTIL_H__
