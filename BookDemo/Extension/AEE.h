#ifndef AEE_H
#define AEE_H
/*======================================================
FILE:  AEE.h

SERVICES:  Base level AEE include file.

GENERAL DESCRIPTION:
	Base level definitions, typedefs, etc. for AEE

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEClassIDs.h"
#include "AEEError.h"
#ifdef BREW_MODULE
#include "AEEModuleDefs.h"
#endif

#define DIRECTORY_CHAR  '/'
#define DIRECTORY_STR   "/"


// Standard Headers

#include "AEEComdef.h"


//
// Basic Types
//

typedef struct
{
   uint16   wYear;
   uint16   wMonth;
   uint16   wDay;
   uint16   wHour;
   uint16   wMinute;
   uint16   wSecond;
   uint16   wWeekDay;
} JulianType;

typedef struct 
{
   int16    x,y;
   int16   dx, dy;
} AEERect;

typedef enum 
{
   SC_UNKNOWN,
   SC_ALPHA,
   SC_DIGIT,
   SC_WHITESPACE
} TChType;

typedef struct _AEEImageInfo
{
   uint16   cx;
   uint16   cy;
   uint16   nColors;
   boolean  bAnimated;
   uint16   cxFrame;
} AEEImageInfo;


typedef uint32             AEECLSID;
typedef uint32             AEEMODID;
typedef uint16             AECHAR;

typedef void (*PFNNOTIFY)(void * pUser);


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

/* the following I really wanted to put in AEEComdef */

#ifndef PSTR /* implies null-terminated string */
typedef  char * PSTR;
#endif

#ifndef PCSTR /* implies null-terminated string */
typedef  const char * PCSTR;
#endif

#ifndef CONSTRAIN
#define  CONSTRAIN( val, min, max ) (((val) < (min)) ? (min) : ((val) > (max)) ? (max) : (val))
#endif

#ifndef BETWEEN
#define  BETWEEN( val, minGE, maxLT ) ((unsigned)(val) - (unsigned)(minGE) < (unsigned)(maxLT) - (unsigned)(minGE))
#endif


#ifndef ARRAY_SIZE
#define  ARRAY_SIZE(a) (sizeof((a))/sizeof((a)[0]))
#endif

#ifndef ARRAY_MEMBER
#define  ARRAY_MEMBER(p,a) (((p) >= (a)) && ((p) < ((a) + ARRAY_SIZE(a))))
#endif


//
// SHARED DIRECTORY NAME
//
// Place this directory as the first name in your directory path to access the
// OEM-specified Shared directory.
//
// Example:
// 
// Shared\foo.txt - will accesss "foo.txt" in the shared directory
//

#define AEE_SHARED_DIR     "Shared"
#define AEE_RINGER_DIR     "ringers"
#define AEE_ADDRBOOK_DIR   "addrbk"

//
//  C-Based object class definition macros.
// 

#define VTBL(iname)       iname##Vtbl

#define QINTERFACE(iname) \
struct _##iname {\
  struct VTBL(iname)  *pvt;\
};\
typedef struct VTBL(iname) VTBL(iname);\
struct VTBL(iname)

// ### Deprecated ### - Use INHERIT_IBase
#define DECLARE_IBASE(iname) \
  uint32  (*AddRef)         (iname*);\
  uint32  (*Release)        (iname*);

#define INHERIT_IBase(iname) \
  uint32  (*AddRef)         (iname*);\
  uint32  (*Release)        (iname*)

// make the name of each vt unique

#define DECLARE_VTBL(iname)      iname   vt##iname; 

// returns an 'iname'Vtbl *

#define GET_PVTBL(p,iname)       ((iname*)p)->pvt

// initialize the vtbl

#ifdef BREW_MODULE
#define INIT_VTBL(p,iname,vt) \
{ \
   static void *glueVtbl; \
   if (!glueVtbl) \
      glueVtbl=(*gftbl->gfGlueVtbl)(&vt,sizeof(vt)>>2); \
   else \
      (*gftbl->gfGlueAddRef)(glueVtbl); \
   GET_PVTBL(p,iname) = (VTBL(iname) *) glueVtbl; \
}
#define FREE_VTBL(p,iname) (*gftbl->gfGlueDelRef)(GET_PVTBL(p,iname))
#else
#define INIT_VTBL(p,iname,vt)    (GET_PVTBL(p,iname) = (VTBL(iname) *)&vt)
#define FREE_VTBL(p,iname)
#endif

#define OBJECT(n) \
  typedef struct n n;\
  struct n

// Get object ptr from interface ptr

#define ME(pme,cls,pif)          (cls * pme = ((cls*)(pif)))


/* 
   || 
   || There's a problem with QINTERFACE(): it wastes namespace and forces 
   ||   implementers to choose a name for a type that's unnecessarily 
   ||   different from the interface name, since the QINTERFACE macro 
   ||   makes an interface into a concrete type with only one member(!), 
   ||   the vtable.
   ||
   || The macros below have all the same type-safety of the QINTERFACE()
   ||   macros, but don't force a concrete type on the object, allowing
   ||   implementers of interfaces to use the same type names, thus 
   ||   obviating the need to do a type cast on entry to a method.
   || 
   || It should be easy to redefine QINTERFACE and its descendants
   ||   in terms of the below.
   || 
*/
#define AEEVTBL(iname) iname##Vtbl

#define AEEINTERFACE(iname) \
typedef struct AEEVTBL(iname) AEEVTBL(iname); \
struct AEEVTBL(iname)

#define AEEINTERFACE_(iname,ibasename) \
typedef struct AEEVTBL(iname) AEEVTBL(iname); \
struct AEEVTBL(iname) 

#define AEEGETPVTBL(p,iname)  (*((AEEVTBL(iname) **)p))

// Define an AEE interface
// (Assumes INHERIT_IClass exists for IClass)
#define AEEINTERFACE_DEFINE(iname)\
typedef struct iname iname;\
AEEINTERFACE(iname) {\
   INHERIT_##iname(iname);\
}


//
// NOTE: The following type is used for declaration of bit-fields. This
// is done so that comparisons to booleans works correctly in both Win and ARM
// environments.

typedef  unsigned int         flg;

//
// Basic Class Typedefs
//

typedef struct _IBase           IBase;
typedef struct _IQueryInterface IQueryInterface;
typedef struct _IModule         IModule;
typedef struct _IApplet         IApplet;
typedef struct _IControl        IControl;
typedef struct _IAStream        IAStream;
typedef struct _IMemAStream     IMemAStream;
typedef struct _INotifier       INotifier;
typedef struct _IShell          IShell;
typedef struct _IHeap           IHeap;
typedef struct _IFileMgr        IFileMgr;
typedef struct _IImage          IImage;
typedef struct _IImage          IViewer;

#define AEE_HI(dwParm)         ((uint16)(((uint32)(dwParm) >> 16) & 0x0000ffff))
#define AEE_LO(dwParm)         ((uint16)((uint32)(dwParm) &  0x0000ffff))

#define AEE_GET_X(dwParm)      AEE_HI(dwParm)
#define AEE_GET_Y(dwParm)      AEE_LO(dwParm)
#define AEE_SET_XY(X,Y)        (((uint32) X) << 16 | ((uint32) Y))
//
// Generic callback header.  This may be used for various interfaces where
// memory allocation for callbacks is prohibitive.
//
// pfnCancel - Filled by the handler.  It can be called at any time to cancel the callback
// pfnNotify - Filled by the caller.  It is called when the callback is issued.
// pNotifyData - Filled by the caller.  It is passed to the callback.
//

typedef struct _AEECallback AEECallback;

typedef void (*PFNCBCANCEL)(AEECallback * pcb);

struct _AEECallback
{
   AEECallback *pNext;            // RESERVED
   void        *pmc;              // RESERVED
   PFNCBCANCEL  pfnCancel;        // Filled by callback handler
   void        *pCancelData;      // Filled by callback handler
   PFNNOTIFY    pfnNotify;        // Filled by caller
   void        *pNotifyData;      // Filled by caller
   void        *pReserved;        // RESERVED - Used by handler
};


#define CALLBACK_Init(pcb,pcf,pcx) {(pcb)->pfnNotify = (PFNNOTIFY)(pcf); \
                                    (pcb)->pNotifyData = (pcx);}

#define CALLBACK_Cancel(pcb)    if (0 != (pcb)->pfnCancel) (pcb)->pfnCancel(pcb)
#define CALLBACK_IsQueued(pcb)  (0 != (pcb)->pfnCancel)


//********************************************************************************************************************************
//
// IBase Definition
//
//********************************************************************************************************************************

QINTERFACE(IBase)
{
   INHERIT_IBase(IBase);
};

#define IBASE_AddRef(p)                   GET_PVTBL(p,IBase)->AddRef(p)
#define IBASE_Release(p)                  GET_PVTBL(p,IBase)->Release(p)

typedef enum { AEE_SUSPEND_NORMAL,     // Suspended by another app starting
               AEE_SUSPEND_EXTERNAL    // Suspended by external event
             } AEESuspendReason;

typedef struct _AEESuspendInfo
{
   int      nErr;                      // App sets if there is some error
   boolean  bCloseDialogs;             // App sets to FALSE to avoid dialogs closing
} AEESuspendInfo;
   
//
// AEE Events - These events are passed to Applets and/or Controls
// 
// Note:  User/App defined events are specified as starting at EVT_USER
//

#define EVT_APP_START            0     // Main App started - dwParam = (AEEAppStart *)
#define EVT_APP_STOP             0x1   // App stopped - dwParam = (boolean *).  Set to FALSE if not closed.
#define EVT_APP_SUSPEND          0x2   // App suspended.  wParam == AEESuspendReason, dwParam = AEESuspendInfo *
#define EVT_APP_RESUME           0x3   // App resume - dwParam = (AEEAppStart *)
#define EVT_APP_CONFIG           0x4   // Alternate App Start - configuration screen should be shown...
#define EVT_APP_HIDDEN_CONFIG    0x5   // Alternate App Start - hidden configuration screen...
#define EVT_APP_BROWSE_URL       0x6   // Called after EVT_APP_START - dwParam = (const AECHAR * pURL)
#define EVT_APP_BROWSE_FILE      0x7   // Called after EVT_APP_START
#define EVT_APP_MESSAGE          0x8   // Text message - dwParam = ASCIIZ

// Key Events

#define EVT_KEY                  0x100 // App key - wShortData = KEYCODE, dwParam = Bitflags for modifier keys
#define EVT_KEY_PRESS            0x101 // App keydown - wShortData = KEYCODE, dwParam = Bitflags for modifier keys
#define EVT_KEY_RELEASE          0x102 // App keyRelease - wShortData = KEYCODE, dwParam = Bitflags for modifier keys
#define EVT_KEY_HELD             0x103 // Key held - wShortData = KEYCODE, dwParam = Bitflags for modifier keys
#define EVT_CHAR                 0x104 // Character Event wParam = AECHAR, dwParam = Bitflags for modifier keys
#define EVT_UPDATECHAR           0x105 // Char update Event wParam = AECHAR, dwParam = Bitflags for modifier keys

// Control Events...

#define EVT_COMMAND              0x200 // App custom control...
#define EVT_CTL_TAB              0x201 // App TAB event sent by controls - dwParam = control, wParam = 0-left, 1-right
#define EVT_CTL_SET_TITLE        0x202 // Message interface to set title - wParam == ID, dwParam == res file (if ID != 0) or text
#define EVT_CTL_SET_TEXT         0x203 // Message interface to set text - wParam == ID, dwParam == res file (if ID != 0) or text
#define EVT_CTL_ADD_ITEM         0x204 // Message interface to add item - dwParam == CtlAddItem
#define EVT_CTL_CHANGING         0x205 // App dwParam = CtlValChange
#define EVT_CTL_MENU_OPEN        0x206 // Sent by ITextCtl before menu activated...
#define EVT_CTL_SKMENU_PAGE_FULL 0x207 // Sent by IMenuCtl when SK menu page is full (dwParam == IMenuCtl *)
#define EVT_CTL_SEL_CHANGED      0x208 // Menu - wParam - selection ID, (dwParam == IMenuCtl *) 
#define EVT_CTL_TEXT_MODECHANGED 0x209 // Sent by ITextCtl when mode was changed - wParam = AEETextInputMode, (dwParam = ITextCtl *)

// Dialog Events...

#define EVT_DIALOG_INIT          0x300 // Dialog Event - Controls created, pre-init values, flags, etc.
#define EVT_DIALOG_START         0x301 // Dialog Event - Dialog opening - wShortData = ID, dwParam = IDialog *
#define EVT_DIALOG_END           0x302 // Dialog Event - Dialog completed normally - wShortData = ID, dwParam = IDialog *
#define EVT_COPYRIGHT_END        0x303 // Dialog Event - Copyright dialog ended

// Shell Events...

#define EVT_ALARM                0x400 // App wShortData = uCode
#define EVT_NOTIFY               0x401 // dwParam = AEENotify *
#define EVT_APP_NO_CLOSE         0x404 // App should not be closed
#define EVT_APP_NO_SLEEP         0x405 // App is working - called after long periods of non-idle app
#define EVT_MOD_LIST_CHANGED     0x406 // List of modules changed.  May be sent while app suspended!
   
#define EVT_BUSY                 (EVT_APP_NO_CLOSE)

// Device Events...
   
#define EVT_FLIP                 0x500 // wParam == TRUE if open, FALSE if closed...
#define EVT_LOCKED               0x501 // wParam == TRUE if ui is locked...
#define EVT_KEYGUARD             0x502 // wParam == TRUE if keyguard is on...

// Clipboard events...

#define EVT_CB_CUT               0x600 // Cut request - dwParam = (const char *)preferred format, NULL for all
#define EVT_CB_COPY              0x601 // Copy request - dwParam = (const char *)preferred format, NULL for all
#define EVT_CB_PASTE             0x602 // Paste request - no parameters

#define EVT_USER                 0x7000   // Start of App/User defined Events.  Private to apps...

#define AEEEvent  uint16

typedef boolean (*AEEHANDLER)(void * pData, AEEEvent evt, uint16 wParam, uint32 lParam);

//
// Note - Control COMMAND IDs above this range are reserved...
//

#define IDC_COMMAND_RESERVED  (0xff00)
  

//********************************************************************************************************************************
//
// IApplet Interface...
//
//********************************************************************************************************************************

#define INHERIT_IApplet(iname) \
   INHERIT_IBase(iname); \
   boolean  (*HandleEvent)(iname * po, AEEEvent evt, uint16 wp, uint32 dwp)

QINTERFACE(IApplet)
{
   INHERIT_IApplet(IApplet);
};


#define IAPPLET_AddRef(p)                    GET_PVTBL(p,IApplet)->AddRef(p)
#define IAPPLET_Release(p)                   GET_PVTBL(p,IApplet)->Release(p)

//********************************************************************************************************************************
//
// IApplet Interface...
//
//********************************************************************************************************************************
#define INHERIT_IModule(iname) \
   INHERIT_IBase(iname); \
   int         (*CreateInstance)(iname * po,IShell * pIShell,AEECLSID ClsId,void ** ppObj); \
   void        (*FreeResources)(iname * po, IHeap * ph, IFileMgr * pfm)
   
QINTERFACE(IModule)
{
   INHERIT_IModule(IModule);
};

#define IMODULE_AddRef(p)                    GET_PVTBL(p,IModule)->AddRef(p)
#define IMODULE_Release(p)                   GET_PVTBL(p,IModule)->Release(p)
#define IMODULE_CreateInstance(p,ps,id,ppo)  GET_PVTBL(p,IModule)->CreateInstance(p,ps,id,ppo)
#define IMODULE_FreeResources(p,ph,pfm)      GET_PVTBL(p,IModule)->FreeResources(p,ph,pfm)

//************************************************************************************************
//
// IControl Interface
//
//************************************************************************************************

//
// Standard Control Properties, so far not implemented in every control
//

#define CP_BORDER          (0x00000001l)  // Control has a border
#define CP_STATIC          (0x00000002l)  // Control is static.  SetActive does nothing...
#define CP_3D_BORDER       (0x00000004l)  // 3D Border
#define CP_ICON            (0x00000008l)  // Item is a tri-state image
#define CP_USE_DEFAULT     (0x00008000l)  // Use default properties

#define CP_RESERVED        (0x0000ffffl)

//
// AddItem structure - Used for adding items via EVT_CTL_ADD_ITEM
//

typedef struct _CtlAddItem
{
   // pText and pImage are used by default.  If they are 
   // not set (NULL), the pszResImage and pszResText will
   // be used with wText and WImage to load the text
   // and/or image respectively.

   const AECHAR * pText;         // Text 
   IImage *       pImage;        // Image 
   const char *   pszResImage;   // Resource for Image
   const char *   pszResText;    // Resource for Text
   uint16         wText;         // Text ID
   uint16         wFont;         // 0 - Default
   uint16         wImage;        // Large Image for the item
   uint16         wItemID;
   uint32         dwData;
} CtlAddItem;
  
typedef struct _CtlValChange
{
   IControl *  pc;
   uint32      dwParam;
   boolean     bValid;
} CtlValChange;

// ### Deprecated ### - Use INHERIT_IControl
#define DECLARE_ICONTROL(iname) \
   boolean     (*HandleEvent)    (iname *, AEEEvent evt, uint16 wParam, uint32 dwParam);\
   boolean     (*Redraw)         (iname *);\
   void        (*SetActive)      (iname *, boolean);\
   boolean     (*IsActive)       (iname *);\
   void        (*SetRect)        (iname *, const AEERect *);\
   void        (*GetRect)        (iname *, AEERect *); \
   void        (*SetProperties)  (iname *, uint32);\
   uint32      (*GetProperties)  (iname *); \
   void        (*Reset)          (iname *);


#define INHERIT_IControl(iname) \
   INHERIT_IBase(iname);\
   boolean     (*HandleEvent)    (iname *, AEEEvent evt, uint16 wParam, uint32 dwParam);\
   boolean     (*Redraw)         (iname *);\
   void        (*SetActive)      (iname *, boolean);\
   boolean     (*IsActive)       (iname *);\
   void        (*SetRect)        (iname *, const AEERect *);\
   void        (*GetRect)        (iname *, AEERect *); \
   void        (*SetProperties)  (iname *, uint32);\
   uint32      (*GetProperties)  (iname *); \
   void        (*Reset)          (iname *)


QINTERFACE(IControl)
{
   INHERIT_IControl(IControl);
};

#define ICONTROL_AddRef(p)                      GET_PVTBL(p,IControl)->AddRef(p)
#define ICONTROL_Release(p)                     GET_PVTBL(p,IControl)->Release(p)
#define ICONTROL_HandleEvent(p,ec,wp,dw)        GET_PVTBL(p,IControl)->HandleEvent(p,ec,wp,dw)
#define ICONTROL_Redraw(p)                      GET_PVTBL(p,IControl)->Redraw(p)
#define ICONTROL_SetActive(p,a)                 GET_PVTBL(p,IControl)->SetActive(p,a)
#define ICONTROL_IsActive(p)                    GET_PVTBL(p,IControl)->IsActive(p)
#define ICONTROL_SetRect(p,prc)                 GET_PVTBL(p,IControl)->SetRect(p,prc)
#define ICONTROL_GetRect(p,prc)                 GET_PVTBL(p,IControl)->GetRect(p,prc)
#define ICONTROL_SetProperties(p,props)         GET_PVTBL(p,IControl)->SetProperties(p,props)
#define ICONTROL_GetProperties(p)               GET_PVTBL(p,IControl)->GetProperties(p)
#define ICONTROL_Reset(p)                       GET_PVTBL(p,IControl)->Reset(p)

//***********************************************************************************************
//
//  Notifier Base Class - This base class must be used for any control created as the 
//  the result of ISHELL_RegisterNotify.  This base class allows the shell to check with the 
//  class to see if there is any pending work to do...
//
//  Wakeup and SetMask are RESERVED for system use ONLY.  They must not be called by
//  client applications!
//
//***********************************************************************************************

//
// Notifier event.  When an INotifier object issues a notification, the
// IAPPLET_HandleEvent call is made and passed a pointer to this structure
// as the dwParam.  
//

// Notify Status - Indicates action taken on notify...

typedef enum { NSTAT_IGNORED,
               NSTAT_PROCESSED,
               NSTAT_STOP} AEENotifyStatus;
typedef struct
{
   AEECLSID          cls;           // Notifier class
   INotifier *       pNotifier;     // Notifier object that issued the notify
   uint32            dwMask;        // Mask of bit(s) that occurred
   void *            pData;         // Event-specific data
   AEENotifyStatus   st;
} AEENotify;

//
// SetMask is called on any change to the Notifier masks with the new array of
// notification masks for the INotifier.  The last mask is 0.
//

#define GET_NOTIFIER_MASK(dwv)         (uint16)(dwv & 0x0000ffffl)
#define GET_NOTIFIER_VAL(dwv)          (uint16)((dwv >> 16) & 0x0000ffff)
#define NOTIFIER_VAL_ANY               (uint16)(0xffff)

// ### Deprecated ### - use INHERIT_INotifier
#define DECLARE_INOTIFIER(iname) \
   void     (*SetMask)(iname *, const uint32 * dwMasks);

#define INHERIT_INotifier(iname) \
   INHERIT_IBase(iname); \
   void     (*SetMask)(iname *, const uint32 * dwMasks)

QINTERFACE(INotifier)
{
   INHERIT_INotifier(INotifier);
};

#define INOTIFIER_AddRef(p)                     GET_PVTBL(p,INotifier)->AddRef(p)
#define INOTIFIER_Release(p)                    GET_PVTBL(p,INotifier)->Release(p)

//***********************************************************************************************
//
//  IQueryInterface Base Class - This class is exposed as a base 
//                                   extension mechanism, this should be added 
//                                   to IBase, eventually, prolly
//
//***********************************************************************************************

// ### Deprecated ### - Use INHERIT_IQueryInterface
#define DECLARE_IQIBASE(iname) \
   DECLARE_IBASE(iname)\
   int (*QueryInterface)(iname *, AEECLSID, void **)

#define INHERIT_IQueryInterface(iname) \
   INHERIT_IBase(iname); \
   int (*QueryInterface)(iname *, AEECLSID, void **)

QINTERFACE(IQueryInterface)
{
   INHERIT_IQueryInterface(IQueryInterface); 
};

#define IQI_AddRef(p)                  GET_PVTBL((p),IQueryInterface)->AddRef((p))
#define IQI_Release(p)                 GET_PVTBL((p),IQueryInterface)->Release((p))
#define IQI_QueryInterface(p,clsid,pp) GET_PVTBL((p),IQueryInterface)->QueryInterface((p),(clsid),(pp))

#define IQI_SELF(p,idReq,pp,idImp) (*(pp)=((((idReq)==(idImp))||((idReq)==AEECLSID_QUERYINTERFACE))?(p):0))


//***********************************************************************************************
//
//  IAStream Base Class - This class is exposed as a simple way to read
//  file, network and memory streams.
//
//***********************************************************************************************

// ### Deprecated ### - Use INHERIT_IAStream
#define DECLARE_IASTREAM(iname) \
   void     (*Readable)(iname * po, PFNNOTIFY pfn, void * pUser);\
   int32    (*Read)(iname * po, void * pDest, uint32 nWant);\
   void     (*Cancel)(iname * po, PFNNOTIFY pfn, void * pUser);

#define INHERIT_IAStream(iname) \
   INHERIT_IBase(iname);\
   void     (*Readable) (iname * po, PFNNOTIFY pfn, void * pUser);\
   int32    (*Read)     (iname * po, void * pDest, uint32 nWant);\
   void     (*Cancel)   (iname * po, PFNNOTIFY pfn, void * pUser)

QINTERFACE(IAStream)
{
   INHERIT_IAStream(IAStream);
};

#define IASTREAM_AddRef(p)           GET_PVTBL(p,IAStream)->AddRef(p)
#define IASTREAM_Release(p)          GET_PVTBL(p,IAStream)->Release(p)
#define IASTREAM_Readable(p,pfn,pu)  GET_PVTBL(p,IAStream)->Readable(p,pfn,pu)
#define IASTREAM_Read(p,pd,n)        GET_PVTBL(p,IAStream)->Read(p,pd,n)
#define IASTREAM_Cancel(p)           GET_PVTBL(p,IAStream)->Cancel(p,NULL,NULL)

//***********************************************************************************************
//
//  IMemAStream - This class is provided to allow the caller to use memory
//  chunks as streams.
//
//***********************************************************************************************

#define INHERIT_IMemAStream(iname) \
   INHERIT_IAStream(iname);\
   void (*Set)  (iname * po, byte * pBuff, uint32 dwSize, uint32 dwOffset, boolean bSysMem);\
   void (*SetEx)(iname * po, byte * pBuff, uint32 dwSize, uint32 nOffset, PFNNOTIFY pUserFreeFn, void *pUserFeeData)

QINTERFACE(IMemAStream)
{
   INHERIT_IMemAStream(IMemAStream);
};

#define IMEMASTREAM_AddRef(p)                        GET_PVTBL(p,IMemAStream)->AddRef(p)
#define IMEMASTREAM_Release(p)                       GET_PVTBL(p,IMemAStream)->Release(p)
#define IMEMASTREAM_Readable(p,pfn,pu)               GET_PVTBL(p,IMemAStream)->Readable(p,pfn,pu)
#define IMEMASTREAM_Read(p,pd,n)                     GET_PVTBL(p,IMemAStream)->Read(p,pd,n)
#define IMEMASTREAM_Cancel(p)                        GET_PVTBL(p,IMemAStream)->Cancel(p,NULL,NULL)
#define IMEMASTREAM_Set(p,pb,dw,o,b)                 GET_PVTBL(p,IMemAStream)->Set(p,pb,dw,o,b)
#define IMEMASTREAM_SetEx(p,pb,dw,o,puffn, puffd)    GET_PVTBL(p,IMemAStream)->SetEx(p,pb,dw,o, puffn, puffd)

#endif    // AEE_H

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 
DIRECTORY_CHAR  

Description:
This macro specifies the directory separation character. Since the character is different on Windows as well
as on handset, using this macro helps to keep the program generic. Also, please note that using '\' in the app
will still cause BREW to replace all '\' with '/' on the handset.

Definition:

#ifdef AEE_SIMULATOR
#define DIRECTORY_CHAR  '\\'
#else
#define DIRECTORY_CHAR  '/'


Members:
 
None

Comments:
None

See Also:
None

=======================================================================
DIRECTORY_STR

Description:
This macro specifies the directory separation string. Since the string is different on Windows as well
as on handset, using this macro helps to keep the program generic. Also, please note that using '\' in the app
will still cause BREW to replace all '\' with '/' on the handset.

Definition:

#ifdef AEE_SIMULATOR
#define DIRECTORY_STR   "\\"
#else
#define DIRECTORY_STR   "/"


Members:
 
None

Comments:
None

See Also:
None

=======================================================================

AEERect

Description:
AEERect is used to define a rectangle used by various Display, Graphics, Text Control,
and other helper functions. 

Definition:
typedef struct 
{
   int16 x,y;
   int16 dx, dy;
} AEERect;

Members:
 
x : The horizontal coordinate for the beginning (top left corner) of the rectangle
y : The vertical coordinate for the beginning (top left corner) of the rectangle
dx : The width of the rectangle (in pixels)
dy : The height of the rectangle (in pixels)


Comments:
None

See Also:
None

=======================================================================
TChType

Description:
TChType is an enumeration used to return the type of the wide character by the GETCHTYPE()
function.

Definition:
typedef enum 
{
   SC_UNKNOWN,
   SC_ALPHA,
   SC_DIGIT,
   SC_WHITESPACE
} TChType;

Members:
 
SC_UNKNOWN : Unknown type
SC_ALPHA : Alphabet type
SC_DIGIT : Numeric type (0-9)
SC_WHITESPACE : White Space


Comments:
None

See Also:
None 


=======================================================================
AEECallback

Description:
   
    This structure specifies the data and functions for a callback 
    registered with the ISHELL_Resume() and other functions.

Definition:
   typedef struct _AEECallback AEECallback; 
   struct _AEECallback
   {
      AEECallback *pNext;
      void        *pmc;
      PFNCBCANCEL  pfnCancel;
      void        *pCancelData;
      PFNNOTIFY    pfnNotify;
      void        *pNotifyData;
      void        *pReserved;
   };

Members:
 
  pNext:  Reserved and the caller should not modify this member 

  pmc: Reserved and the caller should not modify this member

  pfnCancel: Pointer to function called by the callback handler if this 
             callback is cancelled. The caller should set this pointer to NULL.

  pCancelData: Data passed to pfnCancel. The caller should not modify 
                this member.

  pfnNotify: This is the callback function that is invoked by AEE. The 
           caller must set this pointer to the function to be called by the 
           AEE callback handler.

  pNotifyData: Data to be passed to pfnNotify, the caller must set this 
          pointer to the data that must be passed to the pfnNotify function.

  pReserved: Reserved and this member will be used by the callback handler


Comments:
   None

See Also:
   None 

=======================================================================

Macro: CALLBACK_Init

Description: 
  Set up internal members of an AEECallback for scheduling, sets the 
  notify function and the notify context

Definition:

   CALLBACK_Init(pcb,pcf,pcx) {(pcb)->pfnNotify = (pcf); \
                               (pcb)->pNotifyData = (pcx);}

Parameters:
  AEECallback *pcb: pointer to the AEECallback to set up

  PFNNOTIFY pcf: notify function

  void *pcx: notify context

Evaluation Value:
   None

Comments:  
   pcf(pcx) is called when the AEECallback comes due (either via Resume
   or other scheduling operation)

Side Effects: 
   None

See Also:
   None

======================================================================= 

Macro: CALLBACK_Cancel

Description: 
  fire an AEECallback's cancel function, if any.

Definition:

   CALLBACK_Cancel(pcb)    if (0 != (pcb)->pfnCancel) (pcb)->pfnCancel(pcb)

Parameters:
  AEECallback *pcb: pointer to the AEECallback to set up

Evaluation Value:
   None

Comments:  
   Setting up the cancel function is the responsibility of the called
   scheduling API.  E.g.: the ISHELL_Resume cancel function is likely 
   something that removes the callback from a list of pending notifies

   Similarly, clearing the cancel function during a cancel callback or
   before calling the notify function is *also* the responsibility of
   the called scheduling API. E.g.: before ISHELL_Resume fires the 
   notify function, the AEECallback is taken out of a list of pending
   notifies, and so the cancel function in the callback must be set to
   0.

Side Effects: 
   None

See Also:
   None

======================================================================= 

Macro: CALLBACK_IsQueued

Description: 
   deduce whether an AEECallback is pending, scheduled

Definition:

   CALLBACK_IsQueued(pcb) (0 != (pcb)->pfnCancel)


Parameters:
  AEECallback *pcb: pointer to the AEECallback to set up

Evaluation Value:
   boolean TRUE if the AEECallback has a cancel function, is on
          somebody's scheduling, pending list

Comments:
   This is not an exhaustive test and is only a convenience for called 
   scheduling APIs that use the cancel function (should be all of them, 
   but callers must not assume so).

Side Effects: 
   None

See Also:
   None

=======================================================================
AEEImageInfo

Description:
This structure gets the information about an image

Definition:
typedef struct _AEEImageInfo
{
   uint16 cx;
   uint16 cy;
   uint16 nColors;
   boolean bAnimated;
   uint16 cxFrame;
} AEEImageInfo;

Members:
 
cx : The width of the image (in pixels)
cy : The height of the image (in pixels)
nColors : The number of colors in the image
bAnimated : TRUE, if the image contains animation
cxFrame : If the image is divided into frames, this member indicates the width of
each frame


Comments:
None

See Also:
None 


=======================================================================
AEE Events

Description:
These are the defined AEE events that can be received by an applet and/or control.
For each event below the wParam and dwParam parameters, if any, that are passed
to the applet or control are given.

Definition:
 
Applet Events 
 
EVT_APP_START:             Main App started
EVT_APP_STOP:              App stopped: no parameters
EVT_APP_SUSPEND:           App suspended: no parameters
EVT_APP_RESUME:            App resumed: dwParam = ( AEEAppStart * )
EVT_APP_CONFIG:            Alternate App Start: configuration screen should be shown
EVT_APP_HIDDEN_CONFIG:     Alternate App Start: hidden configuration screen
EVT_APP_BROWSE_URL:        Called after EVT_APP_START: dwParam = (const AECHAR * pURL)
EVT_APP_BROWSE_FILE:       Called after EVT_APP_START
EVT_APP_MESSAGE:           Text message: dwParam = ASCIIZ
 
Key Events 
 
EVT_KEY:                   App keyup: wParam = KEYCODE, dwParam = Bitflags for modifier keys
EVT_KEY_PRESS:             App keydown: wParam = KEYCODE, dwParam = Bitflags for modifier keys
EVT_KEY_RELEASE:           App keyheld: wParam = KEYCODE, dwParam = Bitflags for modifier keys
EVT_KEY_HELD:              Key held: wParam = KEYCODE, dwParam = Bitflags for modifier keys
EVT_CHAR                   Character Event: wParam = AECHAR, dwParam = Bitflags for modifier keys
EVT_UPDATECHAR             Character update Event: wParam = AECHAR, dwParam = Bitflags for modifier keys

Control Events 
 
EVT_COMMAND:               App custom control
EVT_CTL_TAB:               App TAB event sent by controls: dwParam = control, wParam = 0-left, 
                           1-right
EVT_CTL_SET_TITLE:         Message interface to set title: wParam = ID, dwParam = res file
                           (if ID != 0) or text
EVT_CTL_SET_TEXT:          Message interface to set text: wParam = ID, dwParam = res file (if ID 
                           != 0) or text
EVT_CTL_ADD_ITEM:          Message interface to add item: dwParam = CtlAddItem
EVT_CTL_CHANGING:          App dwParam = CtlValChange
EVT_CTL_CHANGING:          App dwParam = CtlValChange
EVT_CTL_MENU_OPEN:         Sent by text controls before their associated softkey menus are activated
EVT_CTL_SKMENU_PAGE_FULL:  Sent by IMenuCtl when SK menu page is full
EVT_CTL_SEL_CHANGED:       Sent by IMenuCtl when selection has changed
EVT_CTL_TEXT_MODECHANGED:  Sent by ITextCtl when input mode was changed

Dialog Events 
 
EVT_DIALOG_INIT:           Dialog Event: Controls created, pre-init values, flags, and other items
EVT_DIALOG_START:          Dialog Event: Dialog opening, wParam = ID, dwParam = IDialog * 
EVT_DIALOG_END:            Dialog Event: Dialog completed normally, wParam = ID, dwParam = IDialog * 
EVT_COPYRIGHT_END:         Dialog Event: Copyright dialog ended
 
AEE Shell Events 
 
EVT_ALARM:                 App wParam = uCode
EVT_NOTIFY:                dwParam = AEENotify * 
EVT_BUSY:                  Sent to app to determine if app can be suspended or stopped
 
Device Events 

EVT_FLIP:                  wParam = TRUE if open, FALSE if closed
EVT_LOCKED:                wParam = TRUE if user interface is locked
EVT_KEYGUARD:              wParam = TRUE if keyguard is on

User defined events 

EVT_USER:                  Start of App/User defined Events (Private to apps)

Members:
None: 

Comments
The user defined events start from EVT_USER.

See Also:
None 

=======================================================================
AEE Standard Control Properties

Description:
These are the defined Standard Control Properties.

Definition:
 
CP_BORDER:        Control has a border
CP_STATIC:        Control is static and SetActive has no effect with this control type
CP_3D_BORDER:     3D Border
CP_USE_DEFAULT:   Use default properties

Members:
None

Comments:
None

See Also:
None

=======================================================================
AEENotify

Description:
A pointer to this structure is passed as dwParam when EVT_NOTIFY event is sent to
an app. An app receives this event as part of the notification(s) that it has registered
for. 

Definition:
typedef struct
{
AEECLSID cls;
INotifier * pNotifier;
uint32 dwMask;
void * pData; 
AEENotifyStatus st;
} AEENotify;

Members:
 
cls: Notifier Class
pNotifier: Notifier Object that issued the Notify
dwMask: Mask of bits that occurred
pData: Notification-specific data
st: Indicates to IShell if the app processed the notificaiot


Comments:
None

See Also:
ISHELL_RegisterNotify() 
ISHELL_Notify() 
AEENotifyStatus 

=============================================================================

AEENotifyStatus

Description:
This enumerated type defines the notification status values that are returned to
the shell by an applet that receives a notification. The applet returns the status
of its processing of the notification by setting the st member of the AEENotify
structure it is passed along with the EVT_NOTIFY event.

Definition:
typedef enum 
{
NSTAT_PROCESSED,
NSTAT_IGNORED,
NSTAT_STOP 
} AEENotifyStatus;

Members:
 
NSTAT_PROCESSED: The applet successfully processed the notification
NSTAT_IGNORED: The applet ignored the notification
NSTAT_STOP: The applet processed the notification, and the notification should not
be sent to any other applets that have registered to be notified of this event.


Comments:
None.

See Also:
ISHELL_RegisterNotify() 
ISHELL_Notify() 
AEENotify 

======================================================================= 
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IBase

Description: 
   IBase is the base level object class from which all API object classes are
derived.  It supplies the object reference counting mechanisms that allow objects 
to manage their own memory instances.  

======================================================================= 

Function: IBASE_AddRef()

Description: 
   This method increments the reference count on an object.  This allows the object
to be shared by multiple callers.  The object is freed when the reference count 
reaches 0.

Prototype:

   uint32 IBASE_AddRef(IBase * po);

Parameters:
   po: Pointer to the object

Return Value:

   Returns the incremented reference count for the object.  A valid object returns a positive
   reference count.

Comments:  
   None

Side Effects: 
   None

See Also:
   None
======================================================================= 
Function: IBASE_Release

Description: 
   This function decrements the reference count of an object.  The object is 
freed from memory and is no longer valid once it's reference count reaches 0. 

Prototype:

   uint32 IBASE_Release(IBase * po);

Parameters:

   po: Pointer to the object

Return Value: 
   
   Returns the decremented reference count for the object.  The object has been freed and
   is no longer valid if 0 is returned.  

Comments: 
   None

Side Effects: 
   None

See Also: 
   None

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IQueryInterface

Description: 
   IQueryInterface is the base level object class from which extensible
   API object classes should be derived.  It supplies a standard 
   mechanism to accomplish object extensibility while maintaining 
   binary compatibility

======================================================================= 

Function: IQI_QueryInterface()

Description: 
   This method asks an object for another API contract from the object in 
   question.

Prototype:

   int IQI_QueryInterface(IQueryInterface *po, AEECLSID idReq, void **ppo)


Parameters:
   po: Pointer to the object
   idReq:  Requested class ID exposed by the object
   ppo: (out) Returned object.  Filled by this method

Return Value:
   SUCCESS - Interface found
   ENOMEMORY - Insufficient memory
   ECLASSNOTSUPPORT - Requested interface is unsupported

Comments:  
   The pointer in *ppo is set to the new interface (with refcount accordingly
   positive), or NULL if the class ID is not supported by the object.

   The macro IQI_SELF() can be used as the body of the QueryInterface() 
     method in any QueryInterface descendant that implements only one 
     interface 

   Use it like so:
  
      int ISomething_QueryInterface(ISomething *me, AEECLSID idReq, void **ppo)
      {
         if (NULL != IQI_SELF(me,idReq,ppo,AEECLSID_SOMETHING)) {
            IQIBASE_AddRef(*ppo);
            return SUCCESS;
         } else {
            return ECLASSNOTSUPPORT;
         }
      }

Side Effects: 
   None

See Also:
   None

=======================================================================

Macro: IQI_SELF

Description: 
   helper macro for those implementing an object with a single interface

Definition:
   IQI_SELF(p,idReq,pp,idImp) (*(pp)=((((idReq)==(idImp))||((idReq)==AEECLSID_QUERYINTERFACE))?(p):0))

Parameters:
  p: the object 

  idReq: the clsid passed to QueryInterface, the requested clsid

  pp: output interface pointer

  idImpl: the clsid of the interface (other than IQueryInterface) that 
           the object implements

Evaluation Value:
   p if the object is either idReq or idImpl

Comments:  
   Use it like so:
   
   int ISomething_QueryInterface(ISomething *me, AEECLSID idReq, void **ppo)
   {
      if (NULL != IQI_SELF(me,idReq,ppo,AEECLSID_SOMETHING)) {
         IQIBASE_AddRef(*ppo);
         return SUCCESS;
      } else {
         return ECLASSNOTSUPPORT;
      }
   }


Side Effects: 
   ppo is set to p or NULL

See Also:
   None

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IApplet

Description: 
   IApplet is the class that represents an AEE "Applet".  The class is derived 
from IBase and is generated by the associated IModule when requested via 
IBASE_CreateInstance.  The IApplet interface exposes a very simple HandleEvent 
routine.  This routine is called by the AEE Shell in response to events generated 
by the system or by other components or applets.

Please note that IAPPLET_HandleEvent should only be called by the AEE Shell.
Events sent by other applets or components should be sent via the 
ISHELL_SendEvent method.

======================================================================= 

Function: IAPPLET_HandleEvent()

Description: 
   This method provides the main event processing for an AEE applet.  It is called 
when any event is passed to the applet.  Events can include system-level notifications, 
keypress events, etc.  Note that this mechanism is also used to support applet 
startup/shutdown/susped/resume.  The HandleEvent method is also called in response 
to system alarms or notifications.  In that case, if the applet is not currently 
running, the applet will be loaded and the event will be sent to the applet. In such 
cases, the EVT_APP_START event will not be sent to the applet.
This function must be implemented by all applets.

Prototype:

   boolean IAPPLET_HandleEvent(IApplet * po, AEEEvent evt, uint16 wp, uint32 dwp);

Parameters:
   po: Pointer to the object
   evt: Event code
   wp: 16-bit context parameter
   dwp: 32-bit context parameter

Return Value:

   Returns TRUE if the event was handled by the applet.  

Comments:  
   This method should only be called by the AEE Shell

Side Effects: 
   None

See Also:  
   ISHELL_SendEvent

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IModule

Description: 
   The IModule interface provides a mechanism for controlling access to a 
grouping of associated applets or components.  The module provides a single
point of entry for the AEE Shell to request classes owned by the module.  In 
most cases, a module exposes a single applet or shared-component class.  However,
the use of the IModule interface allows for modules to expose a wide variety of 
classes without fixed entry-points to expose each such class.  Further, the 
module can serve as a base object for any associated objects.  This may allow 
them to share memory, etc. via the private implementation of the Module class.

======================================================================= 

Function: IMODULE_CreateInstance()

Description: 
   The CreateInstance class is the single most important interface exposed by the 
module to the AEE.  It provides the mechanism for the AEE to request classes on an 
as-needed basis from the module.  Upon successful creation of the requested object 
class, the module should return the object with a positive reference count.  Also 
note that the requested class must be implemented in accordance with the class 
definition specified for the class.

Prototype:

   int   IMODULE_CreateInstance(IModule * po,IShell * pIShell,AEECLSID ClsId,void ** ppObj);


Parameters:
   po: Pointer to the object
   pIShell: Pointer to the AEE Shell
   ClsId:  Requested class ID exposed by the module
   ppObj:  Returned object.  Filled by the CreateInstance method

Return Value:

   SUCCESS - Object class was created
   ENOMEMORY - Insufficient memory
   ECLASSNOTSUPPORT - Requested class is unsupported

Comments:  
   Object must be returned with a positive reference count

Side Effects: 
   None

See Also:
   None

======================================================================= 
Function: IMODULE_FreeResources

Description: 
   This method is called by the Shell when the OEM or Shell detects a low 
memory or storage condition.  This can include low RAM or Flash/File storage.  The module is passed a pointer to IHeap and/or IFileMgr
depending upon the specific condition involved.  The module should release
any unused RAM/File storage under this condition.  

Prototype:

   void IMODULE_FreeResources(IModule * po, IHeap * ph, IFileMgr * pfm);

Parameters:

   po: Pointer to the object
   ph: Pointer to IHeap
   pfm: Pointer to IFileMgr

Return Value: 
   None

Comments: 
   None

Side Effects: 
   None

See Also: 
   None

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IControl

Description: 
   The AEE supports the concept of user interface objects or Controls.  This 
mechanism is supported by instantiating a class for the desired control object 
and calling both standard and (in some cases) specific methods for the class.  
In general, the use of an AEE control works in the following manner.

- Control is created
- Control is positioned (if necessary)
- Control is initialized with data (text, menu items, etc.)
- Control is sent events via HandleEvent
- Control is Released when no longer needed

Although controls can be created directly by the application, they can
also be created by placing them in a Dialog.  In that case, the Dialog manager
automatically creates, manages (passes events) and deletes the control.  The
application is called at various stages in creation and when the control
is active to allow it to update, query or change the control.

IControl is the base class object from which all UI controls
are derived.  The interface provides common methods exposed by all 
UI controls.  This allows the Shell to manipulate controls in a generic
manner in support of such features as IDialog.

======================================================================= 

Function: ICONTROL_HandleEvent()

Description: 
      This method provides the main event processing for a control.  It is called 
when any event is passed to the control. Events mainly include keypress events.
This function must be implemented by all controls.

Prototype:

   boolean ICONTROL_HandleEvent(IControl * po, AEEEvent evt, uint16 wp, uint32 dwp);

Parameters:
   po: Pointer to the control object
   evt: Event code
   wp: 16-bit event data
   dwp: 32-bit event data

Return Value:

   Returns TRUE if the event was processed by the control. FALSE otherwise.

Comments:
   None
   
Side Effects: 
   None

See Also:
   None
======================================================================= 

Function: ICONTROL_Redraw()

Description: 
   This method instructs the control to redraw it's contents.Under normal 
conditions, UI controls do not redraw their contents when the underlying data 
behind the control changes.  This allows several data updates to occur while 
minimizing screen flashes.  For example, several items can be added to a menu 
with no visible effect until the Redraw method is called.

Prototype:

   boolean ICONTROL_Redraw(IControl * po);

Parameters:
   po: Pointer to the control object

Return Value:

   Returns TRUE if the control was redrawn. FALSE otherwise.

Comments:
   None
   
Side Effects: 
   None

See Also:
   None
======================================================================= 

Function: ICONTROL_SetActive()

Description: 
   This method instructs the control to enter/leave focus or selected mode.  
The concept of focus is left somewhat to the control.  In the case of menus, 
focus indicates that the menu is active.  In terms of text controls it means 
the control is active and in edit mode.  This call usually results in the 
underlying control redrawing it's contents.

It is important to note that controls will still have their HandleEvent
method called even when they are inactive.  This allows them to process 
special events such as scrolling multi-line text controls.

Prototype:

   void ICONTROL_SetActive(IControl * po,boolean bActive);

Parameters:

po: Pointer to the control object
bActive: Specifies whether to activate(TRUE) or deactivate (FALSE) the control.

Return Value: 
   None

Comments: 
   None

Side Effects: 
   None

See Also: 
   None

======================================================================= 

Function: ICONTROL_IsActive()

Description: 
   This method returns the active or focus state of the control.

Prototype:

   boolean ICONTROL_IsActive(IControl * po);

Parameters:

po: Pointer to the control object

Return Value: 
   TRUE if the control is active.  FALSE otherwise.

Comments: 
   None

Side Effects: 
   None

See Also: 
   None
======================================================================= 

Function: ICONTROL_SetRect()

Description: 
   This method sets the active screen coordinates of the control. This may 
result in the control redrawing it's contents.

Prototype:

   void ICONTROL_SetRect(IControl * po, const AEERect * prc);

Parameters:

po: Pointer to the control object
prc: The bounding rectangle for the control

Return Value: 
   None

Comments: 
   None

Side Effects: 
   May redraw the control

See Also: 
   ICONTROL_GetRect
======================================================================= 

Function: ICONTROL_GetRect()

Description: 
   This method fills a pointer to an input RECT with the active screen 
coordinates for the control.  This is particularly useful after a control 
is created to determine it's optimal/default size and position.

Prototype:

   void ICONTROL_GetRect(IControl * po, AEERect * prc);

Parameters:

po: Pointer to the control object
prc: The rectangle to be filled with the coordinates of the control

Return Value: 
   None

Comments: 
   None

Side Effects: 
   None

See Also: 
   ICONTROL_SetRect
======================================================================= 

Function: ICONTROL_SetProperties()

Description: 
   This method sets control specific properties or flags.  Although some 
properties are defined across controls (CP_FRAME, etc.), most controls
expose a range of properties to allow more specific control over the object.

Prototype:

   void ICONTROL_SetProperties(IControl * po, uint32 dwProps);

Parameters:

po: Pointer to the control object
dwProps: 32-bit set of flags/properties

Return Value: 
   None

Comments: 
   None

Side Effects: 
   May redraw the control

See Also: 
   ICONTROL_GetProperties

======================================================================= 

Function: ICONTROL_GetProperties()

Description: 
   This method returns the control specific properties or flags.

Prototype:

   uint32 ICONTROL_GetProperties(IControl * po);

Parameters:
   po: Pointer to the control object

Return Value: 
   32-bit properties for the control

Comments: 
   None

Side Effects: 
   None

See Also: 
   ICONTROL_SetProperties

======================================================================= 

Function: ICONTROL_Reset()

Description: 
   This method instructs the control to reset (free/delete) it's
contents as well as to immediately leave active/focus mode.  This is 
useful in freeing all underlying memory in text or menu controls or 
removing all menu items in a single call.

Prototype:

   void ICONTROL_Reset(IControl * po);

Parameters:
   po: Pointer to the control object

Return Value: 
   None

Comments: 
   None

Side Effects: 
   None

See Also: 
   ICONTROL_SetActive

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: INotifier

Description: 
   The AEE supports the concept of notifications. Notification is a mechanism where-in
   different classes can register for events when certain incidents occur in other classes. 
   For example, an applet can regsiter to be notified when say, there is an incoming SMS 
   message.	This means, whenever there is an incoming SMS message, the applet that has 
   registered to be notified will receive an event. This event is EVT_NOTIFY.
	
   Notifiers are the classes that generate notifications. For ex: the TAPI class acts as a
   notifier and it sends out notifications when there is an incoming SMS message.
   Notifiers are classes that want to send out notifications when certain incidents
   occur. Whenever a notifier needs to send out notifications, it uses the ISHELL_Notify() 
   method. 
	
   Applets can register for notifications i.e. they can register to be notified whenever
   certain incidents occur in the notifiers. To do this, the function ISHELL_RegisterNotify()
   must be used. For example, an applet can use this mechanism to register for 
   notification from the AEECLSID_TAPI interface for events specific to that class.  
	
   INotifier is the interface that specifies the functions that must be implemented by
   any class that needs to be a notifier. Any class that wants to send out
   notifications so that other applets can receive it must implement the functions
   in the INotifier interface.

   Here is a broad description of the steps that must be followed by a class
   if it needs to be a notifier:

   1. The class must implement the INotifier interface. 
   2. Define the set of notifications (or masks) that the class can issue. For ex: some
   of the notifications that the the TAPI class can issue are: NMASK_TAPI_SMS, 
   NMASK_TAPI_TS_PAGE and NMASK_TAPI_TS_VMAIL. These masks must be made available to 
   other applets so the applets that are interested in these notifications can register 
   for them.
   3. Whenever the notifier class wants to issue a notification, it must invoke
   the ISHELL_Notify() function. The shell will then take care of informing all the
   applets that have registered for this notification.

   
   In addition to the standard IBase methods (AddRef() and Release()), the INotifier interface
   has the following additional method(s):

======================================================================= 

Function: INOTIFIER_SetMask()

Description: 
   This method is invoked by the Shell to inform the notifier class of
   all the notifications (issued by that class) that other applets are
   interested in. 
   This function must be implemented by all notifiers (i.e. by all classes that 
   implement the INotifier interface). This function is strictly meant to be
   invoked internally by the Shell. It must never be directly invoked by other 
   applets or classes.


Prototype:

   void	INOTIFIER_SetMask(INotifier * po, const uint32 * dwMasks);

Parameters:
   po: Pointer to the INotifier interface
	
   dwMasks: Specifies the array of masks representing the notifications that 
   other applets are interested in. 
   A class may be capable	of emitting multiple notifications  (say, 
   NMASK_TAPI_SMS, NMASK_TAPI_TS_PAGE and NMASK_TAPI_TS_VMAIL).However, the applets 
   that have registered for notifications may only be interested in the 
   NMASK_TAPI_SMS and NMASK_TAPI_TS_PAGE notifications. Whenever an application registers
   for a notification using ISHELL_RegisterNotify(), it must specify the mask for
   the notification that it is interested in. The shell then invokes this function
   INOTIFIER_SetMask() on the notifier to inform the notifier of all the notifications
   that the applets are interested in.


Return Value:
   None

Comments:
   None
   
Side Effects: 
   None

See Also:
   ISHELL_RegisterNotify, ISHELL_Notify

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IAStream

Description: 
   The AEE supports the concept of streams. The user can stream data
   from a file or the network using the IAStream interface. 
   An IAStream interface object can be created by typecasting an 
   existing IFile or ISocket object For e.g.
      IAStream * pIStream = (IAStream *) pIFile; //pIFile is an existing IFile 
                                                 //interface object


   In addition to the standard IBase methods (AddRef() and Release()), 
   the IAStream interface has the following additional method(s):

======================================================================= 

Function: IASTREAM_Readable()

Description: 
   This method allows the user to specify a function pfn which to be used
   by the IAStream object to notify the user when the stream has data 
   available to be read.


Prototype:
   void   IASTREAM_Readable(IAStream *pIAStream, PFNNOTIFY pfn, void * pUser)

Parameters:
   pIAStream: Pointer to the IAStream interface
   pfn      : Pointer to user function that must be called by the IAStream object
              when data is available for reading.
   pUser    : Pointer to user data that must be used as parameter when calling pfn.
	

Return Value:
   None

Comments:
   None
   
Side Effects: 
   None

See Also:
   None

======================================================================= 

Function: IASTREAM_Read()

Description: 
   This method is used to read the nBytes number of bytes from the
   stream pIAStream into the destination buffer pDest. If fewer than
   nBytes are available in the stream, the method returns the actual
   number of bytes read.


Prototype:
   int32    IASTREAM_Read(IAStream *pIAStream, void * pDest, uint32 nBytes)


Parameters:
   pIAStream: Pointer to the IAStream interface
   pDest    : Pointer to buffer into which the data should be read.
   nBytes   : Number of bytes to read from the stream.
	

Return Value:
   Number of bytes read.

Comments:
   None
   
Side Effects: 
   None

See Also:
   None

======================================================================= 

Function: IASTREAM_Cancel()

Description: 
   This method allows the user to cancel a notification function previously
   registered using IASTREAM_Readable.


Prototype:
   void   IASTREAM_Cancel(IAStream *pIAStream, PFNNOTIFY pfn, void * pUser)

Parameters:
   pIAStream: Pointer to the IAStream interface
   pfn      : Pointer to user function that was set as the notification function
              in IASTREAM_Readable.
   pUser    : Pointer to user data that was provided to be used as parameter when 
              calling pfn.
	

Return Value:
   None.

Comments:
   None
   
Side Effects: 
   None

See Also:
   IASTREAM_Readable

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IMemAStream

Description: 
   The IMemAStream interface extends the IAStream interface to allow
   a specified memory chunk to be read as a stream. An instance of the
   IMemAStream can be created using the ISHELL_CreateInstance method
   (with class ID AEECLSID_MEMASTREAM). The specified memory chunk is 
   freed when the IMemAStream object is released.

   In addition to the standard IBase methods (AddRef() and Release()),
   and the standard IAStream methods (Readable(), Read(), and Cancel()),
   the IMemAStream interface has the following additional method(s):

======================================================================= 

Function: IMEMASTREAM_AddRef()

Description: 
   This method increments the reference count on the IMemAStream object.  
   This allows the object to be shared by multiple callers.  
   The object is freed when the reference count reaches 0.

Prototype:

	uint32 IIMEMASTREAM_AddRef(IMemAStream * po);


Parameters:
   po: Pointer to the IMemAStream object

Return Value:

   Returns the incremented reference count for the object.  
   A valid object returns a positive  reference count.

Comments:  
   None

Side Effects: 
   None

See Also:
   None
======================================================================= 
Function: IMEMASTREAM_Release

Description: 
   This function decrements the reference count of the IMemAStream object.  The object is 
freed from memory and is no longer valid once it's reference count reaches 0. 
When the reference count reaches zero, the memory buffer (that has been previously set 
using the call to IMEMASTREAM_Set()) is also freed.

Prototype:

   uint32 IMEMASTREAM_Release(IMemAStream * po);

Parameters:

   po: Pointer to the IMemAStream object

Return Value: 
   
   Returns the decremented reference count for the object.  The object has been freed and
   is no longer valid if 0 is returned.  

Comments: 
   None

Side Effects: 
   None

See Also: 
   None

=======================================================================
Function: IMEMASTREAM_Readable()

Description: 
   This method allows the user to specify a function pfn which to be used
   by the IMemAStream object to notify the user when the stream has data 
   available to be read. For the MemStream object, the callback will be called
   rightaway since the data is to be read from a memory buffer invoked using
   the Set() function.


Prototype:
   void   IMEMASTREAM_Readable(IMemAStream *pIStream, PFNNOTIFY pfn, void * pUser)

Parameters:
   pIStream : Pointer to the IMemAStream interface
   pfn      : Pointer to user function that must be called by the IMemAStream object
              when data is available for reading.
   pUser    : Pointer to user data that must be used as parameter when calling pfn.
	

Return Value:
   None

Comments:
   None
   
Side Effects: 
   None

See Also:
   None

======================================================================= 

Function: IMEMASTREAM_Read()

Description: 
   This method is used to read the nBytes number of bytes from the
   stream pIMemAStream into the destination buffer pDest. If fewer than
   nBytes are available in the stream, the method returns the actual
   number of bytes read.
   This function will attempt to read data from the buffer that has been set
   into the IMemAStream object using IMEMASTREAM_Set().Hence, prior to calling this function, 
   the function IMEMASTREAM_Set() must have been invoked to set a valid memory buffer into 
   the IMemAStream object.
   
Prototype:
   int32    IMEMASTREAM_Read(IMemAStream *pIStream, void * pDest, uint32 nBytes)


Parameters:
   pIStream : Pointer to the IMemAStream interface
   pDest    : Pointer to buffer into which the data should be read.
   nBytes   : Number of bytes to read from the stream. 
	

Return Value:
   Number of bytes read. If nBytes is greater than the
   size of the data available in the buffer (set using IMEMASTREAM_Set()), then only the
   available data is read and this size is returned.

Comments:
   None
   
Side Effects: 
   None

See Also:
   None

======================================================================= 

Function: IMEMASTREAM_Cancel()

Description: 
   This method allows the user to cancel a notification function previously
   registered using IMEMASTREAM_Readable.


Prototype:
   void   IMEMASTREAM_Cancel(IMemAStream *pIStream, PFNNOTIFY pfn, void * pUser)

Parameters:
   pIStream : Pointer to the IMemAStream interface
   pfn      : Pointer to user function that was set as the notification function
              in IMEMASTREAM_Readable.
   pUser    : Pointer to user data that was provided to be used as parameter when 
              calling pfn.
	

Return Value:
   None.

Comments:
   None
   
Side Effects: 
   None

See Also:
   IMEMASTREAM_Readable
======================================================================= 

Function: IMEMASTREAM_Set()

Description: 
   This method allows the user to set the memory chunk that needs to be
   read as a stream. An instance of the IMemAStream object must already
   exist. The responsibility of freeing the buffer pBuff lies with the
   IMEMASTREAM interface. User must not free this buffer.
   This buffer will be freed when either of the following two actions happen: 
   1) When the MemSTream object is released using IMEMASTREAM_Release() 
   2)When a subsequent call to IMEMASTREAM_Set is done with another buffer. 
   If a buffer has already been set into the stream (using a previous call to
   this function), that buffer is freed'd before setting the new buffer.
   Hence, it is dangerous to do two consecutive calls to IMEMASTREAM_Set()
   with the same buffer.

Prototype:
   void     IMEMASTREAM_Set( 
                             IMemAStream * pIMemAStream, 
                             byte * pBuff, 
                             uint32 dwSize, 
                             uint32 dwOffset, 
                             boolean bSysMem
                           )

Parameters:
   pIMemAStream: Pointer to the IMemAStream interface
   pBuff       : Pointer to memory chunk that needs to be read as a stream.
   dwSize      : Size of the memory chunk
   dwOffset    : Offset from the beginning of the memory chunk to be set as the start
                 data for the memory stream
   bSysMem     : Flag to specify if the memory for the buffer pBuff belongs to
   the user area or the system memory. This flag is used to decide whether the routines 
   FREE() or SYSFREE() must be used by the IMemAStream object to free the buffer when this 
   object is released or when another Set() is  made. For ex: if user performs a MALLOC()
   to allocate the buffer, then bSysMem must be set to FALSE.
	

Return Value:
   None

Comments:
   None
   
Side Effects: 
   None

See Also:
   None

======================================================================= 
Function: IMEMASTREAM_SetEx()

Description: 
   This method behaves the same as IMEMASTREAM_Set, with the additional
   feature of a User callback to free the memory chunk. The user can
   specify a callback function pUserFreeFn which is called when the 
   memory chunk pBuff needs to be freed.

Prototype:
   void     IMEMASTREAM_SetEx( 
                             IMemAStream * pIMemAStream, 
                             byte *        pBuff, 
                             uint32        dwSize, 
                             uint32        dwOffset, 
                             PFNNOTIFY     pUserFreeFn,
                             void *        pUserFreeData
                           )

Parameters:
   pIMemAStream  : Pointer to the IMemAStream interface
   pBuff         : Pointer to memory chunk that needs to be read as a stream.
   dwSize        : Size of the memory chunk
   dwOffset      : Offset from the beginning of the memory chunk to be set as the start
                   data for the memory stream
   pUserFreeFn   : Callback function to be called when the memory chunk needs to be freed, (may be NULL, if the memory needn't be freed)
   pUserFreeData : Parameter to be passed to pUserFreeFn

Return Value:
   None

Comments:
   None
   
Side Effects: 
   IMEMASTREAM_Set

See Also:
   None

=======================================================================
Function: AEE_HI()         

Description: 
   This is a macro that can be used to extract the upper 16 bits of a
   given 32-bit value

Prototype:

uint16 AEE_HI(uint32 dwParm)         


Parameters:
   dwParm: 32-bit value whose upper 16 bits needs to be extracted

Return Value:
   Upper 16 bits of the given 32-bit value

Comments:
   None
   
Side Effects: 
   None

See Also:
   AEE_LO

======================================================================= 
Function: AEE_LO()         

Description: 
   This is a macro that can be used to extract the lower 16 bits of a
   given 32-bit value

Prototype:

uint16 AEE_LO(uint32 dwParm)         


Parameters:
   dwParm: 32-bit value whose lower 16 bits needs to be extracted

Return Value:
   lower 16 bits of the given 32-bit value

Comments:
   None
   
Side Effects: 
   None

See Also:
   AEE_HI

======================================================================= 
Function: AEE_GET_X()         

Description: 
   This is a macro that can be used to extract the X-Coordinate value from
   the given 32-bit dwParam. This is used in conjunction with the AVK_TAP key code.
   When a TAP click occurs, the app receives the event EVT_KEY with
   wParam set to AVK_TAP and dwParam containing the X and Y coordinates of the location
   of the tap click. The upper 16 bits of the dwParam contains the X coordinate while
   the lower 16 bits contains the Y coordinate of the click. This macro (AEE_GET_X()) can be
   used to get the X coordinate of the tap click.
   In the SDK, the same event is received when the mouse is single-clicked inside the
   screen portion of the device being emulated.

Prototype:

uint16 AEE_GET_X(uint32 dwParm)         


Parameters:
   dwParm: 32-bit value containing the X and Y coordinate of the click

Return Value:
   X coodrinate of the TAP click

Comments:
   None
   
Side Effects: 
   None

See Also:
   AEE_GET_Y
======================================================================= 
Function: AEE_GET_Y()         

Description: 
   This is a macro that can be used to extract the Y-Coordinate value from
   the given 32-bit dwParam. This is used in conjunction with the AVK_TAP key code.
   When a TAP click occurs, the app receives the event EVT_KEY with
   wParam set to AVK_TAP and dwParam containing the X and Y coordinates of the location
   of the tap click. The upper 16 bits of the dwParam contains the X coordinate while
   the lower 16 bits contains the Y coordinate of the click. This macro (AEE_GET_Y()) can be
   used to get the Y coordinate of the tap click.
   In the SDK, the same event is received when the mouse is single-clicked inside the
   screen portion of the device being emulated.

Prototype:

uint16 AEE_GET_Y(uint32 dwParm)         


Parameters:
   dwParm: 32-bit value containing the X and Y coordinate of the click

Return Value:
   Y coodrinate of the TAP click

Comments:
   None
   
Side Effects: 
   None

See Also:
   AEE_GET_X

=======================================================================*/

