/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __IVALUEOBJECT_H__
#define __IVALUEOBJECT_H__

#include "AEE.h"

#define INHERIT_IValueObject(iname) \
    INHERIT_IQI(iname); \
    void    (*SetValue)(iname* po, void* pData, int nSize, PFNNOTIFY pfnFree); \
    void*   (*GetValue)(iname* po, int* pnSize)

AEEINTERFACE_DEFINE(IValueObject);

#define IValueObject_Release(po)                            AEEGETPVTBL(po, IValueObject)->Release(po)
#define IValueObject_SetValue(po, pData, nSize, pfnFree)    AEEGETPVTBL(po, IValueObject)->SetValue(po, pData, nSize, pfnFree)
#define IValueObject_GetValue(po, pnSize)                   AEEGETPVTBL(po, IValueObject)->GetValue(po, pnSize)

#endif // __IVALUEOBJECT_H__
