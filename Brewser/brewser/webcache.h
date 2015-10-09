/*
  ============================================================================

FILE:  webcache.h

SERVICES:  
       WebCache: an object that caches WebRespInfo structs and the 
         associated data.

GENERAL DESCRIPTION:
       WebCache stores the returned relevant data from an IWebResp, namely
         the WebRespInfo.

REVISION HISTORY: 
        Tue Feb 25 16:23:37 2003: Created

  ============================================================================
  ============================================================================

               Copyright © 1999-2003 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/
#ifndef WEBCACHE_H
#define WEBCACHE_H /* #ifndef WEBCACHE_H */

#include "AEE.h"
#include "AEEWeb.h"

#include "webcache.bid"

#ifndef WEBREQUEST_CACHEINVALIDATE
#define WEBREQUEST_CACHEINVALIDATE 0x0040
#endif /* #ifndef WEBREQUEST_CACHEINVALIDATE */

#ifndef WEBREQUEST_CACHECHECKEXPIRES
#define WEBREQUEST_CACHECHECKEXPIRES 0x0080
#endif /* #ifndef WEBREQUEST_CACHECHECKEXPIRED  */


/* @@@ this should be in AEEWeb.h */
#if !defined(INHERIT_IWeb)
#define INHERIT_IWeb(iname) \
   INHERIT_IWebOpts(iname);\
   void  (*GetResponse) (iname *po, IWebResp **ppiwresp, \
                         AEECallback *pcb, const char *cpszUrl, ...);\
   void  (*GetResponseV)(iname *po, IWebResp **ppiwresp, \
                         AEECallback *pcb, const char *cpszUrl, \
                         WebOpt *awozList)
#endif /* #if !defined(INHERIT_IWeb) */

typedef struct IWebCache IWebCache;

#define INHERIT_IWebCache(iname) \
   INHERIT_IWeb(iname);\
   uint32 (*SetFlags)(iname *po, uint32 uFlags);\
   void   (*SetLimits)(iname *po, uint32 uMinRAM, uint32 uMaxDisk)\

AEEINTERFACE(IWebCache) {
   INHERIT_IWebCache(IWebCache);
};

#define IWEBCACHE_AddRef(p)                 AEEGETPVTBL((p),IWebCache)->AddRef((p))
#define IWEBCACHE_Release(p)                AEEGETPVTBL((p),IWebCache)->Release((p))
#define IWEBCACHE_QueryInterface(p,i,o)     AEEGETPVTBL((p),IWebCache)->QueryInterface((p),(i),(o))
#define IWEBCACHE_AddOpt(p,o)               AEEGETPVTBL((p),IWebCache)->AddOpt((p),(o))
#define IWEBCACHE_RemoveOpt(p,i,n)          AEEGETPVTBL((p),IWebCache)->RemoveOpt((p),(i),(n))
#define IWEBCACHE_GetOpt(p,i,n,o)           AEEGETPVTBL((p),IWebCache)->GetOpt((p),(i),(n),(o))
#define IWEBCACHE_GetResponse(p,args)       AEEGETPVTBL((p),IWebCache)->GetResponse args
#define IWEBCACHE_GetResponseV(p,r,cb,u,ol) AEEGETPVTBL((p),IWebCache)->GetResponseV((p),(r),(cb),(u),(ol))
#define IWEBCACHE_SetFlags(p,b)             AEEGETPVTBL((p),IWebCache)->SetFlags((p),(b))
#define IWEBCACHE_SetLimits(p,r,d)          AEEGETPVTBL((p),IWebCache)->SetLimits((p),(r),(d))

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

   int  IWebCache_New(IShell *piShell, AEECLSID clsid, void **ppo);
   void IWebCache_Dump(IWebCache *me); /* debugging purposes */
#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef WEBCACHE_H */
