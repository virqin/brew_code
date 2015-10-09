/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __IDESKTOPWINDOW_H__
#define __IDESKTOPWINDOW_H__

#include "IContainer.h"
#include "IWindow.h"
#include "XUtil.h"

#define INHERIT_IDesktopWindow(iname) \
    INHERIT_IContainer(iname); \
    boolean   (*HandleAppEvent)(iname* po, AEEEvent evt, uint16 wParam, uint32 dwParam); \
    void      (*SetDisplay)(iname* po, IDisplay* pIDisplay); \
    IDisplay* (*GetDisplay)(iname* po)

AEEINTERFACE_DEFINE(IDesktopWindow);

#define IDesktopWindow_HandleEvent(po, e, w, dw) AEEGETPVTBL(po, IDesktopWindow)->HandleAppEvent(po, e, w, dw)
#define IDesktopWindow_SetDisplay(po, display)   AEEGETPVTBL(po, IDesktopWindow)->SetDisplay(po, display)
#define IDesktopWindow_GetDisplay(po)            AEEGETPVTBL(po, IDesktopWindow)->GetDisplay(po)

static __inline IContainer* IDESKTOPWINDOW_TO_ICONTAINER(IDesktopWindow* po)
{
    return (IContainer*)po;
}

static __inline IWidget* IDESKTOPWINDOW_TO_IWIDGET(IDesktopWindow* po)
{
    return (IWidget*)po;
}

static __inline void IDesktopWindow_AddWindow(IDesktopWindow* po, IWindow* pWnd)
{
    IContainer_Insert((IContainer*)po, (IWidget*)pWnd, -1, NULL);
}

static __inline void IDesktopWindow_RemoveWindow(IDesktopWindow* po, IWindow* pWnd)
{
    IContainer_RemoveChild((IContainer*)po, (IWidget*)pWnd);
}

static __inline IWindow* IDesktopWindow_GetActiveWindow(IDesktopWindow* po)
{
    return (IWindow*)IContainer_GetChild((IContainer*)po, IContainer_GetChildCount((IContainer*)po) - 1);
}

static __inline void IDesktopWindow_ActivateWindow(IDesktopWindow* po, IWindow* pWnd)
{
    if (pWnd != IDesktopWindow_GetActiveWindow(po))
    {
        ADDREFIF(pWnd);
        IDesktopWindow_RemoveWindow(po, pWnd);
        IDesktopWindow_AddWindow(po, pWnd);
        RELEASEIF(pWnd);
    }
}

#endif // __IDESKTOPWINDOW_H__
