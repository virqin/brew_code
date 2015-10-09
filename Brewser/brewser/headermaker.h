#ifndef HEADERMAKER_H
#define HEADERMAKER_H
/*
  ===========================================================================
  
  FILE: headermaker.h
   
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
#include "AEESource.h"


/**
  || Function
  || --------
  || typedef boolean (*PFNHEADERMADE)(void *pMadeData, const char *cpszName,
  ||                                  char *pszValue);
  ||
  ||
  || Description
  || -----------
  || client-supplied callback that HeaderMaker calls with a complete 
  ||   header
  ||
  || Parameters
  || ----------
  || void *pMadeData: user data (supplied to HeaderMaker_CtorZ)
  || const char *cpszName: header name that's made
  || char *pszValue: memory buffer holding null-terminated value of
  ||                  the header
  ||
  || Returns
  || -------
  || TRUE if the caller wants to keep pszValue (which must then be freed 
  ||    using FREE()
  || FALSE otherwise
  ||
  || Remarks
  || -------
  || If a headermaker fails to re-assemble a header because of an allocation
  ||   failure, pfnHeaderMade is called with pszValue set to NULL.
  ||
  */
typedef boolean (*PFNHEADERMADE)(void *pMadeData, const char *cpszName,
                                 char *pszValue);

typedef struct HeaderMaker
{
   const char    *cpszzHeaders;
   
   const char    *cpszMaking;

   char          *pszMade;
   int            nMadeLen;
   
   PFNHEADERMADE  pfnMade;
   void          *pMadeData;
} HeaderMaker;


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**
  || Function
  || --------
  || void HeaderMaker_CtorZ(HeaderMaker *me, const char *cpszzHeaders,
  ||                        PFNHEADERMADE pfnMade, void *pMadeData);
  || 
  ||
  || Description
  || -----------
  || initialize a HeaderMaker
  ||
  || Parameters
  || ----------
  || HeaderMaker *me: zero-initialized memory into which a HeaderMaker
  ||                    can pour itself
  || const char *cpszzHeaders: doubly-null terminated string, names of 
  ||                           headers to pull out of the header "stream",
  ||                           not copied!
  || PFNHEADERMADE pfnMade: where to call with a re-constructed header
  || void *pMadeData: what to call pfnMade with
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  || cpszzHeaders must stay valid for the life of HeaderMaker, HeaderMaker
  ||  will not copy nor free the headers
  ||
  */
void HeaderMaker_CtorZ(HeaderMaker *me, const char *cpszzHeaders,
                       PFNHEADERMADE pfnMade, void *pMadeData);

/**
  || Function
  || --------
  || void HeaderMaker_Dtor(HeaderMaker *me);
  ||
  || Description
  || -----------
  || cleanup after a HeaderMaker
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
  || doesn't free me
  ||
  */
void HeaderMaker_Dtor(HeaderMaker *me);

/**
  || Function
  || --------
  || void HeaderMaker_Header(void *p, const char *cpszName, GetLine *pglVal)
  ||
  || Description
  || -----------
  || received header callback for a web transaction
  ||
  || Parameters
  || ----------
  || void *p: the HeaderMaker 
  || const char *cpszName: the name of the web header (like "Content-Type")
  || GetLine *pglVal: the value of the header, like "text/html"
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  || cpszName must be NULL in the case of continuation header line parts..
  ||
  */
void HeaderMaker_Header(void *p, const char *cpszName, GetLine *pglVal);


/**
  || Function
  || --------
  || const char *HeaderMaker_SetHeaders(HeaderMaker *me,
  ||                                    const char *cpszzHeaders);
  ||
  || Description
  || -----------
  || re-set the headers of interest to cpszzHeaders
  ||
  || Parameters
  || ----------
  || HeaderMaker *me: the HeaderMaker
  || const char *cpszzHeaders: doubly-null terminated list of headers of 
  ||                            interest (not copied)
  ||
  || Returns
  || -------
  || what cpszzHeaders were previously set to 
  ||
  || Remarks
  || -------
  || cpszzHeaders must stay valid for the life of HeaderMaker, HeaderMaker
  ||  will not copy nor free the headers
  ||
  */
const char *HeaderMaker_SetHeaders(HeaderMaker *me,const char *cpszzHeaders);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef HEADERMAKER_H */
