#ifndef __STATE__
#define __STATE__



class STATE   //����Ķ���
{
   public:
                  STATE(GAME * l_pApp);
   virtual        ~STATE();

   GAME          * pApp;  //GAME Ϊ��Ϸ����ģ��
   
   virtual boolean  Init() =0;  //��ʼ��
   virtual void    Proc() = 0;    //���ݴ���
   virtual void    Draw() = 0;    //��ʾ����
   virtual void    KeyProc(int keyCode,int keyMode) = 0;    //��������
   virtual boolean  HandleEvent(AEEEvent eCode, uint16 wParam, uint32 dwParam)=0; //�¼�����
  
};


#endif