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
	AECHAR pwszMenuTitle[7]= {0x5B8C,0x6574,0x7A0B,0x5E8F,0x793A,0x4F8B,0};    //完整程序示例
	AECHAR pwszMenuText1[5]= {0x56FE,0x7247,0x6D4F,0x89C8,0};    //图片浏览
	AECHAR pwszMenuText2[9]= {0x8BF7,0x6DFB,0x52A0,0x0020,0x52A8,0x753B,0x529F,0x80FD,0};    //请添加 动画功能
	AECHAR pwszMenuText3[9]= {0x8BF7,0x6DFB,0x52A0,0x0020,0x5E2E,0x52A9,0x529F,0x80FD,0};    //请添加 帮助功能
	AECHAR pwszMenuText4[5]= {0x9000,0x51FA,0x7A0B,0x5E8F,0};    //退出程序

	
	//建立IMenuCtl接口
	if(SUCCESS!=(ISHELL_CreateInstance(m_pIShell,AEECLSID_MENUCTL,(void**)&pIMenuCtl) ) )
		return FALSE;
	
	ISHELL_GetDeviceInfo(m_pIShell,&di);
	SETAEERECT(&rect,0,0,di.cxScreen,di.cyScreen);
	
	
	//未选中菜单的风格
	nNormal.ft = AEE_FT_EMPTY;
	nNormal.xOffset = nNormal.yOffset = 0;
	nNormal.roImage = AEE_RO_TRANSPARENT;
	
	//选中菜单项的风格
	nSel.ft = AEE_FT_BOX;
	nSel.xOffset =nSel.yOffset = 0;
	nSel.roImage = AEE_RO_TRANSPARENT;
	
	//菜单颜色定义
	mcs.cFrame=MENUCTL_FRAME_COLOR;
	mcs.cBack=MENUCTL_UNSELECT_ITEM_COLOR;
	mcs.cText=MENUCTL_UNSELECT_TEXT_COLOR;
	mcs.cSelBack=MENUCTL_SELECT_ITEM_COLOR;
	mcs.cSelText=MENUCTL_SELECT_TEXT_COLOR;
	mcs.cScrollbarFill=MENUCTL_SCROLL_BAR_FILLCOLOR;
	mcs.cScrollbar=MENUCTL_SCROLL_BAR_COLOR;
	mcs.wMask=MENUCTL_COLOR_MASK;	
	
	
	
	IMENUCTL_SetRect( pIMenuCtl,&rect);//设置菜单屏幕矩形
	IMENUCTL_SetColors( pIMenuCtl,&mcs);//设置颜色信息
	IMENUCTL_SetProperties( pIMenuCtl,IMENUCTL_GetProperties( pIMenuCtl)|MP_NO_REDRAW);	//设置属性
	IMENUCTL_SetStyle( pIMenuCtl,&nNormal,&nSel);	//设置菜单风格
	
	
	//菜单项属性
	cai.pImage=NULL;     //没有图片
	cai.pszResImage=NULL;
	cai.pszResText=NULL;
	cai.wFont=AEE_FONT_NORMAL; //正常字体
	cai.wText=0;   
	cai.wImage=0;
	
	IMENUCTL_SetTitle( pIMenuCtl,NULL,0,pwszMenuTitle); //设置标题
	
	cai.wItemID=MENU_ITEM_1;   //第一项id
	cai.pText=pwszMenuText1;	//第一项显示文字
	IMENUCTL_AddItemEx( pIMenuCtl,&cai);	//添加第一项
	
	cai.wItemID=MENU_ITEM_2;
	cai.pText=pwszMenuText2;
	IMENUCTL_AddItemEx( pIMenuCtl,&cai);
	
	cai.wItemID=MENU_ITEM_3;
	cai.pText=pwszMenuText3;
	IMENUCTL_AddItemEx( pIMenuCtl,&cai);
	
	cai.wItemID=MENU_ITEM_4;
	cai.pText=pwszMenuText4;
	IMENUCTL_AddItemEx( pIMenuCtl,&cai);
	
	IMENUCTL_SetActive( pIMenuCtl,TRUE);  //激活菜单
	
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
		case MENU_ITEM_1:    //第1项被按下
			pApp->del_state=pApp->cur_state;
			pApp->cur_state=new PicBrowser(pApp);
			pApp->changeState=TRUE;
			break;
		case MENU_ITEM_2:		//第2项被按下
			pApp->del_state = pApp->cur_state;
			pApp->cur_state = new read(pApp);
			pApp->changeState = TRUE;
			break;
		case MENU_ITEM_3:		//第3项被按下
			pApp->del_state = pApp->cur_state;
			pApp->cur_state = new Ani(pApp);
			pApp->changeState = TRUE;
			
			break;
		case MENU_ITEM_4:		//第4项被按下
			pApp->bExitGame=TRUE;
			break;
		default:
			break;
		}
	}
	return TRUE;
}