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
	pwszTitle=(AECHAR*)MALLOC(sizeof(AECHAR)*(MAX_STRING_LEN1+1));   //�����ַ����������ռ�

	if(!pwszTitle)
	{
		return FALSE;
	}

	ISHELL_LoadResString(m_pIShell,ANI_RES_FILE,IDS_TITLE,pwszTitle,sizeof(AECHAR)*MAX_STRING_LEN); //�����ַ���

	m_pRun=ISHELL_LoadResBitmap(m_pIShell,ANI_RES_FILE,IDB_BMP_GIRL	);		//����ͼƬ

	if(!m_pRun)
	{
		return FALSE;
	}

	IBITMAP_SetTransparencyColor(m_pRun,
		IBITMAP_RGBToNative(m_pRun,MAKE_RGB(255,0,255))); //����͸��ɫ��ע�⣬��Ҫת��Ϊ����ɫ

	IBITMAP_GetInfo(m_pRun,&bi,sizeof(AEEBitmapInfo));			//ȡ��ͼƬ��Ϣ�������õ�����ͼƬ�Ŀ��

	m_nRunw=(int)bi.cx;										//Ů��ͼƬ���		
	m_nRunh=(int)bi.cy;
	m_nRunx=(di.cxScreen-m_nRunw/FRAME_COUNT)/2;  //Ů����ˮƽλ��
	m_nRuny=di.cyScreen;									//Ů���ĳ�ʼ��ֱ����λ��

	m_nStep=0;													//����������ʼΪ0
	m_nLcdH=di.cyScreen;
	m_nwith = di.cxScreen;

	return TRUE;
}

void Ani::Proc()
{
	IDISPLAY_ClearScreen(m_pIDisplay);

	IDISPLAY_DrawText(m_pIDisplay,AEE_FONT_NORMAL,pwszTitle,-1,0,0,0,0);

	IDISPLAY_BitBlt(m_pIDisplay,
		m_nRunx,					//ͼƬ����Ļ�ϵ�x����
		m_nRuny,					//ͼƬ����Ļ�ϵ�y����
		m_nRunw/FRAME_COUNT, //ͼƬ����Ļ�ϵļ��п��
		m_nRunh,					//ͼƬ����Ļ�ϵļ��и߶�
		m_pRun,					//Ҫ��ʾ��ͼƬ�ӿ�
		(m_nStep % FRAME_COUNT)*(m_nRunw/FRAME_COUNT),	//ͼƬ������ʾ������������Ͻǵ�ˮƽλ��
		0,																				//ͼƬ������ʾ������������ϽǵĴ�ֱλ��
		AEE_RO_TRANSPARENT);

	//��͸��ɫ��ʽ��ͼƬ
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




	//��������+1

	if (m_speed == 0)
	{
		m_nRuny-=5;//Ů����ֱλ������2����
	}
	else
	{
		m_nRuny -= 1;
	}



	if (m_nRunx + m_nRunw < 0)
	{
		m_nRunx =m_nwith ;
	}

	if(m_nRuny+m_nRunh<0)  //�Ƴ���Ļ��
	{
		m_nRuny=m_nLcdH;		//�ص��׶�
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
			ISHELL_PostEvent(m_pIShell,AEECLSID_EG,EVT_USER_RESET,0,0);  //���ͻ��������¼�
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


