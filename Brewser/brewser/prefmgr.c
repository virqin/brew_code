/*
  ===========================================================================

  FILE:  prefmgr.c
  
  SERVICES:  
    Get/Set config preferences.
  
  GENERAL DESCRIPTION:
    Persistent configuration preferences manager.
  
  REVISION HISTORY: 
    9/16/2002  2:13pm Created

  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/
#include "AEEStdLib.h"     /* for WSTRLEN, etc */
#include "AEEShell.h"
#include "prefmgr.h"
#include "util.h"

#define PREFVER   2     // preference version

typedef struct PrefHeader {
   uint16   nSize; /* true size, though records are padded to 32 bits */
   uint16   nId;
} PrefHeader;


void Prefs_CtorZ(Prefs *me, IShell *piShell)
{
   me->piShell = piShell;
   ISHELL_AddRef(piShell);
}

void Prefs_Dtor(Prefs *me)
{
   Prefs_Update(me);

   RELEASEIF(me->piShell);
   FREEIF(me->pData);
   me->cbSize = me->cbAlloc = 0;
}

static int Prefs_Grow(Prefs *me, int cbSpaceNeeded)
{
   if ((cbSpaceNeeded > 0) && ((me->cbSize + cbSpaceNeeded) > me->cbAlloc)) {
      int cbNew = (((me->cbAlloc + cbSpaceNeeded) / 128) + 1) * 128;
      void *pNew = REALLOC(me->pData, cbNew);

      if ((void *)0 == pNew) {
         return ENOMEMORY;
      }

      me->pData = pNew;
      me->cbAlloc = cbNew;
   }
   return SUCCESS;
}

static int Prefs_Load(Prefs *me)
{
   int nErr = SUCCESS;

   if ((byte *)0 == me->pData) {
      int nSize = ISHELL_GetAppPrefs(me->piShell, PREFVER, 0, 0);
      nErr = Prefs_Grow(me, nSize);

      if (SUCCESS == nErr) {
         nErr = ISHELL_GetAppPrefs(me->piShell, PREFVER, me->pData, 
                                   me->cbAlloc);
         me->cbSize = nSize;
      }
   }

   return nErr;
}


/*
   Finds a PrefHeader that matches 'id'
   in the prefs data blob. Knows when it
   has reached the end by me->cbSize
*/
static PrefHeader *Prefs_Find(Prefs *me, uint16 nId)
{
   byte *ph;
   byte *pEnd;
   
   pEnd = me->pData + me->cbSize;

   for (ph = me->pData; ph < pEnd; 
        ph += ((((PrefHeader *)ph)->nSize+3)&~3)) {

      if (((PrefHeader *)ph)->nId == nId) {
         return (PrefHeader *)ph;
      }
   }
   
   return 0;
}



void Prefs_Update(Prefs *me)
{
   if (me->bDirty && me->pData && me->cbSize) {
      ISHELL_SetAppPrefs(me->piShell, PREFVER, me->pData, me->cbSize);
      me->bDirty = 0;
   }
}

/* Updates the Shell's preference database and
   frees memory allocated on behalf of Prefs object.
   Use Flush when you are done with Prefs for a while.
*/
void Prefs_Flush(Prefs *me)
{
   Prefs_Update(me);
   FREEIF(me->pData);
   me->cbSize = 0;
   me->cbAlloc = 0;
   me->bDirty = 0;
}

/* Returns:
   SUCCESS
   ENOMEMORY   memory could not be allocated
*/   
int Prefs_Replace(Prefs *me, uint16 nId, const void *pData, int nDataSize)
{
   int nErr = SUCCESS;
   int nNewBlkSize = ((nDataSize+3)&~3) + sizeof(PrefHeader);
   PrefHeader *ph = 0;
   
   nErr = Prefs_Load(me);

   if (SUCCESS == nErr) {
      
      ph = Prefs_Find(me, nId);

      if ((PrefHeader *)0 != ph) {
   
         // if we found an existing record
         byte *pe     = (byte*)ph + ((ph->nSize+3)&~3);
         byte *ps     = (byte*)(ph + 1);
         int blklen   = (int)me->cbSize - (pe - me->pData);
         int diff     = nNewBlkSize - ((ph->nSize+3)&~3);
   
         if (0 != diff) {
            // adjust size
            nErr = Prefs_Grow(me, diff);
   
            if (SUCCESS == nErr) {
               // move data to adjust for string
               MEMMOVE(pe + diff, pe, blklen);
               ph->nSize = nDataSize+sizeof(PrefHeader);
               me->cbSize += diff;
            }
         }
   
         if (SUCCESS == nErr) {
            // move new data into newly adjusted space
            MEMMOVE(ps, pData, nDataSize);
         }
   
      } else {
         
         nErr = Prefs_Grow(me, nNewBlkSize);
         
         if (SUCCESS == nErr) {

            byte *pe = me->pData + me->cbSize;

            ph = (PrefHeader*)pe;
            ph->nId = nId;
            ph->nSize = nDataSize+sizeof(PrefHeader);
            pe = (byte*)(ph + 1);
   
            MEMMOVE(pe, pData, nDataSize);
            me->cbSize += nNewBlkSize;
         }

      }
   }

   if (nErr == SUCCESS) {
      me->bDirty = 1;
   }

   return nErr;
}

int Prefs_GetPtr(Prefs *me, uint16 id, const void **ppPtr, int *pnSize)
{
   void *pPtr = 0;

   int nErr = Prefs_Load(me);

   if (SUCCESS == nErr) {
      PrefHeader *ph = Prefs_Find(me, id);

      if ((PrefHeader *)0 != ph) {
         *pnSize = ph->nSize - sizeof(PrefHeader);
         pPtr = (void *)(ph + 1);
      } else {
         nErr = EBADITEM;
      }
   }

   *ppPtr = pPtr;

   return nErr;
}

/*
   Returns:
      SUCCESS if successful.
      EBADITEM if item not found
      ENOMEMORY if could not allocate memory
      
      if 'pData' == NULL or cbSize is not sufficient to
      store the data, it will return a negative number, 
      the absolute value of which indicates the size
      required to store the data.
*/
int Prefs_Get(Prefs *me, uint16 id, void *pData, int *pnSize)
{
   int          nSize;
   int          nErr;
   const void  *p;

   nErr = Prefs_GetPtr(me, id, &p, &nSize);

   if (SUCCESS == nErr) {

      if ((void *)0 != pData) {
         if (nSize <= (uint16)*pnSize) {
            MEMCPY(pData, p, nSize);
         } else {
            nErr = EBADPARM;
         }
      }

      *pnSize = nSize;
   }

   return nErr;
}

#if 0

void Prefs_UnitTest(IShell *piShell)
{
   Prefs prf;

#if 0
   {
      int err = ISHELL_SetAppPrefs(piShell, PREFVER, 0, 0);
   }
#endif /* if 0 */

#if 1
   typedef struct {
      int ione;
      int itwo;
      int ithree;
   } test;

   char *psz;
   int  nSize;
   test *pt;
   test t;
   int nErr;

   ZEROAT(&prf);

   Prefs_CtorZ(&prf, piShell);

   t.ione = 1;
   t.itwo = 2;
   t.ithree = 3;

   Prefs_Replace(&prf, 1, "hello there", sizeof("hello there"));
   Prefs_Replace(&prf, 2, "another string", sizeof("another string"));
   Prefs_Replace(&prf, 3, &t, sizeof(t));
   Prefs_Replace(&prf, 4, "more here", sizeof("more here"));
   
   Prefs_Replace(&prf, 2, "replace the second string", sizeof("replace the second string"));
   Prefs_Replace(&prf, 1, "replace the first freakin string", sizeof("replace the first freakin string"));
   Prefs_Replace(&prf, 4, "try the last string", sizeof("try the last string"));

   nErr = Prefs_GetPtr(&prf, 1, (const void **)&psz, &nSize);
   
   Prefs_Replace(&prf, 1, "short str", sizeof("short str"));

   // get the stuff
   nErr = Prefs_GetPtr(&prf, 1, (const void **)&psz ,&nSize);
   nErr = Prefs_GetPtr(&prf, 2, (const void **)&psz ,&nSize);
   nErr = Prefs_GetPtr(&prf, 3, (const void **)&pt  ,&nSize);
   nErr = Prefs_GetPtr(&prf, 4, (const void **)&psz ,&nSize);

   Prefs_Flush(&prf);

   Prefs_Dtor(&prf);
#endif /* if 1 */
}

#endif /* if 0 */
