/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __ICLASSFACTORY_H__
#define __ICLASSFACTORY_H__

#include "AEE.h"

typedef void* (*PFNNEW)();

typedef struct ClassMap
{
    AEECLSID clsid;
    PFNNEW   pfnNew;
} ClassMap;


#define INHERIT_IClassFactory(iname) \
    INHERIT_IQI(iname); \
    int (*CreateInstance)(iname* po, AEECLSID id, void** ppobj); \
    int (*RegisterClass)(iname* po, const ClassMap* pMap, int nCount)

AEEINTERFACE_DEFINE(IClassFactory);

#define IClassFactory_CreateInstance(po, id, ppobj)     AEEGETPVTBL(po, IClassFactory)->CreateInstance(po, id, ppobj)
#define IClassFactory_RegisterClass(po, pMap, nCount)   AEEGETPVTBL(po, IClassFactory)->RegisterClass(po, pMap, nCount)

#endif // __ICLASSFACTORY_H__
