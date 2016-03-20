#ifndef SCENEUTILS_H
#define SCENEUTILS_H

#include "framework.h"
#include "../source.brh"



//FOR DREW IMAGE
#define IDF_TOP           1
#define IDF_DOWN          2
#define IDF_VCENTER       4

#define IDF_LEFT          8
#define IDF_RIGHT         16
#define IDF_HCENTER       32

//FOR DREW STRING
#define IDS_TOP           1
#define IDS_DOWN          2

#define IDS_LEFT          4
#define IDS_RIGHT         8


#define DEVICE_HEIGHT  getDeviceHeight()  //屏幕分辨率高度
#define DEVICE_WIDTH   getDeviceWidth()   //屏幕分辨率宽度

#define AVK_LEFT_SOFT_KEY    AVK_SOFT1    //左软键宏定义
#define AVK_RIGHT_SOFT_KEY   AVK_SOFT2    //右软键宏定义


#define DRAW_CANVAS_CONSTANT 4
#define SCROLL_BAR_SIZE      20

#define GET_FONT_HEIGHT(font)  IDISPLAY_GetFontMetrics(((AEEApplet*) GETAPPINSTANCE() )->m_pIDisplay, font, NULL, NULL)

#define CMD_LINE_HEIGHT (GET_FONT_HEIGHT(AEE_FONT_LARGE)+DRAW_CANVAS_CONSTANT)



typedef struct _ListItem {
	int16 ImageID;
	AECHAR Text[32];
} ListItem;

typedef enum _ListBehavior
{
	SELECT    = 0,
	MOVE_UP   = 1,
	MOVE_DOWN = 2,
	DO_NOTHING= 3
} ListBehavior;

typedef struct _List {
	AEERect   ListRect;
	AEERect   listDrawRect;
	AEEFont   Font;
	RGBVAL    BGColor;
	RGBVAL    TextColor;
	RGBVAL    S_BGColor;
	RGBVAL    S_TextColor;
	int       SelectedID;
	int       ListCount;
	int       ItemHeight; 
	boolean   haveICON;
	boolean   haveScrollBar;
	ListItem* listItem;
} List;



void drawString(const AECHAR *string,int x,int y,
				RGBVAL backgroundColor,RGBVAL fontColor,AEEFont font,int situation);
void drawImage(IImage * pImage, int x,int y, int situation);
void drawCMDLine(int16 leftStringID,int16 rightStringID);
AVKType cmdLine_onPointerDown(int16 x, int16 y);

//获取设备屏幕高度
uint16 getDeviceHeight(void);
//获取设备屏幕宽度
uint16 getDeviceWidth(void);


#endif