/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __WIDGETPROP_H__
#define __WIDGETPROP_H__

#include "WidgetEvent.h"
#include "IWidget.h"
#include "IEventDispatcher.h"
#include "IValueObject.h"

typedef struct IContainer IContainer;

enum
{
    PROP_HANDLER,

    PROP_PARENT,

    PROP_ID,
    PROP_NAME,

    PROP_BORDERWIDTH,

    PROP_CLIENTRECT,

    PROP_BORDERCOLOR,
    PROP_INACTIVE_BORDERCOLOR,
    PROP_ACTIVE_BORDERCOLOR,
    PROP_SELECTED_INACTIVE_BORDERCOLOR,
    PROP_SELECTED_ACTIVE_BORDERCOLOR,

    PROP_BGCOLOR,
    PROP_INACTIVE_BGCOLOR,
    PROP_ACTIVE_BGCOLOR,
    PROP_SELECTED_INACTIVE_BGCOLOR,
    PROP_SELECTED_ACTIVE_BGCOLOR,

    PROP_BGIMAGE,
    PROP_INACTIVE_BGIMAGE,
    PROP_ACTIVE_BGIMAGE,
    PROP_SELECTED_INACTIVE_BGIMAGE,
    PROP_SELECTED_ACTIVE_BGIMAGE,

    PROP_FGCOLOR,
    PROP_INACTIVE_FGCOLOR,
    PROP_ACTIVE_FGCOLOR,
    PROP_SELECTED_INACTIVE_FGCOLOR,
    PROP_SELECTED_ACTIVE_FGCOLOR,

    PROP_ENABLED,
    PROP_VISIBLE,
    PROP_ACTIVE,
    PROP_SELECTED,
    PROP_CANTAKEFOCUS,

    PROP_FLAGS,

    PROP_MASKBITMAP,

    PROP_EVENT_DISPATCHER,

    PROP_DATAOBJECT,

    // Container
    PROP_FOCUS,

    PROP_COMMON_END,

    PROP_SPECIAL = 0x1000,

    PROP_USER = 0x8000
};

#define DEFAULT_WIDGET_WIDTH    100
#define DEFAULT_WIDGET_HEIGHT   100

#define DEFAULT_WIDGET_BORDERWIDTH      2
#define DEFAULT_CONTAINER_BORDERWIDTH   0

#define DEFAULT_LISTBOX_ITEMSIZE        24

#define DEFAULT_WIDGET_BORDERCOLOR_INACTIVE             MAKE_RGB(0x80, 0x80, 0x80)
#define DEFAULT_WIDGET_BORDERCOLOR_ACTIVE               MAKE_RGB(0xFF, 0xA0, 0x00)
#define DEFAULT_WIDGET_BORDERCOLOR_SELECTED_INACTIVE    MAKE_RGB(0xC0, 0xC0, 0xC0)
#define DEFAULT_WIDGET_BORDERCOLOR_SELECTED_ACTIVE      MAKE_RGB(0xFF, 0x80, 0x00)

#define DEFAULT_WIDGET_BGCOLOR_INACTIVE                 MAKE_RGB(0xF0, 0xF8, 0xFF)
#define DEFAULT_WIDGET_BGCOLOR_ACTIVE                   MAKE_RGB(0xC0, 0xC0, 0xFF)
#define DEFAULT_WIDGET_BGCOLOR_SELECTED_INACTIVE        MAKE_RGB(0xA0, 0xA0, 0xC0)
#define DEFAULT_WIDGET_BGCOLOR_SELECTED_ACTIVE          MAKE_RGB(0x80, 0x80, 0xFF)

#define DEFAULT_WIDGET_FGCOLOR_INACTIVE                 RGB_BLACK
#define DEFAULT_WIDGET_FGCOLOR_ACTIVE                   RGB_BLACK
#define DEFAULT_WIDGET_FGCOLOR_SELECTED_INACTIVE        RGB_BLACK
#define DEFAULT_WIDGET_FGCOLOR_SELECTED_ACTIVE          RGB_BLACK

#define DEFAULT_CONTAINER_BGCOLOR_INACTIVE          MAKE_RGB(0xF0, 0xF8, 0xFF)
#define DEFAULT_CONTAINER_BGCOLOR_ACTIVE            MAKE_RGB(0xF0, 0xF8, 0xFF)
#define DEFAULT_CONTAINER_BGCOLOR_SELECTED_INACTIVE MAKE_RGB(0xF0, 0xF8, 0xFF)
#define DEFAULT_CONTAINER_BGCOLOR_SELECTED_ACTIVE   MAKE_RGB(0xF0, 0xF8, 0xFF)

typedef boolean (*PFNHANDLER)(void *pCtx, AEEEvent evt, uint16 wParam, uint32 dwParam);

typedef struct HookHandler
{
    PFNHANDLER pfnHandler;
    void*      pContext;
    PFNNOTIFY  pfnFree;
} HookHandler;

#define HookHandler_Init(p, ph, ctx, pf) \
    (p)->pfnHandler = ph, (p)->pContext = ctx, (p)->pfnFree = pf

#define HookHandler_Call(p, e, w, dw) \
    (p)->pfnHandler((p)->pContext, (e), (w), (dw))

static __inline void IWidget_SetHookHandler(IWidget* po, HookHandler* pHandler)
{
    IWidget_SetProperty(po, PROP_HANDLER, pHandler);
}

// Parent
static __inline void IWidget_SetParent(IWidget* po, IContainer* piContainer)
{
    IWidget_SetProperty(po, PROP_PARENT, piContainer);
}

static __inline IContainer* IWidget_GetParent(IWidget* po)
{
    IContainer* piContainer = FALSE;
    IWidget_GetProperty(po, PROP_PARENT, &piContainer);
    return piContainer;
}

// ID
static __inline void IWidget_SetID(IWidget* po, int nID)
{
    IWidget_SetProperty(po, PROP_ID, nID);
}

static __inline int IWidget_GetID(IWidget* po)
{
    int nID = 0;
    IWidget_GetProperty(po, PROP_ID, &nID);
    return nID;
}

// Name
static __inline void IWidget_SetName(IWidget* po, const char* pszName)
{
    IWidget_SetProperty(po, PROP_NAME, pszName);
}

static __inline const char* IWidget_GetName(IWidget* po)
{
    char* pszName = NULL;
    IWidget_GetProperty(po, PROP_NAME, &pszName);
    return pszName;
}

// BorderWidth
static __inline void IWidget_SetBorderWidth(IWidget* po, int nBorderWidth)
{
    IWidget_SetProperty(po, PROP_BORDERWIDTH, nBorderWidth);
}

static __inline int IWidget_GetBorderWidth(IWidget* po)
{
    int nBorderWidth = 0;
    IWidget_GetProperty(po, PROP_BORDERWIDTH, &nBorderWidth);
    return nBorderWidth;
}

// ClientRect
static __inline void IWidget_GetClientRect(IWidget* po, AEERect* prc)
{
    IWidget_GetProperty(po, PROP_CLIENTRECT, prc);
}

// BorderColor
static __inline void IWidget_SetBorderColor(IWidget* po, RGBVAL rgb)
{
    IWidget_SetProperty(po, PROP_BORDERCOLOR, rgb);
}

static __inline RGBVAL IWidget_GetBorderColor(IWidget* po)
{
    RGBVAL rgb;
    IWidget_GetProperty(po, PROP_BORDERCOLOR, &rgb);
    return rgb;
}

// BGColor
static __inline void IWidget_SetBGColor(IWidget* po, RGBVAL rgb)
{
    IWidget_SetProperty(po, PROP_BGCOLOR, rgb);
}

static __inline RGBVAL IWidget_GetBGColor(IWidget* po)
{
    RGBVAL rgb;
    IWidget_GetProperty(po, PROP_BGCOLOR, &rgb);
    return rgb;
}

// BGImage
static __inline void IWidget_SetBGImage(IWidget* po, IImage* piImage)
{
    IWidget_SetProperty(po, PROP_BGIMAGE, piImage);
}

static __inline IImage* IWidget_GetBGImage(IWidget* po)
{
    IImage *piImage = NULL;
    IWidget_GetProperty(po, PROP_BGIMAGE, &piImage);
    return piImage;
}

// FGColor
static __inline void IWidget_SetFGColor(IWidget* po, RGBVAL rgb)
{
    IWidget_SetProperty(po, PROP_FGCOLOR, rgb);
}

static __inline RGBVAL IWidget_GetFGColor(IWidget* po)
{
    RGBVAL rgb;
    IWidget_GetProperty(po, PROP_FGCOLOR, &rgb);
    return rgb;
}

// Enabled
static __inline void IWidget_Enabled(IWidget* po, boolean bEnabled)
{
    IWidget_SetProperty(po, PROP_ENABLED, bEnabled);
}

static __inline boolean IWidget_IsEnabled(IWidget* po)
{
    boolean bEnabled = FALSE;
    IWidget_GetProperty(po, PROP_ENABLED, &bEnabled);
    return bEnabled;
}

// Visible
static __inline void IWidget_SetVisible(IWidget* po, boolean bVisible)
{
    IWidget_SetProperty(po, PROP_VISIBLE, bVisible);
}

static __inline boolean IWidget_IsVisible(IWidget* po)
{
    boolean bVisible = FALSE;
    IWidget_GetProperty(po, PROP_VISIBLE, &bVisible);
    return bVisible;
}

// Active
static __inline void IWidget_SetActive(IWidget* po, boolean bActive)
{
    IWidget_SetProperty(po, PROP_ACTIVE, bActive);
}

static __inline boolean IWidget_IsActive(IWidget* po)
{
    boolean bActive = FALSE;
    IWidget_GetProperty(po, PROP_ACTIVE, &bActive);
    return bActive;
}

// Selected
static __inline void IWidget_SetSelected(IWidget* po, boolean bSelected)
{
    IWidget_SetProperty(po, PROP_SELECTED, bSelected);
}

static __inline boolean IWidget_IsSelected(IWidget* po)
{
    boolean bSelected = FALSE;
    IWidget_GetProperty(po, PROP_SELECTED, &bSelected);
    return bSelected;
}

// CanTakeFocus
static __inline void IWidget_EnableTakeFocus(IWidget* po, boolean bCanTakeFocus)
{
    IWidget_SetProperty(po, PROP_CANTAKEFOCUS, bCanTakeFocus);
}

static __inline boolean IWidget_CanTakeFocus(IWidget* po)
{
    boolean bCanTakeFocus = FALSE;
    IWidget_GetProperty(po, PROP_CANTAKEFOCUS, &bCanTakeFocus);
    return bCanTakeFocus;
}

// Flags
static __inline void IWidget_SetFlags(IWidget* po, uint32 dwFlags)
{
    IWidget_SetProperty(po, PROP_FLAGS, dwFlags);
}

static __inline uint32 IWidget_GetFlags(IWidget* po)
{
    uint32 dwFlags = 0;
    IWidget_GetProperty(po, PROP_FLAGS, &dwFlags);
    return dwFlags;
}

// MaskBitmap
static __inline void IWidget_SetMaskBitmap(IWidget* po, IBitmap *pIBitmap)
{
    IWidget_SetProperty(po, PROP_MASKBITMAP, pIBitmap);
}

static __inline IBitmap* IWidget_GetMaskBitmap(IWidget* po)
{
    IBitmap* pIBitmap = NULL;
    IWidget_GetProperty(po, PROP_MASKBITMAP, &pIBitmap);
    return pIBitmap;
}

// EventDispatcher
static __inline void IWidget_SetEventDispatcher(IWidget* po, IEventDispatcher* pIEventDispatcher)
{
    IWidget_SetProperty(po, PROP_EVENT_DISPATCHER, pIEventDispatcher);
}

static __inline IEventDispatcher* IWidget_GetEventDispatcher(IWidget* po)
{
    IEventDispatcher* pIEventDispatcher = NULL;
    IWidget_GetProperty(po, PROP_EVENT_DISPATCHER, &pIEventDispatcher);
    return pIEventDispatcher;
}

static __inline void IWidget_AddListener(IWidget* po, Listener* pListener)
{
    IEventDispatcher_AddListener(IWidget_GetEventDispatcher(po), pListener);
}

static __inline void IWidget_RemoveListener(IWidget* po, Listener* pListener)
{
    IEventDispatcher_RemoveListener(IWidget_GetEventDispatcher(po), pListener);
}

// DataObject
static __inline boolean IWidget_SetDataObject(IWidget* po, IQI* pData)
{
    return IWidget_SetProperty(po, PROP_DATAOBJECT, pData);
}

static __inline boolean IWidget_GetDataObject(IWidget* po, IQI** ppData)
{
    return IWidget_GetProperty(po, PROP_DATAOBJECT, ppData);
}

static __inline boolean IWidget_SetValueData(IWidget* po, void* pData, int nSize, PFNNOTIFY pfnFree)
{
    IValueObject* piValueObject = NULL;
    IWidget_GetDataObject(po, (IQI**)&piValueObject);
    if (piValueObject) {
        IValueObject_SetValue(piValueObject, pData, nSize, pfnFree);
        return TRUE;
    }
    return FALSE;
}

static __inline boolean IWidget_GetValueData(IWidget* po, void** ppData, int* pnSize)
{
    IValueObject* piValueObject = NULL;
    IWidget_GetDataObject(po, (IQI**)&piValueObject);
    if (piValueObject) {
        *ppData = IValueObject_GetValue(piValueObject, pnSize);
        return TRUE;
    }
    return FALSE;
}

/*******************************************************************************
  Container
*******************************************************************************/

// special definitions for focus
#define WIDGET_FOCUS_NONE     ((IWidget*)0)  // cancel current focus
#define WIDGET_FOCUS_FIRST    ((IWidget*)1)  // move focus to first focusable widget in the container
#define WIDGET_FOCUS_LAST     ((IWidget*)2)  // move focus to last focusable widget in the container 
#define WIDGET_FOCUS_NEXT     ((IWidget*)3)  // move focus to next focusable widget in the container  
#define WIDGET_FOCUS_PREV     ((IWidget*)4)  // move focus to previous focusable widget in the container   

static __inline void IContainer_MoveFocus(IContainer* po, IWidget* piWidget)
{
    IWidget_SetProperty((IWidget*)po, PROP_FOCUS, piWidget);
}

static __inline IWidget* IContainer_GetFocus(IContainer* po)
{
    IWidget* piwFocus = NULL;
    IWidget_GetProperty((IWidget*)po, PROP_FOCUS, &piwFocus);
    return piwFocus;
}

#endif // __WIDGETPROP_H__
