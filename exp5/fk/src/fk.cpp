/*
*  �ļ����ƣ�fk.cpp

*  ժҪ��
		����ļ���������Brew�����ص������Ĵ���


*  �汾��1.0.1
*  �޸��ߣ�����è
*  ������ڣ�2008��8��21��
* 
*  ȡ���汾��1.0.0 
*  ԭ����  ������
*  ������ڣ�δ֪

*/ 
#include "fk.h"

extern "C" 

///////////////////////////////
//����ʵ�ֲ���

int AEEClsCreateInstance(AEECLSID ClsId, IShell * plShell, IModule * po, void ** ppObj)
{
   *ppObj = NULL;

   if ( ClsId == AEECLSID_EG )
   {
      if ( AEEApplet_New(sizeof(GAME), ClsId, plShell, po, (IApplet * *) ppObj, (AEEHANDLER) EG_HandleEvent, (PFNFREEAPPDATA) EG_FreeAppData) == TRUE )
      {
         EG_InitAppData((GAME *) *ppObj);
         return (AEE_SUCCESS);
      }
   }

   return (EFAILED);
}

static boolean EG_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   GAME       * pGame   = (GAME *) pi;
   AEEApplet  * pA      = (AEEApplet *) pi;
	
   switch ( eCode )
   {
      case EVT_APP_START:
         pGame->InitGame();
         return (TRUE);
      case EVT_APP_STOP:
         return TRUE;
      case EVT_APP_NO_SLEEP:
         return TRUE;
      case EVT_APP_NO_CLOSE:
         return FALSE;
      case EVT_APP_SUSPEND:
		  pGame->OnSuspend();
         return TRUE;
      case EVT_APP_RESUME:
		  pGame->OnResume();
         return TRUE;

      case EVT_KEY:
      case EVT_KEY_PRESS:
      case EVT_KEY_RELEASE:
			pGame->HandleKeyEvent(wParam, eCode);
			break;
		case EVT_COMMAND:
      default:
			pGame->cur_state->HandleEvent(eCode,wParam,dwParam);     
   }
   return TRUE;
}

void EG_InitAppData(GAME * pGame)
{
}

void EG_FreeAppData(GAME * pGame)
{
   pGame->ExitGame();
} 

void FreeIF(IBase ** ppif)
{
   if ( ppif && *ppif )
   {
      IBASE_Release(*ppif);
      *ppif = NULL;
   }
}


//  ����������


 void  *  operator  new(size_t size)
{
   void * memptr  = 0;
   memptr = MALLOC(size);
   MEMSET(memptr, 0, size);
   return memptr;
}

void  operator delete(void * p)
{
   if ( p )
   {
      FREE(p);p = NULL;
   }
}


void * operator new[](size_t size)
{
   void * memptr  = 0;
   memptr = MALLOC(size);
   MEMSET(memptr, 0, size);
   return memptr;
}

void operator delete[](void * p)
{
   if ( p )
   {
      FREE(p);p = NULL;
   }
}



