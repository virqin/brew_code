#ifndef _H_ANI
#define _H_ANI



class Ani:public STATE
{
public:
	Ani(GAME * lapp);
	~Ani();
	IShell		*m_pIShell;
	IDisplay		*m_pIDisplay;

	IBitmap	*m_pRun;			//跑步图片
	AECHAR	*pwszTitle;			//标题文字

	int m_nStep;					//动画计数

	int m_nRunx;					//跑步位置x坐标
	int m_nRuny;					//跑步位置y坐标
	int m_nRunw;					//跑步图片宽度(水平方向共4帧)
	int m_nRunh;					//跑步图片高度

	int m_nLcdH;					//屏幕高度
	int m_nwith;                   //屏幕宽度
	int m_state;
	int m_speed;

	boolean  Init();   //初始化
	void    Proc();     //数据处理
	void    Draw();     //显示处理
	void    KeyProc(int keyCode,int Mode);     //按键处理
	boolean  HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam); //事件处理
	
	 
};


#endif