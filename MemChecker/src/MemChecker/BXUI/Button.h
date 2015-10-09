/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "Container.h"

// CButton class
typedef struct CButton
{
    CContainer  base;

    boolean     m_bPressed;
} CButton;

IWidget*    CButton_New();
void        CButton_Ctor(CButton *pMe);
uint32      CButton_Release(IWidget* po);
boolean     CButton_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void        CButton_Draw(IWidget* po, DrawContext* pDrawContext);

#endif // __BUTTON_H__
