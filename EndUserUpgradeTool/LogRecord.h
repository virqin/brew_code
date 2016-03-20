// LogRecord.h: interface for the CLogRecord class.
// CLogRecord类提供了将通过端口发送的数据保存到文件中的功能,可以导入到
// 程序中，通过调用提供的WriteLog(void *pBuf, int nLen, BOOL bWrite)函数来保存
// 端口所发送的数据；同时提供了其他接口来配置log文件的保存格式等。
//
// 作者: 谢卓 2009-07-15
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGRECORD_H__8BBCBAAE_CABE_4D23_84A1_534441092DC3__INCLUDED_)
#define AFX_LOGRECORD_H__8BBCBAAE_CABE_4D23_84A1_534441092DC3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLogRecord  
{
public:
	void SetLogRenew();
	void SetLogRecordCount(int nCount);
	void SetSavedItemLen(int nLen,BOOL bWrite);
	void CloseLog();
	BOOL OpenPort(int nPort,BOOL bOpen);
	BOOL WriteLog(void *pBuf,int nLen,BOOL bWrite);
	CLogRecord();
	virtual ~CLogRecord();

private:
	CStdioFile	m_fileSave;
	BOOL	m_bOpened;
	CString m_strCurrentRecordPath;
	CFile	m_savefile;
	CString m_strfileName;
	int		m_nSavedLine;
	int		m_nWriteVadLen;
	int		m_nReadVadLen;
	int		m_nRecordLogCount;
protected:
	void InitRecord();
	BOOL OpenLog();
	BOOL WriteHeader(int nType);
	BOOL DeleteRecord();
};

#endif // !defined(AFX_LOGRECORD_H__8BBCBAAE_CABE_4D23_84A1_534441092DC3__INCLUDED_)
