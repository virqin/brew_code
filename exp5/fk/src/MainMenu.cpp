#include "fk.h"




MainMenu::MainMenu(GAME * l_pApp):STATE(l_pApp)
{
	pApp=l_pApp;
	m_pIDisplay=l_pApp->m_pIDisplay;
	m_pIShell=pApp->m_pIShell;
}
MainMenu::~MainMenu()
{
	RELEASEIF(pIMenuCtl);
}



boolean	MainMenu::Init()
{
	AEERect rect;
	AEEDeviceInfo di;
	AEEItemStyle nNormal;
	AEEItemStyle nSel;
	AEEMenuColors mcs;
	CtlAddItem cai;
	AECHAR pwszMenuTitle[7]= {0x5B8C,0x6574,0x7A0B,0x5E8F,0x793A,0x4F8B,0};    //��������ʾ��
	AECHAR pwszMenuText1[5]= {0x56FE,0x7247,0x6D4F,0x89C8,0};    //ͼƬ���
	AECHAR pwszMenuText2[9]= {0x8BF7,0x6DFB,0x52A0,0x0020,0x52A8,0x753B,0x529F,0x80FD,0};    //����� ��������
	AECHAR pwszMenuText3[9]= {0x8BF7,0x6DFB,0x52A0,0x0020,0x5E2E,0x52A9,0x529F,0x80FD,0};    //����� ��������
	AECHAR pwszMenuText4[5]= {0x9000,0x51FA,0x7A0B,0x5E8F,0};    //�˳�����

	
	//����IMenuCtl�ӿ�
	if(SUCCESS!=(ISHELL_CreateInstance(m_pIShell,AEECLSID_MENUCTL,(void**)&pIMenuCtl) ) )
		return FALSE;
	
	ISHELL_GetDeviceInfo(m_pIShell,&di);
	SETAEERECT(&rect,0,0,di.cxScreen,di.cyScreen);
	
	
	//δѡ�в˵��ķ��
	nNormal.ft = AEE_FT_EMPTY;
	nNormal.xOffset = nNormal.yOffset = 0;
	nNormal.roImage = AEE_RO_TRANSPARENT;
	
	//ѡ�в˵���ķ��
	nSel.ft = AEE_FT_BOX;
	nSel.xOffset =nSel.yOffset = 0;
	nSel.roImage = AEE_RO_TRANSPARENT;
	
	//�˵���ɫ����
	mcs.cFrame=MENUCTL_FRAME_COLOR;
	mcs.cBack=MENUCTL_UNSELECT_ITEM_COLOR;
	mcs.cText=MENUCTL_UNSELECT_TEXT_COLOR;
	mcs.cSelBack=MENUCTL_SELECT_ITEM_COLOR;
	mcs.cSelText=MENUCTL_SELECT_TEXT_COLOR;
	mcs.cScrollbarFill=MENUCTL_SCROLL_BAR_FILLCOLOR;
	mcs.cScrollbar=MENUCTL_SCROLL_BAR_COLOR;
	mcs.wMask=MENUCTL_COLOR_MASK;	
	
	
	
	IMENUCTL_SetRect( pIMenuCtl,&rect);//���ò˵���Ļ����
	IMENUCTL_SetColors( pIMenuCtl,&mcs);//������ɫ��Ϣ
	IMENUCTL_SetProperties( pIMenuCtl,IMENUCTL_GetProperties( pIMenuCtl)|MP_NO_REDRAW);	//��������
	IMENUCTL_SetStyle( pIMenuCtl,&nNormal,&nSel);	//���ò˵����
	
	
	//�˵�������
	cai.pImage=NULL;     //û��ͼƬ
	cai.pszResImage=NULL;
	cai.pszResText=NULL;
	cai.wFont=AEE_FONT_NORMAL; //��������
	cai.wText=0;   
	cai.wImage=0;
	
	IMENUCTL_SetTitle( pIMenuCtl,NULL,0,pwszMenuTitle); //���ñ���
	
	cai.wItemID=MENU_ITEM_1;   //��һ��id
	cai.pText=pwszMenuText1;	//��һ����ʾ����
	IMENUCTL_AddItemEx( pIMenuCtl,&cai);	//��ӵ�һ��
	
	cai.wItemID=MENU_ITEM_2;
	cai.pText=pwszMenuText2;
	IMENUCTL_AddItemEx( pIMenuCtl,&cai);
	
	cai.wItemID=MENU_ITEM_3;
	cai.pText=pwszMenuText3;
	IMENUCTL_AddItemEx( pIMenuCtl,&cai);
	
	cai.wItemID=MENU_ITEM_4;
	cai.pText=pwszMenuText4;
	IMENUCTL_AddItemEx( pIMenuCtl,&cai);
	
	IMENUCTL_SetActive( pIMenuCtl,TRUE);  //����˵�
	
	return TRUE;
	return TRUE;
}

void		MainMenu::Proc()
{

}
void		MainMenu::Draw()
{
	IDISPLAY_ClearScreen( m_pIDisplay);
	
	IMENUCTL_Redraw( pIMenuCtl);
	
	IDISPLAY_Update( m_pIDisplay);	
}

void		MainMenu::KeyProc(int keyCode,int keyMode)
{
	if(keyMode==EVT_KEY)
	{
		if( pIMenuCtl!=NULL)
		{
			IMENUCTL_HandleEvent( pIMenuCtl,EVT_KEY,keyCode,0);
		}
	}

}

boolean	MainMenu::HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	if(eCode==EVT_COMMAND)
	{
		switch(wParam) 
		{
		case MENU_ITEM_1:    //��1�����
			pApp->del_state=pApp->cur_state;
			pApp->cur_state=new PicBrowser(pApp);
			pApp->changeState=TRUE;
			break;
		case MENU_ITEM_2:		//��2�����
			pApp->del_state = pApp->cur_state;
			pApp->cur_state = new read(pApp);
			pApp->changeState = TRUE;
			break;
		case MENU_ITEM_3:		//��3�����
			pApp->del_state = pApp->cur_state;
			pApp->cur_state = new Ani(pApp);
			pApp->changeState = TRUE;
			
			break;
		case MENU_ITEM_4:		//��4�����
			pApp->bExitGame=TRUE;
			break;
		default:
			break;
		}
	}
	return TRUE;
}