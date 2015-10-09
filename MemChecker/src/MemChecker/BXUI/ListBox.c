/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "ListBox.h"
#include "XUtil.h"
#include "WidgetEvent.h"
#include "AEEPointerHelpers.h"
#include "StaticCtl.h"
#include "ListBox_IFace.h"

IWidget* CListBox_New()
{
    return (IWidget*)NewVT(CListBox, IWidget);
}

void CListBox_Ctor(CListBox *pMe)
{
    DECLARE_PVTBL(pvt, IWidget);

    CControl_Ctor(&pMe->base);

    VT_FUNC(pvt, CListBox, Release);
    VT_FUNC(pvt, CListBox, Draw);

    HookHandler_Init(&pMe->base.m_hookHandler, CListBox_HandleEvent, pMe, NULL);

    pMe->base.m_pDataObject = CVector_New();
    pMe->m_pChild = CStaticCtl_New();

    pMe->m_nItemSize = DEFAULT_LISTBOX_ITEMSIZE;
    IWidget_SetSize(pMe->m_pChild, pMe->base.m_rcClient.dx, pMe->m_nItemSize);
}

uint32 CListBox_Release(IWidget* po)
{
    DECLARE_ME(CListBox);

    if (1 == CBase_Refs(po))
    {
        RELEASEIF(pMe->m_pChild);
    }

    return CControl_Release(po);
}

static __inline boolean CListBox_IsVarItemSize(CListBox* pMe)
{
    return 0 != (pMe->base.m_dwFlags & LBF_VARIABLEITEMSIZE);
}

static __inline boolean CListBox_IsValidIndex(CListBox* pMe, int nIndex)
{
    return 0 < nIndex && nIndex < (int)IVector_Size(pMe->base.m_pDataObject);
}

static void CListBox_EnsureItemVisible(CListBox* pMe, int nIndex)
{
    AEERect rcItem;

    if (nIndex < pMe->m_nTopIndex)
    {
        CListBox_SetTopIndex(pMe, pMe->m_nTopIndex - 1);
        return;
    }

    CListBox_GetItemRect(pMe, nIndex, &rcItem);
    if (rcItem.y + rcItem.dy > pMe->base.m_rcClient.y + pMe->base.m_rcClient.dy)
    {
        CListBox_SetTopIndex(pMe, pMe->m_nTopIndex + 1);
    }
}

static __inline void CListBox_Fire(CListBox* pMe, uint32 nCode, uint32 dwParam)
{
    IEventDispatcher_NotifyEx(pMe->base.m_pIEventDispatcher, nCode, pMe, dwParam);
}

int CListBox_ItemHitTest(CListBox* pMe, int x, int y)
{
    int i, nCount;
    nCount = IVector_Size(pMe->base.m_pDataObject);
    for (i = pMe->m_nTopIndex; i < nCount; i++)
    {
        AEERect rcItem;
        CListBox_GetItemRect(pMe, (uint16)i, &rcItem);
        if (AEE_PtInRect(&rcItem, x, y))
            return i;
    }

    return -1;
}

boolean CListBox_HandlePointerEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CListBox);
    int x = AEE_POINTER_GET_X((char*)dwParam);
    int y = AEE_POINTER_GET_Y((char*)dwParam);

    switch (eCode)
    {
    case XEVT_MOUSEDOWN:
        if (IWidget_HitTest(po, x, y))
        {
            int nIndex = 0;

            pMe->m_bPointerDown = TRUE;
            nIndex = CListBox_ItemHitTest(pMe, x, y);
            if (-1 != nIndex)
            {
                CListBox_EnsureItemVisible(pMe, nIndex);
                IWidget_SetFocusIndex(po, nIndex);
            }
            return TRUE;
        }

        break;
    case XEVT_MOUSEMOVE:
        if (pMe->m_bPointerDown)
        {
            return TRUE;
        }

        break;
    case XEVT_MOUSEUP:
        if (pMe->m_bPointerDown)
        {
            pMe->m_bPointerDown = FALSE;

            if (IWidget_HitTest(po, x, y) 
                && pMe->m_nFocusIndex == CListBox_ItemHitTest(pMe, x, y))
                CListBox_Fire(pMe, XEVT_CLICK, 0);

            return TRUE;
        }

        break;

    default:
        break;
    }

    return FALSE;
}

boolean CListBox_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CListBox);

    switch (eCode)
    {
    case XEVT_GETPROPERTY:
        switch(wParam)
        {
        case PROP_FOCUSINDEX:
            *(int*)dwParam = pMe->m_nFocusIndex;
            return TRUE;
        default:
            break;
        }
        break;

    case XEVT_SETPROPERTY:
        switch(wParam)
        {
        case PROP_FOCUSINDEX:
        case PROP_SELECTINDEX:
            {
                int nNewFocusIndex = (int)dwParam;
                if (nNewFocusIndex != pMe->m_nFocusIndex)
                {
                    if (CListBox_IsValidIndex(pMe, nNewFocusIndex)) {
                        pMe->m_nFocusIndex = nNewFocusIndex;
                    } else {
                        pMe->m_nFocusIndex = -1;
                    }
                    CListBox_Fire(pMe, XEVT_FOCUS_CHANGE, 0);
                    if (PROP_SELECTINDEX == wParam) {
                        CListBox_Fire(pMe, XEVT_FOCUS_SELECT, 0);
                    }
                    IWidget_Invalidate(po, NULL);
                    return TRUE;
                }
            }
            break;

        default:
            break;
        }
        break;

    case XEVT_KEY:
        switch (wParam)
        {
        case AVK_UP:
            {
                int nFocusIndex = pMe->m_nFocusIndex - 1;
                if (CListBox_IsValidIndex(pMe, nFocusIndex))
                {
                    CListBox_EnsureItemVisible(pMe, nFocusIndex);
                    IWidget_SetFocusIndex(po, nFocusIndex);
                }
            }
            return TRUE;

        case AVK_DOWN:
            {
                int nFocusIndex = pMe->m_nFocusIndex + 1;
                if (CListBox_IsValidIndex(pMe, nFocusIndex))
                {
                    CListBox_EnsureItemVisible(pMe, nFocusIndex);
                    IWidget_SetFocusIndex(po, nFocusIndex);
                }
            }
            return TRUE;

        case AVK_SELECT:
            CListBox_Fire(pMe, XEVT_FOCUS_SELECT, 0);
            return TRUE;

        default:
            break;
        }
        break;

    case POINTER_EVENT_CASE:
        if (CListBox_HandlePointerEvent(po, eCode, wParam, dwParam))
            return TRUE;
        break;

    default:
        break;
    }

    return CControl_DefHandleEvent(po, eCode, wParam, dwParam);
}

void CListBox_Draw(IWidget* po, DrawContext* pDrawContext)
{
    DECLARE_ME(CListBox);
    DECLARE_DRAWCONTEXT(pDrawContext);
    int nCount = 0;
    int i = 0;
    AEERect rcDisp, rcClient;

    CControl_Draw(po, pDrawContext);

    if (NULL == pMe->m_pChild) {
        return;
    }

    IDISPLAY_GetClipRect(pIDisplay, &rcDisp);
    rcClient = pMe->base.m_rcClient;
    AEE_OffsetRect(&rcClient, x, y);
    if (!AEE_IntersectRect(&rcDisp, &rcDisp, &rcClient)) {
        return;
    }

    nCount = IVector_Size(pMe->base.m_pDataObject);
    for (i = pMe->m_nTopIndex; i < nCount; i++)
    {
        void *pItemData = NULL;
        AEERect rcItem, rcClip;

        CListBox_GetItemRect(pMe, i, &rcItem);
        if (rcItem.y > pMe->base.m_rcClient.dy)
            break;
        AEE_OffsetRect(&rcItem, x + pMe->base.m_rcClient.x, y + pMe->base.m_rcClient.y);
        if (AEE_IntersectRect(&rcClip, &rcDisp, &rcItem))
        {
            DrawContext stItemDC = *pDrawContext;

            stItemDC.x = rcItem.x;
            stItemDC.y = rcItem.y;
            IDISPLAY_SetClipRect(pIDisplay, &rcClip);
            IWidget_SetPosition(pMe->m_pChild, rcItem.x, rcItem.y);
            IVector_GetAt(pMe->base.m_pDataObject, i, &pItemData);
            IWidget_SetValueData(pMe->m_pChild, pItemData, -1, NULL);
            IWidget_SetSelected(pMe->m_pChild, (boolean)(pMe->m_nFocusIndex == i));
            IWidget_Draw(pMe->m_pChild, &stItemDC);
        }
    }

    return;
}

//==============================================================================
int CListBox_GetItemIndex(CListBox* pMe, uint16 uiItemID)
{
    int nErr = SUCCESS;
    int i = 0;
    CtlAddItem *pai = NULL;
    int nCount = IVector_Size(pMe->base.m_pDataObject);

    for (i = 0; i < nCount; i++)
    {
        nErr = IVector_GetAt(pMe->base.m_pDataObject, i, &pai);
        if (SUCCESS != nErr)
        {
            continue;
        }

        if (pai->wItemID == uiItemID)
        {
            return i;
        }
    }

    return -1;
}

void CListBox_GetItemRect(CListBox * pMe, int nIndex, AEERect* prc)
{
    if (CListBox_IsVarItemSize(pMe))
    {
        int i;

        *prc = pMe->base.m_rcClient;
        for (i = pMe->m_nTopIndex; i <= nIndex; i++)
        {
            prc->dy = (int16)pMe->m_nItemSize;
            prc->y += prc->dy;
        }
        prc->y -= prc->dy;
    }
    else
    {
        SETAEERECT(prc, 
            0, 
            pMe->m_nItemSize * (nIndex - pMe->m_nTopIndex), 
            pMe->base.m_rcClient.dx, 
            pMe->m_nItemSize);
    }
}

int CListBox_GetTopIndex(CListBox * pMe)
{
    return pMe->m_nTopIndex;
}

int CListBox_GetVisibleCount(CListBox* pMe)
{
    int i, nCount;

    nCount = IVector_Size(pMe->base.m_pDataObject);
    for (i = pMe->m_nTopIndex; i < nCount; i++)
    {
        AEERect rcItem;

        CListBox_GetItemRect(pMe, i, &rcItem);
        if (rcItem.y + rcItem.dy > pMe->base.m_rcClient.y + pMe->base.m_rcClient.dy)
            break;
    }

    return i - pMe->m_nTopIndex;
}

void CListBox_SetTopIndex(CListBox * pMe, int nIndex)
{
    if (!CListBox_IsValidIndex(pMe, nIndex))
        return;

    pMe->m_nTopIndex = nIndex;
}
