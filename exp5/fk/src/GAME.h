#ifndef __GAME__
#define __GAME__


///////////////////
//前向声明

class          STATE;
class				MainMenu;


///////////////////
//主结构定义

class GAME : public AEEApplet
{
   public:
                  GAME();
                  ~GAME();

   void           Exit();        //结束程序     
   void           ExitGame();		//退出游戏时的必要操作

   void           OnSuspend();   //中断处理
   void           OnResume();    //中断恢复处理

   void           StopScheduler();//停止帧循环
	void           InitGame();		//游戏初始化
   void           StartScheduler();//开始帧循环
   static void    Scheduler(GAME * pGame);//帧循环处理
   void           HandleKeyEvent(int keycode, int mode);//按键处理
   int            getRandNumber(int nRange); //取随机数


	
 
   STATE         * cur_state;  //活动窗口指针
   STATE         * del_state;  //将要删除的窗口指针
   bool           changeState; //状态切换标志

	int            nAppClsID;  //应用Class ID


   ////////////////////////////////////////////
	//按键定义
   boolean        bkeyPress;   //是否处于按键持续按下状态
	boolean			bKeyPressIgnore; //是否忽略KeyPress事件
   int            m_keycode;    //当前按键代码
	int				m_keymode;	  //按键模式
   int            nRandomSeed;  //随机种子

 
   ///////////////////////////////////////////////
   // SCHEDULER VARIABLES
   bool           bstart;    //循环是否开始标志
   int            cur_time;
   int            frame_time; 
  
   int            m_nMs1;     //时间变量
   int            m_nMs2;     //时间变量
	boolean			bExitGame;  //退出标志

   ///////////////////////////////////////////////
	// 长度数据
	int            lcdH;			//屏幕高度
	int				lcdW;			//屏幕宽度
   int            GAME_HEIGHT;//游戏区域高度
	int				GAME_WIDTH; //游戏区域宽度
   int				nNoarmalFontH;//普通字体高度
};


#endif