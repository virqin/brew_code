/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "Control.h"
#include "Base.h"
#include "XUtil.h"

static __inline void CControl_CalcClientRect(CControl* pMe);

IWidget* CControl_New()
{
    return (IWidget*)NewVT(CControl, IWidget);
}

void CControl_Ctor(CControl* pMe)
{
    DECLARE_PVTBL(pvt, IWidget);

    CBase_Ctor(&pMe->base);

    VT_FUNC(pvt, CControl, Release);
    VT_FUNC(pvt, CControl, HandleEvent);
    VT_FUNC(pvt, CControl, Draw);
    VT_FUNC(pvt, CControl, SetRect);
    VT_FUNC(pvt, CControl, GetRect);
    VT_FUNC(pvt, CControl, HitTest);

    HookHandler_Init(&pMe->m_hookHandler, CControl_DefHandleEvent, pMe, NULL);

    SETAEERECT(&pMe->m_rc, 0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);

    pMe->m_nBorderWidth = DEFAULT_WIDGET_BORDERWIDTH;

    SET_DEFAULT_WIDGET_STATE_PROP(pMe, m_rgbBorderColors, WIDGET, BORDERCOLOR);
    SET_DEFAULT_WIDGET_STATE_PROP(pMe, m_rgbBGColors, WIDGET, BGCOLOR);
    SET_DEFAULT_WIDGET_STATE_PROP(pMe, m_rgbFGColors, WIDGET, FGCOLOR);

    pMe->m_bEnabled = TRUE;
    pMe->m_bVisible = TRUE;
    pMe->m_bCanTakeFocus = TRUE;

    pMe->m_pIEventDispatcher = CEventDispatcher_New();

    CControl_CalcClientRect(pMe);
}

uint32 CControl_Release(IWidget* po)
{
    DECLARE_ME(CControl);
    int i = 0;

    if (1 == CBase_Refs(po))
    {
        FREEIF(pMe->m_pszName);
        for (i = 0; i < WIDGET_STATE_MAX; i++)
        {
            if (pMe->m_pBGImage[i])
            {
                RELEASEIF(pMe->m_pBGImage[i]);
            }
        }
        RELEASEIF(pMe->m_piMaskBitmap);
        RELEASEIF(pMe->m_pIEventDispatcher);
        RELEASEIF(pMe->m_pDataObject);
    }

    return CBase_Release((IBase*)po);
}

static __inline void CControl_GetClientRect(CControl* pMe, AEERect *prc)
{
    SETAEERECT(prc, 
        pMe->m_nBorderWidth, 
        pMe->m_nBorderWidth, 
        pMe->m_rc.dx - pMe->m_nBorderWidth * 2, 
        pMe->m_rc.dy - pMe->m_nBorderWidth * 2);
}

static __inline void CControl_CalcClientRect(CControl* pMe)
{
    CControl_GetClientRect(pMe, &pMe->m_rcClient);

    // Let the sub class to know that client rect has changed
    IWidget_SetProperty((IWidget*)pMe, PROP_CLIENTRECT, &pMe->m_rcClient);
}

boolean CControl_GetProperty(IWidget* po, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CControl);
    boolean bHandled = TRUE;

    switch(wParam)
    {
    case PROP_PARENT:
        *(IContainer**)dwParam = pMe->m_pParent;
        break;
    case PROP_ID:
        *(int*)dwParam = pMe->m_nID;
        break;
    case PROP_NAME:
        *(char**)dwParam = pMe->m_pszName;
        break;
    case PROP_BORDERWIDTH:
        *(int*)dwParam = pMe->m_nBorderWidth;
        break;
    case PROP_CLIENTRECT:
        *(AEERect*)dwParam = pMe->m_rcClient;
        break;
    case PROP_BORDERCOLOR:
    case PROP_INACTIVE_BORDERCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbBorderColors[WIDGET_STATE_INACTIVE];
        break;
    case PROP_ACTIVE_BORDERCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbBorderColors[WIDGET_STATE_ACTIVE];
        break;
    case PROP_SELECTED_INACTIVE_BORDERCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbBorderColors[WIDGET_STATE_SELECTED_INACTIVE];
        break;
    case PROP_SELECTED_ACTIVE_BORDERCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbBorderColors[WIDGET_STATE_SELECTED_ACTIVE];
        break;
    case PROP_BGCOLOR:
    case PROP_INACTIVE_BGCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbBGColors[WIDGET_STATE_INACTIVE];
        break;
    case PROP_ACTIVE_BGCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbBGColors[WIDGET_STATE_ACTIVE];
        break;
    case PROP_SELECTED_INACTIVE_BGCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbBGColors[WIDGET_STATE_SELECTED_INACTIVE];
        break;
    case PROP_SELECTED_ACTIVE_BGCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbBGColors[WIDGET_STATE_SELECTED_ACTIVE];
        break;
    case PROP_BGIMAGE:
    case PROP_INACTIVE_BGIMAGE:
        *(IImage**)dwParam = pMe->m_pBGImage[WIDGET_STATE_INACTIVE];
        break;
    case PROP_ACTIVE_BGIMAGE:
        *(IImage**)dwParam = pMe->m_pBGImage[WIDGET_STATE_ACTIVE];
        break;
    case PROP_SELECTED_INACTIVE_BGIMAGE:
        *(IImage**)dwParam = pMe->m_pBGImage[WIDGET_STATE_SELECTED_INACTIVE];
        break;
    case PROP_SELECTED_ACTIVE_BGIMAGE:
        *(IImage**)dwParam = pMe->m_pBGImage[WIDGET_STATE_SELECTED_ACTIVE];
        break;
    case PROP_FGCOLOR:
    case PROP_INACTIVE_FGCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbFGColors[WIDGET_STATE_INACTIVE];
        break;
    case PROP_ACTIVE_FGCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbFGColors[WIDGET_STATE_ACTIVE];
        break;
    case PROP_SELECTED_INACTIVE_FGCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbFGColors[WIDGET_STATE_SELECTED_INACTIVE];
        break;
    case PROP_SELECTED_ACTIVE_FGCOLOR:
        *(RGBVAL*)dwParam = pMe->m_rgbFGColors[WIDGET_STATE_SELECTED_ACTIVE];
        break;
    case PROP_ENABLED:
        *(boolean*)dwParam = pMe->m_bEnabled;
        break;
    case PROP_VISIBLE:
        *(boolean*)dwParam = pMe->m_bVisible;
        break;
    case PROP_ACTIVE:
        *(boolean*)dwParam = pMe->m_bActive;
        break;
    case PROP_SELECTED:
        *(boolean*)dwParam = pMe->m_bSelected;
        break;
    case PROP_CANTAKEFOCUS:
        *(boolean*)dwParam = pMe->m_bCanTakeFocus;
        break;
    case PROP_FLAGS:
        *(uint32*)dwParam = pMe->m_dwFlags;
        break;
    case PROP_MASKBITMAP:
        *(IBitmap**)dwParam = pMe->m_piMaskBitmap;
        break;
    case PROP_EVENT_DISPATCHER:
        *(IEventDispatcher**)dwParam = pMe->m_pIEventDispatcher;
        break;
    case PROP_DATAOBJECT:
        *(IQI**)dwParam = pMe->m_pDataObject;
        break;
    default:
        bHandled = FALSE;
        break;
    }

    return bHandled;
}

boolean CControl_SetProperty(IWidget* po, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CControl);
    boolean bHandled = TRUE;

    switch(wParam)
    {
    case PROP_HANDLER:
        {
            HookHandler hdTemp = *(HookHandler*)dwParam;
            *(HookHandler*)dwParam = pMe->m_hookHandler;
            pMe->m_hookHandler = hdTemp;
        }
        break;
    case PROP_PARENT:
        pMe->m_pParent = (IContainer*)dwParam;
        break;
    case PROP_ID:
        pMe->m_nID = (int)dwParam;
        break;
    case PROP_NAME:
        FREEIF(pMe->m_pszName);
        if ((char*)dwParam) {
            pMe->m_pszName = STRDUP((char*)dwParam);
        }
        break;
    case PROP_BORDERWIDTH:
        pMe->m_nBorderWidth = (int)dwParam;
        CControl_CalcClientRect(pMe);
        goto lblInvalidate;
    case PROP_BORDERCOLOR:
        pMe->m_rgbBorderColors[WIDGET_STATE_INACTIVE] = (RGBVAL)dwParam;
        pMe->m_rgbBorderColors[WIDGET_STATE_ACTIVE] = (RGBVAL)dwParam;
        pMe->m_rgbBorderColors[WIDGET_STATE_SELECTED_INACTIVE] = (RGBVAL)dwParam;
        pMe->m_rgbBorderColors[WIDGET_STATE_SELECTED_ACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_INACTIVE_BORDERCOLOR:
        pMe->m_rgbBorderColors[WIDGET_STATE_INACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_ACTIVE_BORDERCOLOR:
        pMe->m_rgbBorderColors[WIDGET_STATE_ACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_SELECTED_INACTIVE_BORDERCOLOR:
        pMe->m_rgbBorderColors[WIDGET_STATE_SELECTED_INACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_SELECTED_ACTIVE_BORDERCOLOR:
        pMe->m_rgbBorderColors[WIDGET_STATE_SELECTED_ACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_BGCOLOR:
        pMe->m_rgbBGColors[WIDGET_STATE_INACTIVE] = (RGBVAL)dwParam;
        pMe->m_rgbBGColors[WIDGET_STATE_ACTIVE] = (RGBVAL)dwParam;
        pMe->m_rgbBGColors[WIDGET_STATE_SELECTED_INACTIVE] = (RGBVAL)dwParam;
        pMe->m_rgbBGColors[WIDGET_STATE_SELECTED_ACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_INACTIVE_BGCOLOR:
        pMe->m_rgbBGColors[WIDGET_STATE_INACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_ACTIVE_BGCOLOR:
        pMe->m_rgbBGColors[WIDGET_STATE_ACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_SELECTED_INACTIVE_BGCOLOR:
        pMe->m_rgbBGColors[WIDGET_STATE_SELECTED_INACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_SELECTED_ACTIVE_BGCOLOR:
        pMe->m_rgbBGColors[WIDGET_STATE_SELECTED_ACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_BGIMAGE:
        ADDREFIF((IImage*)dwParam);
        pMe->m_pBGImage[WIDGET_STATE_INACTIVE] = (IImage*)dwParam;
        RELEASEIF(pMe->m_pBGImage[WIDGET_STATE_ACTIVE]);
        RELEASEIF(pMe->m_pBGImage[WIDGET_STATE_SELECTED_INACTIVE]);
        RELEASEIF(pMe->m_pBGImage[WIDGET_STATE_SELECTED_ACTIVE]);
        goto lblInvalidate;
    case PROP_INACTIVE_BGIMAGE:
        ADDREFIF((IImage*)dwParam);
        pMe->m_pBGImage[WIDGET_STATE_INACTIVE] = (IImage*)dwParam;
        goto lblInvalidate;
    case PROP_ACTIVE_BGIMAGE:
        ADDREFIF((IImage*)dwParam);
        pMe->m_pBGImage[WIDGET_STATE_ACTIVE] = (IImage*)dwParam;
        goto lblInvalidate;
    case PROP_SELECTED_INACTIVE_BGIMAGE:
        ADDREFIF((IImage*)dwParam);
        pMe->m_pBGImage[WIDGET_STATE_SELECTED_INACTIVE] = (IImage*)dwParam;
        goto lblInvalidate;
    case PROP_SELECTED_ACTIVE_BGIMAGE:
        ADDREFIF((IImage*)dwParam);
        pMe->m_pBGImage[WIDGET_STATE_SELECTED_ACTIVE] = (IImage*)dwParam;
        goto lblInvalidate;
    case PROP_FGCOLOR:
        pMe->m_rgbFGColors[WIDGET_STATE_INACTIVE] = (RGBVAL)dwParam;
        pMe->m_rgbFGColors[WIDGET_STATE_ACTIVE] = (RGBVAL)dwParam;
        pMe->m_rgbFGColors[WIDGET_STATE_SELECTED_INACTIVE] = (RGBVAL)dwParam;
        pMe->m_rgbFGColors[WIDGET_STATE_SELECTED_ACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_INACTIVE_FGCOLOR:
        pMe->m_rgbFGColors[WIDGET_STATE_INACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_ACTIVE_FGCOLOR:
        pMe->m_rgbFGColors[WIDGET_STATE_ACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_SELECTED_INACTIVE_FGCOLOR:
        pMe->m_rgbFGColors[WIDGET_STATE_SELECTED_INACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_SELECTED_ACTIVE_FGCOLOR:
        pMe->m_rgbFGColors[WIDGET_STATE_SELECTED_ACTIVE] = (RGBVAL)dwParam;
        goto lblInvalidate;
    case PROP_ENABLED:
        pMe->m_bEnabled = (boolean)dwParam;
        goto lblInvalidate;
    case PROP_VISIBLE:
        pMe->m_bVisible = (boolean)dwParam;
        goto lblInvalidate;
    case PROP_ACTIVE:
        pMe->m_bActive = (boolean)dwParam;
        if (pMe->m_bActive) {
            DBGPRINTF("CControl_HandleEvent: --%s-- Got Focus", pMe->m_pszName);
        } else {
            DBGPRINTF("CControl_HandleEvent: --%s-- Lost Focus", pMe->m_pszName);
        }
        bHandled = FALSE;
        goto lblInvalidate;
    case PROP_SELECTED:
        pMe->m_bSelected = (boolean)dwParam;
        bHandled = FALSE;
        goto lblInvalidate;
    case PROP_CANTAKEFOCUS:
        pMe->m_bCanTakeFocus = (boolean)dwParam;
        break;
    case PROP_FLAGS:
        pMe->m_dwFlags = dwParam;
        goto lblInvalidate;
    case PROP_MASKBITMAP:
        ADDREFIF((IBitmap*)dwParam);
        pMe->m_piMaskBitmap = (IBitmap*)dwParam;
        break;
    case PROP_EVENT_DISPATCHER:
        ADDREFIF((IEventDispatcher*)dwParam);
        pMe->m_pIEventDispatcher = (IEventDispatcher*)dwParam;
        break;
    case PROP_DATAOBJECT:
        ADDREFIF((IQI*)dwParam);
        pMe->m_pDataObject = (IQI*)dwParam;
        break;

lblInvalidate:
        IWidget_Invalidate(po, NULL);
        break;

    default:
        bHandled = FALSE;
        break;
    }

    return bHandled;
}

boolean CControl_DefHandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    boolean bHandled = FALSE;

    switch (eCode)
    {
    case XEVT_GETPROPERTY:
        bHandled = CControl_GetProperty(po, wParam, dwParam);
        break;

    case XEVT_SETPROPERTY:
        bHandled = CControl_SetProperty(po, wParam, dwParam);
        break;

    default:
        break;
    }

    return bHandled;
}

boolean CControl_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    DECLARE_ME(CControl);

    return HookHandler_Call(&pMe->m_hookHandler, eCode, wParam, dwParam);
}

void CControl_Draw(IWidget* po, DrawContext* pDrawContext)
{
    DECLARE_ME(CControl);
    DECLARE_DRAWCONTEXT(pDrawContext);
    RGBVAL rgbBorder = pMe->m_rgbBorderColors[CURRENT_WIDGET_STATE(pMe)];
    RGBVAL rgbBG = pMe->m_rgbBGColors[CURRENT_WIDGET_STATE(pMe)];
    IImage *piBGImage = pMe->m_pBGImage[CURRENT_WIDGET_STATE(pMe)];
    AEERect rc = {0};
    AEERect rcBorder = {0};

    SETAEERECT(&rc, x, y, pMe->m_rc.dx, pMe->m_rc.dy);
    if (pMe->m_nBorderWidth > 0)
    {
        // Left Border
        SETAEERECT(&rcBorder, rc.x, rc.y, pMe->m_nBorderWidth, rc.dy);
        IDISPLAY_FillRect(pIDisplay, &rcBorder, rgbBorder);

        // Top Border
        SETAEERECT(&rcBorder, rc.x, rc.y, rc.dx, pMe->m_nBorderWidth);
        IDISPLAY_FillRect(pIDisplay, &rcBorder, rgbBorder);

        // Right Border
        SETAEERECT(&rcBorder, rc.x + rc.dx - pMe->m_nBorderWidth, rc.y, pMe->m_nBorderWidth, rc.dy);
        IDISPLAY_FillRect(pIDisplay, &rcBorder, rgbBorder);

        // Bottom Border
        SETAEERECT(&rcBorder, rc.x, rc.y + rc.dy - pMe->m_nBorderWidth, rc.dx, pMe->m_nBorderWidth);
        IDISPLAY_FillRect(pIDisplay, &rcBorder, rgbBorder);
    }

    // Client rectangle to draw inside
    rc = pMe->m_rcClient;
    AEE_OffsetRect(&rc, x, y);

    // BG Color
    if (RGB_NONE != rgbBG) {
        IDISPLAY_FillRect(pIDisplay, &rc, rgbBG);
    }

    // BG Image
    if (NULL == piBGImage) {
        piBGImage = pMe->m_pBGImage[WIDGET_STATE_INACTIVE];
    }
    if (piBGImage) {
        IImage_SetDrawSize(piBGImage, rc.dx, rc.dy);
        IImage_Draw(piBGImage, rc.x, rc.y);
    }

#if 0
    // Draw the mask region(for debugging)
    if (pMe->m_piMaskBitmap)
    {
        int nx = 0, ny = 0;

        for (ny = 0; ny < pMe->m_rcClient.dy; ny++)
            for (nx = 0; nx < pMe->m_rcClient.dx; nx++)
            {
                boolean bOpaque = FALSE;

                if (SUCCESS == IBitmap_TestOpacity(pMe->m_piMaskBitmap, nx, ny, &bOpaque)
                    && bOpaque)
                {
                    AEERect rcPoint = {0, 0, 1, 1};
                    rcPoint.x = x + pMe->m_rcClient.x + nx;
                    rcPoint.y = y + pMe->m_rcClient.y + ny;
                    IDISPLAY_FillRect(pIDisplay, &rcPoint, 0x00FF0000);
                }
            }
    }
#endif
}

void CControl_SetRect(IWidget* po, const AEERect* prc)
{
    DECLARE_ME(CControl);

    pMe->m_rc = *prc;
    CControl_CalcClientRect(pMe);

    IWidget_Invalidate(po, NULL);
}

void CControl_GetRect(IWidget* po, AEERect* prc)
{
    DECLARE_ME(CControl);

    *prc = pMe->m_rc;
}

boolean CControl_HitTest(IWidget* po, int x, int y)
{
    DECLARE_ME(CControl);
    AEERect rc;

    SETAEERECT(&rc, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy);
    if (AEE_PtInRect(&rc, x, y))
    {
        boolean bOpaque = FALSE;

        if (pMe->m_piMaskBitmap)
        {
            int nErr = IBitmap_TestOpacity(
                pMe->m_piMaskBitmap, 
                x - pMe->m_rcClient.x, 
                y - pMe->m_rcClient.y, 
                &bOpaque);

            if (SUCCESS == nErr) {
                return bOpaque;
            }
        }

        return TRUE;
    }

    return FALSE;
}
