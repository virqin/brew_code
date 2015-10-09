/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __ICONTAINER_H__
#define __ICONTAINER_H__

#include "IWidget.h"
#include "WidgetProp.h"

// IContainer Interface
#define INHERIT_IContainer(iname) \
    INHERIT_IWidget(iname); \
    void        (*Invalidate)(iname* po, IWidget* piWidget, const AEERect* prc); \
    int         (*Insert)(iname* po, IWidget* piWidget, int nIndex, uint32 dwLayout); \
    int         (*Remove)(iname* po, int nIndex); \
    uint32      (*GetChildCount)(iname* po); \
    IWidget*    (*GetChild)(iname* po, int nIndex)

AEEINTERFACE_DEFINE(IContainer);

#define IContainer_Invalidate(p, widget, prc)   AEEGETPVTBL(p, IContainer)->Invalidate(p, widget, prc)
#define IContainer_Insert(p, widget, pos, ly)   AEEGETPVTBL(p, IContainer)->Insert(p, widget, pos, ly)
#define IContainer_Remove(p, index)             AEEGETPVTBL(p, IContainer)->Remove(p, index)
#define IContainer_GetChildCount(p)             AEEGETPVTBL(p, IContainer)->GetChildCount(p)
#define IContainer_GetChild(p, index)           AEEGETPVTBL(p, IContainer)->GetChild(p, index)

static __inline IWidget* ICONTAINER_TO_IWIDGET(IContainer* po)
{
    return (IWidget*)po;
}

static __inline int IContainer_GetChildIndex(IContainer* po, IWidget* piWidget)
{
    int nCount = IContainer_GetChildCount(po);
    IWidget* piwTemp = NULL;

    while (nCount-- > 0)
    {
        piwTemp = IContainer_GetChild(po, nCount);
        if (piwTemp == piWidget) {
            return nCount;
        }
    }

    return -1;
}

static __inline int IContainer_RemoveChild(IContainer* po, IWidget* piWidget)
{
    int nIndex = IContainer_GetChildIndex(po, piWidget);

    if (-1 != nIndex) {
        return IContainer_Remove(po, nIndex);
    }

    return EFAILED;
}

static __inline void IContainer_RemoveAllChild(IContainer* po)
{
    int nCount = IContainer_GetChildCount(po);

    while (nCount-- > 0) {
        IContainer_Remove(po, nCount);
    }
}

static __inline void IWidget_Invalidate(IWidget* piWidget, AEERect* prc)
{
    IContainer* piContainer = NULL;

    piContainer = IWidget_GetParent(piWidget);
    if (piContainer) {
        IContainer_Invalidate(piContainer, piWidget, prc);
    }
}

#endif // __ICONTAINER_H__
