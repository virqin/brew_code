/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __MENU_H__
#define __MENU_H__

#include "Control.h"
#include "ListBox.h"

// CMenu class
typedef struct CMenu
{
    CControl    base;

    CListBox*   m_pList;
    boolean     m_bPointerDown;
    uint16      m_xOffset;
    uint16      m_yOffset;
} CMenu;

typedef struct CMenuItemInfo
{
    const AECHAR*   pText;
    boolean bChecked;
    uint8   nEnableState;
    CMenu* pSubMenu;
    IImage* pImgChecked;
    IImage* pImgUnchecked;
    uint32  dwData;
} CMenuItemInfo;

//==============================================================================
IWidget    *CMenu_New();
void        CMenu_Ctor(CMenu *pMe);
uint32      CMenu_Release(IWidget* po);
boolean     CMenu_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void        CMenu_Draw(IWidget* po, DrawContext* pDrawContext);

//==============================================================================
boolean     CMenu_CheckItem(CMenu* pMe, uint16 nItemID, boolean bCheck);
boolean     CMenu_DeleteItem(CMenu* pMe, uint16 nItemID);
int         CMenu_GetItemCount(CMenu* pMe);
boolean CMenu_GetItemData
(
 CMenu* pMe,
 uint16 nItemID,
 uint32* plData
 );
uint16      CMenu_GetItemID(CMenu* pMe, int nIdx);
IImage*     CMenu_GetItemImage(CMenu* pMe, uint16 nItemID, int nImgType);
int         CMenu_GetItemIndex(CMenu* pMe, uint16 nItemID);
void        CMenu_GetItemRect(CMenu* pMe, uint16 nIdx, AEERect * prc);
uint16      CMenu_GetSel(CMenu * pMe);

void        CMenu_SetItemImage(CMenu* pMe, uint16 nItemID, IImage* pImgUnchecked, IImage* pImgChecked);
void        CMenu_SetOwnerDrawCB(CListBox* pMe, PFNINDEXER pfnCB, void * pvContext);
void        CMenu_SetSel(CMenu* pMe, uint16 nItemID);

#endif // __MENU_H__
