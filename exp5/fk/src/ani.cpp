#include "fk.H"

Ani::Ani(GAME * lapp):STATE(lapp)
{
	m_pIShell = pApp->m_pIShell;
	m_pIDisplay = pApp->m_pIDisplay;
}
Ani::~Ani()
{

}

boolean Ani::Init()
{
	AEEBitmapInfo bi;
	AEEDeviceInfo di;
	//run *pme = NULL;
	ISHELL_GetDeviceInfo(m_pIShell,&di);
	pwszTitle=(AECHAR*)MALLOC(sizeof(AECHAR)*(MAX_STRING_LEN1+1));   //分配字符串缓冲区空间

	if(!pwszTitle)
	{
		return FALSE;
	}

	ISHELL_LoadResString(m_pIShell,ANI_RES_FILE,IDS_TITLE,pwszTitle,sizeof(AECHAR)*MAX_STRING_LEN); //载入字符串

	m_pRun=ISHELL_LoadResBitmap(m_pIShell,ANI_RES_FILE,IDB_BMP_GIRL	);		//载入图片

	if(!m_pRun)
	{
		return FALSE;
	}

	IBITMAP_SetTransparencyColor(m_pRun,
		IBITMAP_RGBToNative(m_pRun,MAKE_RGB(255,0,255))); //设置透明色，注意，需要转换为本地色

	IBITMAP_GetInfo(m_pRun,&bi,sizeof(AEEBitmapInfo));			//取得图片信息，这里用到的是图片的宽高

	m_nRunw=(int)bi.cx;										//女孩图片宽度		
	m_nRunh=(int)bi.cy;
	m_nRunx=(di.cxScreen-m_nRunw/FRAME_COUNT)/2;  //女孩的水平位置
	m_nRuny=di.cyScreen;									//女孩的初始竖直方向位置

	m_nStep=0;													//动画计数初始为0
	m_nLcdH=di.cyScreen;
	m_nwith = di.cxScreen;

	return TRUE;
}

void Ani::Proc()
{
	IDISPLAY_ClearScreen(m_pIDisplay);

	IDISPLAY_DrawText(m_pIDisplay,AEE_FONT_NORMAL,pwszTitle,-1,0,0,0,0);

	IDISPLAY_BitBlt(m_pIDisplay,
		m_nRunx,					//图片在屏幕上的x坐标
		m_nRuny,					//图片在屏幕上的y坐标
		m_nRunw/FRAME_COUNT, //图片在屏幕上的剪切宽度
		m_nRunh,					//图片在屏幕上的剪切高度
		m_pRun,					//要显示的图片接口
		(m_nStep % FRAME_COUNT)*(m_nRunw/FRAME_COUNT),	//图片上需显示区域相对于左上角的水平位移
		0,																				//图片上需显示区域相对于左上角的垂直位移
		AEE_RO_TRANSPARENT);

	//以透明色方式画图片
	//pme->m_state = 3;

	m_nStep++;	

	if (m_state == 1)
	{
		m_nRunx +=2;

	}
	if (m_state == 2)
	{
		m_nRunx -=2;

	}




	//动画计数+1

	if (m_speed == 0)
	{
		m_nRuny-=5;//女孩垂直位置上移2像素
	}
	else
	{
		m_nRuny -= 1;
	}



	if (m_nRunx + m_nRunw < 0)
	{
		m_nRunx =m_nwith ;
	}

	if(m_nRuny+m_nRunh<0)  //移出屏幕了
	{
		m_nRuny=m_nLcdH;		//回到底端
	}

	
	IDISPLAY_Update(m_pIDisplay);

}

void Ani::Draw()
{

}


void Ani::KeyProc(int keyCode,int Mode)
{
	if(Mode==EVT_KEY)
	{
		switch(keyCode) 
		{
		case AVK_CLR:
			pApp->del_state=pApp->cur_state;
			pApp->cur_state=new MainMenu(pApp);
			pApp->changeState=TRUE;
			break;
		case AVK_SELECT:
			ISHELL_PostEvent(m_pIShell,AEECLSID_EG,EVT_USER_RESET,0,0);  //发送画面重置事件
			break;
		case AVK_UP:
			m_speed = 0;
			Proc();
			break;
		case AVK_DOWN:
			m_speed = 1;
			Proc();
			break;
		case AVK_LEFT:
			m_state = 1;
			Proc();
			break;
		case AVK_RIGHT:
			m_state = 2;
			Proc();
			break;
		default:
			break;
		}
    }
}

boolean Ani:: HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	switch(eCode) 
	{
	case EVT_USER_RESET:


		break;
	default:
		;
	}
	return TRUE;
}


