/*
  ===========================================================================

  FILE:  bookmarks.h
  
  SERVICES:  
    IWeb API sample applet.
  
  GENERAL DESCRIPTION:
    Bookmarks UI for Brewser applet implmented as a local CGI approach.
  
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
#ifndef _BOOKMARKS_H_
#define _BOOKMARKS_H_

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


int LCGIResp_Bookmarks(LCGIResp *me, UrlParts *pup);

int LCGIResp_Config(LCGIResp *me, UrlParts *pup);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif   //_BOOKMARKS_H_

