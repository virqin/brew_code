/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "Window.h"
#include "WidgetProp.h"
#include "Container.h"

IWindow* CWindow_New()
{
    return (IWindow*)NewVT(CWindow, IWindow);
}

void CWindow_Ctor(CWindow* pMe)
{
    CContainer_Ctor((CContainer*)pMe);
}

uint32 CWindow_Release(IWindow* po)
{
    if (1 == CBase_Refs(po))
    {
    }

    return CContainer_Release((IContainer*)po);
}

boolean CWindow_HandleEvent(IWindow* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    return CContainer_HandleEvent((IContainer*)po, eCode, wParam, dwParam);
}

void CWindow_Draw(IWindow* po, DrawContext* pDrawContext)
{
    CContainer_Draw((IContainer*)po, pDrawContext);
}