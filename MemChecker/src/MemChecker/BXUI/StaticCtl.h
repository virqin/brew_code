/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __STATICCTL_H__
#define __STATICCTL_H__

#include "Control.h"

// CStaticCtl class
typedef struct CStaticCtl
{
    CControl base;
} CStaticCtl;

IWidget*    CStaticCtl_New();
void        CStaticCtl_Ctor(CStaticCtl *pMe);
uint32      CStaticCtl_Release(IWidget* po);
boolean     CStaticCtl_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void        CStaticCtl_Draw(IWidget* po, DrawContext* pDrawContext);

#endif // __STATICCTL_H__
