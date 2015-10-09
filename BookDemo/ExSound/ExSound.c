#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions

#include "AEEFile.h"			// File interface definitions
#include "AEESound.h"			// Sound Interface definitions
#include "AEESoundPlayer.h"
#include "AEEFile.h"
#include "AEEMedia.h"
#include "AEEStdLib.h"
#include "ExSound.bid"
#include "ExSound_res.h"

typedef struct _ExSound {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
	AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
	IDisplay      *pIDisplay;  // give a standard way to access the Display interface
	IShell        *pIShell;    // give a standard way to access the Shell interface
	IMedia *pIMedia;
	boolean bContinue;
	byte *pbBuffer;

} ExSound;

static  boolean ExSound_HandleEvent(ExSound* pMe, 
																		AEEEvent eCode, uint16 wParam, 
																		uint32 dwParam);
boolean ExSound_InitAppData(ExSound* pMe);
void    ExSound_FreeAppData(ExSound* pMe);

int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;
	if( ClsId == AEECLSID_EXSOUND )
	{
		if( AEEApplet_New(sizeof(ExSound),
			ClsId,
			pIShell,
			po,
			(IApplet**)ppObj,
			(AEEHANDLER)ExSound_HandleEvent,
			(PFNFREEAPPDATA)ExSound_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
		{
			if(ExSound_InitAppData((ExSound*)*ppObj))
			{
				return(AEE_SUCCESS);
			}
			else
			{
				IAPPLET_Release((IApplet*)*ppObj);
				return EFAILED;
			}
		}
	}
	return(EFAILED);
}
static void CBSound(void *pUser,AEESoundPlayerCmd eCBType,AEESoundPlayerStatus eSPStatus,uint32 dwParam)
{
	switch(eCBType)
	{
	case AEE_SOUNDPLAYER_STATUS_CB:
		break;
	case AEE_SOUNDPLAYER_PLAY_CB:
		break;
	case AEE_SOUNDPLAYER_TIME_CB:
		break;
	case AEE_SOUNDPLAYER_SOUND_CB:
		break;
	case AEE_SOUNDPLAYER_VOLUME_CB:
		break;
	}
}

static void CBMediaNotify(void *pUser,AEEMediaCmdNotify *pCmdNotify)
{
	ExSound *pMedia = (ExSound*)pUser;
	if(pMedia->bContinue)
	{
		if( pCmdNotify->nCmd == MM_CMD_PLAY)
		{
			if(pCmdNotify->nStatus == MM_STATUS_DONE)
			{
				IMEDIA_Play(pMedia->pIMedia);
			}
		}
	}
}
static boolean ExSound_HandleEvent(ExSound* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  

	switch (eCode) 
	{
	case EVT_APP_START:    
		{
			//AEESoundPlayerInfo info;
			/*
			ISHELL_CreateInstance(pMe->pIShell,AEECLSID_SOUNDPLAYER,(void**)&pMe->pISoundPlayer);
			ISOUNDPLAYER_RegisterNotify(pMe->pISoundPlayer,CBSound,pMe);
			info.eInput = SDT_FILE;
			info.pData = "begin.mid";
			
			if(ISOUNDPLAYER_SetInfo(pMe->pISoundPlayer,&info)!=SUCCESS)
			{
				//Ê§°Ü´¦Àí
			}
			ISOUNDPLAYER_SetVolume(pMe->pISoundPlayer,10);
			ISOUNDPLAYER_SetTempo(pMe->pISoundPlayer,1);
			ISOUNDPLAYER_FastForward(pMe->pISoundPlayer,5000);*/
			//pMe->pISoundPlayer = ISHELL_LoadSound(pMe->pIShell,"begin.mid");
			//ISOUNDPLAYER_Play(pMe->pISoundPlayer);
			
			AEEMediaData data;
			byte *pBuffer;
			uint32 bufSize;
			ISHELL_CreateInstance(pMe->pIShell,AEECLSID_MEDIAMIDI,(void**)&pMe->pIMedia);
	
			ISHELL_GetResSize(pMe->pIShell,EXSOUND_RES_FILE,IDM_BEGIN,RESTYPE_IMAGE,&bufSize);
			pMe->pbBuffer = ISHELL_LoadResData(pMe->pIShell,EXSOUND_RES_FILE,IDM_BEGIN,RESTYPE_IMAGE);
			pBuffer = RESBLOB_DATA((AEEResBlob*)pMe->pbBuffer);

			data.clsData = MMD_BUFFER;
			data.dwSize = bufSize-((AEEResBlob*)pMe->pbBuffer)->bDataOffset;
			data.pData = pBuffer;
			
			IMEDIA_SetMediaData(pMe->pIMedia,&data);
			IMEDIA_RegisterNotify(pMe->pIMedia,CBMediaNotify,pMe);
			pMe->bContinue = TRUE;
			IMEDIA_Play(pMe->pIMedia);
			
			/*
			AEEMediaData data;
			data.clsData=MMD_FILE_NAME;
			data.pData="begin.mid";
			AEEMediaUtil_CreateMedia(pMe->pIShell,&data,&pMe->pIMedia);
			IMEDIA_Play(pMe->pIMedia);
			*/

		}

		return(TRUE);

	case EVT_APP_STOP:

		return(TRUE);

	case EVT_APP_SUSPEND:
		return(TRUE);
	case EVT_APP_RESUME:


		return(TRUE);
	case EVT_APP_MESSAGE:

		return(TRUE);
	case EVT_KEY:

		return(TRUE);

	default:
		break;
	}

	return FALSE;
}

boolean ExSound_InitAppData(ExSound* pMe)
{
	
	pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
	ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

	pMe->pIDisplay = pMe->a.m_pIDisplay;
	pMe->pIShell   = pMe->a.m_pIShell;
	
	return TRUE;
}

void ExSound_FreeAppData(ExSound* pMe)
{
	/*
	if(pMe->pISoundPlayer)
	{
		ISOUNDPLAYER_Stop(pMe->pISoundPlayer);
		ISOUNDPLAYER_RegisterNotify(pMe->pISoundPlayer,NULL,NULL);
		ISOUNDPLAYER_Release(pMe->pISoundPlayer);
		pMe->pISoundPlayer = NULL;
	}*/
	if(pMe->pIMedia)
	{
		IMEDIA_Stop(pMe->pIMedia);
		ISHELL_FreeResData(pMe->pIShell,pMe->pbBuffer);
		IMEDIA_RegisterNotify(pMe->pIMedia,NULL,NULL);
		IMEDIA_Release(pMe->pIMedia);
		pMe->pIMedia = NULL;
	}
}
