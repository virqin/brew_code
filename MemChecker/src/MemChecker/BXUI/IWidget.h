/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __IWIDGET_H__
#define __IWIDGET_H__

#include "IHandler.h"

typedef struct DrawContext
{
    IDisplay* pIDisplay;
    int       x;
    int       y;
} DrawContext;

#define DECLARE_DRAWCONTEXT(pCtx) \
    IDisplay* pIDisplay = (pCtx)->pIDisplay; \
    int x = (pCtx)->x; \
    int y = (pCtx)->y

// IWidget Interface
#define INHERIT_IWidget(iname) \
    INHERIT_IHandler(iname); \
    void    (*Draw)(iname* po, DrawContext* pDrawContext); \
    void    (*SetRect)(iname* po, const AEERect* prc); \
    void    (*GetRect)(iname* po, AEERect* prc); \
    boolean (*HitTest)(iname* po, int x, int y)

AEEINTERFACE_DEFINE(IWidget);

#define IWidget_AddRef(p)                   AEEGETPVTBL(p, IWidget)->AddRef(p)
#define IWidget_Release(p)                  AEEGETPVTBL(p, IWidget)->Release(p)
#define IWidget_QueryInterface(p, iid, ppo) AEEGETPVTBL(p, IWidget)->QueryInterface(p, iid, ppo)
#define IWidget_HandleEvent(p, e, w, dw)    AEEGETPVTBL(p, IWidget)->HandleEvent(p, e, w, dw)
#define IWidget_Draw(p, dc)                 AEEGETPVTBL(p, IWidget)->Draw(p, dc)
#define IWidget_SetRect(p, prc)             AEEGETPVTBL(p, IWidget)->SetRect(p, prc)
#define IWidget_GetRect(p, prc)             AEEGETPVTBL(p, IWidget)->GetRect(p, prc)
#define IWidget_HitTest(p, x, y)            AEEGETPVTBL(p, IWidget)->HitTest(p, x, y)

#define IWidget_GetProperty(p, id, pVal)    AEEGETPVTBL(p, IWidget)->HandleEvent(p, XEVT_GETPROPERTY, id, (uint32)pVal)
#define IWidget_SetProperty(p, id, val)     AEEGETPVTBL(p, IWidget)->HandleEvent(p, XEVT_SETPROPERTY, id, (uint32)val)


static __inline void IWidget_SetRectEx(IWidget* po, int x, int y, int dx, int dy)
{
    AEERect rc;
    SETAEERECT(&rc, x, y, dx, dy);
    IWidget_SetRect(po, &rc);
}

static __inline void IWidget_SetPosition(IWidget* po, int x, int y)
{
    AEERect rc;
    IWidget_GetRect(po, &rc);
    rc.x = (int16)x;
    rc.y = (int16)y;
    IWidget_SetRect(po, &rc);
}

static __inline void IWidget_SetSize(IWidget* po, int dx, int dy)
{
    AEERect rc;
    IWidget_GetRect(po, &rc);
    rc.dx = (int16)dx;
    rc.dy = (int16)dy;
    IWidget_SetRect(po, &rc);
}

#endif // __IWIDGET_H__
