/**
*	file: dialog.c
*	author: yuleiming
*/
#include "ui_dialog.h"
#include "mainApplet.h"
#include "sceneUtils.h"

static void _dlg_draw_common();
static void _dlg_draw_icon(BFFDlgType type);
static void _dlg_draw_process();
static void _dlg_backup_screen();

static void _dlg_draw_common()
{
	IDisplay* display = BFF_DISPLAY();
	BFFDlgData* data = &GetMainAppData()->mDlgData;

	//background
	IDISPLAY_DrawRect(display,&data->mRect,data->mBorderColor,data->mBackgroundColor,IDF_RECT_FRAME|IDF_RECT_FILL);
	
}

static void _dlg_draw_icon(BFFDlgType type)
{
	IDisplay* display = BFF_DISPLAY();
	BFFDlgData* data = &GetMainAppData()->mDlgData;
	IImage* pImage;

	switch(type)
	{
	case ESEDlgNote:
		initImageByID(&pImage,IDI_ASK_ICO);break;
	case ESEDlgQuery:
		initImageByID(&pImage,IDI_ASK_ICO);break;
	case ESEDlgProcess:
		initImageByID(&pImage,IDI_ASK_ICO);break;
	case ESEDlgError:
		initImageByID(&pImage,IDI_ASK_ICO);break;
	default:break;
	}
	drawImage(pImage,data->mIconRect.x,data->mIconRect.y,IDF_TOP|IDF_LEFT);
	IIMAGE_Release(pImage);
}

static void _dlg_draw_process(BFFDlgData* data)
{
	IDisplay* display = BFF_DISPLAY();
	AEERect processBarLeftRect;
	AEERect processBarRightRect;

	IDISPLAY_DrawRect(display,&data->mProcessBarRect,data->mBorderColor,0,IDF_RECT_FRAME);

	processBarLeftRect.x = data->mProcessBarRect.x + 1;
	processBarLeftRect.y = data->mProcessBarRect.y + 1;
	processBarLeftRect.dx = (int16)((data->mProcessBarRect.dx - 2) * data->mProcessPercent / 100);
	processBarLeftRect.dy = data->mProcessBarRect.dy - 2;

	processBarRightRect.x = processBarLeftRect.x + processBarLeftRect.dx + 1;
	processBarRightRect.y = processBarLeftRect.y;
	processBarRightRect.dx = data->mProcessBarRect.dx - 3 - processBarLeftRect.dx;
	processBarRightRect.dy = processBarLeftRect.dy;
	
	IDISPLAY_DrawRect(display,&processBarLeftRect,0,data->mProcessBarColor,IDF_RECT_FILL);
	IDISPLAY_DrawRect(display,&processBarRightRect,0,data->mBackgroundColor,IDF_RECT_FILL);
	IDISPLAY_Update(display);

	data->mProcessPercent = (data->mProcessPercent + KProcessPercentInterval) % 100;
	if(data->mProcessPercent < 2) data->mProcessPercent = 100;
	ISHELL_SetTimer(BFF_SHELL(),KProcessTimerInterval,(PFNNOTIFY)_dlg_draw_process,data);

}

int se_dlg_init()
{
	BFFDlgData* data = &GetMainAppData()->mDlgData;

	data->pScreenBackup = NULL;
	
	//rect
	data->mRect.x = 0;
	data->mRect.y = (int16)(getDeviceHeight() * KHeightPercentOfScreen);
	data->mRect.dx = getDeviceWidth();
	data->mRect.dy = getDeviceHeight() - data->mRect.y;

	//process bar rect
	data->mProcessBarRect.dx = (int16)(data->mRect.dx * KProcessBarWidthPercentOfDlg);
	data->mProcessBarRect.dy = KProcessBarHeightInPixels;
	data->mProcessBarRect.x = (int16)((data->mRect.dx - data->mProcessBarRect.dx)/2);
	data->mProcessBarRect.y = data->mRect.y + (int16)(data->mRect.dy * KProcessBarOffsetYPercent);

	//label rect
	data->mLabelRect.x = data->mProcessBarRect.x;
	data->mLabelRect.y = data->mRect.y + data->mRect.dy * KLabelOffsetYPercent;
	data->mLabelRect.dx = data->mRect.dx - 2 * data->mLabelRect.x - KIconWidthInPixel - KGapBetweenLabelAndIconInPixel;
	data->mLabelRect.dy = data->mProcessBarRect.y - data->mRect.y;

	//icon rect
	data->mIconRect.x = data->mLabelRect.x + data->mLabelRect.dx + KGapBetweenLabelAndIconInPixel;
	data->mIconRect.y = data->mLabelRect.y - 5;
	data->mIconRect.dx = KIconWidthInPixel;
	data->mIconRect.dy = KIconWidthInPixel;
	
	//default spec
	data->mBackgroundColor = /*BFF_RGB_WHITE_SMOKE*/MAKE_RGB(0xB1,0xB3,0xB8);
	data->mBorderColor = BFF_RGB_BLACK;
	data->mFontColor = BFF_RGB_BLACK;
	data->mProcessBarColor = /*BFF_RGB_BLUE_VIOLET*/MAKE_RGB(57,128,244);

	return SUCCESS;
}

void se_dlg_finalize()
{
	BFFDlgData* dlgData = &GetMainAppData()->mDlgData;
	dismiss_current_dlg();
}

void se_dlg_hit_LSK()
{
	BFFDlgData* dlgData = &GetMainAppData()->mDlgData;
	switch(dlgData->mType)
	{
	case ESEDlgNote:
	case ESEDlgQuery:
	case ESEDlgError:
		{
			dismiss_current_dlg();
			if(dlgData->pcbLSK)
				dlgData->pcbLSK();
			break;
		}
	case ESEDlgProcess:
	case ESEDlgNone:
	default: break;
	}
}

void se_dlg_hit_RSK()
{
	BFFDlgData* dlgData = &GetMainAppData()->mDlgData;
	switch(dlgData->mType)
	{
	case ESEDlgQuery:
		{
			dismiss_current_dlg();
			if(dlgData->pcbRSK)
				dlgData->pcbRSK();
			break;
		}
	case ESEDlgProcess:
		{
			if(dlgData->pcbRSK)
			{
				dismiss_current_dlg();
				dlgData->pcbRSK();
			}
			break;
		}
	case ESEDlgError:
	case ESEDlgNote:
	case ESEDlgNone:
	default: break;
	}
}

static void _dlg_backup_screen()
{
	BFFDlgData* dlgData = &GetMainAppData()->mDlgData;
	IBitmap* pDeviceBitmap;
	IDISPLAY_GetDeviceBitmap(BFF_DISPLAY(),&pDeviceBitmap);
	IBITMAP_CreateCompatibleBitmap( pDeviceBitmap, &dlgData->pScreenBackup, getDeviceWidth(), getDeviceHeight());
	IBITMAP_BltIn( dlgData->pScreenBackup, 0, 0, getDeviceWidth(),getDeviceHeight(), pDeviceBitmap, 0, 0, AEE_RO_COPY );
	IBITMAP_Release( pDeviceBitmap );
}


int error_dlg_run_with_error_code(int errCode)
{
	int16 errCodeStrId;
	AECHAR* content;
	BFFDlgData* dlgData = &GetMainAppData()->mDlgData;
	if(current_dlg_type() != ESEDlgNone) return EBADSTATE;
	if(errCode < 1) return EBADPARM;

	/*if(errCode >= K_ERR_OTHER)
	{
		errCodeStrId = IDS_STRING_ERR_OTHER;
	}
	else
	{
		errCodeStrId = IDS_STRING_ERR_CU_NOT_EXIST - 1 + errCode;
	}*/
	/*²âÊÔ´úÂë*/
	initStringByID(&content,IDS_APP_NAME);

	//backuped screen
	_dlg_backup_screen();
	_dlg_draw_common();

	//label
	IDisplay_SetColor(BFF_DISPLAY(),CLR_USER_BACKGROUND,dlgData->mBackgroundColor);
	IDisplay_SetColor(BFF_DISPLAY(),CLR_USER_TEXT,dlgData->mFontColor);
	if(dlgData->pLabel)
		ISTATIC_Release(dlgData->pLabel);
	ISHELL_CreateInstance(BFF_SHELL(), AEECLSID_STATIC, (void **)&dlgData->pLabel);
	ISTATIC_SetRect(dlgData->pLabel,&dlgData->mLabelRect);
	ISTATIC_SetText(dlgData->pLabel,NULL,content,AEE_FONT_NORMAL,AEE_FONT_NORMAL);
	ISTATIC_Redraw(dlgData->pLabel);
	FREE(content);

	//icon
	_dlg_draw_icon(ESEDlgError);
	drawCMDLine(IDS_CMD_OK,0);
	dlgData->mType = ESEDlgError;
	dlgData->pcbLSK = NULL;
	dlgData->pcbRSK = NULL;
	return SUCCESS;

}


//LSK: Ok	RSK: Cancel
int query_dlg_run(AECHAR* content,BFFDialogCallbackFunc* cbLSK,BFFDialogCallbackFunc* cbRSK)
{
	BFFDlgData* dlgData = &GetMainAppData()->mDlgData;

	if(current_dlg_type() != ESEDlgNone) return EBADSTATE;

	//backuped screen
	_dlg_backup_screen();

	_dlg_draw_common();

	//label
	IDisplay_SetColor(BFF_DISPLAY(),CLR_USER_BACKGROUND,dlgData->mBackgroundColor);
	IDisplay_SetColor(BFF_DISPLAY(),CLR_USER_TEXT,dlgData->mFontColor);
	if(dlgData->pLabel)
		ISTATIC_Release(dlgData->pLabel);
	ISHELL_CreateInstance(BFF_SHELL(), AEECLSID_STATIC, (void **)&dlgData->pLabel);
	ISTATIC_SetRect(dlgData->pLabel,&dlgData->mLabelRect);
	ISTATIC_SetText(dlgData->pLabel,NULL,content,AEE_FONT_NORMAL,AEE_FONT_NORMAL);
	ISTATIC_Redraw(dlgData->pLabel);

	//icon
	_dlg_draw_icon(ESEDlgQuery);
	drawCMDLine(IDS_CMD_OK,IDS_CMD_CANCEL);
	dlgData->mType = ESEDlgQuery;
	dlgData->pcbLSK = cbLSK;
	dlgData->pcbRSK = cbRSK;
	return SUCCESS;
}

//LSK: Ok
int error_dlg_run(AECHAR* content,BFFDialogCallbackFunc* cbLSK)
{
	BFFDlgData* dlgData = &GetMainAppData()->mDlgData;

	if(current_dlg_type() != ESEDlgNone) return EBADSTATE;

	//backuped screen
	_dlg_backup_screen();

	_dlg_draw_common();

		//label
	IDisplay_SetColor(BFF_DISPLAY(),CLR_USER_BACKGROUND,dlgData->mBackgroundColor);
	IDisplay_SetColor(BFF_DISPLAY(),CLR_USER_TEXT,dlgData->mFontColor);
	if(dlgData->pLabel)
		ISTATIC_Release(dlgData->pLabel);
	ISHELL_CreateInstance(BFF_SHELL(), AEECLSID_STATIC, (void **)&dlgData->pLabel);
	ISTATIC_SetRect(dlgData->pLabel,&dlgData->mLabelRect);
	ISTATIC_SetText(dlgData->pLabel,NULL,content,AEE_FONT_NORMAL,AEE_FONT_NORMAL);
	ISTATIC_Redraw(dlgData->pLabel);

	//icon
	_dlg_draw_icon(ESEDlgError);
	drawCMDLine(IDS_CMD_OK,0);
	dlgData->mType = ESEDlgError;
	dlgData->pcbLSK = cbLSK;
	dlgData->pcbRSK = NULL;
	return SUCCESS;
}

//LSK: Ok
int note_dlg_run(AECHAR* content,BFFDialogCallbackFunc* cbLSK)
{
	BFFDlgData* dlgData = &GetMainAppData()->mDlgData;

	if(current_dlg_type() != ESEDlgNone) return EBADSTATE;

	//backuped screen
	_dlg_backup_screen();

	_dlg_draw_common();

	//label
	IDisplay_SetColor(BFF_DISPLAY(),CLR_USER_BACKGROUND,dlgData->mBackgroundColor);
	IDisplay_SetColor(BFF_DISPLAY(),CLR_USER_TEXT,dlgData->mFontColor);
	if(dlgData->pLabel)
		ISTATIC_Release(dlgData->pLabel);
	ISHELL_CreateInstance(BFF_SHELL(), AEECLSID_STATIC, (void **)&dlgData->pLabel);
	ISTATIC_SetRect(dlgData->pLabel,&dlgData->mLabelRect);
	ISTATIC_SetText(dlgData->pLabel,NULL,content,AEE_FONT_NORMAL,AEE_FONT_NORMAL);
	ISTATIC_Redraw(dlgData->pLabel);
	
	//icon
	_dlg_draw_icon(ESEDlgNote);
	drawCMDLine(IDS_CMD_OK,0);
	dlgData->mType = ESEDlgNote;
	dlgData->pcbLSK = cbLSK;
	dlgData->pcbRSK = NULL;
	return SUCCESS;
}

//RSK: Cancel
int process_dlg_run(AECHAR* content,BFFDialogCallbackFunc* cbRSK)
{
	BFFDlgData* dlgData = &GetMainAppData()->mDlgData;
	
	if(current_dlg_type() != ESEDlgNone) return EBADSTATE;

	//backuped screen
	_dlg_backup_screen();

	_dlg_draw_common();

	//draw label
	IDisplay_SetColor(BFF_DISPLAY(),CLR_USER_BACKGROUND,dlgData->mBackgroundColor);
	IDisplay_SetColor(BFF_DISPLAY(),CLR_USER_TEXT,dlgData->mFontColor);
	if(dlgData->pLabel)
		ISTATIC_Release(dlgData->pLabel);
	ISHELL_CreateInstance(BFF_SHELL(), AEECLSID_STATIC, (void **)&dlgData->pLabel);
	ISTATIC_SetRect(dlgData->pLabel,&dlgData->mLabelRect);
	ISTATIC_SetText(dlgData->pLabel,NULL,content,AEE_FONT_NORMAL,AEE_FONT_NORMAL);
	ISTATIC_Redraw(dlgData->pLabel);

	//draw icon
	_dlg_draw_icon(ESEDlgProcess);

	//draw process bar
	dlgData->mProcessPercent = 20;
	_dlg_draw_process(dlgData);

	//CBA
	if(cbRSK != NULL)
		drawCMDLine(0,IDS_CMD_CANCEL);
	else
		drawCMDLine(0,0);
	dlgData->mType = ESEDlgProcess;
	dlgData->pcbLSK = NULL;
	dlgData->pcbRSK = cbRSK;
	return SUCCESS;
}

int dismiss_current_dlg()
{
	BFFDlgData* dlgData = &GetMainAppData()->mDlgData;
	IDisplay* display = BFF_DISPLAY();
	if(dlgData->mType == ESEDlgNone) return EBADSTATE;

	if(dlgData->pLabel)
	{
		ISTATIC_Release(dlgData->pLabel);
		dlgData->pLabel = NULL;
	}
	switch(dlgData->mType)
	{
	case ESEDlgProcess:
		{
			ISHELL_CancelTimer(BFF_SHELL(),(PFNNOTIFY)_dlg_draw_process,dlgData);
			break;
		}
	default:break;
	}
	dlgData->mType = ESEDlgNone;

	//recover backuped screen
	if(dlgData->pScreenBackup)
	{
		IDISPLAY_ClearScreen(display);
		IDISPLAY_BitBlt(display,0,0,getDeviceWidth(),getDeviceHeight(),dlgData->pScreenBackup,0,0,AEE_RO_COPY);
		IDISPLAY_Update(display);
		IBITMAP_Release(dlgData->pScreenBackup);
		dlgData->pScreenBackup = NULL;
	}
	
	return SUCCESS;
}

BFFDlgType current_dlg_type()
{
	BFFDlgData* dlgData = &GetMainAppData()->mDlgData;
	return dlgData->mType;
}