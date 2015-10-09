/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __IEVENTDISPATCHER_H__
#define __IEVENTDISPATCHER_H__

#include "AEE.h"

typedef struct XEvent
{
    uint32 nCode;
    void*  pSender;
    uint32 dwParam;
} XEvent;

typedef void (*PFNLISTENER)(void* pUserData, XEvent* pEvent);

typedef struct Listener
{
    PFNLISTENER pfnListener;
    void*       pUserData;
} Listener;

#define Listener_Init(pListener, pfn, pd) \
    do { \
        (pListener)->pfnListener = (PFNLISTENER)pfn; \
        (pListener)->pUserData = pd;} \
    while (0)

#define INHERIT_IEventDispatcher(iname) \
    INHERIT_IQI(iname); \
    int  (*AddListener)(iname* po, Listener* pListener); \
    void (*RemoveListener)(iname* po, Listener* pListener); \
    void (*Notify)(iname* po, XEvent* pEvnet)

AEEINTERFACE_DEFINE(IEventDispatcher);

#define IEventDispatcher_AddListener(po, pListener)     AEEGETPVTBL(po, IEventDispatcher)->AddListener(po, pListener)
#define IEventDispatcher_RemoveListener(po, pListener)  AEEGETPVTBL(po, IEventDispatcher)->RemoveListener(po, pListener)
#define IEventDispatcher_Notify(po, pEvent)             AEEGETPVTBL(po, IEventDispatcher)->Notify(po, pEvent)

static __inline void IEventDispatcher_NotifyEx(IEventDispatcher* po, uint32 eCode, void* pSender, uint32 dwParam)
{
    XEvent sEvent = {0};
    sEvent.nCode = eCode;
    sEvent.pSender = pSender;
    sEvent.dwParam = dwParam;
    IEventDispatcher_Notify(po, &sEvent);
}

#endif // __IEVENTDISPATCHER_H__
