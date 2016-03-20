 
/**  
***     Common.c  
***  
***     画面描画用  
***     Copyright (C) 2006 SophiaCradle Incorporated.  
***     All rights reserved.  
**/   
   
#include "Common.h"   
   
void COMMON_Init(Common* common, IDisplay* display, IShell* shell, int buffer_size)   
{   
    AEEDeviceInfo devinfo;   
   
    common->display = display;   
    common->size = buffer_size;   
    common->start_line = 0;   
    common->lines = 0;   
    common->message = (AECHAR*)MALLOC(buffer_size * sizeof(AECHAR));   
    common->message[0] = 0;   
    common->message[buffer_size - 1] = 0;   
   
    // 画面サイズを取得   
    ISHELL_GetDeviceInfo(shell, &devinfo);   
    common->width = devinfo.cxScreen;   
    common->height = devinfo.cyScreen;   
   
    // フォント情報を取得   
    common->font_height = IDISPLAY_GetFontMetrics(common->display, AEE_FONT_NORMAL, NULL, NULL);   
    return;   
}   
   
void COMMON_Free(Common* common)   
{   
    FREE(common->message);   
    return;   
}   
   
void COMMON_Draw(Common* common)   
{   
    IDisplay* d = common->display;   
    int i = 0;   
    int j = 0;   
    int len = WSTRLEN(common->message);   
   
    IDISPLAY_ClearScreen(d);   
    common->lines = 0;   
    while ((i < len - 1) && ((common->lines - common->start_line) * common->font_height < common->height)) {   
        AECHAR t;   
        int l;   
   
        IDISPLAY_MeasureTextEx(d, AEE_FONT_NORMAL, common->message + i, len - i, common->width, &j);   
        for (l = 0; l < j; l++) {   
            if (common->message[i + l] == 0x0D || common->message[i + l] == '\n') {   
                j = l;   
                break;   
            }   
        }   
        t = common->message[i + j];   
        common->message[i + j] = 0;   
        if (common->start_line <= common->lines) {   
            IDISPLAY_DrawText(d, AEE_FONT_NORMAL, common->message + i, -1, 0, (common->lines - common->start_line) * common->font_height, NULL, IDF_ALIGN_LEFT);   
        }   
        common->message[i + j] = t;   
        i += j;   
        if (t == 0x0D) {   
            i += 2;   
        }   
        if (t == '\n') {   
            i++;   
        }   
        common->lines++;   
    }   
    IDISPLAY_Update(d);   
    return;   
}   
   
void COMMON_ClearBuffer(Common* common)   
{   
    common->message[0] = 0;   
    return;   
}   
   
void COMMON_WriteString(Common* common, const char* str)   
{   
    int size = (STRLEN(str) + 1) * sizeof(AECHAR);   
    AECHAR* wstr = (AECHAR*)MALLOC(size);   
   
    STRTOWSTR(str, wstr, size);   
    WSTRLCAT(common->message, wstr, common->size * sizeof(AECHAR));   
    FREE(wstr);   
    return;   
}   
   
void COMMON_WriteWideString(Common* common, const AECHAR* wstr)   
{   
    WSTRLCAT(common->message, wstr, common->size * sizeof(AECHAR));   
    return;   
}   
   
void COMMON_NextLine(Common* common)   
{   
    common->start_line++;   
    if (common->start_line > common->lines - (common->height / common->font_height)) {   
        common->start_line--;   
    }   
    return;   
}   
   
void COMMON_PreviousLine(Common* common)   
{   
    common->start_line--;   
    if (common->start_line < 0) {   
        common->start_line = 0;   
    }   
    return;   
}   







