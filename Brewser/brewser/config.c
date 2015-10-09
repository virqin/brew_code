/*
  ===========================================================================

  FILE:  config.c
  
  SERVICES:  
    lcgi config
  
  GENERAL DESCRIPTION:
    Config UI for BREWser implmented as a local CGI approach.
  
  REVISION HISTORY: 
    8/22/2002  12:58pm Created

  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/
#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEWeb.h"
#include "AEESource.h"

#include "brewser.h"

#include "lcgieng.h"

#include "util.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
///////////////////
// Configuration //
///////////////////

static int Config_Menu(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                 IDX_CFGMENU, ppBlob, pnBlobLen);

   return nErr;
}


////////////////////
// Config Proxies //
////////////////////

static int Config_Proxies(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr;
   BrewserProxyCfg cfgDef;
   const BrewserProxyCfg *pcfg = 0;

   ZEROAT(&cfgDef);

   nErr = IShell_LoadResBlob(me->plcgie->piShell, 
                             BREWSER_RES_FILE, 
                             IDX_CFGPROXY, ppBlob, pnBlobLen);
   
   if (SUCCESS == nErr) {
      nErr = Brewser_GetProxyCfgPtr(me->pbsr, &pcfg);
      if (SUCCESS != nErr) {
         pcfg = &cfgDef;
      }
   }

   if (SUCCESS == nErr) {
      const char *cpszHttpHost = pcfg->szStrings;
      const char *cpszFtpHost  = cpszHttpHost + STRLEN(cpszHttpHost) + 1;
      const char *cpszGophHost = cpszFtpHost  + STRLEN(cpszFtpHost) + 1;
      const char *cpszOthHost  = cpszGophHost + STRLEN(cpszGophHost) + 1;
      const char *cpszOthSchm  = cpszOthHost  + STRLEN(cpszOthHost) + 1;
      char szHttpPort[6];
      char szFtpPort[6];
      char szGophPort[6];
      char szOthPort[6];

      if (0 != pcfg->usHttpPort) {
         SNPRINTF(szHttpPort,sizeof(szHttpPort),"%d",pcfg->usHttpPort);
      } else {
         szHttpPort[0] = '\0';
      }
      if (0 != pcfg->usFtpPort) {
         SNPRINTF(szFtpPort,sizeof(szFtpPort),"%d",pcfg->usFtpPort);
      } else {
         szFtpPort[0] = '\0';
      }
      if (0 != pcfg->usGophPort) {
         SNPRINTF(szGophPort,sizeof(szGophPort),"%d",pcfg->usGophPort);
      } else {
         szGophPort[0] = '\0';
      }
      if (0 != pcfg->usOthPort) {
         SNPRINTF(szOthPort,sizeof(szOthPort),"%d",pcfg->usOthPort);
      } else {
         szOthPort[0] = '\0';
      }

      /* Template:
        <html>
        <h2 align=center>Proxies</h2>
        <a href="lcgi:cfg.{invoke}.prox.reset">Reset to default</a>
        <form action="lcgi:cfg.{invoke}.prox.ok">
        <input type=checkbox name=httpon {httpon}>Proxy HTTP<br>
        Host <b>:</b> Port<br>
        <input type=text size=13 name=httphost value="{httphost}">&#160;<b>:</b>
        <input type=text size=5 name=httpport value="{httpport}"><br>
        <input type=checkbox name=ftpon {ftpon}>Proxy FTP<br>
        Host <b>:</b> Port<br>
        <input type=text size=13 name=ftphost value="{ftphost}">&#160;<b>:</b>
        <input type=text size=5 name=ftpport value="{ftpport}"><br>
        <input type=checkbox name=gophon {gophon}>Proxy Gopher<br>
        Host <b>:</b> Port<br>
        <input type=text size=13 name=gophost value="{gophhost}">&#160;<b>:</b>
        <input type=text size=5 name=gopport value="{gophport}"><br>
        <input type=checkbox name=othon {othon}>Proxy Other<br>
        Host <b>:</b> Port<br>
        <input type=text size=13 name=othhost value="{othhost}">&#160;<b>:</b>
        <input type=text size=5 name=othport value="{othport}"><br>
        Schemes: (';' separated)<br>
        <input type=text size=50 name=othschm value="{othschm}"><br>
        <a href="x-submit:">Save</a>
        </form>
        </html>
      */
      if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, 
                                 "httpon%s\0httphost%Qs\0httpport%Qs\0"
                                 "ftpon%s\0ftphost%Qs\0ftpport%Qs\0"
                                 "gophon%s\0gophhost%Qs\0gophport%Qs\0"
                                 "othon%s\0othschm%Qs\0"
                                 "othhost%Qs\0othport%Qs\0",
                                 (pcfg->bHttpOn ? "checked" : ""),
                                 cpszHttpHost, szHttpPort,
                                 (pcfg->bFtpOn ? "checked" : ""),
                                 cpszFtpHost, szFtpPort,
                                 (pcfg->bGophOn ? "checked" : ""),
                                 cpszGophHost, szGophPort,
                                 (pcfg->bOthOn ? "checked" : ""),
                                 cpszOthSchm, 
                                 cpszOthHost, szOthPort)) {
         nErr = ENOMEMORY;
      }
   }

   return nErr;
}

static int Config_ProxyReset(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr;

   nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                             IDX_CFGPROXYRES, ppBlob, pnBlobLen);

   return nErr;
}

static int Config_ProxyResetDone(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   int   nErr;
   char *pszDef = 0;

   nErr = IShell_LoadResSz(me->plcgie->piShell, BREWSER_RES_FILE, 
                           IDS_DEFPROXYSETTINGS, &pszDef);
   
   if (SUCCESS == nErr) {
      nErr = Brewser_SubmitProxyCfg(me->pbsr,pszDef);
   }
   
   FREEIF(pszDef);

   // load success page
   if (SUCCESS == nErr) {
      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                IDX_CFGPROXYRESX, ppBlob, pnBlobLen);
   }

   return nErr;
}

static int Config_ProxySubmit(LCGIResp *me, char *pszURLData, 
                              AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr;

   nErr = Brewser_SubmitProxyCfg(me->pbsr,pszURLData);

   // construct success page
   if (SUCCESS == nErr) {
      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                IDX_CFGPROXYSUB, ppBlob, pnBlobLen);
   }

   return nErr;
}

////////////////////
// Config Cookies //
////////////////////

/* lcgi:cfg.cookies */
static int Config_Cookies(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   int     nErr;
   boolean bCookiesOn;

   nErr = Brewser_GetCookiesEnabled(me->pbsr, &bCookiesOn);

   if (SUCCESS == nErr) {
      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                IDX_CFGCOOKIES, ppBlob, pnBlobLen);
   }

   if (SUCCESS == nErr) {

      /* Template:
         <html>
         <h2 align=center>Cookies</h2>
         <form action="lcgi:cfg.{invoke}.cook.sub">
         <input type=radio name=cookieson value=1 {enabled}>Enabled<br>
         <input type=radio name=cookieson value=0 {disabled}>Disabled<br>
         <a href="x-submit:">Save</a>
         </form>
         <a href="lcgi:cfg.{invoke}.cook.del">Delete all cookies</a><br>
         <a href="lcgi:cfg.{invoke}.cook.info">About cookies...</a>
         </html>
      */
      if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, 
                                 "enabled%s\0disabled%s\0",
                                 (bCookiesOn ? "checked" : ""),
                                 (bCookiesOn ? "" : "checked")  )) {
         nErr = ENOMEMORY;
      }
   }

   return nErr;
}


/* lcgi:cfg.cooksub */
static int Config_CookiesSubmit(LCGIResp *me, char *pszURLData, 
                                AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr;
   boolean bCookiesOn;
   const char *cpszCookiesOn = "1";
   char *pszEnabled = 0;

   // parse form fields
   IWebUtil_GetFormFields(me->plcgie->piWebUtil, pszURLData, 
                          "cookieson\0", &cpszCookiesOn);

   bCookiesOn = (0 != ATOI(cpszCookiesOn));

   nErr = Brewser_SetCookiesEnabled(me->pbsr,bCookiesOn);

   if (SUCCESS == nErr) {
      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                IDX_CFGCOOKSUB, ppBlob, pnBlobLen);
   }

   if (SUCCESS == nErr) {
      uint16 id = (bCookiesOn ? IDS_ENABLED : IDS_DISABLED);
      nErr = IShell_LoadResSz(me->plcgie->piShell, BREWSER_RES_FILE, 
                              id, &pszEnabled);
   }

   if (SUCCESS == nErr) {
      /* Template
         <html>
         Cookies are <b>{enab}.</b><br>
         <a href="brewser:back?to=lcgi:cfg.{invoke}.menu&past=lcgi:cfg.{invoke}.">Continue</a>
         </html>
      */
      if (0 < AEEResBlob_StrRepl(ppBlob,pnBlobLen,"enab%s\0",pszEnabled)) {
         nErr = ENOMEMORY;
      }
   }

   FREEIF(pszEnabled);
   return nErr;
}


/* lcgi:cfg.cookdel */
static int Config_CookiesDelete(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   // confirm 'Delete Cookies'
   int nErr;
   nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                             IDX_CFGCOOKDEL, ppBlob, pnBlobLen);
   return nErr;
}

/* lcgi:cfg.cookxdel */
static int Config_CookiesDeleteDone(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr;
   
   Brewser_CookiesClearAll(me->pbsr);

   nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                             IDX_CFGCOOKDELX, ppBlob, pnBlobLen);

   return nErr;
}




/* lcgi:cfg.cookinfo */
static int Config_CookiesInfo(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   return IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                             IDX_CFGCOOKINFO, ppBlob, pnBlobLen);
}


/////////////////
// Config Mail //
/////////////////
static int Config_Mail(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   const char *cpszMailTo;
   int   nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                   IDX_CFGMAIL, ppBlob, pnBlobLen);
   
   if (SUCCESS == nErr) {
      nErr = Brewser_GetMailToPtr(me->pbsr,&cpszMailTo);
   }
   
   if (SUCCESS == nErr) {
      /*
         <html>
         <h2 align=center>Mailto Handler</h2>
         <form action="lcgi:cfg.{invoke}.mail.sub">
         Enter URL for Mailto: links
         <input type=text size=50 name=mailto value="{mailto}">
         <a href="x-submit:">Save</a>
         </form>
         </html>
      */
      if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, 
                                 "mailto%Qs\0", cpszMailTo)) {
         nErr = ENOMEMORY;
      }
   }

   return nErr;
}

static int Config_MailSubmit(LCGIResp *me, char *pszURLData,
                             AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr;
   const char *cpszMailTo = "";

   IWebUtil_GetFormFields(me->plcgie->piWebUtil, pszURLData, 
                          "mailto\0", &cpszMailTo);

   nErr = Brewser_SetMailTo(me->pbsr, cpszMailTo);

   if (SUCCESS == nErr) {

      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                IDX_CFGMAILSUB, ppBlob, pnBlobLen);
   }

   if (SUCCESS == nErr) {
      /*
         <html>
         Mailto handler set to: {mailto}<br>
         <a href="brewser:back?to=lcgi:cfg.{invoke}.menu&past=lcgi:cfg.{invoke}.">Continue</a>
         </html>
      */
      if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, 
                                 "mailto%Qs\0", cpszMailTo)) {
         nErr = ENOMEMORY;
      }
   }
      
   return nErr;
}

static int Config_MailReset(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr;

   nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                             IDX_CFGMAILRES, ppBlob, pnBlobLen);

   return nErr;
}

static int Config_MailResetDone(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   int   nErr;
   char *pszDef = 0;

   nErr = IShell_LoadResSz(me->plcgie->piShell, BREWSER_RES_FILE, 
                           IDS_DEFMAILTO, &pszDef);
   
   if (SUCCESS == nErr) {
      nErr = Brewser_SetMailTo(me->pbsr,pszDef);
   }
   
   FREEIF(pszDef);

   // load success page
   if (SUCCESS == nErr) {
      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                IDX_CFGMAILRESX, ppBlob, pnBlobLen);
   }

   return nErr;
}

/////////////////
// Config HomePage //
/////////////////
static int Config_HomePage(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   const char *cpszHomePage;

   int nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                 IDX_CFGHOMEPAGE, ppBlob, pnBlobLen);
   
   if (SUCCESS == nErr) {
      nErr = Brewser_GetHomePagePtr(me->pbsr,&cpszHomePage);
   }
   
   if (SUCCESS == nErr) {
      /*
       <html>
       <h2 align=center>Home Page</h2>
       <form action="lcgi:cfg.{invoke}.homepage.sub">
       Enter Home Page URL
       <input type=text size=50 name=homepage value="{homepage}">
       <a href="x-submit:">Save</a>
       </form>
       </html>
      */
      if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, "homepage%Qs\0", 
                                 cpszHomePage)) {
         nErr = ENOMEMORY;
      }
   }

   return nErr;
}

static int Config_HomePageReset(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr;

   nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                             IDX_CFGHOMEPAGERES, ppBlob, pnBlobLen);

   return nErr;
}

static int Config_HomePageResetDone(LCGIResp *me, AEEResBlob **ppBlob, int *pnBlobLen)
{
   int   nErr;
   char *pszDef = 0;

   nErr = IShell_LoadResSz(me->plcgie->piShell, BREWSER_RES_FILE, 
                           IDS_DEFHOMEPAGE, &pszDef);
   
   if (SUCCESS == nErr) {
      nErr = Brewser_SetHomePage(me->pbsr,pszDef);
   }
   
   FREEIF(pszDef);

   // load success page
   if (SUCCESS == nErr) {
      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                IDX_CFGHOMEPAGERESX, ppBlob, pnBlobLen);
   }

   return nErr;
}

static int Config_HomePageSubmit(LCGIResp *me, char *pszURLData,
                                 AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr;
   const char *cpszHomePage = "";
   char *pHomePageData;

   IWebUtil_GetFormFields(me->plcgie->piWebUtil, pszURLData, 
                          "homepage\0", &cpszHomePage);

   {
      int nLen = IWebUtil_GuessUrl(me->plcgie->piWebUtil,cpszHomePage, 0, 0);

      pHomePageData = (char *)MALLOC(nLen);
   
      if ((char *)0 != pHomePageData) {
         IWebUtil_GuessUrl(me->plcgie->piWebUtil,cpszHomePage,
                           pHomePageData, nLen);
         cpszHomePage = pHomePageData;
      }
   }

   nErr = Brewser_SetHomePage(me->pbsr, cpszHomePage);

   if (SUCCESS == nErr) {
      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                IDX_CFGHOMEPAGESUB, ppBlob, pnBlobLen);
   }

   if (SUCCESS == nErr) {
      /*
         <html>
         Home Page set to: {homepageto}<br>
         <a href="brewser:back?to=lcgi:cfg.{invoke}.menu&past=lcgi:cfg.{invoke}.">Continue</a>
         </html>
      */
      if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, 
                                 "homepage%Qs\0", cpszHomePage)) {
         nErr = ENOMEMORY;
      }
   }

   FREEIF(pHomePageData);
   return nErr;
}


/* lcgi:cfg */
int LCGIResp_Config(LCGIResp *me, UrlParts *pup)
{
   int nErr = EFAILED;
   const char *cpszUrl = (char*)MEMCHR(pup->cpcPath, '.', UP_PATHLEN(pup)) + 1;
   
   AEEResBlob *pBlob = 0;
   int nBlobLen = 0;

   if (!LCGIResp_FindOrMakeInvoke(me,pup,&cpszUrl)) {
      return SUCCESS;
   }

   // lcgi:cfg.menu
   if (STRBEGINS("menu", cpszUrl)) {
      nErr = Config_Menu(me, &pBlob, &nBlobLen);
   } else if (LCGIResp_SafeReferer(me)) {
      if (STRBEGINS("proxies.reset.sub", cpszUrl)) {
         nErr = Config_ProxyResetDone(me, &pBlob, &nBlobLen);
      }
      else if (STRBEGINS("proxies.reset", cpszUrl)) {
         nErr = Config_ProxyReset(me, &pBlob, &nBlobLen);
      }
      else if (STRBEGINS("proxies.ok", cpszUrl)) {
         char *pszURLData = 0;
         nErr = ERR_STRDUP((char*)pup->cpcSrch + 1, &pszURLData);
         if (SUCCESS == nErr) {
            nErr = Config_ProxySubmit(me,pszURLData,&pBlob,&nBlobLen);
         }
         FREEIF(pszURLData);
      }
      else if (STRBEGINS("proxies", cpszUrl)) {
         nErr = Config_Proxies(me, &pBlob, &nBlobLen);
      } 
      else if (STRBEGINS("cookies.sub", cpszUrl)) {
         char *pszURLData = 0;
         nErr = ERR_STRDUP((char*)pup->cpcSrch + 1, &pszURLData);
         if (SUCCESS == nErr) {
            nErr = Config_CookiesSubmit(me, pszURLData, &pBlob, &nBlobLen);
         }
         FREEIF(pszURLData);
      }
      else if (STRBEGINS("cookies.del.sub", cpszUrl)) {
         nErr = Config_CookiesDeleteDone(me, &pBlob, &nBlobLen);
      }
      else if (STRBEGINS("cookies.del", cpszUrl)) {
         nErr = Config_CookiesDelete(me, &pBlob, &nBlobLen);
      }
      else if (STRBEGINS("cookies.info", cpszUrl)) {
         nErr = Config_CookiesInfo(me, &pBlob, &nBlobLen);
      }
      else if (STRBEGINS("cookies", cpszUrl)) {
         nErr = Config_Cookies(me, &pBlob, &nBlobLen);
      }
      else if (STRBEGINS("mailto.reset.sub", cpszUrl)) {
         nErr = Config_MailResetDone(me, &pBlob, &nBlobLen);
      }
      else if (STRBEGINS("mailto.reset", cpszUrl)) {
         nErr = Config_MailReset(me, &pBlob, &nBlobLen);
      }
      else if (STRBEGINS("mailto.sub", cpszUrl)) {
         char *pszURLData = 0;
         nErr = ERR_STRDUP((char*)pup->cpcSrch + 1, &pszURLData);
         if (SUCCESS == nErr) {
            nErr = Config_MailSubmit(me, pszURLData, &pBlob, &nBlobLen);
         }
         FREEIF(pszURLData);
      }
      else if (STRBEGINS("mailto", cpszUrl)) {
         nErr = Config_Mail(me, &pBlob, &nBlobLen);
      }
      else if (STRBEGINS("homepage.reset.sub", cpszUrl)) {
         nErr = Config_HomePageResetDone(me, &pBlob, &nBlobLen);
      }
      else if (STRBEGINS("homepage.reset", cpszUrl)) {
         nErr = Config_HomePageReset(me, &pBlob, &nBlobLen);
      }
      else if (STRBEGINS("homepage.sub", cpszUrl)) {
         char *pszURLData = 0;
         nErr = ERR_STRDUP((char*)pup->cpcSrch + 1, &pszURLData);
         if (SUCCESS == nErr) {
            nErr = Config_HomePageSubmit(me, pszURLData, &pBlob, &nBlobLen);
         }
         FREEIF(pszURLData);
      }
      else if (STRBEGINS("homepage", cpszUrl)) {
         nErr = Config_HomePage(me, &pBlob, &nBlobLen);
      } 
   }

   if ((AEEResBlob *)0 != pBlob) {

      if (0 < AEEResBlob_StrRepl(&pBlob, &nBlobLen,
                                 "invoke%s\0", me->szInvoke)) {
         nErr = ENOMEMORY;
      }
   
      if (SUCCESS == nErr) {
         nErr = ISOURCEUTIL_SourceFromMemory(me->plcgie->piSourceUtil, 
                                             RESBLOB_DATA(pBlob),
                                             STRLEN(RESBLOB_DATA(pBlob)),
                                             GET_HELPER()->free, pBlob,
                                             &me->wri.pisMessage);
      }

      if (SUCCESS == nErr) {
         me->wri.lContentLength = STRLEN(RESBLOB_DATA(pBlob));
         me->wri.nCode = WEBCODE_OK;
      } else {
         FREE(pBlob);
      }
   }

   return nErr;
}






