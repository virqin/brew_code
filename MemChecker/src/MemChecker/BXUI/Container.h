/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

#include "Control.h"
#include "Vector.h"
#include "ILayoutMgr.h"

// CContainer class
typedef struct CContainer
{
    CControl    base;

    IVector*    m_lstChild;
    IWidget*    m_piwFocus;
    ILayoutMgr* m_piLayoutMgr;
} CContainer;

IContainer* CContainer_New();
void        CContainer_Ctor(CContainer* pMe);
uint32      CContainer_Release(IContainer* po);
boolean     CContainer_HandleEvent(IContainer* po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void        CContainer_Draw(IContainer* po, DrawContext* pDrawContext);

void        CContainer_Invalidate(IContainer* po, IWidget* piWidget, const AEERect* prc);
int         CContainer_Insert(IContainer* po, IWidget* piWidget, int nIndex, uint32 dwLayout);
int         CContainer_Remove(IContainer* po, int nIndex);
uint32      CContainer_GetChildCount(IContainer* po);
IWidget*    CContainer_GetChild(IContainer* po, int nIndex);

#endif // __CONTAINER_H__
