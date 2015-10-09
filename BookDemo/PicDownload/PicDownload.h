#ifndef PICDOWNLOAD_H
#define PICDOWNLOAD_H


#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AEEShell.h"

#include "AEEFile.h"
#include "AEENet.h"
#include "AEEMenu.h"
#define FILENAME_LENGTH 16
typedef enum _AppState
{
	enIdle,
	enMainMenu,
	enWait,
	enFileList,
	enView,
	enFileView,
	enError,
	enView2,
}AppState;

typedef struct _DataHead
{
	uint32 dwCommandId;
	uint32 dwSize;
}DataHead;

typedef struct _PicDownload 
{
	AEEApplet      a;
	AEEDeviceInfo  DeviceInfo;
	IDisplay      *pIDisplay;
	IShell        *pIShell;
	AppState appState;
	IMenuCtl *pIMenuCtlMain;
	IMenuCtl *pIMenuCtlList;
	int16 m_nErrorId;
	AEECallback cbMainTimer; //主定时器
	INetMgr *pNetMgr; //网络管理接口
	ISocket *pSocket; //套接字
	AEECallback cbConnect;
	uint16 m_iPort; //端口号
	AEEDNSResult m_DNSRes; //DNS结果
	byte *m_pbRecvData;
	byte *m_pbSendData;
	int m_nSendNum;
	int m_nRecvNum;
	int32 m_nRecvSize;
	int32 m_nSendSize;
	boolean m_bRecvBuf;
	boolean m_bHasSend;
	int m_nNetState;
	DataHead m_SendDataHead;
	DataHead m_RecvDataHead;
	byte *m_pbRecvBuffer;
	byte m_bSendBuffer[128];
	int m_nSendBufferSize;
	boolean m_bIsReadBuf;
	boolean m_bIsRecv;
	IImage *m_pIImage;	
	int m_nFileSize;
	int m_nItemId;
}PicDownload;

#define GET_FILE_LIST 0x0001 //获取图片列表
#define GET_FILE 0x0002 //获取指定图片
#define SET_FILE_LIST 0x8001
#define SET_FILE 0x8002

#endif