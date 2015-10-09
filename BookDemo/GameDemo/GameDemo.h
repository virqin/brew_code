#ifndef GAMEDEMO_H
#define GAMEDEMO_H

#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEFile.h"			// File interface definitions
#include "AEESound.h"			// Sound Interface definitions
#include "GameDemo.bid"
#include "GameDemo_res.h"
#include "AEEStdLib.h"
#include "AEESprite.h"
#include "AEEMedia.h"


#define RES_FILE GAMEDEMO_RES_FILE 
#define MOVESPEED 5
#define BULLETSPEED 10

typedef enum _GameState
{
	enMainMenu,
	enMainGame,
}GameState;
typedef enum _MenuState
{
	enMenuMain,
	enMenuHelp,
}MenuState;

typedef struct _Menu
{
	uint32 m_iKeyState;
	int m_nWhichActive;
	MenuState m_State;
	boolean m_bUpdate;
}Menu;

typedef struct _MainSprite
{
	ISprite *m_pISprite;
	AEESpriteCmd m_sprite[2];
	uint32 m_iKeyState;
	int dy;
	int offset;
	boolean m_bIsLeft;
}MainSprite;

typedef struct _SpriteProp
{
	int cnt;
	int offset;
	int originX;
	int interval;
	boolean isLeft;
}SpriteProp;

typedef struct tagMap
{
	int m_nX;//地图的左上角的x坐标
	int m_nY;//地图的左上角的y坐标
	int m_nWidth;//地图的宽度，以tile的数目计算
	int m_nHeight;//地图的高度，以tile的数目计算
	int m_nTileSize;//地图的tile的大小，?X?的，必须是整倍数，两个??相等
	IBitmap *m_pIBmp;//地图的图片
	uint16 *m_pMapArray;//地图矩阵
}Map;
typedef struct _MainGame
{
	ISprite *m_pISprite;
	AEETileMap m_Maps[2];
	AEESpriteCmd m_MoveSprites[10];
	AEESpriteCmd m_StaticSprites[10];
	SpriteProp m_MoveSpriteProp[10];
	SpriteProp m_StaticSpriteProp[10];
	Map m_MidMap;
	MainSprite m_MainSprite;
	int bgdx;
	int bgdy;
}MainGame;

typedef struct _GameDemo 
{
	AEEApplet a;
	AEEDeviceInfo DeviceInfo;
	IDisplay *pIDisplay;
	IShell *pIShell;
	AEECallback m_cbMainTimer;//主定时器
	int16 m_nFontHeight;//系统的字体高度
	GameState m_State;
	Menu m_Menu;
	boolean m_bVolOn;
	MainGame m_Game;
	IMedia *m_pIMedia;
	boolean m_bIsContinue;
	boolean m_bIsSave;
}GameDemo;



#endif