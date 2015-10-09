/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __VALUEOBJECT_H__
#define __VALUEOBJECT_H__

#include "IValueObject.h"
#include "Base.h"

typedef struct CValueObject
{
    CBase       base;
    void*       pData;
    int         nSize;
    PFNNOTIFY   pfnFree;
} CValueObject;

IValueObject*   CValueObject_New(void);
void            CValueObject_Ctor(CValueObject* pMe);
void            CValueObject_Dtor(CValueObject* pMe);
uint32          CValueObject_Release(IValueObject* po);
void            CValueObject_SetValue(IValueObject* po, void* pData, int nSize, PFNNOTIFY pfnFree);
void*           CValueObject_GetValue(IValueObject* po, int* pnSize);

#endif // __VALUEOBJECT_H__
