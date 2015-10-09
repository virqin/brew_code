/*
  =========================================================================== 
  FILE: lcgieng.h
  
  SERVICES: LCGI IWebEng
    
  GENERAL DESCRIPTION:

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
#ifndef LCGIENG_H
#define LCGIENG_H

#include "lcgieng.bid"     /* extension class ID */

#include "brewser.h"

/* =========================================================================
   Types
   ========================================================================= */

typedef struct IWebEng
{
   IWebEngVtbl      *pvt;
   IWebEngVtbl       vt;
   unsigned          uRefs;

   IModule          *pim;    /* my owner */

   IShell           *piShell; /* shell services */
   ISourceUtil      *piSourceUtil; /* source util */
   IWebUtil         *piWebUtil;    /* web util */

} LCGIEng;

typedef struct IWebResp
{
   IWebRespVtbl *pvt;
   IWebRespVtbl  vt;

   uint32        uRefs;
   
   LCGIEng      *plcgie;   /* my owner */
   
   AEECallback   cb;

   WebRespInfo   wri;
   
   PFNWEBHEADER  pfnHeaderHandler;
   void         *pHandlerData;

   IWebResp    **ppiwresp;
   AEECallback  *pcbReturn;
   
   UrlParts     *pup; /* pointers into the URL, not copied */

   IWeb         *piWeb;
   
   char         *pszReferer;

   char          szInvoke[9]; /* eight hex digits of random data, 
                                 null terminated */ 
   
   Brewser      *pbsr;        /* the BREWser we're working for */

} LCGIResp;



int LCGIEng_New(IModule *pim,IShell *piShell, AEECLSID clsid, IWebEng **pp);

void LCGIResp_Header(LCGIResp *me, const char *cpszName, const char *cpszValue);
boolean LCGIResp_SafeReferer(LCGIResp *me);

int LCGIResp_Bookmarks(LCGIResp *me, UrlParts *pup);

int LCGIResp_Config(LCGIResp *me, UrlParts *pup);

boolean LCGIResp_FindOrMakeInvoke(LCGIResp *me, UrlParts *pup, 
                                  const char **pcpszInvoke);


#endif //LCGIENG_H


