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

	 boolean  Init();   //��ʼ��
	 void    Proc();     //���ݴ���
	 void    Draw();     //��ʾ����
	 void    KeyProc(int keyCode,int Mode);     //��������
	 boolean  HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam); //�¼�����
	  
};

#endif 