/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __ILAYOUTMGR_H__
#define __ILAYOUTMGR_H__

#include "AEE.h"
#include "IContainer.h"

// ILayoutMgr Interface
#define INHERIT_ILayoutMgr(iname) \
    INHERIT_IQI(iname); \
    void    (*AddChild)(iname* po, IWidget* piWidget,uint32 dwLayout); \
    void    (*RemoveChild)(iname* po, IWidget* piWidget); \
    boolean (*GetLayout)(iname* po, IWidget* piWidget, void** ppLayout); \
    void    (*LayoutContainer)(iname* po, IContainer* piContainer)

AEEINTERFACE_DEFINE(ILayoutMgr);

#define ILayoutMgr_AddRef(p)                AEEGETPVTBL(p, ILayoutMgr)->AddRef(p)
#define ILayoutMgr_Release(p)               AEEGETPVTBL(p, ILayoutMgr)->Release(p)

#define ILayoutMgr_AddChild(p, piw, ly)     AEEGETPVTBL(p, ILayoutMgr)->AddChild(p, piw, ly)
#define ILayoutMgr_RemoveChild(p, piw)      AEEGETPVTBL(p, ILayoutMgr)->RemoveChild(p, piw)
#define ILayoutMgr_GetLayout(p, piw, pp)    AEEGETPVTBL(p, ILayoutMgr)->GetLayout(p, piw, pp)
#define ILayoutMgr_LayoutContainer(p)       AEEGETPVTBL(p, ILayoutMgr)->LayoutContainer(p)

#endif // __ILAYOUTMGR_H__
