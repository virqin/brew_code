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
	AECHAR pwszTitle[5]= {0x8FD9,0x662F,0x6807,0x9898,0};    //���Ǳ���

	ISHELL_GetDeviceInfo(m_pIShell,&di);
	SETAEERECT(&rctText,0,0,di.cxScreen,di.cyScreen);
	if(SUCCESS!=ISHELL_CreateInstance(			//��һ�佨��ISTATIC�ӿ�
		m_pIShell,
		AEECLSID_STATIC,							//ISTATIC�ӿڵ�CLASS ID
		(void**)&(pIStatic)				//ISTATIC�ӿ�ʵ���ĵ�ַ��ָ�룬������void**�͵� 
		))
	{
		return FALSE;                          //�ӿڽ���ʧ���򷵻�FALSE
	}
	if((IShell_TestFile(m_pIShell,FILE_NAME)) == TRUE)
	{
		pwszContent=(AECHAR*)LoadDataFromFile(m_pIShell,
			FILE_NAME,&nLen);//���ļ����ݲ������ݻ�������ַ����pwszContent������ϸ�Ķ��ú������ڲ�ʵ��
		//���������ʾ�����ʹ���ļ��ӿڶ�ȡ�ļ�����

	}

	

	if(pwszContent==NULL)
	{
		return FALSE;  //��������ʧ�ܣ��򷵻�FASLSE
	}

	ISTATIC_SetRect(pIStatic,&rctText);  //����ISTATIC�ؼ�����Ļ����

	ISTATIC_SetText(pIStatic,pwszTitle,pwszContent,AEE_FONT_NORMAL,AEE_FONT_NORMAL);//����ISTATIC�ؼ�����
	ISTATIC_SetActive(pIStatic,TRUE);  //����ؼ�������ؼ��������¼�

	FREEIF(pwszContent);  //�ͷ�����Ŀռ�

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
			ISHELL_PostEvent(m_pIShell,AEECLSID_EG,EVT_USER_RESET,0,0);  //���ͻ��������¼�
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

