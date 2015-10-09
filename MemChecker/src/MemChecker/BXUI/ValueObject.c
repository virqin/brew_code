/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "ValueObject.h"
#include "XUtil.h"

IValueObject* CValueObject_New()
{
    return (IValueObject*)NewVT(CValueObject, IValueObject);
}

void CValueObject_Ctor(CValueObject* pMe)
{
    DECLARE_PVTBL(pvt, IValueObject);

    CBase_Ctor((CBase*)pMe);

    VT_FUNC(pvt, CValueObject, Release);
    VT_FUNC(pvt, CValueObject, SetValue);
    VT_FUNC(pvt, CValueObject, GetValue);
}

void CValueObject_Dtor(CValueObject* pMe)
{
    if (pMe->pData && pMe->pfnFree) {
        pMe->pfnFree(pMe->pData);
    }
}

uint32 CValueObject_Release(IValueObject* po)
{
    DECLARE_ME(CValueObject);

    if (CBase_Refs(po))
    {
        CValueObject_Dtor(pMe);
    }

    return CBase_Release((IBase*)po);
}

void CValueObject_SetValue(IValueObject* po, void* pData, int nSize, PFNNOTIFY pfnFree)
{
    DECLARE_ME(CValueObject);

    if (pMe->pData && pMe->pfnFree) {
        pMe->pfnFree(pMe->pData);
    }

    pMe->pData = pData;
    pMe->nSize = nSize;
    pMe->pfnFree = pfnFree;
}

void* CValueObject_GetValue(IValueObject* po, int* pnSize)
{
    DECLARE_ME(CValueObject);

    if (pnSize) {
        *pnSize = pMe->nSize;
    }

    return pMe->pData;
}
