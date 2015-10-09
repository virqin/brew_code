/*
  ===========================================================================

  FILE:  brewser.c
  
  SERVICES:  
    A BREW browser.
  
  GENERAL DESCRIPTION:
    BREWser is a World-Wide Web browser implemented using built-in features
     of BREW.
  
  REVISION HISTORY: 
    Thu Jul 05 13:54:14 2001: Created

  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/
#include "AEEStdLib.h"     /* for WSTRLEN, etc */
#include "AEEText.h"       /* AEE Text control */
#include "AEEWeb.h"        /* AEE Web interface */
#include "AEESSL.h"        /* AEE SSL interface */
#include "AEEHtmlViewer.h" /* AEE Html interface */

/* not necessary if compiling against a BREW SDK that defines this */
#ifndef HVP_LOADALLIMAGES
#define HVP_LOADALLIMAGES     0x00000008
#endif /* #ifndef HVP_LOADALLIMAGES */

/*
  || shared brewser stuff
*/
#include "brewser.h"

/*
  || webcache stuff
*/
#include "webcache.h"

/* 
   || brewser module unofficial ClassID, kinda spells "brewser"
 */
#include "brewser.bid"

/*
  || utility stuff
*/
#include "util.h"

/*
  || prefs manager
*/
#include "prefmgr.h"

/* 
   || slider stuff helps me with my toolbar
*/
#include "slider.h"

/* 
   || tooltip stuff helps me schedule/draw tooltips 
*/
#include "tooltip.h"

/* 
   || statusbox stuff abstracts the messiness of optional/animated image
*/
#include "statusbox.h"

/*
  || headermaker collects IWeb headers...
*/
#include "headermaker.h"

/*
  || refresher helps with the "Refresh" HTTP header
*/
#include "refresher.h"


#define FARF_CERTS        1
#define FARF_COOKIES      1
#define FARF_COOKIES_DATE 0


/* =======================================================================
   Cookies stuff
   ======================================================================= */

typedef struct Cookie Cookie;
struct Cookie
{
   Cookie     *pNext;       /* next in RAM */

   /* parsed out values */
   uint32      tExpires;   /* CDMA date of expiry, -1 if never expires */
   uint32      tLastUsed;  /* CDMA date of last use */
   uint32      ulFlags;    /* flags */
   int         nSize;      /* my size */

#define CKF_SECURE   0x01  /* secure? */
#define CKF_DISCARD  0x02  /* discard (never saved to disk, but used here) */

   /* helper pointers */
   char       *pszValue;    /* value part, pointer into over-allocated
                               szCookie below */
   char       *pszPath;     /* path part, pointer into over-allocated
                               szCookie below */
   char       *pszDomain;   /* domain (host.domain.com or .domain.com) 
                               pointer into over-allocated
                               szCookie below */

   char        szCookie[1];  /* DUP of FlatDB data, over-allocated */
};

typedef struct CookieMgr
{
   Cookie      *pCkList;   /* cookies in RAM */

   unsigned     bDirty:1;  /* need to write ourselves out, we're different
                              from what's on disk this happens when:
                              
                              1) we get a new persistent cookie
                              2) we get a cookie that overrides one of 
                              our persistent cookies
                              3) we delete a persistent cookie in ClearAll()
                              
                              we don't get dirty when persistent cookies 
                              expire, because they're always expired
                           */

   int          nMinSize;  /* minimum amount of data to keep, even when 
                              culled, if zero, cull all to satisfy ram */

   AEECallback  cbLowRAM;

   IWebUtil    *piWebUtil;
   IShell      *piShell;
   
} CookieMgr;

static void Cookie_Delete(Cookie **pme)
{
   Cookie *me = *pme;
   
   *pme = me->pNext;

   FREE(me);
}

static int Cookie_New(Cookie **ppCkList, WebNVPair *pwnvp,
                      const char *cpcDomain, int nDomainLen,
                      const char *cpcPath, int nPathLen,
                      uint32 tExpires, uint32 ulFlags)
{
   int nNameLen = STRLEN(pwnvp->pcName);
   int nValueLen = STRLEN(pwnvp->pcValue);
   int nSize;

   Cookie *me = MALLOCREC_EX(Cookie,
                             nSize = 
                             (nNameLen+ /* me->dbr already has null-term */
                              nValueLen+1+
                              nDomainLen+1+
                              nPathLen+1));
   
   if ((Cookie *)0 == me) {
      return ENOMEMORY;
   }
   me->nSize = nSize;
   me->tExpires = tExpires;
   me->tLastUsed = GETTIMESECONDS();
   me->ulFlags = ulFlags;

   MEMMOVE(me->szCookie,pwnvp->pcName,nNameLen);

   me->pszValue = me->szCookie + nNameLen + 1;
   MEMMOVE(me->pszValue,pwnvp->pcValue,nValueLen);

   me->pszDomain = me->pszValue + nValueLen + 1;
   MEMMOVE(me->pszDomain,cpcDomain,nDomainLen);
   
   me->pszPath = me->pszDomain + nDomainLen + 1;
   MEMMOVE(me->pszPath,cpcPath,nPathLen);

   me->pNext = *ppCkList;
   *ppCkList = me;

   return SUCCESS;
}


static int Cookie_Deserialize(const char *cpszBuf, Cookie **ppCk)
{
   Cookie *me;
   int     nBufLen = STRLEN(cpszBuf);
   int     nFields;
   
   me = MALLOCREC_EX(Cookie,nBufLen);

   if ((Cookie *)0 == me) {
      return ENOMEMORY;
   }

   /* read record into myself */
   MEMMOVE(me->szCookie,cpszBuf,nBufLen);
   
   nFields = strchop(me->szCookie,"\001");

   if ((7 == nFields) &&
       parseuint32(strchopped_nth(me->szCookie,4),16,&me->tExpires) &&
       parseuint32(strchopped_nth(me->szCookie,5),16,&me->tLastUsed) &&
       parseuint32(strchopped_nth(me->szCookie,6),16,&me->ulFlags)) {
      
      if (me->tExpires > GETTIMESECONDS()) {

         me->pszValue  = strchopped_nth(me->szCookie,1);
         me->pszDomain = strchopped_nth(me->szCookie,2);
         me->pszPath   = strchopped_nth(me->szCookie,3);

         me->pNext = *ppCk;
         *ppCk = me;
         return SUCCESS;
      } else { /* else expired */
         FARF(COOKIES,("cookie %s is expired",cpszBuf));
      }
   } else {
      FARF(COOKIES,("cookie %s parse error",cpszBuf));
   }

   FREE(me);
   return EFAILED;
}

static uint32 IFile_WriteSzField(IFile *pif, const char *cpsz, char cTerm)
{
   return (IFILE_Write(pif,cpsz,STRLEN(cpsz)) + 
           IFILE_Write(pif,&cTerm,1));
}

static int Cookie_Serialize(Cookie *me, IFile *piFile)
{
   if (CKF_DISCARD & me->ulFlags) {
      return SUCCESS;
   }
   
   IFILE_Seek(piFile, _SEEK_END, 0);
   
   IFile_WriteSzField(piFile, me->szCookie, '\001'); 
   IFile_WriteSzField(piFile, me->pszValue, '\001'); 
   IFile_WriteSzField(piFile, me->pszDomain,'\001');
   IFile_WriteSzField(piFile, me->pszPath,  '\001');
   
   {
      char sz[9];

      SNPRINTF(sz,sizeof(sz),"%x",me->tExpires);
      IFile_WriteSzField(piFile,sz,'\001');
      
      SNPRINTF(sz,sizeof(sz),"%x",me->tLastUsed);
      IFile_WriteSzField(piFile,sz,'\001');
      
      SNPRINTF(sz,sizeof(sz),"%x",me->ulFlags);
      IFile_WriteSzField(piFile,sz,'\000');
   }
   
   return SUCCESS;
}

#define COOKIES_FILE "cookies.fdb"

static void CookieMgr_ClearAll(CookieMgr *me)
{
   Cookie *pCk;

   while ((Cookie *)0 != (pCk = me->pCkList)) {
      /* deleting a persistent cookie, am dirty */
      me->bDirty = me->bDirty || !(pCk->ulFlags & CKF_DISCARD);

      Cookie_Delete(&me->pCkList);
   }
}

static boolean CookieMgr_CullExpired(CookieMgr *me)
{
   Cookie *pCk, **lfp;
   uint32  tNow;
   boolean bCulled = FALSE;

   tNow = GETTIMESECONDS();
   lfp = &me->pCkList; 
      
   while ((Cookie *)0 != (pCk = *lfp)) {
      if (pCk->tExpires < tNow) { /* where t is now */
         FARF(COOKIES,("cookie %s is expired",pCk->szCookie));
         Cookie_Delete(lfp);
         bCulled = TRUE;
         continue;
      }
      lfp = &(pCk)->pNext;
   }

   return bCulled;
}

static boolean CookieMgr_CullLRU(CookieMgr *me)
{
   Cookie  *pCk, **lfp;
   int      nSize;
   Cookie **lfpLRU;
   uint32   tLRU;
      
   /* latest that coulda been used is now */
   tLRU = GETTIMESECONDS();

   /* tally size *and* find oldest */
   for (nSize = 0, lfpLRU = 0, lfp = &me->pCkList;
        (Cookie *)0 != (pCk = *lfp); 
        nSize += pCk->nSize, lfp = &pCk->pNext) {
         
      /* if this cookie was last used before my oldest.. */
      if (pCk->tLastUsed < tLRU) {
         tLRU = pCk->tLastUsed;
         lfpLRU = lfp;
      }
   }

   /* found someone to cull, and culling them stays above minimum */
   if (((Cookie **)0 != lfpLRU) &&
       ((nSize - (*lfpLRU)->nSize) > me->nMinSize)) {
      FARF(COOKIES,("cookie %s culled",(*lfpLRU)->szCookie));
      Cookie_Delete(lfpLRU);
      return TRUE;
   }
   return FALSE;
}

static void CookieMgr_LowRAM(void *p)
{
   CookieMgr *me = (CookieMgr *)p;
   
   if (CookieMgr_CullExpired(me) || CookieMgr_CullLRU(me)) {
      ISHELL_RegisterSystemCallback(me->piShell,&me->cbLowRAM,
                                    AEE_SCB_LOW_RAM);
   }
}

static void CookieMgr_CtorZ(CookieMgr *me, IShell *piShell, 
                            IWebUtil *piWebUtil, int nMinSize)
{
   FlatDB fdb;

   me->nMinSize = nMinSize;
   me->piWebUtil = piWebUtil;
   IWEBUTIL_AddRef(piWebUtil);
   
   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   CALLBACK_Init(&me->cbLowRAM,CookieMgr_LowRAM,me);

   if (SUCCESS == FlatDB_Ctor(&fdb, piShell, COOKIES_FILE)) {
      const char *cpszData;
      while ((const char *)0 != (cpszData = FlatDB_NextRecord(&fdb))) {
         Cookie_Deserialize(cpszData,&me->pCkList);
      }
   }

   FlatDB_Dtor(&fdb);
}

static void CookieMgr_Persist(CookieMgr *me)
{
   Cookie *pCk, **lfp;
   uint32  tNow;
   FlatDB  fdb;
   
   tNow = GETTIMESECONDS();

   if (SUCCESS == FlatDB_Ctor(&fdb, me->piShell, COOKIES_FILE)) {
      FlatDB_Truncate(&fdb,0);
      FlatDB_ReplaceRecord(&fdb,0); /* force a write */
      FlatDB_Dtor(&fdb);
   }
   
   if (SUCCESS == FlatDB_Ctor(&fdb, me->piShell, COOKIES_FILE)) {
      lfp = &me->pCkList; 

      while ((Cookie *)0 != (pCk = *lfp)) {
         if (pCk->tExpires < tNow) {
            Cookie_Delete(lfp);
            continue;
         }
         
         if (!(pCk->ulFlags & CKF_DISCARD)) {
            Cookie_Serialize(pCk, fdb.pf);
         }
         
         lfp = &(pCk)->pNext;
      }
   }
   
   FlatDB_Dtor(&fdb);
}


static void CookieMgr_Dtor(CookieMgr *me)
{
   CALLBACK_Cancel(&me->cbLowRAM);

   if (me->bDirty) {
      CookieMgr_Persist(me);
   }
   while ((Cookie *)0 != me->pCkList) {
      Cookie_Delete(&me->pCkList);
   }
   RELEASEIF(me->piShell);
   RELEASEIF(me->piWebUtil);
   ZEROAT(me);
}


static boolean Cookie_Supersedes(Cookie *me, const char *cpszName,
                                 const char *cpcDomain, int nDomainLen,
                                 const char *cpcPath, int nPathLen)
{
   return !STRICMP(me->szCookie,cpszName) &&
      (STRLEN(me->pszDomain) == (unsigned)nDomainLen) &&
      !STRNICMP(me->pszDomain,cpcDomain,nDomainLen) &&
      (STRLEN(me->pszPath) == (unsigned)nPathLen) &&
      !STRNCMP(me->pszPath,cpcPath,nPathLen);
}

static int CookieMgr_SetCookie(CookieMgr *me, char *pszSetCookie, 
                               const char *cpszUrl)
{
   WebNVPair   wnvpCookie;
   const char *cpszPath = 0;
   const char *cpszDomain = 0;
   const char *cpszMaxAge = 0;
   char       *pszExpires = 0;
   uint32      tExpires = -1;
   uint32      ulFlags = 0;
   UrlParts    up;
   int         nDomainLen;
   int         nHostLen;
   int         nPathLen;


   if (((char *)0 == pszSetCookie) || 
       (0 == ParseNVPairs(&pszSetCookie, ";", '=', 0, &wnvpCookie, 1))) {
      return EBADPARM; /* no NAME=VALUE */
   }
   
   /* no value part, so take name as value */
   if (wnvpCookie.pcValue == wnvpCookie.pcName+STRLEN(wnvpCookie.pcName)) {
      char *pc = wnvpCookie.pcValue;
      wnvpCookie.pcValue = wnvpCookie.pcName;
      wnvpCookie.pcName = pc;
   }
   
   {
      WebNVPair wnvp;
      
      while (0 != ParseNVPairs(&pszSetCookie, ";", '=', 0, &wnvp, 1)) {
         if (((char *)0 == cpszDomain) && 
             !STRICMP(wnvp.pcName,"Domain")) {
            cpszDomain = wnvp.pcValue;
         } else if (((char *)0 == cpszPath) && 
                    !STRICMP(wnvp.pcName,"Path")) {
            cpszPath = wnvp.pcValue;
         } else if (((char *)0 == cpszMaxAge) && 
                    !STRICMP(wnvp.pcName,"Max-Age")) {
            cpszMaxAge = wnvp.pcValue;
         } else if (((char *)0 == pszExpires) && 
                    !STRICMP(wnvp.pcName,"Expires")) {
            pszExpires = wnvp.pcValue;
         } else if (!STRCMP(wnvp.pcName,"Secure")) {
            ulFlags |= CKF_SECURE;
         } else if (!STRCMP(wnvp.pcName,"Discard")) {
            ulFlags |= CKF_DISCARD;
         }
      }
   }

   if ((char *)0 != cpszMaxAge) {
      if (!parseuint32(cpszMaxAge,0,&tExpires) || (0 > (long)tExpires)) {
         FARF(COOKIES,("cookie %s, bad max-age",wnvpCookie.pcName));
         tExpires = -1;
      } else {
         tExpires += GETTIMESECONDS();
         /* prefer max-age over expires*/
         pszExpires = 0;
      }
   }
   
   if ((char *)0 != pszExpires) {
      tExpires = WebDate_Parse(pszExpires);
      if (-1 == tExpires) {
         FARF(COOKIES,("cookie %s, bad expires",wnvpCookie.pcName));
      }
   }

   if (-1 == tExpires) {
      ulFlags |= CKF_DISCARD;
   }
   
   IWEBUTIL_ParseUrl(me->piWebUtil,cpszUrl,&up);
      
   if ((char *)0 != cpszPath) {
      if (!STRBEGINS(cpszPath, up.cpcPath)) {
         FARF(COOKIES,("rejecting cookie %s, path %s not a prefix of %s",
                       wnvpCookie.pcName,cpszPath,cpszUrl));
         return EBADPARM;
      }
      nPathLen = STRLEN(cpszPath);
   } else {
      char *pcSlash;

      nPathLen = UP_PATHLEN(&up);
      cpszPath = up.cpcPath;
      
      pcSlash = MEMRCHR(cpszPath,'/',nPathLen);
      if ((char *)0 != pcSlash) {
         nPathLen = (pcSlash - cpszPath) + 1;
      }
   }

   if ((char *)0 != cpszDomain) {
      
      if ('.' == cpszDomain[0]) {
         cpszDomain++;
      }

      if (((char *)0 == STRCHR(cpszDomain,'.')) && 
          STRCMP(cpszDomain,"local")) { 

         FARF(COOKIES,("rejecting cookie %s, domain %s no embedded dot",
                       wnvpCookie.pcName,cpszDomain));
         
         
         /* no embedded dot and not ".local" */
         return EBADPARM;
      }
      
      nDomainLen = STRLEN(cpszDomain);
      nHostLen = UP_HOSTLEN(&up);
      
      /* domain must be shorter than or the same length as host */
      if ((nDomainLen > nHostLen) ||
          /* domain must match after a '.' in host if it's shorter
             (cpcPort == cpcHost+nHostLen) */
          ((nDomainLen != nHostLen) && 
           ('.' != up.cpcPort[-(nDomainLen+1)])) ||
          
          /* domain must match end of host(cpcPort == cpcHost+nHostLen) */
          STRNICMP(cpszDomain,up.cpcPort-nDomainLen,nDomainLen) ||
          
          /* domain must match the whole "domain" part of a hostname,
             see section 3.3.2 of rfc2965, part about:
             
             * The request-host is a HDN (not IP address) and has
             the form HD, where D is the value of the Domain
             attribute, and H is a string that contains one or
             more dots.  
          */
          ((nDomainLen < nHostLen) && 
           MEMCHR(up.cpcHost,'.',nHostLen-nDomainLen-1))) {
         
         FARF(COOKIES,("rejecting cookie %s, domain %s is outside %s",
                       wnvpCookie.pcName,cpszDomain,cpszUrl));
         return EBADPARM;
      }
   } else {
      nDomainLen = UP_HOSTLEN(&up);
      cpszDomain = up.cpcHost;
   }

   // ASSERT(all cookie values valid)

   {
      Cookie  **lfp;
      Cookie  *pCk;
      
      for (lfp = &me->pCkList; (Cookie *)0 != (pCk = *lfp);
           lfp = &(*lfp)->pNext) {
         if (Cookie_Supersedes(pCk,wnvpCookie.pcName,
                               cpszDomain,nDomainLen,
                               cpszPath,nPathLen)) {
            /* I'm dirty if I'm superseding a persistent cookie */
            me->bDirty = me->bDirty || !(pCk->ulFlags & CKF_DISCARD);
            Cookie_Delete(lfp);
            break;
         }
      }

      Cookie_New(&me->pCkList,&wnvpCookie,
                 cpszDomain,nDomainLen,
                 cpszPath,nPathLen,tExpires,ulFlags);
   }

   /* persistent cookie added, am dirty, sooo dirty */
   me->bDirty = me->bDirty || !(ulFlags & CKF_DISCARD);   

   if (!CALLBACK_IsQueued(&me->cbLowRAM)) {
      ISHELL_RegisterSystemCallback(me->piShell,&me->cbLowRAM,AEE_SCB_LOW_RAM);
   }

   FARF(COOKIES,("good cookie!"));

   return SUCCESS;
}


static boolean Cookie_ShouldGo(Cookie *me,UrlParts *pup)
{
   int nDomainLen = STRLEN(me->pszDomain);

   /* my domain must be shorter than or the same length as host */
   if ((nDomainLen > UP_HOSTLEN(pup)) ||
       /* my domain must match after a '.' in host if it's shorter
          (cpcPort == cpcHost+nHostLen) */
       ((nDomainLen != UP_HOSTLEN(pup)) && 
        ('.' != pup->cpcPort[-(nDomainLen+1)])) ||
       /* my domain must match end of host(cpcPort == cpcHost+nHostLen) */
       STRNICMP(me->pszDomain,pup->cpcPort-nDomainLen,nDomainLen) ||
       /* if I'm secure, the URL should be too (only understand https) */
       ((me->ulFlags & CKF_SECURE) && 
        (CSTRLEN("https:") != UP_SCHMLEN(pup) || 
         MEMCMP(pup->cpcSchm,"https:",CSTRLEN("https:"))))) {
      return FALSE;
   }

   return STRBEGINS(me->pszPath, pup->cpcPath);
}

static int CookieMgr_GetCookies(CookieMgr *me, UrlParts *pup, 
                                char *pBuf, int nLen)
{
   Cookie    *pCk;
   Cookie   **lfp;
   BufBound   bb;
   int        nCookies;
   uint32     ulNow = GETTIMESECONDS();

   BufBound_Init(&bb, pBuf, nLen);

   BufBound_Puts(&bb,"Cookie: ");

   nCookies = 0, lfp = &me->pCkList;

   while ((Cookie *)0 != (pCk = *lfp)) {

      if (pCk->tExpires < ulNow) {
         FARF(COOKIES,("cookie %s is expired",
                       pCk->szCookie[0]?pCk->szCookie:pCk->pszValue));
         Cookie_Delete(lfp);
         continue;
      }

      if (Cookie_ShouldGo(pCk,pup)) {
         if (0 != nCookies) {
            BufBound_Puts(&bb,"; ");
         }
         if ('\0' != pCk->szCookie[0]) {
            BufBound_Puts(&bb,pCk->szCookie);
            BufBound_Putc(&bb,'=');
         }
         BufBound_Puts(&bb,pCk->pszValue);
         ++nCookies;
      }

      lfp = &(pCk)->pNext;
   }

   if (0 != nCookies) {
      BufBound_Write(&bb,"\r\n",3);
      return BUFBOUND_WROTE(&bb,pBuf);
   } else {
      return 0;
   }
}

/* =======================================================================
   =======================================================================
   Stuff that should be in BREW 
   =======================================================================
   ======================================================================= */
static int IHtmlViewer_GetAttrValue(IHtmlViewer *me,
                                    int32 nElemLoc,
                                    const char *cpszAttr,
                                    char **ppszValue)
{
   int   nSize;
   char *psz;
   
   nSize = IHTMLVIEWER_GetAttrValue(me,nElemLoc,cpszAttr,0,0);
   if (nSize <= 0) {
      return EBADITEM;
   }

   psz = (char *)REALLOC(*ppszValue,nSize);
   if ((char *)0 == psz) {
      return ENOMEMORY;
   }

   IHTMLVIEWER_GetAttrValue(me,nElemLoc,cpszAttr,psz,nSize);

   *ppszValue = psz;
   return SUCCESS;
}


static int IHtmlViewer_GetElemText(IHtmlViewer *me, 
                                   const char *cpszElem, int nIndex, 
                                   char **ppszText)
{
   int   nSize;
   int   nIdx;
   char *psz;

   nIdx = IHTMLVIEWER_FindElem(me,cpszElem,nIndex);

   if (nIdx < 0) {
      return EBADITEM;
   }
   
   nSize = IHTMLVIEWER_GetElemText(me,nIdx,0,0);
   if (nSize <= 0) {
      return EBADITEM;
   }

   psz = (char *)REALLOC(*ppszText,nSize);
   if ((char *)0 == psz) {
      return ENOMEMORY;
   }

   IHTMLVIEWER_GetElemText(me,nIdx,psz,nSize);
   
   *ppszText = psz;
   return SUCCESS;
}

/*
  ||
  || A "ProxyHost" is a comma and exclamation-point separated list of 
  ||    host specs of the form:
  ||
  ||   .x.com,.z.com!host,.sub!host
  ||
  || The syntax is similar to that of tcpd hosts.allow, but "!" is used
  ||    instead of "EXCEPT".  The above ProxyHost spec says, in English:
  ||
  || "HOSTS IN x.com OR IN z.com, EXCEPT host.[xz].com OR HOSTS 
  ||      IN sub.[xz].com, EXCEPT host.sub.[xz].com"
  ||
  || For example, were the following hosts tested against the above 
  ||    ProxyHost:
  ||
  || foo.x.com      ===> TRUE  (IN x.com)
  || host.x.com     ===> FALSE (IN x.com, EXECPT host.x.com)
  || foo.z.com      ===> TRUE  (IN z.com)
  || foo.sub.z.com  ===> FALSE (IN z.com, EXCEPT IN sub.z.com)
  || host.sub.z.com ===> TRUE  (IN z.com, EXCEPT IN sub.z.com, 
  ||                               EXCEPT host.sub.z.com)
  ||
  || Notes:
  ||   An empty ProxyHost is a special case that matches everything 
  ||      (same as ".", (IN ""))
  || 
  ||   "!" takes precedence over ",", so "!" includes everything to the
  ||      next "!"
  || 
  || 
*/
static boolean Proxy_MatchHost(const char *cpcProxyHost, int nProxyHostLen,
                               const char *cpcHost, int nHostLen)
{
   boolean     bRet = FALSE;
   boolean     bSense = TRUE;
   int         nChunkLen;

   for (nChunkLen = MEMCHREND(cpcProxyHost,'!',nProxyHostLen) - cpcProxyHost;
        nProxyHostLen >= 0;  /* examine empty chunks */
        bSense = !bSense,
           cpcProxyHost += nChunkLen + 1,
           nProxyHostLen -= (nChunkLen + 1), /* may result in -1 len... */
           nChunkLen = MEMCHREND(cpcProxyHost,'!',MAX(0,nProxyHostLen)) - cpcProxyHost) {
      const char *cpcList = cpcProxyHost; /* we destroy these */
      int         nListLen = nChunkLen;   /* we destroy these */
      int         nHostChunkLen;
      
      if (nListLen == 0) { /* match */
         bRet = bSense;
         continue;
      }

      for (nHostChunkLen = MEMCHREND(cpcList,',',nListLen) - cpcList;
           nHostChunkLen > 0; 
           cpcList += (nHostChunkLen + 1), 
              nListLen -= (nHostChunkLen + 1),
              nHostChunkLen = MEMCHREND(cpcList,',',MAX(0,nListLen)) - cpcList) {

         if ((nHostLen == nHostChunkLen) && /* same length */
             !STRNICMP(cpcHost,cpcList,nHostLen)) { /* same string */
            break;
         }
         
         if (cpcList[0] != '.') { /* not ".something", keep looking */
            continue;
         }
         
         if (((nHostLen > nHostChunkLen) &&  /* host long enough */
              !STRNICMP(cpcHost+nHostLen-nHostChunkLen,cpcList,nHostChunkLen)) || /* ends with hostchunk */
             ((nHostLen == (nHostChunkLen - 1)) && /* not quite long enough */
              !STRNICMP(cpcHost,cpcList+1,nHostLen))) { /* ends with, kinda */
            break;
         }
      }

      if (nHostChunkLen > 0) { /* matched whole or end part */
         bRet = bSense;
         nHostLen -= nHostChunkLen;
      }

   }

   return bRet;
}

static boolean Proxy_MatchSpec(UrlParts *pupSpec, UrlParts *pupUrl)
{
   if (((UP_SCHMLEN(pupSpec) != 2) || STRNICMP(pupSpec->cpcSchm,"*:",2)) &&
       ((UP_SCHMLEN(pupSpec) != UP_SCHMLEN(pupUrl)) ||
        STRNICMP(pupSpec->cpcSchm,pupUrl->cpcSchm,UP_SCHMLEN(pupUrl)))) {
      return FALSE;
   }
   /* scheme matched, same as:
      if (((UP_SCHMLEN(pupSpec) == 2) && !STRNICMP(pupSpec->cpcSchm,"*:",2)) ||
          ((UP_SCHMLEN(pupSpec) == UP_SCHMLEN(pupUrl)) &&
           !STRNICMP(upSpec.cpcSchm,pupUrl->cpcSchm,UP_SCHMLEN(pupUrl)))) {
      }
   */

   /* only match the first failover host, ignore port numbers */
   return Proxy_MatchHost(pupSpec->cpcHost, UP_HOSTLEN(pupSpec), 
                          pupUrl->cpcHost, 
                          (STRCHRSEND(pupUrl->cpcHost,"/#?:,") - pupUrl->cpcHost));

}

static int IWebUtil_GetProxyUrl(IWebUtil *me, IWebOpts *piwo, 
                                const char *cpszUrl, const char **pcpszPath)
{
   UrlParts    upUrl;
   int         nErr;
   const char *cpszSpec = 0;
   int         i;
   WebOpt      wo;

   (void)me;

   *pcpszPath = 0;

   nErr = IWEBUTIL_ParseUrl(me,cpszUrl,&upUrl);
   
   if (SUCCESS != nErr) { 
      return nErr;
   }

   for (i = 0; 
        SUCCESS == (nErr = IWEBOPTS_GetOpt(piwo,WEBOPT_PROXYSPEC,i,&wo));
        i++) {

      UrlParts upSpec;
         
      nErr = IWEBUTIL_ParseUrl(me,(const char *)wo.pVal,&upSpec);
      if (SUCCESS != nErr) {
         return nErr;
      }
         
      if (Proxy_MatchSpec(&upSpec,&upUrl)) {
         cpszSpec = (const char *)wo.pVal;
         break;
      }
   }
   
   if (SUCCESS == nErr) { /* found a spec */
      UrlParts upSpec;
      
      nErr = IWEBUTIL_ParseUrl(me,cpszSpec,&upSpec);
      
      if (SUCCESS == nErr) {
         if (UP_PATHLEN(&upSpec) > 1) {
            *pcpszPath = upSpec.cpcPath + 1; /* proxy path */
         } else {
            nErr = EFAILED; /* no proxy */
         }
      }
   }
   
   return nErr;
}

/*
  ===========================================================================
  ===========================================================================
    HtmlDlg stuff
  ===========================================================================
  ===========================================================================
*/
typedef struct HtmlDlg HtmlDlg;
typedef void (*PFNHVNJUMP)(void *p,HViewJump *phvj);

struct HtmlDlg
{
   IDisplay    *piDisplay;
   IHtmlViewer *piHtml;
   
   AEERect      rc;
   PFNNOTIFY    pfnInvalidate;
   void        *pInvalidateData;

   unsigned     bFullScreen:1;
   unsigned     bActive:1;
   
   int          nFrameWidth;

   PFNHVNJUMP   pfnJump;
   void        *pJumpData;
};


static void HtmlDlg_HViewNotify(void *p, HViewNotify *phvn)
{
   HtmlDlg *me = (HtmlDlg *)p;

   switch (phvn->code) {
   case HVN_REDRAW_SCREEN:
      me->bFullScreen = FALSE;
   case HVN_INVALIDATE:
      me->pfnInvalidate(me->pInvalidateData);
      break;

   case HVN_JUMP:
   case HVN_SUBMIT:
      me->pfnJump(me->pJumpData, &phvn->u.jump);
      break;
      
   case HVN_FULLSCREEN_EDIT:
      me->bFullScreen = TRUE;
      break;

   case HVN_DONE:
      break;
   }
}

static boolean HtmlDlg_IsActive(HtmlDlg *me)
{
   return me->bActive;
}

static void HtmlDlg_SetActive(HtmlDlg *me, boolean bActive)
{
   me->bActive = bActive;

   IHTMLVIEWER_SetActive(me->piHtml,bActive);
   me->pfnInvalidate(me->pInvalidateData);
}

static void HtmlDlg_Fill(HtmlDlg *me, 
                         const char *cpszContentType,
                         const char *cpContent, uint32 ulContentLength)
{
   IHTMLVIEWER_SetType(me->piHtml,cpszContentType);
   IHTMLVIEWER_SetData(me->piHtml,cpContent,ulContentLength);
}

static int HtmlDlg_Start(HtmlDlg *me, PFNHVNJUMP pfnJump, void *pJumpData)
{

   if ((PFNHVNJUMP)0 != me->pfnJump) {
      me->pfnJump(me->pJumpData,0);
   }
   me->pfnJump = pfnJump;
   me->pJumpData = pJumpData;

   {
      AEERect rc = me->rc;

      rc.x += me->nFrameWidth;
      rc.y += me->nFrameWidth;
      rc.dx -= (me->nFrameWidth*2);
      rc.dy -= (me->nFrameWidth*2);

      IHTMLVIEWER_SetRect(me->piHtml,&rc);
   }

   HtmlDlg_SetActive(me,TRUE);
   
   return SUCCESS;
}

static void HtmlDlg_End(HtmlDlg *me)
{

   me->pfnJump = 0;
   IHTMLVIEWER_Reset(me->piHtml);

   {
      AEERect rc = me->rc;
      
      rc.dx = rc.dy = 0;
      IHTMLVIEWER_SetRect(me->piHtml,&rc);
   }

   HtmlDlg_SetActive(me,FALSE);
}

static void HtmlDlg_CtorZ(HtmlDlg *me, 
                          IDisplay *piDisplay, IHtmlViewer *piHtml, 
                          AEERect *prc, int nFrameWidth,
                          PFNNOTIFY pfnInvalidate, void *pInvalidateData)
{
   me->piDisplay = piDisplay;
   IDISPLAY_AddRef(piDisplay);

   me->piHtml = piHtml;
   IHTMLVIEWER_AddRef(piHtml);
   IHTMLVIEWER_SetProperties(piHtml, (IHTMLVIEWER_GetProperties(piHtml) | 
                                      HVP_SCROLLBAR | HVP_LOADALLIMAGES |
                                      HVP_INVALIDATION));

   IHTMLVIEWER_SetNotifyFn(piHtml, HtmlDlg_HViewNotify, me);
   me->rc = *prc;

   me->pfnInvalidate   = pfnInvalidate;
   me->pInvalidateData = pInvalidateData;
   me->nFrameWidth     = nFrameWidth;

   HtmlDlg_End(me);
}

static void HtmlDlg_Dtor(HtmlDlg *me)
{
   if ((PFNHVNJUMP)0 != me->pfnJump) {
      me->pfnJump(me->pJumpData,0);
   }

   RELEASEIF(me->piDisplay);
   RELEASEIF(me->piHtml);
   
   ZEROAT(me);
}

static boolean HtmlDlg_HandleEvent(HtmlDlg *me, AEEEvent eCode, 
                                   uint16 wParam, uint32 dwParam)
{
   if (!me->bActive) {
      return FALSE;
   }
   
   if (IHTMLVIEWER_HandleEvent(me->piHtml,eCode,wParam,dwParam)) {
      return TRUE;
   }
   
   if (eCode == EVT_KEY && 
       ((wParam == AVK_CLR) || (wParam == AVK_SELECT))) {
      if ((PFNHVNJUMP)0 != me->pfnJump) {
         me->pfnJump(me->pJumpData,0);
      }
   }
   return TRUE;
}

static void HtmlDlg_Redraw(HtmlDlg *me)
{
   if (!me->bActive) {
      return;
   }
   
   if (!me->bFullScreen) {
      AEERect rc = me->rc;
      int     i;

      for (i = 0; i < me->nFrameWidth-1; i++) {
         IDISPLAY_DrawRect(me->piDisplay,&rc,RGB_BLACK,0xbeef,
                           IDF_RECT_FRAME);
         rc.x  += 1;
         rc.y  += 1;
         rc.dx -= 2;
         rc.dy -= 2;
      }
      IDISPLAY_DrawRect(me->piDisplay,&rc,RGB_WHITE,0xbeef,
                        IDF_RECT_FRAME);
      rc.x  += 1;
      rc.y  += 1;
      rc.dx -= 2;
      rc.dy -= 2;
   }

   IHTMLVIEWER_Redraw(me->piHtml);
}


/*
  ===========================================================================
  ===========================================================================
   AuthInfo stuff
  ===========================================================================
  ===========================================================================
*/

typedef struct AuthInfo AuthInfo;

struct AuthInfo
{
   AuthInfo  *pNext;

   unsigned   bProxy:1;

   char      *pszUser; /* pszUser is allocated, holds pszUser and pszCred */
   char      *pszCred;  /* points into pszUser */

   char       szUrl[1]; /* URL base, and pszRealm, over-allocated */
};


static int AuthInfo_Update(AuthInfo *me, IWebUtil *piWebUtil,
                           const char *cpszUser, const char *cpszPass)
{
   int nUserLen = STRLEN(cpszUser);
   int nPassLen = STRLEN(cpszPass);
   int nCredLen = BASE64LEN(nUserLen+1+nPassLen);

   FREEIF(me->pszUser);
   me->pszCred = 0;

   me->pszUser = (char *)MALLOC(nUserLen+1+nCredLen);

   if ((char *)0 == me->pszUser) {
      return ENOMEMORY;
   }
   me->pszCred = me->pszUser + nUserLen + 1;
   
   STRCPY(me->pszUser,cpszUser);
   SPRINTF(me->pszCred, "%s:%s", cpszUser, cpszPass);

   IWEBUTIL_EncodeBase64(piWebUtil,me->pszCred,
                         STRLEN(me->pszCred),me->pszCred);

   return SUCCESS;
}
   
static int AuthInfo_New(const char *cpszUrl, boolean bProxy, AuthInfo **ppai)
{
   AuthInfo *me = 0;

   me = MALLOCREC_EX(AuthInfo, STRLEN(cpszUrl));
   
   if ((AuthInfo *)0 == me) {
      return ENOMEMORY;
   }
   STRCPY(me->szUrl,cpszUrl);
   me->bProxy = bProxy;
   
   me->pNext = *ppai;
   *ppai = me;
   return SUCCESS;
}

static void AuthInfo_Delete(AuthInfo **pme)
{
   AuthInfo *me = *pme;
   
   *pme = me->pNext;

   FREEIF(me->pszUser);
   me->pszCred = 0;
   FREE(me);
}


/*
  ===========================================================================
  ===========================================================================
    AuthMgr stuff
  ===========================================================================
  ===========================================================================
*/

typedef struct AuthPrompt
{
   unsigned  bProxy:1; /* AITYPE_PROXY or AITYPE_WWW */
   
   unsigned  nRet:31;  /* return value, an AEEError, EFAILED for cancelled,
                          SUCCESS for success, other errors as-is */

   char     *pszRealm;  /* realm */
   char     *pszUrl;    /* url */

} AuthPrompt;

typedef void (*PFNDIALOGWAIT)(void *pDlgData, AEECallback *pcb);
typedef int (*PFNDIALOGSTART)(void *pDlgData,
                              PFNHVNJUMP pfnJump, void *pJumpData);
typedef int (*PFNDIALOGFILL)(void *pDlgData, AEEResBlob *prb, int nLen,
                             uint16 uResId,
                             const char *cpszzTokFormats, ...);
typedef void (*PFNDIALOGEND)(void *pDlgData);

typedef struct AuthMgr
{
   IShell         *piShell;
   IWebUtil       *piWebUtil;

   PFNDIALOGWAIT   pfnDlgWait;
   PFNDIALOGSTART  pfnDlgStart;
   PFNDIALOGFILL   pfnDlgFill;
   PFNDIALOGEND    pfnDlgEnd;
   void           *pDlgData;
   
   AEECallback     cbDlgWait;

   AEECallback    *pcbPrompts; /* list of guys waiting for auth */

   char           *pszUrl; /* copy of in progress URL */
   boolean         bProxy;

   AuthInfo       *paiList;
} AuthMgr;

/* forward decls */
static void AuthMgr_NextPrompt(void *p);

static void AuthMgr_PromptCancel(AEECallback *pcb)
{
   AuthMgr      *me = (AuthMgr *)pcb->pCancelData;
   AEECallback **lfp;

   pcb->pfnCancel = 0;

   for (lfp = &me->pcbPrompts; *lfp != pcb; lfp = &(*lfp)->pNext);

   *lfp = pcb->pNext;
   pcb->pNext = 0;
}

static void AuthMgr_Dtor(AuthMgr *me)
{
   AEECallback *pcb;
   
   CALLBACK_Cancel(&me->cbDlgWait);

   if ((char *)0 != me->pszUrl) {/* in progress */
      me->pfnDlgEnd(me->pDlgData);
      FREEIF(me->pszUrl);
   }

   while ((AuthInfo *)0 != me->paiList) {
      AuthInfo_Delete(&me->paiList);
   }
   
   while ((AEECallback *)0 != (pcb = me->pcbPrompts)) {
      AuthPrompt *pap = (AuthPrompt *)pcb->pReserved;
      pap->nRet = EFAILED;
      ISHELL_Resume(me->piShell,pcb);
   }
   
   RELEASEIF(me->piShell);
   RELEASEIF(me->piWebUtil);
   ZEROAT(me);
}

static AuthInfo *AuthMgr_FindAuthInfo(AuthMgr *me, const char *cpszUrl, 
                                      boolean bProxy, boolean bExact)
{
   AuthInfo *pai;
   AuthInfo *paiMatched = 0;
   
   for (pai = me->paiList; (AuthInfo *)0 != pai; pai = pai->pNext) {

      if (pai->bProxy != bProxy) { /* not my type */
         continue; 
      }
      
      if (bExact) { /* looking for an exact match */
         if (STRCMP(pai->szUrl,cpszUrl)) {
            continue;
         }
         paiMatched = pai;
         break;
      }
      
      /* looking for a sub-match */
      if (!STRBEGINS(pai->szUrl,cpszUrl)) {
         /* no beginning match */
         continue;
      }
      
      if (((AuthInfo *)0 == paiMatched) ||  
          (STRLEN((paiMatched)->szUrl) < STRLEN(pai->szUrl))) {
         /* no previous matched or previous matched less */
         paiMatched = pai;
      }
   }

   return paiMatched;
}

static void AuthMgr_DeleteAuthInfo(AuthMgr *me, AuthInfo *pai)
{
   AuthInfo **lfp;

   if ((AuthInfo *)0 == pai) {
      return;
   }

   for (lfp = &me->paiList; *lfp != pai; lfp = &(*lfp)->pNext);
   AuthInfo_Delete(lfp);
}

static void AuthMgr_PromptRet(AuthMgr *me, int nErr)
{
   AEECallback *pcb;

   while ((AEECallback *)0 != (pcb = me->pcbPrompts)) {
      AuthPrompt *pap;
      
      pap = (AuthPrompt *)pcb->pReserved;
      pap->nRet = nErr;
      ISHELL_Resume(me->piShell,pcb);
   }
}

static void AuthMgr_Jump(void *p, HViewJump *phvj)
{
   AuthMgr   *me = p;
   UrlParts   up;
   AuthInfo  *pai;
   int        nErr;
   
   me->pfnDlgEnd(me->pDlgData);
   
   /* get an exact match */
   pai = AuthMgr_FindAuthInfo(me,me->pszUrl,me->bProxy,TRUE);
   
   if ((HViewJump *)0 == phvj) { /* cancel */
      AuthMgr_DeleteAuthInfo(me,pai);
      nErr = EFAILED; /* means cancelled */
   } else {
      char *pszFormFields;

      IWEBUTIL_ParseUrl(me->piWebUtil,phvj->pszURL,&up);
   
      pszFormFields = STRDUP(up.cpcSrch+1);
   
      if ((char *)0 == pszFormFields) {
         nErr = ENOMEMORY;
      } else {
         char *pszPass = 0;
         char *pszUser = 0;
      
         /* ignore retval, checking user and password */
         IWebUtil_GetFormFields(me->piWebUtil,pszFormFields,"user\0pass\0",
                                &pszUser,&pszPass);
      
         if ((AuthInfo *)0 == pai) {
            nErr = AuthInfo_New(me->pszUrl,me->bProxy,&me->paiList);
            pai = me->paiList;
         } else {
            nErr = SUCCESS;
         }
      
         if (SUCCESS == nErr) {
            nErr = AuthInfo_Update(pai,me->piWebUtil,pszUser,pszPass);
         }
         FREEIF(pszFormFields);
      }
   }

   FREEIF(me->pszUrl);

   AuthMgr_PromptRet(me, nErr);

   AuthMgr_NextPrompt(me);
}

static void AuthMgr_NextPrompt(void *p)
{
   AuthMgr *me = (AuthMgr *)p;
   int      nErr;

   if ((AEECallback *)0 == me->pcbPrompts) { /* I have no prompts */
      return;
   }

   nErr = me->pfnDlgStart(me->pDlgData,AuthMgr_Jump,me);

   if (EALREADY == nErr) {
      me->pfnDlgWait(me->pDlgData,&me->cbDlgWait);
      return;
   }
 
   if (SUCCESS == nErr) {
      uint16      uResId;
      const char *cpszUser;
      AuthPrompt *pap;

      pap = (AuthPrompt *)me->pcbPrompts->pReserved;
   
      if (pap->bProxy) {
         uResId = IDX_PROXYAUTH;
      } else {
         uResId = IDX_AUTH;
      }

      {
         AuthInfo *pai = 
            AuthMgr_FindAuthInfo(me,pap->pszUrl,(boolean)pap->bProxy,FALSE);
         
         if ((AuthInfo *)0 != pai) {
            cpszUser = pai->pszUser;
         } else {
            cpszUser = ""; /* never mind */
         }
      }

      me->pszUrl = STRDUP(pap->pszUrl);
      me->bProxy = pap->bProxy;

      if ((char *)0 == me->pszUrl) {
         nErr = ENOMEMORY;
      } else {
         nErr = SUCCESS;
      }

      if (SUCCESS == nErr) {
         nErr = me->pfnDlgFill(me->pDlgData,0,0,uResId,
                               "url%Qs\0realm%Qs\0user%Qs\0",
                               pap->pszUrl,pap->pszRealm,cpszUser);
      }
   }

   if (SUCCESS != nErr) {
      FREEIF(me->pszUrl);
      AuthMgr_PromptRet(me, nErr);
      me->pfnDlgEnd(me->pDlgData);
   }

}

static void AuthMgr_AuthPrompt(AuthMgr *me,AuthPrompt *pap,AEECallback *pcb)
{
   AEECallback **lfp;

   CALLBACK_Cancel(pcb);

   pcb->pReserved = pap;
   pcb->pfnCancel = AuthMgr_PromptCancel;
   pcb->pCancelData = me;
   
   for (lfp = &me->pcbPrompts;(AEECallback *)0 != *lfp;lfp = &(*lfp)->pNext);

   *lfp = pcb;

   AuthMgr_NextPrompt(me);
}

static void AuthMgr_CtorZ(AuthMgr *me, IShell *piShell, IWebUtil *piWebUtil,
                          PFNDIALOGWAIT pfnDlgWait,
                          PFNDIALOGSTART pfnDlgStart,
                          PFNDIALOGFILL pfnDlgFill,
                          PFNDIALOGEND pfnDlgEnd,
                          void *pDlgData)
{
   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   me->piWebUtil  = piWebUtil;
   IWEBUTIL_AddRef(piWebUtil);

   me->pfnDlgWait  = pfnDlgWait;
   me->pfnDlgStart = pfnDlgStart;
   me->pfnDlgFill  = pfnDlgFill;
   me->pfnDlgEnd   = pfnDlgEnd;
   me->pDlgData    = pDlgData;

   CALLBACK_Init(&me->cbDlgWait,AuthMgr_NextPrompt,me);
}

/*
  ===========================================================================
  ===========================================================================
   X509CertInfo stuff
  ===========================================================================
  ===========================================================================
*/

typedef struct X509CertInfo
{
   X509BasicCert  xbc;
   char          *pszIssuerDN;
   char          *pszSubjectDN;
} X509CertInfo;

static int IX509Chain_GetDNString(IX509Chain *piX509Chain,
                                  int32 nCertWebOpt,
                                  int32 nCertIndex,
                                  uint8 uCertPart,
                                  char *pszDN,
                                  int *pnLen)
{
   /* DNSTRs and DNOIDs have to match */
#define DNSTRS ", cn=\0, ou=\0, o=\0, lo=\0, st=\0, co=\0"
   static const uint32 ulDNOIDs[] = {
      ASNOID_X500_CN,
      ASNOID_X500_OU,
      ASNOID_X500_O,
      ASNOID_X500_LO,
      ASNOID_X500_ST,
      ASNOID_X500_CO,
   };

   X509PartRequest xpr;
   BufBound        bb;
   int             i;

   ZEROAT(&xpr);
   BufBound_Init(&bb,pszDN, *pnLen);

   xpr.nCertWebOpt = nCertWebOpt;
   xpr.nCertIndex  = nCertIndex;
   xpr.uCertPart   = uCertPart;
                  
   for (i = 0; i < ARRAY_SIZE(ulDNOIDs); i++) {
      X509CertPart xcp;
      const char *cpszOID = strchopped_nth(DNSTRS,i);
      ZEROAT(&xcp);

      xpr.uASNOID   = ulDNOIDs[i];
      xpr.nInstance = 0;

      while (SUCCESS == IX509CHAIN_GetFieldPart(piX509Chain,&xpr,&xcp)) {
         xpr.nInstance++;
         BufBound_Puts(&bb,cpszOID);
         BufBound_Write(&bb,(const char *)xcp.pcValue,xcp.uLength);
      }
   }
   /* null terminate */
   BufBound_Write(&bb,"",1);

   if (0 != BUFBOUND_REALLYWROTE(&bb,pszDN)) {
      /* strip comma */
      MEMMOVE(pszDN, pszDN+2, BUFBOUND_REALLYWROTE(&bb,pszDN)-2);
      bb.pcWrite-=2;
   }

   *pnLen = BUFBOUND_WROTE(&bb,pszDN);
   
   return SUCCESS;
}


static int IX509Chain_MakeDNString(IX509Chain *piCerts,
                                   int32 nCertWebOpt, int32 nCertIndex,
                                   uint8 uCertPart, char **ppsz)
{
   int nErr;
   int nLen = 0;

   nErr = IX509Chain_GetDNString(piCerts, nCertWebOpt, nCertIndex, uCertPart,
                                 0, &nLen);

   if (SUCCESS == nErr) {
      nErr = ERR_MALLOC(nLen, ppsz);

      if (SUCCESS == nErr) {
         IX509Chain_GetDNString(piCerts, nCertWebOpt, nCertIndex,uCertPart,
                                *ppsz, &nLen);
      }
   }

   return nErr;
}

static void X509CertInfo_Delete(X509CertInfo *me)
{
   FREEIF(me->pszIssuerDN);
   FREEIF(me->pszSubjectDN);
   FREEIF(me);
}

static int X509CertInfo_New(IX509Chain *piCerts, 
                            int32 nCertWebOpt,int32 nCertIndex,
                            X509CertInfo **ppxci)
{
   X509CertInfo *me = 0;
   int nErr;

   *ppxci = 0;

   nErr = ERR_MALLOCREC(X509CertInfo,&me);

   if (SUCCESS != nErr) {
      goto bail;
   }

   nErr = IX509CHAIN_GetBasic(piCerts, 
                              nCertWebOpt, nCertIndex,
                              &me->xbc);
   if (SUCCESS != nErr) {
      goto bail;
   }
   
   nErr = IX509Chain_MakeDNString(piCerts,nCertWebOpt, nCertIndex,
                                  X509CHAIN_FIELD_SUBJECT,
                                  &me->pszSubjectDN);

   if (SUCCESS != nErr) {
      goto bail;
   }

   FARF(CERTS,("subject \"%s\"",me->pszSubjectDN));
            
   nErr = IX509Chain_MakeDNString(piCerts,nCertWebOpt, nCertIndex,
                                  X509CHAIN_FIELD_ISSUER,
                                  &me->pszIssuerDN);

   if (SUCCESS != nErr) {
      goto bail;
   }

   FARF(CERTS,("issuer \"%s\"",me->pszIssuerDN));

   *ppxci = me;
   me = 0; /* forget about me, everything's cool! */

 bail:
   if ((X509CertInfo *)0 != me) {
      X509CertInfo_Delete(me);
   }

   return nErr;
}


/*
  ===========================================================================
  ===========================================================================
    Brewser stuff
  ===========================================================================
  ===========================================================================
*/

/*
  ===========================================================================
    defines, macros
  ===========================================================================
*/
#ifndef BREWSER_VERSION
#define BREWSER_VERSION    "2.0.0.15"
#endif /* #ifndef BREWSER_VERSION */

#ifdef _DEBUG
#define BREWSER_USERAGENT  "BREWser/"BREWSER_VERSION" (BREW {version}; DeviceId {devid}; built "__DATE__" "__TIME__")"
#else /* #ifdef _DEBUG */
#define BREWSER_USERAGENT  "BREWser/"BREWSER_VERSION" (BREW {version}; DeviceId {devid})"
#endif /* #ifdef _DEBUG */
#define BREWSER_SLIDER_DELAY     -1 /* basically, run as fast as you can */
#define BREWSER_REFRESH_IDLE     60 /* seconds we'll run refreshers without
                                       user input */
#define BREWSER_MINHISTS         20 /* number of sacrosanct history list 
                                       entries */
#define BREWSER_COOKIESMINSIZE 2048 /* minimum number of cookie bytes I'll 
                                       keep when asked to cull */
#define BREWSER_TTDELAY         850  /* delay, in milliseconds, until a 
                                        tooltip shows up */
#define BREWSER_TTEXPIRY       5000  /* delay, in milliseconds, until a 
                                        shown tooltip goes away */

/*
  || command defines, kinda spell stuff
 */
#define IDC_GOTO      (0x00006070)
#define IDC_OPT       (0x00000097)
#define IDC_HELP      (0x000000F1)
#define IDC_CANCEL    (0x0000C9C1)
#define IDC_REFRESH   (0x00000FE5)
#define IDC_INFO      (0x000019F0)
#define IDC_BOOKMARKS (0x0000B003)

/*
  ===========================================================================
    types
  ===========================================================================
*/



/*
  history list entry
 */
typedef struct BrewserHist BrewserHist;
struct BrewserHist
{
   BrewserHist  *pPrev;

   /* stuff from HtmlViewer */
   char         *pszUrl;    /* overwritten by Location on redirect */
   char         *pszMethod; /* method, if not "GET" */
   char         *pszData;   /* POST data string, null if none */

   /* my state */
   unsigned      bStopped:1;   /* stopped while loading (incomplete doc) */
   unsigned      bLoading:1;   /* the viewer is reading data */
   unsigned      bIsBack:1;    /* if it's a backward nav into hist list */
   unsigned      bIsRefresh:1; /* if it's a refresh */

   char         *pszViewState; /* viewer state */

   /* save code? */
   int           nCode;     /* WebRespInfo code, 0 if not set yet */
   long          lContentLength;

   /* save header values */
   char         *pszContentType; /* content type */
   char         *pszExpires;     /* expires */
   char         *pszLastModified;/* last modified  */
   char         *pszRealm;     /* WWW-Authenticate or Proxy-Authenticate 
                                  auth Basic realm= */
   char         *pszLocation;  /* location header value,  */
   X509CertInfo *pxci;         /* present if an SSL page */

   
   /* used for navigation */
   char         *pszAuth;      /* request authentication headers */
   char         *pszCookie;    /* cookie header */
   char         *pszReferer;   /* referer header */

};

typedef struct BrewserAuth 
{
   Brewser     *pbsr;   /* my owner */
   AuthPrompt   ap;     /* stuff for talking to authmgr */
   AEECallback  cb;     /* dialog waiter */
   char        *pszStr; /* holds strings in ap */
} BrewserAuth;

typedef struct BrewserTrust
{
   Brewser           *pbsr;  /* my owner */
   char              *pszHost;
   
   X509CertInfo       xci;
   int                nCode;

   AEECallback        cb;
} BrewserTrust;

/*
  A Brewser is the Applet implementation, holds everything
 */
struct Brewser
{
   IApplet      ia;     /* my vtable, must be first so that casts to Brewser
                           succeed */
   IAppletVtbl  iavt;   /* so I don't have to call mallocrec_ex */

   unsigned     uRef;   /* refcount */
               
   IModule     *pim;    /* my parent the IModule  must be AddRef'd, Released,
                           is how BREW keeps track of when a Module is in 
                           use */
   
   IWeb        *piWeb;   /* IWeb interface, my raison d'être */
   IWebCache   *piWebCache;/* IWebCache interface, QI'd to */
   IWebOpts    *piwoSSL; /* SSL config */
   IWebUtil    *piWebUtil;/* IWebUtil interface */
   IShell      *piShell; /* IShell interface, used for CreateInstance of 
                            service classes, mostly */
               
   AEERect      rcScreen; /* screen rectangle */

   AEECallback  cbRedraw; /* redraw callback, fires on invalidation */
   
   BrewserHist *pbhCur;   /* current history entry, may or may not be
                             in the history list */

   BrewserHist *pbhList;  /* BrewserHist stack.*/
   BrewserHist  bhBottom; /* bottom of BrewserHist stack, 
                             statically allocated */
   
   IDisplay    *piDisplay; /* where we draw */
   StatusBox    sbStatus;  /* bouncing ball-type stuff */

   IMenuCtl    *piMenu; /* softkey menu, for URL mode, HTML mode */
   int16        nSkSaveY;  /* where the softkey menu wants to be when it's 
                              shown */
   int16        nTbSaveY;  /* where I should show the toolbar when fully
                              shown */
   int16        nTbSlideY; /* where I should show the toolbar right now */
   uint16       uLingerSave; /* saved linger value from startup, BREWser uses
                                3 minutes */
   Slider       sTb;       /* toolbar slider */
   Tooltip      ttTbTips;  /* toolbar tooltips timeout */
   
   unsigned     bInRedraw:1;/* redrawing, ignore invalidates from iHtml */

   unsigned     bShowingToolbar:1;/* showing toolbar */
   unsigned     bShowingDialog:1; /* showing my html dlg */

   unsigned     bSuspended:1; /* suspended, don't draw or invalidate */
   unsigned     bDoCookies:1; /* doing cookies */
   unsigned     bNoCache:1;   /* turn off caching altogether */

   char        *pszMailTo;   /* mailto substitution */
   char        *pszUserAgent; /* my user-agent header */
   char         szHeaders[CSTRLEN("Accept-Language: %s\r\n") + 5];

   AEECallback  cbGotResp;   /* how to call me back */
   IWebResp    *piWResp;     /* the answer I get from IWeb */

   IHtmlViewer *piHtml;      /* HtmlViewer woo-hoo! */
   ISourceUtil *piSourceUtil;/* source utility,used for making post bodies*/

   uint32       tWebStart;
   uint32       tWebElapsed;
   uint32       tLastUserEvent;

   HeaderMaker  hm;
   CookieMgr    ckm;
   AuthMgr      am;
   HtmlDlg      htmldlg;

   AEECallback *pcbWaitDialog;  /* list of guys waiting to use htmldlg */

   Prefs        prefs;

   BrewserAuth  ba;             /* auth struct for pbhCur */
   BrewserTrust bt;             /* ssl trust override stuff for pbhCur */

   AEECallback  cbLowRAM;       /* called when the system needs 
                                   some memory back */

   Refresher    *prfsher;       /* if a hist entry does a refresh, this
                                   keeps track */
   
   int           nSpelledDebug; /* for debugging purposes */

   AEECallback   cbInfoDialog;  /* use to wait to show info */
};


/*
  || forward decls (vtable parts)
*/
static uint32 Brewser_AddRef(IApplet *pif);
static uint32 Brewser_Release(IApplet *pif);
static boolean Brewser_HandleEvent(IApplet *pif, AEEEvent eCode, 
                                   uint16 wParam, uint32 dwParam);

/*
  || forward decls (other stuff)
*/
static void Brewser_PushCurHist(Brewser *me);
static boolean Brewser_InProgress(Brewser *me);
static void Brewser_TooltipPush(Brewser *me);
static void Brewser_Invalidate(void *p);
static void Brewser_NavStart(Brewser *me);
static void Brewser_Help(Brewser *me);
static void Brewser_Jump(Brewser *me,HViewJump *phvj,
                         const char *cpszReferer);
static void Brewser_Back(Brewser *me, const char *cpszTo,
                         const char *cpszPast);
static void Brewser_Refresh(Brewser *me);
static boolean Brewser_Stop(Brewser *me);
static void Brewser_Redraw(void *p);

static int Brewser_FillDialogV(Brewser *me, AEEResBlob *prb, int nLen,
                               uint16 uResId, const char *cpszzTokFormats, 
                               va_list args);
static int Brewser_FillDialog(void *p, AEEResBlob *prb, int nLen,
                              uint16 uResId, const char *cpszzTokFormats, ...);
static void Brewser_WaitDialog(void *p, AEECallback *pcb);
static int Brewser_StartDialog(void *p,PFNHVNJUMP pfnJump, void *pJumpData);
static void Brewser_EndDialog(void *p);
static void Brewser_DebugKey(Brewser *me, uint16 wKey);


static void Brewser_TbSlideInc(void *p)
{
   Brewser *me = (Brewser *)p;
   AEERect rc = me->rcScreen;
      
   rc.y   = me->nTbSlideY;
   rc.dy -= me->nTbSaveY;
   
   IMENUCTL_SetRect(me->piMenu,&rc);

   rc.y  = 0;
   rc.dy = me->nTbSlideY;
   
   IHTMLVIEWER_SetRect(me->piHtml,&rc); 
   
   Brewser_Invalidate(me);
}

static void Brewser_HideToolbarDone(void *p)
{
   Brewser *me = (Brewser *)p;

   IHTMLVIEWER_SetActive(me->piHtml,TRUE);
}


static void Brewser_HideToolbar(Brewser *me)
{
   me->bShowingToolbar = FALSE;
   Tooltip_Cancel(&me->ttTbTips);

   IMENUCTL_SetActive(me->piMenu,FALSE);

   Slider_Start(&me->sTb,me->rcScreen.dy,Brewser_HideToolbarDone,
                BREWSER_SLIDER_DELAY);
}

static void Brewser_ShowToolbarDone(void *p)
{
   Brewser *me = (Brewser *)p;

   /* not sure it's necessary to set the selection to 'Back', but 
      for consistency's sake (when we go in and out of URL input mode, 
      selection goes to 'Back') set it here */
   IMENUCTL_SetSel(me->piMenu,IDC_CANCEL);
   IMENUCTL_SetActive(me->piMenu,TRUE);
   me->bShowingToolbar = TRUE;
   Brewser_TooltipPush(me);

   Brewser_Invalidate(me);
}

static void Brewser_ShowToolbar(Brewser *me)
{
   Tooltip_Cancel(&me->ttTbTips);

   IHTMLVIEWER_SetActive(me->piHtml,FALSE);

   Slider_Start(&me->sTb,me->nTbSaveY,Brewser_ShowToolbarDone,
                BREWSER_SLIDER_DELAY);
}

static void BrewserHist_Dtor(BrewserHist *me)
{
   if ((X509CertInfo *)0 != me->pxci) {
      X509CertInfo_Delete(me->pxci);
   }
   FREEIF(me->pszUrl);
   FREEIF(me->pszMethod);
   FREEIF(me->pszData);

   FREEIF(me->pszViewState);

   FREEIF(me->pszContentType);
   FREEIF(me->pszExpires);
   FREEIF(me->pszLastModified);
   FREEIF(me->pszRealm);
   FREEIF(me->pszLocation);

   FREEIF(me->pszAuth);
   FREEIF(me->pszCookie);
   FREEIF(me->pszReferer);
}

static void BrewserHist_Delete(BrewserHist *me)
{
   BrewserHist_Dtor(me);
   FREE(me);
}


#define BREWSER_ASSERT(me, __x__) \
    (!(__x__) && (Brewser_Stop(me),Brewser_Alert(me,IDX_ASSERT,"file%s\0line%d\0assertion%Qs",basename(__FILE__),__LINE__,#__x__),TRUE))

#define BREWSER_FAILED(me,err) \
     Brewser_Stop(me),Brewser_TinyPage(me,IDX_EFAILED,"file%s\0line%d\0err%d\0",basename(__FILE__),__LINE__,err)

static void Brewser_Alert(Brewser *me, uint16 uResId, 
                          const char *cpszzTokFormats, ...)
{
   va_list args;
   union {
      AEEResBlob rb;
      byte       buf[512];
   } resBlob;

   ZEROAT(&resBlob);

   Brewser_EndDialog(me);
   Brewser_StartDialog(me,(PFNHVNJUMP)Brewser_EndDialog,me);

   va_start(args,cpszzTokFormats);
   Brewser_FillDialogV(me,&resBlob.rb,sizeof(resBlob),uResId,
                       cpszzTokFormats,args);
   va_end(args);
}

static void Brewser_SaveViewState(Brewser *me)
{
   BrewserHist *pbh = me->pbhList;
   int nLen;
   
   nLen = IHTMLVIEWER_GetViewState(me->piHtml,0,0);
   FREEIF(pbh->pszViewState);
   pbh->pszViewState = (char *)MALLOC(nLen);
   IHTMLVIEWER_GetViewState(me->piHtml,pbh->pszViewState,nLen);
}


static int Brewser_ResPageV(Brewser *me, AEEResBlob *prb, int nLen,
                             uint16 uResId, 
                             const char *cpszzTokFormats, va_list args)
{
   int         nErr;
   AEEResBlob *prbData = prb;
   int         nSaveLen = nLen;

   nErr = IShell_LoadResBlob(me->piShell,BREWSER_RES_FILE,uResId,
                             &prbData,&nLen);

   if (SUCCESS == nErr) {
      if ((AEEResBlob *)0 == prb) { /* LoadResBlob malloc'd */
         if (0 < AEEResBlob_VStrRepl(&prbData,&nLen,cpszzTokFormats,args)) {
            nErr = ENOMEMORY;
         }
         prb = prbData;
      } else {
         char *pszData = RESBLOB_DATA(prb);
         /* do our best */
         vsnxprintf(pszData,nSaveLen-prb->bDataOffset,pszData,"{}",
                    cpszzTokFormats,args);
         prbData = 0;
      }
   }

   if (SUCCESS == nErr) {
      /* save off viewer state, about to destroy it */
      Brewser_SaveViewState(me);
      
      /* wanna clear URL, call reset */
      IHTMLVIEWER_Reset(me->piHtml);
      IHTMLVIEWER_SetType(me->piHtml,prb->szMimeType);
      IHTMLVIEWER_SetData(me->piHtml,RESBLOB_DATA(prb),-1);
   }

   FREEIF(prbData);

   return nErr;
}

static void Brewser_TinyPage(Brewser *me, uint16 uResId, 
                             const char *cpszzTokFormats, ...)
{
   va_list args;
   union {
      AEEResBlob rb;
      byte       buf[512];
   } resBlob;

   ZEROAT(&resBlob);
   
   va_start(args,cpszzTokFormats);
   Brewser_ResPageV(me,&resBlob.rb,sizeof(resBlob),uResId,
                    cpszzTokFormats,args);
   va_end(args);
}

static int Brewser_ResPage(Brewser *me, AEEResBlob *prb, int nLen,
                           uint16 uResId, const char *cpszzTokFormats, ...)
{
   int nErr;
   va_list args;
   
   va_start(args,cpszzTokFormats);
   nErr = Brewser_ResPageV(me,prb,nLen,uResId, cpszzTokFormats, args);
   va_end(args);

   return nErr;
}


static boolean AuthBasic_ToRealm(char *pszAuth)
{
   char *pszRealm;

   if ((char *)0 == pszAuth) {
      return FALSE;
   }
   
   /* strip leading spaces */
   while (' ' == *pszAuth) {
      pszAuth++;
   }

   pszRealm = STRCHR(pszAuth,' ');
   
   if ((char *)0 == pszRealm) {
      return FALSE;
   }
   
   pszRealm++;

   /* only support auth basic */
   if (MEMCMP(pszAuth,"Basic ",pszRealm-pszAuth)) {
      return FALSE;
   }

   pszRealm = STRISTR(pszRealm, "realm=\"");

   if ((char *)0 == pszRealm) {
      return FALSE;
   }
   pszRealm += sizeof("realm=\"") - 1;
   
   {
      char *psz = STRCHR(pszRealm,'"');

      if ((char *)0 != psz) {
         *psz = 0;
      }
   }

   STRCPY(pszAuth,pszRealm);
   
   return TRUE;
}

static boolean Brewser_HeaderMade(void *p, const char *cpszName, 
                                  char *pszValue)
{
   Brewser     *me = (Brewser *)p;
   BrewserHist *pbhCur = me->pbhCur;

   if (BREWSER_ASSERT(me,(BrewserHist *)0 != pbhCur)) {
      return FALSE;
   }

   if (!STRICMP(cpszName,"Last-Modified")) {

      FREEIF(pbhCur->pszLastModified);
      pbhCur->pszLastModified = pszValue;
      return TRUE;      

   } else if (!STRICMP(cpszName,"Expires")) {

      FREEIF(pbhCur->pszExpires);
      pbhCur->pszExpires = pszValue;
      return TRUE;

   } else if (!STRICMP(cpszName,"Location")) {

      FREEIF(pbhCur->pszLocation);
      pbhCur->pszLocation = pszValue;
      return TRUE;

   } else if (!STRICMP(cpszName,"Refresh")) {
      if ((Refresher *)0 != me->prfsher) {
         Refresher_Delete(me->prfsher);
         me->prfsher = 0;
      }
      Refresher_New(pszValue,&me->prfsher);
      return FALSE;
      
   } else if (!STRICMP(cpszName,"Proxy-Authenticate") ||
              !STRICMP(cpszName,"WWW-Authenticate")) {

      FREEIF(pbhCur->pszRealm);
      if (AuthBasic_ToRealm(pszValue)) {
         pbhCur->pszRealm = pszValue;
         return TRUE;
      }

   } else if (me->bDoCookies && !STRICMP(cpszName,"Set-Cookie")) {
      CookieMgr_SetCookie(&me->ckm,pszValue,pbhCur->pszUrl);
   }

   return FALSE;
}

/**
  || Function
  || --------
  || static void Brewser_Header(void *p, const char *cpszName, 
  ||                              GetLine *pglVal)
  ||
  || Description
  || -----------
  || received header callback for a web transaction
  ||
  || Parameters
  || ----------
  || void *p: a Brewser (the subscriber)
  || const char *cpszName: the name of the web header (like "Content-Type")
  || GetLine *pglVal: the value of the header, like "text/html"
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  || cpszName is NULL in the case of continuation header line parts..
  ||
  */
static void Brewser_Header(void *p, const char *cpszName, GetLine *pglVal)
{
   Brewser *me = (Brewser *)p;
   
   HeaderMaker_Header(&me->hm,cpszName,pglVal);
}

/**
  || Function
  || --------
  || static void Brewser_WebStatus(void *p, WebStatus ws, void *pVal)
  ||
  || Description
  || -----------
  || web status callback for a Web transaction 
  ||
  || Parameters
  || ----------
  || void *p: a Brewser (the subscriber)
  || WebStatus ws: type of status
  || void *pVal: unused as of yet
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static void Brewser_WebStatus(void *p, WebStatus ws, void *pVal)
{
   uint16 wId; /* invalid */
   Brewser *me = (Brewser *)p;

   (void)pVal;

   switch (ws) {
   case WEBS_STARTING:
      wId = IDS_STATSTART;
      break;
   case WEBS_CANCELLED:
      wId = IDS_STATCANCEL;
      break;
   case WEBS_GETHOSTBYNAME:
      wId = IDS_STATGETHOSTBYNAME;
      break;          
   case WEBS_CONNECT:
      wId = IDS_STATCONNECT;
      break;
   case WEBS_SENDREQUEST:
      wId = IDS_STATSEND;
      break;
   case WEBS_READRESPONSE:
      wId = IDS_STATRECV;
      break;
//   case WEBS_CACHEHIT:
//      pszStatus = "** cache hit...\n";
//      break;

   default:
      return;
   }
   
   /* ASSERT wId has been assigned */
   /* show that status! */
   StatusBox_Update(&me->sbStatus,BREWSER_RES_FILE,wId);

}


static void Brewser_Stop2Back(Brewser *me) 
{
   CtlAddItem cai;

   ZEROAT(&cai);

   /* change 'Back' to 'Stop' button */
   cai.pszResImage = BREWSER_RES_FILE;
   cai.wImage      = IDB_BACK;
   cai.wItemID     = IDC_CANCEL;
   IMENUCTL_SetItem(me->piMenu, IDC_CANCEL, MSIF_IMAGE, &cai);

   Tooltip_Cancel(&me->ttTbTips);

   Brewser_Invalidate(me);
}

static void Brewser_Back2Stop(Brewser *me)
{
   CtlAddItem cai;
   
   ZEROAT(&cai);
   /* change 'Back' to 'Stop' button */
   cai.pszResImage = BREWSER_RES_FILE;
   cai.wImage      = IDB_STOP;
   cai.wItemID     = IDC_CANCEL;
   IMENUCTL_SetItem(me->piMenu, IDC_CANCEL, MSIF_IMAGE, &cai);
   /* set the selection to the 'Stop' button */
   IMENUCTL_SetSel(me->piMenu, IDC_CANCEL);
}


/**
  || Function
  || --------
  || static void Brewser_NavStop(Brewser *me)
  ||
  || Description
  || -----------
  || halts the current web transaction or cleans up after
  ||
  || Parameters
  || ----------
  || Brewser *me: the Brewser
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static void Brewser_NavStop(Brewser *me)
{
   /* this cancels any pending web transaction, or readable on the 
      response body.  if nothing is pending, this has no effect */
   CALLBACK_Cancel(&me->cbGotResp);
   
   /* then clean up, if necessary */
   RELEASEIF(me->piWResp); /* let response go */

}

static void Brewser_WhackCurHist(Brewser *me)
{
   BrewserHist *pbhCur = me->pbhCur;

   if ((BrewserHist *)0 == pbhCur) {
      return;
   }
   
   me->pbhCur = 0;
   
   if (pbhCur->bIsBack) {  /* don't trash backs, do them again sometime */
      pbhCur->bLoading = FALSE;
      pbhCur->bIsBack  = FALSE;
      pbhCur->bIsRefresh = FALSE;
   } else {
      BrewserHist_Delete(pbhCur);
   }

}


/**
  || Function
  || --------
  || static boolean Brewser_Stop(Brewser *me)
  ||
  || Description
  || -----------
  || stops any pending web transactions, uncompresses the URL just taken,
  ||   if necessary
  ||
  || Parameters
  || ----------
  || Brewser *me: the Brewser Applet
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static boolean Brewser_Stop(Brewser *me)
{
   BrewserHist *pbhCur = me->pbhCur;

   if ((Refresher *)0 != me->prfsher) {
      Refresher_Delete(me->prfsher);
      me->prfsher = 0;
   }

   StatusBox_Stop(&me->sbStatus);

   if ((BrewserHist *)0 == pbhCur) {
      return FALSE;
   }
   
   if (pbhCur->bLoading) {

      pbhCur->bStopped = TRUE; /* note we aborted */

      IHTMLVIEWER_Stop(me->piHtml);
      /* this calls me back (HVN_DONE), where I do Brewser_PushCurHist() */
      Brewser_TinyPage(me,IDX_STOPPED,"");

   } else {

      Brewser_Stop2Back(me);
      Brewser_NavStop(me);
      Brewser_WhackCurHist(me);

   }

   return TRUE;
}


static int Brewser_FillDialogV(Brewser *me, AEEResBlob *prb, int nLen,
                               uint16 uResId, const char *cpszzTokFormats, 
                               va_list args)
{
   int         nErr;
   AEEResBlob *prbData = prb;
   int         nSaveLen = nLen; /* because LoadResBlob overwrites */

   nErr = IShell_LoadResBlob(me->piShell, BREWSER_RES_FILE, uResId, 
                             &prbData,&nLen);

   if (SUCCESS == nErr) {
      if ((AEEResBlob *)0 == prb) { /* LoadResBlob malloc'd */
         if (0 < AEEResBlob_VStrRepl(&prbData,&nLen,cpszzTokFormats,args)) {
            nErr = ENOMEMORY;
         }
         prb = prbData;
      } else {
         char *pszData = RESBLOB_DATA(prb);
         /* do our best */
         vsnxprintf(pszData,nSaveLen-prb->bDataOffset,pszData,"{}",
                    cpszzTokFormats,args);
         prbData = 0;
      }
   }

   if (SUCCESS == nErr) {
      HtmlDlg_Fill(&me->htmldlg,prb->szMimeType,RESBLOB_DATA(prb),-1);
   }

   FREEIF(prbData);

   return nErr;
}

static int Brewser_FillDialog(void *p, AEEResBlob *prb, int nLen,
                              uint16 uResId, const char *cpszzTokFormats, ...)
{
   int nRet;
   va_list    args;

   va_start(args,cpszzTokFormats);
   nRet = Brewser_FillDialogV(p,prb,nLen,uResId,cpszzTokFormats,args);
   va_end(args);

   return nRet;
}

static void Brewser_WaitDialog_Cancel(AEECallback *pcb)
{
   Brewser     *me = (Brewser *)pcb->pCancelData;
   AEECallback **lfp;

   for (lfp = &me->pcbWaitDialog;pcb != *lfp;lfp = &(*lfp)->pNext);

   *lfp = pcb->pNext;
   pcb->pNext = 0;
   pcb->pfnCancel = 0;
}

static void Brewser_WaitDialog(void *p, AEECallback *pcb)
{
   Brewser      *me = (Brewser *)p; 
   AEECallback **lfp;

   CALLBACK_Cancel(pcb);
   for (lfp = &me->pcbWaitDialog;
        (AEECallback *)0 != *lfp;lfp = &(*lfp)->pNext);
   
   *lfp = pcb;
   pcb->pfnCancel = Brewser_WaitDialog_Cancel;
   pcb->pCancelData = me;
}

static int Brewser_StartDialog(void *p,PFNHVNJUMP pfnJump, void *pJumpData)
{
   Brewser *me = (Brewser *)p;

   if (HtmlDlg_IsActive(&me->htmldlg)) {
      return EALREADY;
   }
   
   IHTMLVIEWER_SetActive(me->piHtml,FALSE);
   IMENUCTL_SetActive(me->piMenu,FALSE);
   me->bShowingDialog = TRUE;
   HtmlDlg_Start(&me->htmldlg,pfnJump,pJumpData);

   return SUCCESS;
}

static void Brewser_EndDialog(void *p)
{
   Brewser *me = (Brewser *)p;
   HtmlDlg_End(&me->htmldlg);

   me->bShowingDialog = FALSE;
   if (me->bShowingToolbar) {
      IMENUCTL_SetActive(me->piMenu,TRUE);
   } else {
      IHTMLVIEWER_SetActive(me->piHtml,TRUE);
   }

   while ((AEECallback *)0 != me->pcbWaitDialog) {
      ISHELL_Resume(me->piShell,me->pcbWaitDialog);
   }
}

static void BrewserAuth_Dtor(BrewserAuth *me)
{
   CALLBACK_Cancel(&me->cb);
   FREEIF(me->pszStr);
   me->ap.pszUrl = me->ap.pszRealm = 0;
   me->ap.bProxy = FALSE;
   me->ap.nRet = SUCCESS;
}

static void BrewserAuth_Done(void *p)
{
   BrewserAuth *me = (BrewserAuth *)p;

   if (me->ap.nRet == SUCCESS) { /* not cancel */
      Brewser_Refresh(me->pbsr);
   } else if (me->ap.nRet != EFAILED) { /* show failure */
      BREWSER_FAILED(me->pbsr,me->ap.nRet);
   }
   BrewserAuth_Dtor(me);
}

static void BrewserAuth_CtorZ(BrewserAuth *me,Brewser *pbsr)
{
   CALLBACK_Init(&me->cb,BrewserAuth_Done,me);
   me->pbsr = pbsr;
}

static int BrewserAuth_Start(BrewserAuth *me, const char *cpszUrl,
                             const char *cpszRealm, boolean bProxy)
{
   UrlParts up;
   int      nUrlLen;
   int      nRealmLen;
   
   /* init */
   BrewserAuth_Dtor(me);
   
   if (bProxy) {
      const char *cpsz;
      int nErr;
      nErr = IWebUtil_GetProxyUrl(me->pbsr->piWebUtil,
                                  (IWebOpts *)me->pbsr->piWeb,
                                  cpszUrl,&cpsz);
      if (SUCCESS != nErr) {
         return nErr;
      }
      cpszUrl = cpsz;
  }

   nRealmLen = STRLEN(cpszRealm);         
   IWEBUTIL_ParseUrl(me->pbsr->piWebUtil,cpszUrl,&up);
   
   /* find last slash in path */
   {
      char *pc = MEMRCHR(up.cpcPath,'/',UP_PATHLEN(&up));
      
      if ((char *)0 != pc) {
         nUrlLen = pc+1 - cpszUrl;
      } else {
         nUrlLen = up.cpcPath+UP_PATHLEN(&up) - cpszUrl;
      }
   }
   
   me->pszStr = (char *)MALLOC(nUrlLen+1 + nRealmLen+1);
   
   if ((char *)0 == me->pszStr) {
      return ENOMEMORY;
   }
   me->ap.pszUrl = me->pszStr;
   MEMMOVE(me->pszStr,cpszUrl,nUrlLen);
   
   me->ap.pszRealm = me->ap.pszUrl+nUrlLen+1;
   MEMMOVE(me->ap.pszRealm,cpszRealm,nRealmLen);
   
   me->ap.bProxy = bProxy;

   AuthMgr_AuthPrompt(&me->pbsr->am,&me->ap,&me->cb);

   return SUCCESS;
}


static const char *Brewser_ErrToDesc(Brewser *me,int nErr,char **ppDescData)
{
   if (SUCCESS != 
       IShell_LoadResSz(me->piShell,BREWSER_RES_FILE,
                        IDS_ERRDESC_UNKNOWN+nErr,ppDescData) &&
       SUCCESS != 
       IShell_LoadResSz(me->piShell,BREWSER_RES_FILE,
                        IDS_ERRDESC_UNKNOWN,ppDescData)) {
      return "";
   } else {
      return *ppDescData;
   }
}


static void Brewser_NavFailed(Brewser *me, int nCode)
{
   const char *cpszDesc;
   char       *pDescData = 0;

   cpszDesc = Brewser_ErrToDesc(me,nCode,&pDescData);

   Brewser_ResPage(me,0,0,IDX_ERRPAGE,"err%d\0desc%Qs\0", nCode, cpszDesc);
   
   FREEIF(pDescData);
}



static void BrewserTrust_Dtor(BrewserTrust *me)
{
   CALLBACK_Cancel(&me->cb);

   FREEIF(me->xci.pszIssuerDN);
   FREEIF(me->xci.pszSubjectDN);
   ZEROAT(&me->xci.xbc);

   FREEIF(me->pszHost);
}

static void BrewserTrust_Jump(void *p, HViewJump *phvj)
{
   BrewserTrust *me = (BrewserTrust *)p;
   
   Brewser_EndDialog(me->pbsr);

   if (((HViewJump *)0 != phvj) && STRCMP(phvj->pszURL,"brewser:cancel")) {
      /* cancel */
      X509TrustOverride tov;
      WebOpt awoz[2];

      ZEROAT(&awoz);

      tov.uCertID = me->xci.xbc.uTrustOverrideID;
      tov.uOverrideBits = X509CHAIN_OVERRIDE_ALL;
      
      awoz[0].nId = WEBOPT_X509_OVERRIDE;
      awoz[0].pVal = &tov;

      IWEB_AddOpt(me->pbsr->piWeb,awoz);

      Brewser_Refresh(me->pbsr);
   }

   BrewserTrust_Dtor(me);
}


static void BrewserTrust_Dialog(void *p)
{
   BrewserTrust *me = (BrewserTrust *)p;
   int   nErr;

   nErr = Brewser_StartDialog(me->pbsr, BrewserTrust_Jump, me);

   if (EALREADY == nErr) {
      Brewser_WaitDialog(me,&me->cb);
   } else if (SUCCESS == nErr) {
      JulianType  dFrom;
      JulianType  dTo;
      char       *pDescData = 0;
      const char *cpszDesc;

      ZEROAT(&dFrom);
      GETJULIANDATE(me->xci.xbc.uStartValidity,&dFrom);

      ZEROAT(&dTo);
      GETJULIANDATE(me->xci.xbc.uEndValidity,&dTo);

      cpszDesc = Brewser_ErrToDesc(me->pbsr,me->nCode,&pDescData);
      
      Brewser_FillDialog(me->pbsr,0,0,IDX_CERTCHECK,
                         "err%d\0"
                         "desc%Qs\0"
                         "host%Qs\0"
                         "subjectdn%Qs\0"
                         "issuerdn%Qs\0"
                         "vfMM%d\0"
                         "vfDD%d\0"
                         "vfYYYY%d\0"
                         "vfHH%d\0"
                         "vfmm%d\0"
                         "vfss%d\0"
                         "vtMM%d\0"
                         "vtDD%d\0"
                         "vtYYYY%d\0"
                         "vtHH%d\0"
                         "vtmm%d\0"
                         "vtss%d\0",
                         me->nCode,
                         cpszDesc,
                         me->pszHost,
                         me->xci.pszSubjectDN,
                         me->xci.pszIssuerDN,
                         dFrom.wMonth,
                         dFrom.wDay,
                         dFrom.wYear,
                         dFrom.wHour,
                         dFrom.wMinute,
                         dFrom.wSecond,
                         dTo.wMonth,
                         dTo.wDay,
                         dTo.wYear,
                         dTo.wHour,
                         dTo.wMinute,
                         dTo.wSecond);
      FREEIF(pDescData);
   } else {
      BrewserTrust_Dtor(me);
      BREWSER_FAILED(me->pbsr,nErr);
   }
}



static int BrewserTrust_Start(BrewserTrust *me, int nCode,
                              X509CertInfo *pxci,
                              const char *cpcHost, int nHostLen)
{
   int nErr;

   /* init */
   BrewserTrust_Dtor(me);
   
   nErr = ERR_MALLOC(nHostLen+1,&me->pszHost);

   if (SUCCESS == nErr) {
      MEMMOVE(me->pszHost,cpcHost,nHostLen);
      
      me->xci.xbc = pxci->xbc;
      me->nCode = nCode;

      nErr = ERR_STRDUP(pxci->pszIssuerDN, &me->xci.pszIssuerDN);

      if (SUCCESS == nErr) {
         nErr = ERR_STRDUP(pxci->pszSubjectDN, &me->xci.pszSubjectDN);
      }
      
      if (SUCCESS == nErr) {
         BrewserTrust_Dialog(me);
      } else {
         BrewserTrust_Dtor(me);
      }
   }

   return nErr;
}

static void BrewserTrust_CtorZ(BrewserTrust *me,Brewser *pbsr)
{
   CALLBACK_Init(&me->cb,BrewserTrust_Dialog,me);
   me->pbsr = pbsr;
}

/**
  || Function
  || --------
  || static void Brewser_GotResp(void *p)
  ||
  || Description
  || -----------
  || web transaction callback, fires when the response body is ready to
  ||  be read, or when an error has occurred
  ||
  || Parameters
  || ----------
  || void *p: the Brewser
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static void Brewser_GotResp(void *p)
{
   Brewser     *me = (Brewser *)p;
   IHtmlViewer *piHtml = me->piHtml;
   WebRespInfo *pwri;
   BrewserHist *pbhCur = me->pbhCur;

   me->tWebElapsed = GET_UPTIMEMS() - me->tWebStart;

   /* get information about how the web transaction went...
      me->piWResp is ***NEVER NULL***, even though the transaction may fail
      for wont of memory
   */
   pwri = IWEBRESP_GetInfo(me->piWResp);

   pbhCur->nCode = pwri->nCode;
   FREEIF(pbhCur->pszContentType);
   if ((char *)0 != pwri->cpszContentType) {
      pbhCur->pszContentType = STRDUP(pwri->cpszContentType);
   }
   pbhCur->lContentLength = pwri->lContentLength;

   /* dig up ssl info, if any, cleanup first */
   if ((X509CertInfo *)0 != pbhCur->pxci) {
      X509CertInfo_Delete(pbhCur->pxci);
      pbhCur->pxci = 0;
   }
   {
      WebOpt      wo;

#define SSL_CRASH_WORKAROUND 
#ifdef SSL_CRASH_WORKAROUND
      /* begin work-around for SSL crash in BREW 2.0.0.x..

         If any error except "SUCCESS" or "TRUST problem", don't try to 
         get the cert info... (BREW 2.0.0.x crashes otherwise) 
      */
      if (SUCCESS ==
          IWEBRESP_GetOpt(me->piWResp, WEBOPT_SSL_SEC_INFO, 0, &wo) &&
          ((SSL_RESULT_SECURED == ((SSLInfo *)wo.pVal)->nResult) ||
           (SSL_RESULT_TRUST_ERR == ((SSLInfo *)wo.pVal)->nResult)))
#endif /* #ifdef CRASH_WORKAROUND */
      
      if (SUCCESS == 
          IWEBRESP_GetOpt(me->piWResp,WEBOPT_SSL_X509_CHAIN,0,&wo)) {

         IX509Chain *piCerts;
         /* don't have to Release() this, unless I AddRef() it.  It has
            the same lifetime as the webresp */
         piCerts = (IX509Chain *)wo.pVal;
         /* copy stuff I care about ;) */
         X509CertInfo_New(piCerts,WEBOPT_X509_CHAIN_CERT,0,
                          &pbhCur->pxci);
      }
   }

   switch (pwri->nCode) {
      /* don't show nothin' */
   case WEBCODE_NOTMODIFIED: /* 304 */
   case WEBCODE_NOCONTENT:  /* 204 */
      Brewser_Stop(me);
      return;
      
   case WEBCODE_MULTIPLECHOICES: /* 300 */
   case WEBCODE_MOVEDPERMANENTLY: /* 301 */
   case WEBCODE_FOUND: /* 302 */
   case WEBCODE_SEEOTHER: /* 303 */
   case WEBCODE_TEMPORARYREDIRECT: /* 307 */
      /*
        @@@ TODO??: Support temporary re-direct by remembering original 
                    request URL?  Neither IE nor Mozilla support it...
      */
      if ((Refresher *)0 != me->prfsher) {
         Refresher_Delete(me->prfsher);
         me->prfsher = 0;
      }
      if ((char *)0 != pbhCur->pszLocation) {
         int          nLen;
         char        *pszUrl;
         
         nLen = IWEBUTIL_MakeUrl(me->piWebUtil,pbhCur->pszUrl,
                                 pbhCur->pszLocation,0,0);
         
         if (SUCCESS != ERR_MALLOC(nLen,&pszUrl)) {
            BREWSER_FAILED(me,ENOMEMORY);
            return;
         }

         IWEBUTIL_MakeUrl(me->piWebUtil,pbhCur->pszUrl,
                          pbhCur->pszLocation,pszUrl,nLen);
         
         FREEIF(pbhCur->pszLocation);
         
         FREEIF(pbhCur->pszUrl);
         pbhCur->pszUrl = pszUrl;

         if (pwri->nCode != WEBCODE_TEMPORARYREDIRECT) {
            /* everything but 307 gets re-done as a "GET" */
            FREEIF(pbhCur->pszMethod);
            FREEIF(pbhCur->pszData);
         }

         Brewser_NavStop(me);
         Brewser_NavStart(me);
         return;
      }
      break;

   case WEBCODE_UNAUTHORIZED:      /* 401 */
   case WEBCODE_PROXYAUTHENTICATIONREQUIRED: /* 407 */
      if ((char *)0 != pbhCur->pszRealm) {
         int nErr;
         
         nErr = BrewserAuth_Start(&me->ba,pbhCur->pszUrl,pbhCur->pszRealm,
                                  (boolean)(pwri->nCode!=WEBCODE_UNAUTHORIZED));
         
         if (SUCCESS != nErr) {
            BREWSER_FAILED(me,nErr);
            return;
         }

         /* If we kickoff authentication, we'll eventually reload.
            In the meantime, fallthrough and show unauthenticated response 
         */
      }
      break;

   case -WEB_ERROR_SSL:
      {
         WebOpt      wo;
         SSLInfo    *pssli;
         int         nErr;
         
         nErr = IWEBRESP_GetOpt(me->piWResp, WEBOPT_SSL_SEC_INFO, 0, &wo);
         if (SUCCESS != nErr) {
            break;
         }

         pssli = (SSLInfo *)wo.pVal;
         
         pbhCur->nCode = pssli->nResult;
         
         /* If not trust error, bail */
         if (SSL_RESULT_TRUST_ERR != pssli->nResult) {
            if (SSL_RESULT_CERT_ERR == pssli->nResult) {
               /* use cert error if some other problem */
               pbhCur->nCode = pssli->TrustResult.nResult;
            }
            break;
         }
         
         DBGPRINTF("X509Trust error %d",pssli->TrustResult.nResult);
         
         /* need cert info in order to do the trust dialog */
         if ((X509CertInfo *)0 != pbhCur->pxci) {
            const char *cpszUrl;
            UrlParts    up;
            
            if (SUCCESS != IWebUtil_GetProxyUrl(me->piWebUtil, 
                                                (IWebOpts *)me->piWeb, 
                                                pbhCur->pszUrl, &cpszUrl)) {
               cpszUrl = pbhCur->pszUrl;
            }
            
            IWEBUTIL_ParseUrl(me->piWebUtil,cpszUrl,&up);
            
            BrewserTrust_Start(&me->bt,pbhCur->nCode,pbhCur->pxci,
                               up.cpcHost,UP_HOSTLEN(&up));
            
            /* If we kickoff trust check, we'll eventually reload.
               In the meantime, fallthrough and show SSL error response 
            */
         }
      }
   }

   /* body may be NULL, weird failure */
   
   IHTMLVIEWER_SetURL(piHtml, pbhCur->pszUrl);

   if ((ISource *)0 == pwri->pisMessage) {
      /* might have error text, if so, code below shows it.. */
      
      if (pbhCur->nCode < 0) {
         pbhCur->nCode = -pbhCur->nCode;
      }
      StatusBox_Stop(&me->sbStatus);
      Brewser_Stop2Back(me); /* set the menu back to 'Back' */
      Brewser_NavFailed(me,pbhCur->nCode);

      Brewser_PushCurHist(me);

   } else {
      pbhCur->bLoading = TRUE;

      /* save off viewer state, about to destroy it */
      Brewser_SaveViewState(me);

      IHTMLVIEWER_SetType(piHtml, pwri->cpszContentType);
      IHTMLVIEWER_LoadSource(piHtml,pwri->pisMessage);
   }

   Brewser_NavStop(me); /* stop cleans up */
}


static int Brewser_GetCurAuthInfo(Brewser *me)
{
   BrewserHist *pbhCur = me->pbhCur;
   const char  *cpszTemplate;
   const char  *cpszProxyCred;
   const char  *cpszWWWCred;

   {
      AuthInfo    *pai;

      pai = AuthMgr_FindAuthInfo(&me->am,pbhCur->pszUrl,FALSE,FALSE);
      
      if ((AuthInfo *)0 != pai) {
         cpszWWWCred = pai->pszCred;
      } else {
         cpszWWWCred = 0;
      }
   }

   {
      AuthInfo    *pai;
      const char *cpszProxyUrl;
      
      if (SUCCESS == 
          IWebUtil_GetProxyUrl(me->piWebUtil, (IWebOpts *)me->piWeb, 
                               pbhCur->pszUrl, &cpszProxyUrl)) {

         pai = AuthMgr_FindAuthInfo(&me->am,cpszProxyUrl,TRUE,FALSE);
      } else {
         pai = 0;
      }
      
      if ((AuthInfo *)0 != pai) {         
         cpszProxyCred = pai->pszCred;
      } else {
         cpszProxyCred = 0;
      }
   }

#define AUTH_WWW_TEMPLATE "Authorization: Basic {wwwcred}\r\n"
#define AUTH_PROXY_TEMPLATE "Proxy-Authorization: Basic {proxycred}\r\n"
#define AUTH_TOKFORMAT "wwwcred%s\0proxycred%s\0"
   
   if (((char *)0 != cpszProxyCred) && ((char *)0 != cpszWWWCred)) {
      cpszTemplate = AUTH_WWW_TEMPLATE AUTH_PROXY_TEMPLATE;
   } else if ((char *)0 != cpszWWWCred) {
      cpszTemplate = AUTH_WWW_TEMPLATE;
   } else if ((char *)0 != cpszProxyCred) {
      cpszTemplate = AUTH_PROXY_TEMPLATE;
   } else { /* nothing to do */
      return SUCCESS;
   }
   
   {
      int          nNeeded;
      
      nNeeded = snxprintf(0,0,
                          cpszTemplate,"{}",AUTH_TOKFORMAT,
                          cpszWWWCred,cpszProxyCred);
      
      pbhCur->pszAuth = (char *)MALLOC(nNeeded);
      
      if ((char *)0 == pbhCur->pszAuth) {
         return ENOMEMORY;
      }
      
      snxprintf(pbhCur->pszAuth,nNeeded,
                cpszTemplate,"{}",AUTH_TOKFORMAT,
                cpszWWWCred,cpszProxyCred);
   }

   return SUCCESS;
}

static void BrewserHist_NavStart(BrewserHist *me)
{
   me->bStopped = FALSE;
   FREEIF(me->pszAuth);
   FREEIF(me->pszLocation);
   FREEIF(me->pszRealm);
   me->lContentLength = -1;
   FREEIF(me->pszContentType);
}


static boolean Brewser_HandleSpecialUrls(Brewser *me, UrlParts *pup)
{
   /* handle mailto rewrite */
   if ((CSTRLEN("mailto:") == UP_SCHMLEN(pup)) &&
       !STRNICMP(pup->cpcSchm,"mailto:",CSTRLEN("mailto:")) &&
       ((char *)0 != me->pszMailTo) && (0 != me->pszMailTo[0])) {
      char *pszUrl;
      int   nLen = STRLEN(me->pszMailTo) + STRLEN(pup->cpcPath) + 1;

      if (SUCCESS == ERR_MALLOC(nLen,&pszUrl)) {
         SNPRINTF(pszUrl,nLen,"%s%s",me->pszMailTo,pup->cpcPath);
         /* re-target mailto... */
         FREEIF(me->pbhCur->pszUrl);
         me->pbhCur->pszUrl = pszUrl;
         /* re-parse url... */
         IWEBUTIL_ParseUrl(me->piWebUtil,pszUrl,pup);   
      }

      return FALSE; /* let them continue */
   }

   /* handle "brewser:" urls */
   if ((CSTRLEN("brewser:") == UP_SCHMLEN(pup)) &&
       !STRNICMP(pup->cpcSchm,"brewser:",CSTRLEN("brewser:"))) {

      if (!MEMCMP(pup->cpcPath,"back",UP_PATHLEN(pup))) {
         const char *cpszTo = 0;
         const char *cpszPast = 0;
         char *pszSrch = 0;      
         
         if (*pup->cpcSrch == '?') {
            pszSrch = STRDUP(pup->cpcSrch+1);
            
            if ((char *)0 != pszSrch) {
               IWebUtil_GetFormFields(me->piWebUtil,pszSrch,
                                      "to\0past\0",
                                      &cpszTo,&cpszPast);
            }
         }
         
         /* Brewser_Back() is careful about this url, does Stop() late */
         Brewser_Back(me,cpszTo,cpszPast);
         
         FREEIF(pszSrch);
         
         return TRUE;
      }

      if (!MEMCMP(pup->cpcPath,"refresh",UP_PATHLEN(pup))) {
         Brewser_Refresh(me);
         return TRUE;
      }
      return FALSE;
   }

   {
      char *pszSchm;

      pszSchm = (char *)MALLOC(UP_SCHMLEN(pup));
      
      if ((char *)0 != pszSchm) {
         AEECLSID clsId;
         
         MEMMOVE(pszSchm,pup->cpcSchm,UP_SCHMLEN(pup)-1);

         clsId = ISHELL_GetHandler(me->piShell,HTYPE_BROWSE,pszSchm);

         FREE(pszSchm);
         
         if ((0 != clsId) && (AEECLSID_BREWSER != clsId)) {
            ISHELL_BrowseURL(me->piShell,pup->cpcSchm);
            Brewser_Stop(me);
            return TRUE;
         }
      }
   }

   return FALSE;
}


static boolean Brewser_JustNamedPos(Brewser *me, UrlParts *pup)
{
   BrewserHist *pbhCur = me->pbhCur;
   const char *pszFrom;
   UrlParts    upFrom;

   if (!UP_HASNAME(pup)) {
      return FALSE;
   }

   if ((pbhCur == me->pbhList) ||  /* doing a refresh */
       me->pbhList->bStopped ||  /* or didn't complete last time */
       ((char *)0 != pbhCur->pszData) || /* or there's data to send */
       ((char *)0 != pbhCur->pszMethod)) { /* or method's non-idempotent */
      /* bail, not just named position */
      return FALSE;
   }
   
   pszFrom = me->pbhList->pszUrl;
   IWEBUTIL_ParseUrl(me->piWebUtil,pszFrom,&upFrom);

   if (((UP_URLLEN(&upFrom) - UP_NAMELEN(&upFrom)) !=
        (UP_URLLEN(pup) - UP_NAMELEN(pup))) ||
       MEMCMP(pszFrom,pbhCur->pszUrl,UP_URLLEN(pup)-UP_NAMELEN(pup))) {
      /* if the Url differs before the named pos */
      return FALSE;
   }

   /* save off viewer state, about to destroy it */
   Brewser_SaveViewState(me);
   
   /* don't do transaction, just adjust named position */
   pbhCur->nCode = me->pbhList->nCode;
   pbhCur->pszContentType = STRDUP(me->pbhList->pszContentType);
   pbhCur->lContentLength = me->pbhList->lContentLength;
   
   IHTMLVIEWER_SetURL(me->piHtml, pbhCur->pszUrl);
   if ((char *)0 != pbhCur->pszViewState) {
      IHTMLVIEWER_SetViewState(me->piHtml,pbhCur->pszViewState);
   } else {
      /* scroll to top, no selection */
      IHTMLVIEWER_SetViewState(me->piHtml,"0,-1");
   }
   StatusBox_Stop(&me->sbStatus);
   Brewser_Stop2Back(me);
   Brewser_PushCurHist(me);

   return TRUE;
}

static void Brewser_NavStart(Brewser *me)
{
   UrlParts     up;
   BrewserHist *pbhCur = me->pbhCur;
   WebOpt       awo[12]; /* @@@ this size has to be updated if anything below
                            changes (new webopt passed to GetResponse()*/
   int          nOptIdx = 0; /* index into awo */
   ISource     *pisBody = 0;

   if (BREWSER_ASSERT(me,(char *)0 != pbhCur->pszUrl)) {
      return;
   }

   /* initialize the history entry */
   BrewserHist_NavStart(pbhCur);

   IWEBUTIL_ParseUrl(me->piWebUtil,pbhCur->pszUrl,&up);

   if (Brewser_HandleSpecialUrls(me,&up)) {
      return;
   }

   /* named position optimization */
   if (Brewser_JustNamedPos(me,&up)) {
      return;
   }

   /* I wanna be notified of headers, status */
   awo[nOptIdx].nId  = WEBOPT_HANDLERDATA;
   awo[nOptIdx].pVal = me;
   nOptIdx++;
   
   /* call me at Brewser_Status with status */
   awo[nOptIdx].nId  = WEBOPT_STATUSHANDLER;
   awo[nOptIdx].pVal = (void *)Brewser_WebStatus;
   nOptIdx++;
   
   awo[nOptIdx].nId  = WEBOPT_HEADERHANDLER;
   awo[nOptIdx].pVal = (void *)Brewser_Header;
   nOptIdx++;
   
   /* create a post body */
   if ((char *)0 != pbhCur->pszData) {
      int      nErr;
      int      nLen = STRLEN(pbhCur->pszData);
      
      nErr = ISOURCEUTIL_SourceFromMemory(me->piSourceUtil,
                                          pbhCur->pszData, nLen,
                                          0,0,&pisBody);

      if (SUCCESS == nErr) {
         awo[nOptIdx].nId  = WEBOPT_BODY;
         awo[nOptIdx].pVal = pisBody;
         nOptIdx++;

         awo[nOptIdx].nId  = WEBOPT_HEADER;
         awo[nOptIdx].pVal = (void *)"Content-Type: application/x-www-form-urlencoded\r\n";
         nOptIdx++;

         awo[nOptIdx].nId  = WEBOPT_CONTENTLENGTH;
         awo[nOptIdx].pVal = (void *)nLen;
         nOptIdx++;
      }
   }
   /* provide method */
   if ((char *)0 != pbhCur->pszMethod) {
      awo[nOptIdx].nId  = WEBOPT_METHOD;
      awo[nOptIdx].pVal = pbhCur->pszMethod;
      nOptIdx++;
   }

   /* request flags... */   
   {
      uint32 uReqFlags = 0;
      
      if (me->bNoCache) {
         uReqFlags |= WEBREQUEST_NOCACHE;
      }

      if (pbhCur->bIsRefresh) { /* force a new cache entry */
         uReqFlags |= WEBREQUEST_CACHEINVALIDATE;
      } 

      if (!pbhCur->bIsBack) { /* accept expired pages for back */
         uReqFlags |= WEBREQUEST_CACHECHECKEXPIRES;
      }

      if ((IWebCache *)0 != me->piWebCache) {
         /* don't give the cache the "no cache" flag, 
            I want to apply it to this request only....
            the rest of these flags stick until the next
            navigation, and so apply to inline images!
         */
         IWEBCACHE_SetFlags(me->piWebCache,uReqFlags);
      }

      awo[nOptIdx].nId  = WEBOPT_FLAGS;
      awo[nOptIdx].pVal = (void *)uReqFlags;
      nOptIdx++;
   }

   /* find any auth info */   
   { 
      if (SUCCESS != Brewser_GetCurAuthInfo(me)) {
         BREWSER_FAILED(me,ENOMEMORY);
         return;
      }
      
      /* contains both WWW and Proxy auth */
      if ((char *)0 != pbhCur->pszAuth) { 
         awo[nOptIdx].nId  = WEBOPT_HEADER;
         awo[nOptIdx].pVal = pbhCur->pszAuth;
         nOptIdx++;
      }
   }
   
   if (pbhCur->pszReferer) {
      awo[nOptIdx].nId  = WEBOPT_HEADER;
      awo[nOptIdx].pVal = pbhCur->pszReferer;
      nOptIdx++;
   }

   FREEIF(pbhCur->pszCookie);

   if (me->bDoCookies) {
      int nLen = CookieMgr_GetCookies(&me->ckm,&up,0,0);

      pbhCur->pszCookie = (char *)MALLOC(nLen);

      if ((char *)0 != pbhCur->pszCookie) {
         nLen = CookieMgr_GetCookies(&me->ckm,&up,pbhCur->pszCookie,nLen);
         if (0 != nLen) {
            awo[nOptIdx].nId  = WEBOPT_HEADER;
            awo[nOptIdx].pVal = pbhCur->pszCookie;
            nOptIdx++;
         }
      }
   }
   
   /* done */
   awo[nOptIdx].nId = WEBOPT_END;
   
   me->tWebStart = GET_UPTIMEMS();
   
   Brewser_Back2Stop(me);

   /* display status */
   StatusBox_Start(&me->sbStatus);

   IWEB_GetResponseV(me->piWeb, &me->piWResp, &me->cbGotResp, 
                     pbhCur->pszUrl,awo);

   /* safe to release this, should have been copied out, AddRef'd */
   RELEASEIF(pisBody);
   
}

static void Brewser_ShowBusyImage(Brewser *me)
{
   IImage *piBusy;
         
   piBusy = ISHELL_LoadResImage(me->piShell, 
                                AEECONTROLS_RES_FILE, 
                                AEE_IDB_BUSY);
         
   if ((IImage *)0 != piBusy) {
      AEEImageInfo  ii;
      AEEDeviceInfo di;
      int16 nX, nY;
            
      ZEROAT(&di);
            
      di.wStructSize = sizeof(di);
            
      ISHELL_GetDeviceInfo(me->piShell, &di);
            
      IIMAGE_GetInfo(piBusy, &ii);
            
      nX = (di.cxScreen - ii.cxFrame) / 2;
      nY = (di.cyScreen - ii.cy) / 2;
            
      if (di.nColorDepth > 2) {
         IIMAGE_SetParm(piBusy, IPARM_ROP, AEE_RO_MASK,0);
      }

      IIMAGE_Draw(piBusy, nX, nY);
      IDISPLAY_UpdateEx(me->piDisplay,FALSE);
   }
   RELEASEIF(piBusy);
}

static void Brewser_ExitPromptJump(void *p, HViewJump *phvj) 
{
   Brewser *me = (Brewser *)p;

   Brewser_EndDialog(me);

   if (((HViewJump *)0 == phvj) || !STRCMP(phvj->pszURL,"brewser:cancel")) {
      /* cancel */
      return;
   }

   Brewser_Redraw(me);
   Brewser_ShowBusyImage(me);
   ISHELL_CloseApplet(me->piShell,FALSE);
}

static void Brewser_ExitPrompt(void *p)
{
   Brewser *me = (Brewser *)p;
   union {
      AEEResBlob rb;
      byte       buf[512];
   } resBlob;

   ZEROAT(&resBlob);

   Brewser_EndDialog(me); /* force me to front */
   /* ignore error */
   Brewser_StartDialog(me,Brewser_ExitPromptJump,me);
   Brewser_FillDialog(me,&resBlob.rb,sizeof(resBlob),IDX_EXITPROMPT,"");
}

static void Brewser_Back(Brewser *me, const char *cpszTo, 
                         const char *cpszPast)
{
   BrewserHist *pbh = 0;

   if ((char *)0 != cpszTo) {
      for (pbh = me->pbhList; 
           ((BrewserHist *)0 != pbh) && STRCMP(pbh->pszUrl,cpszTo);
           pbh = pbh->pPrev);
   }

   /* couldn't find the "to", try for "past" */
   if (((BrewserHist *)0 == pbh) && ((char *)0 != cpszPast)) {
      
      for (pbh = me->pbhList; 
           (pbh != &me->bhBottom) && 
              STRBEGINS(cpszPast,pbh->pszUrl);
           pbh = pbh->pPrev);
      
   }

   /* couldn't find or didn't have "to", or "past" */
   if ((BrewserHist *)0 == pbh) {

      pbh = me->pbhList->pPrev;
      
      if ((BrewserHist *)0 == pbh) {
         /* might not be going anywhere, though */
         Brewser_Stop(me);
         Brewser_ExitPrompt(me);
         return;
      }
   } 

   /* moved this to late so that we could use cpszToWhere as a
      pointer into pbhCur, which gets freed on Stop() */
   Brewser_Stop(me);

   me->pbhCur = pbh;
   pbh->bIsBack = TRUE;

   Brewser_NavStart(me);

}

static void Brewser_PushCurHist(Brewser *me)
{
   BrewserHist *pbhCur = me->pbhCur;

   if (pbhCur == 0) {
      return;
   }

   /* I've allocated some memory to get here, register a low ram 
      callback if I haven't already */
   if (!CALLBACK_IsQueued(&me->cbLowRAM)) {
      ISHELL_RegisterSystemCallback(me->piShell,&me->cbLowRAM,
                                    AEE_SCB_LOW_RAM);
   }

   me->pbhCur = 0;

   if (!pbhCur->bIsBack) { /* forward navigations added to hist list */
      pbhCur->pPrev = me->pbhList;
      me->pbhList = pbhCur;
   } 
   else { /* back navigations trash all but themselves on history list */
      
      BrewserHist *pbhFree;
      
      /* trim history down to pbhCur */
      while ((&me->bhBottom != (pbhFree = me->pbhList)) &&
             (pbhCur != pbhFree)) {
         me->pbhList = pbhFree->pPrev;
         BrewserHist_Delete(pbhFree);
      }
   }

   pbhCur->bLoading   = FALSE;
   pbhCur->bIsBack    = FALSE;
   pbhCur->bIsRefresh = FALSE;

}


static BrewserHist *BrewserHist_New(UrlParts *pupUrl,
                                    const char *cpszMethod,
                                    const char *cpszData, 
                                    const char *cpszReferer)
{
   BrewserHist *me;
   const char *cpszUrl = pupUrl->cpcSchm;

   me = MALLOCREC(BrewserHist);

   if ((BrewserHist *)0 != me) {
      if ((const char *)0 != cpszUrl) {
         me->pszUrl = STRDUP(cpszUrl);

         if ((char *)0 == me->pszUrl) {
            goto bail;
         }
      }

      /* @@@ TODO: take out the STRICMP(cpszMethod, "POST") to support 
         viewers that support other methods
      */
      if (((const char *)0 != cpszMethod) && !STRICMP(cpszMethod,"POST")) {

         me->pszMethod = STRDUP(cpszMethod);

         if ((char *)0 == me->pszMethod) {
            goto bail;
         }

         /* @@@ TODO: take this out when the html viewer takes care of this 
                      for us 
         */
         STRUPPER(me->pszMethod);
      }
      
      if ((const char *)0 != cpszData) {
         me->pszData = STRDUP(cpszData);
         if ((char *)0 == me->pszData) {
            goto bail;
         }
      }

      if ((const char *)0 != cpszReferer) {
         int nLen;
                     
         nLen = CSTRLEN("Referer: ")+STRLEN(cpszReferer)+CSTRLEN("\r\n")+1;
         
         me->pszReferer = (char *)MALLOC(nLen);

         if ((char *)0 != me->pszReferer) {
            SNPRINTF(me->pszReferer,nLen,"Referer: %s\r\n",cpszReferer);
         }
      }

      if (UP_HASNAME(pupUrl)) {
         me->pszViewState = STRDUP(pupUrl->cpcName);
      }

      goto done;

   bail:
      FREEIF(me->pszData);
      FREEIF(me->pszMethod);
      FREEIF(me->pszUrl);
      FREE(me);
   }

 done:
   return me;

}

static void Brewser_Jump(Brewser *me, HViewJump *phvj, 
                         const char *cpszReferer)
{
   UrlParts     up;
   BrewserHist *pbh;
   
   Brewser_Stop(me);

   IWEBUTIL_ParseUrl(me->piWebUtil, phvj->pszURL, &up);

   /* more named position special cases:

      If we're going to the same URL as we're currently viewing, don't 
      get a new history entry.  If there's a named position, just update
      the viewer again.  If there's no named position, do a refresh.
    */
   {
      BrewserHist *pbh = me->pbhList;
      
      if (!pbh->bStopped &&  /* fully loaded */
          !STRCMP(phvj->pszURL,pbh->pszUrl) && /* same URL */
          (((char *)0 == phvj->pszMethod) || 
           !STRICMP(phvj->pszMethod,"GET")) && /* is a "GET" */
          ((char *)0 == pbh->pszMethod)) { /* was a "GET" */

         if (UP_HASNAME(&up) &&  /* has named position */
             !UP_HASSRCH(&up) &&  /* no search string */
             ((char *)0 == phvj->pszData)) {  /* no data to send */

            /*  reset named pos, no transaction */
            IHTMLVIEWER_SetViewState(me->piHtml,up.cpcName);
         } else {

            FREEIF(pbh->pszData);
            pbh->pszData = STRDUP(phvj->pszData);

            Brewser_Refresh(me);
         }
         return;
      }
   }

   pbh = BrewserHist_New(&up,phvj->pszMethod,phvj->pszData,
                         cpszReferer);
   
   if ((BrewserHist *)0 != pbh) {
      
      me->pbhCur = pbh;
      /* kickoff */
      Brewser_NavStart(me);
   } else {
      BREWSER_FAILED(me,ENOMEMORY);
   }
}

/**
  || Function
  || --------
  || static void Brewser_Redraw(void *p)
  ||
  || Description
  || -----------
  || redraws everything in BREWser
  ||
  || Parameters
  || ----------
  || void *p: the Brewser
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  || It should never be necessary to call this function directly, call
  ||   Brewser_Invalidate() instead
  ||
  */
static void Brewser_Redraw(void *p)
{
   Brewser *me = (Brewser *)p;

   if (me->bSuspended || 
       ((IDialog *)0 != ISHELL_GetActiveDialog(me->piShell))) {
      return;
   }

   if (!Slider_Sliding(&me->sTb)) {
      me->bInRedraw = TRUE;
      IHTMLVIEWER_Redraw(me->piHtml);
      me->bInRedraw = FALSE;
   } else {
      AEERect rc = me->rcScreen;
      rc.y   = me->nTbSaveY;
      rc.dy -= me->nTbSaveY;
      IDISPLAY_EraseRect(me->piDisplay,&rc);
   }

   IMENUCTL_Redraw(me->piMenu);

   StatusBox_Redraw(&me->sbStatus);
   Tooltip_Redraw(&me->ttTbTips);
   HtmlDlg_Redraw(&me->htmldlg);
   
   CALLBACK_Cancel(&me->cbRedraw);

   IDISPLAY_Update(me->piDisplay);
}

static void Brewser_Invalidate(void *p)
{
   Brewser *me = (Brewser *)p;
   
   if (!CALLBACK_IsQueued(&me->cbRedraw)) {
      ISHELL_Resume(me->piShell, &me->cbRedraw);
   }
}

static void Brewser_MetaHeaders(Brewser *me, IHtmlViewer *piHtml)
{
   int32 nElemLoc;
   int32 nElemIdx;
   char *pszHeader = 0;
   char *pszValue = 0;
   const char *cpszzHeaders;

   cpszzHeaders = HeaderMaker_SetHeaders(&me->hm,
                                         "Set-Cookie\0"
                                         "Refresh\0");

   for (nElemIdx = 0; 
        0 <= (nElemLoc = IHTMLVIEWER_FindElem(piHtml,"meta", nElemIdx));
        nElemIdx++) {

      if ((SUCCESS != IHtmlViewer_GetAttrValue(piHtml,
                                               nElemLoc,
                                               "http-equiv",
                                               &pszHeader)) ||
          (SUCCESS != IHtmlViewer_GetAttrValue(piHtml,
                                               nElemLoc,
                                               "content",
                                               &pszValue))) {
         continue; /* can't do this one... */
      }

      if (Brewser_HeaderMade(me,pszHeader,pszValue)) {
         pszValue = 0; /* they took it */
      }
   }

   FREEIF(pszHeader);
   FREEIF(pszValue);
   
   HeaderMaker_SetHeaders(&me->hm,cpszzHeaders);
}

static void Brewser_RefresherPromptJump(void *p, HViewJump *phvj)
{
   Brewser *me = (Brewser *)p;

   Brewser_EndDialog(me);

   if (((HViewJump *)0 == phvj) || !STRCMP(phvj->pszURL,"brewser:cancel")) {
      /* cancel */
      return;
   }

   if ((Refresher *)0 != me->prfsher) {
      Refresher_Fire(me->prfsher);
   }
}

static void Brewser_Refresher(void *p, const char *cpszRefreshUrl)
{
   Brewser *me = (Brewser *)p;
   uint32   tIdle = GETTIMESECONDS() - me->tLastUserEvent;

   if (tIdle > BREWSER_REFRESH_IDLE) {
      Brewser_EndDialog(me);
      Brewser_StartDialog(me,Brewser_RefresherPromptJump,me);
      Brewser_FillDialog(me,0,0,IDX_REFRESHPROMPT,"idleseconds%d\0",tIdle);
      return;
   }

   if (((char *)0 != cpszRefreshUrl) && ('\0' != *cpszRefreshUrl)) {
      int   nLen;
      char *pszUrl;
      BrewserHist *pbh = me->pbhList;

      nLen = IWEBUTIL_MakeUrl(me->piWebUtil,pbh->pszUrl,
                              cpszRefreshUrl,0,0);
      
      pszUrl = (char *)MALLOC(nLen);
      
      if ((char *)0 == pszUrl) {
         /* can't handle url allocation failure, just bail */
         return;
      }
      
      IWEBUTIL_MakeUrl(me->piWebUtil,pbh->pszUrl,cpszRefreshUrl,
                       pszUrl,nLen);
      
      FREEIF(pbh->pszReferer);
      pbh->pszReferer = pbh->pszUrl;
      pbh->pszUrl = pszUrl;
   }

   Brewser_Refresh(me);
}

static void Brewser_HViewNotify(void *p, HViewNotify *phvn)
{
   Brewser *me = (Brewser *)p;

   switch (phvn->code) {
   case HVN_REDRAW_SCREEN:
   case HVN_INVALIDATE:
      if (!me->bInRedraw) {
         Brewser_Invalidate(me);
      }
      break;

   case HVN_JUMP:
   case HVN_SUBMIT:
      Brewser_Jump(me, &phvn->u.jump,me->pbhList->pszUrl);
      break;
      
   case HVN_FULLSCREEN_EDIT:
      break;

   case HVN_DONE:
      /* before pushing the current history, pull any HTTP headers 
         out of the document */
      Brewser_MetaHeaders(me,me->piHtml);

      if ((Refresher *)0 != me->prfsher) {
         Refresher_Start(me->prfsher, me->piShell, Brewser_Refresher, me);
      }
      
      IHTMLVIEWER_SetViewState(me->piHtml,me->pbhCur->pszViewState);

      StatusBox_Stop(&me->sbStatus);
      Brewser_Stop2Back(me);
      Brewser_PushCurHist(me);

      break;
   }
}

static boolean Brewser_InProgress(Brewser *me)
{
   return ((BrewserHist *)0 != me->pbhCur);
}

static const struct {
   uint16 uItemId;
   uint16 uImageResId;
   uint16 uTooltipResId;
} auSoftKeyInfo[] = {
   { IDC_CANCEL   , IDB_BACK     , IDS_BACK      }, /* 'Back' button */
   { IDC_REFRESH  , IDB_REFRESH  , IDS_REFRESH   }, /* 'Refresh' button */
   { IDC_GOTO     , IDB_GOTO     , IDS_GOTO      }, /* 'Go' button */
   { IDC_INFO     , IDB_INFO     , IDS_INFO      }, /* 'Info' button */
   { IDC_BOOKMARKS, IDB_BOOKMARKS, IDS_BOOKMARKS }, /* 'Bookmarks' button */
   { IDC_HELP     , IDB_HELP     , IDS_HELP      }, /* 'Help' button */
   { IDC_OPT      , IDB_OPT      , IDS_OPT       }, /* 'Options' button */
};

static void Brewser_SetupSoftKey(Brewser *me)
{
   CtlAddItem cai;
   int        i;

   ZEROAT(&cai);
   /* clear the softkey menu */
   IMENUCTL_DeleteAll(me->piMenu);
   
   cai.pszResImage = BREWSER_RES_FILE;
   
   for (i = 0; i < ARRAY_SIZE(auSoftKeyInfo); i++) {
      cai.wItemID     = auSoftKeyInfo[i].uItemId;
      cai.wImage      = auSoftKeyInfo[i].uImageResId;
      IMENUCTL_AddItemEx(me->piMenu, &cai);
   }
}

static void Brewser_TooltipPush(Brewser *me)
{
   uint16 uResId;
   uint16 uItemId;
   int    i;

   uItemId = IMENUCTL_GetSel(me->piMenu);

   for (i = 0; (i < ARRAY_SIZE(auSoftKeyInfo)) && 
           (uItemId != auSoftKeyInfo[i].uItemId); i++);

   if (i >= ARRAY_SIZE(auSoftKeyInfo)) {
      return;
   }

   uResId = auSoftKeyInfo[i].uTooltipResId;

   if ((uResId == IDS_BACK) && Brewser_InProgress(me)) {
      uResId = IDS_STOP;
   }

   Tooltip_Push(&me->ttTbTips,BREWSER_RES_FILE,uResId,
                (i*100)/(ARRAY_SIZE(auSoftKeyInfo)-1));
}

static boolean Brewser_BackButton(Brewser *me)
{
   if (!Brewser_Stop(me)) {
      Brewser_Back(me,0,0);
   }
   return TRUE;
}

static void Brewser_Refresh(Brewser *me)
{
   Brewser_Stop(me);
   me->pbhCur = me->pbhList;
   me->pbhCur->bIsBack = me->pbhCur->bIsRefresh = TRUE;

   Brewser_NavStart(me);
}

static void Brewser_Help(Brewser *me)
{
   HViewJump hvj;

   ZEROAT(&hvj);
   hvj.pszURL = "res:/"BREWSER_RES_FILE"?2001";

   Brewser_Jump(me, &hvj,0);
}

static void Brewser_InfoDlgJump(void *p, HViewJump *phvj)
{
   Brewser *me = (Brewser *)p;

   Brewser_EndDialog(me);

   if ((HViewJump *)0 == phvj) {
      return;
   }
   
   Brewser_Jump(me,phvj,0);
}

static boolean IsInternalUrl(IWebUtil *piWebUtil, const char *cpszUrl)
{
   UrlParts up;

   IWEBUTIL_ParseUrl(piWebUtil,cpszUrl,&up);

   return (!STRNICMP(up.cpcSchm,"brewser:",UP_SCHMLEN(&up)) ||
           !STRNICMP(up.cpcSchm,"lcgi:",UP_SCHMLEN(&up)));
}

static void Brewser_Info(void *p)
{
   Brewser *me = (Brewser *)p;
   char szContentLength[16];
   BrewserHist *pbh = me->pbhList;
   char *pszTitle;
   char *pszContentType;
   char *pszContentLength;
   uint16 uResId;
   char *pszIssuerDN = "";
   char *pszSubjectDN = "";
   JulianType dFrom;
   JulianType dTo;

   /* must freeif() these */
   char *pTitleMem = 0;
   char *pUnknownMem = 0;

   ZEROAT(&dFrom);   
   ZEROAT(&dTo);

   if (EALREADY == Brewser_StartDialog(me, Brewser_InfoDlgJump, me)) {
      Brewser_WaitDialog(me,&me->cbInfoDialog);
      return;
   }

   if (IsInternalUrl(me->piWebUtil,pbh->pszUrl)) {
      uResId = IDX_INTERNALPAGEINFO;
   } else if ((X509CertInfo *)0 != pbh->pxci) {
      uResId = IDX_SSLPAGEINFO;

      GETJULIANDATE(pbh->pxci->xbc.uStartValidity,&dFrom);
      GETJULIANDATE(pbh->pxci->xbc.uEndValidity,&dTo);
      pszIssuerDN = pbh->pxci->pszIssuerDN;
      pszSubjectDN = pbh->pxci->pszSubjectDN;
   } else {
      uResId = IDX_PAGEINFO;
   }

   IHtmlViewer_GetElemText(me->piHtml,"title",0,&pTitleMem);
   
   if ((char *)0 == pTitleMem) {
      IShell_LoadResSz(me->piShell,BREWSER_RES_FILE,IDS_UNTITLED,&pTitleMem);
   }
   pszTitle = pTitleMem;
   
   if (-1 != pbh->lContentLength) {
      SPRINTF(szContentLength,"%d",pbh->lContentLength);
      pszContentLength = szContentLength;
   } else {
      if ((char *)0 == pUnknownMem) {
         IShell_LoadResSz(me->piShell,BREWSER_RES_FILE,IDS_UNKNOWN,
                          &pUnknownMem);
      }
      pszContentLength = pUnknownMem;
   }
   
   if ((char *)0 != pbh->pszContentType) {
      pszContentType = pbh->pszContentType;
   } else {
      if ((char *)0 == pUnknownMem) {
         IShell_LoadResSz(me->piShell,BREWSER_RES_FILE,IDS_UNKNOWN,
                          &pUnknownMem);
      }
      pszContentType = pUnknownMem;
   }
   
   Brewser_FillDialog(me,0,0,uResId,
                      "title%Qs\0"
                      "url%Qs\0"
                      "contenttype%Qs\0"
                      "contentlength%s\0"
                      "subjectdn%Qs\0"
                      "issuerdn%Qs\0"
                      "vfMM%d\0"
                      "vfDD%d\0"
                      "vfYYYY%d\0"
                      "vfHH%d\0"
                      "vfmm%d\0"
                      "vfss%d\0"
                      "vtMM%d\0"
                      "vtDD%d\0"
                      "vtYYYY%d\0"
                      "vtHH%d\0"
                      "vtmm%d\0"
                      "vtss%d\0",
                      pszTitle?pszTitle:"???",
                      pbh->pszUrl,
                      pszContentType?pszContentType:"???",
                      pszContentLength?pszContentLength:"???",
                      pszSubjectDN,
                      pszIssuerDN,
                      dFrom.wMonth,
                      dFrom.wDay,
                      dFrom.wYear,
                      dFrom.wHour,
                      dFrom.wMinute,
                      dFrom.wSecond,
                      dTo.wMonth,
                      dTo.wDay,
                      dTo.wYear,
                      dTo.wHour,
                      dTo.wMinute,
                      dTo.wSecond);

   FREEIF(pUnknownMem);
   FREEIF(pTitleMem);
}


static boolean Brewser_HtmlModeHandleEvent(Brewser *me,AEEEvent eCode, 
                                           uint16 wParam, uint32 dwParam)
{
   if (HtmlDlg_HandleEvent(&me->htmldlg, eCode, wParam, dwParam)) {
      return TRUE;
   } else if (IHTMLVIEWER_HandleEvent(me->piHtml, eCode, wParam, dwParam)) {
      return TRUE;
   } else if (!me->bSuspended && /* have to keep IMENUCTL from drawing */
              IMENUCTL_HandleEvent(me->piMenu, eCode, wParam, dwParam)) {
      Brewser_Redraw(me);
      return TRUE;
   }

   switch (eCode) {
   case EVT_COMMAND:
      switch (wParam) {
      case IDC_GOTO:
         /* got the "Go" button */
         if (STRCMP(me->pbhList->pszUrl,"lcgi:goto")) {
            HViewJump hvj;
            ZEROAT(&hvj);
            hvj.pszURL = "lcgi:goto";
            Brewser_Jump(me, &hvj,0);
         }
         return TRUE;

      case IDC_CANCEL:
         return Brewser_BackButton(me);

      case IDC_REFRESH:
         Brewser_Refresh(me);
         return TRUE;

      case IDC_INFO:
         Brewser_Info(me);
         return TRUE;

      case IDC_OPT:
         if (STRCMP(me->pbhList->pszUrl,"lcgi:cfg.menu")) {
            HViewJump hvj;
            ZEROAT(&hvj);
            hvj.pszURL = "lcgi:cfg.menu";
            Brewser_Jump(me, &hvj,0);
            Brewser_HideToolbar(me);
         }
         return TRUE;

      case IDC_BOOKMARKS:
         if (STRCMP(me->pbhList->pszUrl,"lcgi:bkmk.list")) {
            HViewJump hvj;
            ZEROAT(&hvj);
            hvj.pszURL = "lcgi:bkmk.list";
            Brewser_Jump(me, &hvj,0);
            Brewser_HideToolbar(me);
         }
         return TRUE;

      case IDC_HELP:
         Brewser_Help(me);
         return TRUE;
      }
      break;

   case EVT_CTL_SEL_CHANGED:
      if ((dwParam == (uint32)me->piMenu) && me->bShowingToolbar) {
         Brewser_TooltipPush(me);
         return TRUE;
      }
      break;

   case EVT_KEY:
      {
         int nSpelledDebug = me->nSpelledDebug;

         me->nSpelledDebug = 0;

         switch (wParam) {
         case AVK_CLR:
            return Brewser_BackButton(me);
            
         case AVK_UP:
         case AVK_DOWN:
            Brewser_HideToolbar(me);
            return TRUE;
            
         case AVK_RIGHT:
         case AVK_LEFT:
            Brewser_ShowToolbar(me);
            return TRUE;
            
         case AVK_STAR:
            /* show brewser version */
            Brewser_Alert(me,IDX_VERSION,"version%s\0date%s\0",
                          BREWSER_VERSION,__DATE__" "__TIME__);
            return TRUE;
            
         default:
            {
               uint16 wKeyNeeded = 0;
               
               me->nSpelledDebug = nSpelledDebug;
               
               switch (me->nSpelledDebug) {
               case 0: /* need 'd' */
               case 1: /* need 'e' */
                  wKeyNeeded = AVK_3;
                  break;
                  
               case 2: /* need 'b' */
                  wKeyNeeded = AVK_2;
                  break;
                  
               case 3: /* need 'u' */
                  wKeyNeeded = AVK_8;
                  break;
                  
               case 4: /* need 'g' */
                  wKeyNeeded = AVK_4;
                  break;
                  
               case 5: /* spelled 'debug' */
                  me->nSpelledDebug = 0;
                  Brewser_DebugKey(me,wParam);
                  return TRUE;
                  
               default: /* huh? */
                  wKeyNeeded = 0;
                  break;
               }
               
               if (wParam == wKeyNeeded) {
                  me->nSpelledDebug++;
               } else {
                  me->nSpelledDebug = 0;
               }
               return TRUE;
            }
            break;
         }
      }
   }
   
   return FALSE;
}


static void Brewser_DebugKey(Brewser *me, uint16 wKey)
{
   AECHAR wsz[32];

   ZEROAT(&wsz);

   switch (wKey) {
   case AVK_0:
      {
         /* dump elapsed time */
         char buf[32];
               
         SPRINTF(buf, "%dms", me->tWebElapsed);
         STRTOWSTR(buf, wsz, sizeof(wsz));
      }
      break;

   case AVK_1:
      /* test assert */
      (void)(BREWSER_ASSERT(me,0==100));
      break;
      
   case AVK_2:
      /* dump current viewer's state  */
      {
         int nLen;
         char *pszViewState;
         nLen = IHTMLVIEWER_GetViewState(me->piHtml,0,0);
         pszViewState = (char *)MALLOC(nLen);
         if ((char *)0 != pszViewState) {
            IHTMLVIEWER_GetViewState(me->piHtml,pszViewState,nLen);
            DBGPRINTF("viewerstate: \"%s\"",pszViewState);
            FREE(pszViewState);
         }
         STRTOWSTR("viewer state", wsz, sizeof(wsz));
      }
      break;

   case AVK_3:
      break;

   case AVK_4:
      /* test statusbox look */
      if (me->sbStatus.bActive) {
         StatusBox_Stop(&me->sbStatus);
      } else {
         StatusBox_Start(&me->sbStatus);
         StatusBox_Update(&me->sbStatus,"brewser.bar",IDS_STATRECV);
         StatusBox_Update(&me->sbStatus,"brewser.bar",IDS_STATSEND);
      }
      break;
      
   case AVK_5:
      {
         AuthInfo *pai;
         
         DBGPRINTF("---- AuthInfo Dump -----------");
         
         for (pai = me->am.paiList;(AuthInfo *)0 != pai;
              pai = pai->pNext) {
            DBGPRINTF("-------------------------------");
            DBGPRINTF("url: %s",pai->szUrl);
            DBGPRINTF("user: %s",pai->pszUser);
            DBGPRINTF("cred: %s",pai->pszCred);
         }
         DBGPRINTF("-------------------------------");
         STRTOWSTR("authinfo dump", wsz, sizeof(wsz));
      }
      break;
      
   case AVK_6:
      {
         BrewserHist *pbh;
         
         DBGPRINTF("---- History Dump -------------");
         
         for (pbh = me->pbhList;(BrewserHist *)0 != pbh;
              pbh = pbh->pPrev) {
            DBGPRINTF("-------------------------------");
            DBGPRINTF("url: %s",pbh->pszUrl);
            DBGPRINTF("code: %d",pbh->nCode);
            if ((char *)0 != pbh->pszMethod) {
               DBGPRINTF("method: %s",pbh->pszMethod);
            }
            if ((char *)0 != pbh->pszData) {
               DBGPRINTF("data: %s",pbh->pszData);
            }
            if ((char *)0 != pbh->pszRealm) {
               DBGPRINTF("realm: %s",pbh->pszRealm);
            }
            if ((char *)0 != pbh->pszLocation) {
               DBGPRINTF("location: %s",pbh->pszLocation);
            }
            if ((char *)0 != pbh->pszAuth) {
               DBGPRINTF("auth: %s",pbh->pszAuth);
            }
            if ((char *)0 != pbh->pszViewState) {
               DBGPRINTF("viewstate: %s",pbh->pszViewState);
            }
            DBGPRINTF("bLoading: %s", pbh->bLoading?"TRUE":"FALSE");
            DBGPRINTF("bIsBack: %s", pbh->bIsBack?"TRUE":"FALSE");
            DBGPRINTF("bIsRefresh: %s", pbh->bIsRefresh?"TRUE":"FALSE");
         }
         DBGPRINTF("-------------------------------");
         STRTOWSTR("history dump", wsz, sizeof(wsz));
      }
      break;
      
   case AVK_7:
      {
         Cookie *pCk;
         
         DBGPRINTF("---- Cookie Dump -------------");
         
         for (pCk = me->ckm.pCkList; ((Cookie *)0 != pCk); 
              pCk = pCk->pNext) {
            char szExpires[20];
            
            if (-1 != pCk->tExpires) {
               SPRINTF(szExpires,"in %ds", 
                       pCk->tExpires-GETTIMESECONDS());
            } else {
               SPRINTF(szExpires,"at exit");
            }
            
            DBGPRINTF("%s%s%s @%s%s expires %s,"
                      " last used %ds ago, flags %d",
                      pCk->szCookie,
                      pCk->szCookie[0]?"=":"",
                      pCk->pszValue,
                      pCk->pszDomain,
                      pCk->pszPath,
                      szExpires,
                      GETTIMESECONDS()-pCk->tLastUsed,
                      pCk->ulFlags);
            
         }
         DBGPRINTF("-------------------------------");
         STRTOWSTR("cookie dump", wsz, sizeof(wsz));
      }
      break;
      
   case AVK_8:
      STRTOWSTR("cache dump", wsz, sizeof(wsz));
      
      if ((IWebCache *)0 != me->piWebCache) {
         IWebCache_Dump(me->piWebCache);
      }
      break;
      
   case AVK_9:
      me->bNoCache = !me->bNoCache;
      {
         char buf[32];
         
         /* dump cache info */
         SPRINTF(buf, "caching %s", me->bNoCache?"off":"on");
         STRTOWSTR(buf, wsz, sizeof(wsz));
      }
      break;
   }

   if (0 != wsz[0]) {
      AEERect rc = me->rcScreen;
      
      rc.x  = 8;
      rc.y  = 20;
      rc.dx -= 16;
      rc.dy = 20;

      IDISPLAY_DrawText(me->piDisplay, AEE_FONT_LARGE, wsz, -1, 
                        0, 0, &rc,
                        (IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|
                         IDF_RECT_FILL|IDF_RECT_FRAME));
      
      IDISPLAY_Update(me->piDisplay);
      ISHELL_SetTimer(me->piShell,750,Brewser_Redraw,me);      
   }

}

/**
  || Function
  || --------
  || static boolean Brewser_HandleEvent(IApplet *p, AEEEvent eCode, 
  ||                                   uint16 wParam, uint32 dwParam)
  ||
  || Description
  || -----------
  || main event dispatcher
  ||
  || Parameters
  || ----------
  || IApplet *p: the Brewser
  || AEEEvent eCode: event code
  || uint16 wParam: unnecessarily small 1st parameter
  || uint32 dwParam: 2nd parameter
  ||
  || Returns
  || -------
  || TRUE if the app handled the event
  || FALSE otherwise
  ||
  || Remarks
  || -------
  ||
  */
static boolean Brewser_HandleEvent(IApplet *p, AEEEvent eCode, uint16 wParam,
                                   uint32 dwParam)
{
   Brewser *me = (Brewser *)p;

   switch (eCode) {
   case EVT_APP_START:
      {
         INetMgr *piNet;
         
         if (SUCCESS == ISHELL_CreateInstance(me->piShell, AEECLSID_NET,
                                              (void **)&piNet)) {
            me->uLingerSave = INETMGR_SetLinger(piNet,3*60); /* 3 minutes */
            INETMGR_Release(piNet);
         }
      }
      me->tLastUserEvent = GETTIMESECONDS();
      me->nTbSlideY = me->nTbSaveY;
      Brewser_ShowToolbar(me);

      me->pbhCur = me->pbhList; /* set to bhBottom */
      me->pbhCur->bIsBack = TRUE;
      Brewser_NavStart(me);
      return TRUE;

   case EVT_APP_RESUME:
      me->bSuspended = 0;
      if (me->bShowingDialog) {
         HtmlDlg_SetActive(&me->htmldlg,TRUE);
      } else if (me->bShowingToolbar) {
         IMENUCTL_SetActive(me->piMenu,TRUE);
      } else {
         IHTMLVIEWER_SetActive(me->piHtml,TRUE);
      }
      Brewser_Invalidate(me);
      return TRUE;

   case EVT_APP_STOP:
      {
         INetMgr *piNet;
         
         if (SUCCESS == ISHELL_CreateInstance(me->piShell, AEECLSID_NET,
                                              (void **)&piNet)) {
            INETMGR_SetLinger(piNet,me->uLingerSave); /* reset linger */
            INETMGR_Release(piNet);
         }
      }

   case EVT_APP_SUSPEND:
      me->bSuspended = 1;
      CALLBACK_Cancel(&me->cbRedraw);

      HtmlDlg_SetActive(&me->htmldlg,FALSE);
      /* 
         if I call IMENUCTL_SetActive() it draws to the screen 
         then calls UpdateEx(TRUE), even though I'm suspended...

         This pre 2.1 BREW bug is easily masked by not giving the 
         IMENUCTL my stack to run on, or by calling 
         IDISPLAY_UpdateEx(FALSE) when we're suspended...
      */
      //      IMENUCTL_SetActive(me->piMenu,FALSE);
      IHTMLVIEWER_SetActive(me->piHtml,FALSE);
      Tooltip_Cancel(&me->ttTbTips);
      Brewser_Stop(me);
      return TRUE;

   case EVT_APP_BROWSE_URL:
      /* if we're in progress, whack that hist, replace with this'n here */
      if (Brewser_InProgress(me)) {
         Brewser_Stop(me);
         Brewser_Refresher(me,(char *)dwParam);
      } else {
         /* else just go */
         HViewJump hvj;
         
         ZEROAT(&hvj);
         hvj.pszURL = (char *)dwParam;
         Brewser_Jump(me, &hvj,0);
      }
      return TRUE;

   case EVT_DIALOG_END: /* need a redraw! */
      Brewser_Invalidate(me);
      return TRUE;

   case EVT_KEY:
      if (AVK_VOLUME_UP == wParam) {
         wParam = AVK_UP;
      } else if (AVK_VOLUME_DOWN == wParam) {
         wParam = AVK_DOWN;         
      }
   case EVT_CHAR:
      me->tLastUserEvent = GETTIMESECONDS();
      
   default:
      if ((IDialog *)0 == ISHELL_GetActiveDialog(me->piShell)) {
         return Brewser_HtmlModeHandleEvent(me,eCode,wParam,dwParam);
      }
   }

   return FALSE;
}

/* 
   @@@ - TODO: something more intellligent could be done about strings 
               contained in a history entry.  E.g. when an entry's not in
               progress, we don't need to keep any header strings 
               (referer, cookies, etc.) around.
*/
static boolean Brewser_CullHistory(Brewser *me)
{
   BrewserHist **lfp;
   BrewserHist  *pbhFree;

   {
      int          nHists;
      BrewserHist *pbh;
      
      for (nHists = 0, pbh = me->pbhList; (BrewserHist *)0 != pbh;
           nHists++, pbh = pbh->pPrev);

      if (BREWSER_MINHISTS > nHists) {
         return FALSE;
      }
   }

   /* none to fry, either nearly empty list, or headed to end of list */
   if ((&me->bhBottom == me->pbhList) || 
       (&me->bhBottom == me->pbhCur)) {
      return FALSE;
   }

   /* find the next-to-oldest history entry */
   for (lfp = &me->pbhList; 
        (pbhFree = *lfp)->pPrev != &me->bhBottom; 
        lfp = &pbhFree->pPrev);
   
   /* navigating to this one... */
   if (pbhFree == me->pbhCur) {
      return FALSE;
   }
   
   /* remove from list */
   *lfp = pbhFree->pPrev;

   /* whack bottom */
   BrewserHist_Dtor(&me->bhBottom);

   /* copy contents to bottom */
   me->bhBottom = *pbhFree;
   me->bhBottom.pPrev = 0;
   ZEROAT(pbhFree);

   BrewserHist_Delete(pbhFree);

   return TRUE;
}

static void Brewser_LowRAM(void *p)
{
   Brewser *me = (Brewser *)p;

   if (Brewser_CullHistory(me)) {
      /* re-register, there may be more history to free */
      ISHELL_RegisterSystemCallback(me->piShell,&me->cbLowRAM,
                                    AEE_SCB_LOW_RAM);
   }
}

#undef GOODFONTS

static void LoadGoodFonts(IShell *piShell, IDisplay *piDisplay)
{
#ifdef GOODFONTS

   IFont *pifNormal = 0;
   IFont *pifBold = 0;
   IFont *pifLarge = 0;

   if ((SUCCESS == ISHELL_CreateInstance(piShell,
                                         AEECLSID_FONT_BASIC11,
                                         (void **)&pifNormal)) &&
       (SUCCESS == ISHELL_CreateInstance(piShell,
                                         AEECLSID_FONT_BASIC11B,
                                         (void **)&pifBold)) &&
       (SUCCESS == ISHELL_CreateInstance(piShell,
                                         AEECLSID_FONT_BASIC14,
                                         (void **)&pifLarge))) {

      pifNormal = IDISPLAY_SetFont(piDisplay,AEE_FONT_NORMAL,pifNormal);
      pifBold = IDISPLAY_SetFont(piDisplay,AEE_FONT_BOLD,pifBold);
      pifLarge = IDISPLAY_SetFont(piDisplay,AEE_FONT_LARGE,pifLarge);

   }
      
   RELEASEIF(pifNormal);
   RELEASEIF(pifBold);
   RELEASEIF(pifLarge);

#endif /* #ifdef GOODFONTS */
}


static void Brewser_AddProxySpec(Brewser *me, const char *cpszSchm,
                                 const char *cpszHost, uint16 usPort)
{
   WebOpt  awo[2];
   int     nLen;
   char   *pszSpec;

   nLen = SNPRINTF(0,0,"%s:///http://%s:%d/",cpszSchm,cpszHost,usPort);
   
   pszSpec = MALLOC(nLen);
   
   if ((char *)0 == pszSpec) {
      return;
   }

   SNPRINTF(pszSpec,nLen,"%s:///http://%s:%d/",cpszSchm,cpszHost, usPort);

   ZEROAT(&awo); /* sets awo[1] to WEBOPT_END */
   awo[0].nId = WEBOPT_PROXYSPEC;
   awo[0].pVal = pszSpec;

   IWEB_AddOpt(me->piWeb,awo);
   FREE(pszSpec);
}


/* GROAN */
static void Brewser_SetupProxySpecs(Brewser *me)
{
   const char *cpszHttpHost;
   const char *cpszFtpHost;
   const char *cpszGophHost;
   const char *cpszOthHost;
   char *pszOthSchm;
   const BrewserProxyCfg *pcfg;

   if (SUCCESS != Brewser_GetProxyCfgPtr(me,&pcfg)) {
      return;
   }
   
   cpszHttpHost = pcfg->szStrings;
   cpszFtpHost  = cpszHttpHost + STRLEN(cpszHttpHost) + 1;
   cpszGophHost = cpszFtpHost  + STRLEN(cpszFtpHost) + 1;
   cpszOthHost  = cpszGophHost + STRLEN(cpszGophHost) + 1;
   pszOthSchm   = STRDUP(cpszOthHost + STRLEN(cpszOthHost) + 1);

   /* fry all current proxy specs */
   while (SUCCESS == IWEB_RemoveOpt(me->piWeb,WEBOPT_PROXYSPEC,0));

   if (pcfg->bHttpOn) {
      Brewser_AddProxySpec(me,"http",cpszHttpHost,pcfg->usHttpPort);
   }
   if (pcfg->bFtpOn) {
      Brewser_AddProxySpec(me,"ftp",cpszFtpHost,pcfg->usFtpPort);
   }
   if (pcfg->bGophOn) {
      Brewser_AddProxySpec(me,"gopher",cpszGophHost,pcfg->usGophPort);
   }
   if (pcfg->bOthOn) {
      int nOth = strchop(pszOthSchm,", ");
      
      while (nOth > 0) {
         nOth--;
         Brewser_AddProxySpec(me,strchopped_nth(pszOthSchm, nOth),
                              cpszOthHost,pcfg->usOthPort);
      }
   }

   FREEIF(pszOthSchm);
}


/**
  || Function
  || --------
  || static void Brewser_CtorZ(Brewser *me, IModule *pim, 
  ||                           IDisplay *piDisplay,
  ||                           IMenuCtl *piMenu,
  ||                           IShell *piShell, IWeb *piWeb, 
  ||                           IWebUtil *piWebUtil,
  ||                           IHtmlViewer *piHtml,
  ||                           IHtmlViewer *piHtmlDlg)
  ||
  || Description
  || -----------
  || initializes a Brewser object into zero-initialized memory
  ||
  || Parameters
  || ----------
  || Brewser *me: new memory
  || IModule *pim: my owner, must be AddRef'd
  || IDisplay *piDisplay: where to draw
  || IMenuCtl *piMenu: new softkey 
  || IShell *piShell: BREW Shell interface pointer
  || IWeb *piWeb: BREW Web interface pointer
  || IWebUtil *piWebUtil: BREW WebUtil
  || IHtmlViewer *piHtml: where output goes..
  || IHtmlViewer *piHtmlDlg: used for dialogs
  ||
  || Returns
  || -------
  || nothing, can't fail..
  ||
  || Remarks
  || -------
  ||
  */
static void Brewser_CtorZ(Brewser *me, IModule *pim, IDisplay *piDisplay,
                          IMenuCtl *piMenu, IShell *piShell, 
                          IWeb *piWeb, IWebUtil *piWebUtil,
                          ISourceUtil *piSourceUtil,
                          IHtmlViewer *piHtml, IHtmlViewer *piHtmlDlg)
{
   /* fix up IApplet vtbl */
   me->ia.pvt = &me->iavt;

   /* Initialize individual entries in the VTBL */
   me->iavt.AddRef      = Brewser_AddRef;
   me->iavt.Release     = Brewser_Release;
   me->iavt.HandleEvent = Brewser_HandleEvent;

   me->uRef = 1;

   me->pbhList = &me->bhBottom;


   /* take my module, must AddRef becuase BREW keeps track of Module usage
      with the IModule's refcount */
   me->pim = pim;
   IMODULE_AddRef(pim);

   /* take shell */
   me->piShell = piShell;
   ISHELL_AddRef(piShell);

   /* take display, we'll be black on white */
   me->piDisplay = piDisplay;
   IDISPLAY_AddRef(piDisplay);
   IDISPLAY_SetColor(me->piDisplay, CLR_USER_BACKGROUND, RGB_WHITE);

   /* take the web */
   me->piWeb = piWeb;
   IWEB_AddRef(piWeb);
   
   IWEB_QueryInterface(piWeb, AEECLSID_WEBCACHE, (void **)&me->piWebCache);

   /* take the webutil */
   me->piWebUtil = piWebUtil;
   IWEBUTIL_AddRef(piWebUtil);

   /* take the sourceutil */
   me->piSourceUtil = piSourceUtil;
   ISOURCEUTIL_AddRef(piSourceUtil);

   /* initialize the callback, where I'll be called when a web request
      completes */
   CALLBACK_Init(&me->cbGotResp, Brewser_GotResp, me);

   /* initialize the low ram callback */
   CALLBACK_Init(&me->cbLowRAM, Brewser_LowRAM, me);

   /* initialize the callback where I'll be called when a it's time
      to redraw html mode */
   CALLBACK_Init(&me->cbRedraw, Brewser_Redraw, me);

   /* initialize the callback where I'll be called when a it's time
      to do info */
   CALLBACK_Init(&me->cbInfoDialog, Brewser_Info, me);

   /* get screen size, language info, BREW version, platform ID, tuck away */
   {
      AEEDeviceInfo di;

      ZEROAT(&di);

      di.wStructSize = sizeof(di);
      
      ISHELL_GetDeviceInfo(me->piShell, &di);
      me->rcScreen.dx = di.cxScreen;
      me->rcScreen.dy = di.cyScreen;

      {
         char szLang[5];
         int i;
         for (i = 0; i < 4; i++) {
            szLang[i] = (char)(di.dwLang & 0x0ff);
            if (' ' == szLang[i]) {
               szLang[i] = 0;
               break;
            }
            di.dwLang >>= 8;
         }
         SNPRINTF(me->szHeaders,sizeof(me->szHeaders),
                  "Accept-Language: %s\r\n",szLang);
      }

      /* make my user agent */
      {
         int  nLen;
         char szVersion[16];
         
         INET_NTOA(HTONL(GETAEEVERSION(0,0,0)),szVersion,sizeof(szVersion));

         nLen = snxprintf(0,0,BREWSER_USERAGENT,"{}",
                          "version%s\0devid%d\0",
                          szVersion,di.dwPlatformID);

         me->pszUserAgent = (char *)MALLOC(nLen);

         if ((char *)0 != me->pszUserAgent) {
            snxprintf(me->pszUserAgent,nLen,BREWSER_USERAGENT,"{}",
                      "version%s\0devid%d\0",
                      szVersion,di.dwPlatformID);
         }
      }
   }

   /* initialize the IWeb with a few options.. 
      unfortunately, the ARM compiler can't do multiple initializations 
      here, we have to do them by hand... */
   {
      WebOpt awo[5]; /* ***IMPORTANT**** grow this if you add more 
                        WebOpts here, or shrink it and call AddOpt() 
                        multiple times */
      int i = 0;

      /* test user-agent, uncomment this section to ship your own user-agent 
         string. if unset, IWeb will send a default.  If set to NULL, no 
         user agent header will be sent */

      awo[i].nId  = WEBOPT_USERAGENT;
      if ((char *)0 != me->pszUserAgent) {
         awo[i].pVal = (void *)me->pszUserAgent;
      } else {
         awo[i].pVal = BREWSER_USERAGENT;
      }
      i++;

      /* this has Accept-Language */
      awo[i].nId = WEBOPT_HEADER;
      awo[i].pVal = (void *)me->szHeaders;
      i++;

      /* provide pointer to 'me' via this back-door to
         other interested parties (i.e. bookmarks) */
      awo[i].nId = WEBOPT_PRIVBREWSER;
      awo[i].pVal = (void *)me;
      i++;

      /* SSL options */
      /* give IWeb any root certs included with BREW */
      if (SUCCESS == ISHELL_CreateInstance(piShell, AEECLSID_SSLROOTCERTS,
                                           (void **)&me->piwoSSL)) {
         WebOpt awoSSL[8];
         int    iSSL = 0;

         /* other SSL options */

         /* gimme web error on trust failure */
         awoSSL[iSSL].nId = WEBOPT_SSL_TRUST_MODE;
         awoSSL[iSSL].pVal = (void *)SSL_TRUST_MODE_CHECK;
         iSSL++;
         
         awoSSL[iSSL].nId = WEBOPT_SSL_WANT_X509_CHAIN;
         awoSSL[iSSL].pVal = (void *)TRUE;
         iSSL++;

         /* allow SSL 3.0 and TLS 1.0 */
         awoSSL[iSSL].nId  = WEBOPT_SSL_ALLOWED_VERSIONS;
         awoSSL[iSSL].pVal = (void *)SSL_VERSION_30;
         iSSL++;

         awoSSL[iSSL].nId  = WEBOPT_SSL_ALLOWED_VERSIONS;
         awoSSL[iSSL].pVal = (void *)SSL_VERSION_TLS10;
         iSSL++;
         
         /* easy to support ciphers (gets like 99% of the web) */
         awoSSL[iSSL].nId = WEBOPT_SSL_ALLOWED_CIPHER_SUITES;
         awoSSL[iSSL].pVal = (void *)SSL_CSUITE_RSA_WITH_RC4_128_SHA;
         iSSL++;

         awoSSL[iSSL].nId = WEBOPT_SSL_ALLOWED_CIPHER_SUITES;
         awoSSL[iSSL].pVal = (void *)SSL_CSUITE_RSA_WITH_RC4_128_MD5;
         iSSL++;

         awoSSL[iSSL].nId = WEBOPT_SSL_ALLOWED_CIPHER_SUITES;
         awoSSL[iSSL].pVal = (void *)SSL_CSUITE_RSA_WITH_DES_CBC_SHA;
         iSSL++;

         awoSSL[iSSL].nId  = WEBOPT_END;
         
         IWEBOPTS_AddOpt(me->piwoSSL,awoSSL);

         /* add to the IWeb as a set of defaults */
         awo[i].nId = WEBOPT_DEFAULTS;
         awo[i].pVal = me->piwoSSL;
         i++;
      }

      /* this is absolutely necessary, do not remove, marks the end of the 
         array of WebOpts */
      awo[i].nId  = WEBOPT_END;
      
      /* add 'em */
      IWEB_AddOpt(me->piWeb,awo);
   }


   /* take the html */
   me->piHtml = piHtml;
   IHTMLVIEWER_AddRef(piHtml);

   IHTMLVIEWER_SetProperties(piHtml, (IHTMLVIEWER_GetProperties(piHtml) | 
                                      HVP_SCROLLBAR | HVP_LOADALLIMAGES |
                                      HVP_INVALIDATION));

   IHTMLVIEWER_SetNotifyFn(piHtml, Brewser_HViewNotify, me);

   /* Pass the IWeb into the IHtmlViewer */
   IHTMLVIEWER_SetIWeb(piHtml, piWeb);

   /* take the Menu */
   me->piMenu = piMenu;
   IMENUCTL_AddRef(piMenu);

   IMENUCTL_SetProperties(me->piMenu,
                          (IMENUCTL_GetProperties(me->piMenu) |
                           MP_MAXSOFTKEYITEMS));

   /* this sucks, populate the toolbar, save off desired dimensions */
   {
      AEERect rc;
      
      Brewser_SetupSoftKey(me);
      IMENUCTL_GetRect(me->piMenu,&rc);
      me->nTbSaveY = rc.y;
      me->nTbSlideY = me->rcScreen.dy;
   }

   IHTMLVIEWER_SetRect(me->piHtml,&me->rcScreen);

   Slider_CtorZ(&me->sTb, piShell,&me->nTbSlideY, Brewser_TbSlideInc, me);

   Tooltip_CtorZ(&me->ttTbTips,piShell,piDisplay,me->nTbSaveY, 
                 me->rcScreen.x, me->rcScreen.dx, 
                 BREWSER_TTDELAY,BREWSER_TTEXPIRY, Brewser_Invalidate,me);

   StatusBox_CtorZ(&me->sbStatus,piShell,piDisplay,
                   BREWSER_RES_FILE,IDB_STATUS,&me->rcScreen,
                   Brewser_Invalidate,me);

   HeaderMaker_CtorZ(&me->hm,
                     "Refresh\0"
                     "Location\0"
                     "Set-Cookie\0"
                     "Proxy-Authenticate\0"
                     "WWW-Authenticate\0"
                     "Expires\0"
                     "Last-Modified\0",
                     Brewser_HeaderMade,me);

   CookieMgr_CtorZ(&me->ckm,piShell,me->piWebUtil,BREWSER_COOKIESMINSIZE);

   {
      AEERect rc;
      
      rc = me->rcScreen;
      
      rc.dx -= ((me->rcScreen.dy - me->nTbSaveY)*4)/5;
      rc.dy -= ((me->rcScreen.dy - me->nTbSaveY)*4)/5;

      rc.x += (me->rcScreen.dx - rc.dx)/2;
      rc.y += (me->rcScreen.dy - rc.dy)/2;
      
      HtmlDlg_CtorZ(&me->htmldlg,me->piDisplay,piHtmlDlg,&rc,4,
                    Brewser_Invalidate,me);
   }

   
   AuthMgr_CtorZ(&me->am, piShell, piWebUtil,
                 Brewser_WaitDialog,Brewser_StartDialog,
                 Brewser_FillDialog,Brewser_EndDialog,me);
   
   BrewserAuth_CtorZ(&me->ba,me);
   BrewserTrust_CtorZ(&me->bt,me);

   Prefs_CtorZ(&me->prefs, piShell);

   {
      boolean bEnabled = TRUE;
      
      if (SUCCESS == Brewser_GetCookiesEnabled(me,&bEnabled)) {
         me->bDoCookies = (0 != bEnabled);
      }
   }
   
   {
      const char *cpsz;
      Brewser_GetMailToPtr(me, &cpsz);
      
      /* free it here because GetMailToPtr() may set it */
      FREEIF(me->pszMailTo);
      me->pszMailTo = STRDUP(cpsz); /* so we can FREEIF() it */
      
      Brewser_GetHomePagePtr(me, &cpsz);
      me->bhBottom.pszUrl = STRDUP(cpsz);
      me->bhBottom.bIsBack = TRUE; /* allow stale cache entries for 
                                      the homepage */
   }

   Brewser_SetupProxySpecs(me);

}


/**
  || Function
  || --------
  || static void Brewser_Dtor(Brewser *me)
  ||
  || Description
  || -----------
  || fries contents of Brewser, doesn't free memory
  ||
  || Parameters
  || ----------
  || Brewser *me: the Brewser
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static void Brewser_Dtor(Brewser *me)
{
   BrewserHist *pbh;

   Brewser_Stop(me);

   FREEIF(me->pszUserAgent);
   FREEIF(me->pszMailTo);

   Prefs_Flush(&me->prefs);
   Prefs_Dtor(&me->prefs);

   HtmlDlg_Dtor(&me->htmldlg);

   CookieMgr_Dtor(&me->ckm);

   HeaderMaker_Dtor(&me->hm);

   Slider_Dtor(&me->sTb);

   Tooltip_Dtor(&me->ttTbTips);

   StatusBox_Dtor(&me->sbStatus);

   CALLBACK_Cancel(&me->cbRedraw);
   CALLBACK_Cancel(&me->cbLowRAM);
   
   while ((pbh = me->pbhList) != &me->bhBottom) {
      me->pbhList = pbh->pPrev;
      BrewserHist_Delete(pbh);
   }
   /* whack bottom */
   BrewserHist_Dtor(&me->bhBottom);
   
   BrewserAuth_Dtor(&me->ba);
   BrewserTrust_Dtor(&me->bt);

   AuthMgr_Dtor(&me->am);
   
   RELEASEIF(me->piSourceUtil);
   RELEASEIF(me->piwoSSL);
   RELEASEIF(me->piWebUtil);
   RELEASEIF(me->piHtml);
   RELEASEIF(me->piWeb);
   RELEASEIF(me->piWebCache);
   RELEASEIF(me->piMenu);
   RELEASEIF(me->piDisplay);
   RELEASEIF(me->piShell);
   RELEASEIF(me->pim);
}

/**
  || Function
  || --------
  || static void Brewser_Delete(Brewser *me)
  ||
  || Description
  || -----------
  || deletes a Brewser
  ||
  || Parameters
  || ----------
  || Brewser *me: the Brewser
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static void Brewser_Delete(Brewser *me)
{
   Brewser_Dtor(me);
   FREE(me);
}


/**
  || Function
  || --------
  || static int Brewser_New(IModule *pim, IShell *piShell, IApplet **pp)
  ||
  || Description
  || -----------
  || makes a new Brewser, the BREW applet class for this Module
  ||
  || Parameters
  || ----------
  || IModule *pim: my owner
  || IShell *piShell: the BREW shell
  || IApplet **pp: [out] where to stuff the new Applet (the new Brewser)
  ||
  || Returns
  || -------
  || ENOMEMORY if allocation fails
  || EPRIVLEVEL if AEECLSID_WEB could not be CreateInstance()d
  ||
  || Remarks
  || -------
  ||
  */
static int Brewser_New(IModule *pim, IShell *piShell, IApplet **pp)
{
   Brewser     *me = 0;
   IDisplay    *piDisplay = 0;
   IWeb        *piWeb = 0;
   IWebUtil    *piWebUtil = 0;
   ISourceUtil *piSourceUtil = 0;
   IHtmlViewer *piHtml = 0;
   IHtmlViewer *piHtmlDlg = 0;
   IMenuCtl    *piMenu = 0;
   int          nErr;

   nErr = ISHELL_CreateInstance(piShell, AEECLSID_DISPLAY,
                                (void **)&piDisplay);

   if (SUCCESS == nErr) {
      LoadGoodFonts(piShell,piDisplay);
   }

   if (SUCCESS == nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_SOFTKEYCTL,
                                   (void **)&piMenu);
   }

   if (SUCCESS == nErr) {
      //      nErr = ISHELL_CreateInstance(piShell, AEECLSID_WEB,(void **)&piWeb);
      nErr = IWebCache_New(piShell,AEECLSID_WEBCACHE,(void **)&piWeb);
   }

   if (SUCCESS == nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_WEBUTIL,
                                   (void **)&piWebUtil);
   }

   if (SUCCESS == nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_SOURCEUTIL,
                                   (void **)&piSourceUtil);
   }

   if (SUCCESS == nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_HTML,(void **)&piHtml);
   }

   if (SUCCESS == nErr) {
      nErr = ISHELL_CreateInstance(piShell, AEECLSID_HTML,
                                   (void **)&piHtmlDlg);
   }

   nErr = ERR_MALLOCREC(Brewser,&me);

   if (SUCCESS == nErr) {
      Brewser_CtorZ(me, pim, piDisplay, /* piText, */piMenu, 
                    piShell, piWeb, piWebUtil, piSourceUtil, 
                    piHtml, piHtmlDlg);
   }
   
   RELEASEIF(piHtmlDlg);
   RELEASEIF(piHtml);
   RELEASEIF(piSourceUtil);
   RELEASEIF(piWebUtil);
   RELEASEIF(piWeb);
   RELEASEIF(piMenu);
   RELEASEIF(piDisplay);
   
   *pp = &me->ia;
   return nErr;
}


/**
  || Function
  || --------
  || static uint32 Brewser_AddRef(IApplet *p)
  || static uint32 Brewser_Release(IApplet *p)
  ||
  || Description
  || -----------
  || increment/decrement refcount on a Brewser
  ||
  || Parameters
  || ----------
  || IApplet *p: a Brewser
  ||
  || Returns
  || -------
  || new refcount
  ||
  || Remarks
  || -------
  || if refcount drops to 0, the Brewser is deleted
  ||
  */
static uint32 Brewser_AddRef(IApplet *p)
{
   Brewser *me = (Brewser *)p;
   return ++me->uRef;
}

static uint32 Brewser_Release(IApplet *p)
{
   Brewser *me = (Brewser *)p;
   uint32 uRef = --me->uRef;

   if (uRef == 0) {
      Brewser_Delete(me);
   }
   return uRef;
}


/* =======================================================================
   Brewser preferences
   ======================================================================= */

// Brewser preferences IDs
#define BSRPREFID_PROXYCFG  1 /* (struct BrewserProxyCfg)         */
#define BSRPREFID_MAILTO    2 /* (string) mailto handler          */
#define BSRPREFID_HOMEPAGE  3 /* (string) homepage                */
#define BSRPREFID_DOCOOKIES 4 /* (boolean) enable/disable cookies */

/* @@@ - TODO: validate ports, presence of hostnames, etc */
int Brewser_SubmitProxyCfg(Brewser *me, char *pszSrch)
{
   int nErr;
   const char *acpszStrings[5];

   const char *cpszHttpOn   = 0;
   const char *cpszFtpOn    = 0;
   const char *cpszGophOn   = 0;
   const char *cpszOthOn    = 0;
   const char *cpszHttpPort = "0";
   const char *cpszFtpPort  = "0";
   const char *cpszGophPort = "0";
   const char *cpszOthPort  = "0";
   BrewserProxyCfg *pcfg = 0;

   {
      int i;
      for (i = 0; i < ARRAY_SIZE(acpszStrings); i++) {
         acpszStrings[i] = "";
      }
   }

   IWebUtil_GetFormFields(me->piWebUtil, pszSrch, 
                          "httpon\0ftpon\0gophon\0othon\0"
                          "httpport\0ftpport\0gophport\0othport\0"
                          "httphost\0ftphost\0gophhost\0othhost\0othschm\0",
                          &cpszHttpOn,  
                          &cpszFtpOn, 
                          &cpszGophOn, 
                          &cpszOthOn,
                          &cpszHttpPort, 
                          &cpszFtpPort, 
                          &cpszGophPort, 
                          &cpszOthPort,
                          &acpszStrings[0], 
                          &acpszStrings[1], 
                          &acpszStrings[2], 
                          &acpszStrings[3], 
                          &acpszStrings[4]);

   nErr = ERR_MALLOCREC_EX(BrewserProxyCfg,
                           STRLEN(acpszStrings[0]) +
                           STRLEN(acpszStrings[1]) +
                           STRLEN(acpszStrings[2]) +
                           STRLEN(acpszStrings[3]) +
                           STRLEN(acpszStrings[4]),
                           &pcfg);

   if (SUCCESS == nErr) {
      pcfg->bHttpOn    = ((char *)0 != cpszHttpOn);
      pcfg->bFtpOn     = ((char *)0 != cpszFtpOn);
      pcfg->bGophOn    = ((char *)0 != cpszGophOn);
      pcfg->bOthOn     = ((char *)0 != cpszOthOn);
      pcfg->usHttpPort = ATOI(cpszHttpPort);
      pcfg->usFtpPort  = ATOI(cpszFtpPort);
      pcfg->usGophPort = ATOI(cpszGophPort);
      pcfg->usOthPort  = ATOI(cpszOthPort);

      {
         char *p = pcfg->szStrings;
         int   nLen;
         int   i;

         for (i = 0; i < ARRAY_SIZE(acpszStrings); i++) {
            nLen = STRLEN(acpszStrings[i]);
            MEMMOVE(p,acpszStrings[i],nLen);
            p += nLen+1;
         }

         nErr = Brewser_SetProxyCfg(me,pcfg,p-(char *)pcfg);

      }
   }
   FREEIF(pcfg);

   return nErr;
}

int Brewser_GetProxyCfgPtr(Brewser *me, const BrewserProxyCfg **ppcfg)
{
   int nErr;
   int nSize;

   nErr = Prefs_GetPtr(&me->prefs, BSRPREFID_PROXYCFG, 
                       (const void **)ppcfg, &nSize);

   if (EBADITEM == nErr) {
      char *psz;
   
      nErr = IShell_LoadResSz(me->piShell, BREWSER_RES_FILE, 
                              IDS_DEFPROXYSETTINGS, &psz);
      if (SUCCESS == nErr) {
         nErr = Brewser_SubmitProxyCfg(me,psz);
      }
      
      FREEIF(psz);
      
      nErr = Prefs_GetPtr(&me->prefs, BSRPREFID_PROXYCFG,
                          (const void **)ppcfg, &nSize);
   }

   return nErr;
}

int Brewser_SetProxyCfg(Brewser *me, const BrewserProxyCfg *pcfg, int nSize)
{
   int nErr;

   nErr = Prefs_Replace(&me->prefs, BSRPREFID_PROXYCFG, pcfg, nSize);

   if (SUCCESS == nErr) {
      Brewser_SetupProxySpecs(me);
   }
   return nErr;
}

int Brewser_GetMailToPtr(Brewser *me, const char **pcpsz)
{
   int nSize;
   int nErr;

   nErr = Prefs_GetPtr(&me->prefs, BSRPREFID_MAILTO, 
                       (const void **)pcpsz, &nSize);

   if (EBADITEM == nErr) {
      char *psz;
      nErr = IShell_LoadResSz(me->piShell,BREWSER_RES_FILE,IDS_DEFMAILTO,
                              &psz);

      if (SUCCESS == nErr) {
         nErr = Brewser_SetMailTo(me, psz);
      }
      FREEIF(psz);
      nErr = Prefs_GetPtr(&me->prefs, BSRPREFID_MAILTO,
                          (const void **)pcpsz, &nSize);
   }

   return nErr;
}

int Brewser_SetMailTo(Brewser *me, const char *cpsz)
{
   int nErr;

   nErr = Prefs_Replace(&me->prefs,BSRPREFID_MAILTO,cpsz,STRLEN(cpsz)+1);
   
   if (SUCCESS == nErr) {
      FREEIF(me->pszMailTo);
      me->pszMailTo = STRDUP(cpsz);
   }

   return nErr;
}

int Brewser_GetHomePagePtr(Brewser *me, const char **pcpsz)
{
   int nSize;
   int nErr;

   nErr = Prefs_GetPtr(&me->prefs, BSRPREFID_HOMEPAGE, 
                       (const void **)pcpsz, &nSize);

   if (EBADITEM == nErr) {
      char *psz;
      nErr = IShell_LoadResSz(me->piShell,BREWSER_RES_FILE,IDS_DEFHOMEPAGE,
                              &psz);

      if (SUCCESS == nErr) {
         nErr = Brewser_SetHomePage(me, psz);
      }
      FREEIF(psz);
      nErr = Prefs_GetPtr(&me->prefs, BSRPREFID_HOMEPAGE, 
                          (const void **)pcpsz, &nSize);
   }

   return nErr;
}

int Brewser_SetHomePage(Brewser *me, const char *cpsz)
{
   int nErr;

   nErr = Prefs_Replace(&me->prefs, BSRPREFID_HOMEPAGE, 
                        cpsz, STRLEN(cpsz)+1);

   return nErr;
}

int Brewser_GetCookiesEnabled(Brewser *me, boolean *pbEnabled)
{
   int nErr;
   int nSize = sizeof(*pbEnabled);

   nErr = Prefs_Get(&me->prefs, BSRPREFID_DOCOOKIES, pbEnabled, &nSize);

   if (EBADITEM == nErr) {
      *pbEnabled = TRUE;
      nErr = SUCCESS;
   }

   return nErr;
}

int Brewser_SetCookiesEnabled(Brewser *me, boolean bEnabled)
{
   int nErr;

   me->bDoCookies = (0 != bEnabled);

   nErr = Prefs_Replace(&me->prefs, BSRPREFID_DOCOOKIES,
                        &bEnabled, sizeof(bEnabled));

   return nErr;
}

/* Config; Delete all cookies */
void Brewser_CookiesClearAll(Brewser *me)
{
   CookieMgr_ClearAll(&me->ckm);
}


/* =======================================================================
   xmod stuff 
   ======================================================================= */
#include "xmod.h"

/* 
   || know about lcgi
*/
#include "lcgieng.h"

/**
  || Function
  || --------
  || static int xModule_CreateInstance(IModule *p, IShell *piShell, 
  ||                                   AEECLSID idClass, void **pp)
  ||
  || Description
  || -----------
  || xModule's CreateInstance implementation
  ||
  || Parameters
  || ----------
  || IModule *p: module 
  || IShell *piShell: BREW shell interface
  || AEECLSID idClass: class id of the desired class
  || void **pp: where to stick the new interface pointer
  ||
  || Returns
  || -------
  || SUCCESS if a brewser could be constructed
  || some other AEE error otherwise, see Brewser_New()
  ||
  || Remarks
  || -------
  || in this Applet, this function only responds to the brewser classid
  ||
  */
int xModule_CreateInstance(IModule *me, IShell *piShell, 
                           AEECLSID idClass, void **pp)
{
   if (idClass == AEECLSID_BREWSER) {
      return Brewser_New(me,piShell,(IApplet **)pp);
   } else if (AEECLSID_LCGIENG == idClass) {
      return LCGIEng_New(me,piShell,idClass, (IWebEng **)pp);
   }
   
   *pp = 0;
   return ECLASSNOTSUPPORT; /* don't understand what you're asking for */
}
