/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "StaticCtl.h"
#include "XUtil.h"
#include "ValueObject.h"

IWidget* CStaticCtl_New()
{
    return (IWidget*)NewVT(CStaticCtl, IWidget);
}

void CStaticCtl_Ctor(CStaticCtl* pMe)
{
    DECLARE_PVTBL(pvt, IWidget);

    CControl_Ctor(&pMe->base);

    VT_FUNC(pvt, CStaticCtl, Release);
    VT_FUNC(pvt, CStaticCtl, Draw);

    HookHandler_Init(&pMe->base.m_hookHandler, CStaticCtl_HandleEvent, pMe, NULL);

    pMe->base.m_pDataObject = CValueObject_New();
}

uint32 CStaticCtl_Release(IWidget* po)
{
    if (1 == CBase_Refs(po))
    {
    }

    return CControl_Release(po);
}

boolean CStaticCtl_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    return CControl_DefHandleEvent(po, eCode, wParam, dwParam);
}

void CStaticCtl_Draw(IWidget* po, DrawContext* pDrawContext)
{
    DECLARE_ME(CStaticCtl);
    DECLARE_DRAWCONTEXT(pDrawContext);
    AEERect rcDisp, rcClient;
    AECHAR* pszText = NULL;
    int nLen = 0;
    RGBVAL clrOld;

    CControl_Draw(po, pDrawContext);

    IDISPLAY_GetClipRect(pIDisplay, &rcDisp);
    rcClient = pMe->base.m_rcClient;
    AEE_OffsetRect(&rcClient, x, y);
    if (!AEE_IntersectRect(&rcDisp, &rcDisp, &rcClient)) {
        return;
    }
    IDISPLAY_SetClipRect(pIDisplay, &rcDisp);

    pszText = IValueObject_GetValue(pMe->base.m_pDataObject, &nLen);
    clrOld = IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, 
        pMe->base.m_rgbFGColors[CURRENT_WIDGET_STATE(&pMe->base)]);
    IDISPLAY_DrawText(pIDisplay, AEE_FONT_NORMAL, pszText, nLen, 
        x + pMe->base.m_rcClient.x, y + pMe->base.m_rcClient.y, 
        &rcClient, IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
    IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, clrOld);
}
