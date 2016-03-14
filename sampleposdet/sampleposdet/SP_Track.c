/*======================================================
FILE:  SP_Track.c

SERVICES: Tracking using IPosDet.
  Track_Init
  Track_Start
  Track_Stop

GENERAL DESCRIPTION:
	Sample code to demonstrate services of IPosDet. See SP_Track.h for the
   description of exported functions.

        Copyright ?2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEComdef.h"
#include "BREWVersion.h"
#include "AEEStdLib.h"
#include "SP_Track.h"
#include "transform.h"

struct _TrackState{
   boolean     bInNotification;     /* When the state machine is notifying the client */
   boolean     bSetForCancellation; /* Track is meant to be cancelled. do so when it is safe. */
   boolean     bInProgress;         /* when tracking is in progress. */
/**************************/

/* For TRACK_AUTO */
   boolean     bModeAuto;
   boolean     bModeLocal;

/* Private members to work with IPosDet */
   AEECallback cbIntervalTimer;
   AEECallback cbInfo;
   AEECallback cbOrientInfo;
   AEEGPSInfo theInfo;
   AEEPositionInfoEx posInfo;
   AEEOrientationAzimuthInfo orientInfo;
/**************************/

/* Clients response members. */
   AEECallback *pcbResp;
   PositionData *pResp;
/**************************/

/* Client passed members. */
   int nPendingFixes;
   int nTrackInterval;
   IPosDet    *pPos;
   IShell     *pShell;
/**************************/
};


static void Track_Notify( TrackState *pts )
{
   pts->bInNotification = TRUE;
   pts->pcbResp->pfnNotify( pts->pcbResp->pNotifyData );
   pts->bInNotification = FALSE;
}

static void Track_Cancel( AEECallback *pcb )
{
   TrackState *pts = (TrackState *)pcb->pCancelData;

   if( TRUE == pts->bInNotification ) {
      /* It is not safe to cleanup from a notification. Defer it. */
      pts->bSetForCancellation = TRUE;
      return;
   }

   /* Kill any ongoing process */
   CALLBACK_Cancel( &pts->cbOrientInfo );
   CALLBACK_Cancel( &pts->cbInfo );
   CALLBACK_Cancel( &pts->cbIntervalTimer );
   
   pts->pcbResp->pfnCancel = 0;
   pts->pcbResp->pCancelData = 0;

   IPOSDET_Release( pts->pPos );
   ISHELL_Release( pts->pShell );

   FREE( pts );
}

static void Track_cbInterval( TrackState *pts )
{
   /* Cancel if it was deferred. */
   if( TRUE == pts->bSetForCancellation ) {

      Track_Cancel( pts->pcbResp );
      return;
   }

   DBGPRINTF( "TRACK : bAuto:%d bLocal:%d", pts->bModeAuto, pts->bModeLocal );

   // Request GPSInfo
   if( TRUE == pts->bInProgress && SUCCESS != IPOSDET_GetGPSInfo( pts->pPos, 
      AEEGPS_GETINFO_LOCATION|AEEGPS_GETINFO_ALTITUDE, AEEGPS_ACCURACY_LEVEL1, 
      &pts->theInfo, &pts->cbInfo ) ) {

	  DBGPRINTF( "@IPOSDET_GetGPSInfo Failed!");
	  
      /* Report a failure and bailout */
      pts->pResp->nErr = AEEGPS_ERR_GENERAL_FAILURE;

      Track_Notify( pts );

      Track_Stop( pts );

   }

   // Request Orientation
   //if( TRUE == pts->bInProgress && SUCCESS != IPOSDET_GetOrientation( pts->pPos, 
   //  AEEORIENTATION_REQ_AZIMUTH, &pts->orientInfo, &pts->cbOrientInfo ) ) {

   //	  DBGPRINTF( "IPOSDET_GetOrientation Failed!");
	  
      /* Report a failure and bailout */
      //pts->pResp->nErr = AEEGPS_ERR_GENERAL_FAILURE;

      //Track_Notify( pts );

      //Track_Stop( pts );

   //}
}

static void Track_Network( TrackState *pts )
{
   AEEGPSConfig config;

   DBGPRINTF( "TRACK NETWORK" );
   
   (void) IPOSDET_GetGPSConfig( pts->pPos, &config );

   config.mode = AEEGPS_MODE_TRACK_NETWORK;

   (void) IPOSDET_SetGPSConfig( pts->pPos, &config );

   pts->bModeLocal = FALSE;
}

static void Track_Local( TrackState *pts )
{
   AEEGPSConfig config;

   DBGPRINTF( "TRACK LOCAL" );

   (void) IPOSDET_GetGPSConfig( pts->pPos, &config );

   config.mode = AEEGPS_MODE_TRACK_LOCAL;

   (void) IPOSDET_SetGPSConfig( pts->pPos, &config );

   pts->bModeLocal = TRUE;
}

static void Track_cbInfo( TrackState *pts )
{
   if( pts->theInfo.status == AEEGPS_ERR_NO_ERR 
      || (pts->theInfo.status == AEEGPS_ERR_INFO_UNAVAIL && pts->theInfo.fValid) ) {

#if MIN_BREW_VERSION(2,1)
      pts->pResp->lat = WGS84_TO_DEGREES( pts->theInfo.dwLat );
      pts->pResp->lon = WGS84_TO_DEGREES( pts->theInfo.dwLon );
#else
      double    wgsFactor;
      wgsFactor = FASSIGN_STR("186413.5111");
      pts->pResp->lat = FASSIGN_INT(pts->theInfo.dwLat);
      pts->pResp->lat = FDIV(pts->pResp->lat, wgsFactor);

      pts->pResp->lon = FASSIGN_INT(pts->theInfo.dwLon);
      pts->pResp->lon = FDIV(pts->pResp->lon, wgsFactor);
#endif /* MIN_BREW_VERSION 2.1 */

    pts->pResp->height = pts->theInfo.wAltitude - 500;
	  pts->pResp->wAzimuth = pts->orientInfo.wAzimuth;
	  pts->pResp->heading = pts->theInfo.wHeading;
	  pts->pResp->velocityHor = FMUL( pts->theInfo.wVelocityHor,0.25);

      pts->pResp->dwFixNum++;

      pts->pResp->nErr = SUCCESS;

      Track_Notify( pts );

      if( (!pts->nPendingFixes || --pts->nPendingFixes > 0) 
         && FALSE == pts->bSetForCancellation ) {

         if( pts->bModeAuto && pts->bModeLocal == FALSE ) {

            /* Try with local first */
            Track_Local( pts );
         }

         ISHELL_SetTimerEx( pts->pShell, pts->nTrackInterval * 1000, &pts->cbIntervalTimer );
      }
      else {

         Track_Stop( pts );
      }
   }
   else {

      if( pts->bModeAuto && pts->bModeLocal ) {

         /* Retry with TRACK_NETWORK */
         Track_Network( pts );
         Track_cbInterval( pts );
      }

      else { 
         
		  DBGPRINTF("@Track_cbInfo status:%u", pts->theInfo.status);

         /* Inform the application of failure code. */
         pts->pResp->nErr = pts->theInfo.status;
         
         Track_Notify( pts );
         
         /* On timeout re-try. For other reasons bailout. */
         if( pts->theInfo.status == AEEGPS_ERR_TIMEOUT ) {
            
            Track_cbInterval( pts );
         }
         else {
			 
            Track_Stop( pts );
         }
      }
   }
}

static void Track_cbOrientInfo( TrackState *pts )
{
	DBGPRINTF("Track_cbOrientInfo Degrees:%d.%d", ((uint16)pts->orientInfo.wAzimuth&(~0x3f))>>6, (pts->orientInfo.wAzimuth&0x3f));

#if 0
	Coordinate c1, c2;
	double dis = 0;
	char szDis[64] = {0};
	AECHAR wcharbuf[32] = {0};	

	//shanghai 31.1774276, 121.5272106
	c1.lat = 31.1774276;
	c1.lon = 121.5272106;
	
	//beijing 39.911954, 116.377817
	c2.lat = 39.911954;
	c2.lon = 116.377817;

	//shenzhen 22.543847, 113.912316
	//c1.lat = 22.543847;
	//c1.lon = 113.912316;
	dis = Track_Calc_Distance(c1, c2);

	
	MEMSET(szDis,0,sizeof(szDis));

	FLOATTOWSTR(dis, wcharbuf, 32);
	WSTRTOSTR(wcharbuf,szDis, 64);

	DBGPRINTF("Track_cbOrientInfo dis:%s", szDis);
#endif
}

/*======================================================================= 
Function: Track_Init()

Description: 
   Creates and initializes a handle for tracking.

Prototype:

   int Track_Init( IShell *pIShell, IPosDet *pIPos, AEECallback *pcb, TrackState **po )

Parameters:
   pIShell: [in]. IShell instance.
   pIPos: [in]. IPosDet instance.
   pcb: [in]. A persistent callback invoked on a position response.
   po: [out]. Handle to internal object.

Return Value:

   SUCCESS
   EBADPARM - One or more of the Invalid arguments
   ENOMEMORY - When system is out of memory.
 
Comments:  
   Invoke the CALLBACK_Cancel( pcb ) to destroy TrackState object.

Side Effects: 
   None

See Also:
   None
=======================================================================*/
int Track_Init( IShell *pIShell, IPosDet *pIPos, AEECallback *pcb, TrackState **po )
{
   int nErr = SUCCESS;
   TrackState *pts = NULL;

   if( !pIShell || !pIPos || !pcb || !po ) {

      nErr = EBADPARM;

   }
   else if( NULL == (pts = MALLOC( sizeof(TrackState) )) ){

      nErr = ENOMEMORY;

   }
   else {

      ZEROAT( pts );

      pts->pShell = pIShell;
      ISHELL_AddRef( pIShell );

      pts->pPos = pIPos;
      IPOSDET_AddRef( pIPos );

      /* Install the notification cb */
      CALLBACK_Cancel( pcb );
      pts->pcbResp = pcb;
      pts->pcbResp->pfnCancel   = Track_Cancel;
      pts->pcbResp->pCancelData = pts;

      CALLBACK_Init( &pts->cbIntervalTimer, Track_cbInterval, pts );
      CALLBACK_Init( &pts->cbInfo, Track_cbInfo, pts );
	  CALLBACK_Init( &pts->cbOrientInfo, Track_cbOrientInfo, pts);
   }

   *po = pts;
   return nErr;

}

/*======================================================================= 
Function: Track_Stop()

Description: 
   Stops the tracking, does not clean up the object, it can be
   further used with Track_Start. Only CALLBACK_Cancel(pcb) releases
   the object.

Prototype:

   int Track_Stop( TrackState *pts );

Parameters:
   pts: [in]. TrackState object created using Track_Init().

Return Value:

   SUCCESS
   EBADPARM - One or more of the Invalid arguments
 
Comments:  
   Invoke the CALLBACK_Cancel( pcb ) to destroy TrackState object.

Side Effects: 
   None

See Also:
   Track_Init()
=======================================================================*/
int Track_Stop( TrackState *pts )
{
   if( !pts ) {
      return EBADPARM;
   }

   DBGPRINTF("@Track_Stop");

   pts->bInProgress = FALSE;

   /* Kill any ongoing process */
   CALLBACK_Cancel( &pts->cbOrientInfo);
   CALLBACK_Cancel( &pts->cbInfo );
   CALLBACK_Cancel( &pts->cbIntervalTimer );

   /* Report that Tracking is halted */
   pts->pResp->nErr = EIDLE;
   
   Track_Notify( pts );

   if( TRUE == pts->bSetForCancellation ) {

      Track_Cancel( pts->pcbResp );
   }
   
   return SUCCESS;
}

/*======================================================================= 
Function: Track_Start()

Description: 
   Starts the tracking using the object created in Track_Init().

Prototype:

   int Track_Start( TrackState *pts, TrackType t, int nFixes, int nInterval, PositionData *pData );

Parameters:
   pts: [in]. TrackState object created using Track_Init().
   t: [in]. Type of tracking.
   nFixes: [in]. Number of fixes.
   nInterval: [in]. Interval between fixes in seconds.
   pData: [in]. Memory in which the Position response is to be filled.

Return Value:

   SUCCESS
   EBADPARM - One or more of the Invalid arguments
   EUNSUPPORTED - Unimplemented
   ENOMEMORY - When system is out of memory.
   EFAILED - General failure.
   EALREADY - When tracking is already in progress.
 
Comments:  
   Invoke the CALLBACK_Cancel( pcb ) to destroy TrackState object.

Side Effects: 
   None

See Also:
   Track_Init()
=======================================================================*/
int Track_Start( TrackState *pts, TrackType t, int nFixes, 
                   int nInterval, PositionData *pData )
{
   int nErr = SUCCESS;

   if( !pts || !pData ) {

      nErr = EBADPARM;
   }
   else if( TRUE == pts->bInProgress ) {

      nErr = EALREADY;
   }
   else {

      AEEGPSConfig config;

	  pts->posInfo.dwSize = sizeof(AEEPositionInfoEx);
	  pts->orientInfo.wSize = sizeof(AEEOrientationAzimuthInfo);

      pData->dwFixNum     = 0;

      pts->pResp          = pData;
      pts->nPendingFixes  = nFixes;
      pts->nTrackInterval = nInterval;
      pts->bModeAuto      = FALSE;
      pts->bModeLocal     = FALSE;

      IPOSDET_GetGPSConfig( pts->pPos, &config );

      /* Configure the IPosDet Instance */
      if( t == TRACK_LOCAL ) {

         config.mode = AEEGPS_MODE_TRACK_LOCAL;
         pts->bModeLocal = TRUE;
      }
      else if( t == TRACK_NETWORK ){

         config.mode = AEEGPS_MODE_TRACK_NETWORK;
      }
      else if( t == TRACK_AUTO ) {

         DBGPRINTF( "TRACK AUTO" );

         if( nFixes == 1 ) {
           
            config.mode = AEEGPS_MODE_ONE_SHOT;
         }
         else {

            config.mode = AEEGPS_MODE_TRACK_LOCAL;
            pts->bModeAuto  = TRUE;
            pts->bModeLocal = TRUE;
         }
      }

      config.nFixes = nFixes;
      config.nInterval = nInterval;

      // ADDING SUPPORT FOR USER-DEFINED PDE IP ADDRESS AND QOS
      config.server = pData->server;
      config.optim = pData->optim;
      config.qos = pData->qos;

      nErr = IPOSDET_SetGPSConfig( pts->pPos, &config );

      if( nErr == EUNSUPPORTED && pts->bModeAuto ) {

         /* As TRACK_LOCAL is unsupported on certain devices. If this is auto mode 
         ** and we tried to track locally, change it network based tracking. */

         pts->bModeAuto = FALSE;
         pts->bModeLocal = FALSE;

         config.mode = AEEGPS_MODE_TRACK_NETWORK;

         nErr = IPOSDET_SetGPSConfig( pts->pPos, &config );
      }

      if( nErr == SUCCESS ) {

         pts->bInProgress    = TRUE;

         Track_cbInterval( pts );
      }

   }
   return nErr;
}

/* Calculate the distance between A and B */
double Track_Calc_Distance( double latA, double lngA, double latB, double lngB )
{
	return distance(latA, lngA, latB, lngB);
}
