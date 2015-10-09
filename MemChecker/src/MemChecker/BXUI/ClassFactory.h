/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __CLASSFACTORY_H__
#define __CLASSFACTORY_H__

#include "IClassFactory.h"
#include "Base.h"

typedef struct CClassFactory
{
    CBase     base;
    ClassMap* m_pMapTable;
    int       m_nCount;
} CClassFactory;

IClassFactory*  CClassFactory_New();
void            CClassFactory_Ctor(CClassFactory* pMe);
uint32          CClassFactory_Release(IClassFactory* po);
int             CClassFactory_QueryInterface(IClassFactory* po, AEEIID iid, void** ppo);
int             CClassFactory_CreateInstance(IClassFactory* po, AEECLSID id, void** ppobj);
int             CClassFactory_RegisterClass(IClassFactory* po, const ClassMap* pMap, int nCount);

#endif
