/*
  ===========================================================================

  FILE:  prefmgr.h
  
  SERVICES:  
    Get/Set config preferences.
  
  GENERAL DESCRIPTION:
    Brewser persistent configuration preferences.
    
  
  REVISION HISTORY: 
    9/16/2002  2:13pm Created

  ===========================================================================
  ===========================================================================
    
               Copyright © 1999-2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/
#ifndef PREFMGR_H
#define PREFMGR_H

#include "AEE.h" /* for IShell, uint16 */


typedef struct {
   IShell  *piShell;
   byte    *pData;
   uint16   cbSize;
   uint16   cbAlloc;
   flg      bDirty : 1;
} Prefs;

void Prefs_CtorZ  (Prefs *me, IShell *piShell);
void Prefs_Dtor   (Prefs *me);
void Prefs_Update (Prefs *me);
void Prefs_Flush  (Prefs *me);
int  Prefs_Replace(Prefs *me, uint16 id, const void *pData, int nDataSize);
int  Prefs_GetPtr (Prefs *me, uint16 id, const void **ppPtr, int *pnSize);
int  Prefs_Get    (Prefs *me, uint16 id, void *pData, int *pnSize);


#endif //PREFMGR_H

