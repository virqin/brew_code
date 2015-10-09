/*======================================================
FILE:  SP_ConfigMenu.c

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "SamplePosDet.h"
#include "SamplePosDet.brh"
#include "AEEMenu.h"
#include "AEEText.h"


/*===========================================================================
                   C O N F I G   M E N U
===========================================================================*/


/*======================================================================= 
Function: SamplePosDet_ConfigMenu_HandleEvent()

Description: 
   Event handler function for the configuration menu.

Prototype:

   boolean SamplePosDet_ConfigMenu_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)

Parameters:
   pMe: [in]. CSamplePosDet instance.
   eCode: [in]. Event code.
   wParam: [in]. Event wParam.
   dwParam: [in]. Event dwParam.

Return Value:

   TRUE - If the event was handled by this menu.
   FALSE - If the event was not handled.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
boolean SamplePosDet_ConfigMenu_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)
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

               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_CONFIG_SERVER,
                  IDS_CONFIG_SERVER, NULL, (uint32)pMenuCtl );

               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_CONFIG_QOS,
                  IDS_CONFIG_QOS, NULL, (uint32)pMenuCtl );

               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_CONFIG_OPT,
                  IDS_CONFIG_OPT, NULL, (uint32)pMenuCtl );

               IMENUCTL_SetTitle( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_CONFIG_TITLE, NULL );
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
      if( wParam == AVK_CLR ) {
         SamplePosDet_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
         bHandled = TRUE;
         break;
      }
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
      bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
      break;

   case EVT_COMMAND:
      if( dwParam == (uint32)pMenuCtl ) {
         switch( wParam ) {
         case IDS_CONFIG_SERVER:
            SamplePosDet_GotoScreen( pMe, SCREENID_SERVER_SETTINGS, 0 );
            bHandled = TRUE;
            break;
         case IDS_CONFIG_QOS:
            SamplePosDet_GotoScreen( pMe, SCREENID_QOS_SETTINGS, 0 );
            bHandled = TRUE;
            break;
         case IDS_CONFIG_OPT:
            SamplePosDet_GotoScreen( pMe, SCREENID_OPTIM_SETTINGS, 0 );
         default:
            break;
         }
      }
      break;
   }
   return bHandled;
}


/*======================================================================= 
Function: SamplePosDet_ServerSettings_HandleEvent()

Description: 
   Event handler function for the server settings menu.

Prototype:

   boolean SamplePosDet_ServerSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)

Parameters:
   pMe: [in]. CSamplePosDet instance.
   eCode: [in]. Event code.
   wParam: [in]. Event wParam.
   dwParam: [in]. Event dwParam.

Return Value:

   TRUE - If the event was handled by this menu.
   FALSE - If the event was not handled.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
boolean SamplePosDet_ServerSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)
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

               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_CONFIG_SERVER_DEFAULT,
                  IDS_CONFIG_SERVER_DEFAULT, NULL, (uint32)pMenuCtl );

               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_CONFIG_SERVER_IP,
                  IDS_CONFIG_SERVER_IP, NULL, (uint32)pMenuCtl );

               IMENUCTL_SetTitle( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_CONFIG_TITLE, NULL );
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
      if( wParam == AVK_CLR ) {
         SamplePosDet_GotoScreen( pMe, SCREENID_CONFIG, 0 );
         bHandled = TRUE;
         break;
      }
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
      bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
      break;

   case EVT_COMMAND:
      if( dwParam == (uint32)pMenuCtl ) {
         switch( wParam ) {
         case IDS_CONFIG_SERVER_DEFAULT:
            pMe->gpsSettings.server.svrType = AEEGPS_SERVER_DEFAULT;
            SamplePosDet_SaveGPSSettings( pMe );
            SamplePosDet_GotoScreen( pMe, SCREENID_CONFIG, 0 );
            bHandled = TRUE;
            break;
         case IDS_CONFIG_SERVER_IP:
            SamplePosDet_GotoScreen( pMe, SCREENID_PDE_SETTINGS, 0 );
            bHandled = TRUE;
            break;
         default:
            break;
         }
      }
      break;
   }
   return bHandled;
}


/*======================================================================= 
Function: SamplePosDet_PDESettings_HandleEvent()

Description: 
   Event handler function for the PDE settings menu.

Prototype:

   boolean SamplePosDet_PDESettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)

Parameters:
   pMe: [in]. CSamplePosDet instance.
   eCode: [in]. Event code.
   wParam: [in]. Event wParam.
   dwParam: [in]. Event dwParam.

Return Value:

   TRUE - If the event was handled by this menu.
   FALSE - If the event was not handled.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
boolean SamplePosDet_PDESettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)
{
   AECHAR * pServer;
   uint32 nResult;
   boolean bHandled = FALSE;
   CTextCtlData *ptcd = SamplePosDet_GetScreenData( pMe );
   ITextCtl *pTextCtl = NULL;
   IMenuCtl *pSoftKeyMenu = NULL;
   char * pszStr;

   if (ptcd) {
      pSoftKeyMenu = ptcd->pSoftKey;
      pTextCtl = ptcd->pTextCtl;
   }

   switch( eCode ) {
   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) {
         if( pTextCtl ) {  /* This is not expected to happen */
            SamplePosDet_DrawScreen( pMe, 0 );
            bHandled = TRUE;
         }
         else {
            if( ISHELL_CreateInstance( pMe->theApp.m_pIShell, AEECLSID_TEXTCTL, 
               (void **)&pTextCtl ) == SUCCESS ) {
               if (ISHELL_CreateInstance(pMe->theApp.m_pIShell, AEECLSID_SOFTKEYCTL,
                  (void **)&pSoftKeyMenu) == SUCCESS) {

                  IMENUCTL_AddItem( pSoftKeyMenu, SAMPLEPOSDET_RES_FILE, IDS_SOFTKEY_SAVE,
                     IDS_SOFTKEY_SAVE, NULL, (uint32)pSoftKeyMenu );

                  ITEXTCTL_SetSoftKeyMenu( pTextCtl, pSoftKeyMenu );
                  ITEXTCTL_SetTitle( pTextCtl, SAMPLEPOSDET_RES_FILE, IDS_PDE_SETTINGS_TITLE, NULL );
                  ITEXTCTL_SetInputMode( pTextCtl, AEE_TM_NUMBERS );

                  // Load the current IP address, if it exists
                  if (AEEGPS_SERVER_IP == pMe->gpsSettings.server.svrType) {
                     pszStr = MALLOC(50);
                     INET_NTOA(pMe->gpsSettings.server.svr.ipsvr.addr, pszStr, 50);
                     pServer = MALLOC(100);
                     STRTOWSTR(pszStr, pServer, 100);
                     ITEXTCTL_SetText(pTextCtl, pServer, -1);
                     FREE(pszStr);
                     FREE(pServer);
                  }

                  ptcd = MALLOC( sizeof(CTextCtlData) );
                  
                  ptcd->pSoftKey = pSoftKeyMenu;
                  ptcd->pTextCtl = pTextCtl;
                  SamplePosDet_SetScreenData( pMe, (void *)ptcd );
                  SamplePosDet_DrawScreen( pMe, 0 );
                  bHandled = TRUE;
               }
            }
         }
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) {
         ITEXTCTL_Release( pTextCtl );
         IMENUCTL_Release( pSoftKeyMenu );
         FREE( ptcd );
         SamplePosDet_SetScreenData( pMe, 0 );
         pMe->currentHandler = 0;
         bHandled = TRUE;
      }
      else if( wParam == SCREEN_PARAM_PAINT ) {
         if( ITEXTCTL_IsActive( pTextCtl ) ) {
            ITEXTCTL_Redraw( pTextCtl );
         }
         else {
            ITEXTCTL_SetActive( pTextCtl, TRUE );  /* Also causes a menu draw */
         }
      }
      break;

   case EVT_KEY:
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
      bHandled = ITEXTCTL_HandleEvent( pTextCtl, eCode, wParam, dwParam );
      if (!bHandled) {
         bHandled = IMENUCTL_HandleEvent( pSoftKeyMenu, eCode, wParam, dwParam );
         
         if(!bHandled && wParam == AVK_CLR && eCode == EVT_KEY) {
            SamplePosDet_GotoScreen( pMe, SCREENID_SERVER_SETTINGS, 0 );
            bHandled = TRUE;
         }
      }
      break;

   case EVT_COMMAND:
      bHandled = ITEXTCTL_HandleEvent( pTextCtl, eCode, wParam, dwParam );
      if (!bHandled) {
         if (IDS_SOFTKEY_SAVE == wParam) {   // Save the new IP address
            pServer = ITEXTCTL_GetTextPtr(pTextCtl);
            pszStr = MALLOC(50);
            WSTRTOSTR(pServer, pszStr, 50);
            if (INET_ATON(pszStr, &nResult)) {
               pMe->gpsSettings.server.svrType = AEEGPS_SERVER_IP;
               pMe->gpsSettings.server.svr.ipsvr.addr = nResult;
               SamplePosDet_SaveGPSSettings(pMe);
               SamplePosDet_GotoScreen(pMe, SCREENID_PORT_SETTINGS, 0);
            }
            else {
               ITEXTCTL_SetTitle(pTextCtl, SAMPLEPOSDET_RES_FILE, IDS_PDE_SETTINGS_INVALID, NULL);
               ITEXTCTL_Redraw(pTextCtl);
            }
            FREE(pszStr);
            // add code to save the new PDE IP address
            bHandled = TRUE;
         }
      }
      break;
   }
   return bHandled;
}


/*======================================================================= 
Function: SamplePosDet_PortSettings_HandleEvent()

Description: 
   Event handler function for the port settings menu.

Prototype:

   boolean SamplePosDet_PortSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)

Parameters:
   pMe: [in]. CSamplePosDet instance.
   eCode: [in]. Event code.
   wParam: [in]. Event wParam.
   dwParam: [in]. Event dwParam.

Return Value:

   TRUE - If the event was handled by this menu.
   FALSE - If the event was not handled.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
boolean SamplePosDet_PortSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)
{
   boolean bHandled = FALSE;
   CTextCtlData *ptcd = SamplePosDet_GetScreenData( pMe );
   ITextCtl *pTextCtl = NULL;
   IMenuCtl *pSoftKeyMenu = NULL;
   char * pszStr;
   AECHAR * pPort;
   uint32 nResult;

   if (ptcd) {
      pSoftKeyMenu = ptcd->pSoftKey;
      pTextCtl = ptcd->pTextCtl;
   }

   switch( eCode ) {
   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) {
         if( pTextCtl ) {  /* This is not expected to happen */
            SamplePosDet_DrawScreen( pMe, 0 );
            bHandled = TRUE;
         }
         else {
            if( ISHELL_CreateInstance( pMe->theApp.m_pIShell, AEECLSID_TEXTCTL, 
               (void **)&pTextCtl ) == SUCCESS ) {
               if (ISHELL_CreateInstance(pMe->theApp.m_pIShell, AEECLSID_SOFTKEYCTL,
                  (void **)&pSoftKeyMenu) == SUCCESS) {

                  IMENUCTL_AddItem( pSoftKeyMenu, SAMPLEPOSDET_RES_FILE, IDS_SOFTKEY_SAVE,
                     IDS_SOFTKEY_SAVE, NULL, (uint32)pSoftKeyMenu );

                  ITEXTCTL_SetSoftKeyMenu( pTextCtl, pSoftKeyMenu );
                  ITEXTCTL_SetTitle( pTextCtl, SAMPLEPOSDET_RES_FILE, IDS_PORT_SETTINGS_TITLE, NULL );
                  ITEXTCTL_SetInputMode( pTextCtl, AEE_TM_NUMBERS );

                  // Load the current port number, if it exists
                  if (AEEGPS_SERVER_IP == pMe->gpsSettings.server.svrType) {
                     pszStr = MALLOC(50);
                     pPort = MALLOC(100);
                     SPRINTF(pszStr, "%d", AEE_ntohs(pMe->gpsSettings.server.svr.ipsvr.port));
                     STRTOWSTR(pszStr, pPort, 100);
                     ITEXTCTL_SetText(pTextCtl, pPort, -1);
                     FREE(pszStr);
                     FREE(pPort);
                  }

                  ptcd = MALLOC( sizeof(CTextCtlData) );

                  ptcd->pSoftKey = pSoftKeyMenu;
                  ptcd->pTextCtl = pTextCtl;
                  SamplePosDet_SetScreenData( pMe, (void *)ptcd );
                  SamplePosDet_DrawScreen( pMe, 0 );
                  bHandled = TRUE;
               }
            }
         }
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) {
         ITEXTCTL_Release( pTextCtl );
         IMENUCTL_Release( pSoftKeyMenu );
         FREE( ptcd );
         SamplePosDet_SetScreenData( pMe, 0 );
         pMe->currentHandler = 0;
         bHandled = TRUE;
      }
      else if( wParam == SCREEN_PARAM_PAINT ) {
         if( ITEXTCTL_IsActive( pTextCtl ) ) {
            ITEXTCTL_Redraw( pTextCtl );
         }
         else {
            ITEXTCTL_SetActive( pTextCtl, TRUE );  /* Also causes a menu draw */
         }
      }
      break;

   case EVT_KEY:
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
      bHandled = ITEXTCTL_HandleEvent( pTextCtl, eCode, wParam, dwParam );
      if (!bHandled) {
         bHandled = IMENUCTL_HandleEvent( pSoftKeyMenu, eCode, wParam, dwParam );
         
         if(!bHandled && wParam == AVK_CLR && eCode == EVT_KEY) {
            SamplePosDet_GotoScreen( pMe, SCREENID_CONFIG, 0 );
            bHandled = TRUE;
         }
      }
      break;
   case EVT_COMMAND:
      bHandled = ITEXTCTL_HandleEvent( pTextCtl, eCode, wParam, dwParam );
      if (!bHandled) {
         if (IDS_SOFTKEY_SAVE == wParam) {
            // Need to validate and save the Port settings here
            pPort = ITEXTCTL_GetTextPtr(pTextCtl);
            pszStr = MALLOC(50);
            WSTRTOSTR(pPort, pszStr, 50);
            if (0 != (nResult = STRTOUL(pszStr, NULL, 10))) {
               pMe->gpsSettings.server.svr.ipsvr.port = AEE_htons((unsigned short)nResult);
               SamplePosDet_SaveGPSSettings(pMe);
               SamplePosDet_GotoScreen(pMe, SCREENID_CONFIG, 0);
            }
            else {
               ITEXTCTL_SetTitle(pTextCtl, SAMPLEPOSDET_RES_FILE, IDS_PORT_SETTINGS_INVALID, NULL);
               ITEXTCTL_Redraw(pTextCtl);
            }
            FREE(pszStr);
            // add code to save the new PDE IP address
            bHandled = TRUE;
         }
      }
      break;
   }
   return bHandled;
}


/*======================================================================= 
Function: SamplePosDet_QosSettings_HandleEvent()

Description: 
   Event handler function for the QoS settings menu.

Prototype:

   boolean SamplePosDet_QosSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)

Parameters:
   pMe: [in]. CSamplePosDet instance.
   eCode: [in]. Event code.
   wParam: [in]. Event wParam.
   dwParam: [in]. Event dwParam.

Return Value:

   TRUE - If the event was handled by this menu.
   FALSE - If the event was not handled.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
boolean SamplePosDet_QosSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)
{
   boolean bHandled = FALSE;
   CTextCtlData *ptcd = SamplePosDet_GetScreenData( pMe );
   ITextCtl *pTextCtl = NULL;
   IMenuCtl *pSoftKeyMenu = NULL;
   char * pszStr;
   AECHAR * pQos;
   uint32 nResult;

   if (ptcd) {
      pSoftKeyMenu = ptcd->pSoftKey;
      pTextCtl = ptcd->pTextCtl;
   }

   switch( eCode ) {
   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) {
         if( pTextCtl ) {  /* This is not expected to happen */
            SamplePosDet_DrawScreen( pMe, 0 );
            bHandled = TRUE;
         }
         else {
            if( ISHELL_CreateInstance( pMe->theApp.m_pIShell, AEECLSID_TEXTCTL, 
               (void **)&pTextCtl ) == SUCCESS ) {
               if (ISHELL_CreateInstance(pMe->theApp.m_pIShell, AEECLSID_SOFTKEYCTL,
                  (void **)&pSoftKeyMenu) == SUCCESS) {

                  IMENUCTL_AddItem( pSoftKeyMenu, SAMPLEPOSDET_RES_FILE, IDS_SOFTKEY_SAVE,
                     IDS_SOFTKEY_SAVE, NULL, (uint32)pSoftKeyMenu );

                  ITEXTCTL_SetSoftKeyMenu( pTextCtl, pSoftKeyMenu );
                  ITEXTCTL_SetTitle( pTextCtl, SAMPLEPOSDET_RES_FILE, IDS_QOS_SETTINGS_TITLE, NULL );
                  ITEXTCTL_SetInputMode( pTextCtl, AEE_TM_NUMBERS );

                  // Load the current QoS value, if it exists
                  pszStr = MALLOC(50);
                  pQos = MALLOC(100);
                  SPRINTF(pszStr, "%d", pMe->gpsSettings.qos);
                  STRTOWSTR(pszStr, pQos, 100);
                  ITEXTCTL_SetText(pTextCtl, pQos, -1);
                  FREE(pszStr);
                  FREE(pQos);

                  ptcd = MALLOC( sizeof(CTextCtlData) );

                  ptcd->pSoftKey = pSoftKeyMenu;
                  ptcd->pTextCtl = pTextCtl;
                  SamplePosDet_SetScreenData( pMe, (void *)ptcd );
                  SamplePosDet_DrawScreen( pMe, 0 );
                  bHandled = TRUE;
               }
            }
         }
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) {
         ITEXTCTL_Release( pTextCtl );
         IMENUCTL_Release( pSoftKeyMenu );
         FREE( ptcd );
         SamplePosDet_SetScreenData( pMe, 0 );
         pMe->currentHandler = 0;
         bHandled = TRUE;
      }
      else if( wParam == SCREEN_PARAM_PAINT ) {
         if( ITEXTCTL_IsActive( pTextCtl ) ) {
            ITEXTCTL_Redraw( pTextCtl );
         }
         else {
            ITEXTCTL_SetActive( pTextCtl, TRUE );  /* Also causes a menu draw */
         }
      }
      break;

   case EVT_KEY:
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
      bHandled = ITEXTCTL_HandleEvent( pTextCtl, eCode, wParam, dwParam );
      if (!bHandled) {
         bHandled = IMENUCTL_HandleEvent( pSoftKeyMenu, eCode, wParam, dwParam );
         
         if(!bHandled && wParam == AVK_CLR && eCode == EVT_KEY) {
            SamplePosDet_GotoScreen( pMe, SCREENID_CONFIG, 0 );
            bHandled = TRUE;
         }
      }
      break;
   case EVT_COMMAND:
      bHandled = ITEXTCTL_HandleEvent( pTextCtl, eCode, wParam, dwParam );
      if (!bHandled) {
         if (IDS_SOFTKEY_SAVE == wParam) {
            // Need to validate and save the Port settings here
            pQos = ITEXTCTL_GetTextPtr(pTextCtl);
            pszStr = MALLOC(50);
            WSTRTOSTR(pQos, pszStr, 50);
            nResult = STRTOUL(pszStr, NULL, 10);
            if (nResult >= 0 && nResult <= 255) {
               pMe->gpsSettings.qos = (AEEGPSQos)nResult;
               SamplePosDet_SaveGPSSettings(pMe);
               SamplePosDet_GotoScreen(pMe, SCREENID_CONFIG, 0);
            }
            else {
               ITEXTCTL_SetTitle(pTextCtl, SAMPLEPOSDET_RES_FILE, IDS_QOS_SETTINGS_INVALID, NULL);
               ITEXTCTL_Redraw(pTextCtl);
            }
            FREE(pszStr);
            bHandled = TRUE;
         }
      }
      break;
   }
   return bHandled;
}


/*======================================================================= 
Function: SamplePosDet_OptimSettings_HandleEvent()

Description: 
   Event handler function for the optimization mode settings menu.

Prototype:

   boolean SamplePosDet_OptimSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)

Parameters:
   pMe: [in]. CSamplePosDet instance.
   eCode: [in]. Event code.
   wParam: [in]. Event wParam.
   dwParam: [in]. Event dwParam.

Return Value:

   TRUE - If the event was handled by this menu.
   FALSE - If the event was not handled.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
boolean SamplePosDet_OptimSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam)
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

               //IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_OPT_NONE,
               //   AEEGPS_OPT_NONE, NULL, (uint32)pMenuCtl );

               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_OPT_SPEED,
                  AEEGPS_OPT_SPEED, NULL, (uint32)pMenuCtl );

               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_OPT_ACCURACY,
                  AEEGPS_OPT_ACCURACY, NULL, (uint32)pMenuCtl );

               //IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_OPT_PAYLOAD,
               //   AEEGPS_OPT_PAYLOAD, NULL, (uint32)pMenuCtl );

               IMENUCTL_AddItem( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_OPT_DEFAULT,
                  AEEGPS_OPT_DEFAULT, NULL, (uint32)pMenuCtl );

               IMENUCTL_SetTitle( pMenuCtl, SAMPLEPOSDET_RES_FILE, IDS_OPT_SETTINGS_TITLE, NULL );
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
      if( wParam == AVK_CLR ) {
         SamplePosDet_GotoScreen( pMe, SCREENID_CONFIG, 0 );
         bHandled = TRUE;
         break;
      }
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
      bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
      break;

   case EVT_COMMAND:
      if( dwParam == (uint32)pMenuCtl ) {
         switch( wParam ) {
         case AEEGPS_OPT_SPEED:
         case AEEGPS_OPT_ACCURACY:
            // Set the optimization mode and save the settings
            pMe->gpsSettings.optim = (AEEGPSOpt)wParam;
            SamplePosDet_SaveGPSSettings(pMe);
            SamplePosDet_GotoScreen( pMe, SCREENID_CONFIG, 0 );
            bHandled = TRUE;
            break;
         default:
            break;
         }
      }
      break;
   }
   return bHandled;
}


/*======================================================================= 
Function: SamplePosDet_InitGPSSettings()

Description: 
   Initializes the GPS configuration to the default values.

Prototype:

   uint32 SamplePosDet_InitGPSSettings( CSamplePosDet *pMe );

Parameters:
   pMe: [in]. CSamplePosDet instance.

Return Value:

   SUCCESS - If the settings were initialized successfully.
   Returns error code otherwise.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
uint32 SamplePosDet_InitGPSSettings(CSamplePosDet *pMe)
{
   IFileMgr   *pIFileMgr = NULL;
   IFile      *pIConfigFile = NULL;
   uint32      nResult = 0;

   // Create the instance of IFileMgr
   nResult = ISHELL_CreateInstance( pMe->theApp.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
   if ( SUCCESS != nResult ) {
      return nResult;
   }

   // If the config file exists, open it and read the settings.  Otherwise, we need to
   // create a new config file.
   nResult = IFILEMGR_Test( pIFileMgr, SPD_CONFIG_FILE );
   if ( SUCCESS == nResult ) {
      pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READ );
      if ( !pIConfigFile ) {
         nResult = EFAILED;
      }
      else {
         nResult = SamplePosDet_ReadGPSSettings( pMe, pIConfigFile );
      }
   }
   else {
      pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_CREATE );
      if ( !pIConfigFile ) {
         nResult = EFAILED;
      }
      else {
         // Setup the default GPS settings
         pMe->gpsSettings.optim  = AEEGPS_OPT_DEFAULT;
         pMe->gpsSettings.qos    = SPD_QOS_DEFAULT;
         pMe->gpsSettings.server.svrType = AEEGPS_SERVER_DEFAULT;
         nResult = SamplePosDet_WriteGPSSettings( pMe, pIConfigFile );
      }
   }

   // Free the IFileMgr and IFile instances
   IFILE_Release( pIConfigFile );
   IFILEMGR_Release( pIFileMgr );

   return nResult;
}



/*======================================================================= 
Function: DistToSemi()

Description: 
   Utility function that determines index of the first semicolon in the
   input string.

Prototype:

   int DistToSemi(const char * pszStr);

Parameters:
   pszStr: [in]. String to check.

Return Value:

   The index of the first semicolon.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
int DistToSemi(const char * pszStr)
{
   int nCount = 0;

   if ( !pszStr ) { 
      return -1;
   }

   while ( *pszStr != 0 ) {
      if ( *pszStr == ';' ) {
         return nCount;
      }
      else {
         nCount++;
         pszStr++;
      }
   }

   return -1;
}



/*======================================================================= 
Function: SamplePosDet_ReadGPSSettings()

Description: 
   Reads the GPS configuration settings from the configuration file.

Prototype:

   uint32 SamplePosDet_ReadGPSSettings(CSamplePosDet *pMe, IFile * pIFile);

Parameters:
   pMe: [in]. CSamplePosDet instance.
   pIFile: [in].  Pointer to the IFile instance representing the config
                  file.

Return Value:

  SUCCESS - If the settings were read successfully.
  Error code otherwise.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
uint32 SamplePosDet_ReadGPSSettings(CSamplePosDet *pMe, IFile * pIFile)
{
   char    *pszBuf = NULL;
   char    *pszTok = NULL;
   char    *pszSvr = NULL;
   char    *pszDelimiter = ";";
   int32   nResult = 0;
   FileInfo fiInfo;

   if ( SUCCESS != IFILE_GetInfo( pIFile, &fiInfo ) ) {
      return EFAILED;
   }

   if ( fiInfo.dwSize == 0 ) {
      return EFAILED;
   }

   // Allocate enough memory to read the full text into memory
   pszBuf = MALLOC( fiInfo.dwSize );

   nResult = IFILE_Read( pIFile, pszBuf, fiInfo.dwSize );
   if ( (uint32)nResult < fiInfo.dwSize ) {
      FREE( pszBuf );
      return EFAILED;
   }

   // Check for an optimization mode setting in the file:
   pszTok = STRSTR( pszBuf, SPD_CONFIG_OPT_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_OPT_STRING );
      pMe->gpsSettings.optim = (AEEGPSOpt)STRTOUL( pszTok, &pszDelimiter, 10 );
   }

   // Check for a QoS setting in the file:
   pszTok = STRSTR( pszBuf, SPD_CONFIG_QOS_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_QOS_STRING );
      pMe->gpsSettings.qos = (AEEGPSQos)STRTOUL( pszTok, &pszDelimiter, 10 );
   }

   // Check for a server type setting in the file:
   pszTok = STRSTR( pszBuf, SPD_CONFIG_SVR_TYPE_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_SVR_TYPE_STRING );
      pMe->gpsSettings.server.svrType = STRTOUL( pszTok, &pszDelimiter, 10 );

      // If the server type is IP, we need to find the ip address and the port number
      if ( AEEGPS_SERVER_IP == pMe->gpsSettings.server.svrType ) {
         pszTok = STRSTR( pszBuf, SPD_CONFIG_SVR_IP_STRING );
         if ( pszTok ) {
            pszTok = pszTok + STRLEN( SPD_CONFIG_SVR_IP_STRING );
            nResult = DistToSemi( pszTok );
            pszSvr = MALLOC( nResult+1 );
            STRNCPY( pszSvr, pszTok, nResult );
            *(pszSvr+nResult) = 0;  // Need to manually NULL-terminate the string
            if ( !INET_ATON( pszSvr, &pMe->gpsSettings.server.svr.ipsvr.addr ) ) {
               FREE( pszBuf );
               FREE( pszSvr );
               return EFAILED;
            }
            FREE( pszSvr );
         }
         pszTok = STRSTR( pszBuf, SPD_CONFIG_SVR_PORT_STRING );
         if ( pszTok ) {
            pszTok = pszTok + STRLEN( SPD_CONFIG_SVR_PORT_STRING );
            pMe->gpsSettings.server.svr.ipsvr.port = AEE_htons((INPort)STRTOUL( pszTok, &pszDelimiter, 10 ));
         }
      }
   }

   FREE( pszBuf );
   
   return SUCCESS;
}


/*======================================================================= 
Function: SamplePosDet_WriteGPSSettings()

Description: 
   Write the GPS configuration settings from the configuration file.

Prototype:

   uint32 SamplePosDet_WriteGPSSettings(CSamplePosDet *pMe, IFile * pIFile);

Parameters:
   pMe: [in]. CSamplePosDet instance.
   pIFile: [in].  Pointer to the IFile instance representing the config
                  file.

Return Value:

  SUCCESS - If the settings were written successfully.
  EFAILED - If an error occured.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
uint32 SamplePosDet_WriteGPSSettings( CSamplePosDet *pMe, IFile * pIFile )
{
   char    *pszBuf;
   int32    nResult;

   pszBuf = MALLOC( 1024 );

   // Truncate the file, in case it already contains data
   IFILE_Truncate( pIFile, 0 );

   // Write out the optimization setting:
   SPRINTF( pszBuf, SPD_CONFIG_OPT_STRING"%d;\r\n", pMe->gpsSettings.optim );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
      FREE(pszBuf);
      return EFAILED;
   }
   
   // Write out the QoS setting:
   SPRINTF( pszBuf, SPD_CONFIG_QOS_STRING"%d;\r\n", pMe->gpsSettings.qos );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
      FREE(pszBuf);
      return EFAILED;
   }
   
   // Write out the server type setting:
   SPRINTF( pszBuf, SPD_CONFIG_SVR_TYPE_STRING"%d;\r\n", pMe->gpsSettings.server.svrType );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
      FREE(pszBuf);
      return EFAILED;
   }
   
   if ( AEEGPS_SERVER_IP == pMe->gpsSettings.server.svrType ) {
      // Write out the IP address setting:
      INET_NTOA( pMe->gpsSettings.server.svr.ipsvr.addr, pszBuf, 50 );
      nResult = IFILE_Write( pIFile, SPD_CONFIG_SVR_IP_STRING, STRLEN( SPD_CONFIG_SVR_IP_STRING ) );
      if ( 0 == nResult ) {
         FREE(pszBuf);
         return EFAILED;
      }
      nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
      if ( 0 == nResult ) {
         FREE(pszBuf);
         return EFAILED;
      }
      nResult = IFILE_Write( pIFile, ";\r\n", STRLEN( ";\r\n" ) );
      if ( 0 == nResult ) {
         FREE(pszBuf);
         return EFAILED;
      }

      // Write out the port setting:
      SPRINTF( pszBuf, SPD_CONFIG_SVR_PORT_STRING"%d;\r\n", AEE_ntohs(pMe->gpsSettings.server.svr.ipsvr.port) );
      nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
      if ( 0 == nResult ) {
         FREE(pszBuf);
         return EFAILED;
      }
   }

   FREE( pszBuf );

   return SUCCESS;
}



/*======================================================================= 
Function: SamplePosDet_SaveGPSSettings()

Description: 
   Opens the configuration file and saves the settings.

Prototype:

   uint32 SamplePosDet_SaveGPSSettings(CSamplePosDet *pMe);

Parameters:
   pMe: [in]. CSamplePosDet instance.

Return Value:

  SUCCESS - If the settings were written successfully.
  Error code otherwise.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
uint32 SamplePosDet_SaveGPSSettings( CSamplePosDet *pMe )
{
   IFileMgr   *pIFileMgr = NULL;
   IFile      *pIConfigFile = NULL;
   uint32      nResult = 0;

   // Create the instance of IFileMgr
   nResult = ISHELL_CreateInstance( pMe->theApp.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
   if ( SUCCESS != nResult ) {
      return nResult;
   }

   // If the config file exists, open it and read the settings.  Otherwise, we need to
   // create a new config file.
   nResult = IFILEMGR_Test( pIFileMgr, SPD_CONFIG_FILE );
   if ( SUCCESS == nResult ) {
      pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE );
      if ( !pIConfigFile ) {
         nResult = EFAILED;
      }
      else {
         nResult = SamplePosDet_WriteGPSSettings( pMe, pIConfigFile );
      }
   }
   else {
      pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_CREATE );
      if ( !pIConfigFile ) {
         nResult = EFAILED;
      }
      else {
         nResult = SamplePosDet_WriteGPSSettings( pMe, pIConfigFile );
      }
   }

   // Free the IFileMgr and IFile instances
   IFILE_Release( pIConfigFile );
   IFILEMGR_Release( pIFileMgr );

   return nResult;
}
