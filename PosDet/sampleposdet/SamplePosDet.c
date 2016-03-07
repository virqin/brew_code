/*======================================================
FILE:  SamplePosDet.c

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "SamplePosDet.bid"
#include "SamplePosdet.h"
/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static boolean SamplePosDet_ScreenHandler( CSamplePosDet *pMe, AEEEvent eCode,
                                          uint16 wParam, uint32 dwParam );
static boolean SamplePosDet_HandleEvent(IApplet * pi, AEEEvent eCode, 
                                      uint16 wParam, uint32 dwParam);
static boolean SamplePosDet_InitApplet( CSamplePosDet *pMe );
static boolean SamplePosDet_StartApplet( CSamplePosDet *pMe );

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
{
   *ppObj = NULL;
		
   if(ClsId == AEECLSID_SAMPLEPOSDET){
      if(AEEApplet_New(sizeof(CSamplePosDet), ClsId, pIShell,po,(IApplet**)ppObj,
         (AEEHANDLER)SamplePosDet_HandleEvent,NULL)
         == TRUE)
      {
		 // Add your code here .....

         return (AEE_SUCCESS);
      }
   }
	return (EFAILED);
}

/*===========================================================================
===========================================================================*/
static boolean SamplePosDet_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   CSamplePosDet *pMe = (CSamplePosDet *)pi;

   switch (eCode) {

   case EVT_APP_START:
      SamplePosDet_InitApplet( pMe );        
      /* Show the copyright and start the application after a delay seconds */
      ISHELL_ShowCopyright(pMe->theApp.m_pIShell);
      ISHELL_SetTimer(pMe->theApp.m_pIShell, 2000, (PFNNOTIFY) SamplePosDet_StartApplet, (uint32*) pMe);
      return TRUE;

   case EVT_APP_STOP:
      if( pMe->currentHandler ) {
         pMe->currentHandler( pMe, EVT_SCREEN, SCREEN_PARAM_CLOSE, 0 );
      }
      return TRUE;

   case EVT_APP_RESUME:
      SamplePosDet_DrawScreen( pMe, 0 );
      return TRUE;

   case EVT_APP_SUSPEND:
      return TRUE;

   case EVT_SCREEN :
      return SamplePosDet_ScreenHandler( pMe, eCode, wParam, dwParam );

   default:
      if( pMe->currentHandler ) {
         return pMe->currentHandler( pMe, eCode, wParam, dwParam );
      }
      break;
   }
   return FALSE;
}

#include "SP_Track.h"
/*===========================================================================
===========================================================================*/
static boolean SamplePosDet_InitApplet( CSamplePosDet *pMe )
{
   boolean bRet = TRUE;
   pMe->currentHandler = 0;

   // Load the GPS settings from the config file, if possible
   SamplePosDet_InitGPSSettings( pMe );

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
	   c1.lat = 22.543847;
	   c1.lon = 113.912316;
	   dis = Track_Calc_Distance(c1.lat, c1.lon, c2.lat, c2.lon);
	   
	   
	   MEMSET(szDis,0,sizeof(szDis));
	   
	   FLOATTOWSTR(dis, wcharbuf, 32);
	   WSTRTOSTR(wcharbuf,szDis, 64);
	   
	   DBGPRINTF("Track_cbOrientInfo dis:%s", szDis);
	   //SamplePosDet_Printf( pMe, 5, 4, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_RECT_FILL, "%s m", szDis );
	   
	}

   return bRet;
}

/*===========================================================================
===========================================================================*/
static boolean SamplePosDet_StartApplet( CSamplePosDet *pMe )
{
   pMe->appScreens[SCREENID_MAINMENU].theHandler     = (AEEHANDLER)SamplePosDet_MainMenu_HandleEvent;
   pMe->appScreens[SCREENID_GETGPSINFO].theHandler   = (AEEHANDLER)SamplePosDet_GetGPSInfo_HandleEvent;
   pMe->appScreens[SCREENID_CONFIG].theHandler       = (AEEHANDLER)SamplePosDet_ConfigMenu_HandleEvent;
   pMe->appScreens[SCREENID_PDE_SETTINGS].theHandler = (AEEHANDLER)SamplePosDet_PDESettings_HandleEvent;
   pMe->appScreens[SCREENID_PORT_SETTINGS].theHandler = (AEEHANDLER)SamplePosDet_PortSettings_HandleEvent;
   pMe->appScreens[SCREENID_OPTIM_SETTINGS].theHandler = (AEEHANDLER)SamplePosDet_OptimSettings_HandleEvent;
   pMe->appScreens[SCREENID_QOS_SETTINGS].theHandler = (AEEHANDLER)SamplePosDet_QosSettings_HandleEvent;
   pMe->appScreens[SCREENID_SERVER_SETTINGS].theHandler = (AEEHANDLER)SamplePosDet_ServerSettings_HandleEvent;
   /* Open the main menu and send a paint. */
   SamplePosDet_GotoScreen( pMe, SCREENID_MAINMENU, 0 );

   return TRUE;
}

/*===========================================================================
===========================================================================*/
typedef struct _GotoScreenParam {
   ScreenId       id;
   AEEHANDLER     pfn;
   uint32         dwParam;
} GotoScreenParam;
static boolean SamplePosDet_ScreenHandler( CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{
   if( wParam == SCREEN_PARAM_GOTO ) {
      if( pMe->currentHandler ) {
         pMe->currentHandler( pMe, EVT_SCREEN, SCREEN_PARAM_CLOSE, 0 );
      }
      
      if( dwParam ) {
         GotoScreenParam *pParam = (GotoScreenParam *)dwParam;
         pMe->currentHandler = (AEEHANDLER)pParam->pfn;
         if( pMe->currentHandler( pMe, EVT_SCREEN, SCREEN_PARAM_INIT, pParam->dwParam ) == FALSE ) {
            pMe->currentHandler = 0;
         }
         FREE( pParam );
         return TRUE;
      }
   }
   else {
      if( pMe->currentHandler ) {
         return pMe->currentHandler( pMe, eCode, wParam, dwParam );
      }
   }

   return FALSE;
}

/*===========================================================================
===========================================================================*/
boolean SamplePosDet_GotoScreen( CSamplePosDet *pMe, ScreenId id, uint32 dwParam )
{
   GotoScreenParam *pParam = (GotoScreenParam *)MALLOC( sizeof(GotoScreenParam) );
   if( pParam ) {
      pParam->id = id;
      pParam->pfn = pMe->appScreens[id].theHandler;
      pParam->dwParam = dwParam;
      ISHELL_PostEvent( pMe->theApp.m_pIShell, AEECLSID_SAMPLEPOSDET, EVT_SCREEN,
         SCREEN_PARAM_GOTO, (uint32)pParam );
      return TRUE;
   }
   return FALSE;
}

/*===========================================================================
===========================================================================*/
void *SamplePosDet_SetScreenData( CSamplePosDet *pMe, void *screenData )
{
   void *oldScreenData = pMe->pScreenData;
   pMe->pScreenData    = screenData;
   return oldScreenData;
}

/*===========================================================================
===========================================================================*/
void *SamplePosDet_GetScreenData( CSamplePosDet *pMe )
{
   return pMe->pScreenData;
}

/*===========================================================================
===========================================================================*/
void SamplePosDet_DrawScreen( CSamplePosDet *pMe, uint32 dwParam )
{
   SamplePosDet_HandleEvent( (IApplet *)pMe, EVT_SCREEN, SCREEN_PARAM_PAINT, dwParam );
}



/*===========================================================================
   HELPER ROUTINES FOR DRAWING.
===========================================================================*/
#define LINEHEIGHT   16   // hack
#define TOPLINE      0
#define ARRAYSIZE(a)   (sizeof(a) / sizeof((a)[0]))
#define min(a,b)  ((a) < (b) ? (a) : (b))

/*===========================================================================
===========================================================================*/
static void xDrawTextA(IDisplay * pd,AEEFont fnt, const char * pszText, int nChars,
                       int x,int y,const AEERect * prcBackground,uint32 dwFlags)
{
   AECHAR wcBuf[80];

   if (nChars < 0)
      nChars = STRLEN(pszText);
   nChars = min(nChars, ARRAYSIZE(wcBuf));

   STR_TO_WSTR((char*)pszText, wcBuf, sizeof(wcBuf));

   IDISPLAY_DrawText(pd, fnt, wcBuf, nChars, x, y, prcBackground, dwFlags);
}

/*===========================================================================
===========================================================================*/
static void xDisplay( AEEApplet *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, const char *psz)
{
	AEEDeviceInfo di;
	AEERect rc;
	int nMaxLines;

	ISHELL_GetDeviceInfo(pMe->m_pIShell,&di);
	nMaxLines = (di.cyScreen / LINEHEIGHT) - 2;
	if (nMaxLines < 1)
		nMaxLines = 1;
   
	rc.x = nCol;
	rc.dx = di.cxScreen - nCol;

	rc.y = nLine * LINEHEIGHT;
   if( dwFlags & IDF_ALIGNVERT_MASK ) {
	   rc.dy = di.cyScreen - rc.y;
   }
   else {
      rc.dy = LINEHEIGHT;
   }

	xDrawTextA(pMe->m_pIDisplay,
            fnt,
				psz, -1, rc.x, rc.y, &rc, dwFlags);

	IDISPLAY_Update(pMe->m_pIDisplay);
}


/*===========================================================================
===========================================================================*/
void SamplePosDet_Printf( CSamplePosDet *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, 
                                const char *szFormat, ... )
{
   char              szBuf[64];
   va_list args;
   va_start( args, szFormat );

   (void)VSNPRINTF( szBuf, 64, szFormat, args );

   va_end( args );
   xDisplay( (AEEApplet *)pMe, nLine, nCol, fnt, dwFlags, szBuf );
}
