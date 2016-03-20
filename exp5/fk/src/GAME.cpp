/*
*  文件名称：GAME.cpp

*  摘要：
		这个文件包含了主结构类GAME的实现部分。
		这是整个程序的总控模块，包括帧循环处理和事件分发处理。
	

*  版本：1.0.1
*  修改者：酷酷的猫 
*  完成日期：2008年8月21日 
* 
*  取代版本：1.0.0
*  原作者  ：疯神
*  完成日期：未知

*/ 


#include "fk.h"

///////////////////////////////
//函数实现部分

GAME::GAME()
{
}

GAME::~GAME()
{
}


/*
*	函数名称: 
		 GAME::InitGame()
*  功能介绍:
		该函数为主结构初始化函数，初始化必要的数据，建立第一个窗口，并引导进入帧循环。
*  输入参数：   
		N/A
*  输出参数：  
		N/A
*  返回值：   
		N/A
*	可能出现的意外：
		N/A
*/ 
void GAME::InitGame()
{
   AEEDeviceInfo  DI;
  
	
	ISHELL_GetDeviceInfo(m_pIShell, &DI);
   lcdW = DI.cxScreen;
   lcdH = DI.cyScreen;
   nNoarmalFontH=IDISPLAY_GetFontMetrics(m_pIDisplay,AEE_FONT_NORMAL,0,0);
   nAppClsID = AEECLSID_EG;

	cur_state = new MainMenu(this);  //这里进入你的第一个界面
	changeState=TRUE;
	StartScheduler();
	return;
}



void GAME::Exit()
{
   ISHELL_CloseApplet(m_pIShell, FALSE);
}

void GAME::ExitGame()
{
   StopScheduler();
   delete(cur_state);
   DELETEIF(del_state);

}

void GAME::StartScheduler()
{
   bstart = true;
   ISHELL_SetTimer(m_pIShell, 10, (PFNNOTIFY) Scheduler, this);
}

void GAME::StopScheduler()
{
   bstart = false;
   ISHELL_CancelTimer(m_pIShell, (PFNNOTIFY) Scheduler, this);
}


void GAME::OnResume()
{
   cur_state->Draw();
   StartScheduler();
}

void GAME::OnSuspend()
{
   StopScheduler();
}


/*
*	函数名称: 
		 GAME::Scheduler(GAME * pGame)
*  功能介绍:
		该函数为帧循环处理函数，处理整个程序的每一帧的行为，最后计算到下一帧的时间并设置Timer。
*  输入参数：   
		N/A
*  输出参数：  
		N/A
*  返回值：   
		N/A
*	可能出现的意外：
		N/A
*/ 
void GAME::Scheduler(GAME * pGame)
{
   int      nInterval;
   STATE   * state   = pGame->cur_state;

   nInterval = DEFAULT_INTERVAL;  //每一帧的间隔

   pGame->frame_time = pGame->cur_time;
   pGame->m_nMs1 = GET_TIMEMS();

   // key process

	if(pGame->bExitGame)  //将要退出
	{
		pGame->Exit();
		return;
	}

	if(pGame->changeState) //状态切换
	{
		DELETEIF(pGame->del_state); //删除原状态
		pGame->changeState = FALSE; 
		pGame->cur_state->Init();  //初始化新状态
	}
	else
	{
		if ( pGame->bkeyPress && (!pGame->bKeyPressIgnore) )//这是按键被按下的第一帧
		{
			state->KeyProc(pGame->m_keycode, EVT_KEY_PRESS);  //处理持续按下事件			
		}
		else
		{
			pGame->bKeyPressIgnore = FALSE;    //下一桢若还是按下状态,则不再忽略
		}//if  

	   state->Proc(); //当前窗口数据处理
     
		state->Draw(); //当前窗口显示 
		IDISPLAY_Update(pGame->m_pIDisplay);

   }//if


   pGame->m_nMs2 = GET_TIMEMS();
   nInterval -= pGame->m_nMs2 - pGame->m_nMs1;  //更改Timer间隔，以保证两帧之间间隔相同
   if ( nInterval < MIN_INTERVAL )
   {
		nInterval = MIN_INTERVAL;      //当本帧处理时间超过初始设定的间隔，则按1最小间隔设定Timer间隔
	}
   ISHELL_SetTimer(pGame->m_pIShell, nInterval, (PFNNOTIFY) GAME::Scheduler, pGame);
}


/*
*	函数名称: 
		 GAME::HandleKeyEvent(int keycode, int mode)
*  功能介绍:
		该函数为主结构的按键处理函数，处理整个程序的按键行为，并将按键事件转发给发给活动窗口。
*  输入参数：   
		N/A
*  输出参数：  
		N/A
*  返回值：   
		N/A
*	可能出现的意外：
		N/A
*/ 
void GAME::HandleKeyEvent(int keycode, int mode)
{
   if ( mode == EVT_KEY_PRESS )
   {
      boolean  bOld  = bkeyPress; // bOld用作记录按键原来是否为Press状态
      bkeyPress = TRUE;  //目前肯定为Press状态
      m_keycode = keycode;

      if ( !bOld )   //若原来没有按下
      {
			cur_state->KeyProc(keycode,EVT_KEY_PRESS);//这里做第一次的立即处理
         bKeyPressIgnore = TRUE;       // 之后的首帧应该忽略Press事件		
      }
   }
   else if ( mode == EVT_KEY_RELEASE )
   {
      bkeyPress = FALSE;  
   }
   else if ( mode == EVT_KEY )
   { 
       cur_state->KeyProc(keycode, mode);   
   }
}


/*
*	函数名称: 
		 GAME::getRandNumber(int nRange)
*  功能介绍:
		该函数为根据给定的种子取得0到nRange的随机整数
*  输入参数：   
		N/A
*  输出参数：  
		N/A
*  返回值：   
		N/A
*	可能出现的意外：
		该函数的随机效果并不十分理想
*/ 
int GAME::getRandNumber(int nRange)
{
   if ( nRandomSeed == 0 )
   {
      nRandomSeed = ISHELL_GetTimeMS(m_pIShell);
   }
   nRandomSeed = nRandomSeed * 1103515245L + 12345;
   return((unsigned) (nRandomSeed / 65536L) % nRange);
}


