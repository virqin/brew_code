/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "IControlWrap.h"
#include "AEEPointerHelpers.h"
#include "Base.h"
#include "WidgetProp.h"

IWidget* CIControlWrap_New()
{
    return (IWidget*)NewVT(CIControlWrap, IWidget);
}

void CIControlWrap_Ctor(CIControlWrap *pMe)
{
    DECLARE_PVTBL(pvt, IWidget);

    CControl_Ctor(&pMe->base);

    pvt->SetRect = CIControlWrap_SetRect;
    pvt->GetRect = CIControlWrap_GetRect;
}

uint32 CIControlWrap_Release(IWidget* po)
{
    DECLARE_ME(CIControlWrap);
    
    if (1 == CBase_Refs(po))
    {
        ICONTROL_Release(pMe->m_pIControl);
    }

    return CControl_Release(po);
}

boolean CIControlWrap_HandlePointerEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CIControlWrap);
    int x = AEE_POINTER_GET_X((char*)dwParam);
    int y = AEE_POINTER_GET_Y((char*)dwParam);

    switch (eCode)
    {
    case XEVT_MOUSEDOWN:
        if (IWidget_HitTest(po, x, y))
        {
            pMe->m_bPointerDown = TRUE;
            IContainer_MoveFocus(pMe->base.m_pParent, (IWidget*)pMe);
            return FALSE;
        }

        break;
    case XEVT_MOUSEMOVE:
        if (pMe->m_bPointerDown)
        {
            return FALSE;
        }

        break;
    case XEVT_MOUSEUP:
        if (pMe->m_bPointerDown)
        {
            pMe->m_bPointerDown = FALSE;
            return FALSE;
        }

        break;
    }

    return FALSE;
}

boolean CIControlWrap_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CIControlWrap);

    if (ICONTROL_HandleEvent(pMe->m_pIControl, eCode, wParam, dwParam))
        return TRUE;

    switch (eCode)
    {
    case POINTER_EVENT_CASE:
        if (CIControlWrap_HandlePointerEvent(po, eCode, wParam, dwParam))
            return TRUE;
        break;
    }
    
    return FALSE;
}

void CIControlWrap_Draw(IWidget* po, DrawContext* pDrawContext)
{
    DECLARE_ME(CIControlWrap);
    
    ICONTROL_Redraw(pMe->m_pIControl);
}

void CIControlWrap_SetRect(IWidget* po, const AEERect* prc)
{
    DECLARE_ME(CIControlWrap);

    ICONTROL_SetRect(pMe->m_pIControl, prc);
}

void CIControlWrap_GetRect(IWidget* po, AEERect* prc)
{
    DECLARE_ME(CIControlWrap);
    
    ICONTROL_GetRect(pMe->m_pIControl, prc);
}

void CIControlWrap_SetActive(IWidget* po, boolean bActive)
{
    DECLARE_ME(CIControlWrap);

    ICONTROL_SetActive(pMe->m_pIControl, bActive);
}

boolean CIControlWrap_IsActive(IWidget* po)
{
    DECLARE_ME(CIControlWrap);

    return ICONTROL_IsActive(pMe->m_pIControl);
}
