/* 
*  �ļ����ƣ�PicBrowser.cpp

	*  ժҪ��
			����ļ�������PicBrowser���ʵ�ֲ��֡�
			���ǽ�������ĵ�һ������ģ�飬����Ļ����ʾһ�п��Ա�ɫ���֣�
			��һ���������û������ƶ���ͼƬ��

	*  �汾��1.0.0
	*  ���ߣ�����è 
	*  ������ڣ�2008��8��21�� 
	* 
	*  ȡ���汾��N/A 
	*  ԭ����  ��N/A 
	*  ������ڣ�N/A 

*/ 

#include "fk.h"

///////////////////////////////
//����ʵ�ֲ���


PicBrowser::PicBrowser(GAME * l_papp):STATE(l_papp)
{
	m_pIDisplay=pApp->m_pIDisplay;
	m_pIShell=pApp->m_pIShell;

	m_bInitSuccess=FALSE;
	
}

PicBrowser::~PicBrowser()  
{
	FREEIF(m_pwszTitle); //�ͷ���Դ
	RELEASEIF(m_pImage);
}


/*
*	��������: 
		 PicBrowser::Init()
*  ���ܽ���:
		�ú���ΪPicBrowser��ĳ�ʼ�������������Ҫ����Դ��
*  ���������   
		N/A
*  ���������  
		N/A
*  ����ֵ��   
		�����Ƿ��ʼ���ɹ�
*	���ܳ��ֵ����⣺
		N/A
*/ 
boolean PicBrowser::Init()
{
	AEEImageInfo mi;
	int nByteLoad=0;
	m_pwszTitle=(AECHAR *)MALLOC(MAX_STRING_LEN+1);  //�����ַ�����Դ�ռ�
	if(m_pwszTitle!=NULL)
	{
		nByteLoad=ISHELL_LoadResString(m_pIShell,FK_RES_FILE,IDS_TITLE,m_pwszTitle,MAX_STRING_LEN); //�����ַ���
	}

	m_pImage=ISHELL_LoadResImage(m_pIShell,FK_RES_FILE,IDB_PIC); //����ͼƬ
	
	if(m_pImage==NULL || nByteLoad==0)
	{
		m_bInitSuccess=FALSE;  //��Դװ��ʧ��
	}
	else
	{
		m_nRed=255;  //��ɫֵ��Rֵ��ʼ��Ϊ255

		//ȡͼƬ��Ϣ
		IIMAGE_GetInfo(m_pImage,&mi);
		m_nWidth=(int)mi.cx;
		m_nHeight=(int)mi.cy;

		//���ü��о��κ�ͼƬ��ʼλ��
		SETAEERECT(&m_rctImage,0,pApp->nNoarmalFontH,pApp->lcdW,pApp->lcdH-pApp->nNoarmalFontH);
		m_nImageDrawX=m_rctImage.x+(m_rctImage.dx-m_nWidth)/2;
		m_nImageDrawY=m_rctImage.y+(m_rctImage.dy-m_nHeight)/2;
		
		m_bInitSuccess=TRUE;//װ�سɹ�
	}

	return m_bInitSuccess;
}



/*
*	��������: 
		 PicBrowser::Proc()
*  ���ܽ���:
		�ú���ΪPicBrowser������ݴ������������Ǹ���ÿ��ı���ʾ���ֵ���ɫֵ��
		
*  ���������   
		N/A
*  ���������  
		N/A
*  ����ֵ��   
		�����Ƿ��ʼ���ɹ�
*	���ܳ��ֵ����⣺
		N/A
*/
void   PicBrowser::Proc()
{
	m_nRed-=3;  //��Timer�ı�������ɫ
	if(m_nRed<0)   
	{
		m_nRed=255;
	}
}

/*
*	��������: 
		 PicBrowser::Draw()
*  ���ܽ���:
		�ú���ΪPicBrowser�����ʾ�����������Ǹ�����Ӧ��λ����Ϣ����ɫ��Ϣ����Ļ����ʾһ���ֺ�һ��ͼƬ��
		
*  ���������   
		N/A
*  ���������  
		N/A
*  ����ֵ��   
		�����Ƿ��ʼ���ɹ�
*	���ܳ��ֵ����⣺
		N/A
*/ 
void   PicBrowser::Draw()
{
	RGBVAL rgb;
	AEERect rctScreen;
	AECHAR pwszFailTip[7]= {0x8D44,0x6E90,0x88C5,0x8F7D,0x5931,0x8D25,0};    //��Դװ��ʧ��

	IDISPLAY_ClearScreen(m_pIDisplay);
	
	SETAEERECT(&rctScreen,0,0,pApp->lcdW,pApp->lcdH);	

	if(!m_bInitSuccess)  //����ʼ��ʧ�ܣ���ʾ֮
	{
		IDISPLAY_DrawText(m_pIDisplay,AEE_FONT_NORMAL,pwszFailTip,-1,0,0,0,0);
		return;
	}
	
	//���»���ɫ����
	rgb=IDISPLAY_SetColor(m_pIDisplay,CLR_USER_TEXT,MAKE_RGB(m_nRed,0,255-m_nRed));
	IDISPLAY_DrawText(m_pIDisplay,AEE_FONT_NORMAL,m_pwszTitle,-1,0,0,0,0);
	IDISPLAY_SetColor(m_pIDisplay,CLR_USER_TEXT,rgb);
	
	//���»�ͼƬ
	IDISPLAY_SetClipRect(m_pIDisplay,&m_rctImage); //���ü��о���
	IDISPLAY_ClearScreen(m_pIDisplay);
	IIMAGE_Draw(m_pImage,m_nImageDrawX,m_nImageDrawY);
	IDISPLAY_DrawRect(m_pIDisplay,&m_rctImage,MAKE_RGB(0,0,0),RGB_NONE,IDF_RECT_FRAME);
	IDISPLAY_SetClipRect(m_pIDisplay,&rctScreen);  //�����о��������

}


/*  
*	��������:  
		PicBrowser::KeyProc(int keycode, int mode)
*  ���ܽ���:
		�ú���ΪPicBrowser��İ�����������
		����������ƶ�ͼƬλ�ã���OK�������û���

*  �������:  
		N/A
*  �������:  
		N/A
*  ����ֵ:   
		�����Ƿ��ʼ���ɹ�
*	���ܳ��ֵ����⣺
		N/A
*/
void   PicBrowser::KeyProc(int keycode, int mode)
{
	//����EVT_KEY�¼�
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
			ISHELL_PostEvent(m_pIShell,AEECLSID_EG,EVT_USER_RESET,0,0);  //���ͻ��������¼�
			break;
		default:
			;
		}		
	}

	//���������EVT_KEY_PRESS�¼�
	if(mode==EVT_KEY_PRESS)   //��������ƶ�ͼƬ
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
*	��������: 
		PicBrowser::HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
*  ���ܽ���:
		�ú���ΪPicBrowser��������¼���������
		���ﴦ����EVT_USER_RESET�¼���
		
*  ���������   
		N/A
*  ���������  
		N/A
*  ����ֵ��   
		�����Ƿ��ʼ���ɹ�
*	���ܳ��ֵ����⣺
		N/A
*/
boolean  PicBrowser::HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	switch(eCode) 
	{
	case EVT_USER_RESET:
		m_nRed=255; //��ɫ����

		//������ͼƬλ������
		m_nImageDrawX=m_rctImage.x+(m_rctImage.dx-m_nWidth)/2;  
		m_nImageDrawY=m_rctImage.y+(m_rctImage.dy-m_nHeight)/2;

		break;
	default:
		;
	}
	return TRUE;
}