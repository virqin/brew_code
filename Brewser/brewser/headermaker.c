/*
  ===========================================================================

  FILE: headermaker.c
   
  SERVICES:  
     IWeb header re-assembly 
   
  DESCRIPTION: 
     HeaderMaker handles an IWeb header callback, re-assembles the current
      header from GetLine struct(s), and passes the header (and 
      optionally the memory) to its owner

  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/

#include "AEEStdLib.h"     /* for WSTRLEN, etc */
#include "headermaker.h"

/* =======================================================================
   HeaderMaker stuff
   ======================================================================= */

/**
  || Function
  || --------
  || static void HeaderMaker_Reset(HeaderMaker *me)
  ||
  || Description
  || -----------
  || forget about currently building header (internal)
  ||
  || Parameters
  || ----------
  || HeaderMaker *me: the HeaderMaker
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static void HeaderMaker_Reset(HeaderMaker *me)
{
   FREEIF(me->pszMade);
   me->nMadeLen = 0;
   me->cpszMaking = "";
}

const char *HeaderMaker_SetHeaders(HeaderMaker *me, const char *cpszzHeaders)
{
   const char *cpszzOld = me->cpszzHeaders;

   me->cpszzHeaders = cpszzHeaders;
   if ((const char *)0 == cpszzHeaders) {
      me->cpszzHeaders = "";
   }
   return cpszzOld;
}

void HeaderMaker_CtorZ(HeaderMaker *me, const char *cpszzHeaders,
                       PFNHEADERMADE pfnMade, void *pMadeData)
{
   me->pfnMade      = pfnMade;
   me->pMadeData    = pMadeData;
   me->cpszMaking   = "";

   HeaderMaker_SetHeaders(me,cpszzHeaders);
}

void HeaderMaker_Dtor(HeaderMaker *me)
{
   FREEIF(me->pszMade);
   ZEROAT(me);
}

/**
  || Function
  || --------
  || static void HeaderMaker_Header(void *p, const char *cpszName, 
  ||                                GetLine *pglVal)
  ||
  || Description
  || -----------
  || received header callback for a web transaction
  ||
  || Parameters
  || ----------
  || void *p: a HeaderMaker (the subscriber)
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
void HeaderMaker_Header(void *p, const char *cpszName, GetLine *pglVal)
{
   HeaderMaker *me = (HeaderMaker *)p;

   if (('\0' == *me->cpszMaking)/*  && ((char *)0 != cpszName) */) {
      const char *cpsz;

      for (cpsz = me->cpszzHeaders; 
           ('\0' != *cpsz) && STRICMP(cpszName,cpsz); 
           cpsz += STRLEN(cpsz)+1);
      
      me->cpszMaking = cpsz;
   }

   if ('\0' == *me->cpszMaking) {
      /* not a header I care about */
      return;
   }

   /* continuation of a header I *do* care about */
   {
      char *pszNewMade;
      pszNewMade = (char *)REALLOC(me->pszMade,me->nMadeLen+pglVal->nLen+1);
      if ((char *)0 == pszNewMade) {
         /* tell client we failed... */
         me->pfnMade(me->pMadeData,me->cpszMaking,0);
         /* bail, give up on this header... */
         HeaderMaker_Reset(me);
         return;
      }
      me->pszMade = pszNewMade;
   }

   MEMMOVE(me->pszMade+me->nMadeLen,pglVal->psz,pglVal->nLen);
   me->nMadeLen += pglVal->nLen;
   /* @@@ pre-2.1 versions of BREW may not zero-initialize
      the new memory when reallocing larger */
   me->pszMade[me->nMadeLen] = 0;

   if (!pglVal->bTruncated) {
      /* done! */
      if (me->pfnMade(me->pMadeData,me->cpszMaking,me->pszMade)) {
         /* they took it */
         me->pszMade = 0;
      }
      HeaderMaker_Reset(me);
   }
}

