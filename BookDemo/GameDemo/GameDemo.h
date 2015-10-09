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
	int m_nX;//��ͼ�����Ͻǵ�x����
	int m_nY;//��ͼ�����Ͻǵ�y����
	int m_nWidth;//��ͼ�Ŀ�ȣ���tile����Ŀ����
	int m_nHeight;//��ͼ�ĸ߶ȣ���tile����Ŀ����
	int m_nTileSize;//��ͼ��tile�Ĵ�С��?X?�ģ�������������������??���
	IBitmap *m_pIBmp;//��ͼ��ͼƬ
	uint16 *m_pMapArray;//��ͼ����
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
	AEECallback m_cbMainTimer;//����ʱ��
	int16 m_nFontHeight;//ϵͳ������߶�
	GameState m_State;
	Menu m_Menu;
	boolean m_bVolOn;
	MainGame m_Game;
	IMedia *m_pIMedia;
	boolean m_bIsContinue;
	boolean m_bIsSave;
}GameDemo;



#endif