/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __SCROLLBAR_H__
#define __SCROLLBAR_H__

#include "Control.h"

typedef enum ESBStyle
{
    SBS_VERT,
    SBS_HORZ
} ESBStyle;

typedef enum ESBAction
{
    SB_LINEDEC,
    SB_LINEINC,
    SB_PAGEDEC,
    SB_PAGEINC,
    SB_THUMBPOS,
    SB_THUMBTRACK
} ESBAction;

typedef struct SBNotifyData
{
    ESBAction eAction;
    int nValue;
} SBNotifyData;

// CScrollBar class
typedef struct CScrollBar
{
    CControl    base;

    boolean     m_bPointerDown;
    ESBStyle    m_eStyle;
    int         m_nMin;
    int         m_nMax;
    int         m_nPage;
    int         m_nValue;

    int         m_nThumbPos;
    int         m_nThumbSize;

    int         m_bTracking;
    int         m_nTrackOffset;
} CScrollBar;

IWidget* CScrollBar_New();
void     CScrollBar_Ctor(CScrollBar* pMe);
uint32   CScrollBar_Release(IWidget* po);
boolean  CScrollBar_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void     CScrollBar_Draw(IWidget* po, DrawContext* pDrawContext);

//==============================================================================
int      CScrollBar_GetMin(CScrollBar* pMe);
int      CScrollBar_GetMax(CScrollBar* pMe);
ESBStyle CScrollBar_GetOrientation(CScrollBar* pMe);
int      CScrollBar_GetPage(CScrollBar* pMe);
int      CScrollBar_GetValue(CScrollBar* pMe);
void     CScrollBar_SetOrientation(CScrollBar* pMe, ESBStyle eOrientation);
void     CScrollBar_SetPage(CScrollBar* pMe, int nPage);
void     CScrollBar_SetValue(CScrollBar* pMe, int nValue);
void     CScrollBar_SetRange(CScrollBar* pMe, int nMin, int nMax);

#endif // __SCROLLBAR_H__
