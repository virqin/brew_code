#ifndef _H_ANI
#define _H_ANI



class Ani:public STATE
{
public:
	Ani(GAME * lapp);
	~Ani();
	IShell		*m_pIShell;
	IDisplay		*m_pIDisplay;

	IBitmap	*m_pRun;			//�ܲ�ͼƬ
	AECHAR	*pwszTitle;			//��������

	int m_nStep;					//��������

	int m_nRunx;					//�ܲ�λ��x����
	int m_nRuny;					//�ܲ�λ��y����
	int m_nRunw;					//�ܲ�ͼƬ���(ˮƽ����4֡)
	int m_nRunh;					//�ܲ�ͼƬ�߶�

	int m_nLcdH;					//��Ļ�߶�
	int m_nwith;                   //��Ļ���
	int m_state;
	int m_speed;

	boolean  Init();   //��ʼ��
	void    Proc();     //���ݴ���
	void    Draw();     //��ʾ����
	void    KeyProc(int keyCode,int Mode);     //��������
	boolean  HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam); //�¼�����
	
	 
};


#endif