/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "ClassList.h"
#include "ClassFactory.h"
#include "ValueObject.h"
#include "Vector.h"
#include "Container.h"
#include "Window.h"
#include "DesktopWindow.h"
#include "StaticCtl.h"
#include "Button.h"
#include "ListBox.h"
#include "ScrollBar.h"
#include "Menu.h"
#include "IControlWrap.h"

IClassFactory* XClassFactory_New()
{
    const ClassMap aClassMap[] = 
    {
        {XCLSID_ValueObject,    CValueObject_New},
        {XCLSID_Vector,         CVector_New},
        {XCLSID_Control,        CControl_New},
        {XCLSID_Container,      CContainer_New},
        {XCLSID_Window,         CWindow_New},
        {XCLSID_DesktopWindow,  CDesktopWindow_New},
        {XCLSID_StaticCtl,      CStaticCtl_New},
        {XCLSID_Button,         CButton_New},
        {XCLSID_ListBox,        CListBox_New},
        {XCLSID_ScrollBar,      CScrollBar_New},
        {XCLSID_Menu,           CMenu_New},
        {XCLSID_IControlWrap,   CIControlWrap_New}
    };
    IClassFactory* po = CClassFactory_New();

    if (po) {
        IClassFactory_RegisterClass(po, aClassMap, ARR_SIZE(aClassMap));
    }

    return po;
}
