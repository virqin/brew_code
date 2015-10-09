/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "Button.h"
#include "XUtil.h"
#include "AEEPointerHelpers.h"
#include "WidgetEvent.h"

IWidget* CButton_New()
{
    return (IWidget*)NewVT(CButton, IContainer);
}

void CButton_Ctor(CButton* pMe)
{
    DECLARE_PVTBL(pvt, IWidget);

    CContainer_Ctor(&pMe->base);

    VT_FUNC(pvt, CButton, Release);
    VT_FUNC(pvt, CButton, Draw);

    HookHandler_Init(&pMe->base.base.m_hookHandler, CButton_HandleEvent, pMe, NULL);

    SET_DEFAULT_WIDGET_STATE_PROP(&pMe->base.base, m_rgbBGColors, WIDGET, BGCOLOR);
}

uint32 CButton_Release(IWidget* po)
{
    if (1 == CBase_Refs(po))
    {
    }

    return CContainer_Release((IContainer*)po);
}

static __inline void CButton_Fire(CButton* pMe)
{
    IEventDispatcher_NotifyEx(pMe->base.base.m_pIEventDispatcher, XEVT_COMMAND, pMe, pMe->base.base.m_nID);
}

boolean CButton_HandlePointerEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CButton);
    int x = AEE_POINTER_GET_X((char*)dwParam);
    int y = AEE_POINTER_GET_Y((char*)dwParam);

    switch (eCode)
    {
    case XEVT_MOUSEDOWN:
        if (IWidget_HitTest(po, x, y))
        {
            pMe->m_bPressed = TRUE;
            IWidget_SetSelected(po, TRUE);
            return TRUE;
        }

        break;
    case XEVT_MOUSEMOVE:
        if (pMe->m_bPressed)
        {
            boolean bSelected = IWidget_HitTest(po, x, y);
            if (bSelected != pMe->base.base.m_bSelected) {
                IWidget_SetSelected(po, bSelected);
            }
            return TRUE;
        }

        break;
    case XEVT_MOUSEUP:
        if (pMe->m_bPressed)
        {
            pMe->m_bPressed = FALSE;
            IWidget_SetSelected(po, FALSE);
            if (IWidget_HitTest(po, x, y)) {
                CButton_Fire(pMe);
            }

            return TRUE;
        }

        break;

    default:
        break;
    }

    return FALSE;
}

boolean CButton_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CButton);

    switch (eCode)
    {
    case XEVT_KEY:
        switch (wParam)
        {
        case AVK_SELECT:
            CButton_Fire(pMe);
            pMe->m_bPressed = FALSE;
            return TRUE;
        }
        break;
    case POINTER_EVENT_CASE:
        if (CButton_HandlePointerEvent(po, eCode, wParam, dwParam))
            return TRUE;
        break;
    }

    return CContainer_HandleEvent((IContainer*)po, eCode, wParam, dwParam);
}

void CButton_Draw(IWidget* po, DrawContext* pDrawContext)
{
    CContainer_Draw((IContainer*)po, pDrawContext);
}
