/*
  ===========================================================================
  
  FILE: refresher.c
   
  SERVICES:  
     Refresher! 
   
  DESCRIPTION: 
     Call me back with a URL after a certain amount of time

  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/

#include "refresher.h"
#include "AEEWeb.h"
#include "util.h"

struct Refresher 
{
   AEECallback  cbTimer;

   RefresherCB  pfnNotify;
   void        *pNotifyData;

   int          nTime;
   char        *pszUrl;
};


/**
  || Function
  || --------
  || void Refresher_Fire(void *p)
  ||
  || Description
  || -----------
  || internal callback runner
  ||
  || Parameters
  || ----------
  || void *p: the Refresher
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
void Refresher_Fire(void *p)
{
   Refresher *me = (Refresher *)p;

   me->pfnNotify(me->pNotifyData,me->pszUrl);
   /* I'll probably be free here, so don't reference me hence */
}

/**
  || Function
  || --------
  || void Refresher_Start(Refresher   *me,
  ||                      IShell      *piShell,
  ||                      RefresherCB  pfnNotify,
  ||                      void        *pNotifyData)
  || 
  ||
  || Description
  || -----------
  || kicks off a Refresher
  ||
  || Parameters
  || ----------
  || Refresher *me: the Refresher
  || IShell *piShell: a BREW IShell (to set a timer)
  || PFNNOTIFY pfnNotify: where to call with URL after timer expires
  || void *pNotifyData: what to call pfnNotify with, user-context
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
void Refresher_Start(Refresher   *me,
                     IShell      *piShell,
                     RefresherCB  pfnNotify,
                     void        *pNotifyData)
{
   me->pfnNotify   = pfnNotify;
   me->pNotifyData = pNotifyData;

   ISHELL_SetTimerEx(piShell,me->nTime*1000,&me->cbTimer);
}

/**
  || Function
  || --------
  || int Refresher_New(char *pszRefresh, Refresher **pprfsher)
  ||
  || Description
  || -----------
  || create a new Refresher object
  ||
  || Parameters
  || ----------
  || const char *cpszRefresh: content of a Refresh header, Refresher
  ||                           parses the URL out for ya!
  || Refresher **pprfsher: pointer to pointer to a new Refresher, 
  ||                        filled by this call
  ||
  || Returns
  || -------
  || SUCCESS on SUCCESS
  || ENOMEMORY on allocation failure
  || EBADPARM if no URL could be parsed out
  ||
  || Remarks
  || -------
  ||
  */
int Refresher_New(char *pszRefresh, Refresher **pprfsher)
{
   char      *pszUrl = 0;
   Refresher *me;
   int        nTime;
   WebNVPair  wnvp;

   /* 
      Every browser seems to implement negative timeouts and "busted"
      Refresh headers differently.  This is because the Refresh 
      header "spec" isn't a spec.
      
      If you think my code sucks, try here for a "spec":
      
        http://home.netscape.com/assist/net_sites/pushpull.html
      
      Weak.
   */

   *pprfsher = 0;

   if (((char *)0 == pszRefresh) || 
        (0 == ParseNVPairs(&pszRefresh, ";", '=', 0, &wnvp, 1))) {
      return EBADPARM; /* no digits */
   }
   /* 
      expect the first "nv pair" to be just a number, so value will be 
      empty, if it's not, bail
   */
   if (STRLEN(wnvp.pcValue)) {
      return EBADPARM;
   }

   {
      char *pc;

      nTime = STRTOUL(wnvp.pcName,&pc,10);
      if (pc == wnvp.pcName) {
         return EBADPARM; /* not a good header, can't make a number out 
                             of the so-called time */
      }
   }

   nTime = MAX(0,nTime); /* treat negatives as 0, which is what
                            Mozilla does */
      
   while (0 != ParseNVPairs(&pszRefresh, ";", '=', 0, &wnvp, 1)) {
      if (!STRICMP(wnvp.pcName,"url")) {
         FREEIF(pszUrl);
         pszUrl = STRDUP(wnvp.pcValue);
         if ((char *)0 == pszUrl) {
            return ENOMEMORY;
         }
      }
   }

   if ((char *)0 != pszUrl) {
      int nLen = dequote(pszUrl, -1);
      pszUrl[nLen] = 0;
   }

   *pprfsher = me = MALLOCREC(Refresher);

   if ((Refresher *)0 == me) {
      FREEIF(pszUrl);
      return ENOMEMORY;
   }
   
   CALLBACK_Init(&me->cbTimer,Refresher_Fire,me);
   me->pszUrl = pszUrl;
   me->nTime = nTime;

   return SUCCESS;
}

/**
  || Function
  || --------
  || void Refresher_Delete(Refresher *me)
  ||
  || Description
  || -----------
  || free a Refresher and cleanup, this cancels pfnNotify
  ||
  || Parameters
  || ----------
  || Refresher *me: the Refresher 
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
void Refresher_Delete(Refresher *me)
{
   CALLBACK_Cancel(&me->cbTimer);
   FREEIF(me->pszUrl);
   FREE(me);
}
