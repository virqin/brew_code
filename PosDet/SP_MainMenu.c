/*======================================================
FILE:  SP_MainMenu.c

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "SamplePosDet.h"
#include "SamplePosDet.brh"
#include "AEEMenu.h"

/*===========================================================================
                   M A I N   M E N U
===========================================================================*/
boolean SamplePosDet_MainMenu_HandleEvent( CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{
   boolean bHandled = FALSE;
   IMenuCtl *pMenuCtl = SamplePosDet_GetScreenData( pMe );

   switch( eCode ) {
   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) {
         if( pMenuCtl ) {  /* This is not expected to happen */
            SamplePosDet_DrawScreen( pMe, 0 );
            bHandled = TRUE;
         }
         else {
            if( ISHELL_CreateInstance( pMe->theApp.m_pIShell, AEECLSID_MENUCTL, 
               (void **)&pMenuCtl ) == SUCCESS ) {
               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_MODE_ONE_SHOT,
                  MAINMENU_ITEM_ONE_SHOT, NULL, (uint32)pMenuCtl );
               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_MODE_TRACK_LOCAL,
                  MAINMENU_ITEM_TRACK_LOCAL, NULL, (uint32)pMenuCtl );
               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_MODE_TRACK_NETWORK,
                  MAINMENU_ITEM_TRACK_NETWORK, NULL, (uint32)pMenuCtl );
               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_MODE_TRACK_SMART,
                  MAINMENU_ITEM_TRACK_AUTO, NULL, (uint32)pMenuCtl );
               // 10/01/04: Adding configuration menu
               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_CONFIG,
                  MAINMENU_ITEM_CONFIG, NULL, (uint32)pMenuCtl );

               IMENUCTL_SetTitle( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_MAIN_TITLE, NULL );
               SamplePosDet_SetScreenData( pMe, (void *)pMenuCtl );
               SamplePosDet_DrawScreen( pMe, 0 );
               bHandled = TRUE;
            }
         }
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) {
         IMENUCTL_Release( pMenuCtl );
         SamplePosDet_SetScreenData( pMe, 0 );
         pMe->currentHandler = 0;
         bHandled = TRUE;
      }
      else if( wParam == SCREEN_PARAM_PAINT ) {
         if( IMENUCTL_IsActive( pMenuCtl ) ) {
            IMENUCTL_Redraw( pMenuCtl );
         }
         else {
            IMENUCTL_SetActive( pMenuCtl, TRUE );  /* Also causes a menu draw */
         }
      }
      break;

   case EVT_KEY:
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
      bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
      break;

   case EVT_COMMAND:
      if( dwParam == (uint32)pMenuCtl ) {
         switch( wParam ) {
         case MAINMENU_ITEM_ONE_SHOT:
         case MAINMENU_ITEM_TRACK_LOCAL:
         case MAINMENU_ITEM_TRACK_NETWORK:
         case MAINMENU_ITEM_TRACK_AUTO:
            SamplePosDet_GotoScreen( pMe, SCREENID_GETGPSINFO, wParam );
            break;
         case MAINMENU_ITEM_CONFIG:
            SamplePosDet_GotoScreen( pMe, SCREENID_CONFIG, wParam );
         default:
            break;
         }
      }
      break;
   }
   return bHandled;
}
