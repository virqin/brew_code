/*
  ===========================================================================

  FILE: lcgieng.c
  
  SERVICES: LCGI IWebEng
  
  GENERAL DESCRIPTION
    LCGI stands for "local CGI", this module implements parts of BREWser's 
     UI as if it were a web application.
        
  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/

#include "lcgieng.bid"     /* extension class ID */

#include "AEEWeb.h"
#include "AEEStdLib.h"

#include "AEETAPI.h" /* for LCGI_Tel() */

/*
  || my decls
*/
#include "lcgieng.h"

/*
  || ReleaseIf, etc.
*/
#include "util.h"

/*
  || bookmarks functionality 
*/
#include "bookmarks.h"


/* =========================================================================
   Types
   ========================================================================= */

/* 
   || forward decls *
*/



/* =======================================================================
   ======================================================================
   ======================================================================
   LCGIResp stuff
   ======================================================================
   ======================================================================
   ======================================================================= */

/* ======================================================================
   ======================================================================
   Goto stuff
   ======================================================================
   ====================================================================== */


#define GOTO_FILE       "goto.fdb"
#define GOTO_MAXBYTES   2048


static void LCGIResp_GotoSave(LCGIResp *me, const char *cpszUrl)
{
   FlatDB db;
   int    cbNew = STRLEN(cpszUrl) + 1;
   const  char *cpszDup;
   int    nErr;
   
   nErr = FlatDB_Ctor(&db, me->plcgie->piShell, GOTO_FILE);
   
   while ((SUCCESS == nErr) &&
          ((char *)0 != (cpszDup = FlatDB_NextRecord(&db)))) {
      
      if (!STRCMP(cpszUrl,cpszDup)) {
         FlatDB_ReplaceRecord(&db, 0);
         FlatDB_Dtor(&db);

         nErr = FlatDB_Ctor(&db, me->plcgie->piShell, GOTO_FILE);
      }
   }

   if (SUCCESS == nErr) {
      FlatDB_Rewind(&db);
      FlatDB_Truncate(&db, GOTO_MAXBYTES - cbNew);
      FlatDB_ReplaceRecord(&db, cpszUrl);
   }

   FlatDB_Dtor(&db);
}

static int LCGIResp_GotoInput(LCGIResp *me)
{
   AEEResBlob *prbGoto = 0;
   int         nGotoLen;
   int         nErr;
   AEEResBlob *prbGotoRecent = 0;
   int         nGotoRecentLen;
   FlatDB      db;
   const char *pszURL;

   nErr = IShell_LoadResBlob(me->plcgie->piShell,
                             BREWSER_RES_FILE,
                             IDX_GOTO,
                             &prbGoto,&nGotoLen);
   
   nErr = IShell_LoadResBlob(me->plcgie->piShell,
                             BREWSER_RES_FILE,
                             IDX_GOTORECENT,
                             &prbGotoRecent,&nGotoRecentLen);

   if (SUCCESS == nErr) {

      /* If DB ops fail, we might still display the go to form... */
      if (SUCCESS == FlatDB_Ctor(&db, me->plcgie->piShell, GOTO_FILE)) {
         while ((char *)0 != (pszURL = FlatDB_NextRecord(&db))) {
            int nMore;
            
            /* replace "{recent}" with recent string, gotorecent has recursive
               definition, and a "{url}" entity */
            nMore = AEEResBlob_StrRepl(&prbGoto, &nGotoLen, "recent%s\0",
                                       RESBLOB_DATA(prbGotoRecent));
            if (nMore > 0) {
               break;
            }
            
            /* replace "{url}" with URL */
            nMore = AEEResBlob_StrRepl(&prbGoto, &nGotoLen, 
                                       "url%Qs\0", pszURL);
            
            if (nMore > 0) {
               break;
            }
         }
      }

      FlatDB_Dtor(&db);
   }
   
   /* clean up */
   AEEResBlob_StrRepl(&prbGoto,&nGotoLen,"url%s\0recent%s\0","","");
   
   if (SUCCESS == nErr) {
      nErr = ISOURCEUTIL_SourceFromMemory(me->plcgie->piSourceUtil,
                                          RESBLOB_DATA(prbGoto),
                                          STRLEN(RESBLOB_DATA(prbGoto)),
                                          GET_HELPER()->free, prbGoto,
                                          &me->wri.pisMessage);
   }

   if (SUCCESS == nErr) {
      me->wri.nCode           = WEBCODE_OK;
      me->wri.lContentLength  = STRLEN(RESBLOB_DATA(prbGoto));
      me->wri.cpszContentType = STRDUP(prbGoto->szMimeType);
      prbGoto = 0; /* forget about it */
   }
   
   FREEIF(prbGoto);
   FREEIF(prbGotoRecent);
   return nErr;
}

static int LCGIResp_GotoSub(LCGIResp *me, UrlParts *pup)
{
   char *pszUrl = 0;

   {
      char  *pszFormFields = (char *)MALLOC(UP_SRCHLEN(pup));
      
      if ((char *)0 == pszFormFields) {
         return ENOMEMORY;
      }
      
      MEMCPY(pszFormFields,pup->cpcSrch+1,UP_SRCHLEN(pup)-1);
      
      IWebUtil_GetFormFields(me->plcgie->piWebUtil,
                             pszFormFields,"url\0",&pszUrl);
      
      if ((char *)0 == pszUrl) {
         me->wri.nCode = WEBCODE_NOTACCEPTABLE;
         FREEIF(pszFormFields);
         return SUCCESS;
      }

      {
         int nLen;

         /* clean up leading and trailing whitespace */
         nLen = trim(pszUrl,STRLEN(pszUrl));
         pszUrl[nLen] = 0;

         nLen++;

         /* shrink the node */
         MEMMOVE(pszFormFields,pszUrl,nLen);
         pszUrl = REALLOC(pszFormFields,nLen);
      }
   }

   {
      int   nGuessLen;
      char *pszGuess;
      
      nGuessLen = IWebUtil_GuessUrl(me->plcgie->piWebUtil,pszUrl,0,0);
      
      pszGuess = (char *)MALLOC(nGuessLen);

      if ((char *)0 != pszGuess) {
         IWebUtil_GuessUrl(me->plcgie->piWebUtil,pszUrl,pszGuess,nGuessLen);
         FREEIF(pszUrl);
         pszUrl = pszGuess;
      }
   }

   LCGIResp_GotoSave(me,pszUrl);

   LCGIResp_Header(me,"Location",pszUrl);
   
   me->wri.nCode = WEBCODE_TEMPORARYREDIRECT;

   FREEIF(pszUrl);

   return SUCCESS;
}


static int LCGIResp_Goto(LCGIResp *me, UrlParts *pup)
{
   if (0 == UP_SRCHLEN(pup)) {
      return LCGIResp_GotoInput(me);
   } else {
      return LCGIResp_GotoSub(me,pup);
   }
}

/* =======================================================================
   ======================================================================
   rest of LCGIResp stuff
   ======================================================================
   ======================================================================= */


boolean LCGIResp_FindOrMakeInvoke(LCGIResp *me, UrlParts *pup, 
                                  const char **pcpszInvoke)
{
   const char *cpszInvoke = *pcpszInvoke;
   char *pc;
   char *pszTo;
   int   nLen;
   unsigned nInvoke;

   /* see if there's an invoke part */
   nInvoke = STRTOUL(cpszInvoke,&pc,16);
   
   /* either an empty invoke (out of memory last time)
      or a good invoke */
   if (((pc == cpszInvoke) || (pc == cpszInvoke+8)) && 
       (*pc == '.')) {
      nLen = pc - cpszInvoke;

      MEMMOVE(me->szInvoke,cpszInvoke,nLen);
      me->szInvoke[nLen] = 0;

      *pcpszInvoke = pc+1;
      return TRUE;
   }

   /* else not an invoke, redirect to an invoke */
   
   pszTo = (char *)MALLOC(STRLEN(pup->cpcSchm)+9+1);

   /* couldn't generate a redirect, so just go without invoke,
      pcpszInvoke doesn't change
   */
   if ((char *)0 == pszTo) {
      me->szInvoke[0] = 0; 
      return TRUE;
   }
   
   nLen = cpszInvoke - pup->cpcSchm;

   GETRAND((byte *)&nInvoke,sizeof(nInvoke));

   MEMMOVE(pszTo,pup->cpcSchm,nLen);
   SPRINTF(pszTo+nLen,"%08x.",nInvoke);
   MEMMOVE(pszTo+nLen+9,cpszInvoke,STRLEN(cpszInvoke)+1);
         
   LCGIResp_Header(me, "Location", pszTo);
   FREE(pszTo);

   me->wri.nCode = WEBCODE_MOVEDPERMANENTLY;

   return FALSE;
}

static void LCGIResp_Dtor(LCGIResp *me)
{
   if ((AEECallback *)0 != me->pcbReturn) {
      me->pcbReturn->pfnCancel = 0; /* turn off my cancel */
      me->pcbReturn = 0;  /* forget about callback */
   }
   CALLBACK_Cancel(&me->cb); /* stops firecallback resume */

   FREEIF(me->pszReferer);
   FREEIF(me->wri.cpszContentType);
   RELEASEIF(me->plcgie);
   RELEASEIF(me->wri.pisMessage);
   RELEASEIF(me->piWeb);
}

static void LCGIResp_Delete(LCGIResp *me)
{
   LCGIResp_Dtor(me);
   FREE(me);
}

static uint32 LCGIResp_AddRef(LCGIResp *me)
{
   return ++me->uRefs;
}

static uint32 LCGIResp_Release(LCGIResp *me)
{
   uint32 uRefs = --me->uRefs;
   
   if (0 == uRefs) {
      LCGIResp_Delete(me);
   }
   return uRefs;
}

static int LCGIResp_QueryInterface(LCGIResp *me, AEECLSID id, void **ppo)
{
   if ((id == AEECLSID_QUERYINTERFACE) ||
       (id == AEECLSID_WEBRESP) || 
       (id == AEECLSID_WEBOPTS)) {
      *ppo = me;
      LCGIResp_AddRef(me);
      return SUCCESS;
   } else {
      *ppo = 0;
      return ECLASSNOTSUPPORT;
   }
}

static int LCGIResp_AddOpt(LCGIResp *me, WebOpt *apwoz)
{
   return EFAILED;
}

static int LCGIResp_RemoveOpt(LCGIResp *me, int32 nOptId, int32 nIndex)
{
   return EFAILED;
}

static int LCGIResp_GetOpt(LCGIResp *me, int32 nOptId, int32 nIndex, WebOpt *pwo)
{
   if (0 == nIndex && (WEBOPT_ANY == nOptId || WEBOPT_FLAGS == nOptId)) {
      pwo->nId = WEBOPT_FLAGS;
      pwo->pVal = (void *)WEBRESPONSE_LOCAL;
      return SUCCESS;
   }

   pwo->nId  = 0;
   pwo->pVal = 0;
   return EFAILED;
}

static WebRespInfo *LCGIResp_GetInfo(LCGIResp *me)
{
   return &me->wri;
}


static void LCGIResp_Cancel(AEECallback *pcb)
{
   LCGIResp *me = (LCGIResp *)pcb->pCancelData;

   me->pcbReturn = 0;
   pcb->pfnCancel = 0; /* turn off my cancel */
   
   LCGIResp_Release(me);
}

void LCGIResp_Header(LCGIResp *me, const char *cpszName, const char *cpszValue)
{
   GetLine gl;
   
   gl.psz = (char *)cpszValue;
   gl.nLen = STRLEN(gl.psz);
   gl.bLeftover = gl.bTruncated = FALSE;
   
   me->pfnHeaderHandler(me->pHandlerData,cpszName,&gl);
}

static void LCGIResp_DummyHeaderHandler(void *p, const char *cpszName,
                                        GetLine *pgl)
{
   (void)p,(void)cpszName,(void)pgl;
}

static int WebHeader_Parse(const char **pcpszHeader, 
                           const char *cpszName, 
                           const char **pcpszValue)
{
   WebNVPair wnvp;
   int       nNameLen = STRLEN(cpszName);

   while (ParseNVPairs((char **)pcpszHeader,"\r\n",':',
                       (WEBUTIL_PAVPF_NOTERMINATE|WEBUTIL_PAVPF_NOTRIM),
                       &wnvp, 1)) {
      const char *pc;
      
      /* wnvp values are unmolested, so gotta do some of my own parsing.. */
      if (STRNICMP(wnvp.pcName,cpszName,nNameLen) ||
          ((char *)0 == (pc = STRCHR(wnvp.pcName+nNameLen,':')))) {
      frag:
         continue;
      }
      /* make sure it's of the form NAME *<sp> <:> *<sp> VALUE */
      while (--pc >= wnvp.pcName + nNameLen) {
         if (*pc != ' ') {
            goto frag;
         }
      }

      *pcpszValue = wnvp.pcValue;

      /* pcpszHeader has been advanced... */
      return *pcpszHeader - wnvp.pcValue;
   }

   return -1;
}

boolean LCGIResp_SafeReferer(LCGIResp *me)
{
   return (((char *)0 != me->pszReferer) && 
           STRBEGINS("lcgi:",me->pszReferer));
}
                                    
static void LCGIResp_CtorZ(LCGIResp *me,
                           IWeb *piWeb,
                           LCGIEng  *plcgie,
                           IWebReq *piwreq,
                           IWebResp **ppiwresp, 
                           AEECallback *pcbReturn)
{
   /* fix up IWebResp vtbl */
   me->pvt = &me->vt;

   /* Initialize individual entries in the VTBL */
   me->vt.AddRef         = LCGIResp_AddRef;
   me->vt.Release        = LCGIResp_Release;
   me->vt.QueryInterface = LCGIResp_QueryInterface;
   me->vt.AddOpt         = LCGIResp_AddOpt;
   me->vt.RemoveOpt      = LCGIResp_RemoveOpt;
   me->vt.GetOpt         = LCGIResp_GetOpt;
   me->vt.GetInfo        = LCGIResp_GetInfo;

   me->uRefs = 1;

   me->plcgie = plcgie;
   IWEBENG_AddRef(plcgie);

   me->piWeb = piWeb;
   IWEB_AddRef(piWeb);

   me->ppiwresp = ppiwresp;


   {
      int    i;
      WebOpt wo;

      if (SUCCESS == IWEBREQ_GetOpt(piwreq,WEBOPT_HEADERHANDLER,0,&wo)) {
         me->pfnHeaderHandler = (PFNWEBHEADER)wo.pVal;
      } else {
         me->pfnHeaderHandler = LCGIResp_DummyHeaderHandler;
      }
      if (SUCCESS == IWEBREQ_GetOpt(piwreq,WEBOPT_HANDLERDATA,0,&wo)) {
         me->pHandlerData = wo.pVal;
      } else {
         me->pHandlerData = 0;
      }

      // get the Brewser* for Get/Set preferences
      IWEB_GetOpt(me->piWeb, WEBOPT_PRIVBREWSER, 0, &wo);
      me->pbsr = (Brewser *)wo.pVal;
      
      for (i = 0; 
           SUCCESS == IWEBREQ_GetOpt(piwreq,WEBOPT_HEADER,i,&wo); i++) {
         const char *cpszReferer;
         int nLen;

         nLen = WebHeader_Parse((const char **)&wo.pVal, 
                                "Referer", &cpszReferer);
         if (nLen > 0) {
            me->pszReferer = (char *)MALLOC(nLen);

            if ((char *)0 != me->pszReferer) {
               MEMMOVE(me->pszReferer,cpszReferer,nLen);
               nLen = trim(me->pszReferer,nLen);
               me->pszReferer[nLen] = 0;
            }
            break;
         }
      }
   }
   
   /* set up callback */
   CALLBACK_Cancel(pcbReturn);
   pcbReturn->pfnCancel = LCGIResp_Cancel;
   pcbReturn->pCancelData = me;
   me->pcbReturn = pcbReturn;
}

static void LCGIResp_FireCallback(void *p)
{
   LCGIResp *me = (LCGIResp *)p;
   AEECallback *pcb = me->pcbReturn;

   me->pcbReturn = 0;  /* forget about callback */

   *me->ppiwresp = me; /* now client owns me */

   pcb->pfnCancel = 0; /* turn off my cancel */

   if ((PFNNOTIFY)0 != pcb->pfnNotify) {
      pcb->pfnNotify(pcb->pNotifyData);
   }
}


static int LCGIResp_Tel(LCGIResp *me, UrlParts *pup)
{
   int    nErr;
   ITAPI *piTAPI = 0;
   char  *pszNumber = 0;

   nErr = ISHELL_CreateInstance(me->plcgie->piShell, AEECLSID_TAPI, 
                                (void **)&piTAPI);

   if (SUCCESS == nErr) {
      nErr = ERR_STRDUP(pup->cpcPath,&pszNumber);

      if (SUCCESS == nErr) {
         char *pc;

         pc = STRCHREND(pszNumber,';');
         *pc = '\0';
         
         nErr = ITAPI_MakeVoiceCall(piTAPI,pszNumber,0);

         if (SUCCESS == nErr) {
            me->wri.nCode = WEBCODE_NOCONTENT; /* 204 */
         }
      }
   }
   
   RELEASEIF(piTAPI);
   FREEIF(pszNumber);

   return nErr;
}

static int LCGIResp_Transaction(LCGIEng *plcgie, 
                                IWeb *piWeb, IWebReq *piwreq, 
                                AEECallback *pcb, IWebResp **ppiwresp)
{
   UrlParts     up;
   int          nErr;
   LCGIResp    *me;

   nErr = ERR_MALLOCREC(LCGIResp,&me);

   if (SUCCESS != nErr) {
      return nErr;
   }
   
   LCGIResp_CtorZ(me,piWeb,plcgie,piwreq,ppiwresp,pcb);

   IWEBUTIL_ParseUrl(plcgie->piWebUtil,IWEBREQ_GetUrl(piwreq),&up);

   if (CSTRLEN("lcgi:") == UP_SCHMLEN(&up) || 
       !STRNICMP(up.cpcSchm, "lcgi:", CSTRLEN("lcgi:"))) {
      /* handle lcgi:data?xxx */
      if (STRBEGINS("bkmk.", up.cpcPath)) {
      
         nErr = LCGIResp_Bookmarks(me, &up);

      } else if (STRBEGINS("cfg.", up.cpcPath)) {
      
         nErr = LCGIResp_Config(me, &up);
      
      } else if (UP_PATHLEN(&up) == CSTRLEN("goto") && 
                 !MEMCMP("goto",up.cpcPath,CSTRLEN("goto"))) {
         nErr = LCGIResp_Goto(me,&up);
      } else {
         nErr = WEB_ERROR_BADURL;
      }
   } else if (CSTRLEN("tel:") == UP_SCHMLEN(&up) &&
              !STRNICMP(up.cpcSchm, "tel:", CSTRLEN("tel:"))) {

      nErr = LCGIResp_Tel(me, &up);

   } else {

      nErr = WEB_ERROR_UNSUPSCHEME;

   }

   if (SUCCESS != nErr) {
      LCGIResp_Release(me);
      me = 0;
   } else {
      CALLBACK_Init(&me->cb,LCGIResp_FireCallback,me);
      ISHELL_Resume(plcgie->piShell,&me->cb);
   }
   
   return nErr;
}

/* =======================================================================
   LCGIEng 
   ======================================================================= */

static void LCGIEng_Delete(LCGIEng *me)
{
   RELEASEIF(me->piSourceUtil);
   RELEASEIF(me->piWebUtil);
   RELEASEIF(me->piShell);
   RELEASEIF(me->pim);
   
   FREE(me);
}


static uint32 LCGIEng_AddRef(LCGIEng *me)
{
   return ++me->uRefs;
}

static uint32  LCGIEng_Release(LCGIEng *me)
{
   unsigned uRefs = --me->uRefs;

   if (0 == uRefs) {
      LCGIEng_Delete(me);
   }
   return uRefs;
}

static int LCGIEng_QueryInterface(LCGIEng *me, AEECLSID id, void **ppo)
{
   if ((id == AEECLSID_QUERYINTERFACE) ||
       (id == AEECLSID_WEBENG) || 
       (id == AEECLSID_LCGIENG)) {
      *ppo = me;
      LCGIEng_AddRef(me);
      return SUCCESS;
   } else {
      *ppo = 0;
      return ECLASSNOTSUPPORT;
   }
}

static int LCGIEng_Transaction(LCGIEng *me, IWeb *piWeb, IWebReq *piwreq, 
                               AEECallback *pcb, IWebResp **ppiwresp)
{
   return LCGIResp_Transaction(me, piWeb,piwreq, pcb, ppiwresp);
}

int LCGIEng_New(IModule *pim,IShell *piShell, AEECLSID clsid, IWebEng **pp)
{
   LCGIEng     *me = 0;
   IWebUtil    *piWebUtil = 0;
   ISourceUtil *piSourceUtil = 0;
   int          nErr;

   (void)clsid;

   nErr = ISHELL_CreateInstance(piShell, AEECLSID_WEBUTIL,
                                (void **)&piWebUtil);
   
   if (SUCCESS == nErr) {
      nErr = ISHELL_CreateInstance(piShell,AEECLSID_SOURCEUTIL,
                                   (void **)&piSourceUtil);
   }
      
   if (SUCCESS == nErr) {
      
      nErr = ERR_MALLOCREC(LCGIEng,&me);
      
      if (SUCCESS == nErr) {
         me->pvt = &me->vt;

         me->vt.AddRef         = LCGIEng_AddRef;
         me->vt.Release        = LCGIEng_Release;
         me->vt.QueryInterface = LCGIEng_QueryInterface;
         me->vt.Transaction    = LCGIEng_Transaction;

         
         me->uRefs = 1;
         
         me->pim = pim;
         IMODULE_AddRef(pim);
         
         me->piShell = piShell;
         ISHELL_AddRef(piShell);
         
         me->piSourceUtil = piSourceUtil;
         ISOURCEUTIL_AddRef(piSourceUtil);
         
         me->piWebUtil = piWebUtil;
         IWEBUTIL_AddRef(piWebUtil);
         
      } else {
         nErr = ENOMEMORY;
      }
   }

   RELEASEIF(piSourceUtil);
   RELEASEIF(piWebUtil);
   
   *pp = me;
   
   return nErr;
}




