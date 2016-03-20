#ifndef _H_READ
#define _H_READ





class read:public STATE
{
public:
	read(GAME *lgpp);
	~read();
	
	IShell		*m_pIShell;
	IDisplay		*m_pIDisplay;
	IStatic     *pIStatic;

	 boolean  Init();   //初始化
	 void    Proc();     //数据处理
	 void    Draw();     //显示处理
	 void    KeyProc(int keyCode,int Mode);     //按键处理
	 boolean  HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam); //事件处理
	  
};

#endif 