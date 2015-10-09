/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __IHANDLER_H__
#define __IHANDLER_H__

#include "AEE.h"

#define INHERIT_IHandler(iname) \
    INHERIT_IQI(iname);\
    boolean (*HandleEvent)(iname* po, AEEEvent evt, uint16 wParam, uint32 dwParam)

AEEINTERFACE_DEFINE(IHandler);

#define IHandler_AddRef(p)                   AEEGETPVTBL(p, IHandler)->AddRef(p)
#define IHandler_Release(p)                  AEEGETPVTBL(p, IHandler)->Release(p)
#define IHandler_QueryInterface(p, iid, ppo) AEEGETPVTBL(p, IHandler)->QueryInterface(p, iid, ppo)
#define IHandler_HandleEvent(p, e, w, dw)    AEEGETPVTBL(p, IHandler)->HandleEvent(p, e, w, dw)

#endif // __IHANDLER_H__
