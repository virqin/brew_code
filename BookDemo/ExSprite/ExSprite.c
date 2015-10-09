/*===========================================================================

FILE: ExSprite.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEESprite.h"
#include "AEEStdLib.h"

#include "ExSprite.bid"

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/

typedef struct _Sprite
{
	int x;
	int y;
	int index;
	int size;
}Sprite;
typedef struct _Map
{
	int m_nX;//地图的左上角的x坐标
	int m_nY;//地图的左上角的y坐标
	int m_nWidth;//地图的宽度，以tile的数目计算
	int m_nHeight;//地图的高度，以tile的数目计算
	int m_nTileSize;//地图的tile的大小
	IBitmap *m_pIBmp;//地图的图片
	uint16 *m_pMapArray;//地图矩阵
}Map;
typedef struct _ExSprite {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface

    // add your own variables here...
		ISprite *pISprite;
		AEECallback cbMainTimer;
		AEETileMap m_Maps[2];
		AEESpriteCmd m_Sprite[2];
		IBitmap *pBmpMap;
		Map map;
		IBitmap *pBmpSprite;
		
		Sprite sprite;

} ExSprite;
static const uint16 mapData[] = {
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0002,0x0002,0x0002,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0002,0x0002,0x0002,0x0002,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0002,0x0002,0x0002,0x0000,0x0000,0x000d|TILE_ROTATE_90,0x000f|TILE_ROTATE_90,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x000e|TILE_ROTATE_90,0x0010|TILE_ROTATE_90,0x0000,0x0000|TILE_ROTATE_90,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0002,0x0002,0x0002,0x0002,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0007,0x000a,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0008,0x0009,0x000b,0x000c,0x0000,
0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,
0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,0x0001,
};




/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean ExSprite_HandleEvent(ExSprite* pMe, 
                                                   AEEEvent eCode, uint16 wParam, 
                                                   uint32 dwParam);
boolean ExSprite_InitAppData(ExSprite* pMe);
void    ExSprite_FreeAppData(ExSprite* pMe);
void ExSprite_LoadResource(ExSprite *pMe);
void ExSprite_FreeResoure(ExSprite *pMe);
static void MainTimerCb(ExSprite *pMe);
static void MainAppDrawMidMap(ExSprite *pMe,Map *pMidMap,boolean isTransparent);
/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;

	if( ClsId == AEECLSID_EXSPRITE )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(ExSprite),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)ExSprite_HandleEvent,
                          (PFNFREEAPPDATA)ExSprite_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(ExSprite_InitAppData((ExSprite*)*ppObj))
			{
				//Data initialized successfully
				return(AEE_SUCCESS);
			}
			else
			{
				//Release the applet. This will free the memory allocated for the applet when
				// AEEApplet_New was called.
				IAPPLET_Release((IApplet*)*ppObj);
				return EFAILED;
			}

        } // end AEEApplet_New

    }

	return(EFAILED);
}


/*===========================================================================
FUNCTION SampleAppWizard_HandleEvent
===========================================================================*/
static boolean ExSprite_HandleEvent(ExSprite* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  

    switch (eCode) 
	{
        // App is told it is starting up
        case EVT_APP_START:                        
		    // Add your code here...
					//ISHELL_SetTimerEx(pMe->pIShell,0,&pMe->cbMainTimer);
					//ISPRITE_DrawTiles(pMe->pISprite,pMe->m_Maps);
					//ISPRITE_DrawSprites(pMe->pISprite,pMe->m_Sprite);
					MainAppDrawMidMap(pMe,&pMe->map,FALSE);
					IDISPLAY_Update(pMe->pIDisplay);
					IDISPLAY_BitBlt(pMe->pIDisplay,pMe->sprite.x,pMe->sprite.y,pMe->sprite.size,pMe->sprite.size,pMe->pBmpSprite,0,pMe->sprite.index*pMe->sprite.size,AEE_RO_TRANSPARENT);
					IDISPLAY_Update(pMe->pIDisplay);

            return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...

      		return(TRUE);


        // App is being suspended 
        case EVT_APP_SUSPEND:
		    // Add your code here...

      		return(TRUE);


        // App is being resumed
        case EVT_APP_RESUME:
		    // Add your code here...

      		return(TRUE);


        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
		    // Add your code here...

      		return(TRUE);

        // A key was pressed. Look at the wParam above to see which key was pressed. The key
        // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
        case EVT_KEY:
		    // Add your code here...

      		return(TRUE);


        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}


// this function is called when your application is starting up
boolean ExSprite_InitAppData(ExSprite* pMe)
{
    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);
    pMe->pIDisplay = pMe->a.m_pIDisplay;
    pMe->pIShell   = pMe->a.m_pIShell;
		

		/*
		
		pMe->m_Sprite[0].unComposite = COMPOSITE_KEYCOLOR;
		pMe->m_Sprite[0].unLayer = SPRITE_LAYER_0;
		pMe->m_Sprite[0].unSpriteIndex = 0;
		pMe->m_Sprite[0].unSpriteSize = SPRITE_SIZE_32X32;
		pMe->m_Sprite[0].unTransform = SPRITE_ROTATE_90;
		pMe->m_Sprite[0].x = 0;
		pMe->m_Sprite[0].y = pMe->DeviceInfo.cyScreen-64;
		pMe->m_Sprite[1].unSpriteSize = SPRITE_SIZE_END;
		pMe->m_Maps[0].pMapArray = (uint16*)map;
		pMe->m_Maps[0].unFlags = MAP_FLAG_WRAP;
		pMe->m_Maps[0].unTileSize = TILE_SIZE_16X16;
		pMe->m_Maps[0].h = MAP_SIZE_16;
		pMe->m_Maps[0].w = MAP_SIZE_16;
		pMe->m_Maps[0].y= pMe->DeviceInfo.cyScreen-256;
		pMe->m_Maps[0].x = 50;
		*/
		ExSprite_LoadResource(pMe);
		pMe->map.m_pMapArray = (uint16*)mapData;
		pMe->map.m_nHeight = 16;
		pMe->map.m_nWidth = 16;
		pMe->map.m_nTileSize = 16;
		pMe->map.m_pIBmp = pMe->pBmpMap;
		pMe->map.m_nX = 0;
		pMe->map.m_nY = 256-pMe->DeviceInfo.cyScreen;
		pMe->sprite.x = 0;
		pMe->sprite.y = pMe->DeviceInfo.cyScreen - 64;
		pMe->sprite.size = 32;
		pMe->sprite.index = 0;

    return TRUE;
}
static void MainTimerCb(ExSprite *pMe)
{
	//ISHELL_SetTimerEx(pMe->pIShell,80,&pMe->cbMainTimer);
	//ISPRITE_DrawTiles(pMe->pISprite,pMe->m_Maps);
	//ISPRITE_DrawSprites(pMe->pISprite,pMe->m_Sprite);
	//IDISPLAY_Update(pMe->pIDisplay);
}

// this function is called when your application is exiting
void ExSprite_FreeAppData(ExSprite* pMe)
{
	ExSprite_FreeResoure(pMe);
}
void ExSprite_LoadResource(ExSprite *pMe)
{
	IBitmap       *pbmScreen = NULL;
	IBitmap       *pbmDib = NULL;
	NativeColor    color;
	AEEBitmapInfo  bi;
	pbmScreen = IDISPLAY_GetDestination(pMe->pIDisplay);
	pbmDib = ISHELL_LoadBitmap(pMe->pIShell,"sprite32.bmp");
	IBITMAP_GetInfo(pbmDib, &bi, sizeof(bi));
	IBITMAP_CreateCompatibleBitmap(pbmScreen, &pMe->pBmpSprite, (uint16)bi.cx, (uint16)bi.cy);
	IBITMAP_BltIn(pMe->pBmpSprite, 0, 0, (uint16)bi.cx, (uint16)bi.cy, pbmDib, 0, 0, AEE_RO_COPY);
	IBITMAP_Release(pbmDib);
	pbmDib = NULL;
	IBITMAP_GetPixel(pMe->pBmpSprite, 0, 0, &color);
	IBITMAP_SetTransparencyColor(pMe->pBmpSprite, color);

	pbmDib = ISHELL_LoadBitmap(pMe->pIShell,"bg16.bmp");
	IBITMAP_GetInfo(pbmDib, &bi, sizeof(bi));
	IBITMAP_CreateCompatibleBitmap(pbmScreen, &pMe->pBmpMap, (uint16)bi.cx, (uint16)bi.cy);
	IBITMAP_BltIn(pMe->pBmpMap, 0, 0, (uint16)bi.cx, (uint16)bi.cy, pbmDib, 0, 0, AEE_RO_COPY);
	IBITMAP_Release(pbmDib);
	pbmDib = NULL;
	IBITMAP_GetPixel(pMe->pBmpMap, 0, 0, &color);
	IBITMAP_SetTransparencyColor(pMe->pBmpMap, color);
	IBITMAP_Release(pbmScreen);

}

void ExSprite_FreeResoure(ExSprite *pMe)
{
	//ISPRITE_Release(pMe->pISprite);
	IBITMAP_Release(pMe->pBmpMap);
	IBITMAP_Release(pMe->pBmpSprite);
}


static void MainAppDrawMidMap(ExSprite *pMe,Map *pMidMap,boolean isTransparent)
{
	int beginX = 0;
	int beginY = 0;
	int nX;
	int nY;
	int tempY;
	int tempX;
	int arrayNum;

	nX=pMidMap->m_nX%pMidMap->m_nTileSize;
	nY=pMidMap->m_nY%pMidMap->m_nTileSize;

	while(TRUE)
	{
		while(TRUE)
		{
			tempY=((beginY+pMidMap->m_nY)/pMidMap->m_nTileSize)%pMidMap->m_nHeight;
			tempX=((beginX+pMidMap->m_nX)/pMidMap->m_nTileSize)%pMidMap->m_nWidth;
			arrayNum=	pMidMap->m_pMapArray[tempX+tempY*pMidMap->m_nWidth]&(0x00FF);
			if(arrayNum!=0xff)
			{
				if(isTransparent)
				{
					IDISPLAY_BitBlt(pMe->pIDisplay,
						beginX-nX,beginY-nY,
						pMidMap->m_nTileSize,pMidMap->m_nTileSize,
						pMidMap->m_pIBmp,
						0,pMidMap->m_nTileSize*arrayNum,
						AEE_RO_TRANSPARENT);
				}
				else
				{
					IDISPLAY_BitBlt(pMe->pIDisplay,
						beginX-nX,beginY-nY,
						pMidMap->m_nTileSize,pMidMap->m_nTileSize,
						pMidMap->m_pIBmp,
						0,pMidMap->m_nTileSize*arrayNum,
						AEE_RO_COPY);
				}

			}
			if(beginX<pMe->DeviceInfo.cxScreen)
			{
				beginX+=pMidMap->m_nTileSize;
			}
			else
			{
				break;
			}
		}
		if(beginY<pMe->DeviceInfo.cyScreen)
		{
			beginY+=pMidMap->m_nTileSize;
			beginX=0;
		}
		else
		{
			break;
		}
	}
}