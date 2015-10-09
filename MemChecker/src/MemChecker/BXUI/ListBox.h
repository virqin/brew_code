/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __LISTBOX_H__
#define __LISTBOX_H__

#include "Control.h"
#include "Vector.h"
#include "ScrollBar.h"
#include "AEEShell.h"

#define LBF_WRAPSELECTION       1
#define LBF_NOSELECT            (1 << 1)
#define LBF_VARIABLEITEMSIZE    (1 << 2)

typedef struct BCListODCBData
{
    int     nIndex;       // Index of the item
    AEERect rc;         // Rect to be used for measuring/rendering
} BCListODCBData;       // Item size information
typedef void (*PFNINDEXER)(void * pvContext, BCListODCBData* is);

// CListBox class
typedef struct CListBox
{
    CControl    base;

    IWidget*    m_pChild;
    boolean     m_bPointerDown;
    int         m_nItemSize;
    int         m_nFocusIndex;
    int         m_nTopIndex;
    PFNINDEXER  m_pfnODCB;
    void*       m_pODCBContext;
} CListBox;

//==============================================================================
IWidget*    CListBox_New();
void        CListBox_Ctor(CListBox *pMe);
uint32      CListBox_Release(IWidget* po);
boolean     CListBox_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void        CListBox_Draw(IWidget* po, DrawContext* pDrawContext);

//==============================================================================
int         CListBox_GetItemIndex(CListBox* pMe, uint16 uiItemID);
void        CListBox_GetItemRect(CListBox * pMe, int nIndex, AEERect* prc);
int         CListBox_GetTopIndex(CListBox * pMe);
int         CListBox_GetVisibleCount(CListBox* pMe);
void        CListBox_SetTopIndex(CListBox * pMe, int nIndex);

#endif // __LISTBOX_H__
