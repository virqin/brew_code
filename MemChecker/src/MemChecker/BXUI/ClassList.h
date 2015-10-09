/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __CLASSLIST_H__
#define __CLASSLIST_H__

#include "IClassFactory.h"

enum
{
    XCLSID_ValueObject,
    XCLSID_Vector,
    XCLSID_Control,
    XCLSID_Container,
    XCLSID_Window,
    XCLSID_DesktopWindow,
    XCLSID_StaticCtl,
    XCLSID_Button,
    XCLSID_ListBox,
    XCLSID_ScrollBar,
    XCLSID_Menu,
    XCLSID_IControlWrap,
};

enum
{
    XIID_IBase,
    XIID_IQI,
    XIID_IValueObject,
    XIID_IVector,
    XIID_IHandler,
    XIID_IWidget,
    XIID_IContainer,
    XIID_IWindow,
    XIID_IDesktopWindow,
};

IClassFactory* XClassFactory_New();

#endif // __CLASSLIST_H__
