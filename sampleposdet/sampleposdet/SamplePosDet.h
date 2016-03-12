/*======================================================
FILE:  SamplePosDet.h

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef __SAMPLEPOSDET_H__
#define __SAMPLEPODET_H__

#include "AEE.h"
#include "AEEShell.h"           // Shell interface definitions
#include "AEEPosDet.h"          // Position Determination interface definitions.
#include "AEEStdLib.h"
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEText.h"
#include "AEEMenu.h"
#include "AEEFile.h"
#include "sampleposdet_res.h"

/*-------------------------------------------------------------------
Custom types and definitions
-------------------------------------------------------------------*/
#define EVT_SCREEN             (EVT_USER+1)

#define SCREEN_PARAM_GOTO      0       /* Private event */
#define SCREEN_PARAM_INIT      1
#define SCREEN_PARAM_CLOSE     2
#define SCREEN_PARAM_PAINT     3

#define SPD_CONFIG_FILE             "config.txt"
#define SPD_CONFIG_OPT_STRING       "GPS_OPTIMIZATION_MODE = "
#define SPD_CONFIG_QOS_STRING       "GPS_QOS = "
#define SPD_CONFIG_SVR_TYPE_STRING  "GPS_SERVER_TYPE = "
#define SPD_CONFIG_SVR_IP_STRING    "GPS_SERVER_IP = "
#define SPD_CONFIG_SVR_PORT_STRING  "GPS_SERVER_PORT = "

#define SPD_QOS_DEFAULT       127

/* An enumeration for different screens in the applet. */
typedef enum _ScreenId {
   SCREENID_MAINMENU,
   SCREENID_GETGPSINFO,
   SCREENID_CONFIG,
   SCREENID_PDE_SETTINGS,
   SCREENID_QOS_SETTINGS,
   SCREENID_PORT_SETTINGS,
   SCREENID_OPTIM_SETTINGS,
   SCREENID_SERVER_SETTINGS,
   SCREENID_MAX         /* Insert new screens above this entry
                        ** and its association in gAppScreens */
} ScreenId;

typedef struct _CTextCtlData {
   ITextCtl *pTextCtl;
   IMenuCtl *pSoftKey;
} CTextCtlData;

/* Maintains the screen to its handler association */
typedef struct _CScreen {
   AEEHANDLER theHandler;
} CScreen;

typedef struct _CSettings {
   AEEGPSOpt      optim;
   AEEGPSQos      qos;
   AEEGPSServer   server;
} CSettings;

/* Application class. This is a singleton object in the applet and it
** has the life of the applet. */
typedef struct _CSamplePosDet {
   AEEApplet   theApp;
   CScreen     appScreens[SCREENID_MAX];
   AEEHANDLER  currentHandler;
   void        *pScreenData;
   CSettings   gpsSettings;
}CSamplePosDet;

typedef enum {
   MAINMENU_ITEM_ONE_SHOT,
   MAINMENU_ITEM_TRACK_LOCAL,
   MAINMENU_ITEM_TRACK_NETWORK,
   MAINMENU_ITEM_DLOAD_FIRST,
   MAINMENU_ITEM_TRACK_AUTO,
   MAINMENU_ITEM_CONFIG
}MainMenuItems;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
extern boolean SamplePosDet_GotoScreen( CSamplePosDet *pMe, ScreenId id, uint32 dwParam );
extern void SamplePosDet_DrawScreen( CSamplePosDet *pMe, uint32 dwParam );
extern void *SamplePosDet_SetScreenData( CSamplePosDet *pMe, void *screenData );
extern void *SamplePosDet_GetScreenData( CSamplePosDet *pMe );

extern boolean SamplePosDet_MainMenu_HandleEvent( CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );
extern boolean SamplePosDet_ConfigMenu_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam);
extern boolean SamplePosDet_GetGPSInfo_HandleEvent( CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );
extern boolean SamplePosDet_PDESettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam);
extern boolean SamplePosDet_PortSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam);
extern boolean SamplePosDet_OptimSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam);
extern boolean SamplePosDet_QosSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam);
extern boolean SamplePosDet_ServerSettings_HandleEvent(CSamplePosDet *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam);
extern void SamplePosDet_Printf( CSamplePosDet *pMe, int nLine, int nCol, AEEFont fnt, 
                                uint32 dwFlags, const char *szFormat, ... );
uint32 SamplePosDet_InitGPSSettings( CSamplePosDet *pMe );
uint32 SamplePosDet_ReadGPSSettings( CSamplePosDet *pMe, IFile * pIFile );
uint32 SamplePosDet_WriteGPSSettings( CSamplePosDet *pMe, IFile * pIFile );
uint32 SamplePosDet_SaveGPSSettings( CSamplePosDet *pMe );

#endif /* !__SAMPLEPOSDET_H__ */
