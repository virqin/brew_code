/*
*  �ļ����ƣ�GAME.cpp

*  ժҪ��
		����ļ����������ṹ��GAME��ʵ�ֲ��֡�
		��������������ܿ�ģ�飬����֡ѭ��������¼��ַ�����
	

*  �汾��1.0.1
*  �޸��ߣ�����è 
*  ������ڣ�2008��8��21�� 
* 
*  ȡ���汾��1.0.0
*  ԭ����  ������
*  ������ڣ�δ֪

*/ 


#include "fk.h"

///////////////////////////////
//����ʵ�ֲ���

GAME::GAME()
{
}

GAME::~GAME()
{
}


/*
*	��������: 
		 GAME::InitGame()
*  ���ܽ���:
		�ú���Ϊ���ṹ��ʼ����������ʼ����Ҫ�����ݣ�������һ�����ڣ�����������֡ѭ����
*  ���������   
		N/A
*  ���������  
		N/A
*  ����ֵ��   
		N/A
*	���ܳ��ֵ����⣺
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

	cur_state = new MainMenu(this);  //���������ĵ�һ������
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
*	��������: 
		 GAME::Scheduler(GAME * pGame)
*  ���ܽ���:
		�ú���Ϊ֡ѭ�����������������������ÿһ֡����Ϊ�������㵽��һ֡��ʱ�䲢����Timer��
*  ���������   
		N/A
*  ���������  
		N/A
*  ����ֵ��   
		N/A
*	���ܳ��ֵ����⣺
		N/A
*/ 
void GAME::Scheduler(GAME * pGame)
{
   int      nInterval;
   STATE   * state   = pGame->cur_state;

   nInterval = DEFAULT_INTERVAL;  //ÿһ֡�ļ��

   pGame->frame_time = pGame->cur_time;
   pGame->m_nMs1 = GET_TIMEMS();

   // key process

	if(pGame->bExitGame)  //��Ҫ�˳�
	{
		pGame->Exit();
		return;
	}

	if(pGame->changeState) //״̬�л�
	{
		DELETEIF(pGame->del_state); //ɾ��ԭ״̬
		pGame->changeState = FALSE; 
		pGame->cur_state->Init();  //��ʼ����״̬
	}
	else
	{
		if ( pGame->bkeyPress && (!pGame->bKeyPressIgnore) )//���ǰ��������µĵ�һ֡
		{
			state->KeyProc(pGame->m_keycode, EVT_KEY_PRESS);  //������������¼�			
		}
		else
		{
			pGame->bKeyPressIgnore = FALSE;    //��һ�������ǰ���״̬,���ٺ���
		}//if  

	   state->Proc(); //��ǰ�������ݴ���
     
		state->Draw(); //��ǰ������ʾ 
		IDISPLAY_Update(pGame->m_pIDisplay);

   }//if


   pGame->m_nMs2 = GET_TIMEMS();
   nInterval -= pGame->m_nMs2 - pGame->m_nMs1;  //����Timer������Ա�֤��֮֡������ͬ
   if ( nInterval < MIN_INTERVAL )
   {
		nInterval = MIN_INTERVAL;      //����֡����ʱ�䳬����ʼ�趨�ļ������1��С����趨Timer���
	}
   ISHELL_SetTimer(pGame->m_pIShell, nInterval, (PFNNOTIFY) GAME::Scheduler, pGame);
}


/*
*	��������: 
		 GAME::HandleKeyEvent(int keycode, int mode)
*  ���ܽ���:
		�ú���Ϊ���ṹ�İ�����������������������İ�����Ϊ�����������¼�ת������������ڡ�
*  ���������   
		N/A
*  ���������  
		N/A
*  ����ֵ��   
		N/A
*	���ܳ��ֵ����⣺
		N/A
*/ 
void GAME::HandleKeyEvent(int keycode, int mode)
{
   if ( mode == EVT_KEY_PRESS )
   {
      boolean  bOld  = bkeyPress; // bOld������¼����ԭ���Ƿ�ΪPress״̬
      bkeyPress = TRUE;  //Ŀǰ�϶�ΪPress״̬
      m_keycode = keycode;

      if ( !bOld )   //��ԭ��û�а���
      {
			cur_state->KeyProc(keycode,EVT_KEY_PRESS);//��������һ�ε���������
         bKeyPressIgnore = TRUE;       // ֮�����֡Ӧ�ú���Press�¼�		
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
*	��������: 
		 GAME::getRandNumber(int nRange)
*  ���ܽ���:
		�ú���Ϊ���ݸ���������ȡ��0��nRange���������
*  ���������   
		N/A
*  ���������  
		N/A
*  ����ֵ��   
		N/A
*	���ܳ��ֵ����⣺
		�ú��������Ч������ʮ������
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


