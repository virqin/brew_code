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

	boolean	Init();  //��ʼ��
   void		Proc() ;    //���ݴ���
   void		Draw() ;    //��ʾ����
   void		KeyProc(int keyCode,int keyMode) ;    //��������
   boolean	HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam); //�¼�����
};

#endif