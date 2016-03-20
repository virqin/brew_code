#ifndef __STATE__
#define __STATE__



class STATE   //基类的定义
{
   public:
                  STATE(GAME * l_pApp);
   virtual        ~STATE();

   GAME          * pApp;  //GAME 为游戏控制模块
   
   virtual boolean  Init() =0;  //初始化
   virtual void    Proc() = 0;    //数据处理
   virtual void    Draw() = 0;    //显示处理
   virtual void    KeyProc(int keyCode,int keyMode) = 0;    //按键处理
   virtual boolean  HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)=0; //事件处理
  
};


#endif