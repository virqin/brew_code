/*
  ===========================================================================

  FILE:  bookmarks.c
  
  SERVICES:  
    IWeb API sample applet - bookmarks feature.
  
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
#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEWeb.h"
#include "AEESource.h"

#include "brewser.h"

#include "lcgieng.h"

#include "util.h"


//////////////////////////////////////////////////////////////////////////////
// BkmFile object

#define BOOKMARKS_FILE  "bookmarks.fdb"


typedef struct BkmDB BkmDB;
struct BkmDB {
   FlatDB      fdb;

   // "current" record:  (holds result from interation/find functions)
   char *      pcAlloc;    // allocated copy
   int         id;
   char *      pszName;
   char *      pszURL;
};

static boolean BkmDB_Open(BkmDB *me, IShell *piShell)
{
   return (FlatDB_Ctor(&me->fdb, piShell, BOOKMARKS_FILE) == SUCCESS);
}

static void BkmDB_Dtor(BkmDB *me)
{
   FREEIF(me->pcAlloc);
   FlatDB_Dtor(&me->fdb);
}


// Read to end of string (\0) or field (\001).  If found, replace field delimiter \001 with \0.
//
// Returns pointer to next field, if there is a next field, or end of string otherwise.
//
static char *ReadField(char *psz)
{
   while (*psz) {
      if (*psz == '\001') {
         *psz = '\0';
         ++psz;
         break;
      }
      ++psz;
   }
   return psz;
}

// Set id, pszName, and pszURL.  Values will always be non-NULL on success.
//
static int BkmDB_ReadRecord(BkmDB *me)
{
   FREEIF(me->pcAlloc);

   me->pcAlloc = STRDUP(me->fdb.pcData + me->fdb.posRec);

   if ((char *)0 != me->pcAlloc) {
      
      me->pszName = ReadField(me->pcAlloc);
      me->pszURL  = ReadField(me->pszName);
      ReadField(me->pszURL);
      
      me->id      = ATOI(me->pcAlloc);

      return TRUE;
   }

   return FALSE;
}

// Advance to next bookmark record
//
static boolean BkmDB_NextEntry(BkmDB *me)
{
   if (FlatDB_NextRecord(&me->fdb)) {
      return BkmDB_ReadRecord(me);
   }

   return 0;
}

static boolean BkmDB_FindEntry(BkmDB *me, int id)
{
   while (BkmDB_NextEntry(me)) {
      if (me->id == id) {
         return 1;
      }
   }   
   return 0;
}

// Delete the current entry
//
static boolean BkmDB_DeleteEntry(BkmDB *me)
{
   return FlatDB_ReplaceRecord(&me->fdb, 0);
}


// replace record if id is valid, otherwise, add new record
//
// id = -1 => new record
//
static boolean BkmDB_AddOrReplaceEntry(BkmDB *me, int id, char *pszName, char *pszURL)
{
   int cbNew;
   char *pcNew;
   boolean bOK = FALSE;

   if (id < 0) {
      FlatDB_Rewind(&me->fdb);
      if (0 == me->fdb.cbData) {
         id = 1;
      } else {
         id = ATOI(me->fdb.pcData) + 1;
      }
   } else if (!BkmDB_FindEntry(me, id)) {
      // place at the start
      FlatDB_Rewind(&me->fdb);
   }

   cbNew = 20 + STRLEN(pszName) + STRLEN(pszURL);
   pcNew = MALLOC(cbNew);
   if ((char *)0 != pcNew) {
      SNPRINTF(pcNew, cbNew, "%d\001%s\001%s", id, pszName, pszURL);
   
      bOK = FlatDB_ReplaceRecord(&me->fdb, pcNew);

      FREE(pcNew);
   }

   return bOK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// Bookmarks

static int Bookmark_List(LCGIResp *me, BkmDB *pdb, 
                         AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr;
   char *pszUntitled = "???";
   char *pUntitledMem = 0;   // "no title" string

   nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                             IDX_BKMLIST, ppBlob, pnBlobLen);

   if (SUCCESS == nErr) {
      
      if (SUCCESS == IShell_LoadResSz(me->plcgie->piShell,BREWSER_RES_FILE,
                                      IDS_UNTITLED, &pUntitledMem)) {
         pszUntitled = pUntitledMem;
      }
      
      while (SUCCESS == nErr && BkmDB_NextEntry(pdb)) {
         char *pszName = pdb->pszName;

         if ('\0' ==  pszName[0]) {
            pszName = pszUntitled;
         }

         if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, "list%s\0",
            "<a href=lcgi:bkmk.{invoke}.action.{id}>{name}</a><br>{list}")) {
            nErr = ENOMEMORY;
            break;
         }

         if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, "id%d\0name%Qs\0",
                                    (long)pdb->id, pszName)) {
            nErr = ENOMEMORY;
            break;
         }
      }
      // clean up unused tokens
      AEEResBlob_StrRepl(ppBlob, pnBlobLen,"list%s\0","");
   }

   FREEIF(pUntitledMem);

   return nErr;
}


static int Bookmark_Action(LCGIResp *me, BkmDB *pdb, int id,
                           AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr = SUCCESS;

   if (!BkmDB_FindEntry(pdb, id)) {
      char szTo[128];
      
      SNPRINTF(szTo,sizeof(szTo),
               "brewser:back?to=lcgi:bkmk.%s.list&past=lcgi:bkmk.%s.list",
               me->szInvoke, me->szInvoke);
      LCGIResp_Header(me, "Location", szTo);

      me->wri.nCode = WEBCODE_MOVEDPERMANENTLY;

   } else {
      char *pURLData;
      char *pszFullURL;
      int nLen;      

      nLen = IWebUtil_GuessUrl(me->plcgie->piWebUtil,pdb->pszURL, 0, 0);
      pURLData = (char *)MALLOC(nLen);

      if ((char *)0 != pURLData) {
         IWebUtil_GuessUrl(me->plcgie->piWebUtil,pdb->pszURL,pURLData, nLen);
         pszFullURL = pURLData;
      } else {
         pszFullURL = pdb->pszURL;
      }

      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE,
                                IDX_BKMACTION, ppBlob, pnBlobLen);

      if (SUCCESS == nErr) {
         /* Template:
            <html>
            {title}<br>{url}<hr>
            <a href="{furl}">Go</a><br>
            <a href="lcgi:bkmk.{invoke}.edit.{id}">Edit</a><br>
            <a href="lcgi:bkmk.{invoke}.del.{id}.{title}">Delete</a>
            </html>
         */
         if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, 
                                    "title%Qs\0url%Qs\0furl%Qs\0id%d\0",
                                    pdb->pszName, pdb->pszURL, pszFullURL, id)) {
            nErr = ENOMEMORY;
         }
      }

      FREEIF(pURLData);
   }

   return nErr;
}

static int Bookmark_Edit(LCGIResp *me, BkmDB *pdb, int id, char *pszURLData, 
                         AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr = EFAILED;
   uint16 nIDRes = 0;

   char *pszName = "";
   char *pszURL = "";

   if (id < 0) {
      IWebUtil_GetFormFields(me->plcgie->piWebUtil, pszURLData, 
                             "title\0addr\0", &pszName, &pszURL);
      nErr = SUCCESS;
      nIDRes = IDX_BKMADD;
   } else if (BkmDB_FindEntry(pdb, id)) {
      pszName = pdb->pszName;
      pszURL = pdb->pszURL;
      nErr = SUCCESS;
      nIDRes = IDX_BKMEDIT;
   }

   if (SUCCESS == nErr) {

      nErr = IShell_LoadResBlob(me->plcgie->piShell,
                                BREWSER_RES_FILE,
                                nIDRes,
                                ppBlob, pnBlobLen);

      if (SUCCESS == nErr) {
         /* Template:
            <html>
            <h2 align=center>{addoredit} Bookmark</h2>
            <form action="lcgi:bkmk.{invoke}.edsub.{id}">
            <b>Title:</b><br>
            <input type=text name=title size=50 value="{title}">
            <b>Address:</b><br>
            <input type=text name=addr size=50 value="{url}"><br>
            <a href="x-submit:">Save</a>
            </form>
            </html>
         */
         if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, 
                                    "id%d\0title%Qs\0url%Qs\0",
                                    id, pszName, pszURL)) {
            nErr = ENOMEMORY;
         }
      }
   }

   return nErr;
}


static int Bookmark_EditSubmit(LCGIResp *me, BkmDB *pdb, int id, 
                               char *pszURLData, 
                               AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr = EFAILED;
   char *pUntitledMem = 0;
   char *pszUntitled = "???";
   char *pszName = "";
   char *pszURL = "";
   
   IWebUtil_GetFormFields(me->plcgie->piWebUtil, pszURLData, 
                          "title\0addr\0", &pszName, &pszURL);

   if (SUCCESS == 
       IShell_LoadResSz(me->plcgie->piShell, BREWSER_RES_FILE, IDS_UNTITLED,
                        &pUntitledMem)) {
      pszUntitled = pUntitledMem;
   }
   
   // strip whitespace from pszName
   trim(pszName,-1);
   
   // strip whitespace from pszURL
   trim(pszURL,-1);

   if (!STRLEN(pszName)) {
      pszName = pszUntitled;
   }

   if (BkmDB_AddOrReplaceEntry(pdb, id, pszName, pszURL)) {

      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                IDX_BKMSAVED, ppBlob, pnBlobLen);

      if (SUCCESS == nErr) {
         /* Template:
            <html>
            {title}<br>{url}<hr>
            Bookmark saved.<br>
            <a href="brewser:back?to=lcgi:bkmk.{invoke}.list&past=lcgi:bkmk.{invoke}">Continue</a>
            </html>
         */
         if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, 
                                    "title%Qs\0url%Qs\0", pszName, pszURL)) {
            nErr = ENOMEMORY;
         }
      }
   } else {
      nErr = EBADITEM;  // could not create record
   }

   FREEIF(pUntitledMem);
   return nErr;
}


static int Bookmark_Delete(LCGIResp *me, BkmDB *pdb, int id,
                           AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr = SUCCESS;
   
   if (!BkmDB_FindEntry(pdb, id)) {
      char szTo[128];
      
      SNPRINTF(szTo,sizeof(szTo),
               "brewser:back?to=lcgi:bkmk.%s.list&past=lcgi:bkmk.%s.list",
               me->szInvoke, me->szInvoke);
      LCGIResp_Header(me, "Location", szTo);

      me->wri.nCode = WEBCODE_MOVEDPERMANENTLY;

   } else {
   
      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                IDX_BKMDEL, ppBlob, pnBlobLen);

      if (SUCCESS == nErr) {
         /* Template:
            <html>
            <h2 align=center>Confirm Delete</h2><p>
            Delete bookmark "{title}"?<br>
            <a href="brewser:back?to=lcgi:bkmk.{invoke}.action.{id}&past=lcgi:bkmk.{invoke}.">Cancel</a><br>
            <a href="lcgi:bkmk.{invoke}.delx.{id}">Delete</a>
            </html>
          */
         if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, 
                                    "title%Qs\0id%d\0", pdb->pszName, id)) {
            nErr = ENOMEMORY;
         }
      }
   }
   return nErr;
}


static int Bookmark_DeleteDone(LCGIResp *me, BkmDB *pdb, int id,
                               AEEResBlob **ppBlob, int *pnBlobLen)
{
   int nErr = EFAILED;

   if (BkmDB_FindEntry(pdb, id)) {
   
      nErr = IShell_LoadResBlob(me->plcgie->piShell, BREWSER_RES_FILE, 
                                IDX_BKMDELX, ppBlob, pnBlobLen);

      if (SUCCESS == nErr) {
          /* Template:
            <html>
            Bookmark "{title}" Deleted.<br>
            <a href="brewser:back?to=lcgi:bkmk.{invoke}.list&past=lcgi:bkmk.{invoke}">Continue</a>
            </html>
         */
         if (0 < AEEResBlob_StrRepl(ppBlob, pnBlobLen, 
                                    "title%Qs\0", pdb->pszName)) {
            nErr = ENOMEMORY;
         } else if (!BkmDB_DeleteEntry(pdb)) {
            nErr = EFAILED;
         }
      }
   }
   return nErr;
}



int LCGIResp_Bookmarks(LCGIResp *me, UrlParts *pup)
{
   int         nErr = EFAILED;
   const char *cpszUrl;
   BkmDB       bkmdb;

   cpszUrl = (char *)MEMCHR(pup->cpcPath, '.', UP_PATHLEN(pup)) + 1;

   if (!LCGIResp_FindOrMakeInvoke(me,pup,&cpszUrl)) {
      return SUCCESS;
   }
   
   ZEROAT(&bkmdb);
   
   me->wri.nCode = WEBCODE_OK;

   if (BkmDB_Open(&bkmdb, me->plcgie->piShell)) {
      
      AEEResBlob *pBlob = 0;
      int nBlobLen = 0;

      // lcgi:bkmk.list
      if (STRBEGINS("list", cpszUrl)) {
         nErr = Bookmark_List(me, &bkmdb, &pBlob, &nBlobLen);
      }
      // lcgi:bkmk.edit.{id}
      else if (STRBEGINS("edit.", cpszUrl)) {
         int id = ATOI(cpszUrl + sizeof("edit.") - 1);
         
         char *pszURLData = 0;
         char *psz = UP_HASSRCH(pup) ? ((char*)pup->cpcSrch + 1) : "";

         nErr = ERR_STRDUP(psz, &pszURLData);
         if (SUCCESS == nErr) {
            nErr = Bookmark_Edit(me, &bkmdb, id, pszURLData, &pBlob, &nBlobLen);
         }
         FREEIF(pszURLData);
      }
      else if (LCGIResp_SafeReferer(me)) {
         // lcgi:bkmk.action.{id}
         if (STRBEGINS("action.", cpszUrl)) {
            int id = ATOI(cpszUrl + sizeof("action.") - 1);
            nErr = Bookmark_Action(me, &bkmdb, id, &pBlob, &nBlobLen);
         }
         // lcgi:bkmk.edsub.{id}
         else if (STRBEGINS("edsub.", cpszUrl)) {
            int id = ATOI(cpszUrl + sizeof("edsub.") - 1);
            char *pszURLData = 0;
            nErr = ERR_STRDUP(pup->cpcSrch + 1, &pszURLData);
            if (SUCCESS == nErr) {
               nErr = Bookmark_EditSubmit(me, &bkmdb, id, pszURLData, 
                                          &pBlob, &nBlobLen);
            }
            FREEIF(pszURLData);
         }
         // lcgi:bkmk.del.{id}
         else if (STRBEGINS("del.", cpszUrl)) {
            int id = ATOI(cpszUrl + sizeof("del.") - 1);
            nErr = Bookmark_Delete(me, &bkmdb, id, &pBlob, &nBlobLen);
         }
         // lcgi:bkmk.delx.{id}
         else if (STRBEGINS("delx.", cpszUrl)) {
            int id = ATOI(cpszUrl + sizeof("delx.") - 1);
            nErr = Bookmark_DeleteDone(me, &bkmdb, id, &pBlob, &nBlobLen);
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
         } else {
            FREE(pBlob);
         }
      }
   }

   BkmDB_Dtor(&bkmdb);

   return nErr;
}


