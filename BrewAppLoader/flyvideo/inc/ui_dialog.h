/**
*	file: dialog.h
*	author: yuleiming
*/
#ifndef _DIALOG_H_
#define _DIALOG_H_

#include "framework.h"

#define KLabelMaxLength 32
#define KHeightPercentOfScreen 5/10
#define KProcessBarHeightInPixels 15
#define KProcessBarWidthPercentOfDlg 8/10
#define KProcessBarOffsetYPercent 5/10
#define KLabelOffsetYPercent 1/6
#define KProcessPercentInterval 20	//increase 20% every time
#define KProcessTimerInterval 500	//500 ms
#define KIconWidthInPixel 30	//icon.width == icon.height
#define KGapBetweenLabelAndIconInPixel 2

typedef void BFFDialogCallbackFunc(void);

typedef enum _bff_dlg_type
{
	ESEDlgNone,	//no dialog running
	ESEDlgNote,	//Ok-empty
	ESEDlgQuery,//Ok-Cancel
	ESEDlgProcess,//Process dialog
	ESEDlgError//OK-ERROR
}	BFFDlgType;	//SmartEyeDlgType

typedef struct _bff_dlg_data
{
	BFFDlgType mType;
	AEERect mRect;
	AEERect mLabelRect;
	AEERect mProcessBarRect;
	AEERect mIconRect;
	RGBVAL mBackgroundColor;
	RGBVAL mBorderColor;
	RGBVAL mFontColor;
	RGBVAL mProcessBarColor;
	int mProcessPercent;
	IStatic* pLabel;
	BFFDialogCallbackFunc* pcbLSK;	//callback function when LSK pressed
	BFFDialogCallbackFunc* pcbRSK;	//callback function when RSK pressed
	IBitmap* pScreenBackup;
}	BFFDlgData;

/*never call these functions*/
int se_dlg_init(void);
void se_dlg_finalize(void);
void se_dlg_hit_LSK(void);
void se_dlg_hit_RSK(void);


/*New functions:*/
//LSK: Ok	RSK: Cancel
int query_dlg_run( AECHAR* content,BFFDialogCallbackFunc* cbLSK,BFFDialogCallbackFunc* cbRSK);
//LSK: Ok
int note_dlg_run( AECHAR* content,BFFDialogCallbackFunc* cbLSK);
//RSK: Cancel
int process_dlg_run( AECHAR* content,BFFDialogCallbackFunc* cbRSK);
//LSK: Ok
int error_dlg_run(AECHAR* content,BFFDialogCallbackFunc* cbLSK);
//LSK: Ok	RSL: Empty
int error_dlg_run_with_error_code(int errCode);

int dismiss_current_dlg(void);
BFFDlgType current_dlg_type(void);

#endif /*_DIALOG_H_*/