/*======================================================
FILE:  SP_GetGPSInfo.c

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "SamplePosDet.h"
#include "SP_Track.h"

/*===========================================================================
                G E T G P S I N F O   S C R E E N
===========================================================================*/
typedef enum {
   GETGPSINFO_PAINT_ALL = 0,
   GETGPSINFO_PAINT_FIXCOUNT,
   GETGPSINFO_PAINT_FIXDATA,
   GETGPSINFO_PAINT_FIXANIM,
   GETGPSINFO_PAINT_ERROR
} GetGPSInfo_PaintRegions;

struct _GetGPSInfo {
   PositionData theInfo;
   IPosDet      *pPosDet;
   AEECallback  cbPosDet;
   AEECallback  cbProgressTimer;
   uint32       dwFixNumber;
   uint32       dwFixDuration;
   uint32       dwFail;
   uint32       dwTimeout;
   uint16       wProgress;
   boolean      bPaused;
   uint16       wMainMenuEntry;
   boolean      bAbort;
   TrackState   *pts;
};

/*===========================================================================
===========================================================================*/
static void SamplePosDet_GetGPSInfo_Callback( CSamplePosDet *pMe )
{
   struct _GetGPSInfo *pGetGPSInfo = SamplePosDet_GetScreenData( pMe );
   GetGPSInfo_PaintRegions rgn = GETGPSINFO_PAINT_FIXDATA;

   if( pGetGPSInfo->theInfo.nErr == SUCCESS ) {
      /* Process new data from IPosDet */
      pGetGPSInfo->dwFixNumber++;
      pGetGPSInfo->dwFixDuration += pGetGPSInfo->wProgress;
      pGetGPSInfo->wProgress = 0;
   }
   else if( pGetGPSInfo->theInfo.nErr == EIDLE ) {
      /* End of tracking */
      CALLBACK_Cancel( &pGetGPSInfo->cbProgressTimer );
   }
   else if( pGetGPSInfo->theInfo.nErr == AEEGPS_ERR_TIMEOUT ) {
      /* Record the timeout and perhaps re-try. */
      pGetGPSInfo->dwTimeout++;
      rgn = GETGPSINFO_PAINT_FIXCOUNT;
   }
   else {
      /* Something is not right here. Requires corrective action. Bailout */
      pGetGPSInfo->bAbort = TRUE;
      rgn = GETGPSINFO_PAINT_ERROR;
   }

   SamplePosDet_DrawScreen( pMe, (uint32)rgn );
}

/*===========================================================================
===========================================================================*/
static void SamplePosDet_GetGPSInfo_SecondTicker( CSamplePosDet *pMe )
{
   struct _GetGPSInfo *pGetGPSInfo = SamplePosDet_GetScreenData( pMe );


   if( pGetGPSInfo->bPaused == FALSE ) {
      pGetGPSInfo->wProgress++;
      SamplePosDet_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_FIXANIM );
   }
   if( pGetGPSInfo->bAbort == FALSE ) {
      ISHELL_SetTimerEx( pMe->theApp.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
   }
}

/*===========================================================================
===========================================================================*/
static void SamplePosDet_GetGPSInfo_Paint( CSamplePosDet *pMe, GetGPSInfo_PaintRegions rgn )
{
   struct _GetGPSInfo *pGetGPSInfo = SamplePosDet_GetScreenData( pMe );

  
   if( rgn == GETGPSINFO_PAINT_ALL ) {
      IDISPLAY_ClearScreen( pMe->theApp.m_pIDisplay );
      SamplePosDet_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "GetGPSInfo" );
      if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_ONE_SHOT ) {
         SamplePosDet_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "One Shot" );
      }
      else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_TRACK_LOCAL ) {
         SamplePosDet_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "Track L" );
      }
      else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_TRACK_NETWORK ) {
         SamplePosDet_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "Track N" );
      }
      else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_TRACK_AUTO ) {
         SamplePosDet_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "Track A" );
      }
   }

   if( rgn == GETGPSINFO_PAINT_FIXCOUNT || rgn == GETGPSINFO_PAINT_ALL ) {
      SamplePosDet_Printf( pMe, 1, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Failed : %d", pGetGPSInfo->dwFail+pGetGPSInfo->dwTimeout );
      SamplePosDet_Printf( pMe, 1, 4, AEE_FONT_NORMAL, IDF_ALIGN_RIGHT, "Timeout : %d", pGetGPSInfo->dwTimeout );
   }

   if( rgn == GETGPSINFO_PAINT_FIXDATA || rgn == GETGPSINFO_PAINT_ALL ) {
#define MAXTEXTLEN   22
      AECHAR wcText[MAXTEXTLEN];
      char   latlonStr[MAXTEXTLEN];

      SamplePosDet_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Fixes : %d", pGetGPSInfo->dwFixNumber );

      FLOATTOWSTR( pGetGPSInfo->theInfo.lat, wcText, MAXTEXTLEN * sizeof(AECHAR) );
      WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
      SamplePosDet_Printf( pMe, 3, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s d", latlonStr );

      FLOATTOWSTR( pGetGPSInfo->theInfo.lon, wcText, MAXTEXTLEN * sizeof(AECHAR) );
      WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
      SamplePosDet_Printf( pMe, 4, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s d", latlonStr );

	  		  
      SamplePosDet_Printf( pMe, 5, 4, AEE_FONT_BOLD, IDF_ALIGN_RIGHT|IDF_RECT_FILL, "%d m", pGetGPSInfo->theInfo.height );

	  
      {
         double fv;
         fv = FASSIGN_INT( pGetGPSInfo->dwFixDuration );
         if( pGetGPSInfo->dwFixNumber ) {
            fv = FDIV(fv, pGetGPSInfo->dwFixNumber);
         }
         FLOATTOWSTR( fv, wcText, MAXTEXTLEN * sizeof(AECHAR) );
         WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
         SamplePosDet_Printf( pMe, 6, 4, AEE_FONT_BOLD, IDF_ALIGN_RIGHT|IDF_RECT_FILL, "Avg %ss", latlonStr );   
      }
   }

   if( rgn == GETGPSINFO_PAINT_ERROR ) {
      SamplePosDet_Printf( pMe, 7, 4, AEE_FONT_BOLD, IDF_ALIGN_BOTTOM|IDF_ALIGN_CENTER|IDF_TEXT_INVERTED, 
         "ABORTED 0x%x", pGetGPSInfo->theInfo.nErr );
   }

   if( rgn == GETGPSINFO_PAINT_FIXANIM || rgn == GETGPSINFO_PAINT_ALL ) {
      SamplePosDet_Printf( pMe, 7, 4, AEE_FONT_NORMAL, IDF_ALIGN_BOTTOM|IDF_ALIGN_CENTER|IDF_RECT_FILL, 
         ".....%d.....", pGetGPSInfo->wProgress );
   }

   if (FABS(pGetGPSInfo->theInfo.lat) > 0)
   {
	   Coordinate c1, c2;
	   double dis = 0;
	   char szDis[64];
	   AECHAR wcharbuf[32];
	   
	   //shanghai 31.1774276, 121.5272106
	   c1.lat = 31.1774276;
	   c1.lon = 121.5272106;
	   
	   //beijing 39.911954, 116.377817
	   c2.lat = 39.911954;
	   c2.lon = 116.377817;
	   
	   //shenzhen 22.543847, 113.912316
	   //c1.lat = 22.543847;
	   //c1.lon = 113.912316;
	   c1.lat = pGetGPSInfo->theInfo.lat;
	   c1.lon = pGetGPSInfo->theInfo.lon;

	   dis = Track_Calc_Distance(c1.lat, c1.lon, c2.lat, c2.lon);
	   
	   
	   MEMSET(szDis,0,sizeof(szDis));
	   
	   FLOATTOWSTR(dis, wcharbuf, 32);
	   WSTRTOSTR(wcharbuf,szDis, 64);
	   
	   DBGPRINTF("Track_cbOrientInfo dis:%s", szDis);
	   SamplePosDet_Printf( pMe, 7, 4, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_RECT_FILL, "%s m", szDis );
	   
	  }
	  {
		  uint16 d1 = 0;
		  uint8 d2 = 0;
		  d1 = ((uint16)(pGetGPSInfo->theInfo.wAzimuth & (~0x3f)))>>6;
		  d2 = (uint8)(pGetGPSInfo->theInfo.wAzimuth & 0x3f);
		  SamplePosDet_Printf( pMe, 8, 4, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_RECT_FILL, "Head %d.%d", d1, d2 );   
	  }

}

/*===========================================================================
===========================================================================*/
boolean SamplePosDet_GetGPSInfo_HandleEvent( CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{
   boolean bHandled = FALSE;
   struct _GetGPSInfo *pGetGPSInfo = SamplePosDet_GetScreenData( pMe );

   switch( eCode ) {

   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) {
         if( pGetGPSInfo == 0 ) {

            pGetGPSInfo = MALLOC( sizeof( struct _GetGPSInfo ) );

            if( pGetGPSInfo ) {
               int nErr = SUCCESS;
               ZEROAT( pGetGPSInfo );

               pGetGPSInfo->theInfo.server = pMe->gpsSettings.server;
               pGetGPSInfo->theInfo.qos = pMe->gpsSettings.qos;
               pGetGPSInfo->theInfo.optim = pMe->gpsSettings.optim;

               pGetGPSInfo->wMainMenuEntry = (uint16)dwParam;
               SamplePosDet_SetScreenData( pMe, (void *)pGetGPSInfo );

               if( ISHELL_CreateInstance( pMe->theApp.m_pIShell, AEECLSID_POSDET,
                  (void **)&pGetGPSInfo->pPosDet ) == SUCCESS ) {

                  CALLBACK_Init( &pGetGPSInfo->cbPosDet, SamplePosDet_GetGPSInfo_Callback, pMe );
                  CALLBACK_Init( &pGetGPSInfo->cbProgressTimer, SamplePosDet_GetGPSInfo_SecondTicker, pMe );

                  nErr = Track_Init( pMe->theApp.m_pIShell, pGetGPSInfo->pPosDet, 
                                        &pGetGPSInfo->cbPosDet, &pGetGPSInfo->pts );
                  if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_ONE_SHOT ) {
                     nErr = Track_Start( pGetGPSInfo->pts, TRACK_NETWORK, 1, 0, &pGetGPSInfo->theInfo );
                  }
                  else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_TRACK_LOCAL ) {
                     nErr = Track_Start( pGetGPSInfo->pts, TRACK_LOCAL, 0, 0, &pGetGPSInfo->theInfo );
                  }
                  else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_TRACK_NETWORK ) {
                     nErr = Track_Start( pGetGPSInfo->pts, TRACK_NETWORK, 0, 0, &pGetGPSInfo->theInfo );
                  }
                  else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_TRACK_AUTO ) {
                     nErr = Track_Start( pGetGPSInfo->pts, TRACK_AUTO, 0, 0, &pGetGPSInfo->theInfo );
                  }
                  else {
                     nErr = EFAILED;
                  }
 
                  SamplePosDet_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
                  if( nErr != SUCCESS ) {
                     pGetGPSInfo->theInfo.nErr = nErr;
                     SamplePosDet_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ERROR );
                  }
                  else {
                     ISHELL_SetTimerEx( pMe->theApp.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
                  }

                  bHandled = TRUE;
               }
            }
         }
         if( !bHandled ) {
            SamplePosDet_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
            bHandled = TRUE;
         }
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) {
         if( pGetGPSInfo ) {
            if( pGetGPSInfo->pPosDet ) {
               CALLBACK_Cancel( &pGetGPSInfo->cbProgressTimer );
               CALLBACK_Cancel( &pGetGPSInfo->cbPosDet );
               IPOSDET_Release( pGetGPSInfo->pPosDet );
            }
            FREE( pGetGPSInfo );
         }
         SamplePosDet_SetScreenData( pMe, 0 );
         pMe->currentHandler = 0;
         bHandled = TRUE;
      }
      else if( wParam == SCREEN_PARAM_PAINT ) {
         if( !pGetGPSInfo->bPaused ) {
            SamplePosDet_GetGPSInfo_Paint( pMe, (GetGPSInfo_PaintRegions)dwParam );
         }
         bHandled = TRUE;
      }
      break;

   case EVT_KEY:
      if( wParam == AVK_CLR ) {
         SamplePosDet_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
         bHandled = TRUE;
      }
      break;

   case EVT_DIALOG_START:
      pGetGPSInfo->bPaused = TRUE;
      bHandled = TRUE;
      break;

   case EVT_DIALOG_END:
      pGetGPSInfo->bPaused = FALSE;
      SamplePosDet_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
      bHandled = TRUE;
      break;

   case EVT_COMMAND:
      break;
   }

   return bHandled;
}

