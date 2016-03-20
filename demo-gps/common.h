/** 
***     Common.h 
*** 
***     ��ʕ`��p 
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

// ������ 
void COMMON_Init(Common* common, IDisplay* display, IShell* shell, int buffer_size); 
// ��� 
void COMMON_Free(Common* common); 
// ��ʕ`�� 
void COMMON_Draw(Common* common); 
// ������o�b�t�@���N���A 
void COMMON_ClearBuffer(Common* common); 
// �������ǉ� 
void COMMON_WriteString(Common* common, const char* str); 
// AECHAR�������ǉ� 
void COMMON_WriteWideString(Common* common, const AECHAR* wstr); 
// �\���s�����̍s�� 
void COMMON_NextLine(Common* common); 
// �\���s��O�̍s�� 
void COMMON_PreviousLine(Common* common); 

#endif 







