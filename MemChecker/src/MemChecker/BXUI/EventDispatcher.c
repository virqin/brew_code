/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "EventDispatcher.h"
#include "XUtil.h"
#include "Vector.h"

IEventDispatcher* CEventDispatcher_New()
{
    return (IEventDispatcher*)NewVT(CEventDispatcher, IEventDispatcher);
}

void CEventDispatcher_Ctor(CEventDispatcher* pMe)
{
    DECLARE_PVTBL(pvt, IEventDispatcher);

    CBase_Ctor(&pMe->base);

    pMe->m_pListeners = CVector_New();
    IVector_SetPfnFree(pMe->m_pListeners, FREE);

    VT_FUNC(pvt, CEventDispatcher, Release);
    VT_FUNC(pvt, CEventDispatcher, AddListener);
    VT_FUNC(pvt, CEventDispatcher, RemoveListener);
    VT_FUNC(pvt, CEventDispatcher, Notify);
}

uint32 CEventDispatcher_Release(IEventDispatcher* po)
{
    DECLARE_ME(CEventDispatcher);

    if (1 == CBase_Refs(po))
    {
        RELEASEIF(pMe->m_pListeners);
    }

    return CBase_Release((IBase*)po);
}

int CEventDispatcher_AddListener(IEventDispatcher* po, Listener* pListener)
{
    DECLARE_ME(CEventDispatcher);
    return IVector_Add(pMe->m_pListeners, pListener);
}

void CEventDispatcher_RemoveListener(IEventDispatcher* po, Listener* pListener)
{
    DECLARE_ME(CEventDispatcher);
    IVector_Delete(pMe->m_pListeners, pListener);
}

void CEventDispatcher_Notify(IEventDispatcher* po, XEvent* pEvent)
{
    DECLARE_ME(CEventDispatcher);
    int nCount = IVector_Size(pMe->m_pListeners);
    int i = 0;
    Listener* pListener = NULL;

    for (i = 0; i < nCount; i++)
    {
        IVector_GetAt(pMe->m_pListeners, i, &pListener);
        if (pListener && pListener->pfnListener) {
            pListener->pfnListener(pListener->pUserData, pEvent);
        }
    }
}
