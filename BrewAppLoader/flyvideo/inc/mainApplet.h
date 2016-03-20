/**
*	file: smartEyeApplet.h
*	author: yuleiming
*/

#ifndef _SMARTEYEAPPLETH
#define _SMARTEYEAPPLETH

#include "framework.h"
#include "ui_dialog.h"

#define LOGTOFILE 0

typedef struct _MainAppData
{
	BFFDlgData mDlgData;    //µ¯³ö¿òÊý¾Ý
	IBitmap* pScreenBackup; //ÆÁÄ»½ØÍ¼
} MainAppData;



MainAppData* GetMainAppData();


#endif /*_SMARTEYEAPPLETH*/