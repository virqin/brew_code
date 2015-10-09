/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __ICONTROLWRAP_H__
#define __ICONTROLWRAP_H__

#include "Control.h"
#include "Window.h"

// CIControlWrap class
#define INHERIT_CIControlWrap(iname) \
    INHERIT_CControl(iname);\
    
typedef struct CIControlWrap
{
    CControl    base;

    IControl*   m_pIControl;
    boolean     m_bPointerDown;
} CIControlWrap;

IWidget*CIControlWrap_New();
void    CIControlWrap_Ctor(CIControlWrap *pMe);
uint32  CIControlWrap_Release(IWidget* po);
boolean CIControlWrap_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void    CIControlWrap_Draw(IWidget* po, DrawContext* pDrawContext);
void    CIControlWrap_SetRect(IWidget* po, const AEERect* prc);
void    CIControlWrap_GetRect(IWidget* po, AEERect* prc);
void    CIControlWrap_SetActive(IWidget* po, boolean bActive);
boolean CIControlWrap_IsActive(IWidget* po);

#endif // __ICONTROLWRAP_H__
