#ifndef __GAME__
#define __GAME__


///////////////////
//ǰ������

class          STATE;
class				MainMenu;


///////////////////
//���ṹ����

class GAME : public AEEApplet
{
   public:
                  GAME();
                  ~GAME();

   void           Exit();        //��������     
   void           ExitGame();		//�˳���Ϸʱ�ı�Ҫ����

   void           OnSuspend();   //�жϴ���
   void           OnResume();    //�жϻָ�����

   void           StopScheduler();//ֹͣ֡ѭ��
	void           InitGame();		//��Ϸ��ʼ��
   void           StartScheduler();//��ʼ֡ѭ��
   static void    Scheduler(GAME * pGame);//֡ѭ������
   void           HandleKeyEvent(int keycode, int mode);//��������
   int            getRandNumber(int nRange); //ȡ�����


	
 
   STATE         * cur_state;  //�����ָ��
   STATE         * del_state;  //��Ҫɾ���Ĵ���ָ��
   bool           changeState; //״̬�л���־

	int            nAppClsID;  //Ӧ��Class ID


   ////////////////////////////////////////////
	//��������
   boolean        bkeyPress;   //�Ƿ��ڰ�����������״̬
	boolean			bKeyPressIgnore; //�Ƿ����KeyPress�¼�
   int            m_keycode;    //��ǰ��������
	int				m_keymode;	  //����ģʽ
   int            nRandomSeed;  //�������

 
   ///////////////////////////////////////////////
   // SCHEDULER VARIABLES
   bool           bstart;    //ѭ���Ƿ�ʼ��־
   int            cur_time;
   int            frame_time; 
  
   int            m_nMs1;     //ʱ�����
   int            m_nMs2;     //ʱ�����
	boolean			bExitGame;  //�˳���־

   ///////////////////////////////////////////////
	// ��������
	int            lcdH;			//��Ļ�߶�
	int				lcdW;			//��Ļ���
   int            GAME_HEIGHT;//��Ϸ����߶�
	int				GAME_WIDTH; //��Ϸ������
   int				nNoarmalFontH;//��ͨ����߶�
};


#endif