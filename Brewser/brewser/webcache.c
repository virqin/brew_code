/*===================================================================
FILE:  webcache.c

SERVICES:  caching Web Interface

DESCRIPTION:

PUBLIC CLASSES AND FUNCTIONS:

        Copyright © 1999-2001 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===================================================================*/
#include "AEEStdLib.h"
#include "webcache.h"
#include "util.h"


#define FARF_LOWRAM      1

/*====================================================================
  macros
  ===================================================================*/

/*
  ||  SLL_FIND(p,pp,phead,next,match) : Find object in singly-linked list.
  || 
  ||      p     = variable to point to result object        (Type *p)
  ||      pp    = variable to point to pointer to result    (Type **p)
  ||      phead = head of list (lvalue)                     (Type *phead)
  ||      next  = name of the field holding 'next' pointers (lvalue)
  ||      match = expression that evaluates to TRUE when 'p' points 
  ||                 to the desired object
  || 
  ||  On exit:
  ||      p points to first matching object and pp pointer to the pointer 
  ||        that points to it.  (Modify *pp to remove p or insert before p).
  ||      If not found, p == NULL and pp = pointer to the final 'next' 
  ||        pointer (or the head if list is empty)
*/
#define SLL_FIND(p,pp,phead,next,match) for ((pp)=&(phead); ((p)=*(pp)) != 0 && !(match); (pp) = &((p)->next))

/*
  ||  SLL_APPEND(phead, pnewitem, type, nextmember)
  || 
  ||      phead    = head of list (lvalue)                     (type *phead)
  ||      pnewitem = variable to point to result object        (type *pnewitem)
  ||      type     = name of type for linked structures
  ||      nextfld  = name of the field holding 'next' pointers (lvalue)
*/
#define SLL_APPEND(phead, pnewitem, type, nextfld) \
   do {                                                 \
      type **lfp_ = &(phead);                           \
      while (*lfp_)                                     \
         lfp_ = &((*lfp_)->nextfld);                    \
      *lfp_ = (pnewitem);                               \
      (pnewitem)->nextfld = 0;                          \
   } while (0)

/*====================================================================
  deee-fines
  ===================================================================*/

/*====================================================================
  typedefs
  ===================================================================*/

typedef struct WebCacheEntry WebCacheEntry;

struct IWebCache
{
   AEEVTBL(IWebCache) *pvt;

   uint32         uRef;

   IShell        *piShell;
   IWeb          *piWeb; /* delegate to this guy for most stuff */

   AEECallback    cbLowRAM;

   unsigned       uFlags:16;  /* only care about a couple flags */
   unsigned       bLoading:1; /* loading from disk, don't discard in
                                 LowRAM */
   unsigned       bDirty:1;   /* need to re-persist */

   uint32         uMinRAM;    /* LowRAM toss threshold */
   uint32         uMaxDisk;   /* Serialization threshold */
   
   WebCacheEntry *pwceList; /* actual cache */

   /* "class" vtables */
   AEEVTBL(IWebCache)  vtWebCache;
   AEEVTBL(IWebResp)   vtWebCacheResp;
   AEEVTBL(ISource)    vtWCESource;

};

typedef struct WebCacheEntryWaiter
{
   void          *pHandlerData;
   PFNWEBSTATUS   pfnStatusHandler;
   PFNWEBHEADER   pfnHeaderHandler;

   /* the following supplied for WebCacheEntry's work */
   long           lHeadersLen;
   WebStatus      wsLast;
   void          *pwsLastData;

} WCEWaiter;

typedef struct MemChunk
{
   char     *pData; /* is unlocked memory */
   int       nLen;
} MemChunk;

typedef struct ISource
{
   AEEVTBL(ISource) *pvt;

   uint32          uRef;
   IWebCache      *piwc;
   WebCacheEntry  *pwce;       /* I get everything from here */
   int             nReadSoFar; /* how much of pwce->nReadSoFar 
                                  I've consumed */
} IWCESource;

struct WebCacheEntry
{
   WebCacheEntry  *pNext;

   uint32          uRef;

   IWebOpts       *piWebOpts; /* holds options for request */
   IWebCache      *piwc;      /* not addref'd, he's my owner */
   IWeb           *piWeb;
   IShell         *piShell;

   unsigned        bGotResp:1;
   unsigned        bShouldKeep:1;

   AEECallback     cbWeb;    /* used for IWEB_GetResponse() */
   AEECallback     cbRead;   /* used for ISOURCE_Readable() */
 
   int             nLastRead;
   int             nReadSoFar;

   MemChunk        mcBody;    /* body, is unlocked memory */
   MemChunk        mcHeaders; /* headers, is unlocked memory */

   WebStatus       wsLast; /* for those just joining... */
   void           *pwsLastData;

   IWebResp       *piWebResp;
   WebRespInfo     wri;  /* stuff I got from IWeb */
   uint32          tLastUsed;

   AEECallback    *pcbWaiters; /* people waiting for Ready() */
   AEECallback    *pcbReaders; /* people waiting for Readable() */

   char            szUrl[1]; /* over-allocated */
};

typedef struct IWebResp
{
   AEEVTBL(IWebResp) *pvt;
   
   uint32             uRef;

   IWebCache         *piwc;
   IWebOpts          *piWebOpts; /* holds options for request */

   AEECallback        cb; /* to wait on cache entry */

   WebRespInfo        wri; /* mostly points to pwce, but 
                              holds an ISource specific to this 
                              response */
   WebCacheEntry     *pwce;
   WCEWaiter          wcew; /* holds stuff that helps me get 
                               headers, status */
   IWebResp         **ppiwrespRet;   
   AEECallback       *pcbRet;
   
} IWebCacheResp;

/*====================================================================
  forward function decls (vtable members)
  ===================================================================*/

/* IWebCache methods */
static uint32 IWebCache_AddRef(IWebCache *piwc);
static uint32 IWebCache_Release(IWebCache *piwc);
static int IWebCache_QueryInterface(IWebCache *piwc, AEECLSID id, void **ppo);
static int IWebCache_AddOpt(IWebCache *piwc, WebOpt *awoz);
static int IWebCache_RemoveOpt(IWebCache *piwc, int32 nOptId, int32 nIndex);
static int IWebCache_GetOpt(IWebCache *piwc, int32 nOptId, int32 nIndex, WebOpt *pwo);
static void IWebCache_GetResponse(IWebCache *piwc, IWebResp **ppiwresp, 
                                  AEECallback *pcb, const char *cpszUrl, ...);
static void IWebCache_GetResponseV(IWebCache *piwc, IWebResp **ppiwresp, 
                                   AEECallback *pcb, const char *cpszUrl, 
                                   WebOpt *awozList);
static uint32 IWebCache_SetFlags(IWebCache *me, uint32 uFlags);
static void IWebCache_SetLimits(IWebCache *me, uint32 uMinRAM, 
                                uint32 uMaxDisk);

/* ISource methods */
static uint32  IWCESource_AddRef(ISource *me);
static uint32  IWCESource_Release(ISource *me);
static int     IWCESource_QueryInterface(ISource *me, AEECLSID cls, void **pp);
static int32   IWCESource_Read(ISource *me, char *pcBuf, int32 cbBuf);
static void    IWCESource_Readable(ISource *me, AEECallback *pcb);


/* IWebCacheResp methods */
static uint32 IWebCacheResp_AddRef(IWebCacheResp *piwcr);
static uint32 IWebCacheResp_Release(IWebCacheResp *piwcr);
static int IWebCacheResp_QueryInterface(IWebCacheResp *piwcr, AEECLSID id, void **ppo);
static int IWebCacheResp_AddOpt(IWebCacheResp *piwcr, WebOpt *awoz);
static int IWebCacheResp_RemoveOpt(IWebCacheResp *piwcr, int32 nOptId, int32 nIndex);
static int IWebCacheResp_GetOpt(IWebCacheResp *piwcr, int32 nOptId, int32 nIndex, WebOpt *pwo);
static WebRespInfo *IWebCacheResp_GetInfo(IWebCacheResp *piwcr);

/* other forward decls */
static int IWebCacheResp_Transaction(IWebCache *piwc, IWebResp **ppiwrespRet, 
                                     AEECallback *pcbRet, const char *cpszUrl, 
                                     WebOpt *awozList);
static void IWebCache_DropEntry(IWebCache *me, WebCacheEntry *pwce);

static int IWCESource_New(IWebCache *piwc, WebCacheEntry *pwce, void **pp);


/*====================================================================
  MemChunk stuff 
  ===================================================================*/
static void MemChunk_Dtor(MemChunk *me)
{
   FREEIF(me->pData);
   me->nLen = 0;
}

static boolean MemChunk_Resize(MemChunk *me, int nLen)
{
   boolean  bRet;

   LOCKMEM((void **)&me->pData);

   bRet = (SUCCESS == ERR_REALLOC(nLen, &me->pData));

   if (bRet) {
      me->nLen = nLen;
   }

   UNLOCKMEM((void **)&me->pData);   

   return bRet;
}


/*====================================================================
  WebCacheEntry stuff 
  ===================================================================*/

static void WebCacheEntry_Delete(WebCacheEntry *me)
{
   CALLBACK_Cancel(&me->cbWeb);
   CALLBACK_Cancel(&me->cbRead);

   RELEASEIF(me->piWeb);
   RELEASEIF(me->piShell);

   RELEASEIF(me->piWebOpts);

   RELEASEIF(me->piWebResp);
   RELEASEIF(me->wri.pisMessage);

   FREEIF(me->wri.cpszCharset);
   FREEIF(me->wri.cpszContentType);

   MemChunk_Dtor(&me->mcBody);
   MemChunk_Dtor(&me->mcHeaders);

   FREE(me);
}

static int WebCacheEntry_New(IWebCache *piwc, IWeb *piWeb, IShell *piShell,
                             const char *cpszUrl, IWebOpts *piWebOpts,
                             WebCacheEntry **ppwce)
{
   WebCacheEntry *me;
   int            nErr;

   {
      WebOpt awo[2];
      
      awo[0].nId = WEBOPT_COPYOPTS;
      awo[0].pVal = (void *)TRUE;
      awo[1].nId = WEBOPT_END;
      
      nErr = IWEBOPTS_AddOpt(piWebOpts, awo);
   }

   if (nErr == SUCCESS) {
      nErr = ERR_MALLOCREC_EX(WebCacheEntry,STRLEN(cpszUrl),&me);
   }
   
   if (SUCCESS == nErr) {
      STRCPY(me->szUrl,cpszUrl);
      me->uRef = 1;
      
      me->piWebOpts = piWebOpts;
      IWEBOPTS_AddRef(piWebOpts);
      
      me->piwc = piwc;

      me->piWeb = piWeb;
      IWEB_AddRef(piWeb);
      
      me->piShell = piShell;
      ISHELL_AddRef(piShell);
      
      WEBRESPINFO_CTOR(&me->wri);

      me->wsLast = (WebStatus)-1;
   }

   *ppwce = me;

   return nErr;
}

static uint32 WebCacheEntry_AddRef(WebCacheEntry *me)
{
   return ++me->uRef;
}

static void WebCacheEntry_DropTimer(void *me);
static uint32 WebCacheEntry_Release(WebCacheEntry *me)
{
   uint32 uRef = --me->uRef;

   if (0 == uRef) {

      WebCacheEntry_Delete(me);

   } else if (1 == uRef) { /* only the piwc holds me */
      
      RELEASEIF(me->piWebResp);

      if (!me->bShouldKeep) { /* should cache */
         if ((0 < me->nLastRead) || (ISOURCE_WAIT == me->nLastRead)) {
            /* if I'm not done, gimme a second, I might finish ;) */
            CALLBACK_Init(&me->cbWeb, WebCacheEntry_DropTimer,me);
            ISHELL_SetTimerEx(me->piShell,1000,&me->cbWeb);
         } else {
            IWebCache_DropEntry(me->piwc, me);            
         }
      }
   }

   return uRef;
}

static void WebCacheEntry_DropTimer(void *p)
{
   WebCacheEntry *me = (WebCacheEntry *)p;

   WebCacheEntry_AddRef(me);
   me->nLastRead = ISOURCE_ERROR;
   WebCacheEntry_Release(me);
}

static void WebCacheEntry_UberCancel(AEECallback *pcb)
{
   AEECallback **lfp = (AEECallback **)pcb->pCancelData;

   for (; pcb != *lfp; lfp = &(*lfp)->pNext);
   
   *lfp = pcb->pNext;
   pcb->pNext = 0;

   pcb->pfnCancel = 0;
}


static void WebCacheEntry_CatchupWaitersStatus(WebCacheEntry *me)
{
   WebStatus  ws = me->wsLast;
   void      *pData = me->pwsLastData;
   
   for (;;) {
      AEECallback *pcb;
      WCEWaiter   *pwcew;
      
      for (pcb = me->pcbWaiters; (AEECallback *)0 != pcb; pcb = pcb->pNext) {
         pwcew = (WCEWaiter *)pcb->pReserved;

         if ((pwcew->wsLast != ws) || (pwcew->pwsLastData != pData)) {
            break;
         }
      }
      
      if ((AEECallback *)0 == pcb) {
         break;
      }
      pwcew->wsLast = ws;
      pwcew->pwsLastData = pData;
         
      if ((PFNWEBSTATUS)0 != pwcew->pfnStatusHandler) {
         pwcew->pfnStatusHandler(pwcew->pHandlerData, ws, pData);
      }
   }
}

static void WebCacheEntry_WebStatus(void *p, WebStatus ws, void *pData)
{
   WebCacheEntry *me = (WebCacheEntry *)p;

   me->wsLast = ws;
   me->pwsLastData = pData;

   WebCacheEntry_CatchupWaitersStatus(me);
   
}

static void WebCacheEntry_CatchupWaitersHeaders(WebCacheEntry *me)
{
   if ((0 >= me->mcHeaders.nLen) ||
       ('\0' != me->mcHeaders.pData[me->mcHeaders.nLen-1])) {
      return;
   }

   for (;;) {
      AEECallback *pcb;
      WCEWaiter   *pwcew;
      
      for (pcb = me->pcbWaiters; (AEECallback *)0 != pcb; pcb = pcb->pNext) {
         pwcew = (WCEWaiter *)pcb->pReserved;
         
         if (pwcew->lHeadersLen < me->mcHeaders.nLen) {
            break;
         }
      }
      
      if ((AEECallback *)0 == pcb) {
         break;
      }
      
      LOCKMEM((void **)&me->mcHeaders.pData);
      {
         char    *pszName;
         GetLine  glVal;
      
         pszName = me->mcHeaders.pData+pwcew->lHeadersLen;
         pwcew->lHeadersLen += STRLEN(pszName)+1;
         glVal.psz = me->mcHeaders.pData+pwcew->lHeadersLen;
         glVal.nLen = STRLEN(glVal.psz);
         glVal.bLeftover = glVal.bTruncated = FALSE;
         pwcew->lHeadersLen += glVal.nLen+1;
         
         if ((PFNWEBHEADER)0 != pwcew->pfnHeaderHandler) {
            pwcew->pfnHeaderHandler(pwcew->pHandlerData, pszName, &glVal);
         }
      }
      UNLOCKMEM((void **)&me->mcHeaders.pData);

   }
}

static void WebCacheEntry_WebHeader(void *p, const char *cpszName, 
                                    GetLine *pglVal)
{
   WebCacheEntry *me = (WebCacheEntry *)p;
   int            nNameLen;
   int            nValLen;

   /* don't wanna trust that pglVal->psz has no '\0's, I depend on 
      them for data format */
   nValLen = STRLEN(pglVal->psz);

   /* end of header, insert '\0' into header buffer */
   if (!pglVal->bTruncated) {
      nValLen++;
   }

   if ((char *)0 != cpszName) {
      nNameLen = STRLEN(cpszName)+1;
   } else {
      nNameLen = 0;
      if ((0 != me->mcHeaders.nLen) &&
          ('\0' == me->mcHeaders.pData[me->mcHeaders.nLen-1])) {
         return; /* have to bail, missed most of this header, 
                    don't append a fragment */
      }
   }

   if (!MemChunk_Resize(&me->mcHeaders,me->mcHeaders.nLen+nNameLen+nValLen)) {
      return;
   }

   MEMMOVE(me->mcHeaders.pData+me->mcHeaders.nLen-nNameLen-nValLen,
           cpszName,nNameLen);
   MEMMOVE(me->mcHeaders.pData+me->mcHeaders.nLen-nValLen,
           pglVal->psz,nValLen);

   WebCacheEntry_CatchupWaitersHeaders(me);

}

static void WebCacheEntry_ReadBody(void *p)
{
   WebCacheEntry *me = (WebCacheEntry *)p;
   int nRead;
   int nAvail;

   {
      AEECallback *pcb;
      while ((AEECallback *)0 != (pcb = me->pcbReaders)) {
         ISHELL_Resume(me->piShell, pcb);
      }
   }

   if ((ISource *)0 == me->wri.pisMessage) {
      return;
   }

   nAvail = me->mcBody.nLen - me->nReadSoFar;
      
   if (nAvail < 512) {
      if (!MemChunk_Resize(&me->mcBody,me->mcBody.nLen+512)) {
         me->nLastRead = ISOURCE_ERROR;
         goto done;
      }
      nAvail = me->mcBody.nLen-me->nReadSoFar;
   }
      
   LOCKMEM((void **)&me->mcBody.pData);
   nRead = ISOURCE_Read(me->wri.pisMessage,
                        me->mcBody.pData+me->nReadSoFar,nAvail);
   UNLOCKMEM((void **)&me->mcBody.pData);
      
   me->nLastRead = nRead;
      
   if (ISOURCE_WAIT != nRead) {
         
      if (0 >= nRead) {
         /* report error */
         if (ISOURCE_END == nRead) {
            me->bShouldKeep = TRUE;
            me->wri.lContentLength = me->nReadSoFar;
            me->piwc->bDirty = TRUE;
         }
            
      done:
         MemChunk_Resize(&me->mcBody,me->nReadSoFar);
         RELEASEIF(me->wri.pisMessage);
         return;
      }
         
      me->nReadSoFar += nRead;
   }

   CALLBACK_Init(&me->cbRead, WebCacheEntry_ReadBody,me);
   ISOURCE_Readable(me->wri.pisMessage,&me->cbRead);
}

static void WebCacheEntry_GetRespInfo(WebCacheEntry *me, WebRespInfo *pwri,
                                      IWebResp **ppiwresp)
{
   int nErr;

   pwri->nCode           = me->wri.nCode;
   pwri->lContentLength  = me->wri.lContentLength;
   pwri->cpszContentType = me->wri.cpszContentType;
   pwri->cpszCharset     = me->wri.cpszCharset;
   pwri->tExpires        = me->wri.tExpires;
   pwri->tModified       = me->wri.tModified;

   *ppiwresp = me->piWebResp;

   if ((IWebResp *)0 != me->piWebResp) {
      IWEBRESP_AddRef(me->piWebResp);
   }

   nErr = IWCESource_New(me->piwc, me, (void **)&pwri->pisMessage);

   if (SUCCESS != nErr) {
      pwri->nCode = -nErr;
   }

}

static void WebCacheEntry_ParseHeaders(WebCacheEntry *me)
{
   char *pszName, *pszValue, *pEnd;

   /* no headers */
   if (0 >= me->mcHeaders.nLen) {
      return;
   }

   /* enforce null termination, or bail */
   if (('\0' != me->mcHeaders.pData[me->mcHeaders.nLen-1]) &&
       !MemChunk_Resize(&me->mcHeaders,me->mcHeaders.nLen+1)) {
      return;
   }

   pEnd = me->mcHeaders.pData + me->mcHeaders.nLen;
   for (pszName = me->mcHeaders.pData; pszName < pEnd;
        pszName = pszValue + STRLEN(pszValue) + 1) {

      pszValue = pszName + STRLEN(pszName) + 1;

      if (!STRICMP(pszName, "Expires")) {
         char *pszExpires = STRDUP(pszValue);
         if ((char *)0 != pszExpires) {
            me->wri.tExpires = WebDate_Parse(pszExpires);
         }
         FREEIF(pszExpires);
      }
   }
}

static void WebCacheEntry_Ready(WebCacheEntry *me)
{
   AEECallback *pcb;

   /* don't need these anymore */
   RELEASEIF(me->piWebOpts);

   WebCacheEntry_CatchupWaitersStatus(me);
   WebCacheEntry_CatchupWaitersHeaders(me);

   while ((AEECallback *)0 != (pcb = me->pcbWaiters)) {
      ISHELL_Resume(me->piShell, pcb);
   }

}

static void WebCacheEntry_GotResp(void *p)
{
   WebCacheEntry *me = (WebCacheEntry *)p;
   WebRespInfo *pwri;
   
   me->bGotResp = TRUE;

   pwri = IWEBRESP_GetInfo(me->piWebResp);

   me->wri.nCode           = pwri->nCode;
   me->wri.pisMessage      = pwri->pisMessage;
   me->wri.lContentLength  = pwri->lContentLength;
   me->wri.cpszContentType = STRDUP(pwri->cpszContentType);
   me->wri.cpszCharset     = STRDUP(pwri->cpszCharset);
   me->wri.tModified       = pwri->tModified;
   /* me->wri.tExpires parsed myself... */

   WebCacheEntry_ParseHeaders(me);

   if ((ISource *)0 != pwri->pisMessage) {
      ISOURCE_AddRef(me->wri.pisMessage);
      me->nLastRead = ISOURCE_WAIT;
   } else {
      me->nLastRead = ISOURCE_END;
   }      

   WebCacheEntry_Ready(me);
}


static boolean WebCacheEntry_IsExpired(WebCacheEntry *me)
{
   return (me->bGotResp && (me->wri.tExpires < (int32)GETTIMESECONDS()));
}

static void WebCacheEntry_Readable(WebCacheEntry *me, AEECallback *pcb)
{
   if (CALLBACK_IsQueued(&me->cbRead)) {
      CALLBACK_Cancel(pcb);

      SLL_APPEND(me->pcbReaders,pcb,AEECallback,pNext);

      pcb->pfnCancel = WebCacheEntry_UberCancel;
      pcb->pCancelData = &me->pcbReaders;
   } else {
      ISHELL_Resume(me->piShell,pcb);
   }
}

static void WebCacheEntry_Wait(WebCacheEntry *me,
                               WCEWaiter     *pwcew,
                               AEECallback   *pcb)
{
   CALLBACK_Cancel(pcb);

   SLL_APPEND(me->pcbWaiters,pcb,AEECallback,pNext);
   
   pcb->pfnCancel = WebCacheEntry_UberCancel;
   pcb->pCancelData = &me->pcbWaiters;
   pcb->pReserved = pwcew;

   /* setup the guy's waiter vars */
   pwcew->lHeadersLen = 0;
   pwcew->wsLast = (WebStatus)-1;
   
   if (!me->bGotResp) {
      if (CALLBACK_IsQueued(&me->cbWeb)) {
         /* already working on it... */
         return;
      }
      CALLBACK_Init(&me->cbWeb, WebCacheEntry_GotResp,me);
      IWEB_GetResponse(me->piWeb,
                       (me->piWeb,
                        &me->piWebResp,&me->cbWeb,
                        me->szUrl,
                        WEBOPT_HANDLERDATA, me,
                        WEBOPT_STATUSHANDLER,WebCacheEntry_WebStatus,
                        WEBOPT_HEADERHANDLER,WebCacheEntry_WebHeader,
                        WEBOPT_DEFAULTS, me->piWebOpts,
                        WEBOPT_END, me->piWebOpts));
   } else {
      /* 
         __must__ re-initialize and re-schedule the callback, since I 
         re-use cbWeb for the "gimme a second" timer 
      */
         
      CALLBACK_Init(&me->cbWeb, WebCacheEntry_Ready,me);
      ISHELL_Resume(me->piShell, &me->cbWeb);
   }

}

#define IFILE_WRITE(f,p,l) (((uint32)(l)) == IFILE_Write((f),(p),(l)))
#define IFILE_READ(f,p,l)  (((int32)(l)) == IFILE_Read((f),(p),(l)))

static boolean IFile_WriteRecord(IFile *piFile, const char *pRecord, 
                                 int nRecordLen)
{
   return (IFILE_WRITE(piFile,&nRecordLen,sizeof(nRecordLen)) &&
           IFILE_WRITE(piFile,pRecord,nRecordLen));
}

static boolean IFile_ReadRecord(IFile *piFile, char **ppRecord, 
                                int *pnRecordLen)
{
   return (IFILE_READ(piFile,pnRecordLen,sizeof(*pnRecordLen)) &&
           (SUCCESS == ERR_REALLOC(*pnRecordLen,ppRecord)) &&
           ((0 == *pnRecordLen) || /* don't call read(0,0), it busts */
            IFILE_READ(piFile,*ppRecord,*pnRecordLen)));
}

static uint32 WebCacheEntry_Size(WebCacheEntry *me)
{
   if (me->bShouldKeep) {
      FARF(ALWAYS,("WebCacheEntry_Size == %d",
                   me->mcHeaders.nLen+me->mcBody.nLen+STRLEN(me->szUrl)+sizeof(*me)));
      
      return (me->mcHeaders.nLen +
              me->mcBody.nLen +
              STRLEN(me->szUrl) +
              sizeof(*me) +
              ((char *)0==me->wri.cpszCharset?
               0:STRLEN(me->wri.cpszCharset)+1) +
              ((char *)0==me->wri.cpszContentType?
               0:STRLEN(me->wri.cpszContentType)+1));
   } else {
      FARF(ALWAYS,("WebCacheEntry_Size == %d ***not counted****",
                   me->mcHeaders.nLen+me->mcBody.nLen+STRLEN(me->szUrl)+sizeof(*me)));
      return 0;
   }

}

static boolean WebCacheEntry_Deserialize(IWebCache *piwc,
                                         IShell *piShell, IFile *piFile, 
                                         WebCacheEntry **ppwce)
{
   boolean bRet;
   int     nLen;
   WebCacheEntry *me = 0;
   
   bRet = (IFILE_READ(piFile,&nLen,sizeof(nLen)) 
           && 
           (SUCCESS == ERR_MALLOCREC_EX(WebCacheEntry,nLen,&me))
           &&
           IFILE_READ(piFile,me->szUrl,nLen) 
           &&
           IFILE_READ(piFile,&me->wri.nCode,sizeof(me->wri.nCode))
           &&
           IFILE_READ(piFile,&me->wri.lContentLength,
                      sizeof(me->wri.lContentLength))
           &&
           IFile_ReadRecord(piFile,&(char *)me->wri.cpszContentType,&nLen)
           &&
           IFile_ReadRecord(piFile,&(char *)me->wri.cpszCharset,&nLen)
           &&
           IFILE_READ(piFile,&me->wri.tExpires,sizeof(me->wri.tExpires))
           &&
           IFILE_READ(piFile,&me->wri.tModified,sizeof(me->wri.tModified))
           &&
           IFILE_READ(piFile,&me->tLastUsed,sizeof(me->tLastUsed))
           &&
           IFILE_READ(piFile,&me->nReadSoFar,sizeof(me->nReadSoFar))
           &&
           IFILE_READ(piFile,&me->nLastRead,sizeof(me->nLastRead))
           &&
           IFile_ReadRecord(piFile,&me->mcHeaders.pData,
                            &(int)me->mcHeaders.nLen)
           &&
           IFile_ReadRecord(piFile,&me->mcBody.pData,&(int)me->mcBody.nLen));
   
   if (!bRet) {
      if ((WebCacheEntry *)0 != me) {
         WebCacheEntry_Delete(me);
         me = 0;
      }
   } else {
      me->uRef = 1;
      me->bGotResp = 1;
      me->bShouldKeep = 1;
      me->piShell = piShell;
      me->piwc = piwc;
   }
   
   *ppwce = me;

   return bRet;
}

/* this is ugly */
static int WebCacheEntry_Serialize(WebCacheEntry *me, IFile *piFile)
{
   if (!me->bShouldKeep) { /* shouldn't persist */
      return 0;
   }

   if (IFile_WriteRecord(piFile,me->szUrl,STRLEN(me->szUrl)+1)
       &&
       IFILE_WRITE(piFile,&me->wri.nCode,sizeof(me->wri.nCode))
       &&
       IFILE_WRITE(piFile,&me->wri.lContentLength,
                   sizeof(me->wri.lContentLength))
       &&
       IFile_WriteRecord(piFile,me->wri.cpszContentType,
                         (char *)0 == me->wri.cpszContentType?
                         0:STRLEN(me->wri.cpszContentType)+1)
       &&
       IFile_WriteRecord(piFile,me->wri.cpszCharset,
                         (char *)0 == me->wri.cpszCharset?
                         0:STRLEN(me->wri.cpszCharset)+1)
       &&
       IFILE_WRITE(piFile,&me->wri.tExpires,
                   sizeof(me->wri.tExpires))
       &&
       IFILE_WRITE(piFile,&me->wri.tModified,
                   sizeof(me->wri.tModified))
       &&
       IFILE_WRITE(piFile,&me->tLastUsed,sizeof(me->tLastUsed))
       &&
       IFILE_WRITE(piFile,&me->nReadSoFar,sizeof(me->nReadSoFar))
       &&
       IFILE_WRITE(piFile,&me->nLastRead,sizeof(me->nLastRead))
       &&
       IFile_WriteRecord(piFile,me->mcHeaders.pData,
                         me->mcHeaders.nLen)
       &&
       IFile_WriteRecord(piFile,me->mcBody.pData,me->mcBody.nLen)) {

      /* return what we wrote */
      return (STRLEN(me->szUrl)+1 +
              sizeof(me->wri.nCode) +
              sizeof(me->wri.lContentLength) +
              ((char *)0 == me->wri.cpszContentType?
               0:STRLEN(me->wri.cpszContentType)+1) +
              ((char *)0 == me->wri.cpszCharset?
               0:STRLEN(me->wri.cpszCharset)+1) +
              sizeof(me->wri.tExpires) +
              sizeof(me->wri.tModified) +
              sizeof(me->tLastUsed) +
              sizeof(me->nReadSoFar) +
              sizeof(me->nLastRead) +
              me->mcHeaders.nLen +              
              me->mcBody.nLen);
   } else {
      return -1;
   }

}

/*====================================================================
  WebCache stuff 
  ===================================================================*/
#define WEBCACHE_FILE    "webcache.dat"
#define WEBCACHE_VERSION  4 /* @@@ TODO change whenever format changes */

static void IWebCache_DropEntry(IWebCache *me, WebCacheEntry *pwce)
{
   WebCacheEntry **lfp;
   
   for (lfp = &me->pwceList; pwce != *lfp; lfp = &(*lfp)->pNext) {
      if ((WebCacheEntry *)0 == *lfp) { /* not in my list */
         return;
      }
   }
   
   *lfp = pwce->pNext;
   pwce->pNext = 0;
   
   WebCacheEntry_Release(pwce);
}


static void IWebCache_Delete(IWebCache *me)
{
   WebCacheEntry *pwce;
   CALLBACK_Cancel(&me->cbLowRAM);

   if (me->bDirty) {
      IFile *piFile = 0;
      int    nErr;
   
      nErr = IShell_OpenFile(me->piShell, 
                             WEBCACHE_FILE, (_OFM_READWRITE|_OFM_CREATE), 
                             &piFile);
      if (SUCCESS == nErr) {
         AEECLSID clsid = AEECLSID_WEBCACHE;
         /* @@@ TODO change whenever format changes */
         int    version = WEBCACHE_VERSION;
         uint32 uWrote = 0;
         /* write for 5 seconds max */
         uint32 tOverTime = GETUPTIMEMS() + (5*1000); 
         
         IFILE_Write(piFile,&clsid,sizeof(clsid));
         IFILE_Write(piFile,&version,sizeof(version));
         
         while (((WebCacheEntry *)0 != me->pwceList) && 
                (uWrote < me->uMaxDisk) &&
                (tOverTime > GETUPTIMEMS())) {
            WebCacheEntry *pwceNewest;
            int            nSize;

            for (pwceNewest = me->pwceList, pwce = pwceNewest->pNext;
                 ((WebCacheEntry *)0 != pwce); /* not end */
                 pwce = pwce->pNext) {
               
               if (pwce->tLastUsed > pwceNewest->tLastUsed) { /* is newer */
                  pwceNewest = pwce;
               }
            }

            nSize = WebCacheEntry_Serialize(pwceNewest,piFile);
            if (nSize < 0) {
               break;
            }
            uWrote += nSize;
            IWebCache_DropEntry(me,pwceNewest);
         }
         IFILE_Truncate(piFile,IFILE_Seek(piFile,_SEEK_CURRENT,0));         
      }
      RELEASEIF(piFile);
   }
   
   while ((WebCacheEntry *)0 != (pwce = me->pwceList)) {
      IWebCache_DropEntry(me,pwce);
   }
   
   RELEASEIF(me->piShell);
   RELEASEIF(me->piWeb);
   FREEIF(me);
}

void IWebCache_Dump(IWebCache *me)
{
   WebCacheEntry *pwce;
   DBGPRINTF("------ Cache Dump -------------");
   for (pwce = me->pwceList; (WebCacheEntry*)0 != pwce; pwce = pwce->pNext) {
      DBGPRINTF("url: %s",pwce->szUrl);
      DBGPRINTF("expires in %d seconds",pwce->wri.tExpires-GETTIMESECONDS());
   }
   DBGPRINTF("-------------------------------");
}



static uint32 IWebCache_Size(IWebCache *me)
{
   uint32         uSize;
   WebCacheEntry *pwce;

   for (pwce = me->pwceList, uSize = 0; (WebCacheEntry *)0 != pwce;
        uSize += WebCacheEntry_Size(pwce),pwce = pwce->pNext);

   FARF(ALWAYS,("IWebCache_Size == %d",uSize));

   return uSize;
}

static void IWebCache_LowRAM(void *p)
{
   IWebCache *me = (IWebCache *)p;
   WebCacheEntry *pwceDrop;

   /* if loading, just let loading fail */
   
   if (me->bLoading) {
      return;
   }
   
   /* toss the oldest one not in use */
   
   FARF(LOWRAM,("IWebCache_LowRAM"));
   
   /* find oldest entry */
   {
      WebCacheEntry *pwce;
      
      for (pwce = me->pwceList, pwceDrop = 0;
           ((WebCacheEntry *)0 != pwce); /* not end */
           pwce = pwce->pNext) {
         
         if ((1 == pwce->uRef) && /* am only holder */
             (((WebCacheEntry *)0 == pwceDrop) || /* no oldest yet */
              (pwce->tLastUsed < pwceDrop->tLastUsed))) { /* is older */
            pwceDrop = pwce;
         }
      }
   }
   
   /* found one, and it won't take me below my minimum */
   if ((WebCacheEntry *)0 != pwceDrop && 
       (IWebCache_Size(me)-WebCacheEntry_Size(pwceDrop) > me->uMinRAM)) {
      FARF(LOWRAM,("IWebCache_LowRAM dropping %s",pwceDrop->szUrl));
      IWebCache_DropEntry(me,pwceDrop);
   } else {
      FARF(LOWRAM,("IWebCache_LowRAM not dropping, %s",pwceDrop?"minimum reached":"nothing to drop"));
   }

   FARF(LOWRAM,("IWebCache_LowRAM done"));
   ISHELL_RegisterSystemCallback(me->piShell,&me->cbLowRAM,AEE_SCB_LOW_RAM);

}

static int IWebCache_GetEntry(IWebCache *me, const char *cpszUrl, 
                              IWebOpts *piWebOpts, WebCacheEntry **ppwce)
{
   int            nErr;
   WebCacheEntry *pwce = 0;
   WebCacheEntry **lfp;
   boolean        bCache = 
      (0 == (me->uFlags & WEBREQUEST_NOCACHE));
   boolean        bInvalidate = 
      (0 != (me->uFlags & WEBREQUEST_CACHEINVALIDATE));
   boolean        bCheckExpires = 
      (0 != (me->uFlags & WEBREQUEST_CACHECHECKEXPIRES));

   {
      WebOpt wo;

      if (SUCCESS == IWEBOPTS_GetOpt(piWebOpts,WEBOPT_FLAGS,0,&wo)) {
         uint32 uFlags = (uint32)wo.pVal;
         
         bCache = bCache && 
            (0 == (uFlags & WEBREQUEST_NOCACHE));

         bInvalidate = bInvalidate || !bCache ||
            (0 != (uFlags & WEBREQUEST_CACHEINVALIDATE));
         
         bCheckExpires = bCheckExpires ||
            (0 != (uFlags & WEBREQUEST_CACHECHECKEXPIRES));
      }
   }

   /* @@@ TODO - make FIND smarter */ (void)piWebOpts;
   SLL_FIND(pwce,lfp,me->pwceList,pNext,!STRCMP(cpszUrl,pwce->szUrl));

   if ((WebCacheEntry *)0 != pwce) { /* something to invalidate/expire */
      
      if (bInvalidate || /* wanna invalidate */
          (bCheckExpires && WebCacheEntry_IsExpired(pwce))) {
         IWebCache_DropEntry(me,pwce);
         pwce = 0;
      }
   }

   if (bCache) {
      /* found a caching or working job */
      if ((WebCacheEntry *)0 != pwce) {
         nErr = SUCCESS;
      } else {
         /* need a new one */
         nErr = WebCacheEntry_New(me, me->piWeb, me->piShell, 
                                  cpszUrl,piWebOpts,&pwce);
         
         if (SUCCESS == nErr) {
            pwce->pNext = me->pwceList; /* put on head of my list */
            me->pwceList = pwce;
         }
      }
   } else {
      nErr = ERESOURCENOTFOUND;
      pwce = 0;      
   }

   if (SUCCESS == nErr) {
      pwce->tLastUsed = GETTIMESECONDS();
      WebCacheEntry_AddRef(pwce); /* for copying into ppwce */
   }

   *ppwce = pwce; /* fix up return value */
   
   return nErr;
}


int IWebCache_New(IShell *piShell, AEECLSID clsid, void **ppo)
{
   IWebCache *me = 0;
   IWeb      *piWeb = 0;
   int        nErr;

   nErr = ISHELL_CreateInstance(piShell, AEECLSID_WEB, (void **)&piWeb);

   if (SUCCESS == nErr) {
      nErr = ERR_MALLOCREC(IWebCache,&me);
   }

   if (SUCCESS == nErr) {
      me->piWeb = piWeb;
      IWEB_AddRef(piWeb);
      
      me->piShell = piShell;
      ISHELL_AddRef(piShell);

      me->pvt = &me->vtWebCache;

      me->vtWebCache.AddRef         = IWebCache_AddRef;
      me->vtWebCache.Release        = IWebCache_Release;
      me->vtWebCache.QueryInterface = IWebCache_QueryInterface;
      me->vtWebCache.AddOpt         = IWebCache_AddOpt;
      me->vtWebCache.RemoveOpt      = IWebCache_RemoveOpt;
      me->vtWebCache.GetOpt         = IWebCache_GetOpt;
      me->vtWebCache.GetResponse    = IWebCache_GetResponse;
      me->vtWebCache.GetResponseV   = IWebCache_GetResponseV;
      me->vtWebCache.SetFlags       = IWebCache_SetFlags;
      me->vtWebCache.SetLimits      = IWebCache_SetLimits;

      /* I hold all the vtables for objects that descend from me (saves RAM) */
      me->vtWCESource.AddRef         = IWCESource_AddRef;
      me->vtWCESource.Release        = IWCESource_Release;
      me->vtWCESource.QueryInterface = IWCESource_QueryInterface;
      me->vtWCESource.Read           = IWCESource_Read;
      me->vtWCESource.Readable       = IWCESource_Readable;

      me->vtWebCacheResp.AddRef         = IWebCacheResp_AddRef;
      me->vtWebCacheResp.Release        = IWebCacheResp_Release;
      me->vtWebCacheResp.QueryInterface = IWebCacheResp_QueryInterface;
      me->vtWebCacheResp.AddOpt         = IWebCacheResp_AddOpt;
      me->vtWebCacheResp.RemoveOpt      = IWebCacheResp_RemoveOpt;
      me->vtWebCacheResp.GetOpt         = IWebCacheResp_GetOpt;
      me->vtWebCacheResp.GetInfo        = IWebCacheResp_GetInfo;

      me->uRef = 1;

      CALLBACK_Init(&me->cbLowRAM, IWebCache_LowRAM, me);
      ISHELL_RegisterSystemCallback(me->piShell,&me->cbLowRAM,AEE_SCB_LOW_RAM);
      
      me->bLoading = TRUE;
      {
         IFile *piFile = 0;
         
         if (SUCCESS == 
             IShell_OpenFile(me->piShell, WEBCACHE_FILE, 
                             (_OFM_READWRITE|_OFM_CREATE), &piFile)) {
            AEECLSID clsid;
            int      version; 
            
            if (IFILE_READ(piFile,&clsid,sizeof(clsid)) &&
                IFILE_READ(piFile,&version,sizeof(version)) &&
                (AEECLSID_WEBCACHE == clsid) &&
                (WEBCACHE_VERSION == version)) {
               WebCacheEntry *pwce;

               while (WebCacheEntry_Deserialize(me,me->piShell,piFile,&pwce)) {
                  SLL_APPEND(me->pwceList,pwce,WebCacheEntry,pNext);
               }
            }

            RELEASEIF(piFile);
         }
      }
      me->bLoading = FALSE;

      /* defaults */
      me->uMinRAM  = 16*1024;
      me->uMaxDisk = 128*1024;
   }

   RELEASEIF(piWeb);

   *ppo = me;
   
   return nErr;
}

static uint32 IWebCache_AddRef(IWebCache *me)
{
   return ++me->uRef;
}

static uint32 IWebCache_Release(IWebCache *me)
{
   uint32 uRef = --me->uRef;

   if (0 == uRef) {
      IWebCache_Delete(me);
   }

   return uRef;
}

static int IWebCache_QueryInterface(IWebCache *me, AEECLSID id, void **ppo)
{
   if ((AEECLSID_QUERYINTERFACE == id) || 
       (AEECLSID_WEBCACHE == id) ||
       (AEECLSID_WEB == id) ||
       (AEECLSID_WEBOPTS == id)) {
      *ppo = me;
      IWebCache_AddRef(me);
      return SUCCESS;
   } else {
      *ppo = 0;
      return ECLASSNOTSUPPORT;
   }
}

static int IWebCache_AddOpt(IWebCache *me, WebOpt *awoz)
{
   return IWEB_AddOpt(me->piWeb, awoz);
}

static int IWebCache_RemoveOpt(IWebCache *me, int32 nOptId, int32 nIndex)
{
   return IWEB_RemoveOpt(me->piWeb, nOptId, nIndex);
}

static int IWebCache_GetOpt(IWebCache *me, int32 nOptId, int32 nIndex, WebOpt *pwo)
{
   return IWEB_GetOpt(me->piWeb, nOptId, nIndex, pwo);
}

static void IWebCache_GetResponseV(IWebCache *me, IWebResp **ppiwresp, 
                                   AEECallback *pcb, const char *cpszUrl, 
                                   WebOpt *awozList)
{
   int nErr;

   nErr = IWebCacheResp_Transaction(me, ppiwresp, pcb, cpszUrl, awozList);

   if (SUCCESS != nErr) { /* couldn't start cache transaction, punt */
      IWEB_GetResponseV(me->piWeb, ppiwresp, pcb, cpszUrl, awozList);
   }
}

static void IWebCache_GetResponse(IWebCache *me, IWebResp **ppiwresp, 
                                  AEECallback *pcb, const char *cpszUrl, ...)
{
   WebOpt *awozList = (WebOpt *)(((char *)&cpszUrl) + sizeof(cpszUrl));

   IWebCache_GetResponseV(me, ppiwresp, pcb, cpszUrl, awozList);
}

static uint32 IWebCache_SetFlags(IWebCache *me, uint32 uFlags)
{
   uint32 uRet = me->uFlags;

   me->uFlags = uFlags;

   return uRet;
}

static void IWebCache_SetLimits(IWebCache *me, uint32 uMinRAM, 
                                uint32 uMaxDisk)
{
   me->uMinRAM  = uMinRAM;
   me->uMaxDisk = uMaxDisk;
}


/*====================================================================
  IWebCacheResp stuff 
  ===================================================================*/

static void IWebCacheResp_Delete(IWebCacheResp *me)
{
   CALLBACK_Cancel(&me->cb);

   RELEASEIF(me->wri.pisMessage);

   if ((WebCacheEntry *)0 != me->pwce) {
      WebCacheEntry_Release(me->pwce);
      me->pwce = 0;
   }
   
   IWEBOPTS_Release(me->piWebOpts);
   IWebCache_Release(me->piwc);

   FREEIF(me);
}

static void IWebCacheResp_GotResp(void *p)
{
   IWebCacheResp *me = (IWebCacheResp *)p;
   AEECallback   *pcbRet;
   IWebResp      *piWebResp = 0;
   
   pcbRet = me->pcbRet;

   me->pcbRet = 0;  /* forget about callback */
   pcbRet->pfnCancel = 0; /* turn off my cancel */

   WebCacheEntry_GetRespInfo(me->pwce, &me->wri, &piWebResp);

   {
      WebOpt wo;

      /*  this is hairy..... */

      if ( ((IWebResp *)0 != piWebResp) && /* if original exists */
           
           ((0 > me->wri.nCode) || /* and internal error */

            /* or local response */
            ((SUCCESS == IWEBRESP_GetOpt(piWebResp,WEBOPT_FLAGS,0,&wo)) &&
             (0 != ((uint32)wo.pVal & WEBRESPONSE_LOCAL))))
           
           ) {

         *me->ppiwrespRet = piWebResp; /* now client owns original */
         piWebResp = 0; /* prevent releaseif below */
      } else {
         /* give caller me, otherwise caller keeps original */
         *me->ppiwrespRet = me; /* now client owns me */
         me = 0; /* prevent releaseif below */
      }
   }

   RELEASEIF(piWebResp);
   RELEASEIF(me);

   if ((PFNNOTIFY)0 != pcbRet->pfnNotify) {
      pcbRet->pfnNotify(pcbRet->pNotifyData);
   }

}

static void IWebCacheResp_CancelTransaction(AEECallback *pcb)
{
   IWebCacheResp *me = (IWebCacheResp *)pcb->pCancelData;

   pcb->pfnCancel = 0; /* turn off my cancel */
   me->pcbRet = 0;  /* forget about callback */

   IWebCacheResp_Release(me);
}

static int IWebCacheResp_Transaction(IWebCache *piwc, 
                                     IWebResp **ppiwrespRet,
                                     AEECallback *pcbRet, 
                                     const char *cpszUrl,
                                     WebOpt *awozList)
{
   IWebCacheResp *me = 0;
   int            nErr;
   IWebOpts      *piWebOpts = 0;
   WebCacheEntry *pwce = 0;

   nErr = ISHELL_CreateInstance(piwc->piShell,AEECLSID_WEBOPTS,
                                (void **)&piWebOpts);

   if (SUCCESS == nErr) { /* construct a request */
      if ((WebOpt *)0 != awozList) {
         nErr = IWEBOPTS_AddOpt(piWebOpts, awozList);
      }
   }
   
   if (SUCCESS == nErr) {
      nErr = IWebCache_GetEntry(piwc, cpszUrl, piWebOpts, &pwce);
   }

   if (SUCCESS == nErr) {
      nErr = ERR_MALLOCREC(IWebCacheResp,&me);
   }

   if (SUCCESS == nErr) {
      me->pvt = &piwc->vtWebCacheResp;

      me->uRef = 1;

      me->ppiwrespRet = ppiwrespRet; /* save return spot */
      
      CALLBACK_Cancel(pcbRet);

      pcbRet->pfnCancel   = IWebCacheResp_CancelTransaction;
      pcbRet->pCancelData = me;

      me->pcbRet = pcbRet;
      
      me->piwc = piwc;
      IWEBCACHE_AddRef(piwc); /* copy this guy */

      me->piWebOpts = piWebOpts;
      IWEBOPTS_AddRef(piWebOpts);
      
      me->pwce = pwce;
      WebCacheEntry_AddRef(pwce);
  
      WEBRESPINFO_CTOR(&me->wri);

      {
         WebOpt wo;

         if (SUCCESS == 
             IWEBOPTS_GetOpt(piWebOpts,WEBOPT_STATUSHANDLER,0,&wo)) {
            me->wcew.pfnStatusHandler = (PFNWEBSTATUS)wo.pVal;
         }

         if (SUCCESS == 
             IWEBOPTS_GetOpt(piWebOpts,WEBOPT_HEADERHANDLER,0,&wo)) {
            me->wcew.pfnHeaderHandler = (PFNWEBHEADER)wo.pVal;
         }

         if (SUCCESS == 
             IWEBOPTS_GetOpt(piWebOpts,WEBOPT_HANDLERDATA,0,&wo)) {
            me->wcew.pHandlerData = wo.pVal;
         }
      }

      CALLBACK_Init(&me->cb, IWebCacheResp_GotResp, me);
      WebCacheEntry_Wait(me->pwce,&me->wcew,&me->cb);
   }
   
   if ((WebCacheEntry *)0 != pwce) {
      WebCacheEntry_Release(pwce);
      pwce = 0;
   }

   RELEASEIF(piWebOpts); /* release and zero out piWebOpts */
   
   return nErr;
}

static uint32 IWebCacheResp_AddRef(IWebCacheResp *me)
{
   return ++me->uRef;
}
static uint32 IWebCacheResp_Release(IWebCacheResp *me)
{
   uint32 uRef = --me->uRef;

   if (0 == uRef) {
      IWebCacheResp_Delete(me);
   }

   return uRef;
}

static int IWebCacheResp_QueryInterface(IWebCacheResp *me, AEECLSID id, 
                                        void **ppo)
{
   if ((AEECLSID_QUERYINTERFACE == id) || 
       (AEECLSID_WEBRESP == id) ||
       (AEECLSID_WEBOPTS == id)) {
      *ppo = me;
      IWebCacheResp_AddRef(me);
      return SUCCESS;
   } else {
      *ppo = 0;
      return ECLASSNOTSUPPORT;
   }
}

static int IWebCacheResp_AddOpt(IWebCacheResp *me, WebOpt *awoz)
{
   (void)me,(void)awoz;
   return EFAILED;
}

static int IWebCacheResp_RemoveOpt(IWebCacheResp *me, int32 nOptId, 
                                   int32 nIndex)
{
   (void)me,(void)nOptId,(void)nIndex;
   return EFAILED;
}

static int IWebCacheResp_GetOpt(IWebCacheResp *me, int32 nOptId, 
                                int32 nIndex, WebOpt *pwo)
{
   (void)me,(void)nOptId,(void)nIndex,(void)pwo;
   return EFAILED;
}

static WebRespInfo *IWebCacheResp_GetInfo(IWebCacheResp *me)
{
   return &me->wri;
}

/*====================================================================
  WCESource stuff 
  ===================================================================*/

static int IWCESource_New(IWebCache *piwc, WebCacheEntry *pwce, void **pp)
{
   IWCESource *me = 0;
   int      nErr;

   nErr = ERR_MALLOCREC(IWCESource, &me);

   if (SUCCESS == nErr) {
      me->pvt = &piwc->vtWCESource;
      
      me->uRef = 1;

      me->pwce = pwce;
      WebCacheEntry_AddRef(pwce);

      me->piwc = piwc;
      IWebCache_AddRef(piwc);
   }

   *pp = me;
   return nErr;
}

static void IWCESource_Delete(ISource *me)
{
   /* must release cache entry before cache */
   if ((WebCacheEntry *)0 != me->pwce) {
      WebCacheEntry_Release(me->pwce);
      me->pwce = 0;
   }

   RELEASEIF(me->piwc);

   FREE(me);
}

static uint32 IWCESource_AddRef(ISource *me)
{
   return ++me->uRef;
}

static uint32 IWCESource_Release(ISource *me)
{
   uint32 uRef = --me->uRef;
   
   if (0 == uRef) {
      IWCESource_Delete(me);
   }

   return uRef;
}

static int IWCESource_QueryInterface(ISource *me, AEECLSID id, void **ppo)
{
   if ((AEECLSID_QUERYINTERFACE == id) || 
       (AEECLSID_SOURCE == id)) {
      *ppo = me;
      IWCESource_AddRef(me);
      return SUCCESS;
   } else {
      *ppo = 0;
      return ECLASSNOTSUPPORT;
   }
}

static int32 IWCESource_Read(ISource *me, char *pcBuf, int32 cbBuf)
{
   WebCacheEntry *pwce = me->pwce;
   int            nAvail;

   WebCacheEntry_ReadBody(pwce); /* kicks off read */

   nAvail = pwce->nReadSoFar - me->nReadSoFar;

   if (0 >= nAvail) { /* happens on error, wait, or end */
      me->nReadSoFar = pwce->nReadSoFar;
      return pwce->nLastRead;
   }
   
   nAvail = MIN(nAvail,cbBuf);

   MEMMOVE(pcBuf,pwce->mcBody.pData+me->nReadSoFar,nAvail);

   me->nReadSoFar += nAvail;
   
   return nAvail;
}

static void IWCESource_Readable(ISource *me, AEECallback *pcb)
{
   WebCacheEntry *pwce = me->pwce;
   int nAvail = pwce->nReadSoFar - me->nReadSoFar;

   if ((0 != nAvail) || /* we're ahead, or behind, OR.... */
       (ISOURCE_WAIT != pwce->nLastRead)) { /* we're even, but have stopped 
                                               OR have just made progress */
      ISHELL_Resume(pwce->piShell, pcb);
   } else {
      WebCacheEntry_Readable(pwce, pcb);
   }
}


