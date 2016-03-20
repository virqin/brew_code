// LogRecord.h: interface for the CLogRecord class.
// CLogRecord���ṩ�˽�ͨ���˿ڷ��͵����ݱ��浽�ļ��еĹ���,���Ե��뵽
// �����У�ͨ�������ṩ��WriteLog(void *pBuf, int nLen, BOOL bWrite)����������
// �˿������͵����ݣ�ͬʱ�ṩ�������ӿ�������log�ļ��ı����ʽ�ȡ�
//
// ����: л׿ 2009-07-15
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
