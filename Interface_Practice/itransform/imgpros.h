#ifndef  IMGPROS_H_
#define  IMGPROS_H_

#include "AEE.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEEShell.h"
#include "AEETransform.h"



#define SPLASH_DISPLAY_TIMER         750  
#define N  2 



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
    uint16			m_cxWidth;							//屏幕宽度
    uint16			m_cyHeight;							//屏幕高度
    uint16			m_nColorDepth;						//颜色深度
    AEERect			m_rScreenRect;

    IImage*		    m_pSplash;
    IImage*         m_pImage;
    ITransform*     m_iTransform;
    IMenuCtl*       m_pMenu;
    IMenuCtl*	    m_pISpeedMenu;
    CImgProsAppPrefs m_Prefs;
}CImgProsApp;
#endif
