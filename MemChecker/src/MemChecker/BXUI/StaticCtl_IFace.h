/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __STATICCTL_IFACE_H__
#define __STATICCTL_IFACE_H__

#include "IValueObject.h"

// Text
static __inline boolean IWidget_SetText(IWidget* po, const AECHAR* pszText, boolean bAutoFree)
{
    return IWidget_SetValueData(po, (void*)pszText, 
        ((NULL == pszText) ? 0 : WSTRLEN(pszText)), 
        (bAutoFree ? FREE : NULL));
}

static __inline boolean IWidget_GetText(IWidget* po, AECHAR** pszText)
{
    return IWidget_GetValueData(po, pszText, NULL);
}

#endif // __STATICCTL_IFACE_H__
