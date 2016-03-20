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
 * @english
 * Event Handlers.
 * @endenglish
 *
 *
 * @author Takahiko Kawasaki
 * @file
 */


#ifndef BFF_HANDLERS_H
#define BFF_HANDLERS_H


/*------------------------------------------------------------------------*
 * HEADERS
 *------------------------------------------------------------------------*/
#include "AEEShell.h"
#include "AEETAPI.h"
#include "AEEDeviceNotifier.h"
#include "AEEMenu.h"
#include "AEEText.h"
#include "bff/bff-types.h"


/*------------------------------------------------------------------------*
 * TYPEDEFS
 *------------------------------------------------------------------------*/
/**
 * @english
 * An event handler that can modify values of event parameters.
 * @endenglish
 *
 */
typedef boolean
(*Bff_Handler_genericFixupFn)(
    Bff *bff, AEEEvent *ev, uint16 *w, uint32 *dw, boolean *consumed);


/**
 * @english
 * A generic event handler.
 * @endenglish
 *
 */
typedef boolean
(*Bff_Handler_genericDispatchFn)(
    Bff *bff, AEEEvent ev, uint16 w, uint32 dw, boolean *consumed);


/**
 * @english
 * An event handler that is invoked first.
 * @endenglish
 *
 */
typedef Bff_Handler_genericFixupFn Bff_Handler_preFixupFn;


/**
 * @english
 * An event handler that is invoked after preFixup().
 * @endenglish
 *
 * @japanese
 * preFixup() の後に実行されるイベントハンドラ�? * @endjapanese
 */
typedef Bff_Handler_genericDispatchFn Bff_Handler_preDispatchFn;


/**
 * @english
 * An event handler that is invoked after onXxx(). Even if one of
 * preFixup(), preDispatch() and onXxx() has consumed an event,
 * postDispatch() is always invoked (and the consumed event is
 * given in such a case). The value of '*consumed' which is given
 * as the fifth argument is TRUE if one of preFixup(), preDispatch()
 * and onXxx() has consumed an event before the event is passed to
 * postDispatch(). Otherwise, FALSE is given.
 * @endenglish
 *
 * @japanese
 * onXxx() の後に実行されるイベントハンドラー。preFixUP(), preDispatch()
 * もしくは onXxx() のいずれかがイベントを消費したとしても�? * (その場合は消費されたイベントを渡され�? postDispatch()
 * は常に実行されます。第五引数として渡される *consumed の値は�?!--
 * -->イベント�?postDispatch() に渡される前に、preFixup(),
 * preDispatch(), onXxx() のいずれかにより消費済みの場合、TRUE
 * になります。それ以外の場合�?FALSE です�? *
 * @endjapanese
 */
typedef Bff_Handler_genericDispatchFn Bff_Handler_postDispatchFn;


/**
 * @english
 * An event handler that is invoked after postDispatch(). Even if
 * one of preFixup(), preDispatch(), onXxx() and postDispatch()
 * has consumed an event, postFixup() is always invoked (and the
 * consumed event is given in such a case). The value of '*consumed'
 * which is given as the fifth argument is TRUE if one of preFixup(),
 * preDispatch(), onXxx() and postDispatch() has consumed an event
 * before the event is passed to postFixup(). Otherwise, FALSE is
 * given.
 * @endenglish
 *
 * @japanese
 * postDispatch() の後に実行されるイベントハンドラー。preFixup(),
 * preDispatch(), onXxx(), postDispatch() のいずれかがイベント�?!--
 * -->消費したとしても�?その場合は消費されたイベントを渡され�?
 * postFixup() は常に実行されます。第五引数として渡される *consumed
 * の値は、イベントが postFixup() に渡される前に、preFixup(),
 * preDispatch(), onXxx(), postDispatch() のいずれかにより<!--
 * -->消費済みの場合、TRUE になります。それ以外の場合�?FALSE です�? * @endjapanese
 */
typedef Bff_Handler_genericFixupFn Bff_Handler_postFixupFn;


#if defined(EVT_APP_START) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppStartFn)(
    Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed);
#endif /* defined(EVT_APP_START) || defined(DOXYGEN) */


#if defined(EVT_APP_STOP) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppStopFn)(Bff *bff, boolean *close, boolean *consumed);
#endif /* defined(EVT_APP_STOP) || defined(DOXYGEN) */


#if defined(EVT_APP_SUSPEND) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppSuspendFn)(
    Bff *bff, AEESuspendReason reason, AEESuspendInfo *info,
    boolean *consumed);
#endif /* defined(EVT_APP_SUSPEND) || defined(DOXYGEN) */


#if defined(EVT_APP_RESUME) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppResumeFn)(
    Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed);
#endif /* defined(EVT_APP_RESUME) || defined(DOXYGEN) */


#if defined(EVT_APP_CONFIG) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppConfigFn)(
    Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed);
#endif /* defined(EVT_APP_CONFIG) || defined(DOXYGEN) */


#if defined(EVT_APP_HIDDEN_CONFIG) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppHiddenConfigFn)(
    Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed);
#endif /* defined(EVT_APP_HIDDEN_CONFIG) || defined(DOXYGEN) */


#if defined(EVT_APP_BROWSE_URL) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppBrowseUrlFn)(
    Bff *bff, const char *url, boolean *consumed);
#endif /* defined(EVT_APP_BROWSE_URL) || defined(DOXYGEN) */


#if defined(EVT_APP_BROWSE_FILE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppBrowseFileFn)(
    Bff *bff, const char *file, boolean *consumed);
#endif /* defined(EVT_APP_BROWSE_FILE) || defined(DOXYGEN) */


#if defined(EVT_APP_MESSAGE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppMessageFn)(
    Bff *bff, AEESMSEncoding encoding, char *text, boolean *consumed);
#endif /* defined(EVT_APP_MESSAGE) || defined(DOXYGEN) */


/* if greater than or equal to BREW 2.1.0 */


#if defined(EVT_ASYNC_ERROR) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAsyncErrorFn)(
    Bff *bff, AEEAsyncError *error, boolean *consumed);
#endif /* defined(EVT_ASYNC_ERROR) || defined(DOXYGEN) */


#if defined(EVT_APP_TERMINATE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppTerminateFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_APP_TERMINATE) || defined(DOXYGEN) */


#if defined(EVT_EXIT) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onExitFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_EXIT) || defined(DOXYGEN) */


/* end if greater than or equal to BREW 2.1.0 */


/* if greater than or equal to BREW 3.0.0 */


#if defined(EVT_APP_RESTART) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppRestartFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_APP_RESTART) || defined(DOXYGEN) */


#if defined(EVT_EXT_STOP) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onExtStopFn)(
    Bff *bff, AEECLSID *extensions, boolean *consumed);
#endif /* defined(EVT_EXT_STOP) || defined(DOXYGEN) */


#if defined(EVT_EXT_CREATE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onExtCreateFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_EXT_CREATE) || defined(DOXYGEN) */


#if defined(EVT_EXT_RELEASE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onExtReleaseFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_EXT_RELEASE) || defined(DOXYGEN) */


/* end if greater than or equal to BREW 3.0.0 */


/* if greater than or equal to BREW 3.1.0 */


#if defined(EVT_APP_MESSAGE_EX) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppMessageExFn)(
    Bff *bff, uint32 transactionId, boolean *consumed);
#endif /* defined(EVT_APP_MESSAGE_EX) || defined(DOXYGEN) */


#if defined(EVT_APP_START_BACKGROUND) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppStartBackgroundFn)(
    Bff *bff, uint16 flags, AEEAppStart *appStart, boolean *consumed);
#endif /* defined(EVT_APP_START_BACKGROUND) || defined(DOXYGEN) */


#if defined(EVT_APP_WOULD_START) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppWouldStartFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_APP_WOULD_START) || defined(DOXYGEN) */

#if defined(EVT_APP_POST_URL) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppPostUrlFn)(Bff *bff, const char *url, boolean *consumed);
#endif /* defined(EVT_APP_POST_URL) || defined(DOXYGEN) */


/* end if greater than or equal to BREW 3.1.0 */


/* if greater than or equal to BREW 4.0.0 */


#if defined(EVT_APP_START_WINDOW) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppStartWindowFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_APP_START_WINDOW) || defined(DOXYGEN) */


/* end if greater than or equal to BREW 4.0.0 */


#if defined(EVT_KEY) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onKeyFn)(
    Bff *bff, AVKType key, uint32 modifiers, boolean *consumed);
#endif /* defined(EVT_KEY) || defined(DOXYGEN) */


#if defined(EVT_KEY_PRESS) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onKeyPressFn)(
    Bff *bff, AVKType key, uint32 modifiers, boolean *consumed);
#endif /* defined(EVT_KEY_PRESS) || defined(DOXYGEN) */


#if defined(EVT_KEY_RELEASE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onKeyReleaseFn)(
    Bff *bff, AVKType key, uint32 modifiers, boolean *consumed);
#endif /* defined(EVT_KEY_RELEASE) || defined(DOXYGEN) */


#if defined(EVT_CHAR) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCharFn)(
    Bff *bff, AECHAR ch, uint32 modifiers, boolean *consumed);
#endif /* defined(EVT_CHAR) || defined(DOXYGEN) */


#if defined(EVT_UPDATECHAR) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onUpdatecharFn)(
    Bff *bff, AECHAR ch, uint32 modifiers, boolean *consumed);
#endif /* defined(EVT_UPDATECHAR) || defined(DOXYGEN) */


/* if less than BREW 3.0.0 */


#if defined(EVT_KEY_HELD) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onKeyHeldFn)(
    Bff *bff, AVKType key, uint32 modifiers boolean *consumed);
#endif /* defined(EVT_KEY_HELD) || defined(DOXYGEN) */


/* end if less than BREW 3.0.0 */


/* if greater than or equal to BREW 3.1.0 */


#if defined(EVT_KEY_HOOK) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onKeyHookFn)(
    Bff *bff, AVKType key, uint32 modifiers, boolean *consumed);
#endif /* defined(EVT_KEY_HOOK) || defined(DOXYGEN) */


#if defined(EVT_KEY_HOOK_PRESS) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onKeyHookPressFn)(
    Bff *bff, AVKType key, uint32 modifiers, boolean *consumed);
#endif /* defined(EVT_KEY_HOOK_PRESS) || defined(DOXYGEN) */


#if defined(EVT_KEY_HOOK_RELEASE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onKeyHookReleaseFn)(
    Bff *bff, AVKType key, uint32 modifiers, boolean *consumed);
#endif /* defined(EVT_KEY_HOOK_RELEASE) || defined(DOXYGEN) */


/* end if greater than or equal to BREW 3.1.0 */


#if defined(EVT_COMMAND) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCommandFn)(
    Bff *bff, uint16 w, uint32 dw, boolean *consumed);
#endif /* defined(EVT_COMMAND) || defined(DOXYGEN) */


#if defined(EVT_CTL_TAB) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCtlTabFn)(
    Bff *bff, IControl *control, boolean right, boolean *consumed);
#endif /* defined(EVT_CTL_TAB) || defined(DOXYGEN) */


#if defined(EVT_CTL_SET_TITLE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCtlSetTitleFn)(
    Bff *bff, uint16 id, char *fileOrText, boolean *consumed);
#endif /* defined(EVT_CTL_SET_TITLE) || defined(DOXYGEN) */


#if defined(EVT_CTL_SET_TEXT) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCtlSetTextFn)(
    Bff *bff, uint16 id, char *fileOrText, boolean *consumed);
#endif /* defined(EVT_CTL_SET_TEXT) || defined(DOXYGEN) */


#if defined(EVT_CTL_ADD_ITEM) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCtlAddItemFn)(
    Bff *bff, CtlAddItem *item, boolean *consumed);
#endif /* defined(EVT_CTL_ADD_ITEM) || defined(DOXYGEN) */


#if defined(EVT_CTL_CHANGING) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCtlChangingFn)(
    Bff *bff, CtlValChange *change, boolean *consumed);
#endif /* defined(EVT_CTL_CHANGING) || defined(DOXYGEN) */


#if defined(EVT_CTL_MENU_OPEN) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCtlMenuOpenFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_CTL_MENU_OPEN) || defined(DOXYGEN) */


#if defined(EVT_CTL_SKMENU_PAGE_FULL) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCtlSkmenuPageFullFn)(
    Bff *bff, IMenuCtl *menuCtl, boolean *consumed);
#endif /* defined(EVT_CTL_SKMENU_PAGE_FULL) || defined(DOXYGEN) */


#if defined(EVT_CTL_SEL_CHANGED) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCtlSelChangedFn)(
    Bff *bff, uint16 id, ITextCtl *textCtl, boolean *consumed);
#endif /* defined(EVT_CTL_SEL_CHANGED) || defined(DOXYGEN) */


#if defined(EVT_CTL_TEXT_MODECHANGED) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCtlTextModechangedFn)(
    Bff *bff, AEETextInputMode mode, ITextCtl *textCtl, boolean *consumed);
#endif /* defined(EVT_CTL_TEXT_MODECHANGED) || defined(DOXYGEN) */


#if defined(EVT_DIALOG_INIT) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onDialogInitFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_DIALOG_INIT) || defined(DOXYGEN) */


#if defined(EVT_DIALOG_START) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onDialogStartFn)(
    Bff *bff, uint16 id, IDialog *dialog, boolean *consumed);
#endif /* defined(EVT_DIALOG_START) || defined(DOXYGEN) */


#if defined(EVT_DIALOG_END) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onDialogEndFn)(
    Bff *bff, uint16 id, IDialog *dialog, boolean *consumed);
#endif /* defined(EVT_DIALOG_END) || defined(DOXYGEN) */


#if defined(EVT_COPYRIGHT_END) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCopyrightEndFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_COPYRIGHT_END) || defined(DOXYGEN) */


#if defined(EVT_ALARM) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAlarmFn)(Bff *bff, uint16 userCode, boolean *consumed);
#endif /* defined(EVT_ALARM) || defined(DOXYGEN) */


#if defined(EVT_NOTIFY) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onNotifyFn)(Bff *bff, AEENotify *notify, boolean *consumed);
#endif /* defined(EVT_NOTIFY) || defined(DOXYGEN) */


#if defined(EVT_APP_NO_CLOSE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppNoCloseFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_APP_NO_CLOSE) || defined(DOXYGEN) */


#if defined(EVT_APP_NO_SLEEP) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onAppNoSleepFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_APP_NO_SLEEP) || defined(DOXYGEN) */


#if defined(EVT_MOD_LIST_CHANGED) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onModListChangedFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_MOD_LIST_CHANGED) || defined(DOXYGEN) */


/* if greater than or equal to BREW 3.1.4 */


#if defined(EVT_NOTIFY_FAILURE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onNotifyFailureFn)(
    Bff *bff, AEENotifyFailure *failure, boolean *consumed);
#endif /* defined(EVT_NOTIFY_FAILURE) || defined(DOXYGEN) */


/* end if greater than or equal to BREW 3.1.4 */


#if defined(EVT_FLIP) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onFlipFn)(
    Bff *bff, boolean open, uint8 id, uint8 x, uint8 y, uint8 z,
    boolean *consumed);
#endif /* defined(EVT_FLIP) || defined(DOXYGEN) */


#if defined(EVT_LOCKED) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onLockedFn)(Bff *bff, boolean locked, boolean *consumed);
#endif /* defined(EVT_LOCKED) || defined(DOXYGEN) */


#if defined(EVT_KEYGUARD) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onKeyguardFn)(
    Bff *bff, boolean on, uint8 id, uint8 x, uint8 y, uint8 z,
    boolean *consumed);
#endif /* defined(EVT_KEYGUARD) || defined(DOXYGEN) */


/* if greater than or equal to BREW 3.0.0 */


#if defined(EVT_HEADSET) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onHeadsetFn)(Bff *bff, boolean plugged, boolean *consumed);
#endif /* defined(EVT_HEADSET) || defined(DOXYGEN) */


#if defined(EVT_PEN_DOWN) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onPenDownFn)(
    Bff *bff, int16 x, int16 y, uint16 milliseconds, boolean *consumed, uint16 w, uint32 dw);
#endif /* defined(EVT_PEN_DOWN) || defined(DOXYGEN) */


#if defined(EVT_PEN_MOVE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onPenMoveFn)(
    Bff *bff, int16 x, int16 y, uint16 milliseconds, boolean dropped,
    boolean *consumed);
#endif /* defined(EVT_PEN_MOVE) || defined(DOXYGEN) */


#if defined(EVT_PEN_UP) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onPenUpFn)(
    Bff *bff, int16 x, int16 y, uint16 milliseconds, boolean *consumed,uint16 w,uint32 dw);
#endif /* defined(EVT_PEN_UP) || defined(DOXYGEN) */


/* end if greater than or equal to BREW 3.0.0 */


/* if greater than or equal to BREW 3.1.0 */


#if defined(EVT_PEN_STALE_MOVE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onPenStaleMoveFn)(
    Bff *bff, int16 x, int16 y, boolean *consumed);
#endif /* defined(EVT_PEN_STALE_MOVE) || defined(DOXYGEN) */


#if defined(EVT_JOYSTICK_POS) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onJoystickPosFn)(
    Bff *bff, int16 x, int16 y, uint16 milliseconds, boolean dropped,
    boolean *consumed);
#endif /* defined(EVT_JOYSTICK_POS) || defined(DOXYGEN) */


#if defined(EVT_JOYSTICK_STALE_POS) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onJoystickStalePosFn)(
    Bff *bff, int16 x, int16 y, boolean *consumed);
#endif /* defined(EVT_JOYSTICK_STALE_POS) || defined(DOXYGEN) */


#if defined(EVT_SCR_ROTATE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onScrRotateFn)(
    Bff *bff, AEEScrOrientation orientation, uint32 screenId,
    boolean *consumed);
#endif /* defined(EVT_SCR_ROTATE) || defined(DOXYGEN) */


/* end if greater than or equal to BREW 3.1.0 */


#if defined(EVT_CB_CUT) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCbCutFn)(
    Bff *bff, const char *preferredFormat, boolean *consumed);
#endif /* defined(EVT_CB_CUT) || defined(DOXYGEN) */


#if defined(EVT_CB_COPY) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCbCopyFn)(
    Bff *bff, const char *preferredFormat, boolean *consumed);
#endif /* defined(EVT_CB_COPY) || defined(DOXYGEN) */


#if defined(EVT_CB_PASTE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onCbPasteFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_CB_PASTE) || defined(DOXYGEN) */

#if defined(EVT_POINTER_DOWN) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onPointerDownFn)(Bff *bff, int16 x, int16 y, uint16 milliseconds, boolean *consumed, uint16 w, uint32 dw);
#endif /* defined(EVT_POINTER_DOWN) || defined(DOXYGEN) */

#if defined(EVT_POINTER_MOVE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onPointerMoveFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_POINTER_MOVE) || defined(DOXYGEN) */

#if defined(EVT_POINTER_UP) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onPointerUpFn)(Bff *bff, int16 x, int16 y, uint16 milliseconds, boolean *consumed, uint16 w, uint32 dw);
#endif /* defined(EVT_POINTER_UP) || defined(DOXYGEN) */

#if defined(EVT_POINTER_STALE_MOVE) || defined(DOXYGEN)
typedef boolean
(*Bff_Handler_onPointerStaleMoveFn)(Bff *bff, boolean *consumed);
#endif /* defined(EVT_POINTER_STALE_MOVE) || defined(DOXYGEN) */

/**
 * @english
 * An event handler for events whose event codes are equal to or greater
 * than EVT_USER.
 * @endenglish
 *
 * @japanese
 * EVT_USER と等しいかそれ以上の値のエラーコードを持つイベント用�?!--
 * -->イベントハンドラ�? * @endjapanese
 */
typedef Bff_Handler_genericDispatchFn Bff_Handler_onUserFn;


/**
 * @english
 * An event handler for events which are not treated by other onXxx()
 * event handlers.
 * @endenglish
 *
 * @japanese
 * 他の onXxx() イベントハンドラーにより扱われないイベント用�?!--
 * -->イベントハンドラ�? * @endjapanese
 */
typedef Bff_Handler_genericDispatchFn Bff_Handler_onOtherFn;


/*------------------------------------------------------------------------*
 * STRUCT DECLARATIONS
 *------------------------------------------------------------------------*/
/*
struct Bff_Handlers;
typedef struct Bff_Handlers Bff_Handlers;*/



/*------------------------------------------------------------------------*
 * STRUCTS
 *------------------------------------------------------------------------*/
/**
 * @english
 * Event Handlers
 *
 * Bff_Dispatcher dispatches BREW events using a given instance of
 * Bff_Handlers.
 *
 * <b>The content of Bff_Handlers varies depending on the BREW version</b>,
 * so the set of data fields of Bff_Handlers described in the doxygen
 * document your are seeing now may be different from the set which is
 * usable when you compile your applications. Refer to the actual header
 * file directly.
 * @endenglish
 *
 * @japanese
 * イベントハンドラー群
 *
 * Bff_Dispatcher は、渡され�?Bff_Handlers のインスタンスを用いて、BREW
 * イベントをディスパッチします�? *
 * <b>Bff_Handlers の内容は、BREW のバージョンにより異なります�?/b>
 * そのため、現在あなたが閲覧してい�?doxygen ドキュメントに記�?!--
 * -->されてい�?Bff_Handlers のデータフィールドのセットは、あなた�?!--
 * -->アプリケーションをコンパイルするときに使用可能なセットとは�?!--
 * -->異なっているかもしれません。実際のヘッダファイルを直接参�?!--
 * -->してください�? * @endjapanese
 */
typedef struct _Bff_Handlers
{
    Bff_Handler_preFixupFn             preFixup;
    Bff_Handler_preDispatchFn          preDispatch;
    Bff_Handler_postDispatchFn         postDispatch;
    Bff_Handler_postFixupFn            postFixup;

#if defined(EVT_APP_START) || defined(DOXYGEN)
    Bff_Handler_onAppStartFn           onAppStart;
#endif /* defined(EVT_APP_START) || defined(DOXYGEN) */

#if defined(EVT_APP_STOP) || defined(DOXYGEN)
    Bff_Handler_onAppStopFn            onAppStop;
#endif /* defined(EVT_APP_STOP) || defined(DOXYGEN) */

#if defined(EVT_APP_SUSPEND) || defined(DOXYGEN)
    Bff_Handler_onAppSuspendFn         onAppSuspend;
#endif /* defined(EVT_APP_SUSPEND) || defined(DOXYGEN) */

#if defined(EVT_APP_RESUME) || defined(DOXYGEN)
    Bff_Handler_onAppResumeFn          onAppResume;
#endif /* defined(EVT_APP_RESUME) || defined(DOXYGEN) */

#if defined(EVT_APP_CONFIG) || defined(DOXYGEN)
    Bff_Handler_onAppConfigFn          onAppConfig;
#endif /* defined(EVT_APP_CONFIG) || defined(DOXYGEN) */

#if defined(EVT_APP_HIDDEN_CONFIG) || defined(DOXYGEN)
    Bff_Handler_onAppHiddenConfigFn    onAppHiddenConfig;
#endif /* defined(EVT_APP_HIDDEN_CONFIG) || defined(DOXYGEN) */

#if defined(EVT_APP_BROWSE_URL) || defined(DOXYGEN)
    Bff_Handler_onAppBrowseUrlFn       onAppBrowseUrl;
#endif /* defined(EVT_APP_BROWSE_URL) || defined(DOXYGEN) */

#if defined(EVT_APP_BROWSE_FILE) || defined(DOXYGEN)
    Bff_Handler_onAppBrowseFileFn      onAppBrowseFile;
#endif /* defined(EVT_APP_BROWSE_FILE) || defined(DOXYGEN) */

#if defined(EVT_APP_MESSAGE) || defined(DOXYGEN)
    Bff_Handler_onAppMessageFn         onAppMessage;
#endif /* defined(EVT_APP_MESSAGE) || defined(DOXYGEN) */
    /* if greater than or equal to BREW 2.1.0 */

#if defined(EVT_ASYNC_ERROR) || defined(DOXYGEN)
    Bff_Handler_onAsyncErrorFn         onAsyncError;
#endif /* defined(EVT_ASYNC_ERROR) || defined(DOXYGEN) */

#if defined(EVT_APP_TERMINATE) || defined(DOXYGEN)
    Bff_Handler_onAppTerminateFn       onAppTerminate;
#endif /* defined(EVT_APP_TERMINATE) || defined(DOXYGEN) */

#if defined(EVT_EXIT) || defined(DOXYGEN)
    Bff_Handler_onExitFn               onExit;
#endif /* defined(EVT_EXIT) || defined(DOXYGEN) */
    /* end if greater than or equal to BREW 2.1.0 */
    /* if greater than or equal to BREW 3.0.0 */

#if defined(EVT_APP_RESTART) || defined(DOXYGEN)
    Bff_Handler_onAppRestartFn         onAppRestart;
#endif /* defined(EVT_APP_RESTART) || defined(DOXYGEN) */

#if defined(EVT_EXT_STOP) || defined(DOXYGEN)
    Bff_Handler_onExtStopFn            onExtStop;
#endif /* defined(EVT_EXT_STOP) || defined(DOXYGEN) */

#if defined(EVT_EXT_CREATE) || defined(DOXYGEN)
    Bff_Handler_onExtCreateFn          onExtCreate;
#endif /* defined(EVT_EXT_CREATE) || defined(DOXYGEN) */

#if defined(EVT_EXT_RELEASE) || defined(DOXYGEN)
    Bff_Handler_onExtReleaseFn         onExtRelease;
#endif /* defined(EVT_EXT_RELEASE) || defined(DOXYGEN) */
    /* end if greater than or equal to BREW 3.0.0 */
    /* if greater than or equal to BREW 3.1.0 */

#if defined(EVT_APP_MESSAGE_EX) || defined(DOXYGEN)
    Bff_Handler_onAppMessageExFn       onAppMessageEx;
#endif /* defined(EVT_APP_MESSAGE_EX) || defined(DOXYGEN) */

#if defined(EVT_APP_START_BACKGROUND) || defined(DOXYGEN)
    Bff_Handler_onAppStartBackgroundFn onAppStartBackground;
#endif /* defined(EVT_APP_START_BACKGROUND) || defined(DOXYGEN) */

#if defined(EVT_APP_WOULD_START) || defined(DOXYGEN)
    Bff_Handler_onAppWouldStartFn      onAppWouldStart;
#endif /* defined(EVT_APP_WOULD_START) || defined(DOXYGEN) */

#if defined(EVT_APP_POST_URL) || defined(DOXYGEN)
    Bff_Handler_onAppPostUrlFn         onAppPostUrl;
#endif /* defined(EVT_APP_POST_URL) || defined(DOXYGEN) */
    /* end if greater than or equal to BREW 3.1.0 */
    /* if greater than or equal to BREW 4.0.0 */

#if defined(EVT_APP_START_WINDOW) || defined(DOXYGEN)
    Bff_Handler_onAppStartWindowFn     onAppStartWindow;
#endif /* defined(EVT_APP_START_WINDOW) || defined(DOXYGEN) */
    /* end if greater than or equal to BREW 4.0.0 */

#if defined(EVT_KEY) || defined(DOXYGEN)
    Bff_Handler_onKeyFn                onKey;
#endif /* defined(EVT_KEY) || defined(DOXYGEN) */

#if defined(EVT_KEY_PRESS) || defined(DOXYGEN)
    Bff_Handler_onKeyPressFn           onKeyPress;
#endif /* defined(EVT_KEY_PRESS) || defined(DOXYGEN) */

#if defined(EVT_KEY_RELEASE) || defined(DOXYGEN)
    Bff_Handler_onKeyReleaseFn         onKeyRelease;
#endif /* defined(EVT_KEY_RELEASE) || defined(DOXYGEN) */

#if defined(EVT_CHAR) || defined(DOXYGEN)
    Bff_Handler_onCharFn               onChar;
#endif /* defined(EVT_CHAR) || defined(DOXYGEN) */

#if defined(EVT_UPDATECHAR) || defined(DOXYGEN)
    Bff_Handler_onUpdatecharFn         onUpdatechar;
#endif /* defined(EVT_UPDATECHAR) || defined(DOXYGEN) */
    /* if less than BREW 3.0.0 */

#if defined(EVT_KEY_HELD) || defined(DOXYGEN)
    Bff_Handler_onKeyHeldFn            onKeyHeld;
#endif /* defined(EVT_KEY_HELD) || defined(DOXYGEN) */
    /* end if less than BREW 3.0.0 */
    /* if greater than or equal to BREW 3.1.0 */

#if defined(EVT_KEY_HOOK) || defined(DOXYGEN)
    Bff_Handler_onKeyHookFn            onKeyHook;
#endif /* defined(EVT_KEY_HOOK) || defined(DOXYGEN) */

#if defined(EVT_KEY_HOOK_PRESS) || defined(DOXYGEN)
    Bff_Handler_onKeyHookPressFn       onKeyHookPress;
#endif /* defined(EVT_KEY_HOOK_PRESS) || defined(DOXYGEN) */

#if defined(EVT_KEY_HOOK_RELEASE) || defined(DOXYGEN)
    Bff_Handler_onKeyHookReleaseFn     onKeyHookRelease;
#endif /* defined(EVT_KEY_HOOK_RELEASE) || defined(DOXYGEN) */
    /* end if greater than or equal to BREW 3.1.0 */

#if defined(EVT_COMMAND) || defined(DOXYGEN)
    Bff_Handler_onCommandFn            onCommand;
#endif /* defined(EVT_COMMAND) || defined(DOXYGEN) */

#if defined(EVT_CTL_TAB) || defined(DOXYGEN)
    Bff_Handler_onCtlTabFn             onCtlTab;
#endif /* defined(EVT_CTL_TAB) || defined(DOXYGEN) */

#if defined(EVT_CTL_SET_TITLE) || defined(DOXYGEN)
    Bff_Handler_onCtlSetTitleFn        onCtlSetTitle;
#endif /* defined(EVT_CTL_SET_TITLE) || defined(DOXYGEN) */

#if defined(EVT_CTL_SET_TEXT) || defined(DOXYGEN)
    Bff_Handler_onCtlSetTextFn         onCtlSetText;
#endif /* defined(EVT_CTL_SET_TEXT) || defined(DOXYGEN) */

#if defined(EVT_CTL_ADD_ITEM) || defined(DOXYGEN)
    Bff_Handler_onCtlAddItemFn         onCtlAddItem;
#endif /* defined(EVT_CTL_ADD_ITEM) || defined(DOXYGEN) */

#if defined(EVT_CTL_CHANGING) || defined(DOXYGEN)
    Bff_Handler_onCtlChangingFn        onCtlChanging;
#endif /* defined(EVT_CTL_CHANGING) || defined(DOXYGEN) */

#if defined(EVT_CTL_MENU_OPEN) || defined(DOXYGEN)
    Bff_Handler_onCtlMenuOpenFn        onCtlMenuOpen;
#endif /* defined(EVT_CTL_MENU_OPEN) || defined(DOXYGEN) */

#if defined(EVT_CTL_SKMENU_PAGE_FULL) || defined(DOXYGEN)
    Bff_Handler_onCtlSkmenuPageFullFn  onCtlSkmenuPageFull;
#endif /* defined(EVT_CTL_SKMENU_PAGE_FULL) || defined(DOXYGEN) */

#if defined(EVT_CTL_SEL_CHANGED) || defined(DOXYGEN)
    Bff_Handler_onCtlSelChangedFn      onCtlSelChanged;
#endif /* defined(EVT_CTL_SEL_CHANGED) || defined(DOXYGEN) */

#if defined(EVT_CTL_TEXT_MODECHANGED) || defined(DOXYGEN)
    Bff_Handler_onCtlTextModechangedFn onCtlTextModechanged;
#endif /* defined(EVT_CTL_TEXT_MODECHANGED) || defined(DOXYGEN) */

#if defined(EVT_DIALOG_INIT) || defined(DOXYGEN)
    Bff_Handler_onDialogInitFn         onDialogInit;
#endif /* defined(EVT_DIALOG_INIT) || defined(DOXYGEN) */

#if defined(EVT_DIALOG_START) || defined(DOXYGEN)
    Bff_Handler_onDialogStartFn        onDialogStart;
#endif /* defined(EVT_DIALOG_START) || defined(DOXYGEN) */

#if defined(EVT_DIALOG_END) || defined(DOXYGEN)
    Bff_Handler_onDialogEndFn          onDialogEnd;
#endif /* defined(EVT_DIALOG_END) || defined(DOXYGEN) */

#if defined(EVT_COPYRIGHT_END) || defined(DOXYGEN)
    Bff_Handler_onCopyrightEndFn       onCopyrightEnd;
#endif /* defined(EVT_COPYRIGHT_END) || defined(DOXYGEN) */

#if defined(EVT_ALARM) || defined(DOXYGEN)
    Bff_Handler_onAlarmFn              onAlarm;
#endif /* defined(EVT_ALARM) || defined(DOXYGEN) */

#if defined(EVT_NOTIFY) || defined(DOXYGEN)
    Bff_Handler_onNotifyFn             onNotify;
#endif /* defined(EVT_NOTIFY) || defined(DOXYGEN) */

#if defined(EVT_APP_NO_CLOSE) || defined(DOXYGEN)
    Bff_Handler_onAppNoCloseFn         onAppNoClose;
#endif /* defined(EVT_APP_NO_CLOSE) || defined(DOXYGEN) */

#if defined(EVT_APP_NO_SLEEP) || defined(DOXYGEN)
    Bff_Handler_onAppNoSleepFn         onAppNoSleep;
#endif /* defined(EVT_APP_NO_SLEEP) || defined(DOXYGEN) */

#if defined(EVT_MOD_LIST_CHANGED) || defined(DOXYGEN)
    Bff_Handler_onModListChangedFn     onModListChanged;
#endif /* defined(EVT_MOD_LIST_CHANGED) || defined(DOXYGEN) */
    /* if greater than or equal to BREW 3.1.4 */

#if defined(EVT_NOTIFY_FAILURE) || defined(DOXYGEN)
    Bff_Handler_onNotifyFailureFn      onNotifyFailure;
#endif /* defined(EVT_NOTIFY_FAILURE) || defined(DOXYGEN) */
    /* end if greater than or equal to BREW 3.1.4 */

#if defined(EVT_FLIP) || defined(DOXYGEN)
    Bff_Handler_onFlipFn               onFlip;
#endif /* defined(EVT_FLIP) || defined(DOXYGEN) */

#if defined(EVT_LOCKED) || defined(DOXYGEN)
    Bff_Handler_onLockedFn             onLocked;
#endif /* defined(EVT_LOCKED) || defined(DOXYGEN) */

#if defined(EVT_KEYGUARD) || defined(DOXYGEN)
    Bff_Handler_onKeyguardFn           onKeyguard;
#endif /* defined(EVT_KEYGUARD) || defined(DOXYGEN) */
    /* if greater than or equal to BREW 3.0.0 */

#if defined(EVT_HEADSET) || defined(DOXYGEN)
    Bff_Handler_onHeadsetFn            onHeadset;
#endif /* defined(EVT_HEADSET) || defined(DOXYGEN) */

#if defined(EVT_PEN_DOWN) || defined(DOXYGEN)
    Bff_Handler_onPenDownFn            onPenDown;
#endif /* defined(EVT_PEN_DOWN) || defined(DOXYGEN) */

#if defined(EVT_PEN_MOVE) || defined(DOXYGEN)
    Bff_Handler_onPenMoveFn            onPenMove;
#endif /* defined(EVT_PEN_MOVE) || defined(DOXYGEN) */

#if defined(EVT_PEN_UP) || defined(DOXYGEN)
    Bff_Handler_onPenUpFn              onPenUp;
#endif /* defined(EVT_PEN_UP) || defined(DOXYGEN) */
    /* end if greater than or equal to BREW 3.0.0 */
    /* if greater than or equal to BREW 3.1.0 */

#if defined(EVT_PEN_STALE_MOVE) || defined(DOXYGEN)
    Bff_Handler_onPenStaleMoveFn       onPenStaleMove;
#endif /* defined(EVT_PEN_STALE_MOVE) || defined(DOXYGEN) */

#if defined(EVT_JOYSTICK_POS) || defined(DOXYGEN)
    Bff_Handler_onJoystickPosFn        onJoystickPos;
#endif /* defined(EVT_JOYSTICK_POS) || defined(DOXYGEN) */

#if defined(EVT_JOYSTICK_STALE_POS) || defined(DOXYGEN)
    Bff_Handler_onJoystickStalePosFn   onJoystickStalePos;
#endif /* defined(EVT_JOYSTICK_STALE_POS) || defined(DOXYGEN) */

#if defined(EVT_SCR_ROTATE) || defined(DOXYGEN)
    Bff_Handler_onScrRotateFn          onScrRotate;
#endif /* defined(EVT_SCR_ROTATE) || defined(DOXYGEN) */
    /* end if greater than or equal to BREW 3.1.0 */

#if defined(EVT_CB_CUT) || defined(DOXYGEN)
    Bff_Handler_onCbCutFn              onCbCut;
#endif /* defined(EVT_CB_CUT) || defined(DOXYGEN) */

#if defined(EVT_CB_COPY) || defined(DOXYGEN)
    Bff_Handler_onCbCopyFn             onCbCopy;
#endif /* defined(EVT_CB_COPY) || defined(DOXYGEN) */

#if defined(EVT_CB_PASTE) || defined(DOXYGEN)
    Bff_Handler_onCbPasteFn            onCbPaste;
#endif /* defined(EVT_CB_PASTE) || defined(DOXYGEN) */

#if defined(EVT_POINTER_DOWN) || defined(DOXYGEN)
    Bff_Handler_onPointerDownFn            onPointerDown;
#endif /* defined(EVT_POINTER_DOWN) || defined(DOXYGEN) */

//#if defined(EVT_POINTER_MOVE) || defined(DOXYGEN)
//    Bff_Handler_onPointerMoveFn            onPointerMove;
//#endif /* defined(EVT_POINTER_MOVE) || defined(DOXYGEN) */

#if defined(EVT_POINTER_UP) || defined(DOXYGEN)
    Bff_Handler_onPointerUpFn            onPointerUp;
#endif /* defined(EVT_POINTER_UP) || defined(DOXYGEN) */

//#if defined(EVT_POINTER_STALE_MOVE) || defined(DOXYGEN)
//    Bff_Handler_onPointerStaleMoveFn            onPointerStaleMove;
//#endif /* defined(EVT_POINTER_STALE_MOVE) || defined(DOXYGEN) */

    Bff_Handler_onUserFn               onUser;
    Bff_Handler_onOtherFn              onOther;
};


/*------------------------------------------------------------------------*
 * FUNCTION PROTOTYPES
 *------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @english
 * Clear a Bff_Handlers structure.
 *
 * @param[in] handlers  A Bff_Handlers structure to be cleared. If NULL is
 *                      given, nothing is done.
 * @endenglish
 *
 * @japanese
 * Bff_Handlers 構造体をクリアする�? *
 * @param[in] handlers  クリアす�?Bff_Handlers 構造体。ヌルポインタが<!--
 *                      -->渡された場合、何もしません�? * @endjapanese
 */
void
Bff_Handlers_clear(Bff_Handlers *handlers);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* BFF_HANDLERS_H */
