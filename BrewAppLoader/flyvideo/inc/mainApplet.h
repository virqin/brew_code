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
	BFFDlgData mDlgData;    //����������
	IBitmap* pScreenBackup; //��Ļ��ͼ
} MainAppData;



MainAppData* GetMainAppData();


#endif /*_SMARTEYEAPPLETH*/