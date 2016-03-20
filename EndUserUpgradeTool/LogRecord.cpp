// LogRecord.cpp: implementation of the CLogRecord class.
// CLogRecord类提供了将通过端口发送的数据保存到文件中的功能
// 
// 作者: 谢卓 2009-07-15

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
* 函数名称： // ~CLogRecord()
* 功能描述： // 析构函数，释放资源，并删除log文件
* 输入参数： // 无
* 输出参数： // 无
* 返 回 值： // 无
* 其它说明： // 无
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
CLogRecord::~CLogRecord()
{
	CloseLog();
	DeleteRecord();
}

/**********************************************************************
* 函数名称： // CloseLog()
* 功能描述： // 如果log文件打开，关闭保存文件
* 输入参数： // 无
* 输出参数： // 无
* 返 回 值： // 无
* 其它说明： // 无
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称： // WriteHeader()
* 功能描述： // 写入数据头，每一条数据的前半部分都是固定格式
* 输入参数： // 无
* 输出参数： // 无
* 返 回 值： // 返回true
* 其它说明： // 无
* 修改日期        版本号     修改人	      修改内容
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
		case 0:	//写数据
			strTmp = "  (READ)   ";
			break;
		case 1: //读取数据
			strTmp = "  (WRITE)  ";
			break;
		case 2: //打开端口
			strTmp = "  <CLOSE>  ";
			break;
		case 3: //关闭端口
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
* 函数名称： // InitRecord()
* 功能描述： // 给所保存的log文件设置初始数据，包括获取log存放路径等信息
* 输入参数： // 无
* 输出参数： // 无
* 返 回 值： // 无
* 其它说明： // 无
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
void CLogRecord::InitRecord()
{
	//得到当前工作目录路径
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


	m_bOpened = FALSE;	//初始化打开标记
	m_nSavedLine = 0;	//记录行数
	m_nWriteVadLen = 256;
	m_nReadVadLen = 1024;
	m_nRecordLogCount = 20;
}

/**********************************************************************
* 函数名称： // DeleteRecord()
* 功能描述： // 删除多余的log文件，保存的个数可以通过SetLogRecordCount改变
* 输入参数： // 无
* 输出参数： // 无
* 返 回 值： // 返回True
* 其它说明： // 无
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称： // WriteLog()
* 功能描述： // 记录log信息功能
* 输入参数： // void *pBuf：保存数据的缓冲区, int nLen：数据长度
*			 // BOOL bWrite:TRUE标示写入操作，False标示读取操作
* 输出参数： // 无
* 返 回 值： // 保存成功则返回true；否则返回false
* 其它说明： // 无
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称： // OpenLog()
* 功能描述： // 以创建的方式打开保存log的文件，并写入文件头
* 输入参数： // 无
* 输出参数： // 无
* 返 回 值： // 成功执行则返回true；否则返回false
* 其它说明： // 无
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称： // OpenPort()
* 功能描述： // 端口开关记录操作，用来记录端口的操作信息
* 输入参数： // int nPort:打开关闭的端口号；bOpen:TRUE表示打开，False表示关闭
* 输出参数： // 无
* 返 回 值： // 成功保存返回TRUE，否则FALSE
* 其它说明： // 无
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称： // SetSavedItemLen()
* 功能描述： // 修改被记录log的有效长度，因为有的包数据量太大，如果全部记录
				会导致log文件太大，并且其记录的意义也不大，所以之需要将前面
				一部分数据记录下来供分析使用。
* 输入参数： // nLen:需要记录的有效长度；bWrite:TRUE表示设置写入数据长度
				False表示设置读取log的长度
* 输出参数： // 无
* 返 回 值： // 无
* 其它说明： // 无
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称： // SetLogRecordCount()
* 功能描述： // 设置Log文件保存的数量，因为如果大量记录log会一直占用磁盘
				的容量，这里采用的方法是，如果log文件大于指定的数量，自动
				删除最早记录数据。
* 输入参数： // nCount:设置要保存的数量
* 输出参数： // 无
* 返 回 值： // 无
* 其它说明： // 无
* 修改日期        版本号     修改人	      修改内容
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
* 函数名称： // SetLogRenew()
* 功能描述： // 重新打开log文件并开始记录
* 输入参数： // 无
* 输出参数： // 无
* 返 回 值： // 无
* 其它说明： // 无
* 修改日期        版本号     修改人	      修改内容
* -----------------------------------------------
* 2009/07/21	     V1.0
***********************************************************************/
void CLogRecord::SetLogRenew()
{
	CloseLog();
	InitRecord();
	OpenLog();
}
