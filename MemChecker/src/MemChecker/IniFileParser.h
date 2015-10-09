/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __INIFILEPARSER_H__
#define __INIFILEPARSER_H__

#include "AEEShell.h"

typedef void (*IniParseLineCB)(void *pUser, const char *pszKey, int nLenKey, const char *pszValue, int nLenValue);

int IniFileParser_LoadFile(IShell *pIShell, IniParseLineCB pfnHandler, void *pUser);

#endif // __INIFILEPARSER_H__
