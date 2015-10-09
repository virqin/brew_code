#include "PicDownload.h"
#include "PicDownload.bid"
#include "PicDownload_res.h"
#include "AEEStdLib.h"


#define RES_FILE PICDOWNLOAD_RES_FILE 

static  boolean PicDownload_HandleEvent(PicDownload* pMe, AEEEvent eCode, uint16 wParam,uint32 dwParam);
boolean PicDownload_InitAppData(PicDownload* pMe);
void    PicDownload_FreeAppData(PicDownload* pMe);
void PicDownload_ChangeState(PicDownload *pMe);
void PicDownload_InitMainMenu(PicDownload* pMe);
static void GetHostByName(PicDownload* pMe);
static void CBConnect(PicDownload *pMe,int nError);
static void PicDownload_MainLoop(PicDownload *pMe);
void SetFileData(PicDownload *pMe,byte *pData,int size);

boolean PicDownload_MainMenuHandleEvent(PicDownload *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
boolean PicDownload_FileListHandleEvent(PicDownload *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
boolean PicDownload_ViewHandleEvent(PicDownload *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
boolean PicDownload_FileViewHandleEvent(PicDownload *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

void ErrorTip(PicDownload *pMe,int16 errorId);

void SetTimeOut(PicDownload *pMe,int32 timeOut);
void TimeOut(PicDownload *pMe);
void CancelTimeOut(PicDownload *pMe);

static void CBSend(PicDownload *pMe);
static void CBRecv(PicDownload *pMe);
void RecvData(PicDownload *pMe,byte *pbData,int32 nRecvSize);
void SendData(PicDownload *pMe,byte *pbData,int32 nSendSize);
void Send(PicDownload *pMe,void *pData,int32 iSize);
void Recv(PicDownload *pMe,void *pData,int32 iSize);
void NetworkProcess(PicDownload *pMe);
void ProcessRecvData(PicDownload *pMe);
void UpdateScreen(PicDownload *pMe);
void PicDownload_InitNetwork(PicDownload *pMe);
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;
	if( ClsId == AEECLSID_PICDOWNLOAD )
	{
		if( AEEApplet_New(sizeof(PicDownload),
			ClsId,
			pIShell,
			po,
			(IApplet**)ppObj,
			(AEEHANDLER)PicDownload_HandleEvent,
			(PFNFREEAPPDATA)PicDownload_FreeAppData))
		{
			if(PicDownload_InitAppData((PicDownload*)*ppObj))
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

void NetworkRelease(PicDownload* pMe)
{
	if(pMe->pSocket)
	{
		ISOCKET_Cancel(pMe->pSocket,NULL,NULL);
		ISOCKET_Release(pMe->pSocket);
		pMe->pSocket = NULL;
	}
	if(pMe->pNetMgr)
	{
		INETMGR_Release(pMe->pNetMgr);
		pMe->pNetMgr = NULL;
	}
	if(pMe->m_pbRecvBuffer)
	{
		FREE(pMe->m_pbRecvBuffer);
		pMe->m_pbRecvBuffer = NULL;
	}
}
static boolean PicDownload_HandleEvent(PicDownload* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	switch (eCode) 
	{
	case EVT_APP_START:    
		ISHELL_SetTimerEx(pMe->pIShell,0,&pMe->cbMainTimer);
		IMENUCTL_SetActive(pMe->pIMenuCtlMain,TRUE);
		pMe->appState = enMainMenu;
		return(TRUE);
	case EVT_APP_STOP:
		return(TRUE);
	case EVT_APP_SUSPEND:
		switch(pMe->appState)
		{
		case enMainMenu:
			IMENUCTL_SetActive(pMe->pIMenuCtlMain,FALSE);
			break;
		case enWait:
			NetworkRelease(pMe);
			break;
		case enFileList:
			IMENUCTL_SetActive(pMe->pIMenuCtlList,FALSE);
			NetworkRelease(pMe);
			break;
		case enView:
			NetworkRelease(pMe);
			break;
		case enFileView:
			IMENUCTL_SetActive(pMe->pIMenuCtlList,FALSE);
			break;
		case enError:
			break;
		case enView2:
			break;
		}
		return(TRUE);
	case EVT_APP_RESUME:
		switch(pMe->appState)
		{
		case enMainMenu:
			IMENUCTL_SetActive(pMe->pIMenuCtlMain,TRUE);
			break;
		case enWait:
			PicDownload_InitNetwork(pMe);
			pMe->appState = enWait;
			break;
		case enFileList:
			PicDownload_InitNetwork(pMe);
			IMENUCTL_SetActive(pMe->pIMenuCtlList,FALSE);
			pMe->appState = enWait;
		case enView:
			PicDownload_InitNetwork(pMe);
			pMe->appState = enWait;
			break;
		case enFileView:
			IMENUCTL_SetActive(pMe->pIMenuCtlList,TRUE);
			break;
		case enError:
			break;
		case enView2:
			break;
		}
		return(TRUE);
	case EVT_APP_MESSAGE:
		return(TRUE);
	case EVT_APP_NO_SLEEP:
		return TRUE;
	default:
		{
			switch(pMe->appState)
			{
			case enMainMenu:
				return PicDownload_MainMenuHandleEvent(pMe,eCode,wParam,dwParam);
			case enFileList:
				return PicDownload_FileListHandleEvent(pMe,eCode,wParam,dwParam);
			case enView:
				return PicDownload_ViewHandleEvent(pMe,eCode,wParam,dwParam);
			case enView2:
				{
					if(eCode==EVT_KEY&&wParam==AVK_CLR)
					{
						IMENUCTL_SetActive(pMe->pIMenuCtlList,TRUE);
						pMe->appState = enFileView;
						return TRUE;
					}
				}
				break;
			case enFileView:
				return PicDownload_FileViewHandleEvent(pMe,eCode,wParam,dwParam);
			}
		}
		break;
	}
	return FALSE;
}
void NetworkProcess(PicDownload *pMe)
{	
	if(pMe->m_nNetState>=0)
	{
		if(pMe->m_bIsRecv)
		{
			if(pMe->m_bRecvBuf)
			{
				if(pMe->m_RecvDataHead.dwSize!=0)
				{
					if(!pMe->m_bIsReadBuf)
					{
						if(pMe->m_pbRecvBuffer)
						{
							FREE(pMe->m_pbRecvBuffer);
						}
						pMe->m_pbRecvBuffer = (byte*)MALLOC(pMe->m_RecvDataHead.dwSize);
						Recv(pMe,pMe->m_pbRecvBuffer,pMe->m_RecvDataHead.dwSize);
						pMe->m_bIsReadBuf = TRUE;
					}
					else
					{
						pMe->m_bIsRecv = FALSE;
						pMe->m_bIsReadBuf = FALSE;
						ProcessRecvData(pMe);
					}
				}
				else
				{
					pMe->m_bIsRecv = FALSE;
					pMe->m_bIsReadBuf = FALSE;
					ProcessRecvData(pMe);
				}
			}
		}
	}
}
void SetFileList(PicDownload *pMe,byte *pData,int size)
{
	int i;
	int length = size/(FILENAME_LENGTH*2);
	AECHAR buffer[FILENAME_LENGTH];
	IMENUCTL_DeleteAll(pMe->pIMenuCtlList);
	for(i=0;i<length;i++)
	{
		MEMSET(buffer,0,sizeof(buffer));
		MEMCPY(buffer,(AECHAR*)pData+i*FILENAME_LENGTH,sizeof(buffer));
		IMENUCTL_AddItem(pMe->pIMenuCtlList,RES_FILE,-1,i,buffer,i);
	}
	IMENUCTL_SetActive(pMe->pIMenuCtlList,TRUE);
	IMENUCTL_Redraw(pMe->pIMenuCtlList);
}
void ProcessRecvData(PicDownload *pMe)
{
	switch(pMe->m_RecvDataHead.dwCommandId)
	{
	case SET_FILE_LIST:
		{
			SetFileList(pMe,pMe->m_pbRecvData,pMe->m_RecvDataHead.dwSize);
			pMe->appState = enFileList;
		}
		break;
	case SET_FILE:
		{
			SetFileData(pMe,pMe->m_pbRecvBuffer,pMe->m_RecvDataHead.dwSize);
			pMe->appState = enView;
		}
		break;
	}
}
void UpdateScreen(PicDownload *pMe)
{
	switch(pMe->appState)
	{
	case enFileList:
		break;
	case enWait:
		{
			AEERect rect;
			AECHAR buffer[20]={0};
			rect.x = 0;
			rect.y= 0;
			rect.dx = pMe->DeviceInfo.cxScreen;
			rect.dy = pMe->DeviceInfo.cyScreen;
			IDISPLAY_FillRect(pMe->pIDisplay,&rect,MAKE_RGB(255,255,255));
			ISHELL_LoadResString(pMe->pIShell,RES_FILE,IDS_WAIT,buffer,sizeof(buffer));
			IDISPLAY_DrawText(pMe->pIDisplay,AEE_FONT_NORMAL,buffer,-1,
				0,0,NULL,
				IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
		}
		break;
	case enError:
		{
			AEERect rect;
			AECHAR buffer[20]={0};
			rect.x = 0;
			rect.y= 0;
			rect.dx = pMe->DeviceInfo.cxScreen;
			rect.dy = pMe->DeviceInfo.cyScreen;
			IDISPLAY_FillRect(pMe->pIDisplay,&rect,MAKE_RGB(255,255,255));
			ISHELL_LoadResString(pMe->pIShell,RES_FILE,pMe->m_nErrorId,buffer,sizeof(buffer));
			IDISPLAY_DrawText(pMe->pIDisplay,AEE_FONT_NORMAL,buffer,-1,
				0,0,NULL,
				IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
		}
		break;
	case enView:
	case enView2:
		IIMAGE_Draw(pMe->m_pIImage,0,0);
		break;
	}
	IDISPLAY_Update(pMe->pIDisplay);
}
static void PicDownload_MainLoop(PicDownload *pMe)
{
	ISHELL_SetTimerEx(pMe->pIShell,80,&pMe->cbMainTimer);
	NetworkProcess(pMe);
	UpdateScreen(pMe);

}
boolean PicDownload_InitAppData(PicDownload* pMe)
{
	pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
	ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);
	pMe->pIDisplay = pMe->a.m_pIDisplay;
	pMe->pIShell   = pMe->a.m_pIShell;
	PicDownload_InitMainMenu(pMe);
	pMe->m_nNetState = -1;
	CALLBACK_Init(&pMe->cbMainTimer,PicDownload_MainLoop,pMe);
	return TRUE;
}
void PicDownload_InitMainMenu(PicDownload* pMe)
{
	AEERect rect;
	ISHELL_CreateInstance(pMe->pIShell,AEECLSID_MENUCTL,(void**)&pMe->pIMenuCtlMain);

	rect.x = 0;
	rect.y = 0;
	rect.dx = pMe->DeviceInfo.cxScreen;
	rect.dy = pMe->DeviceInfo.cyScreen;
	IMENUCTL_SetRect(pMe->pIMenuCtlMain,&rect);
	IMENUCTL_SetTitle(pMe->pIMenuCtlMain,RES_FILE,IDS_MAINMENU_TITLE,NULL);
	IMENUCTL_AddItem(pMe->pIMenuCtlMain,RES_FILE,IDS_MAINMENU_DOWNLOAD,IDS_MAINMENU_DOWNLOAD,NULL,IDS_MAINMENU_DOWNLOAD);
	IMENUCTL_AddItem(pMe->pIMenuCtlMain,RES_FILE,IDS_MAINMENU_VIEW,IDS_MAINMENU_VIEW,NULL,IDS_MAINMENU_VIEW);
	IMENUCTL_AddItem(pMe->pIMenuCtlMain,RES_FILE,IDS_MAINMENU_EXIT,IDS_MAINMENU_EXIT,NULL,IDS_MAINMENU_EXIT);
	IMENUCTL_EnableCommand(pMe->pIMenuCtlMain,TRUE);

	ISHELL_CreateInstance(pMe->pIShell,AEECLSID_MENUCTL,(void**)&pMe->pIMenuCtlList);
	IMENUCTL_SetRect(pMe->pIMenuCtlList,&rect);
	IMENUCTL_SetTitle(pMe->pIMenuCtlList,RES_FILE,IDS_LIST_TITLE,NULL);
	IMENUCTL_EnableCommand(pMe->pIMenuCtlList,TRUE);

}
void PicDownload_FreeAppData(PicDownload* pMe)
{
	if(pMe->pIMenuCtlMain)
	{
		IMENUCTL_Release(pMe->pIMenuCtlMain);
		pMe->pIMenuCtlMain = NULL;
	}
	if(pMe->pIMenuCtlList)
	{
			IMENUCTL_Release(pMe->pIMenuCtlList);
		pMe->pIMenuCtlList = NULL;
	}
	if(pMe->m_pbRecvBuffer)
	{
		FREE(pMe->m_pbRecvBuffer);
		pMe->m_pbRecvBuffer = NULL;
	}
	if(pMe->pSocket)
	{
		ISOCKET_Cancel(pMe->pSocket,NULL,NULL);
		ISOCKET_Release(pMe->pSocket);
		pMe->pSocket = NULL;
	}
	if(pMe->pNetMgr)
	{
		INETMGR_Release(pMe->pNetMgr);
		pMe->pNetMgr = NULL;
	}
	if(pMe->m_pIImage)
	{
		IIMAGE_Release(pMe->m_pIImage);
		pMe->m_pIImage = NULL;
	}
}

void PicDownload_ChangeState(PicDownload *pMe)
{
}
boolean PicDownload_MainMenuHandleEvent(PicDownload *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	switch(eCode)
	{
	case EVT_COMMAND:
		{
			switch(wParam)
			{
			case IDS_MAINMENU_DOWNLOAD:
				IMENUCTL_SetActive(pMe->pIMenuCtlMain,FALSE);
				PicDownload_InitNetwork(pMe);
				pMe->appState = enWait;
				return TRUE;
			case IDS_MAINMENU_VIEW:
				IMENUCTL_SetActive(pMe->pIMenuCtlMain,FALSE);
				{
					IFileMgr *pIFileMgr;
					FileInfo fileInfo;
					AECHAR buffer[16]={0};
					int i = 0;
					ISHELL_CreateInstance(pMe->pIShell,AEECLSID_FILEMGR,(void**)&pIFileMgr);
					IFILEMGR_EnumInit(pIFileMgr,"*.png",FALSE);
					IMENUCTL_DeleteAll(pMe->pIMenuCtlList);
					while(IFILEMGR_EnumNext(pIFileMgr,&fileInfo))
					{
						MEMSET(buffer,0,sizeof(buffer));
						STRTOWSTR(fileInfo.szName,buffer,sizeof(buffer));
						IMENUCTL_AddItem(pMe->pIMenuCtlList,RES_FILE,-1,i,buffer,i);
						i++;
					}
					IFILEMGR_Release(pIFileMgr);
					IMENUCTL_SetActive(pMe->pIMenuCtlList,TRUE);
					pMe->appState = enFileView;
				}
				return TRUE;
			case IDS_MAINMENU_EXIT:
				ISHELL_CloseApplet(pMe->pIShell,FALSE);
				return TRUE;
			}
		}
		break;
	default:
		if(IMENUCTL_HandleEvent(pMe->pIMenuCtlMain,eCode,wParam,dwParam))
		{
			return TRUE;
		}
		else
		{
			if(eCode==EVT_KEY&&wParam==AVK_CLR)
			{
				return FALSE;
			}
		}
	}
	return FALSE;
}

void PicDownload_InitNetwork(PicDownload *pMe)
{
	ISHELL_CreateInstance(pMe->pIShell, AEECLSID_NET, (void **)&pMe->pNetMgr);
	INETMGR_SetLinger(pMe->pNetMgr,100);
	pMe->pSocket = INETMGR_OpenSocket(pMe->pNetMgr, AEE_SOCK_STREAM);
	CALLBACK_Init(&pMe->cbConnect,GetHostByName,pMe);
	pMe->m_nSendSize = 0;
	pMe->m_nRecvSize = 0;
	pMe->m_bRecvBuf = FALSE;
	pMe->m_bHasSend = FALSE;
	pMe->m_nNetState = -1;
	pMe->m_bIsReadBuf = FALSE;
	SetTimeOut(pMe,30);
	pMe->m_iPort = 2050;
	INETMGR_GetHostByName(pMe->pNetMgr, &pMe->m_DNSRes, "127.0.0.1", &pMe->cbConnect);
}
static void GetHostByName(PicDownload *pMe)
{
	switch(pMe->m_DNSRes.nResult)
	{
	case AEE_NET_BADDOMAIN:
	case AEE_NET_UNKDOMAIN: 
	case AEE_NET_ETIMEDOUT:
	case EUNSUPPORTED:
	case ENOMEMORY:
		//给出错误提示信息
		break;
	default:
		if(pMe->m_DNSRes.nResult > 0) // 成功解析
		{
			uint16 iPort;
			int nRes;
			iPort = HTONS(pMe->m_iPort);
			SetTimeOut(pMe,30);
			if((nRes = ISOCKET_Connect(pMe->pSocket, pMe->m_DNSRes.addrs[0], iPort, 
				(PFNCONNECTCB)CBConnect, (void *)pMe)) != AEE_NET_SUCCESS)
			{
				if (nRes != AEE_NET_WOULDBLOCK)
				{
					ErrorTip(pMe,IDS_ERROR_1);
				}
			}
		}
		else
		{
			ErrorTip(pMe,IDS_ERROR_1);
		}
		break;
	}
}

void SetTimeOut(PicDownload *pMe,int32 timeOut)
{
	ISHELL_CancelTimer(pMe->pIShell, (PFNNOTIFY)TimeOut, pMe);
	ISHELL_SetTimer(pMe->pIShell,timeOut*1000 , (PFNNOTIFY)TimeOut, pMe);
}
void CancelTimeOut(PicDownload *pMe)
{
	ISHELL_CancelTimer(pMe->pIShell, (PFNNOTIFY)TimeOut, pMe);
}
void TimeOut(PicDownload *pMe)
{
	ErrorTip(pMe,IDS_ERROR_TIMEOUT);
}

static void CBConnect(PicDownload *pMe,int nError)
{
	CancelTimeOut(pMe);
	if(ISOCKET_GetLastError(pMe->pSocket)!=AEE_NET_SUCCESS)
	{
		// 给出错误提示信息
		ErrorTip(pMe,IDS_ERROR_1);
		return;
	}
	//已经连接
	pMe->m_nNetState = 0;
	pMe->m_SendDataHead.dwCommandId = GET_FILE_LIST;
	pMe->m_SendDataHead.dwSize = 0;
	Send(pMe,&pMe->m_SendDataHead,sizeof(pMe->m_SendDataHead));
	Recv(pMe,&pMe->m_RecvDataHead,sizeof(pMe->m_RecvDataHead));
	pMe->m_bIsRecv = TRUE;
}
void ErrorTip(PicDownload *pMe,int16 errorId)
{
	pMe->appState = enError;
	pMe->m_nErrorId = errorId;
}


static void CBSend(PicDownload *pMe)
{
	SendData(pMe,pMe->m_pbSendData + pMe->m_nSendSize,pMe->m_nSendNum - pMe->m_nSendSize);
}

static void CBRecv(PicDownload *pMe)
{
	RecvData(pMe,pMe->m_pbRecvData+pMe->m_nRecvSize,pMe->m_nRecvNum-pMe->m_nRecvSize);
}

void SendData(PicDownload *pMe,byte *pbData,int32 nSendSize)
{
	int32 nResult;
	while(nSendSize > 0) 
	{
		SetTimeOut(pMe,30);
		nResult = ISOCKET_Write(pMe->pSocket,pbData, (uint16)nSendSize);
		if(nResult == AEE_NET_WOULDBLOCK) 
		{
			SetTimeOut(pMe,30);
			ISOCKET_Writeable(pMe->pSocket, (PFNNOTIFY)CBSend, (void *)pMe);
			return;
		}
		else if(nResult == AEE_NET_ERROR) 
		{
			ErrorTip(pMe,IDS_ERROR_1);
			return;
		}
		pMe->m_nSendSize += nResult;
		nSendSize -= nResult;
		pbData += nResult;
	}
	if(nSendSize == 0) 
	{
		pMe->m_nSendSize = 0;
		pMe->m_bHasSend = TRUE;
	}
	CancelTimeOut(pMe);
}

void RecvData(PicDownload *pMe,byte *pbData,int32 nRecvSize)
{
	int nResult;
	while(nRecvSize > 0)
	{
		SetTimeOut(pMe,30);
		nResult = ISOCKET_Read(pMe->pSocket, pbData, nRecvSize);
		if(nResult == AEE_NET_WOULDBLOCK)
		{
			SetTimeOut(pMe,30);
			ISOCKET_Readable(pMe->pSocket, (PFNNOTIFY)CBRecv, (void*)pMe); 
			return;
		}
		else if(nResult == AEE_NET_ERROR) 
		{
			ErrorTip(pMe,IDS_ERROR_1);
			return;
		}
		else if(nResult > 0)
		{
			pMe->m_nRecvSize += nResult;
			nRecvSize -= nResult;
			pbData += nResult;
		}
		else
		{
			ErrorTip(pMe,IDS_ERROR_1);
			return;
		}
	}
	if(nRecvSize == 0)
	{
		pMe->m_nRecvSize = 0;
		pMe->m_bRecvBuf = TRUE;
	}
	CancelTimeOut(pMe);
}
void Send(PicDownload *pMe,void *pData,int32 iSize)
{
	if(!pMe->pSocket)
	{
		return;
	}
	pMe->m_pbSendData = (byte*)pData;
	pMe->m_nSendNum = iSize;
	pMe->m_bHasSend = FALSE;
	ISOCKET_Cancel(pMe->pSocket, (PFNNOTIFY)CBSend, (void *)pMe);
	SendData(pMe,pMe->m_pbSendData,iSize);
}

void Recv(PicDownload *pMe,void *pData,int32 iSize)
{
	if(!pMe->pSocket)
	{
		return;
	}
	pMe->m_pbRecvData = (byte*)pData;
	pMe->m_nRecvNum = iSize;
	pMe->m_bRecvBuf = FALSE;
	ISOCKET_Cancel(pMe->pSocket, (PFNNOTIFY)CBRecv, (void *)pMe);
	RecvData(pMe,pMe->m_pbRecvData,iSize);
}
boolean PicDownload_FileListHandleEvent(PicDownload *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	switch(eCode)
	{
	case EVT_COMMAND:
		{
			pMe->m_nItemId = IMENUCTL_GetSel(pMe->pIMenuCtlList);
			IMENUCTL_SetActive(pMe->pIMenuCtlList,FALSE);
			{
				pMe->m_nSendBufferSize = 0;
				pMe->m_SendDataHead.dwCommandId = GET_FILE;
				pMe->m_SendDataHead.dwSize = sizeof(pMe->m_nItemId);
				MEMCPY(pMe->m_bSendBuffer,&pMe->m_SendDataHead,sizeof(pMe->m_SendDataHead));
				pMe->m_nSendBufferSize += sizeof(pMe->m_SendDataHead);
				MEMCPY(pMe->m_bSendBuffer+pMe->m_nSendBufferSize,&pMe->m_nItemId,sizeof(pMe->m_nItemId));
				pMe->m_nSendBufferSize += sizeof(pMe->m_nItemId);
				Send(pMe,pMe->m_bSendBuffer,pMe->m_nSendBufferSize);
				pMe->m_bIsRecv = TRUE;
				Recv(pMe,&pMe->m_RecvDataHead,sizeof(pMe->m_RecvDataHead));
				pMe->appState = enWait;
			}
			return TRUE;
		}
		break;
	default:
		if(IMENUCTL_HandleEvent(pMe->pIMenuCtlList,eCode,wParam,dwParam))
		{
			return TRUE;
		}
		else
		{
			if(eCode==EVT_KEY&&wParam==AVK_CLR)
			{
				return FALSE;
			}
		}
	}
	return FALSE;
}

void SetFileData(PicDownload *pMe,byte *pData,int size)
{
	byte *pBuffer = (byte*)MALLOC(size);
	IMemAStream *pIStream;
	if(pBuffer==NULL)
	{
		//这个地方应该报错
	}
	MEMCPY(pBuffer,pData,size);
	ISHELL_CreateInstance(pMe->pIShell, AEECLSID_MEMASTREAM, (void **)&pIStream);
	IMEMASTREAM_Set(pIStream,pBuffer,size,0,FALSE);
	if(pMe->m_pIImage)
	{
		IIMAGE_Release(pMe->m_pIImage);
	}
	ISHELL_CreateInstance(pMe->pIShell, AEECLSID_PNG, (void **)&pMe->m_pIImage);
	IIMAGE_SetStream(pMe->m_pIImage,(IAStream*)pIStream);
	pMe->m_nFileSize = size;
	IMEMASTREAM_Release(pIStream);
}
boolean PicDownload_ViewHandleEvent(PicDownload *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	switch(eCode)
	{
	case EVT_KEY:
		switch(wParam)
		{
		case AVK_CLR:
			{
				pMe->appState = enFileList;
				IMENUCTL_SetActive(pMe->pIMenuCtlList,TRUE);
				return TRUE;
			}
			break;
		case AVK_1:
			{
				IFileMgr *pIFileMgr;
				IFile *pIFile;
				CtlAddItem item;
				char buffer[20]={0};
				char buffer2[20] = {0};
				ISHELL_CreateInstance(pMe->pIShell,AEECLSID_FILEMGR,(void**)&pIFileMgr);
				IMENUCTL_GetItem(pMe->pIMenuCtlList,pMe->m_nItemId,&item);
				WSTRTOSTR(item.pText,buffer,sizeof(buffer));
				SPRINTF(buffer2,"%s.png",buffer);
				pIFile = IFILEMGR_OpenFile(pIFileMgr,buffer2,_OFM_CREATE);
				IFILE_Write(pIFile,pMe->m_pbRecvBuffer,pMe->m_nFileSize);
				IFILE_Release(pIFile);
				IFILEMGR_Release(pIFileMgr);
				return TRUE;
			}
			break;
		}
		break;
	}
	return FALSE;
}
boolean PicDownload_FileViewHandleEvent(PicDownload *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	switch(eCode)
	{
	case EVT_COMMAND:
		{
			pMe->m_nItemId = IMENUCTL_GetSel(pMe->pIMenuCtlList);
			IMENUCTL_SetActive(pMe->pIMenuCtlList,FALSE);
			{
				CtlAddItem item;
				char buffer[20]={0};
				IMENUCTL_GetItem(pMe->pIMenuCtlList,pMe->m_nItemId,&item);
				WSTRTOSTR(item.pText,buffer,sizeof(buffer));
				if(pMe->m_pIImage)
				{
					IIMAGE_Release(pMe->m_pIImage);
				}
				pMe->m_pIImage = ISHELL_LoadImage(pMe->pIShell,buffer);
				pMe->appState = enView2;
			}
			return TRUE;
		}
		break;
	default:
		if(IMENUCTL_HandleEvent(pMe->pIMenuCtlList,eCode,wParam,dwParam))
		{
			return TRUE;
		}
		else
		{
			if(eCode==EVT_KEY&&wParam==AVK_CLR)
			{
				IMENUCTL_SetActive(pMe->pIMenuCtlList,FALSE);
				IMENUCTL_SetActive(pMe->pIMenuCtlMain,TRUE);
				pMe->appState = enMainMenu;
				return TRUE;
			}
		}
	}
	return FALSE;
}