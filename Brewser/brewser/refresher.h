#ifndef REFRESHER_H
#define REFRESHER_H /* #ifndef REFRESHER_H */
/*
  ===========================================================================
  
  FILE: refresher.h
   
  SERVICES:  
     Refresher! 
   
  DESCRIPTION: 
     Parse and act on an HTTP Refresh: header, call me back with a URL 
     after a certain amount of time.

  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/

#include "AEEShell.h"

typedef struct Refresher Refresher;

/**
  || Function
  || --------
  || typedef void (*RefresherCB)(void *p, const char *cpszUrl);
  ||
  || Description
  || -----------
  || time to refresh callback
  ||
  || Parameters
  || ----------
  || void *p: user-supplied context, given to Start()
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
typedef void (*RefresherCB)(void *p, const char *cpszUrl);


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

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
                     void        *pNotifyData);

/**
  || Function
  || --------
  || void Refresher_Fire(void *p)
  ||
  || Description
  || -----------
  || internal callback runner, normally, but can be
  ||  used to force the callback immediately
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
void Refresher_Fire(void *p);

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
int Refresher_New(char *pszRefresh, Refresher **pprfsher);

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
void Refresher_Delete(Refresher *me);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef REFRESHER_H */
