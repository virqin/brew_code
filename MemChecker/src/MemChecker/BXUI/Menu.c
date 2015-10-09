/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "Menu.h"
#include "XUtil.h"
#include "AEEPointerHelpers.h"
#include "ListBox_IFace.h"

IWidget *CMenu_New()
{
    return (IWidget *)NewVT(CMenu, IWidget);
}

void CMenu_Ctor(CMenu *pMe)
{
    DECLARE_PVTBL(pvt, IWidget);

    CControl_Ctor(&pMe->base);

    VT_FUNC(pvt, CMenu, Release);
    VT_FUNC(pvt, CMenu, Draw);

    HookHandler_Init(&pMe->base.m_hookHandler, CMenu_HandleEvent, pMe, NULL);

    pMe->base.m_bVisible = FALSE;

    pMe->base.m_bCanTakeFocus = FALSE;

    CListBox_New((IWidget **)&pMe->m_pList);

    pMe->m_xOffset = 5;
    pMe->m_yOffset = 5;
}

uint32 CMenu_Release(IWidget* po)
{
    DECLARE_ME(CMenu);
    int i, nCount;

    if (1 == CBase_Refs(po))
    {
        // Free the memory of CMenuItemInfo
        nCount = CMenu_GetItemCount(pMe);
        for (i = 0; i < nCount; i++)
        {
            int nErr = SUCCESS;
            CMenuItemInfo* pmii;
            nErr = IWidget_ListData_GetItemData((IWidget*)pMe->m_pList, CMenu_GetItemID(pMe, i), &pmii);
            if (SUCCESS == nErr)
            {
                FREEIF(pmii);
            }
        }

        RELEASEIF(pMe->m_pList);
    }

    return CControl_Release(po);
}

static __inline void CMenu_Fire(CMenu* pMe)
{
    if (CMenu_GetSel(pMe) != 0)
    {
        //BW_PostEvent(EVT_COMMAND, CMenu_GetSel(pMe), (uint32)pMe);
    }
}

int CMenu_ItemHitTest(CMenu* pMe, int x, int y)
{
    int i, nCount;
    nCount = CMenu_GetItemCount(pMe);
    for (i = 0; i < nCount; i++)
    {
        AEERect rcItem;
        CMenu_GetItemRect(pMe, (uint16)i, &rcItem);
        if (AEE_PtInRect(&rcItem, x, y))
            return i;
    }

    return -1;
}

boolean CMenu_HandlePointerEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CMenu);
    int x = AEE_POINTER_GET_X((char*)dwParam);
    int y = AEE_POINTER_GET_Y((char*)dwParam);

    switch (eCode)
    {
    case XEVT_MOUSEDOWN:
        if (IWidget_HitTest(po, x, y))
        {
            pMe->m_bPointerDown = TRUE;
            CMenu_SetSel(pMe, CMenu_GetItemID(pMe, CMenu_ItemHitTest(pMe, x, y)));
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
                && CMenu_GetSel(pMe) == CMenu_GetItemID(pMe, CMenu_ItemHitTest(pMe, x, y)))
                CMenu_Fire(pMe);

            return TRUE;
        }

        break;

    default:
        break;
    }

    return FALSE;
}

boolean CMenu_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CMenu);

    switch (eCode)
    {
    case XEVT_KEY:
        switch (wParam)
        {
        case AVK_UP:
        case AVK_DOWN:
            {
                int nFocusIndex = CMenu_GetItemIndex(pMe, CMenu_GetSel(pMe));
                switch (wParam)
                {
                case AVK_UP:
                    nFocusIndex--;
                    break;
                case AVK_DOWN:
                    nFocusIndex++;
                    break;
                }
                CMenu_SetSel(pMe, CMenu_GetItemID(pMe, nFocusIndex));
            }

            return TRUE;
        case AVK_SELECT:
            CMenu_Fire(pMe);
            return TRUE;
        }
        break;
    case POINTER_EVENT_CASE:
        if (CMenu_HandlePointerEvent(po, eCode, wParam, dwParam))
            return TRUE;
        break;
    }

    return FALSE;
}

void CMenu_Draw(IWidget* po, DrawContext* pDrawContext)
{
    DECLARE_ME(CMenu);
    DECLARE_DRAWCONTEXT(pDrawContext);
    int i, nCount;
    AEEImageInfo info;
    RGBVAL clrOld;
    int16 cxMaxImage = 0;
    int16 cxMaxText = 0;

    clrOld = IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_NONE);
    nCount = CMenu_GetItemCount(pMe);

    for (i = 0; i < nCount; i++)
    {
        CMenuItemInfo mii = {0};
        AEERect rcItem;
        int nFocusIndex = 0;
        CMenu_GetItemRect(pMe, (uint16)i, &rcItem);

        IWidget_GetFocusIndex((IWidget*)pMe->m_pList, &nFocusIndex);
        if (CMenu_GetItemID(pMe, i) == nFocusIndex)
        {
            AEE_InflateRect(&rcItem, -1, -1);
            AEE_InflateRect(&rcItem, 1, 1);
        }

        info.cx = 0;
        info.cy = 0;

        // Unchecked image
        if (mii.pImgUnchecked != NULL)
        {
            //BIImage_GetInfo(mii.pImgUnchecked, &info);
            cxMaxImage = MAX(cxMaxImage, info.cx);
//             if (!mii.bChecked)
//                 BIImage_Draw(mii.pImgUnchecked, rcItem.x + pMe->m_xOffset, rcItem.y + (rcItem.dy - info.cy) / 2);
        }

        // Checked image
        if (mii.pImgChecked != NULL)
        {
//             BIImage_GetInfo(mii.pImgChecked, &info);

            if (mii.pSubMenu == NULL)
                cxMaxImage = MAX(cxMaxImage, info.cx);

//             if (mii.bChecked)
//                 BIImage_Draw(mii.pImgChecked, rcItem.x + pMe->m_xOffset, rcItem.y + (rcItem.dy - info.cy) / 2);
        }

        cxMaxText = MAX(cxMaxText, (int16)IDISPLAY_MeasureText(pIDisplay, AEE_FONT_NORMAL, mii.pText));
    }

    if (cxMaxImage > 0) cxMaxImage += 5; // 图标与文字之间间隔距离

    for (i = 0; i < nCount; i++)
    {
        CMenuItemInfo mii = {0};
        AEERect rcItem;

        CMenu_GetItemRect(pMe, (uint16)i, &rcItem);

        IDISPLAY_DrawText(pIDisplay, AEE_FONT_NORMAL, 
            mii.pText, -1, 
            rcItem.x + pMe->m_xOffset + cxMaxImage, rcItem.y + pMe->m_yOffset, 
            &rcItem, IDF_TEXT_TRANSPARENT | IDF_ALIGN_MIDDLE);
        if (mii.pSubMenu != NULL)
        {
//             BIImage_GetInfo(mii.pImgChecked, &info);
//             BIImage_Draw(mii.pImgChecked, 
//                 rcItem.x + pMe->m_xOffset + cxMaxImage + cxMaxText + 5, 
//                 rcItem.y + (rcItem.dy - info.cy) / 2);
        }
    }

    IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, clrOld);
}

//==============================================================================
boolean CMenu_CheckItem(CMenu* pMe, uint16 nItemID, boolean bCheck)
{
    int nErr = SUCCESS;
    CMenuItemInfo* pmii;
    nErr = IWidget_ListData_GetItemData((IWidget*)pMe->m_pList, nItemID, &pmii);
    if (SUCCESS == nErr)
    {
        pmii->bChecked = bCheck;
        return TRUE;
    }

    return FALSE;
}

boolean CMenu_DeleteItem(CMenu* pMe, uint16 nItemID)
{
    int nErr = SUCCESS;
    CMenuItemInfo* pmii;
    nErr = IWidget_ListData_GetItemData((IWidget*)pMe->m_pList, nItemID, &pmii);
    if (SUCCESS == nErr)
    {
        FREEIF(pmii);
        nErr = IWidget_ListData_DeleteItem((IWidget*)pMe->m_pList, nItemID);
        if (SUCCESS == nErr) {
            return TRUE;
        }
    }

    return FALSE;
}

int CMenu_GetItemCount(CMenu* pMe)
{
    return IWidget_ListData_GetItemCount((IWidget*)pMe->m_pList);
}

boolean CMenu_GetItemData
(
 CMenu* pMe,
 uint16 nItemID,
 uint32* plData
 )
{
    int nErr = SUCCESS;
    CMenuItemInfo* pmii;
    nErr = IWidget_ListData_GetItemData((IWidget*)pMe->m_pList, nItemID, &pmii);
    if (SUCCESS == nErr)
    {
        *plData = pmii->dwData;
        return TRUE;
    }
    return FALSE;
}

uint16 CMenu_GetItemID(CMenu * pMe, int nIdx)
{
    return (uint16)-1;
}

IImage* CMenu_GetItemImage(CMenu* pMe, uint16 nItemID, int nImgType)
{
    int nErr = SUCCESS;
    CMenuItemInfo* pmii;
    nErr = IWidget_ListData_GetItemData((IWidget*)pMe->m_pList, nItemID, &pmii);
    if (SUCCESS == nErr)
    {
        switch (nImgType)
        {
        case 0:
            return pmii->pImgUnchecked;
            break;
        case 1:
            return pmii->pImgChecked;
            break;
        }
    }

    return NULL;
}

int CMenu_GetItemIndex(CMenu* pMe, uint16 nItemID)
{
    return CListBox_GetItemIndex(pMe->m_pList, nItemID);
}

void CMenu_GetItemRect(CMenu * pMe, uint16 nIdx, AEERect * prc)
{
    CListBox_GetItemRect(pMe->m_pList, nIdx, prc);
}

uint16 CMenu_GetSel(CMenu * pMe)
{
    int nFocusIndex = 0;
    IWidget_GetFocusIndex((IWidget*)pMe->m_pList, &nFocusIndex);
    return (uint16)nFocusIndex;
}

void CMenu_SetItemImage(CMenu* pMe, uint16 nItemID, IImage* pImgUnchecked, IImage* pImgChecked)
{
    int nErr = SUCCESS;
    CMenuItemInfo* pmii;
    nErr = IWidget_ListData_GetItemData((IWidget*)pMe->m_pList, nItemID, &pmii);
    if (SUCCESS == nErr)
    {
        pmii->pImgUnchecked = pImgUnchecked;
        pmii->pImgChecked = pImgChecked;
    }
}

void CMenu_SetOwnerDrawCB(CListBox* pMe, PFNINDEXER pfnCB, void * pvContext)
{
    
}

void CMenu_SetSel(CMenu * pMe, uint16 nItemID)
{
    IWidget_SetFocusIndex((IWidget*)pMe->m_pList, nItemID);
}
