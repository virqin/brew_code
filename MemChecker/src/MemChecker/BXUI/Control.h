/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "Base.h"
#include "IContainer.h"
#include "EventDispatcher.h"

enum
{
    WIDGET_STATE_INACTIVE,
    WIDGET_STATE_ACTIVE,
    WIDGET_STATE_SELECTED_INACTIVE,
    WIDGET_STATE_SELECTED_ACTIVE,
    WIDGET_STATE_MAX
};

#define CURRENT_WIDGET_STATE(po) (((po)->m_bActive ? WIDGET_STATE_ACTIVE : WIDGET_STATE_INACTIVE) + 2 * ((po)->m_bSelected ? 1 : 0))

#define SET_WIDGET_STATE_PROP(p, member_name, inactive_prop, active_prop, selected_inactive_prop, selected_active_prop) \
    (p)->member_name[WIDGET_STATE_INACTIVE] = (inactive_prop), \
    (p)->member_name[WIDGET_STATE_ACTIVE] = (active_prop), \
    (p)->member_name[WIDGET_STATE_SELECTED_INACTIVE] = (selected_inactive_prop), \
    (p)->member_name[WIDGET_STATE_SELECTED_ACTIVE] = (selected_active_prop)

#define SET_DEFAULT_WIDGET_STATE_PROP(p, member_name, type_name, prop_name) \
    SET_WIDGET_STATE_PROP(p, member_name, \
    DEFAULT_##type_name##_##prop_name##_INACTIVE, \
    DEFAULT_##type_name##_##prop_name##_ACTIVE, \
    DEFAULT_##type_name##_##prop_name##_SELECTED_INACTIVE, \
    DEFAULT_##type_name##_##prop_name##_SELECTED_ACTIVE)

// CControl class
typedef struct CControl
{
    CBase       base;

    IContainer* m_pParent;

    int         m_nID;
    char*       m_pszName;
    AEERect     m_rc;
    AEERect     m_rcClient;

    int         m_nBorderWidth;

    RGBVAL      m_rgbBorderColors[WIDGET_STATE_MAX];

    RGBVAL      m_rgbBGColors[WIDGET_STATE_MAX];

    RGBVAL      m_rgbFGColors[WIDGET_STATE_MAX];

    IImage*     m_pBGImage[WIDGET_STATE_MAX];

    boolean     m_bEnabled;
    boolean     m_bVisible;
    boolean     m_bActive;
    boolean     m_bSelected;
    boolean     m_bCanTakeFocus;

    uint32      m_dwFlags;

    IBitmap*    m_piMaskBitmap;

    //==================================================
    HookHandler m_hookHandler;
    IEventDispatcher* m_pIEventDispatcher;

    void*       m_pDataObject;
} CControl;

IWidget*    CControl_New();
void        CControl_Ctor(CControl *pMe);
uint32      CControl_Release(IWidget* po);
boolean     CControl_DefHandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
boolean     CControl_HandleEvent(IWidget* po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void        CControl_Draw(IWidget* po, DrawContext* pDrawContext);
void        CControl_SetRect(IWidget* po, const AEERect* prc);
void        CControl_GetRect(IWidget* po, AEERect* prc);
boolean     CControl_HitTest(IWidget* po, int x, int y);

#endif // __CONTROL_H__
