/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __WIDGETEVENT_H__
#define __WIDGETEVENT_H__

typedef struct XEventInfo
{
    uint16  eCode;
    uint16  wParam;
    uint32  dwParam;
} XEventInfo;

enum
{
    XEVT_SYSTEM,

    XEVT_GETPROPERTY,
    XEVT_SETPROPERTY,

    XEVT_MOUSEDOWN,
    XEVT_MOUSEMOVE,
    XEVT_MOUSEUP,
    XEVT_KEY,
    XEVT_KEYDOWN,
    XEVT_KEYUP,

    XEVT_NOTIFY,

    XEVT_USER,

    XEVT_UNDEFINED = -1
};

enum
{
    XEVT_CLICK,
    XEVT_DOUBLECLICK,

    XEVT_COMMAND,
    XEVT_SCROLL,

    XEVT_FOCUS_CHANGE,
    XEVT_FOCUS_SELECT,
};

#endif
