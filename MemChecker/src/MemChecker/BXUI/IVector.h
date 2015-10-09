/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __IVECTOR_H__
#define __IVECTOR_H__

#include "AEE.h"

#define INHERIT_IVector(iname) \
    INHERIT_IQI(iname); \
    uint32      (*Size)(iname* po); \
    int         (*GetAt)(iname* po, uint32 nIndex, void** ppe); \
    int         (*Replace)(iname* po, uint32 nIndex, void* pe); \
    int         (*Insert)(iname* po, uint32 nIndex, void* pe); \
    int         (*DeleteAt)(iname* po, uint32 nIndex); \
    void        (*DeleteAll)(iname* po); \
    PFNNOTIFY   (*SetPfnFree)(iname* po, PFNNOTIFY pfnFree); \
    void        (*SetGrowCount)(iname* po, int n); \
    int         (*Reserve)(iname* po, uint32 n)

AEEINTERFACE_DEFINE(IVector);

#define IVector_Release(po)             AEEGETPVTBL(po, IVector)->Release(po)
#define IVector_Size(po)                AEEGETPVTBL(po, IVector)->Size(po)
#define IVector_GetAt(po, nIndex, ppe)  AEEGETPVTBL(po, IVector)->GetAt(po, nIndex, ppe)
#define IVector_Replace(po, nIndex, pe) AEEGETPVTBL(po, IVector)->Replace(po, nIndex, pe)
#define IVector_Insert(po, nIndex, pe)  AEEGETPVTBL(po, IVector)->Insert(po, nIndex, pe)
#define IVector_DeleteAt(po, nIndex)    AEEGETPVTBL(po, IVector)->DeleteAt(po, nIndex)
#define IVector_DeleteAll(po)           AEEGETPVTBL(po, IVector)->DeleteAll(po)
#define IVector_SetPfnFree(po, pfnFree) AEEGETPVTBL(po, IVector)->SetPfnFree(po, pfnFree)
#define IVector_SetGrowCount(po, n)     AEEGETPVTBL(po, IVector)->SetGrowCount(po, n)
#define IVector_Reserve(po, n)          AEEGETPVTBL(po, IVector)->Reserve(po, n)

#define IVector_Add(po, pe)             IVector_Insert(po, (uint32)-1, pe)

static __inline uint32 IVector_GetIndex(IVector* po, void* pe)
{
    int nCount = IVector_Size(po);
    void *peTmp = NULL;

    while (nCount-- > 0)
    {
        if (SUCCESS == IVector_GetAt(po, nCount, &peTmp) && peTmp == pe) {
            return nCount;
        }
    }

    return (uint32)-1;
}

static __inline int IVector_Delete(IVector* po, void* pe)
{
    int nIndex = IVector_GetIndex(po, pe);
    if (nIndex != -1) {
        return IVector_DeleteAt(po, nIndex);
    }

    return EFAILED;
}

#endif // __IVECTOR_H__
