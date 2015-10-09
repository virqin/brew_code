/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "Container.h"
#include "XUtil.h"
#include "AEEPointerHelpers.h"

IContainer* CContainer_New()
{
    return (IContainer*)NewVT(CContainer, IContainer);
}

void CContainer_Ctor(CContainer* pMe)
{
    DECLARE_PVTBL(pvt, IContainer);

    CControl_Ctor(&pMe->base);

    VT_FUNC(pvt, CContainer, Release);
    VT_FUNC(pvt, CContainer, Draw);

    VT_FUNC(pvt, CContainer, Invalidate);
    VT_FUNC(pvt, CContainer, Insert);
    VT_FUNC(pvt, CContainer, Remove);
    VT_FUNC(pvt, CContainer, GetChildCount);
    VT_FUNC(pvt, CContainer, GetChild);

    HookHandler_Init(&pMe->base.m_hookHandler, CContainer_HandleEvent, pMe, NULL);

    pMe->base.m_nBorderWidth = DEFAULT_CONTAINER_BORDERWIDTH;

    SET_DEFAULT_WIDGET_STATE_PROP(&pMe->base, m_rgbBGColors, CONTAINER, BGCOLOR);

    pMe->m_lstChild = CVector_New();
    IVector_SetPfnFree(pMe->m_lstChild, ReleaseObject);
}

uint32 CContainer_Release(IContainer* po)
{
    DECLARE_ME(CContainer);

    if (1 == CBase_Refs(po))
    {
        RELEASEIF(pMe->m_lstChild);
    }

    return CControl_Release((IWidget*)po);
}

static __inline boolean Widget_CanTakeFocus(IWidget* piWidget)
{
    return IWidget_IsVisible(piWidget) && IWidget_CanTakeFocus(piWidget);
}

static boolean Widget_InContainerTree(IWidget* piWidget, IContainer* piContainer)
{
    while (piWidget)
    {
        piWidget = (IWidget*)IWidget_GetParent(piWidget);
        if (piContainer == (IContainer*)piWidget) {
            return TRUE;
        }
    }

    return FALSE;
}

static IWidget* CContainer_FindFocusNodeFromIndex(CContainer* pMe, int nIndex, boolean bForward)
{
    int i = 0;
    int nCount = CContainer_GetChildCount((IContainer*)pMe);
    IWidget* piWidget = NULL;

    for (i = nIndex; 
        bForward ? (i < nCount) : (i >= 0); 
        bForward ? i++ : i--)
    {
        IVector_GetAt(pMe->m_lstChild, i, &piWidget);
        if (piWidget && Widget_CanTakeFocus(piWidget)) {
            return piWidget;
        }
    }

    return NULL;
}

static IWidget* CContainer_FindNextFocusNode(CContainer* pMe, IWidget* piwStart, boolean bForward)
{
    int nIndex = IContainer_GetChildIndex((IContainer*)pMe, piwStart);

    if (-1 != nIndex) {
        nIndex += (bForward ? 1 : -1);
        return CContainer_FindFocusNodeFromIndex(pMe, nIndex, bForward);
    }

    return NULL;
}

static boolean CContainer_MoveFocus(CContainer* pMe, IWidget* piWidget)
{
    IWidget* piwOldFocus = pMe->m_piwFocus;
    boolean bResult = FALSE;

    if (NULL == pMe->m_piwFocus)
    {
        if (WIDGET_FOCUS_NEXT == piWidget) {
            piWidget = WIDGET_FOCUS_FIRST;
        } else if (WIDGET_FOCUS_PREV == piWidget) {
            piWidget = WIDGET_FOCUS_LAST;
        }
    }

    if (WIDGET_FOCUS_NONE == piWidget)
    {
        bResult = TRUE;
    }
    else
    {
        if (WIDGET_FOCUS_FIRST == piWidget)
        {
            piWidget = CContainer_FindFocusNodeFromIndex(pMe, 0, TRUE);
        }
        else if (WIDGET_FOCUS_LAST == piWidget)
        {
            piWidget = CContainer_FindFocusNodeFromIndex(pMe, 
                CContainer_GetChildCount((IContainer*)pMe) - 1, FALSE);
        }
        else if (WIDGET_FOCUS_PREV == piWidget)
        {
            piWidget = CContainer_FindNextFocusNode(pMe, pMe->m_piwFocus, FALSE);
        }
        else if (WIDGET_FOCUS_NEXT == piWidget)
        {
            piWidget = CContainer_FindNextFocusNode(pMe, pMe->m_piwFocus, TRUE);
        }
        else
        {
            boolean bFound = FALSE;

            if (IWidget_CanTakeFocus(piWidget))
            {
                IWidget* piwNode = NULL;
                int nCount = CContainer_GetChildCount((IContainer*)pMe);
                int i = 0;
                for (i = 0; i < nCount; i++)
                {
                    piwNode = CContainer_GetChild((IContainer*)pMe, i);
                    if (piwNode == piWidget) {
                        bFound = TRUE;
                        break;
                    }

                    if (Widget_InContainerTree(piWidget, (IContainer*)piwNode)) {
                        IContainer_MoveFocus((IContainer*)piwNode, piWidget);
                        bFound = TRUE;
                        break;
                    }
                }
            }

            if (!bFound) {
                piWidget = NULL;
            }
        }

        bResult = (0 != piWidget);
    }

    if (bResult && piWidget != piwOldFocus)
    {
        if (piWidget) {
            IWidget_SetActive(piWidget, TRUE);
        }

        if (piwOldFocus) {
            IWidget_SetActive(piwOldFocus, FALSE);
        }

        pMe->m_piwFocus = piWidget;
    }

    return bResult;
}

boolean CContainer_HandlePointerEvent(CContainer* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    int x, y;
    int nCount = IVector_Size(pMe->m_lstChild);

    AEE_POINTER_GET_XY((char*)dwParam, &x, &y);
    x -= pMe->base.m_rcClient.x;
    y -= pMe->base.m_rcClient.y;
    DBGPRINTF("CContainer_HandlePointerEvent: eCode=%d --%s-- x=%d, y=%d", eCode, IWidget_GetName((IWidget*)pMe), x, y);

    while (nCount-- > 0)
    {
        IWidget* piWidget = NULL;
        AEERect rc = {0};

        IVector_GetAt(pMe->m_lstChild, nCount, &piWidget);
        if (NULL == piWidget) {
            continue;
        }

        IWidget_GetRect(piWidget, &rc);
        AEE_POINTER_SET_XY((char*)dwParam, wParam, x - rc.x, y - rc.y);

        if (XEVT_MOUSEDOWN == eCode)
        {
            if (IWidget_IsVisible(piWidget)
                && IWidget_IsEnabled(piWidget)
                && IWidget_HitTest(piWidget, x - rc.x, y - rc.y))
            {
                DBGPRINTF("CContainer_HandlePointerEvent: --%s-- x=%d, y=%d -- being hit.", IWidget_GetName(piWidget), x - rc.x, y - rc.y);
                CContainer_MoveFocus(pMe, piWidget);
                return IWidget_HandleEvent(piWidget, eCode, wParam, dwParam);
            }
        }
        else
        {
            if (IWidget_HandleEvent(piWidget, eCode, wParam, dwParam)) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

boolean CContainer_HandleEvent(IContainer* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CContainer);

    switch (eCode)
    {
    case XEVT_GETPROPERTY:
        switch(wParam)
        {
        case PROP_FOCUS:
            *(IWidget**)dwParam = pMe->m_piwFocus;
        	return TRUE;

        default:
            break;
        }
    case XEVT_SETPROPERTY:
        switch (wParam)
        {
        case PROP_FOCUS:
            CContainer_MoveFocus(pMe, (IWidget*)dwParam);
            return TRUE;

        default:
            break;
        }

    default:
        break;
    }

    if (CControl_DefHandleEvent((IWidget*)po, eCode, wParam, dwParam)) {
        return TRUE;
    }

    // move the focus to first control
    if (XEVT_SETPROPERTY == eCode 
        && PROP_ACTIVE == wParam
        && NULL == pMe->m_piwFocus) {
        return CContainer_MoveFocus(pMe, WIDGET_FOCUS_FIRST);
    }

    if (AEE_IsPointerEvent(eCode))
    {
        if (CContainer_HandlePointerEvent(pMe, eCode, wParam, dwParam)) {
            return TRUE;
        }
    }
    else
    {
        if (pMe->m_piwFocus != NULL 
            && IWidget_IsVisible(pMe->m_piwFocus) 
            && IWidget_HandleEvent(pMe->m_piwFocus, eCode, wParam, dwParam))
            return TRUE;
    }

    // default to switch focus through direction key
    switch (eCode)
    {
    case XEVT_KEY:
        switch (wParam)
        {
        case AVK_LEFT:
        case AVK_UP:
            if (CContainer_MoveFocus(pMe, WIDGET_FOCUS_PREV)) {
                return TRUE;
            }
            break;

        case AVK_RIGHT:
        case AVK_DOWN:
            if (CContainer_MoveFocus(pMe, WIDGET_FOCUS_NEXT)) {
                return TRUE;
            }
            break;

        default:
            break;
        }

        break;
    }

    return FALSE;
}

void CContainer_Draw(IContainer* po, DrawContext* pDrawContext)
{
    DECLARE_ME(CContainer);
    DECLARE_DRAWCONTEXT(pDrawContext);
    int nCount = IVector_Size(pMe->m_lstChild);
    int i = 0;
    AEERect rcDisp, rcClient;

    CControl_Draw((IWidget*)po, pDrawContext);

    IDISPLAY_GetClipRect(pIDisplay, &rcDisp);
    rcClient = pMe->base.m_rcClient;
    AEE_OffsetRect(&rcClient, x, y);
    if (!AEE_IntersectRect(&rcDisp, &rcDisp, &rcClient)) {
        return;
    }

    for (i = 0; i < nCount; i++)
    {
        IWidget* piWidget = NULL;
        AEERect rcDraw, rcClip;

        IVector_GetAt(pMe->m_lstChild, i, &piWidget);
        IWidget_GetRect(piWidget, &rcDraw);
        AEE_OffsetRect(&rcDraw, x + pMe->base.m_rcClient.x, y + pMe->base.m_rcClient.y);
        if (IWidget_IsVisible(piWidget) 
            && AEE_IntersectRect(&rcClip, &rcDisp, &rcDraw))
        {
            DrawContext stWidgetDC = *pDrawContext;
            stWidgetDC.x = rcDraw.x;
            stWidgetDC.y = rcDraw.y;
            IDISPLAY_SetClipRect(pIDisplay, &rcClip);
            IWidget_Draw(piWidget, &stWidgetDC);

#if 0
            // Debug code, start
            if (IWidget_GetName(piWidget)) {
                RGBVAL rgbOld = IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, 0xFF0000FF);
                int nSize = (STRLEN(IWidget_GetName(piWidget)) + 1) * 2;
                AECHAR *pwszName = MALLOC(nSize);
                STRTOWSTR(IWidget_GetName(piWidget), pwszName, nSize);
                IDISPLAY_SetClipRect(pIDisplay, &rcClip);
                IDISPLAY_DrawText(pIDisplay, AEE_FONT_LARGE, pwszName, -1, 0, 0, &rcDraw, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE | IDF_TEXT_TRANSPARENT);
                FREEIF(pwszName);
                IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, rgbOld);
            }
#endif
            // Debug code, end
        }
    }
}

//==============================================================================
void CContainer_Invalidate(IContainer* po, IWidget* piWidget, const AEERect* prc)
{
    DECLARE_ME(CContainer);

    if (pMe->base.m_pParent)
    {
        AEERect rcInvalid;
        AEERect rcWidget;

        IWidget_GetRect(piWidget, &rcWidget);
        if (NULL == prc) {
            rcInvalid = rcWidget;
        } else {
            rcInvalid = *prc;
            AEE_OffsetRect(&rcInvalid, rcWidget.x, rcWidget.y);
        }
        AEE_OffsetRect(&rcInvalid, pMe->base.m_rcClient.x, pMe->base.m_rcClient.y);

        IContainer_Invalidate(pMe->base.m_pParent, ICONTAINER_TO_IWIDGET(po), &rcInvalid);
    }
}

int CContainer_Insert(IContainer* po, IWidget* piWidget, int nIndex, uint32 dwLayout)
{
    DECLARE_ME(CContainer);
    int nErr = SUCCESS;

    if (NULL == piWidget) {
        return EBADPARM;
    }

    if (pMe->m_piLayoutMgr) {
        ILayoutMgr_AddChild(pMe->m_piLayoutMgr, piWidget, dwLayout);
    }

    nErr = IVector_Insert(pMe->m_lstChild, nIndex, piWidget);
    if (SUCCESS == nErr) {
        ADDREFIF(piWidget);
        IWidget_SetParent(piWidget, po);
    }

    return nErr;
}

int CContainer_Remove(IContainer* po, int nIndex)
{
    DECLARE_ME(CContainer);
    int nErr = SUCCESS;
    IWidget* piWidget = NULL;

    if (pMe->m_piLayoutMgr) {
        ILayoutMgr_RemoveChild(pMe->m_piLayoutMgr, IContainer_GetChild(po, nIndex));
    }

    nErr = IVector_GetAt(pMe->m_lstChild, nIndex, &piWidget);
    nErr = IVector_DeleteAt(pMe->m_lstChild, nIndex);
    if (SUCCESS != nErr) {
        return nErr;
    }

    if (piWidget) {
        IWidget_SetParent(piWidget, NULL);
    }

    if (piWidget == pMe->m_piwFocus) {
        pMe->m_piwFocus = NULL;
    }

    return nErr;
}

uint32 CContainer_GetChildCount(IContainer* po)
{
    DECLARE_ME(CContainer);

    return IVector_Size(pMe->m_lstChild);
}

IWidget* CContainer_GetChild(IContainer* po, int nIndex)
{
    DECLARE_ME(CContainer);
    IWidget* piWidget = NULL;

    IVector_GetAt(pMe->m_lstChild, nIndex, &piWidget);

    return piWidget;
}
