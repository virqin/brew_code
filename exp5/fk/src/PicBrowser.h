#ifndef _H_FIRST_WINDOW
#define _H_FIRST_WINDOW





//////////////////////
//窗口模块定义

class PicBrowser : public STATE
{
	public:
		PicBrowser (GAME * l_papp);
		~PicBrowser();
	
		IShell		*m_pIShell;
		IDisplay		*m_pIDisplay;
	
		IImage		*m_pImage;    //图片
		AECHAR		*m_pwszTitle; //标题文字
		boolean		m_bInitSuccess; //是否载入成功
		int			m_nRed;			//颜色R值
		
		int			m_nWidth;		//图片宽度
		int			m_nHeight;		//图片高度
		AEERect		m_rctImage;		//图片显示的剪切矩形

		int			m_nImageDrawX; //图片显示的左上角X坐标
		int			m_nImageDrawY; //图片显示的左上角Y坐标

		boolean		Init() ;
		void			Proc() ;
		void			Draw() ;
		void			KeyProc(int keycode, int mode) ;
		boolean		HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam);
};

#endif