/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "XUtil.h"
#include "IWindow.h"
#include "Container.h"

// CWindow class
typedef struct CWindow
{
    CContainer base;
} CWindow;

IWindow*    CWindow_New();
void        CWindow_Ctor(CWindow* pMe);
uint32      CWindow_Release(IWindow* po);
boolean     CWindow_HandleEvent(IWindow* po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void        CWindow_Draw(IWindow* po, DrawContext* pDrawContext);

#endif // __WINDOW_H__
