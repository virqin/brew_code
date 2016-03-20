// LogRecord.cpp: implementation of the CLogRecord class.
// CLogRecord���ṩ�˽�ͨ���˿ڷ��͵����ݱ��浽�ļ��еĹ���
// 
// ����: л׿ 2009-07-15

//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LogRecord.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogRecord::CLogRecord()
{
	InitRecord();
}

/**********************************************************************
* �������ƣ� // ~CLogRecord()
* ���������� // �����������ͷ���Դ����ɾ��log�ļ�
* ��������� // ��
* ��������� // ��
* �� �� ֵ�� // ��
* ����˵���� // ��
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
CLogRecord::~CLogRecord()
{
	CloseLog();
	DeleteRecord();
}

/**********************************************************************
* �������ƣ� // CloseLog()
* ���������� // ���log�ļ��򿪣��رձ����ļ�
* ��������� // ��
* ��������� // ��
* �� �� ֵ�� // ��
* ����˵���� // ��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
void CLogRecord::CloseLog()
{
	if(m_bOpened)
	{
		m_fileSave.Close();
	}
}

/**********************************************************************
* �������ƣ� // WriteHeader()
* ���������� // д������ͷ��ÿһ�����ݵ�ǰ�벿�ֶ��ǹ̶���ʽ
* ��������� // ��
* ��������� // ��
* �� �� ֵ�� // ����true
* ����˵���� // ��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
BOOL CLogRecord::WriteHeader(int nType)
{
	CString strLogHeader;
	CString strTmp;

	OpenLog();

	switch(nType) 
	{
		case 0:	//д����
			strTmp = "  (READ)   ";
			break;
		case 1: //��ȡ����
			strTmp = "  (WRITE)  ";
			break;
		case 2: //�򿪶˿�
			strTmp = "  <CLOSE>  ";
			break;
		case 3: //�رն˿�
			strTmp = "  <OPEN>   ";
			break;
		default:
			strTmp = "---Error---";
			break;
	}
	
	strLogHeader.Format("%4d",m_nSavedLine++);
	strLogHeader.Replace(' ','0');
	strLogHeader += strTmp;
	if(nType == 3)
	{
		strLogHeader = "\n" +strLogHeader;
	}
	
	m_fileSave.WriteString(strLogHeader);

	return TRUE;
}

/**********************************************************************
* �������ƣ� // InitRecord()
* ���������� // ���������log�ļ����ó�ʼ���ݣ�������ȡlog���·������Ϣ
* ��������� // ��
* ��������� // ��
* �� �� ֵ�� // ��
* ����˵���� // ��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
void CLogRecord::InitRecord()
{
	//�õ���ǰ����Ŀ¼·��
	char szPath[256];
	CTime currentTime;
	CString strTime;
	CFileFind fileFinder;
	int	 nLen;	

	currentTime = CTime::GetCurrentTime();
	strTime = currentTime.Format("%Y-%m-%d-%H-%M-%S.log");

	GetModuleFileName(NULL,szPath,MAX_PATH);
	//	::GetCurrentDirectory(256, szPath);
	m_strCurrentRecordPath=szPath;
	nLen = m_strCurrentRecordPath.ReverseFind('\\');
	m_strCurrentRecordPath = m_strCurrentRecordPath.Left(nLen);
	m_strCurrentRecordPath += "\\log";

	if(!fileFinder.FindFile(m_strCurrentRecordPath))
	{
		::CreateDirectory(m_strCurrentRecordPath,NULL);
	}
	
	m_strfileName.Format("%s\\%s",m_strCurrentRecordPath,strTime);


	m_bOpened = FALSE;	//��ʼ���򿪱��
	m_nSavedLine = 0;	//��¼����
	m_nWriteVadLen = 256;
	m_nReadVadLen = 1024;
	m_nRecordLogCount = 20;
}

/**********************************************************************
* �������ƣ� // DeleteRecord()
* ���������� // ɾ�������log�ļ�������ĸ�������ͨ��SetLogRecordCount�ı�
* ��������� // ��
* ��������� // ��
* �� �� ֵ�� // ����True
* ����˵���� // ��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
BOOL CLogRecord::DeleteRecord()
{
	CString		strFile;
	CFileFind	finder;
	BOOL		bWorking;
	int			nfileNum = 0;

	strFile.Format("%s\\*.log",m_strCurrentRecordPath);
	bWorking = finder.FindFile(strFile);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		nfileNum++;
	}
	finder.Close();

	if(nfileNum >m_nRecordLogCount)
	{
		nfileNum = nfileNum-m_nRecordLogCount;

		bWorking = finder.FindFile(strFile);
		while (bWorking&&nfileNum)
		{
			bWorking = finder.FindNextFile();
			nfileNum--;
			DeleteFile(finder.GetFilePath());
		}
	}
	finder.Close();

	return TRUE;
}

/**********************************************************************
* �������ƣ� // WriteLog()
* ���������� // ��¼log��Ϣ����
* ��������� // void *pBuf���������ݵĻ�����, int nLen�����ݳ���
*			 // BOOL bWrite:TRUE��ʾд�������False��ʾ��ȡ����
* ��������� // ��
* �� �� ֵ�� // ����ɹ��򷵻�true�����򷵻�false
* ����˵���� // ��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
BOOL CLogRecord::WriteLog(void *pBuf, int nLen, BOOL bWrite)
{
	CString strTmp;
	CString strAllWrited;
	byte	*pHead = (byte*)pBuf;
	int		nLogLen;

	static DWORD nTimeBegin = GetTickCount();
	if(!WriteHeader(bWrite))
	{
		return FALSE;
	}
	
	DWORD nTimeSpan = GetTickCount() - nTimeBegin;
	strTmp.Format("[%08lu]  ", nTimeSpan);
	m_fileSave.WriteString(strTmp);

	strTmp.Format("%5d",nLen);
	strTmp.Replace(' ','0');
	strTmp += " | ";
	m_fileSave.WriteString(strTmp);

	if(bWrite)
	{
		nLogLen = nLen>m_nWriteVadLen?m_nWriteVadLen:nLen;
	}
	else
	{
		nLogLen = nLen>m_nReadVadLen?m_nReadVadLen:nLen;		
	}

	for(int i=0;i<nLogLen;i++)
	{
		strTmp.Format("%2x",pHead[i]);
		strTmp.Replace(' ','0');
		strTmp +=' ';
		m_fileSave.WriteString(strTmp);
	}
	m_fileSave.WriteString("\n");

	return TRUE;
}

/**********************************************************************
* �������ƣ� // OpenLog()
* ���������� // �Դ����ķ�ʽ�򿪱���log���ļ�����д���ļ�ͷ
* ��������� // ��
* ��������� // ��
* �� �� ֵ�� // �ɹ�ִ���򷵻�true�����򷵻�false
* ����˵���� // ��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
BOOL CLogRecord::OpenLog()
{
	CString strTitle;

	if(m_bOpened)
	{
		return TRUE;
	}

	if (!m_fileSave.Open(m_strfileName,CFile::modeCreate|CFile::modeReadWrite))
	{
		return FALSE;
	}
	strTitle = "\nLine   Func      Time    Length    Data(Hex)\n";
	m_fileSave.WriteString(strTitle);
	m_bOpened = TRUE;

	return TRUE;
}

/**********************************************************************
* �������ƣ� // OpenPort()
* ���������� // �˿ڿ��ؼ�¼������������¼�˿ڵĲ�����Ϣ
* ��������� // int nPort:�򿪹رյĶ˿ںţ�bOpen:TRUE��ʾ�򿪣�False��ʾ�ر�
* ��������� // ��
* �� �� ֵ�� // �ɹ����淵��TRUE������FALSE
* ����˵���� // ��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
BOOL CLogRecord::OpenPort(int nPort,BOOL bOpen)
{
	CString strPort;
	int ntype;

	ntype = bOpen + 2;
	if(!WriteHeader(ntype))
	{
		return FALSE;
	}
	strPort.Format("COM%d\n",nPort);
	m_fileSave.WriteString(strPort);

	return TRUE;
}

/**********************************************************************
* �������ƣ� // SetSavedItemLen()
* ���������� // �޸ı���¼log����Ч���ȣ���Ϊ�еİ�������̫�����ȫ����¼
				�ᵼ��log�ļ�̫�󣬲������¼������Ҳ��������֮��Ҫ��ǰ��
				һ�������ݼ�¼����������ʹ�á�
* ��������� // nLen:��Ҫ��¼����Ч���ȣ�bWrite:TRUE��ʾ����д�����ݳ���
				False��ʾ���ö�ȡlog�ĳ���
* ��������� // ��
* �� �� ֵ�� // ��
* ����˵���� // ��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
void CLogRecord::SetSavedItemLen(int nLen, BOOL bWrite)
{
	if(nLen <0)
	{
		return;
	}

	if(bWrite)
	{
		m_nWriteVadLen = nLen;
	}
	else
	{
		m_nReadVadLen = nLen;
	}
}

/**********************************************************************
* �������ƣ� // SetLogRecordCount()
* ���������� // ����Log�ļ��������������Ϊ���������¼log��һֱռ�ô���
				��������������õķ����ǣ����log�ļ�����ָ�����������Զ�
				ɾ�������¼���ݡ�
* ��������� // nCount:����Ҫ���������
* ��������� // ��
* �� �� ֵ�� // ��
* ����˵���� // ��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
void CLogRecord::SetLogRecordCount(int nCount)
{
	if(nCount >0)
	{
		m_nRecordLogCount = nCount;
	}
}

/**********************************************************************
* �������ƣ� // SetLogRenew()
* ���������� // ���´�log�ļ�����ʼ��¼
* ��������� // ��
* ��������� // ��
* �� �� ֵ�� // ��
* ����˵���� // ��
* �޸�����        �汾��     �޸���	      �޸�����
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
void CLogRecord::SetLogRenew()
{
	CloseLog();
	InitRecord();
	OpenLog();
}
