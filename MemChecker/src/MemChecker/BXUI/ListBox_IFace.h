/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __LISTBOX_IFACE_H__
#define __LISTBOX_IFACE_H__

#include "IVector.h"

enum
{
    PROP_FOCUSINDEX = PROP_SPECIAL,
    PROP_SELECTINDEX,
};

// FocusIndex
static __inline boolean IWidget_SetFocusIndex(IWidget* po, int nFocusIndex)
{
    return IWidget_SetProperty(po, PROP_FOCUSINDEX, nFocusIndex);
}

static __inline boolean IWidget_GetFocusIndex(IWidget* po, int* pnFocusIndex)
{
    return IWidget_GetProperty(po, PROP_PARENT, &pnFocusIndex);
}

// SelectIndex
static __inline boolean IWidget_SelectIndex(IWidget* po, int nIndex)
{
    return IWidget_SetProperty(po, PROP_SELECTINDEX, nIndex);
}

static __inline int IWidget_ListData_SetPfnFree(IWidget* po, PFNNOTIFY pfnFree)
{
    IVector* piList = NULL;
    if (IWidget_GetDataObject(po, (IQI**)&piList))
    {
        IVector_SetPfnFree(piList, pfnFree);
        return SUCCESS;
    }
    return EFAILED;
}

static __inline int IWidget_ListData_AddItem(IWidget* po, void* pItemData)
{
    IVector* piList = NULL;
    if (IWidget_GetDataObject(po, (IQI**)&piList))
    {
        return IVector_Add(piList, pItemData);
    }
    return EFAILED;
}

static __inline void IWidget_ListData_DeleteAll(IWidget* po)
{
    IVector* piList = NULL;
    if (IWidget_GetDataObject(po, (IQI**)&piList))
    {
        IVector_DeleteAll(piList);
    }
}

static __inline int IWidget_ListData_DeleteItem(IWidget* po, int nIndex)
{
    IVector* piList = NULL;
    if (IWidget_GetDataObject(po, (IQI**)&piList))
    {
        return IVector_DeleteAt(piList, nIndex);
    }
    return EFAILED;
}

static __inline int IWidget_ListData_GetItemCount(IWidget* po)
{
    IVector* piList = NULL;
    if (IWidget_GetDataObject(po, (IQI**)&piList))
    {
        return IVector_Size(piList);
    }
    return 0;
}

static __inline int IWidget_ListData_GetItemData(IWidget* po, int nIndex, void** pData)
{
    IVector* piList = NULL;
    if (IWidget_GetDataObject(po, (IQI**)&piList))
    {
        return IVector_GetAt(piList, nIndex, pData);
    }
    return EFAILED;
}

#endif // __LISTBOX_IFACE_H__
