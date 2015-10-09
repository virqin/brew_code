/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __BASE_H__
#define __BASE_H__

#include "AEE.h"

typedef struct CBase
{
    AEEVTBL(IBase) *pvt;
    uint32 nRefs;
} CBase;

#define CBase_Refs(po) ((CBase *)po)->nRefs

uint32  CBase_AddRef(IBase *po);

uint32  CBase_Release(IBase *po);

static __inline void CBase_Ctor(CBase *me)
{
    me->pvt->AddRef = CBase_AddRef;
    me->pvt->Release = CBase_Release;

    me->nRefs = 1;
}

#endif // __BASE_H__
