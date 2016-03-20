  
/**  
***     GPSApp.c  
***  
***     このソースコードは、エラー処理やストップ＆サスペンドなどの処理が抜けています。  
***     Copyright (C) 2006 SophiaCradle Incorporated.  
***     All rights reserved.  
**/   
   
#include "AEEModGen.h"   
#include "AEEAppGen.h"   
#include "AEEStdLib.h"   
#include "AEEShell.h"   
#include "Common.h"   
#include "AEEPosDet.h"   
   
typedef struct GPSApp {   
    AEEApplet a; // アプレット構造体の先頭メンバは必ず AEEApplet 型にすること。   
    Common common; //描画用   
   
    IPosDet* posdet;   
    AEEGPSInfo gpsinfo; //  AEEGPSInfo 構造体の変数 ( GPS から取得したデータを保存)   
    AEECallback callback;   
} GPSApp;   
   
static boolean GPSApp_HandleEvent(GPSApp* app, AEEEvent eCode, uint16 wParam, uint32 dwParam);   
static boolean OnAppStart(GPSApp* app);   
static boolean OnAppStop(GPSApp* app);   
static boolean OnKey(GPSApp* app, uint16 key);   
static void GetGPS(GPSApp* app);   
static void OnGPSNotify(GPSApp* app);   
   
int gps_AEEClsCreateInstance(AEECLSID ClsId, IShell* pIShell, IModule* po, void** ppObj)   
{   
    *ppObj = NULL;   
//    if (ClsId == AEECLSID_GPSAPP) 
	{   
        if (AEEApplet_New(sizeof(GPSApp), ClsId, pIShell, po, (IApplet**)ppObj, (AEEHANDLER)GPSApp_HandleEvent, NULL) == TRUE) {   
            return AEE_SUCCESS;   
        }   
    }   
    return EFAILED;   
}   
   
static boolean GPSApp_HandleEvent(GPSApp* app, AEEEvent eCode, uint16 wParam, uint32 dwParam)   
{   
    switch (eCode) {   
        case EVT_APP_START:   
            return OnAppStart(app);   
   
        case EVT_APP_STOP:   
            return OnAppStop(app);   

		case EVT_PEN_DOWN:
			if (AEE_GET_Y(dwParam) > 150)
			{
				COMMON_NextLine(&app->common);   
				COMMON_Draw(&app->common);   
			}
			else
			{
				COMMON_PreviousLine(&app->common);   
				COMMON_Draw(&app->common);   
			}
			return TRUE;

        case EVT_KEY:
			if (wParam == AVK_SEND)
			{
				GetGPS(app);   
				return TRUE;
			}
    }   
    return FALSE;   
}   
   
// アプレットが開始したときに呼び出される。   
static boolean OnAppStart(GPSApp* app)   
{   
    IDisplay* display = app->a.m_pIDisplay;   
    IShell* shell = app->a.m_pIShell;   
   
    // IPosDet インタフェースのインスタンス化   
    app->posdet = NULL;   
    ISHELL_CreateInstance(app->a.m_pIShell, AEECLSID_POSDET, (void**)&app->posdet);   
   
    COMMON_Init(&app->common, display, shell, 32768);   
    COMMON_WriteString(&app->common, "Push Select Key to get GPS information\n");   
    COMMON_Draw(&app->common);   
    return TRUE;   
}   
   
// アプレットが終了したときに呼び出される。   
static boolean OnAppStop(GPSApp* app)   
{   
    CALLBACK_Cancel(&app->callback);   
    IPOSDET_Release(app->posdet);   
    COMMON_Free(&app->common);   
    return TRUE;   
}   
   
// キーが押されたときに呼び出される。   
static boolean OnKey(GPSApp* app, uint16 key)   
{   
    switch (key) {   
        case AVK_SEND:   
            GetGPS(app);   
            return TRUE;   
   
        case AVK_DOWN:   
            COMMON_NextLine(&app->common);   
            COMMON_Draw(&app->common);   
            return TRUE;   
   
        case AVK_UP:   
            COMMON_PreviousLine(&app->common);   
            COMMON_Draw(&app->common);   
            return TRUE;   
    }   
    return FALSE;   
}   
   
// 位置情報の取得   
static void GetGPS(GPSApp* app)   
{   
    int ret;   
    char buffer[80];   
   
    // コールバック関数の設定   
    CALLBACK_Init(&app->callback, (PFNNOTIFY)OnGPSNotify, (void*)app);   
    
	// 位置情報の取得   
    ret = IPOSDET_GetGPSInfo(app->posdet, AEEGPS_GETINFO_LOCATION, AEEGPS_ACCURACY_HIGHEST, 
		&app->gpsinfo, 
		&app->callback);   

    // 上記関数の戻り値をチェック   
    switch (ret) {   
        case EPRIVLEVEL:   
            STRCPY(buffer, "EPRIVLEVEL");   
            break;   
   
        case EBADPARM:   
            STRCPY(buffer, "EBADPARM");   
            break;   
   
        case EUNSUPPORTED:   
            STRCPY(buffer, "EUNSUPPORTED");   
            break;   
   
        case EFAILED:   
            STRCPY(buffer, "EFAILED");   
            break;   
   
        case SUCCESS:   
            STRCPY(buffer, "SUCCESS");   
            break;   
   
        default:   
            STRCPY(buffer, "DEFAULT");   
            break;   
    }   
    // 失敗していたらコールバックをキャンセル   
    if (ret != SUCCESS) {   
        CALLBACK_Cancel(&app->callback);   
    }   
   
    // 位置情報取得関数の戻り値を出力   
    COMMON_WriteString(&app->common, buffer);   
    COMMON_WriteString(&app->common, "\n");   
    COMMON_Draw(&app->common);   
    return;   
}   
   
// 位置情報を取得後の処理   
static void OnGPSNotify(GPSApp* app)   
{   
    char szLat[32];   
    char szLon[32];   
    char szBuf[64];   
    char latchar[64];   
    char lonchar[64];   
    AECHAR latwchar[64];   
    AECHAR lonwchar[64];   
    double lat;   
    double lon;   
   
    lat = WGS84_TO_DEGREES(app->gpsinfo.dwLat);   
    lon = WGS84_TO_DEGREES(app->gpsinfo.dwLon);   
    FLOATTOWSTR(lat, latwchar, 64);   
    FLOATTOWSTR(lon, lonwchar, 64);   
    WSTRTOSTR(latwchar, latchar, 64);   
    WSTRTOSTR(lonwchar, lonchar, 64);   
   
    // 緯度の取得   
    SPRINTF(szLat, "Latitude = %d", app->gpsinfo.dwLat);   
    // 経度の取得   
    SPRINTF(szLon, "Longitude = %d", app->gpsinfo.dwLon);   
    // 緯度の描画   
    COMMON_WriteString(&app->common, szLat);   
    COMMON_WriteString(&app->common, "\n");   
    COMMON_WriteString(&app->common, latchar);   
    COMMON_WriteString(&app->common, "\n");   
    // 経度の描画   
    COMMON_WriteString(&app->common, szLon);   
    COMMON_WriteString(&app->common, "\n");   
    COMMON_WriteString(&app->common, lonchar);   
    COMMON_WriteString(&app->common, "\n");   
    // レスポンスのエラーチェック   
    switch (app->gpsinfo.status) {   
        case AEEGPS_ERR_NO_ERR:   
            STRCPY(szBuf, "SUCCESS !");   
            break;   
   
        case AEEGPS_ERR_GENERAL_FAILURE:   
            STRCPY(szBuf, "AEEGPS_ERR_GENERAL_FAILURE");   
            break;   
   
        case AEEGPS_ERR_TIMEOUT:   
            STRCPY(szBuf, "AEEGPS_ERR_TIMEOUT");   
            break;   
   
        case AEEGPS_ERR_ACCURACY_UNAVAIL:   
            STRCPY(szBuf, "AEEGPS_ERR_ACCURACY_UNAVAIL");   
            break;   
   
        case AEEGPS_ERR_INFO_UNAVAIL:   
            STRCPY(szBuf, "AEEGPS_ERR_INFO_UNAVAIL");   
            break;   
   
        default:   
            STRCPY(szBuf, "DEFAULT");   
            break;   
    }   
    // レスポンスのステータスを描画   
    COMMON_WriteString(&app->common, szBuf);   
    COMMON_WriteString(&app->common, "\n");   
    COMMON_Draw(&app->common);   
    return;   
}   







