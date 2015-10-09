/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "ScrollBar.h"
#include "XUtil.h"
#include "AEEPointerHelpers.h"
#include "WidgetEvent.h"

IWidget* CScrollBar_New()
{
    return (IWidget*)NewVT(CScrollBar, IWidget);
}

void CScrollBar_Ctor(CScrollBar *pMe)
{
    DECLARE_PVTBL(pvt, IWidget);

    CControl_Ctor(&pMe->base);

    VT_FUNC(pvt, CScrollBar, Release);
    VT_FUNC(pvt, CScrollBar, Draw);

    HookHandler_Init(&pMe->base.m_hookHandler, CScrollBar_HandleEvent, pMe, NULL);

    pMe->m_eStyle = SBS_VERT;

    pMe->m_nMin = 0;
    pMe->m_nMax = 99;
    CScrollBar_SetPage(pMe, 10);
}

uint32 CScrollBar_Release(IWidget* po)
{
    return CControl_Release(po);
}

static __inline void CScrollBar_Fire(CScrollBar* pMe, ESBAction eAction, int nValue)
{
    SBNotifyData sNotifyData = {0};
    sNotifyData.eAction = eAction;
    sNotifyData.nValue = nValue;
    IEventDispatcher_NotifyEx(pMe->base.m_pIEventDispatcher, XEVT_SCROLL, pMe, (uint32)&sNotifyData);
}

static __inline int CScrollBar_GetBarPos(CScrollBar* pMe)
{
    return ((pMe->m_eStyle == SBS_HORZ) ? pMe->base.m_rcClient.x : pMe->base.m_rcClient.y);
}

static __inline int CScrollBar_GetBarSize(CScrollBar* pMe)
{
    return ((pMe->m_eStyle  == SBS_HORZ) ? pMe->base.m_rcClient.dx : pMe->base.m_rcClient.dy);
}

static __inline int CScrollBar_GetScrollableSize(CScrollBar* pMe)
{
    return CScrollBar_GetBarSize(pMe) - pMe->m_nThumbSize;
}

static __inline int CScrollBar_GetScrollRange(CScrollBar* pMe)
{
    return pMe->m_nMax - pMe->m_nMin;
}

static __inline int CScrollBar_GetTotalRange(CScrollBar* pMe)
{
    return CScrollBar_GetScrollRange(pMe) + pMe->m_nPage;
}

static __inline void CScrollBar_AdjustValue(CScrollBar* pMe)
{
    pMe->m_nValue = CONSTRAIN(pMe->m_nValue, pMe->m_nMin, pMe->m_nMax);
}

static void CScrollBar_SetValueInternal(CScrollBar* pMe, int nValue)
{
    pMe->m_nValue = nValue;
    CScrollBar_AdjustValue(pMe);

    DBGPRINTF("##CScrollBar_SetValue: %d", pMe->m_nValue);
}

static void CScrollBar_CalcThumbSize(CScrollBar* pMe)
{
    int nBarSize = CScrollBar_GetBarSize(pMe);
    int nTotalRange = CScrollBar_GetScrollRange(pMe) + pMe->m_nPage;
    if (nTotalRange == 0)
    {
        pMe->m_nThumbSize = nBarSize; // Avoid Error "Divided by Zero"
    }
    else
    {
        double dbBarSize = FASSIGN_INT(nBarSize);
        double dbPage = FASSIGN_INT(pMe->m_nPage);
        double dbTotalRange = FASSIGN_INT(nTotalRange);
        double dbTemp = FMUL(dbBarSize, FDIV(dbPage, dbTotalRange));

        pMe->m_nThumbSize = FLTTOINT(dbTemp); // "(double)nBarSize * pMe->m_nPage / nTotalRange"
    }

    pMe->m_nThumbSize = CONSTRAIN(pMe->m_nThumbSize, 5, nBarSize);
}

static void CScrollBar_SetThumbPos(CScrollBar* pMe, int nPos)
{
    pMe->m_nThumbPos = CONSTRAIN(nPos, 0, CScrollBar_GetScrollableSize(pMe));
}

static int CScrollBar_GetValueFromPos(CScrollBar* pMe, int nPos)
{
    double dbRange = FASSIGN_INT(CScrollBar_GetScrollRange(pMe));
    double dbPos = FASSIGN_INT(nPos);
    double dbScrollableSize = FASSIGN_INT(CScrollBar_GetScrollableSize(pMe));
    double dbTemp = FMUL(dbRange, FDIV(dbPos, dbScrollableSize));

    // "pMe->m_nMin + (int)(CScrollBar_GetRange(pMe) * (double)nPos / CScrollBar_GetScrollableSize(pMe) + 0.5)"
    return pMe->m_nMin + FLTTOINT(FADD(dbTemp, 0.5));
}

static int CScrollBar_GetPosFromValue(CScrollBar* pMe, int nValue)
{
    double dbScrollableSize = FASSIGN_INT(CScrollBar_GetScrollableSize(pMe));
    double dbValue = FASSIGN_INT(nValue - pMe->m_nMin);
    double dbRange = CScrollBar_GetScrollRange(pMe);
    double dbTemp = FMUL(dbScrollableSize, FDIV(dbValue, dbRange));

    // "CScrollBar_GetScrollableSize(pMe) * (double)(nValue - pMe->m_nMin) / CScrollBar_GetRange(pMe)"
    return FLTTOINT(dbTemp);
}

static void CScrollBar_GetThumbRect(CScrollBar* pMe, AEERect* prc)
{
    if (pMe->m_eStyle == SBS_HORZ)
    {
        prc->x = pMe->base.m_rcClient.x + (int16)pMe->m_nThumbPos;
        prc->y = pMe->base.m_rcClient.y;
        prc->dx = (int16)pMe->m_nThumbSize;
        prc->dy = pMe->base.m_rcClient.dy;
    }
    else if (pMe->m_eStyle == SBS_VERT)
    {
        prc->x = pMe->base.m_rcClient.x;
        prc->y = pMe->base.m_rcClient.y + (int16)pMe->m_nThumbPos;
        prc->dx = pMe->base.m_rcClient.dx;
        prc->dy = (int16)pMe->m_nThumbSize;
    }
}

static ESBAction CScrollBar_ActionHitTest(CScrollBar* pMe, int x, int y)
{
    int nPosTest = 0;

    if (pMe->m_eStyle == SBS_HORZ)
    {
        nPosTest = x;
    }
    else if (pMe->m_eStyle == SBS_VERT)
    {
        nPosTest = y;
    }
    nPosTest -= CScrollBar_GetBarPos(pMe);

    if (nPosTest < pMe->m_nThumbPos)
    {
        return SB_PAGEDEC;
    }
    else if (nPosTest < pMe->m_nThumbPos + pMe->m_nThumbSize)
    {
        return SB_THUMBTRACK;
    }
    else
    {
        return SB_PAGEINC;
    }
}

static boolean CScrollBar_HandlePointerEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CScrollBar);
    int x = AEE_POINTER_GET_X((char*)dwParam);
    int y = AEE_POINTER_GET_Y((char*)dwParam);

    switch (eCode)
    {
    case XEVT_MOUSEDOWN:
        if (IWidget_HitTest(po, x, y))
        {
            ESBAction eAction;

            pMe->m_bPointerDown = TRUE;

            eAction = CScrollBar_ActionHitTest(pMe, x, y);
            switch(eAction)
            {
            case SB_PAGEDEC:
            case SB_PAGEINC:
                {
                    CScrollBar_SetValue(pMe, pMe->m_nValue + pMe->m_nPage * ((SB_PAGEDEC == eAction) ? -1 : 1));
                    CScrollBar_Fire(pMe, eAction, pMe->m_nValue);
                }
                break;
            case SB_THUMBTRACK:
                pMe->m_bTracking = TRUE;
                pMe->m_nTrackOffset = ((SBS_HORZ == pMe->m_eStyle) ? x : y)
                    - CScrollBar_GetBarPos(pMe)
                    - pMe->m_nThumbPos;
                break;
            }

            return TRUE;
        }

        break;
    case XEVT_MOUSEMOVE:
        if (pMe->m_bPointerDown)
        {
            if (pMe->m_bTracking)
            {
                if (pMe->m_eStyle == SBS_HORZ)
                {
                    CScrollBar_SetThumbPos(pMe, x - pMe->m_nTrackOffset - CScrollBar_GetBarPos(pMe));
                }
                else if (pMe->m_eStyle == SBS_VERT)
                {
                    CScrollBar_SetThumbPos(pMe, y - pMe->m_nTrackOffset - CScrollBar_GetBarPos(pMe));
                }
                CScrollBar_SetValueInternal(pMe, CScrollBar_GetValueFromPos(pMe, pMe->m_nThumbPos));
                IWidget_Invalidate(po, NULL);
                CScrollBar_Fire(pMe, SB_THUMBTRACK, pMe->m_nValue);
            }

            return TRUE;
        }

        break;
    case XEVT_MOUSEUP:
        if (pMe->m_bPointerDown)
        {
            pMe->m_bPointerDown = FALSE;

            pMe->m_bTracking = FALSE;
            return TRUE;
        }

        break;

    default:
        break;
    }

    return FALSE;
}

boolean CScrollBar_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CScrollBar);

    switch (eCode)
    {
    case XEVT_SETPROPERTY:
        if (PROP_CLIENTRECT == wParam)
        {
            CScrollBar_CalcThumbSize(pMe);
            return TRUE;
        }
        break;
    case XEVT_KEY:
        switch (wParam)
        {
        case AVK_LEFT:
        case AVK_RIGHT:
            if (pMe->m_eStyle == SBS_HORZ)
            {
                CScrollBar_SetValue(pMe, pMe->m_nValue + (wParam == AVK_LEFT ? -1 : 1));
                CScrollBar_Fire(pMe, SB_LINEDEC + (wParam - AVK_LEFT), pMe->m_nValue);
                return TRUE;
            }
            break;

        case AVK_UP:
        case AVK_DOWN:
            if (pMe->m_eStyle == SBS_VERT)
            {
                CScrollBar_SetValue(pMe, pMe->m_nValue + (wParam == AVK_UP ? -1 : 1));
                CScrollBar_Fire(pMe, SB_LINEDEC + (wParam - AVK_UP), pMe->m_nValue);
                return TRUE;
            }
            break;

        default:
            break;
        }
        break;
    case POINTER_EVENT_CASE:
        if (CScrollBar_HandlePointerEvent(po, eCode, wParam, dwParam))
            return TRUE;
        break;

    default:
        break;
    }

    return CControl_DefHandleEvent(po, eCode, wParam, dwParam);
}

void CScrollBar_Draw(IWidget* po, DrawContext* pDrawContext)
{
    DECLARE_ME(CScrollBar);
    DECLARE_DRAWCONTEXT(pDrawContext);
    AEERect rcThumb;

    CControl_Draw(po, pDrawContext);

    CScrollBar_GetThumbRect(pMe, &rcThumb);
    AEE_OffsetRect(&rcThumb, x, y);
    IDISPLAY_FillRect(pIDisplay, &rcThumb, pMe->base.m_rgbFGColors[CURRENT_WIDGET_STATE(&pMe->base)]);
}

//==============================================================================
int CScrollBar_GetMin(CScrollBar* pMe)
{
    return pMe->m_nMin;
}

int CScrollBar_GetMax(CScrollBar* pMe)
{
    return pMe->m_nMax;
}

ESBStyle CScrollBar_GetOrientation(CScrollBar* pMe)
{
    return pMe->m_eStyle;
}

int CScrollBar_GetPage(CScrollBar* pMe)
{
    return pMe->m_nPage;
}

int CScrollBar_GetValue(CScrollBar* pMe)
{
    return pMe->m_nValue;
}

void CScrollBar_SetOrientation(CScrollBar* pMe, ESBStyle eOrientation)
{
    switch(eOrientation)
    {
    case SBS_HORZ:
    case SBS_VERT:
        pMe->m_eStyle = eOrientation;
        break;
    }
}

void CScrollBar_SetPage(CScrollBar* pMe, int nPage)
{
    if (nPage < 1 || CScrollBar_GetScrollRange(pMe) > MAX_INT32 - nPage)
        return;

    pMe->m_nPage = nPage;
    CScrollBar_CalcThumbSize(pMe);
    IWidget_Invalidate((IWidget *)pMe, NULL);
}

void CScrollBar_SetValue(CScrollBar* pMe, int nValue)
{
    CScrollBar_SetValueInternal(pMe, nValue);
    CScrollBar_SetThumbPos(pMe, CScrollBar_GetPosFromValue(pMe, pMe->m_nValue));
    IWidget_Invalidate((IWidget *)pMe, NULL);
}

void CScrollBar_SetRange(CScrollBar* pMe, int nMin, int nMax)
{
    if (nMin > nMax || nMax - nMin > MAX_INT32 - pMe->m_nPage)
        return;

    pMe->m_nMin = nMin;
    pMe->m_nMax = nMax;

    CScrollBar_AdjustValue(pMe);
    CScrollBar_CalcThumbSize(pMe);
    IWidget_Invalidate((IWidget *)pMe, NULL);
}
