/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "MsgBox.h"
#include "XUtil.h"

uint32 CMsgBox_Release(IWindow* po)
{
    if (1 == CBase_Refs(po))
    {
    }

    return CWindow_Release(po);
}

boolean CMsgBox_HandleEvent(IWindow* po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    switch (eCode)
    {
    case XEVT_KEY:
        switch (wParam)
        {
        case AVK_CLR:
            RELEASEIF(po);
            return TRUE;
        }
        break;
    }

    return CWindow_HandleEvent(po, eCode, wParam, dwParam);
}

void CMsgBox_Draw(IWindow* po, DrawContext* pDrawContext)
{
    DECLARE_ME(CMsgBox);
    DECLARE_DRAWCONTEXT(pDrawContext);
    RGBVAL clrOld;
    AEERect rc;
    int16 cxScreen, cyScreen;
    
    GetScreenSize((uint16*)&cxScreen, (uint16*)&cyScreen);
    rc.x = 20;
    rc.y = cyScreen - 180;
    rc.dx = cxScreen - rc.x * 2;
    rc.dy = 140;
    
    IDISPLAY_FillRect(pIDisplay, &rc, 0xC0C0C000);
    
    clrOld = IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, RGB_BLACK);
    rc.x += 10;
    rc.dx -= 20;
    rc.y += 10;
    rc.dy -= 20;
//  IDISPLAY_DrawTextML_BEGIN(pMe->m_pIDisplay, AEE_FONT_LARGE, 
//      pMe->m_sMsg, -1, rc.x, rc.y, 5, &rc);
//  IDISPLAY_DrawHollowText(pMe->m_pIDisplay, Font, 
//      pcLineBegin, nLineChars, rc.x, yLine, &rc, 
//      IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER, 0x0000C000);
//  IDISPLAY_DrawTextML_END()
        IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, clrOld);
    
    CWindow_Draw(po, pDrawContext);
}

IWindow* CMsgBox_New(IShell* pIShell, const char* pszResFile, uint16 nResID, const AECHAR* pMsgText)
{
    CMsgBox* pMe = MALLOCREC_VTBL(CMsgBox, IWindow);

    if (NULL == pMe)
    {
        return NULL;
    }

    if (pMsgText != NULL)
    {
        WSTRCPY(pMe->m_sMsg, pMsgText);
    }
    else
    {
        ISHELL_LoadResString(pIShell, pszResFile, nResID, 
            pMe->m_sMsg, sizeof(pMe->m_sMsg));
    }

    return (IWindow*)pMe;
}
