// SST_F350Dlg.h : header file
//

#if !defined(AFX_SST_F350DLG_H__4109E3FE_573D_4F3E_BB68_9E2B6B05F6BE__INCLUDED_)
#define AFX_SST_F350DLG_H__4109E3FE_573D_4F3E_BB68_9E2B6B05F6BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "operatepage.h"
#include "Packet.h"
#include "EStatic.h"
#include "TextProgressCtrl.h"
#include "CIniFile.h"

/////////////////////////////////////////////////////////////////////////////
// CSST_F350Dlg dialog
#define UNKN_CONNECT    0
#define DIAG_CONNECT    1
#define DLAD_CONNECT    2
#define SDLD_CONNECT    3
#define REPR_CONNECT    4

class CSST_F350Dlg:public CDialog
{
    // Construction
public:
	BOOL HSDLMainFuncNew(char FilePath[_MAX_DIR], int nMinRate, int nMaxRate, CString strActionID);
  	int  ReceiveHSDLRsp();
	BOOL WritePacketDataChecksum(CFile&hReadFile,int iOffset,int nBlockTotalLen,int nOneStep,int CurRate,BOOL bCRC, int *checksum);
	int  ReadPacketData(int iOffset,int nBlockTotalLen,int nMaxRate,int nMinRate);

	BOOL                    HSDL_EFSmbn();
    BOOL                    HSDLImage();
    BOOL                    HSDLmainFunc(char FilePath[_MAX_DIR],
                                         int nMinRate,
                                         int nMaxRate,
                                         CString strActionID);
    BOOL                    ReadMeid();
    BOOL                    IsDownFileSelect();
    BOOL                    _WriteFileToPhone(CString PathName, CString strSource, CString SourcePathName); 
    BOOL                    RestoreHidemenu();
    BOOL                    RestoreWap();
    BOOL                    RestoreMms();
    BOOL                    RestoreBrewFile();
    BOOL                    RestoreServiceNumbersFile();
    BOOL                    RestoreSndSetFile();
    BOOL                    RestoreDMFile();
    BOOL                    RestoreCameraFile();
    BOOL                    RestoreCallFile();
    BOOL                    RestoreLightFile();
    BOOL                    RestorePrlFile();
    BOOL                    WriteFileToPhone(CString PathName, CString strSource);
    BOOL                    RestoreEriFile();
    BOOL                    RestoreNVofDL();
    BOOL                    BackNVofDL();
    BOOL                    SDLmodeDownloadMbn();
    BOOL                    DLmodeDownloadMbn();
    BOOL                    RepairMbn();
    BOOL                    SpecialVersionTest();
    BOOL                    SearchComPort(BOOL bSuspend);
    BOOL                    m_nSelectMEID;
    void                    InitSetPhonePort();
    BOOL                    DLmodeDownload();
    BOOL                    SDLModeSoftWare();
    BOOL                    m_bHaveRefresh;
    CString                 m_strBackupNvPath;
    BOOL                    SendSPC();
    void                    InitCurrentPathName();

    BOOL                    m_bRunSuccess;
    BOOL                    DownloadMbn();
    BOOL                    RestoreNV(CString NVFilePathIfo, int nBackupType = 0);
    BOOL                    BackupNV(CString NVFileName, int nBackupType = 0);
    void                    ShowRatio(int ratioValue, int max = 100);
    BOOL                    DownloadImage();
    BOOL                    DownloadEFSmbn();
    BOOL                    DownloadOEMbootloader();
    BOOL                    DownloadSecondbootloader();
    BOOL                    DownloadSecondbootloaderheader();
    BOOL                    DownloadPribootloader();
    BOOL                    DownloadPartitionTable();
    BOOL                    EraseEFS();
    BOOL                    RefreshPort();
    BOOL                    BackupToFile();
    BOOL                    BackupVersion();
    BOOL                    m_bSuspend;
    BOOL                    m_bUnopenedPort[100];
    BOOL                    RepairModeSoftware();
    BOOL                    RepConnect();

    void                    SetShowState();
    BOOL                    StreamDownloadConnect();
    BOOL                    DownloadConnect();
    BOOL                    DiagConnect();
    int                     m_nConnectType;
    BOOL                    ConnectPhone();
    int                     GetTotalBlock(int total);
    BOOL                    DLUserFile(void);
    void                    ShowProgress(double data);
                            CSST_F350Dlg(CWnd *pParent = NULL); // standard constructor
                            ~CSST_F350Dlg();

    //---- ****  全局变量  ****----//

    CString                 HandsetInfo;
    int                     m_HandsetModelNo;
    int                     m_nBoundRate;
    int                     m_ComPort1;
    BOOL                    b_ComOpen;
    BOOL                    Conect0OK;
    CPacket                *m_pPacket;
    CWinThread             *pStateThread;
    nv_item_type            item_data;
    int                     PageActiveNum;
    //////////////////////////////////////////////////////////////////////////
    CString                 m_strArpgFileName;
    UINT                    m_nGoAddress;

    //////////////////////////////////////////////////////////////////////////
    DWORD                   m_nESN; 
    phone_interface_info    m_phone_info;
    phone_interface_info    boot_info;
    phone_interface_info    bin_info;
    BOOL                    m_bResv8k;

    void                    SlideWnd(int *buf, int &len, int *NoRspbuf, int &NoRsplen);
    BOOL                    IsDLMatch(BOOL &IsResv8K);
    void                    GoStartPoint(void);
    BOOL                    StartPortMonitor(void);
    BOOL                    MonitorPhoneState(void);
    BOOL                    InitPort(int nPort, int nBaud);

    BOOL                    DLBootFile1(void);
    BOOL                    GetSoftWareVer(CString bufdef);

    // Dialog Data
    //{{AFX_DATA(CSST_F350Dlg)
    enum { IDD = IDD_SST_F350_DIALOG };
    CEStatic    m_clPort;
    CEStatic    m_bmpStatus;
    CTextProgressCtrl   m_ProgressCtrl;
    CComboBox   m_combCom;
    CTabCtrl    m_tabCtrl;
    CString m_strSpcCode;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSST_F350Dlg)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:

    void                    InitCombox();
    bool                    InitPage();
    HICON                   m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(CSST_F350Dlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSelchangeTabPage(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSelchangeComboCom();
    afx_msg void OnButtonAbout();
    afx_msg bool OnButtonExit();
    afx_msg void OnClose();
    afx_msg void OnCancel();
    afx_msg void OnOK();
       afx_msg BOOL OnDeviceChange( UINT nEventType, DWORD dwData );
    afx_msg void OnButtonRefresh();
    //}}AFX_MSG
                            DECLARE_MESSAGE_MAP()
private:
    BOOL                    RestoreDMUkeyFile();
    BOOL                    RestoreDefaultNV(CString strNVFilePath);
    BOOL                    DelDirectoryinPhone(CString strPhone);
    BOOL                    RefurbishmentToDefault();
    void                    SetPageRect(CWnd *pWnd);
    void                    InitialData();
    //  CPageNV m_PageNV;
    CPageDL                 m_PageDL;
    //  CRefurDlg   m_PageReF;
    //  CDlgDMU     m_PageDmu;
    static UINT             PhoneStatusThread(LPVOID pParam);      //启动第一个线程
    CString                 m_strFlashPath;
    BOOL                    m_bEnableEraser;
    CString                 m_strMeid;
    CString                 m_strPhoneVersion;
    CString                 m_strSoftWareVersion;

    //  流下载
    SDLnode                *pSDnode;
    int                     m_nStreamBlocks;
    CWinThread             *m_pStreamRead;
    BOOL                    m_bStartSD;
    int                     m_nRWPace;
    CString                 strCurrentDirectory;
    //备份flash
    CString                 m_strBackFlashPath;
    int                     m_nBackStart;
    int                     m_nBackEnd;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SST_F350DLG_H__4109E3FE_573D_4F3E_BB68_9E2B6B05F6BE__INCLUDED_)

