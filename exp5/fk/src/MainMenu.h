#ifndef _H_MAIN
#define _H_MAIN




class MainMenu:public STATE
{
public:
	MainMenu(GAME * l_pApp);
	~MainMenu();

	IShell	*m_pIShell;
	IDisplay *m_pIDisplay;
	IMenuCtl *pIMenuCtl;

	boolean	Init();  //初始化
   void		Proc() ;    //数据处理
   void		Draw() ;    //显示处理
   void		KeyProc(int keyCode,int keyMode) ;    //按键处理
   boolean	HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam); //事件处理
};

#endif