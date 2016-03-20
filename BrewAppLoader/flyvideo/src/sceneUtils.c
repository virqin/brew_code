#include "sceneUtils.h"


void drawString(const AECHAR *string,int x,int y,
				RGBVAL backgroundColor,RGBVAL fontColor,AEEFont font,int situation)
{
	AEERect stringRect;
	IDisplay* pIDisplay = ((AEEApplet*) GETAPPINSTANCE() )->m_pIDisplay;
	int xx = situation&12;
	int yy = situation&3;
	IDISPLAY_SetColor(pIDisplay, CLR_USER_TEXT, fontColor);
	IDISPLAY_SetColor(pIDisplay, CLR_USER_BACKGROUND, backgroundColor);
	stringRect.dx = IDISPLAY_MeasureText(pIDisplay, font,string);
	stringRect.dy = IDISPLAY_GetFontMetrics(pIDisplay,font,NULL,NULL);

	if(xx == IDS_LEFT)
	{
		stringRect.x = x;
	}
	else if(xx == IDS_RIGHT)
	{
		stringRect.x = x-stringRect.dx;
	}

	if(yy == IDS_TOP)
	{
		stringRect.y = y;
	}
	else if(yy == IDS_DOWN)
	{
		stringRect.y = y-stringRect.dy;
	}
	IDISPLAY_DrawText(pIDisplay,font,string,-1,stringRect.x,stringRect.y, &stringRect, NULL);
}


void drawImage(IImage * pImage, int x,int y, int situation)
{
	int xx;
	int yy;

	AEEImageInfo rInfo;
	xx = situation&56;
	yy = situation&7;
	IIMAGE_GetInfo( pImage, &rInfo ); 

	if(xx == IDF_LEFT)
	{
		x=x;
	}
	else if(xx == IDF_RIGHT)
	{
		x = x - rInfo.cx;
	}
	else if(xx == IDF_HCENTER)
	{
		x = x - rInfo.cx/2;
	}

	if(yy == IDF_TOP)
	{
		y=y;
	}
	else if(yy == IDF_DOWN)
	{
		y=y-rInfo.cy;
	}
	else if(yy == IDF_VCENTER)

	{
		y=y-rInfo.cy/2;
	}

	IIMAGE_Start(pImage, x, y);
}


void drawCMDLine(int16 leftStringID,int16 rightStringID)
{
	IDisplay  *pIDisplay = ((AEEApplet*) GETAPPINSTANCE() )->m_pIDisplay;
	AEERect   cmdLineRect;
	AEEDeviceInfo  deviceInfo;
	ISHELL_GetDeviceInfo(((AEEApplet*) GETAPPINSTANCE() )->m_pIShell,&deviceInfo);

	cmdLineRect.x  = 0;
	cmdLineRect.y  = deviceInfo.cyScreen - CMD_LINE_HEIGHT;
	cmdLineRect.dx = deviceInfo.cxScreen;
	cmdLineRect.dy = CMD_LINE_HEIGHT;

	IDISPLAY_DrawRect(pIDisplay,&cmdLineRect,0xFFFFFF00,0xFFFFFF00,IDF_RECT_FILL);

	if(leftStringID != NULL)
	{
		AECHAR    *string = NULL;
		initStringByID(&string,leftStringID);
		drawString(string,2,cmdLineRect.y+2,0xFFFFFF00,0x000000FF,AEE_FONT_LARGE,IDS_TOP|IDS_LEFT);
		FREE(string);
	}
	if(rightStringID != NULL)
	{
		AECHAR    *string = NULL;
		initStringByID(&string,rightStringID);
		drawString(string,cmdLineRect.dx-2,cmdLineRect.y+2,0xFFFFFF00,0x000000FF,AEE_FONT_LARGE,IDS_TOP|IDS_RIGHT);
		FREE(string);
	}
}

AVKType cmdLine_onPointerDown(int16 x, int16 y)
{
	if(y>DEVICE_HEIGHT - CMD_LINE_HEIGHT)
	{
		if(x<DEVICE_WIDTH/2)
		{
			return AVK_LEFT_SOFT_KEY;
		}
		else if(x>DEVICE_WIDTH/2)
		{
			return AVK_RIGHT_SOFT_KEY;
		}
	}
	return NULL;
}


uint16 getDeviceHeight()
{
	AEEDeviceInfo  deviceInfo;
	ISHELL_GetDeviceInfo(((AEEApplet*) GETAPPINSTANCE() )->m_pIShell,&deviceInfo);
	return deviceInfo.cyScreen;
}

uint16 getDeviceWidth()
{
	AEEDeviceInfo  deviceInfo;
	ISHELL_GetDeviceInfo(((AEEApplet*) GETAPPINSTANCE() )->m_pIShell,&deviceInfo);
	return deviceInfo.cxScreen;
}