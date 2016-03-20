#ifndef  IMGPROS_H_
#define  IMGPROS_H_

#include "AEE.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEEShell.h"
#include "AEEIBitmapScale.h"

#define SPLASH_DISPLAY_TIMER         750   
#define PREFS_MAX_FWYS 64

typedef struct
{
	int DefaultFwy;
	char DefaultRamp[PREFS_MAX_FWYS];
} CImgProsAppPrefs;

typedef struct _CImgProsApp
{
	AEEApplet		a;
	AEEDeviceInfo   m_DeviceInfo;
	uint16			m_cxWidth;							//��Ļ���
	uint16			m_cyHeight;							//��Ļ�߶�
	uint16			m_nColorDepth;						//��ɫ���
	AEERect			m_rScreenRect;

	AEERect			m_prcSrc;
	AEERect			m_prcDst;
	AEERect			m_prcClip;

	IImage*		    m_pImage1;
	IBitmapScale*   m_iBitmapScale; 
    
	IMenuCtl*       m_pMenu;
	boolean		    m_bSuspended;

	CImgProsAppPrefs m_Prefs;

}CImgProsApp;

static boolean CImgProsApp_HandleEvent(CImgProsApp* pApp,AEEEvent eCode, uint16 wParam, uint32 dwParam );
static void    CImgProsApp_FreeAppData(CImgProsApp* pApp);
static boolean CImgProsApp_InitAppData(IApplet*po,uint32 clsid,IShell* pIShell);
static void CImgProsApp_DisplaySplashScreen(CImgProsApp * pApp);
static void CImgProsApp_DisplayMainMenu( CImgProsApp* pApp);
static void SetMenuStyle(CImgProsApp* pMe, IMenuCtl* pIMenu);
static void CImgProsApp_DisplayImage(CImgProsApp * pApp,int number);

#endif