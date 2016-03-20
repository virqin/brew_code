#if !defined(AFX_OPERATEPAGE_H__6546E6AD_58CA_40A1_B3AF_3A2243471E5F__INCLUDED_)
#define AFX_OPERATEPAGE_H__6546E6AD_58CA_40A1_B3AF_3A2243471E5F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// operatepage.h : header file
/*
//	定义用户类型结构体，根据用户类型来执行不同的下载操作流程
typedef struct _USER_TYPE
{
	BOOL	bSuperUser;			//如果是超级用户为TRUE;
	int		guestPhoneType;		//普通用户时有效;用来标记机卡一体和机卡分离类型 0代表机卡分离，1:机卡一体
	struct  _superSet			//超级用户时有效
	{
		int		nDloadTimes;	//允许用户下载的次数
		BOOL	bLock;			//标示手机是否是锁网版本	TRUE :锁网
		BOOL	bRestoreLock;	//锁网时有效，表示下载完成后是否锁网,True表示加锁
		char	filePath[300];	//锁网版本并且执行锁功能时，存放加锁sid文件
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
	char strSoftStructDir[_MAX_DIR];		// 软件部定义的版本软件路径
	char strFlexfilePath[_MAX_DIR];	//flexfile文件路径
	char strNV4050Path[_MAX_DIR];	//nv4050文件路径
	char strEFSDir[_MAX_DIR];	//铃音图片目录路径
	char strNVBackupDir[_MAX_DIR];//NV备份目录路径
	char striFilePath[_MAX_PATH];//版本文件路径
	char strBootFilePath[_MAX_PATH];//Boot文件路径
	char strFlashPrgFilePath[_MAX_PATH];//armprg路径
	char strnFilePath[_MAX_PATH];//NV备份文件路径
	int  nPort;
	int  nDataRate;
	char strDefaultNVFile[_MAX_PATH];//缺省NV文件名
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
