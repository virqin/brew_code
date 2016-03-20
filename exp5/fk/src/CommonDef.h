#ifndef __COMMONDEF__
#define __COMMONDEF__


//////////////////////
//��������
#define MAX_STRING_LEN  100      //�����Դ�ַ�������
#define DEFAULT_INTERVAL 40		//Ĭ��֡ʱ����
#define MIN_INTERVAL		 1			//��С֡ʱ����


//////////////////////
//�Զ����¼�
#define EVT_USER_RESET   EVT_USER+1 //���������¼�


/////////////////////////////////////////
//��ʾ���ֶ���
const AECHAR WSZTIP1[10]=  {0x0031,0x0020,0x0050,0x0072,0x0065,0x0073,0x0073,0x0065,0x0064,0};    //1 Pressed
const AECHAR WSZTIP2[10]=  {0x0032,0x0020,0x0050,0x0072,0x0065,0x0073,0x0073,0x0065,0x0064,0};    //2 Pressed
const AECHAR WSZTIP3[10]=  {0x0033,0x0020,0x0050,0x0072,0x0065,0x0073,0x0073,0x0065,0x0064,0};    //3 Pressed



/////////////////////////////////////////
//�˵���ID����
#define MENU_ITEM_1  0
#define MENU_ITEM_2  1
#define MENU_ITEM_3  2
#define MENU_ITEM_4  3


/////////////////////////////////////////
//��ɫ����
#define  RGB_YELLOW MAKE_RGB(255,255,0)
#define  RGB_GREEN MAKE_RGB(0,255,0)
#define  RGB_RED MAKE_RGB(255,0,0)

#define MENUCTL_COLOR_MASK MC_FRAME| MC_BACK | MC_SEL_BACK |MC_SEL_TEXT |MC_TEXT| MC_SCROLLBAR_FILL | MC_SCROLLBAR

#define MENUCTL_FRAME_COLOR RGB_RED

#define MENUCTL_SELECT_TEXT_COLOR RGB_GREEN
#define MENUCTL_UNSELECT_TEXT_COLOR RGB_GREEN

#define MENUCTL_SELECT_ITEM_COLOR MAKE_RGB(0,0,0)
#define MENUCTL_UNSELECT_ITEM_COLOR MAKE_RGB(0,0,0)

#define MENUCTL_SCROLL_BAR_COLOR RGB_GREEN
#define MENUCTL_SCROLL_BAR_FILLCOLOR MAKE_RGB(0,0,0)






///////////////////////
//���������
#define RELEASEIF(p)          FreeIF((IBase **)&(p))
#define DELETEIF(p)  { if(p) { delete (p);     (p)=NULL; } }


#endif