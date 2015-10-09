/*
  ===========================================================================

  FILE:  brewser.h
  
  SERVICES:  
    Provides access to Brewser object
  
  GENERAL DESCRIPTION:
    BREWser is a World-Wide Web browser implemented using built-in features
     of BREW.
  
  REVISION HISTORY: 
    9/13/2002  5:59pm Created

  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/
#ifndef BREWSER_H
#define BREWSER_H

// private WebOpt for storing Brewser* 
#define WEBOPT_PRIVBREWSER    (WEBOPT_32BIT_LAST-1)

// The Brewser applet object
typedef struct Brewser Brewser;

typedef struct BrewserProxyCfg {
   flg         bHttpOn  : 1;
   flg         bFtpOn   : 1;
   flg         bGophOn  : 1;
   flg         bOthOn   : 1;
   uint16      usHttpPort;
   uint16      usFtpPort;
   uint16      usGophPort;
   uint16      usOthPort;
   char        szStrings[5]; /* include space for null-termination */
   /* where szStrings is over-allocated, laid out like so:

      httphost\000ftphost\000gophhost\000othhost\000othschm\000
   */

} BrewserProxyCfg;

int Brewser_SubmitProxyCfg(Brewser *me, char *pszSrch);

int Brewser_GetProxyCfgPtr(Brewser *me, const BrewserProxyCfg **pcfg);
int Brewser_SetProxyCfg(Brewser *me, const BrewserProxyCfg *pcfg, int nSize);

int Brewser_GetMailToPtr(Brewser *me, const char **pcpsz);
int Brewser_SetMailTo(Brewser *me, const char *cpsz);

int Brewser_GetHomePagePtr(Brewser *me, const char **pcpsz);
int Brewser_SetHomePage(Brewser *me, const char *cpsz);

int Brewser_GetCookiesEnabled(Brewser *me, boolean *pbEnabled);
int Brewser_SetCookiesEnabled(Brewser *me, boolean bEnabled);

void Brewser_CookiesClearAll(Brewser *me);


/*
  || brewser resource info
 */
#include "brewser_res.h"

#ifndef BREWSER_RES_FILE
#define BREWSER_RES_FILE "brewser.bar"
#endif /* #ifndef BREWSER_RES_FILE */

#endif //BREWSER_H
