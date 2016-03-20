/* 
*  文件名称：PicBrowser.cpp

	*  摘要：
			这个文件包含了PicBrowser类的实现部分。
			这是进入程序后的第一个窗口模块，在屏幕上显示一行可以变色的字，
			和一幅可以由用户控制移动的图片。

	*  版本：1.0.0
	*  作者：酷酷的猫 
	*  完成日期：2008年8月21日 
	* 
	*  取代版本：N/A 
	*  原作者  ：N/A 
	*  完成日期：N/A 

*/ 

#include "fk.h"

///////////////////////////////
//函数实现部分


PicBrowser::PicBrowser(GAME * l_papp):STATE(l_papp)
{
	m_pIDisplay=pApp->m_pIDisplay;
	m_pIShell=pApp->m_pIShell;

	m_bInitSuccess=FALSE;
	
}

PicBrowser::~PicBrowser()  
{
	FREEIF(m_pwszTitle); //释放资源
	RELEASEIF(m_pImage);
}


/*
*	函数名称: 
		 PicBrowser::Init()
*  功能介绍:
		该函数为PicBrowser类的初始化函数，载入必要的资源。
*  输入参数：   
		N/A
*  输出参数：  
		N/A
*  返回值：   
		返回是否初始化成功
*	可能出现的意外：
		N/A
*/ 
boolean PicBrowser::Init()
{
	AEEImageInfo mi;
	int nByteLoad=0;
	m_pwszTitle=(AECHAR *)MALLOC(MAX_STRING_LEN+1);  //申请字符串资源空间
	if(m_pwszTitle!=NULL)
	{
		nByteLoad=ISHELL_LoadResString(m_pIShell,FK_RES_FILE,IDS_TITLE,m_pwszTitle,MAX_STRING_LEN); //载入字符串
	}

	m_pImage=ISHELL_LoadResImage(m_pIShell,FK_RES_FILE,IDB_PIC); //载入图片
	
	if(m_pImage==NULL || nByteLoad==0)
	{
		m_bInitSuccess=FALSE;  //资源装载失败
	}
	else
	{
		m_nRed=255;  //颜色值的R值初始化为255

		//取图片信息
		IIMAGE_GetInfo(m_pImage,&mi);
		m_nWidth=(int)mi.cx;
		m_nHeight=(int)mi.cy;

		//设置剪切矩形和图片初始位置
		SETAEERECT(&m_rctImage,0,pApp->nNoarmalFontH,pApp->lcdW,pApp->lcdH-pApp->nNoarmalFontH);
		m_nImageDrawX=m_rctImage.x+(m_rctImage.dx-m_nWidth)/2;
		m_nImageDrawY=m_rctImage.y+(m_rctImage.dy-m_nHeight)/2;
		
		m_bInitSuccess=TRUE;//装载成功
	}

	return m_bInitSuccess;
}



/*
*	函数名称: 
		 PicBrowser::Proc()
*  功能介绍:
		该函数为PicBrowser类的数据处理函数。作用是根据每桢改变提示文字的颜色值。
		
*  输入参数：   
		N/A
*  输出参数：  
		N/A
*  返回值：   
		返回是否初始化成功
*	可能出现的意外：
		N/A
*/
void   PicBrowser::Proc()
{
	m_nRed-=3;  //随Timer改变字体颜色
	if(m_nRed<0)   
	{
		m_nRed=255;
	}
}

/*
*	函数名称: 
		 PicBrowser::Draw()
*  功能介绍:
		该函数为PicBrowser类的显示函数。作用是根据相应的位置信息和颜色信息在屏幕上显示一行字和一副图片。
		
*  输入参数：   
		N/A
*  输出参数：  
		N/A
*  返回值：   
		返回是否初始化成功
*	可能出现的意外：
		N/A
*/ 
void   PicBrowser::Draw()
{
	RGBVAL rgb;
	AEERect rctScreen;
	AECHAR pwszFailTip[7]= {0x8D44,0x6E90,0x88C5,0x8F7D,0x5931,0x8D25,0};    //资源装载失败

	IDISPLAY_ClearScreen(m_pIDisplay);
	
	SETAEERECT(&rctScreen,0,0,pApp->lcdW,pApp->lcdH);	

	if(!m_bInitSuccess)  //若初始化失败，提示之
	{
		IDISPLAY_DrawText(m_pIDisplay,AEE_FONT_NORMAL,pwszFailTip,-1,0,0,0,0);
		return;
	}
	
	//以下画彩色文字
	rgb=IDISPLAY_SetColor(m_pIDisplay,CLR_USER_TEXT,MAKE_RGB(m_nRed,0,255-m_nRed));
	IDISPLAY_DrawText(m_pIDisplay,AEE_FONT_NORMAL,m_pwszTitle,-1,0,0,0,0);
	IDISPLAY_SetColor(m_pIDisplay,CLR_USER_TEXT,rgb);
	
	//以下画图片
	IDISPLAY_SetClipRect(m_pIDisplay,&m_rctImage); //设置剪切矩形
	IDISPLAY_ClearScreen(m_pIDisplay);
	IIMAGE_Draw(m_pImage,m_nImageDrawX,m_nImageDrawY);
	IDISPLAY_DrawRect(m_pIDisplay,&m_rctImage,MAKE_RGB(0,0,0),RGB_NONE,IDF_RECT_FRAME);
	IDISPLAY_SetClipRect(m_pIDisplay,&rctScreen);  //将剪切矩形设回来

}


/*  
*	函数名称:  
		PicBrowser::KeyProc(int keycode, int mode)
*  功能介绍:
		该函数为PicBrowser类的按键处理函数。
		按方向键将移动图片位置，按OK键将重置画面

*  输入参数:  
		N/A
*  输出参数:  
		N/A
*  返回值:   
		返回是否初始化成功
*	可能出现的意外：
		N/A
*/
void   PicBrowser::KeyProc(int keycode, int mode)
{
	//处理EVT_KEY事件
	if(mode==EVT_KEY)
	{
		switch(keycode) 
		{
		case AVK_CLR:
			pApp->del_state=pApp->cur_state;
			pApp->cur_state=new MainMenu(pApp);
			pApp->changeState=TRUE;
			break;
		case AVK_SELECT:
			ISHELL_PostEvent(m_pIShell,AEECLSID_EG,EVT_USER_RESET,0,0);  //发送画面重置事件
			break;
		default:
			;
		}		
	}

	//处理方向键的EVT_KEY_PRESS事件
	if(mode==EVT_KEY_PRESS)   //按方向键移动图片
	{
		switch(keycode) 
		{
		case AVK_UP:
			if( (--m_nImageDrawY) <= m_rctImage.y-m_nHeight )
				m_nImageDrawY = m_rctImage.y-m_nHeight;
			break;
		case AVK_DOWN:
			if( (++m_nImageDrawY) >= m_rctImage.y+m_rctImage.dy)
				m_nImageDrawY = m_rctImage.y+m_rctImage.dy;
			break;
		case AVK_LEFT:
			if( (--m_nImageDrawX) <= m_rctImage.x-m_nWidth)
				m_nImageDrawX = m_rctImage.x-m_nWidth;
			break;
		case AVK_RIGHT:
			if( (++m_nImageDrawX) >= m_rctImage.x+m_rctImage.dx )
				m_nImageDrawX = m_rctImage.x+m_rctImage.dx;
			break;
		default:
			;
		}
	}
		
}


/*
*	函数名称: 
		PicBrowser::HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
*  功能介绍:
		该函数为PicBrowser类的其他事件处理函数。
		这里处理了EVT_USER_RESET事件。
		
*  输入参数：   
		N/A
*  输出参数：  
		N/A
*  返回值：   
		返回是否初始化成功
*	可能出现的意外：
		N/A
*/
boolean  PicBrowser::HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	switch(eCode) 
	{
	case EVT_USER_RESET:
		m_nRed=255; //颜色重置

		//以下是图片位置重置
		m_nImageDrawX=m_rctImage.x+(m_rctImage.dx-m_nWidth)/2;  
		m_nImageDrawY=m_rctImage.y+(m_rctImage.dy-m_nHeight)/2;

		break;
	default:
		;
	}
	return TRUE;
}