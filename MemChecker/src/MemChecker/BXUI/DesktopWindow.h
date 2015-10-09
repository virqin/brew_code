/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __DESKTOPWINDOW_H__
#define __DESKTOPWINDOW_H__

#include "IDesktopWindow.h"
#include "Container.h"

typedef struct CDesktopWindow
{
    CContainer  base;

    IDisplay*   m_pIDisplay;
    AEERect     m_rcInvalid;
    AEECallback m_cbUpdate;
} CDesktopWindow;

IDesktopWindow* CDesktopWindow_New(void);
void            CDesktopWindow_Ctor(CDesktopWindow* pMe);
uint32          CDesktopWindow_Release(IDesktopWindow* po);
void            CDesktopWindow_Invalidate(IDesktopWindow* po, IWidget* piWidget, const AEERect* prc);
boolean         CDesktopWindow_HandleAppEvent(IDesktopWindow* po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void            CDesktopWindow_SetDisplay(IDesktopWindow* po, IDisplay* pIDisplay);
IDisplay*       CDesktopWindow_GetDisplay(IDesktopWindow* po);

#endif // __DESKTOPWINDOW_H__
