#include "fk.h"
#include "read.h"
#include "helpfunc.h"

#define  FILE_NAME "a.txt"

read::read(GAME *lgpp):STATE(lgpp)
	 {
		 m_pIShell = pApp->m_pIShell;
		 m_pIDisplay = pApp->m_pIDisplay;

	 }
read::~read()
{
	RELEASEIF(pIStatic);
}


boolean read::Init()
{
	AEERect rctText;
	AEEDeviceInfo di;
	//di.wStructSize = sizeof(AEEDeviceInfo);
	AECHAR * pwszContent=NULL;
	int nLen=0;
	AECHAR pwszTitle[5]= {0x8FD9,0x662F,0x6807,0x9898,0};    //这是标题

	ISHELL_GetDeviceInfo(m_pIShell,&di);
	SETAEERECT(&rctText,0,0,di.cxScreen,di.cyScreen);
	if(SUCCESS!=ISHELL_CreateInstance(			//这一句建立ISTATIC接口
		m_pIShell,
		AEECLSID_STATIC,							//ISTATIC接口的CLASS ID
		(void**)&(pIStatic)				//ISTATIC接口实例的地址的指针，必须是void**型的 
		))
	{
		return FALSE;                          //接口建立失败则返回FALSE
	}
	if((IShell_TestFile(m_pIShell,FILE_NAME)) == TRUE)
	{
		pwszContent=(AECHAR*)LoadDataFromFile(m_pIShell,
			FILE_NAME,&nLen);//读文件内容并将内容缓冲区地址赋予pwszContent，请仔细阅读该函数的内部实现
		//这个函数揭示了如何使用文件接口读取文件内容

	}

	

	if(pwszContent==NULL)
	{
		return FALSE;  //内容载入失败，则返回FASLSE
	}

	ISTATIC_SetRect(pIStatic,&rctText);  //设置ISTATIC控件的屏幕矩形

	ISTATIC_SetText(pIStatic,pwszTitle,pwszContent,AEE_FONT_NORMAL,AEE_FONT_NORMAL);//设置ISTATIC控件内容
	ISTATIC_SetActive(pIStatic,TRUE);  //激活控件，否则控件不接受事件

	FREEIF(pwszContent);  //释放申请的空间

	return TRUE;
}

void read::Proc()
{

}

void read::Draw()
{
	IDISPLAY_ClearScreen(m_pIDisplay);

	ISTATIC_Redraw(pIStatic);

	IDISPLAY_Update(m_pIDisplay);

}

void read::KeyProc(int keyCode,int Mode)
{
	if(Mode==EVT_KEY)
	{
		switch(keyCode) 
		{
		case AVK_CLR:
			pApp->del_state=pApp->cur_state;
			pApp->cur_state=new MainMenu(pApp);
			pApp->changeState=TRUE;
			break;
		case AVK_SELECT:
			ISHELL_PostEvent(m_pIShell,AEECLSID_EG,EVT_USER_RESET,0,0);  //发送画面重置事件
			break;
		default:
			break;
		}
	}

}

boolean read:: HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	switch(eCode) 
	{
	case EVT_USER_RESET:
		

		break;
	default:
		;
	}
	return TRUE;
}

