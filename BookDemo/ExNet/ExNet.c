#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEStdLib.h"

#include "AEEFile.h"			// File interface definitions
#include "AEENet.h"				// Socket interface definitions

#include "ExNet.bid"
typedef struct _ExNet {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
	AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
	IDisplay      *pIDisplay;  // give a standard way to access the Display interface
	IShell        *pIShell;    // give a standard way to access the Shell interface


	INetMgr *m_pNetMgr; //网络管理接口
	ISocket *m_pSocket; //套接字
uint32 m_iNetState;//网络状态，可以查询，0代表成功，除了0之外，全部为错误状态
uint16 m_iPort; //端口号
	AEECallback m_cbConnect;	//连接回调函数
	AEEDNSResult m_DNSRes; //DNS结果
	boolean m_bRecvBuf;//查询是否buffer已经读取完毕
	boolean m_bHasSend;//查询是否已经发送完毕，只有发送完毕才能够接收。
	
		
	AECHAR m_bSendBuffer[4];
	AECHAR m_bRecvBuffer[4];
	byte *m_pbData;
	int32 m_nSendSize;
	int32 m_nRecvSize;
	int32 m_nSize;

} ExNet;

static  boolean ExNet_HandleEvent(ExNet* pMe, 
																	AEEEvent eCode, uint16 wParam, 
																	uint32 dwParam);
boolean ExNet_InitAppData(ExNet* pMe);
void    ExNet_FreeAppData(ExNet* pMe);
void SendData(ExNet *pMe,byte *pbData,int32 nSendSize);
void RecvData(ExNet *pMe,byte *pbData,int32 nRecvSize);
static void CBSend(ExNet* pMe);
static void CBRecv(ExNet* pMe);
static void GetHostByName(ExNet *pMe);
static void CBConnect(ExNet *pMe,int /*nError*/);
void Connect(ExNet *pMe,const char *pszAddr,uint16 iPort);
int InitNetwork(ExNet *pMe);
void Connect(ExNet *pMe,const char *pszAddr,uint16 iPort);

int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;

	if( ClsId == AEECLSID_EXNET )
	{
		if( AEEApplet_New(sizeof(ExNet),
			ClsId,
			pIShell,
			po,
			(IApplet**)ppObj,
			(AEEHANDLER)ExNet_HandleEvent,
			(PFNFREEAPPDATA)ExNet_FreeAppData) )
		{
			if(ExNet_InitAppData((ExNet*)*ppObj))
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

static boolean ExNet_HandleEvent(ExNet* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  

	switch (eCode) 
	{
		// App is told it is starting up
	case EVT_APP_START:                        
		// Add your code here...
		InitNetwork(pMe);
		Connect(pMe,"localhost",10200);

		return(TRUE);


		// App is told it is exiting
	case EVT_APP_STOP:
		// Add your code here...

		return(TRUE);


		// App is being suspended 
	case EVT_APP_SUSPEND:
		// Add your code here...

		return(TRUE);


		// App is being resumed
	case EVT_APP_RESUME:

		return(TRUE);

	case EVT_APP_MESSAGE:
		// Add your code here...

		return(TRUE);
	case EVT_KEY:


		return(TRUE);
	default:
		break;
	}

	return FALSE;
}

boolean ExNet_InitAppData(ExNet* pMe)
{
	pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
	ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);
	pMe->pIDisplay = pMe->a.m_pIDisplay;
	pMe->pIShell   = pMe->a.m_pIShell;
	pMe->m_iPort = 10200;
	return TRUE;
}
void ExNet_FreeAppData(ExNet* pMe)
{

}

static void CBSend(ExNet* pMe)
{
	SendData(pMe,pMe->m_pbData + pMe->m_nSendSize,pMe->m_nSize - pMe->m_nSendSize);
}

static void CBRecv(ExNet* pMe)
{
	RecvData(pMe,pMe->m_pbData+pMe->m_nRecvSize,pMe->m_nSize-pMe->m_nRecvSize);
}

void Send(ExNet* pMe,void *pData,int32 iSize)
{
	if(!pMe->m_pSocket)
	{
		return;
	}
	pMe->m_pbData = (byte*)pData;
	pMe->m_nSize = iSize;
	pMe->m_bHasSend = FALSE;
	ISOCKET_Cancel(pMe->m_pSocket, (PFNNOTIFY)CBRecv, (void *)pMe);
	ISOCKET_Cancel(pMe->m_pSocket, (PFNNOTIFY)CBSend, (void *)pMe);

	SendData(pMe,pMe->m_pbData,iSize);
}

void Recv(ExNet *pMe,void *pData,int32 iSize)
{
	if(!pMe->m_pSocket)
	{
		return;
	}
	pMe->m_pbData = (byte*)pData;
	pMe->m_nSize = iSize;
	pMe->m_bRecvBuf = FALSE;
	ISOCKET_Cancel(pMe->m_pSocket, (PFNNOTIFY)CBRecv, (void *)pMe);
	ISOCKET_Cancel(pMe->m_pSocket, (PFNNOTIFY)CBSend, (void *)pMe);

	RecvData(pMe,pMe->m_pbData,iSize);
}

void SendData(ExNet *pMe,byte *pbData,int32 nSendSize)
{
	int32 nResult;
	while(nSendSize > 0) 
	{
		nResult = ISOCKET_Write(pMe->m_pSocket,pbData, (uint16)nSendSize);

		if(nResult == AEE_NET_WOULDBLOCK) 
		{
			ISOCKET_Writeable(pMe->m_pSocket, (PFNNOTIFY)CBSend, (void *)pMe);
			return;
		}
		else if(nResult == AEE_NET_ERROR) 
		{
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
		Recv(pMe,pMe->m_bRecvBuffer,sizeof(pMe->m_bRecvBuffer));
	}
}

void RecvData(ExNet *pMe,byte *pbData,int32 nRecvSize)
{
	int nResult;
	while(nRecvSize > 0)
	{
		nResult = ISOCKET_Read(pMe->m_pSocket, pbData, nRecvSize);

		if(nResult == AEE_NET_WOULDBLOCK)
		{
			ISOCKET_Readable(pMe->m_pSocket, (PFNNOTIFY)CBRecv, (void*)pMe); 
			return;
		}
		else if(nResult == AEE_NET_ERROR) 
		{
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
			return;
		}
	}
	if(nRecvSize == 0)
	{
		pMe->m_nRecvSize = 0;
		pMe->m_bRecvBuf = TRUE;
		IDISPLAY_DrawText(pMe->pIDisplay,AEE_FONT_NORMAL,pMe->m_bRecvBuffer,-1,0,0,NULL,IDF_TEXT_TRANSPARENT);
		IDISPLAY_Update(pMe->a.m_pIDisplay);
	}
}


void GetHostByName(ExNet *pMe)
{
	switch(pMe->m_DNSRes.nResult)
	{
	case AEE_NET_BADDOMAIN:
	case AEE_NET_UNKDOMAIN: 
	case AEE_NET_ETIMEDOUT:
	case EUNSUPPORTED:
	case AEE_NET_GENERAL_FAILURE:
	case AEE_NET_EOPNOTSUPP:
		//给出错误提示信息
		break;
	default:
		if(pMe->m_DNSRes.nResult > 0) // 成功解析
		{
			uint16 iPort;
			int nRes;
			AECHAR buffer[] ={'S','u','c','e','s','s',0};
			//IDISPLAY_DrawText(pMe->pIDisplay,AEE_FONT_NORMAL,buffer,-1,0,0,NULL,IDF_TEXT_TRANSPARENT);
			//IDISPLAY_Update(pMe->a.m_pIDisplay);
			iPort = HTONS(pMe->m_iPort);
			if((nRes = ISOCKET_Connect(pMe->m_pSocket, pMe->m_DNSRes.addrs[0], iPort, 
				(PFNCONNECTCB)CBConnect, (void *)pMe)) != AEE_NET_SUCCESS)
			{
			}
		}
		else
		{
		}
		break;
	}
}

void CBConnect(ExNet *pMe,int nError)
{
	if(ISOCKET_GetLastError(pMe->m_pSocket)!=AEE_NET_SUCCESS)
	{
		// 给出错误提示信息
		//pNet->m_pApp->ErrorTip(IDS_ERROR_CONNECT);
		return;
	}
	pMe->m_iNetState = 0;	
	pMe->m_bSendBuffer[0] = '1';
	pMe->m_bSendBuffer[1] = '2';
	pMe->m_bSendBuffer[2] = '3';
	pMe->m_bSendBuffer[3] = 0;
	Send(pMe,pMe->m_bSendBuffer,sizeof(pMe->m_bSendBuffer));
	
}
int InitNetwork(ExNet *pMe)
{
	if(ISHELL_CreateInstance(pMe->pIShell, AEECLSID_NET, (void **)&pMe->m_pNetMgr)!=SUCCESS)
	{
		return EFAILED;
	}
	pMe->m_pSocket = INETMGR_OpenSocket(pMe->m_pNetMgr, AEE_SOCK_STREAM);
	CALLBACK_Init(&pMe->m_cbConnect,GetHostByName,pMe);
	return SUCCESS;
}

void Connect(ExNet *pMe,const char *pszAddr,uint16 iPort)
{
	// 设置超时
	// 超时到，设置错误状态
	pMe->m_iPort = iPort;
	INETMGR_GetHostByName(pMe->m_pNetMgr, &pMe->m_DNSRes, pszAddr, &pMe->m_cbConnect);
}