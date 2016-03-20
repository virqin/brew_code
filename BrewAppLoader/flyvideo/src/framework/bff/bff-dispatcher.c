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
 * BREW Foundation Framework, Event Dispatcher.
 *
 * @author Takahiko Kawasaki
 * @file
 */


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/

#include "bff/bff.h"
#include "bff/bff-dispatcher.h"
#include "bff/bff-handlers.h"
#include "bff/bff-version-brew.h"
//modified by yuleiming
#include "mainApplet.h"
#include "sceneUtils.h"

#if BFF_BREW_VERSION_GE(3,1,3,0)
#include "AEEPointerHelpers.h"
#endif


/*------------------------------------------------------------------------*
 * STATIC FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
static boolean
method_dispatcher_dispatch(
    Bff_Dispatcher *self, Bff *bff, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed);


static boolean
dispatchMain(
    Bff_Dispatcher *self, Bff *bff, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed);


static boolean
dispatchPerEvent(
    Bff_Handlers *handlers, Bff *bff, AEEEvent ev, uint16 w, uint32 dw,
    boolean *consumed);


/*------------------------------------------------------------------------*
 * STATIC FUNCTIONS
 *------------------------------------------------------------------------*/
static boolean
method_dispatcher_dispatch(
    Bff_Dispatcher *self, Bff *bff, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed)
{
    if (!self || !bff || !ev || !w || !dw || !consumed)
    {
        if (consumed)
        {
            *consumed = FALSE;
        }

        return FALSE;
    }

    return dispatchMain(self, bff, ev, w, dw, consumed);
}


static boolean
dispatchMain(
    Bff_Dispatcher *self, Bff *bff, AEEEvent *ev, uint16 *w, uint32 *dw,
    boolean *consumed)
{
    Bff_Handlers *handlers = &self->handlers;
    boolean consumedDefault = *consumed;
    boolean consumed2;
    boolean ret = FALSE;
    boolean ret2;


    /* If preFixup() is given, */
    if (handlers->preFixup)
    {
        /* Invoke preFixup(). */
        ret = handlers->preFixup(bff, ev, w, dw, consumed);
        if (*consumed)
        {
            /*
             * preFixup() has consumed the event. Skip preDispatch() and
             * onXxx(), and jump to the post process.
             */
            goto post_process;
        }
    }

    /* If preDispatch() is given, */
    if (handlers->preDispatch)
    {
        /* Pass the default value of 'consumed' to preDispatch(). */
        *consumed = consumedDefault;

        /* Invoke preDispatch(). */
        ret = handlers->preDispatch(bff, *ev, *w, *dw, consumed);
        if (*consumed)
        {
            /*
             * preDispatch() has consumed the event. Skip onXxx() and
             * jump to the post process.
             */
            goto post_process;
        }
    }

    /* Pass the default value of 'consumed' to onXxx(). */
    *consumed = consumedDefault;

    /* Try to invoke onXxx(). */
    ret = dispatchPerEvent(handlers, bff, *ev, *w, *dw, consumed);

post_process:
    /* If postDispatch() is given, */
    if (handlers->postDispatch)
    {
        /*
         * Pass the current status of event consumption to postDispatch().
         * Implementation of postDispatch() needs to be aware that the
         * value of the fifth argument may not equal to 'consumedDefault'.
         */
        consumed2 = *consumed;

        /* Invoke postDispatch(). */
        ret2 = handlers->postDispatch(bff, *ev, *w, *dw, &consumed2);
        if (consumed2)
        {
            /*
             * postDispatch() has consumed the event. Only in this case,
             * values of 'ret' and 'consumed' are overwritten by the
             * values returned by postDispatch().
             *
             *   +--------------+----------------+--------------+
             *   | old value of | returned value | new value of |
             *   | *consumed    | of consumed2   | *consumed    |
             *   +--------------+----------------+--------------+
             *   |     TRUE     |      TRUE      |     TRUE     |
             *   +--------------+----------------+--------------+
             *   |     TRUE     |      FALSE     |     TRUE     |
             *   +--------------+----------------+--------------+
             *   |     FALSE    |      TRUE      |     TRUE     |
             *   +--------------+----------------+--------------+
             *   |     FALSE    |      FALSE     |     FALSE    |
             *   +--------------+----------------+--------------+
             */
            ret       = ret2;
            *consumed = consumed2;
        }
    }

    /* If postFixup() is given, */
    if (handlers->postFixup)
    {
        /*
         * Pass the current status of event consumption to postFixup().
         * Implementation of postFixup() needs to be aware that the
         * value of the fifth argument may not equal to 'consumedDefault'.
         */
        consumed2 = *consumed;

        /* Invoke postDispatch(). */
        ret2 = handlers->postFixup(bff, ev, w, dw, &consumed2);
        if (consumed2)
        {
            /*
             * postFixup() has consumed the event. Only in this case,
             * values of 'ret' and 'consumed' are overwritten by the
             * values returned by postFixup().
             *
             *   +--------------+----------------+--------------+
             *   | old value of | returned value | new value of |
             *   | *consumed    | of consumed2   | *consumed    |
             *   +--------------+----------------+--------------+
             *   |     TRUE     |      TRUE      |     TRUE     |
             *   +--------------+----------------+--------------+
             *   |     TRUE     |      FALSE     |     TRUE     |
             *   +--------------+----------------+--------------+
             *   |     FALSE    |      TRUE      |     TRUE     |
             *   +--------------+----------------+--------------+
             *   |     FALSE    |      FALSE     |     FALSE    |
             *   +--------------+----------------+--------------+
             */
            ret       = ret2;
            *consumed = consumed2;
        }
    }

    return ret;
}


static boolean
dispatchPerEvent(
    Bff_Handlers *handlers, Bff *bff, AEEEvent ev, uint16 w, uint32 dw,
    boolean *consumed)
{
    switch (ev)
    {
#ifdef EVT_APP_START
        case EVT_APP_START:
            if (handlers->onAppStart)
                return handlers->onAppStart(
                           bff, w, (AEEAppStart *)dw, consumed);
            break;
#endif /* EVT_APP_START */

#ifdef EVT_APP_STOP
        case EVT_APP_STOP:
            if (handlers->onAppStop)
                return handlers->onAppStop(bff, (boolean *)dw, consumed);
            break;
#endif /* EVT_APP_STOP */

#ifdef EVT_APP_SUSPEND
        case EVT_APP_SUSPEND:
            if (handlers->onAppSuspend)
                return handlers->onAppSuspend(
                           bff, (AEESuspendReason)w, (AEESuspendInfo *)dw,
                           consumed);
            break;
#endif /* EVT_APP_SUSPEND */

#ifdef EVT_APP_RESUME
        case EVT_APP_RESUME:
            if (handlers->onAppResume)
                return handlers->onAppResume(
                           bff, w, (AEEAppStart *)dw, consumed);
            break;
#endif /* EVT_APP_RESUME */

#ifdef EVT_APP_CONFIG
        case EVT_APP_CONFIG:
            if (handlers->onAppConfig)
                return handlers->onAppConfig(
                           bff, w, (AEEAppStart *)dw, consumed);
            break;
#endif /* EVT_APP_CONFIG */

#ifdef EVT_APP_HIDDEN_CONFIG
        case EVT_APP_HIDDEN_CONFIG:
            if (handlers->onAppHiddenConfig)
                return handlers->onAppHiddenConfig(
                           bff, w, (AEEAppStart *)dw, consumed);
            break;
#endif /* EVT_APP_HIDDEN_CONFIG */

#ifdef EVT_APP_BROWSE_URL
        case EVT_APP_BROWSE_URL:
            if (handlers->onAppBrowseUrl)
                return handlers->onAppBrowseUrl(
                           bff, (const char *)dw, consumed);
            break;
#endif /* EVT_APP_BROWSE_URL */

#ifdef EVT_APP_BROWSE_FILE
        case EVT_APP_BROWSE_FILE:
            if (handlers->onAppBrowseFile)
                return handlers->onAppBrowseFile(
                           bff, (const char *)dw, consumed);
            break;
#endif /* EVT_APP_BROWSE_FILE */

#ifdef EVT_APP_MESSAGE
        case EVT_APP_MESSAGE:
            if (handlers->onAppMessage)
                return handlers->onAppMessage(
                           bff, (AEESMSEncoding)w, (char *)dw, consumed);
            break;
#endif /* EVT_APP_MESSAGE */

/* if greater than or equal to BREW 2.1.0 */
#ifdef EVT_ASYNC_ERROR
        case EVT_ASYNC_ERROR:
            if (handlers->onAsyncError)
                return handlers->onAsyncError(
                           bff, (AEEAsyncError *)dw, consumed);
            break;
#endif /* EVT_ASYNC_ERROR */

#ifdef EVT_APP_TERMINATE
        case EVT_APP_TERMINATE:
            if (handlers->onAppTerminate)
                return handlers->onAppTerminate(bff, consumed);
            break;
#endif /* EVT_APP_TERMINATE */

#ifdef EVT_EXIT
        case EVT_EXIT:
            if (handlers->onExit)
                return handlers->onExit(bff, consumed);
            break;
#endif /* EVT_EXIT */

/* end if greater than or equal to BREW 2.1.0 */

/* if greater than or equal to BREW 3.0.0 */
#ifdef EVT_APP_RESTART
        case EVT_APP_RESTART:
            if (handlers->onAppRestart)
                return handlers->onAppRestart(bff, consumed);
            break;
#endif /* EVT_APP_RESTART */

#ifdef EVT_EXT_STOP
        case EVT_EXT_STOP:
            if (handlers->onExtStop)
                return handlers->onExtStop(bff, (AEECLSID *)dw, consumed);
            break;
#endif /* EVT_EXT_STOP */

#ifdef EVT_EXT_CREATE
        case EVT_EXT_CREATE:
            if (handlers->onExtCreate)
                return handlers->onExtCreate(bff, consumed);
            break;
#endif /* EVT_EXT_CREATE */

#ifdef EVT_EXT_RELEASE
        case EVT_EXT_RELEASE:
            if (handlers->onExtRelease)
                return handlers->onExtRelease(bff, consumed);
            break;
#endif /* EVT_EXT_RELEASE */

/* end if greater than or equal to BREW 3.0.0 */

/* if greater than or equal to BREW 3.1.0 */
#ifdef EVT_APP_MESSAGE_EX
        case EVT_APP_MESSAGE_EX:
            if (handlers->onAppMessageEx)
                return handlers->onAppMessageEx(bff, dw, consumed);
            break;
#endif /* EVT_APP_MESSAGE_EX */

#ifdef EVT_APP_START_BACKGROUND
        case EVT_APP_START_BACKGROUND:
            if (handlers->onAppStartBackground)
                return handlers->onAppStartBackground(
                           bff, w, (AEEAppStart *)dw, consumed);
            break;
#endif /* EVT_APP_START_BACKGROUND */

#ifdef EVT_APP_WOULD_START
        case EVT_APP_WOULD_START:
            if (handlers->onAppWouldStart)
                return handlers->onAppWouldStart(bff, consumed);
            break;
#endif /* EVT_APP_WOULD_START */

#ifdef EVT_APP_POST_URL
        case EVT_APP_POST_URL:
            if (handlers->onAppPostUrl)
                return handlers->onAppPostUrl(
                           bff, (const char *)dw, consumed);
            break;
#endif /* EVT_APP_POST_URL */

/* end if greater than or equal to BREW 3.1.0 */

/* if greater than or equal to BREW 4.0.0 */
#ifdef EVT_APP_START_WINDOW
        case EVT_APP_START_WINDOW:
            if (handlers->onAppStartWindow)
                return handlers->onAppStartWindow(bff, consumed);
            break;
#endif /* EVT_APP_START_WINDOW */

/* end if greater than or equal to BREW 4.0.0 */

        /* Key events */
#ifdef EVT_KEY
        case EVT_KEY:
            if (handlers->onKey)
			// modified by yuleiming
			{
				//if dialog showing
				if(current_dlg_type() != ESEDlgNone)
				{
					if(w == AVK_SOFT1)	//LSK
					{
						se_dlg_hit_LSK();
					}
					else if(w == AVK_SOFT2)	//RSK
					{
						se_dlg_hit_RSK();
					}
					return *consumed = TRUE;
				}
				else
				{
					return handlers->onKey(bff, w, dw, consumed);
				}
			}
            break;
#endif /* EVT_KEY */

#ifdef EVT_KEY_PRESS
        case EVT_KEY_PRESS:
            if (handlers->onKeyPress)
                return handlers->onKeyPress(bff, w, dw, consumed);
            break;
#endif /* EVT_KEY_PRESS */

#ifdef EVT_KEY_RELEASE
        case EVT_KEY_RELEASE:
            if (handlers->onKeyRelease)
                return handlers->onKeyRelease(bff, w, dw, consumed);
            break;
#endif /* EVT_KEY_RELEASE */

#ifdef EVT_CHAR
        case EVT_CHAR:
            if (handlers->onChar)
                return handlers->onChar(bff, (AECHAR)w, dw, consumed);
            break;
#endif /* EVT_CHAR */

#ifdef EVT_UPDATECHAR
        case EVT_UPDATECHAR:
            if (handlers->onUpdatechar)
                return handlers->onUpdatechar(bff, (AECHAR)w, dw, consumed);
            break;
#endif /* EVT_UPDATECHAR */

/* if less than BREW 3.0.0 */
#ifdef EVT_KEY_HELD
        case EVT_KEY_HELD:
            if (handlers->onKeyHeld)
                return handlers->onKeyHeld(bff, w, dw, consumed);
            break;
#endif /* EVT_KEY_HELD */

/* end if less than BREW 3.0.0 */

/* if greater than or equal to BREW 3.1.0 */
        /* Key hook events */
#ifdef EVT_KEY_HOOK
        case EVT_KEY_HOOK:
            if (handlers->onKeyHook)
                return handlers->onKeyHook(bff, w, dw, consumed);
            break;
#endif /* EVT_KEY_HOOK */

#ifdef EVT_KEY_HOOK_PRESS
        case EVT_KEY_HOOK_PRESS:
            if (handlers->onKeyHookPress)
                return handlers->onKeyHookPress(bff, w, dw, consumed);
            break;
#endif /* EVT_KEY_HOOK_PRESS */

#ifdef EVT_KEY_HOOK_RELEASE
        case EVT_KEY_HOOK_RELEASE:
            if (handlers->onKeyHookRelease)
                return handlers->onKeyHookRelease(bff, w, dw, consumed);
            break;
#endif /* EVT_KEY_HOOK_RELEASE */

/* end if greater than or equal to BREW 3.1.0 */

        /* Control events */
#ifdef EVT_COMMAND
        case EVT_COMMAND:
            if (handlers->onCommand)
                return handlers->onCommand(bff, w, dw, consumed);
            break;
#endif /* EVT_COMMAND */

#ifdef EVT_CTL_TAB
        case EVT_CTL_TAB:
            if (handlers->onCtlTab)
                return handlers->onCtlTab(
                           bff, (IControl *)dw, (boolean)w, consumed);
            break;
#endif /* EVT_CTL_TAB */

#ifdef EVT_CTL_SET_TITLE
        case EVT_CTL_SET_TITLE:
            if (handlers->onCtlSetTitle)
                return handlers->onCtlSetTitle(
                           bff, w, (char *)dw, consumed);
            break;
#endif /* EVT_CTL_SET_TITLE */

#ifdef EVT_CTL_SET_TEXT
        case EVT_CTL_SET_TEXT:
            if (handlers->onCtlSetText)
                return handlers->onCtlSetText(bff, w, (char *)dw, consumed);
            break;
#endif /* EVT_CTL_SET_TEXT */

#ifdef EVT_CTL_ADD_ITEM
        case EVT_CTL_ADD_ITEM:
            if (handlers->onCtlAddItem)
                return handlers->onCtlAddItem(
                           bff, (CtlAddItem *)dw, consumed);
            break;
#endif /* EVT_CTL_ADD_ITEM */

#ifdef EVT_CTL_CHANGING
        case EVT_CTL_CHANGING:
            if (handlers->onCtlChanging)
                return handlers->onCtlChanging(
                           bff, (CtlValChange *)dw, consumed);
            break;
#endif /* EVT_CTL_CHANGING */

#ifdef EVT_CTL_MENU_OPEN
        case EVT_CTL_MENU_OPEN:
            if (handlers->onCtlMenuOpen)
                return handlers->onCtlMenuOpen(bff, consumed);
            break;
#endif /* EVT_CTL_MENU_OPEN */

#ifdef EVT_CTL_SKMENU_PAGE_FULL
        case EVT_CTL_SKMENU_PAGE_FULL:
            if (handlers->onCtlSkmenuPageFull)
                return handlers->onCtlSkmenuPageFull(
                           bff, (IMenuCtl *)dw, consumed);
            break;
#endif /* EVT_CTL_SKMENU_PAGE_FULL */

#ifdef EVT_CTL_SEL_CHANGED
        case EVT_CTL_SEL_CHANGED:
            if (handlers->onCtlSelChanged)
                return handlers->onCtlSelChanged(
                           bff, w, (ITextCtl *)dw, consumed);
            break;
#endif /* EVT_CTL_SEL_CHANGED */

#ifdef EVT_CTL_TEXT_MODECHANGED
        case EVT_CTL_TEXT_MODECHANGED:
            if (handlers->onCtlTextModechanged)
                return handlers->onCtlTextModechanged(
                           bff, (AEETextInputMode)w, (ITextCtl *)dw,
                           consumed);
            break;
#endif /* EVT_CTL_TEXT_MODECHANGED */

        /* Dialog events */
#ifdef EVT_DIALOG_INIT
        case EVT_DIALOG_INIT:
            if (handlers->onDialogInit)
                return handlers->onDialogInit(bff, consumed);
            break;
#endif /* EVT_DIALOG_INIT */

#ifdef EVT_DIALOG_START
        case EVT_DIALOG_START:
            if (handlers->onDialogStart)
                return handlers->onDialogStart(
                           bff, w, (IDialog *)dw, consumed);
            break;
#endif /* EVT_DIALOG_START */

#ifdef EVT_DIALOG_END
        case EVT_DIALOG_END:
            if (handlers->onDialogEnd)
                return handlers->onDialogEnd(
                           bff, w, (IDialog *)dw, consumed);
            break;
#endif /* EVT_DIALOG_END */

#ifdef EVT_COPYRIGHT_END
        case EVT_COPYRIGHT_END:
            if (handlers->onCopyrightEnd)
                return handlers->onCopyrightEnd(bff, consumed);
            break;
#endif /* EVT_COPYRIGHT_END */

        /* Shell events */
#ifdef EVT_ALARM
        case EVT_ALARM:
            if (handlers->onAlarm)
                return handlers->onAlarm(bff, w, consumed);
            break;
#endif /* EVT_ALARM */

#ifdef EVT_NOTIFY
        case EVT_NOTIFY:
            if (handlers->onNotify)
                return handlers->onNotify(bff, (AEENotify *)dw, consumed);
            break;
#endif /* EVT_NOTIFY */

#ifdef EVT_APP_NO_CLOSE
        case EVT_APP_NO_CLOSE: /* == EVT_BUSY */
            if (handlers->onAppNoClose)
                return handlers->onAppNoClose(bff, consumed);
            break;
#endif /* EVT_APP_NO_CLOSE */

#ifdef EVT_APP_NO_SLEEP
        case EVT_APP_NO_SLEEP:
            if (handlers->onAppNoSleep)
                return handlers->onAppNoSleep(bff, consumed);
            break;
#endif /* EVT_APP_NO_SLEEP */

#ifdef EVT_MOD_LIST_CHANGED
        case EVT_MOD_LIST_CHANGED:
            if (handlers->onModListChanged)
                return handlers->onModListChanged(bff, consumed);
            break;
#endif /* EVT_MOD_LIST_CHANGED */

/* if greater than or equal to BREW 3.1.4 */
#ifdef EVT_NOTIFY_FAILURE
        case EVT_NOTIFY_FAILURE:
            if (handlers->onNotifyFailure)
                return handlers->onNotifyFailure(
                           bff, (AEENotifyFailure *)dw, consumed);
            break;
#endif /* EVT_NOTIFY_FAILURE */

/* end if greater than or equal to BREW 3.1.4 */

        /* Device events */
#ifdef EVT_FLIP
        case EVT_FLIP:
            if (handlers->onFlip)
                return handlers->onFlip(
                           bff, (boolean)w,
                           (uint8)DEVNOTIFY_GET_ID(dw),
                           (uint8)DEVNOTIFY_GET_X(dw),
                           (uint8)DEVNOTIFY_GET_Y(dw),
                           (uint8)DEVNOTIFY_GET_Z(dw), consumed);
            break;
#endif /* EVT_FLIP */

#ifdef EVT_LOCKED
        case EVT_LOCKED:
            if (handlers->onLocked)
                return handlers->onLocked(bff, (boolean)w, consumed);
            break;
#endif /* EVT_LOCKED */

#ifdef EVT_KEYGUARD
        case EVT_KEYGUARD:
            if (handlers->onKeyguard)
                return handlers->onKeyguard(
                           bff, (boolean)w,
                           (uint8)DEVNOTIFY_GET_ID(dw),
                           (uint8)DEVNOTIFY_GET_X(dw),
                           (uint8)DEVNOTIFY_GET_Y(dw),
                           (uint8)DEVNOTIFY_GET_Z(dw), consumed);
            break;
#endif /* EVT_KEYGUARD */

/* if greater than or equal to BREW 3.0.0 */
#ifdef EVT_HEADSET
        case EVT_HEADSET:
            if (handlers->onHeadset)
                return handlers->onHeadset(bff, (boolean)w, consumed);
            break;
#endif /* EVT_HEADSET */

        /* Pen events */
#ifdef EVT_PEN_DOWN
        case EVT_PEN_DOWN:
            if (handlers->onPenDown)
			{
				//modified by yuleiming
				//if dialog showing
				if(current_dlg_type() != ESEDlgNone)
				{
					if(AEE_GET_Y(dw) > DEVICE_HEIGHT - CMD_LINE_HEIGHT)
					{
						ISHELL_HandleEvent(BFF_SHELL(), EVT_KEY, cmdLine_onPointerDown(AEE_GET_X(dw),AEE_GET_Y(dw)), NULL);
					}
					return (*consumed = TRUE);
				}
				else
				{
					return handlers->onPenDown(
                           bff, AEE_GET_X(dw), AEE_GET_Y(dw),
                           (uint16)(w >> 1), consumed, w, dw);
				}
				break;
			}
#endif /* EVT_PEN_DOWN */

#ifdef EVT_PEN_MOVE
        case EVT_PEN_MOVE:
            if (handlers->onPenMove)
			{
				//modified by yuleiming
				//if dialog showing
				if(current_dlg_type() != ESEDlgNone)
				{
					return *consumed = TRUE;
				}
				else
				{
					return handlers->onPenMove(
                           bff, AEE_GET_X(dw), AEE_GET_Y(dw),
                           (uint16)(w >> 1), (boolean)(w & 1), consumed);
				}
				break;
			}
#endif /* EVT_PEN_MOVE */

#ifdef EVT_PEN_UP
        case EVT_PEN_UP:
            if (handlers->onPenUp)
			{
				//modified by yuleiming
				//if dialog showing
				if(current_dlg_type() != ESEDlgNone)
				{
					return *consumed = TRUE;
				}
				else
				{
					return handlers->onPenUp(
                           bff, AEE_GET_X(dw), AEE_GET_Y(dw),
                           (uint16)(w >> 1), consumed, w, dw);
				}
				break;
			}
#endif /* EVT_PEN_UP */

/* end if greater than or equal to BREW 3.0.0 */

/* if greater than or equal to BREW 3.1.0 */
#ifdef EVT_PEN_STALE_MOVE
        case EVT_PEN_STALE_MOVE:
            if (handlers->onPenStaleMove)
                return handlers->onPenStaleMove(
                           bff, AEE_GET_X(dw), AEE_GET_Y(dw), consumed);
            break;
#endif /* EVT_PEN_STALE_MOVE */

        /* Joystick events */
#ifdef EVT_JOYSTICK_POS
        case EVT_JOYSTICK_POS:
            if (handlers->onJoystickPos)
                return handlers->onJoystickPos(
                           bff, AEE_GET_X(dw), AEE_GET_Y(dw),
                           (uint16)(w >> 1), (boolean)(w & 1), consumed);
            break;
#endif /* EVT_JOYSTICK_POS */

#ifdef EVT_JOYSTICK_STALE_POS
        case EVT_JOYSTICK_STALE_POS:
            if (handlers->onJoystickStalePos)
                return handlers->onJoystickStalePos(
                           bff, AEE_GET_X(dw), AEE_GET_Y(dw), consumed);
            break;
#endif /* EVT_JOYSTICK_STALE_POS */

        /* Rotation events */
#ifdef EVT_SCR_ROTATE
        case EVT_SCR_ROTATE:
            if (handlers->onScrRotate)
                return handlers->onScrRotate(
                           bff, (AEEScrOrientation)w, dw, consumed);
            break;
#endif /* EVT_SCR_ROTATE */

/* end if greater than or equal to BREW 3.1.0 */

        /* Clipboard events */
#ifdef EVT_CB_CUT
        case EVT_CB_CUT:
            if (handlers->onCbCut)
                return handlers->onCbCut(bff, (const char *)dw, consumed);
            break;
#endif /* EVT_CB_CUT */

#ifdef EVT_CB_COPY
        case EVT_CB_COPY:
            if (handlers->onCbCopy)
                return handlers->onCbCopy(bff, (const char *)dw, consumed);
            break;
#endif /* EVT_CB_COPY */

#ifdef EVT_CB_PASTE
        case EVT_CB_PASTE:
            if (handlers->onCbPaste)
                return handlers->onCbPaste(bff, consumed);
            break;
#endif /* EVT_CB_PASTE */

#ifdef EVT_POINTER_DOWN
		case EVT_POINTER_DOWN:
			if (handlers->onPointerDown)
			{
				//modified by yuleiming
				//if dialog showing
				if(current_dlg_type() != ESEDlgNone)
				{
					if(AEE_POINTER_GET_Y((const char*)dw) > DEVICE_HEIGHT - CMD_LINE_HEIGHT)
					{
						ISHELL_HandleEvent(BFF_SHELL(), EVT_KEY, cmdLine_onPointerDown(AEE_POINTER_GET_X((const char*)dw),AEE_POINTER_GET_Y((const char*)dw)), NULL);
					}
					return (*consumed = TRUE);
				}
				else
				{
					return handlers->onPointerDown(bff, AEE_POINTER_GET_X((const char*)dw), AEE_POINTER_GET_Y((const char*)dw),(uint16)(w >> 1), consumed, w, dw);
				}
				break;
			}
#endif /* EVT_POINTER_DOWN */

//#ifdef EVT_POINTER_MOVE
//		case EVT_POINTER_MOVE:
//			if (handlers->onPointerMove)
//				return handlers->onPointerMove(bff,consumed);
//			break;
//#endif /* EVT_POINTER_MOVE */

#ifdef EVT_POINTER_UP
		case EVT_POINTER_UP:
			if (handlers->onPointerUp)
			{
				//modified by yuleiming
				//if dialog showing
				if(current_dlg_type() != ESEDlgNone)
				{
					return *consumed = TRUE;
				}
				else
				{
					return handlers->onPointerUp(bff, AEE_POINTER_GET_X((const char*)dw), AEE_POINTER_GET_Y((const char*)dw),(uint16)(w >> 1), consumed, w, dw);
				}
				break;
			}
#endif /* EVT_POINTER_UP */

//#ifdef EVT_POINTER_STALE_MOVE
//		case EVT_POINTER_STALE_MOVE:
//			if (handlers->onPointerStaleMove)
//				return handlers->onPointerStaleMove(bff,consumed);
//			break;
//#endif /* EVT_POINTER_STALE_MOVE */

        /* Other events */
        default:
            if (ev >= EVT_USER)
            {
                /* User events */
                if (handlers->onUser)
                {
                    return handlers->onUser(bff, ev, w, dw, consumed);
                }
            }
            else
            {
                /* Other events */
                if (handlers->onOther)
                {
                    return handlers->onOther(bff, ev, w, dw, consumed);
                }
            }
            break;
    }

    *consumed = FALSE;

    return FALSE;
}


/*------------------------------------------------------------------------*
 * FUNCTIONS
 *------------------------------------------------------------------------*/
int
Bff_Dispatcher_initialize(Bff_Dispatcher *instance, Bff_Handlers *handlers)
{
    if (instance == NULL || handlers == NULL)
    {
        return EBADPARM;
    }

    instance->dispatch = method_dispatcher_dispatch;
    instance->handlers = *handlers;

    return SUCCESS;
}
