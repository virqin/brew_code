/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __XUTIL_H__
#define __XUTIL_H__

#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEAppGen.h"

static __inline boolean cmpfn_memory(const void *p1, const void *p2, int n)
{
    return (0 == MEMCMP(p1, p2, n));
}

static __inline boolean cmpfn_str(const void *p1, const void *p2, int n)
{
    (void)n;
    return (0 == STRCMP(*(char **)p1, *(char **)p2));
}

static __inline boolean cmpfn_strchr(const void *p1, const void *p2, int n)
{
    (void)n;
    return (NULL != STRCHR((const char *)p1, *(char *)p2));
}

#define Array_Find(pArray, nLen, pValue, cmpfn, cmparg, nPos) \
    do { \
        for (nPos = 0; nPos < nLen; nPos++) \
        { \
            if (cmpfn((pValue), &(pArray)[nPos], cmparg)) { break; } \
        } \
        if (nPos >= nLen) { nPos = -1; } \
    } while (0)

#define Array_Find_Rev(pArray, nLen, pValue, cmpfn, cmparg, nPos) \
    do { \
        for (nPos = nLen - 1; nPos >= 0; nPos--) \
        { \
            if (cmpfn((pValue), &(pArray)[nPos], cmparg)) { break; } \
        } \
        if (nPos < 0) { nPos = -1; } \
    } while (0)

//==============================================================================

// Release helper
#ifndef RELEASEIF
#define RELEASEIF(p)       RELEASEPPIF((IBase**)(void*)&p)
static __inline void RELEASEPPIF(IBase **p) 
{
    if (*p) {
        IBASE_Release(*p);
        *p = 0;
    }
}
#endif

#ifndef ADDREFIF
#define ADDREFIF(p) do { if (p) IBASE_AddRef((IBase*)(void *)(p)); } while(0)
#endif

#ifndef RELEASE_IF
#define RELEASE_IF(typ,ptr) if (ptr) { typ##_Release((ptr)); }
#endif

#ifndef ADDREF_IF
#define ADDREF_IF(typ,ptr)  if (ptr) { typ##_AddRef((ptr)); }
#endif

// allocates an object with an appended vtbl
#ifndef MALLOCREC_VTBL
#define MALLOCREC_VTBL(typ,vtt) MALLOCREC_EX(typ, sizeof(AEEVTBL(vtt)))
#endif

#ifndef MALLOCARRAY
#define MALLOCARRAY(type,size) ((type*)MALLOC(sizeof(type)*(size)))
#endif

#ifndef CAST
#define CAST(t, exp) ((t)(void*)(exp))
#endif

static void ReleaseObject(void* pe) {
    IBase_Release(pe);
}

#ifndef New

typedef void (*PFNCTOR)(void *po);

static __inline void *New_Helper(void *po, PFNCTOR pfnCtor)
{
    if (NULL == po) {
        return NULL;
    }

    pfnCtor(po);
    return po;
}

#define New(type) \
    (type *)New_Helper(MALLOCREC(sizeof(type), type##_Ctor)

static __inline void *NewVT_Helper(void *po, uint32 nSize, PFNCTOR pfnCtor)
{
    if (NULL == po) {
        return NULL;
    }

    *(void**)po = (char *)po + nSize; // Initialize the pointer of vtbl
    pfnCtor(po);
    return po;
}

#define NewVT(type, vtt) \
    (type *)NewVT_Helper(MALLOCREC_VTBL(type, vtt), sizeof(type), type##_Ctor)

#endif // New


#define STRUCT(struct_name) \
    typedef struct struct_name struct_name; \
    struct struct_name

#define POINTER_EVENT_CASE XEVT_MOUSEDOWN: \
    case XEVT_MOUSEMOVE: \
    case XEVT_MOUSEUP

#define AEE_IsPointerEvent(eCode) \
    (XEVT_MOUSEDOWN <= eCode && eCode <= XEVT_MOUSEUP)

#define AEE_IsKeyEvent(eCode) \
    (XEVT_KEYDOWN == eCode || XEVT_KEY == eCode || XEVT_KEYUP == eCode)

#define AEE_IsRectEmpty(prc) (((prc)->dx <= 0) || ((prc)->dy <= 0))

#define AEE_PtInRect(prc, l, t) \
    ((l) >= (prc)->x && (l) < (prc)->x + (prc)->dx && \
    (t) >= (prc)->y && (t) < (prc)->y + (prc)->dy)

#define AEE_OffsetRect(prc, nx, ny) \
    (prc)->x += (int16)(nx), (prc)->y += (int16)(ny)

#define AEE_InflateRect(prc, nx, ny) \
    (prc)->x -= (nx), (prc)->y -= (ny), \
    (prc)->dx += (nx) * 2, (prc)->dy += (ny) * 2

#define AEE_SetRectEmpty(prc) ((prc)->dx = (prc)->dy = 0)

boolean AEE_IntersectRect(AEERect *prcDest, const AEERect *prcSrc1, const AEERect *prcSrc2);
boolean AEE_SubtractRect(AEERect *prcResult, const AEERect *prcBase, const AEERect *prcSub);
void    AEE_UnionRect(AEERect *prcResult, const AEERect *prcSrc1, const AEERect *prcSrc2);

static __inline AEEApplet *GetApplet() {
    return (AEEApplet*)GETAPPINSTANCE();
}

static __inline IShell *GetShell() {
    return GetApplet()->m_pIShell;
}

static __inline IDisplay *GetDisplay() {
    return GetApplet()->m_pIDisplay;
}

#define DECLARE_ME(classname)       classname* pMe = (classname*)po
#define DECLARE_PVTBL(pvt, vtt)     AEEVTBL(vtt)* pvt = AEEGETPVTBL(pMe, vtt)
#define VT_FUNC(pvt, type, func)    pvt->func = type##_##func

int IBitmap_TestOpacity(IBitmap* piBitmap, int x, int y, boolean* pb);

int ISHELL_LoadResStringEx(IShell *pIShell, const char *pszBaseFile, uint16 nResID, AECHAR **ppBuff);

//==============================================================================
static void GetScreenSize(uint16* cx, uint16* cy)
{
    AEEDeviceInfo* pdi;
    pdi = MALLOC(sizeof(AEEDeviceInfo));
    if (pdi == NULL)
        return;
    ISHELL_GetDeviceInfo(GetShell(), pdi);
    *cx = pdi->cxScreen;
    *cy = pdi->cyScreen;
    FREEIF(pdi);
}

#endif // __XUTIL_H__
