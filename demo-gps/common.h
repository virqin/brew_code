/** 
***     Common.h 
*** 
***     画面描画用 
***     Copyright (C) 2006 SophiaCradle Incorporated. 
***     All rights reserved. 
**/ 

#ifndef __COMMON_H 
#define __COMMON_H 

#include "AEEStdLib.h" 
#include "AEEShell.h" 

typedef struct { 
    IDisplay* display; 
    int width; 
    int height; 
    int start_line; 
    int lines; 
    int font_height; 
	
    AECHAR* message; 
    int size; 
} Common; 

// 初期化 
void COMMON_Init(Common* common, IDisplay* display, IShell* shell, int buffer_size); 
// 解放 
void COMMON_Free(Common* common); 
// 画面描画 
void COMMON_Draw(Common* common); 
// 文字列バッファをクリア 
void COMMON_ClearBuffer(Common* common); 
// 文字列を追加 
void COMMON_WriteString(Common* common, const char* str); 
// AECHAR文字列を追加 
void COMMON_WriteWideString(Common* common, const AECHAR* wstr); 
// 表示行を次の行に 
void COMMON_NextLine(Common* common); 
// 表示行を前の行に 
void COMMON_PreviousLine(Common* common); 

#endif 







