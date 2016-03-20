#if !defined(AFX_OPERATEPAGE_H__6546E6AD_58CA_40A1_B3AF_3A2243471E5F__INCLUDED_)
#define AFX_OPERATEPAGE_H__6546E6AD_58CA_40A1_B3AF_3A2243471E5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// operatepage.h : header file
/*
//	�����û����ͽṹ�壬�����û�������ִ�в�ͬ�����ز�������
typedef struct _USER_TYPE
{
	BOOL	bSuperUser;			//����ǳ����û�ΪTRUE;
	int		guestPhoneType;		//��ͨ�û�ʱ��Ч;������ǻ���һ��ͻ����������� 0����������룬1:����һ��
	struct  _superSet			//�����û�ʱ��Ч
	{
		int		nDloadTimes;	//�����û����صĴ���
		BOOL	bLock;			//��ʾ�ֻ��Ƿ��������汾	TRUE :����
		BOOL	bRestoreLock;	//����ʱ��Ч����ʾ������ɺ��Ƿ�����,True��ʾ����
		char	filePath[300];	//�����汾����ִ��������ʱ����ż���sid�ļ�
	}superSet;					
}UserType;
*/

typedef struct tagSETTING
{
	//lichenyang 20060317
	char partitionPath[_MAX_DIR];
	char pblPath[_MAX_DIR];
	char qcsblhd_cfgdataPath[_MAX_DIR];
	char qcsblPath[_MAX_DIR];
	char oemsblhdPath[_MAX_DIR];
	char oemsblPath[_MAX_DIR];
	char amsshdPath[_MAX_DIR];
	char amssPath[_MAX_DIR];
	char efsmbnPath[_MAX_PATH] ;
	//lichenyang 20060317
	BOOL bFlexfile;
	BOOL bNV4050;
	BOOL bPicTone;
	char strSoftStructDir[_MAX_DIR];		// ���������İ汾���·��
	char strFlexfilePath[_MAX_DIR];	//flexfile�ļ�·��
	char strNV4050Path[_MAX_DIR];	//nv4050�ļ�·��
	char strEFSDir[_MAX_DIR];	//����ͼƬĿ¼·��
	char strNVBackupDir[_MAX_DIR];//NV����Ŀ¼·��
	char striFilePath[_MAX_PATH];//�汾�ļ�·��
	char strBootFilePath[_MAX_PATH];//Boot�ļ�·��
	char strFlashPrgFilePath[_MAX_PATH];//armprg·��
	char strnFilePath[_MAX_PATH];//NV�����ļ�·��
	int  nPort;
	int  nDataRate;
	char strDefaultNVFile[_MAX_PATH];//ȱʡNV�ļ���
}SETTING, * PSETTING;

class CPageDL : public CDialog
{
	// Construction
public:
	CString m_strSelPort;
	void SetUserFilePath(CString strPath);
	CString GetCurrentDir();
	//int m_nBackupType;
	BOOL SaveIniFile();
	BOOL ReadIniFile();
	BOOL SetDownloadFile(CString path);
	void SetUser(BOOL bSuper);

	CPageDL(CWnd* pParent = NULL);   // standard constructor
	BOOL FolderOrFileIsExist(CString szPath);

	CString m_CurrentDir;
	CString	m_FlashfilePath;
	// Dialog Data
	//{{AFX_DATA(CPageDL)
	enum { IDD = IDD_DIALOG_DL };
	CString	m_UserFilePath;
	SETTING	m_stSetting;
	CString	m_NVfilePath;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageDL)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CPageDL)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonUserFilePath();
	afx_msg void OnButtonStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CPageNV dialog

class CPageNV : public CDialog
{
// Construction
public:
	CPageNV(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPageNV)
	enum { IDD = IDD_DIALOG_NV };
	CString	m_NVfile;
	CString	m_NVfilePath;
	int		m_nNam;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageNV)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPageNV)
	afx_msg void OnButtonBkbFilePath();
	afx_msg void OnButtonRsbFile();
	afx_msg void OnButtonBksStart();
	afx_msg void OnButtonRssStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CRefurDlg dialog

class CRefurDlg : public CDialog
{
	// Construction
public:
	CRefurDlg(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CRefurDlg)
	enum { IDD = IDD_DIALOG_REFUR };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRefurDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CRefurDlg)
	afx_msg void OnButtonRefur();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CDlgDMU dialog

class CDlgDMU : public CDialog
{
	// Construction
public:
	CDlgDMU(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgDMU)
	enum { IDD = IDD_DIALOG_DMU };
	CString	m_strDMUPath;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDMU)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgDMU)
	afx_msg void OnButtonBrowse();
	afx_msg void OnButtonStart();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPERATEPAGE_H__6546E6AD_58CA_40A1_B3AF_3A2243471E5F__INCLUDED_)
