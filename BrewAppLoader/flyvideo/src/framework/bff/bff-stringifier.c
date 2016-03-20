/*
 * Copyright (c) 2008 Takahiko Kawasaki
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


/**
 * BREW Foundation Framework, Stringifier.
 *
 * @author Takahiko Kawasaki
 * @file
 */


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEShell.h"
#include "bff/bff-stringifier.h"
#include "bff/bff-version-brew.h"


/*------------------------------------------------------------------------*
 * FUNCTIONS
 *------------------------------------------------------------------------*/
const char *
Bff_Stringifier_stringifyEvent(AEEEvent event)
{
    switch (event)
    {
#ifdef EVT_APP_START
        case EVT_APP_START:
            return "EVT_APP_START";
#endif /* EVT_APP_START */

#ifdef EVT_APP_STOP
        case EVT_APP_STOP:
            return "EVT_APP_STOP";
#endif /* EVT_APP_STOP */

#ifdef EVT_APP_SUSPEND
        case EVT_APP_SUSPEND:
            return "EVT_APP_SUSPEND";
#endif /* EVT_APP_SUSPEND */

#ifdef EVT_APP_RESUME
        case EVT_APP_RESUME:
            return "EVT_APP_RESUME";
#endif /* EVT_APP_RESUME */

#ifdef EVT_APP_CONFIG
        case EVT_APP_CONFIG:
            return "EVT_APP_CONFIG";
#endif /* EVT_APP_CONFIG */

#ifdef EVT_APP_HIDDEN_CONFIG
        case EVT_APP_HIDDEN_CONFIG:
            return "EVT_APP_HIDDEN_CONFIG";
#endif /* EVT_APP_HIDDEN_CONFIG */

#ifdef EVT_APP_BROWSE_URL
        case EVT_APP_BROWSE_URL:
            return "EVT_APP_BROWSE_URL";
#endif /* EVT_APP_BROWSE_URL */

#ifdef EVT_APP_BROWSE_FILE
        case EVT_APP_BROWSE_FILE:
            return "EVT_APP_BROWSE_FILE";
#endif /* EVT_APP_BROWSE_FILE */

#ifdef EVT_APP_MESSAGE
        case EVT_APP_MESSAGE:
            return "EVT_APP_MESSAGE";
#endif /* EVT_APP_MESSAGE */

/* if greater than or equal to BREW 2.1.0 */
#ifdef EVT_ASYNC_ERROR
        case EVT_ASYNC_ERROR:
            return "EVT_ASYNC_ERROR";
#endif /* EVT_ASYNC_ERROR */

#ifdef EVT_APP_TERMINATE
        case EVT_APP_TERMINATE:
            return "EVT_APP_TERMINATE";
#endif /* EVT_APP_TERMINATE */

#ifdef EVT_EXIT
        case EVT_EXIT:
            return "EVT_EXIT";
#endif /* EVT_EXIT */

/* end if greater than or equal to BREW 2.1.0 */

/* if greater than or equal to BREW 3.0.0 */
#ifdef EVT_APP_RESTART
        case EVT_APP_RESTART:
            return "EVT_APP_RESTART";
#endif /* EVT_APP_RESTART */

#ifdef EVT_EXT_STOP
        case EVT_EXT_STOP:
            return "EVT_EXT_STOP";
#endif /* EVT_EXT_STOP */

#ifdef EVT_EXT_CREATE
        case EVT_EXT_CREATE:
            return "EVT_EXT_CREATE";
#endif /* EVT_EXT_CREATE */

#ifdef EVT_EXT_RELEASE
        case EVT_EXT_RELEASE:
            return "EVT_EXT_RELEASE";
#endif /* EVT_EXT_RELEASE */

/* end if greater than or equal to BREW 3.0.0 */

/* if greater than or equal to BREW 3.1.0 */
#ifdef EVT_APP_MESSAGE_EX
        case EVT_APP_MESSAGE_EX:
            return "EVT_APP_MESSAGE_EX";
#endif /* EVT_APP_MESSAGE_EX */

#ifdef EVT_APP_START_BACKGROUND
        case EVT_APP_START_BACKGROUND:
            return "EVT_APP_START_BACKGROUND";
#endif /* EVT_APP_START_BACKGROUND */

#ifdef EVT_APP_WOULD_START
        case EVT_APP_WOULD_START:
            return "EVT_APP_WOULD_START";
#endif /* EVT_APP_WOULD_START */

#ifdef EVT_APP_POST_URL
        case EVT_APP_POST_URL:
            return "EVT_APP_POST_URL";
#endif /* EVT_APP_POST_URL */

/* end if greater than or equal to BREW 3.1.0 */

/* if greater than or equal to BREW 4.0.0 */
#ifdef EVT_APP_START_WINDOW
        case EVT_APP_START_WINDOW:
            return "EVT_APP_START_WINDOW";
#endif /* EVT_APP_START_WINDOW */

/* end if greater than or equal to BREW 4.0.0 */

        /* Key events */
#ifdef EVT_KEY
        case EVT_KEY:
            return "EVT_KEY";
#endif /* EVT_KEY */

#ifdef EVT_KEY_PRESS
        case EVT_KEY_PRESS:
            return "EVT_KEY_PRESS";
#endif /* EVT_KEY_PRESS */

#ifdef EVT_KEY_RELEASE
        case EVT_KEY_RELEASE:
            return "EVT_KEY_RELEASE";
#endif /* EVT_KEY_RELEASE */

#ifdef EVT_CHAR
        case EVT_CHAR:
            return "EVT_CHAR";
#endif /* EVT_CHAR */

#ifdef EVT_UPDATECHAR
        case EVT_UPDATECHAR:
            return "EVT_UPDATECHAR";
#endif /* EVT_UPDATECHAR */

/* if less than BREW 3.0.0 */
#ifdef EVT_KEY_HELD
        case EVT_KEY_HELD:
            return "EVT_KEY_HELD";
#endif /* EVT_KEY_HELD */

/* end if less than BREW 3.0.0 */

/* if greater than or equal to BREW 3.1.0 */
        /* Key hook events */
#ifdef EVT_KEY_HOOK
        case EVT_KEY_HOOK:
            return "EVT_KEY_HOOK";
#endif /* EVT_KEY_HOOK */

#ifdef EVT_KEY_HOOK_PRESS
        case EVT_KEY_HOOK_PRESS:
            return "EVT_KEY_HOOK_PRESS";
#endif /* EVT_KEY_HOOK_PRESS */

#ifdef EVT_KEY_HOOK_RELEASE
        case EVT_KEY_HOOK_RELEASE:
            return "EVT_KEY_HOOK_RELEASE";
#endif /* EVT_KEY_HOOK_RELEASE */

/* end if greater than or equal to BREW 3.1.0 */

        /* Control events */
#ifdef EVT_COMMAND
        case EVT_COMMAND:
            return "EVT_COMMAND";
#endif /* EVT_COMMAND */

#ifdef EVT_CTL_TAB
        case EVT_CTL_TAB:
            return "EVT_CTL_TAB";
#endif /* EVT_CTL_TAB */

#ifdef EVT_CTL_SET_TITLE
        case EVT_CTL_SET_TITLE:
            return "EVT_CTL_SET_TITLE";
#endif /* EVT_CTL_SET_TITLE */

#ifdef EVT_CTL_SET_TEXT
        case EVT_CTL_SET_TEXT:
            return "EVT_CTL_SET_TEXT";
#endif /* EVT_CTL_SET_TEXT */

#ifdef EVT_CTL_ADD_ITEM
        case EVT_CTL_ADD_ITEM:
            return "EVT_CTL_ADD_ITEM";
#endif /* EVT_CTL_ADD_ITEM */

#ifdef EVT_CTL_CHANGING
        case EVT_CTL_CHANGING:
            return "EVT_CTL_CHANGING";
#endif /* EVT_CTL_CHANGING */

#ifdef EVT_CTL_MENU_OPEN
        case EVT_CTL_MENU_OPEN:
            return "EVT_CTL_MENU_OPEN";
#endif /* EVT_CTL_MENU_OPEN */

#ifdef EVT_CTL_SKMENU_PAGE_FULL
        case EVT_CTL_SKMENU_PAGE_FULL:
            return "EVT_CTL_SKMENU_PAGE_FULL";
#endif /* EVT_CTL_SKMENU_PAGE_FULL */

#ifdef EVT_CTL_SEL_CHANGED
        case EVT_CTL_SEL_CHANGED:
            return "EVT_CTL_SEL_CHANGED";
#endif /* EVT_CTL_SEL_CHANGED */

#ifdef EVT_CTL_TEXT_MODECHANGED
        case EVT_CTL_TEXT_MODECHANGED:
            return "EVT_CTL_TEXT_MODECHANGED";
#endif /* EVT_CTL_TEXT_MODECHANGED */

        /* Dialog events */
#ifdef EVT_DIALOG_INIT
        case EVT_DIALOG_INIT:
            return "EVT_DIALOG_INIT";
#endif /* EVT_DIALOG_INIT */

#ifdef EVT_DIALOG_START
        case EVT_DIALOG_START:
            return "EVT_DIALOG_START";
#endif /* EVT_DIALOG_START */

#ifdef EVT_DIALOG_END
        case EVT_DIALOG_END:
            return "EVT_DIALOG_END";
#endif /* EVT_DIALOG_END */

#ifdef EVT_COPYRIGHT_END
        case EVT_COPYRIGHT_END:
            return "EVT_COPYRIGHT_END";
#endif /* EVT_COPYRIGHT_END */

        /* Shell events */
#ifdef EVT_ALARM
        case EVT_ALARM:
            return "EVT_ALARM";
#endif /* EVT_ALARM */

#ifdef EVT_NOTIFY
        case EVT_NOTIFY:
            return "EVT_NOTIFY";
#endif /* EVT_NOTIFY */

#ifdef EVT_APP_NO_CLOSE
        case EVT_APP_NO_CLOSE:
            return "EVT_APP_NO_CLOSE"; /* == EVT_BUSY */
#endif /* EVT_APP_NO_CLOSE */

#ifdef EVT_APP_NO_SLEEP
        case EVT_APP_NO_SLEEP:
            return "EVT_APP_NO_SLEEP";
#endif /* EVT_APP_NO_SLEEP */

#ifdef EVT_MOD_LIST_CHANGED
        case EVT_MOD_LIST_CHANGED:
            return "EVT_MOD_LIST_CHANGED";
#endif /* EVT_MOD_LIST_CHANGED */

/* if greater than or equal to BREW 3.1.4 */
#ifdef EVT_NOTIFY_FAILURE
        case EVT_NOTIFY_FAILURE:
            return "EVT_NOTIFY_FAILURE";
#endif /* EVT_NOTIFY_FAILURE */

/* end if greater than or equal to BREW 3.1.4 */

        /* Device events */
#ifdef EVT_FLIP
        case EVT_FLIP:
            return "EVT_FLIP";
#endif /* EVT_FLIP */

#ifdef EVT_LOCKED
        case EVT_LOCKED:
            return "EVT_LOCKED";
#endif /* EVT_LOCKED */

#ifdef EVT_KEYGUARD
        case EVT_KEYGUARD:
            return "EVT_KEYGUARD";
#endif /* EVT_KEYGUARD */

/* if greater than or equal to BREW 3.0.0 */
#ifdef EVT_HEADSET
        case EVT_HEADSET:
            return "EVT_HEADSET";
#endif /* EVT_HEADSET */

        /* Pen events */
#ifdef EVT_PEN_DOWN
        case EVT_PEN_DOWN:
            return "EVT_PEN_DOWN";
#endif /* EVT_PEN_DOWN */

#ifdef EVT_PEN_MOVE
        case EVT_PEN_MOVE:
            return "EVT_PEN_MOVE";
#endif /* EVT_PEN_MOVE */

#ifdef EVT_PEN_UP
        case EVT_PEN_UP:
            return "EVT_PEN_UP";
#endif /* EVT_PEN_UP */

/* end if greater than or equal to BREW 3.0.0 */

/* if greater than or equal to BREW 3.1.0 */
#ifdef EVT_PEN_STALE_MOVE
        case EVT_PEN_STALE_MOVE:
            return "EVT_PEN_STALE_MOVE";
#endif /* EVT_PEN_STALE_MOVE */

        /* Joystick events */
#ifdef EVT_JOYSTICK_POS
        case EVT_JOYSTICK_POS:
            return "EVT_JOYSTICK_POS";
#endif /* EVT_JOYSTICK_POS */

#ifdef EVT_JOYSTICK_STALE_POS
        case EVT_JOYSTICK_STALE_POS:
            return "EVT_JOYSTICK_STALE_POS";
#endif /* EVT_JOYSTICK_STALE_POS */

        /* Rotation events */
#ifdef EVT_SCR_ROTATE
        case EVT_SCR_ROTATE:
            return "EVT_SCR_ROTATE";
#endif /* EVT_SCR_ROTATE */

/* end if greater than or equal to BREW 3.1.0 */

        /* Clipboard events */
#ifdef EVT_CB_CUT
        case EVT_CB_CUT:
            return "EVT_CB_CUT";
#endif /* EVT_CB_CUT */

#ifdef EVT_CB_COPY
        case EVT_CB_COPY:
            return "EVT_CB_COPY";
#endif /* EVT_CB_COPY */

#ifdef EVT_CB_PASTE
        case EVT_CB_PASTE:
            return "EVT_CB_PASTE";
#endif /* EVT_CB_PASTE */

        default:
            return NULL;
    }
}


const char *
Bff_Stringifier_stringifyKey(AVKType key)
{
    switch (key)
    {
        case AVK_UNDEFINED:
            return "AVK_UNDEFINED";

        case AVK_FIRST:
            return "AVK_FIRST";

        case AVK_0:
            return "AVK_0";

        case AVK_1:
            return "AVK_1";

        case AVK_2:
            return "AVK_2";

        case AVK_3:
            return "AVK_3";

        case AVK_4:
            return "AVK_4";

        case AVK_5:
            return "AVK_5";

        case AVK_6:
            return "AVK_6";

        case AVK_7:
            return "AVK_7";

        case AVK_8:
            return "AVK_8";

        case AVK_9:
            return "AVK_9";

        case AVK_STAR:
            return "AVK_STAR";

        case AVK_POUND:
            return "AVK_POUND";

        case AVK_POWER:
            return "AVK_POWER";

        case AVK_END:
            return "AVK_END";

        case AVK_SEND:
            return "AVK_SEND";

        case AVK_CLR:
            return "AVK_CLR";

        case AVK_UP:
            return "AVK_UP";

        case AVK_DOWN:
            return "AVK_DOWN";

        case AVK_LEFT:
            return "AVK_LEFT";

        case AVK_RIGHT:
            return "AVK_RIGHT";

        case AVK_SELECT:
            return "AVK_SELECT";

        case AVK_SOFT1:
            return "AVK_SOFT1";

        case AVK_SOFT2:
            return "AVK_SOFT2";

        case AVK_SOFT3:
            return "AVK_SOFT3";

        case AVK_SOFT4:
            return "AVK_SOFT4";

        case AVK_FUNCTION1:
            return "AVK_FUNCTION1";

        case AVK_FUNCTION2:
            return "AVK_FUNCTION2";

        case AVK_FUNCTION3:
            return "AVK_FUNCTION3";

        case AVK_FUNCTION:
            return "AVK_FUNCTION";

        case AVK_MENU:
            return "AVK_MENU";

        case AVK_INFO:
            return "AVK_INFO";

        case AVK_SHIFT:
            return "AVK_SHIFT";

        case AVK_MESSAGE:
            return "AVK_MESSAGE";

        case AVK_MUTE:
            return "AVK_MUTE";

        case AVK_STORE:
            return "AVK_STORE";

        case AVK_RECALL:
            return "AVK_RECALL";

        case AVK_PUNC1:
            return "AVK_PUNC1";

        case AVK_PUNC2:
            return "AVK_PUNC2";

        case AVK_VOLUME_UP:
            return "AVK_VOLUME_UP";

        case AVK_VOLUME_DOWN:
            return "AVK_VOLUME_DOWN";

        case AVK_WEB_ACCESS:
            return "AVK_WEB_ACCESS";

        case AVK_VOICE_MEMO:
            return "AVK_VOICE_MEMO";

        case AVK_SPEAKER:
            return "AVK_SPEAKER";

#if BFF_BREW_VERSION_GE(1,1,0,0)
        case AVK_TAP:
            return "AVK_TAP";
#endif /* BFF_BREW_VERSION_GE(1,1,0,0) */

#if BFF_BREW_VERSION_GE(2,0,0,0)
        case AVK_PTT:
            return "AVK_PTT";

        case AVK_LSHIFT:
            return "AVK_LSHIFT";

        case AVK_RSHIFT:
            return "AVK_RSHIFT";

        case AVK_LCTRL:
            return "AVK_LCTRL";

        case AVK_RCTRL:
            return "AVK_RCTRL";

        case AVK_LALT:
            return "AVK_LALT";

        case AVK_RALT:
            return "AVK_RALT";

        case AVK_CAPLK:
            return "AVK_CAPLK";

        case AVK_SCRLK:
            return "AVK_SCRLK";

        case AVK_NUMLK:
            return "AVK_NUMLK";

        case AVK_LNGHANGUL:
            return "AVK_LNGHANGUL";

        case AVK_LNGJUNJA:
            return "AVK_LNGJUNJA";

        case AVK_LNGFIN:
            return "AVK_LNGFIN";

        case AVK_LNGHANJA:
            return "AVK_LNGHANJA";

        case AVK_PRSCRN:
            return "AVK_PRSCRN";

        case AVK_BREAK:
            return "AVK_BREAK";

        case AVK_TXINSERT:
            return "AVK_TXINSERT";

        case AVK_TXDELETE:
            return "AVK_TXDELETE";

        case AVK_TXHOME:
            return "AVK_TXHOME";

        case AVK_TXEND:
            return "AVK_TXEND";

        case AVK_TXPGUP:
            return "AVK_TXPGUP";

        case AVK_TXPGDOWN:
            return "AVK_TXPGDOWN";
#endif /* BFF_BREW_VERSION_GE(2,0,0,0) */

#if BFF_BREW_VERSION_GE(3,0,0,0)
        case AVK_GP_1:
            return "AVK_GP_1";

        case AVK_GP_2:
            return "AVK_GP_2";

        case AVK_GP_3:
            return "AVK_GP_3";

        case AVK_GP_4:
            return "AVK_GP_4";

        case AVK_GP_5:
            return "AVK_GP_5";

        case AVK_GP_6:
            return "AVK_GP_6";

        case AVK_GP_SL:
            return "AVK_GP_SL";

        case AVK_GP_SR:
            return "AVK_GP_SR";
#endif /* BFF_BREW_VERSION_GE(3,0,0,0) */

#if BFF_BREW_VERSION_GE(3,1,3,0)
        case AVK_CAMERA:
            return "AVK_CAMERA";

        case AVK_VIDEO_PHONE:
            return "AVK_VIDEO_PHONE";

        case AVK_APP_SWITCH:
            return "AVK_APP_SWITCH";
#endif /* BFF_BREW_VERSION_GE(3,1,0,0) */

#if BFF_BREW_VERSION_GE(4,0,0,0)
        case AVK_PLAY:
            return "AVK_PLAY";

        case AVK_PAUSE:
            return "AVK_PAUSE";

        case AVK_SKIP_FWD:
            return "AVK_SKIP_FWD";

        case AVK_SKIP_RWD:
            return "AVK_SKIP_RWD";

        case AVK_FFWD:
            return "AVK_FFWD";

        case AVK_RWD:
            return "AVK_RWD";

        case AVK_STOP:
            return "AVK_STOP";

        case AVK_HEADSET_FUNC:
            return "AVK_HEADSET_FUNC";
#endif /* BFF_BREW_VERSION_GE(4,0,0,0) */

        default:
            return NULL;
    }
}
