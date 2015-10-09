/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __IWINDOW_H__
#define __IWINDOW_H__

#include "IContainer.h"

// IWindow Interface
#define INHERIT_IWindow(iname) \
    INHERIT_IContainer(iname)
AEEINTERFACE_DEFINE(IWindow);

#define IWindow_HandleEvent(p, e, w, dw)    AEEGETPVTBL(p, IWindow)->HandleEvent(p, e, w, dw)
#define IWindow_Draw(p, dc)                 AEEGETPVTBL(p, IWindow)->Draw(p, dc)
#define IWindow_SetRect(p, prc)             AEEGETPVTBL(p, IWindow)->SetRect(p, prc)
#define IWindow_GetRect(p, prc)             AEEGETPVTBL(p, IWindow)->GetRect(p, prc)
#define IWindow_HitTest(p, x, y)            AEEGETPVTBL(p, IWindow)->HitTest(p, x, y)

#define IWindow_Invalidate(p, widget, prc)  AEEGETPVTBL(p, IWindow)->Invalidate(p, widget, prc)
#define IWindow_Insert(p, widget, pos, ly)  AEEGETPVTBL(p, IWindow)->Insert(p, widget, pos, ly)
#define IWindow_Remove(p, index)            AEEGETPVTBL(p, IWindow)->Remove(p, index)
#define IWindow_GetChildCount(p)            AEEGETPVTBL(p, IWindow)->GetChildCount(p)
#define IWindow_GetChild(p, index)          AEEGETPVTBL(p, IWindow)->GetChild(p, index)

static __inline void IWindow_SetRectEx(IWindow* po, int x, int y, int dx, int dy)
{
    IWidget_SetRectEx((IWidget*)po, x, y, dx, dy);
}

static __inline void IWindow_SetPosition(IWindow* po, int x, int y)
{
    IWidget_SetPosition((IWidget*)po, x, y);
}

static __inline void IWindow_SetSize(IWindow* po, int dx, int dy)
{
    IWidget_SetSize((IWidget*)po, dx, dy);
}

#endif // __IWINDOW_H__
