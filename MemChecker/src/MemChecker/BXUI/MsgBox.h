/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __MSGBOX_H__
#define __MSGBOX_H__

#include "XUtil.h"
#include "Window.h"

STRUCT(CMsgBox)
{
    CWindow base;

    AECHAR m_sMsg[256];
};

IWindow* CMsgBox_New(IShell* pIShell, const char* pszResFile, uint16 nResID, const AECHAR* pMsgText);

#endif // __MSGBOX_H__
