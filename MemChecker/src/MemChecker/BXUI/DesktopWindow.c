/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "DesktopWindow.h"
#include "XUtil.h"

IDesktopWindow* CDesktopWindow_New()
{
    return (IDesktopWindow*)NewVT(CDesktopWindow, IDesktopWindow);
}

void CDesktopWindow_Ctor(CDesktopWindow* pMe)
{
    DECLARE_PVTBL(pvt, IDesktopWindow);

    CContainer_Ctor((CContainer*)pMe);

    VT_FUNC(pvt, CDesktopWindow, Release);
    VT_FUNC(pvt, CDesktopWindow, Invalidate);

    VT_FUNC(pvt, CDesktopWindow, HandleAppEvent);
    VT_FUNC(pvt, CDesktopWindow, SetDisplay);
    VT_FUNC(pvt, CDesktopWindow, GetDisplay);
}

uint32 CDesktopWindow_Release(IDesktopWindow* po)
{
    return CContainer_Release((IContainer*)po);
}

static void CDesktopWindow_Redraw(CDesktopWindow* pMe)
{
    DrawContext stDrawContext;

    DBGPRINTF("##CDesktopWindow_Redraw -- x: %d, y: %d, dx: %d, dy: %d", 
        pMe->m_rcInvalid.x, pMe->m_rcInvalid.y, 
        pMe->m_rcInvalid.dx, pMe->m_rcInvalid.dy);
    AEE_OffsetRect(&pMe->m_rcInvalid, pMe->base.base.m_rc.x, pMe->base.base.m_rc.y);
    IDISPLAY_SetClipRect(pMe->m_pIDisplay, &pMe->m_rcInvalid);

    stDrawContext.pIDisplay = pMe->m_pIDisplay;
    stDrawContext.x = pMe->base.base.m_rc.x;
    stDrawContext.y = pMe->base.base.m_rc.y;
    IWidget_Draw((IWidget*)pMe, &stDrawContext);

    IDISPLAY_UpdateEx(pMe->m_pIDisplay, FALSE);
    IDISPLAY_SetClipRect(pMe->m_pIDisplay, NULL);
    AEE_SetRectEmpty(&pMe->m_rcInvalid);
}

void CDesktopWindow_Invalidate(IDesktopWindow* po, IWidget* piWidget, const AEERect* prc)
{
    DECLARE_ME(CDesktopWindow);
    AEERect rcInvalid;
    AEERect rcWidget;

    if (piWidget)
    {
        IWidget_GetRect(piWidget, &rcWidget);
        if (NULL == prc) {
            rcInvalid = rcWidget;
        } else {
            rcInvalid = *prc;
            AEE_OffsetRect(&rcInvalid, rcWidget.x, rcWidget.y);
        }
        AEE_OffsetRect(&rcInvalid, pMe->base.base.m_rcClient.x, pMe->base.base.m_rcClient.y);

        if (AEE_IsRectEmpty(&pMe->m_rcInvalid)) {
            pMe->m_rcInvalid = rcInvalid;
        } else {
            AEE_UnionRect(&pMe->m_rcInvalid, &pMe->m_rcInvalid, &rcInvalid);
        }
    }
    else
    {
        pMe->m_rcInvalid = pMe->base.base.m_rc;
    }

    if (!AEE_IsRectEmpty(&pMe->m_rcInvalid))
    {
        CALLBACK_Init(&pMe->m_cbUpdate, CDesktopWindow_Redraw, pMe);
        ISHELL_Resume(GetShell(), &pMe->m_cbUpdate);
    }
}

boolean CDesktopWindow_HandleAppEvent(IDesktopWindow* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    {
        AEEEvent aSysEvent[] = {
            EVT_POINTER_DOWN, 
            EVT_POINTER_MOVE, 
            EVT_POINTER_UP, 
            EVT_KEY, 
            EVT_KEY_PRESS, 
            EVT_KEY_RELEASE
        };
        AEEEvent aXEvent[] = {
            XEVT_MOUSEDOWN, 
            XEVT_MOUSEMOVE, 
            XEVT_MOUSEUP, 
            XEVT_KEY, 
            XEVT_KEYDOWN, 
            XEVT_KEYUP
        };
        int nIndex = 0;

        Array_Find(aSysEvent, ARR_SIZE(aSysEvent), &eCode, cmpfn_memory, sizeof(eCode), nIndex);
        if (-1 != nIndex) {
            return IWidget_HandleEvent((IWidget*)po, aXEvent[nIndex], wParam, dwParam);
        }
    }

    {
        XEventInfo evt;
        evt.eCode = eCode;
        evt.wParam = wParam;
        evt.dwParam = dwParam;
        return IWidget_HandleEvent((IWidget*)po, XEVT_SYSTEM, eCode, (uint32)&evt);
    }
}

void CDesktopWindow_SetDisplay(IDesktopWindow* po, IDisplay* pIDisplay)
{
    DECLARE_ME(CDesktopWindow);
    pMe->m_pIDisplay = pIDisplay;
}

IDisplay* CDesktopWindow_GetDisplay(IDesktopWindow* po)
{
    DECLARE_ME(CDesktopWindow);
    return pMe->m_pIDisplay;
}

