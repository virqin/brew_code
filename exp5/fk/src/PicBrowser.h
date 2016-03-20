#ifndef _H_FIRST_WINDOW
#define _H_FIRST_WINDOW





//////////////////////
//����ģ�鶨��

class PicBrowser : public STATE
{
	public:
		PicBrowser (GAME * l_papp);
		~PicBrowser();
	
		IShell		*m_pIShell;
		IDisplay		*m_pIDisplay;
	
		IImage		*m_pImage;    //ͼƬ
		AECHAR		*m_pwszTitle; //��������
		boolean		m_bInitSuccess; //�Ƿ�����ɹ�
		int			m_nRed;			//��ɫRֵ
		
		int			m_nWidth;		//ͼƬ���
		int			m_nHeight;		//ͼƬ�߶�
		AEERect		m_rctImage;		//ͼƬ��ʾ�ļ��о���

		int			m_nImageDrawX; //ͼƬ��ʾ�����Ͻ�X����
		int			m_nImageDrawY; //ͼƬ��ʾ�����Ͻ�Y����

		boolean		Init() ;
		void			Proc() ;
		void			Draw() ;
		void			KeyProc(int keycode, int mode) ;
		boolean		HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);
};

#endif