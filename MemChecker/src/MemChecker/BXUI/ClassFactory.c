/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "ClassFactory.h"
#include "XUtil.h"

IClassFactory* CClassFactory_New()
{
    return (IClassFactory*)NewVT(CClassFactory, IClassFactory);
}

void CClassFactory_Ctor(CClassFactory* pMe)
{
    DECLARE_PVTBL(pvt, IClassFactory);

    CBase_Ctor(&pMe->base);

    VT_FUNC(pvt, CClassFactory, Release);
    VT_FUNC(pvt, CClassFactory, QueryInterface);
    VT_FUNC(pvt, CClassFactory, CreateInstance);
    VT_FUNC(pvt, CClassFactory, RegisterClass);
}


uint32 CClassFactory_Release(IClassFactory* po)
{
    DECLARE_ME(CClassFactory);

    if (1 == CBase_Refs(po))
    {
        FREEIF(pMe->m_pMapTable);
    }

    return CBase_Release((IBase*)po);
}

int CClassFactory_QueryInterface(IClassFactory* po, AEEIID iid, void** ppo)
{
    (void)po;
    (void)iid;
    (void)ppo;
    return ECLASSNOTSUPPORT;
}

int CClassFactory_CreateInstance(IClassFactory* po, AEECLSID id, void** ppobj)
{
    DECLARE_ME(CClassFactory);
    int i = 0;

    for (i = 0; i < pMe->m_nCount; i++)
    {
        if (pMe->m_pMapTable[i].clsid == id)
        {
            *ppobj = pMe->m_pMapTable[i].pfnNew();
            return SUCCESS;
        }
    }

    return ECLASSNOTSUPPORT;
}

int CClassFactory_RegisterClass(IClassFactory* po, const ClassMap* pMap, int nCount)
{
    DECLARE_ME(CClassFactory);

    FREEIF(pMe->m_pMapTable);
    pMe->m_pMapTable = MALLOC(nCount * sizeof(ClassMap));
    pMe->m_nCount = nCount;
    if (NULL == pMe->m_pMapTable)
    {
        return ENOMEMORY;
    }

    MEMCPY(pMe->m_pMapTable, pMap, nCount * sizeof(ClassMap));

    return TRUE;
}
