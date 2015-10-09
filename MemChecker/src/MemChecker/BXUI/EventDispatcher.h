/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __EVENTDISPATCHER_H__
#define __EVENTDISPATCHER_H__

#include "IEventDispatcher.h"
#include "Base.h"
#include "IVector.h"

typedef struct CEventDispatcher
{
    CBase       base;

    IVector*    m_pListeners;
} CEventDispatcher;

IEventDispatcher* CEventDispatcher_New();
void              CEventDispatcher_Ctor(CEventDispatcher* pMe);
uint32            CEventDispatcher_Release(IEventDispatcher* po);
int               CEventDispatcher_AddListener(IEventDispatcher* po, Listener* pListener);
void              CEventDispatcher_RemoveListener(IEventDispatcher* po, Listener* pListener);
void              CEventDispatcher_Notify(IEventDispatcher* po, XEvent* pEvent);

#endif // __EVENTDISPATCHER_H__
