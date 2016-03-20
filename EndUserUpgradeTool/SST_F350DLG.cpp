// SST_F350Dlg.cpp : implementation file
//
/*********************************************************************
* 版权所有 (C)2001, 深圳市中兴通讯股份有限公司。
* 
* 文件名称： // SST_F350Dlg.cpp
* 文件标识： // 
* 内容摘要： // SST_F350_P300V1.00.00程序的主界面，在此处理了用户
                在主界面的各种操作命令，是人机界面的接口；同时在该文
                件中派生的线程实现与手机的通讯。
* 其它说明： // 
* 当前版本： // SST_F350_P300V1.00.08
* 作    者： // 谢  卓
* 完成日期： // 2010/01/7
* 
* 修改记录1：// 修改历史记录，包括修改日期、修改者及修改内容
* 修改日期：
* 版 本 号：
* 修 改 人：
* 修改内容： 
* 修改记录2：…
**********************************************************************/



#include "stdafx.h"
#include "SST_F350.h"
#include "SST_F350Dlg.h"
#include <MATH.H>//lichenyang 20060514~~~~~~~~~~~~~
#include <DBT.H>

#define     STR_VERIZON_ERI_FILE    "Verizon_ERI.bin"
#define     STR_VERIZON_DMU_KEY_FILE    "DMU/10.key"

#define TRANS_DATA_RSP  2
#define CRC_SUCCESS_RSP 3
#define CRC_FAIL_RSP    4

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BUFSIZE 1000
#define TXLIMIT 3
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for adept About

class CAboutDlg:public CDialog
{
public:
            CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
            DECLARE_MESSAGE_MAP()
    HICON   m_hIcon;
};

CAboutDlg::CAboutDlg():CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    SetIcon(m_hIcon,TRUE);          // Set big icon
    SetIcon(m_hIcon,FALSE);     // Set small icon

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

extern CString  g_szPropName;       //自己定义一个属性名
extern HANDLE   g_hValue;           //自己定义一个属性值


/////////////////////////////////////////////////////////////////////////////
// CSST_F350Dlg dialog

CSST_F350Dlg::CSST_F350Dlg(CWnd *pParent /*=NULL*/):CDialog(CSST_F350Dlg::IDD,pParent)
{
    //{{AFX_DATA_INIT(CSST_F350Dlg)
    m_strSpcCode = _T("000000");
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_strPhoneVersion = "";
    m_strSoftWareVersion = "";
    m_strMeid = "";
    m_bRunSuccess = FALSE;
    InitialData();
}

CSST_F350Dlg::~CSST_F350Dlg()
{
    if (m_pPacket != NULL)
    {
        delete m_pPacket;
        m_pPacket = NULL;
    }
}
void CSST_F350Dlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSST_F350Dlg)
    DDX_Control(pDX, IDC_STATIC_PORT, m_clPort);
    DDX_Control(pDX, IDC_STATIC_BMP, m_bmpStatus);
    DDX_Control(pDX, IDC_PROGRESS, m_ProgressCtrl);
    DDX_Control(pDX, IDC_COMBO_COM, m_combCom);
    DDX_Control(pDX, IDC_TAB_PAGE, m_tabCtrl);
    DDX_Text(pDX, IDC_EDIT_SPCCODE, m_strSpcCode);
    DDV_MaxChars(pDX, m_strSpcCode, 6);
    //}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CSST_F350Dlg, CDialog)
    //{{AFX_MSG_MAP(CSST_F350Dlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PAGE, OnSelchangeTabPage)
    ON_CBN_SELCHANGE(IDC_COMBO_COM, OnSelchangeComboCom)
    ON_BN_CLICKED(IDC_BUTTON_ABOUT, OnButtonAbout)
    ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
    ON_WM_CLOSE()
    ON_MESSAGE(WM_DEVICECHANGE,OnDeviceChange) 
    ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSST_F350Dlg message handlers

BOOL CSST_F350Dlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu  *pSysMenu    = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING,IDM_ABOUTBOX,strAboutMenu);
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon,TRUE);          // Set big icon
    SetIcon(m_hIcon,FALSE);     // Set small icon

    // TODO: Add extra initialization here
    SetProp(m_hWnd,g_szPropName,g_hValue);  

    InitCurrentPathName();
    InitPage();
    InitCombox();
    //OnButtonRefresh();
    //刷新端口
    InitSetPhonePort();
    //  m_bmpStatus.ReLoadBitmap(IDB_SEARCHING);
    StartPortMonitor();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSST_F350Dlg::InitSetPhonePort()
{
    int nSel;

    SearchComPort(TRUE);
    nSel = m_combCom.FindString(0,m_PageDL.m_strSelPort);
    m_combCom.SetCurSel(nSel);
    OnSelchangeComboCom();
}
void CSST_F350Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        OnButtonAbout();
    }
    else
    {
        CDialog::OnSysCommand(nID,lParam);
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSST_F350Dlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC    dc  (this); // device context for painting

        SendMessage(WM_ICONERASEBKGND,(WPARAM) dc.GetSafeHdc(),0);

        // Center icon in client rectangle
        int     cxIcon  = GetSystemMetrics(SM_CXICON);
        int     cyIcon  = GetSystemMetrics(SM_CYICON);
        CRect   rect;
        GetClientRect(&rect);
        int x   = (rect.Width() - cxIcon + 1) / 2;
        int y   = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x,y,m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSST_F350Dlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

bool CSST_F350Dlg::InitPage()
{
    m_tabCtrl.InsertItem(0,"Download Page");

    m_PageDL.Create(IDD_DIALOG_DL,this);
    SetPageRect(&m_PageDL);

    m_PageDL.ShowWindow(SW_SHOW);

    return true;
}

void CSST_F350Dlg::InitCurrentPathName()
{
    //得到当前工作目录路径
    char    szPath[256];
    int     nLen;
    GetModuleFileName(NULL,szPath,MAX_PATH);

    //  ::GetCurrentDirectory(256, szPath);
    strCurrentDirectory = szPath;
    nLen = strCurrentDirectory.ReverseFind('\\');
    strCurrentDirectory = strCurrentDirectory.Left(nLen);
}
void CSST_F350Dlg::InitialData()
{
    m_nConnectType = REPR_CONNECT;
    m_bSuspend = FALSE;
    m_ComPort1 = 1;
    m_nBoundRate = 115200;
    b_ComOpen = FALSE;
    Conect0OK = FALSE;
    m_pPacket = new CPacket;
    m_nESN = 0;
    PageActiveNum = 0;
    pStateThread = NULL;
    HandsetInfo = ""; 
    m_strFlashPath = "";
    m_bEnableEraser = FALSE;
    //  备份flash信息
    m_strFlashPath = "";
    m_nBackStart = 0;
    m_nBackEnd = 0;
}

void CSST_F350Dlg::SetPageRect(CWnd *pWnd)
{
    CRect   rect;

    m_tabCtrl.GetWindowRect(&rect);
    rect.top += 10;
    rect.right -= 10;
    rect.bottom -= 40;
    pWnd->MoveWindow(rect);
}

void CSST_F350Dlg::InitCombox()
{
    m_combCom.SetCurSel(0);
}

BOOL CSST_F350Dlg::MonitorPhoneState()
{
    if (!b_ComOpen)
    {
        if (!InitPort(m_ComPort1,m_nBoundRate))
        {
            GoStartPoint();
            return FALSE;
        }
        else
        {
            b_ComOpen = TRUE;
        }
    }

    if (!ConnectPhone())
    {
        GoStartPoint();
        return FALSE;
    }
    if (PageActiveNum == 0)
    {
        return TRUE;
    }

    GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(FALSE);
    m_PageDL.GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
    m_PageDL.GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
    //  m_PageNV.GetDlgItem(IDC_BUTTON_BKS)->EnableWindow(FALSE);
    //  m_PageNV.GetDlgItem(IDC_BUTTON_RSS)->EnableWindow(FALSE);
    //  m_PageReF.GetDlgItem(IDC_BUTTON_REFUR)->EnableWindow(FALSE);
    //  m_PageDmu.GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_COMBO_COM)->EnableWindow(FALSE); 
    GetDlgItem(IDC_BUTTON_REFRESH)->EnableWindow(FALSE);
    m_tabCtrl.EnableWindow(FALSE);          
    m_bmpStatus.ReLoadBitmap(IDB_RUNNING);          

    switch (PageActiveNum)
    {
        case 11:
        if (!RestorePrlFile())
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Failed!");
            m_bmpStatus.ReLoadBitmap(IDB_FAIL);
            ::Sleep(3000);
        }
        else
        {
            m_bRunSuccess = TRUE;
            //重启手机
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Reset handset.....");
            int Error   = 0;        //search handset
            do
            {
                ::Sleep(500);
                Error++;
                if (Error > 1)
                {
                    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Searching handset.....");
                }
            }
            while ((!ConnectPhone()) && (Error < 40));
            if (Error >= 40)
            {
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Fail! Can't find the handset!");
            }
            ::Sleep(500);
            ConnectPhone();
            ::Sleep(500);
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Success!");
            m_bmpStatus.ReLoadBitmap(IDB_PASS);
        }           
        break;
        case 12:
        if (!RestoreEriFile())
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Failed!");
            m_bmpStatus.ReLoadBitmap(IDB_FAIL);
            ::Sleep(3000);
        }
        else
        {
            m_bRunSuccess = TRUE;
            //重启手机
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Reset handset.....");
            int Error   = 0;        //search handset
            do
            {
                ::Sleep(500);
                Error++;
                if (Error > 1)
                {
                    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Searching handset.....");
                }
            }
            while ((!ConnectPhone()) && (Error < 40));
            if (Error >= 40)
            {
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Fail! Can't find the handset!");
            }
            ::Sleep(500);
            ConnectPhone();
            ::Sleep(500);
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Success!");
            m_bmpStatus.ReLoadBitmap(IDB_PASS);
        }           
        break;
        case 13:
        if (!DownloadMbn())
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Failed!");
            m_bmpStatus.ReLoadBitmap(IDB_FAIL);
            ::Sleep(3000);
        }
        else
        {
            m_bmpStatus.ReLoadBitmap(IDB_PASS); 
            Sleep(3000);
        }
        m_bRunSuccess = TRUE;           
        break;
        case 14:
        if (!RepairMbn())
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Failed!");
            m_bmpStatus.ReLoadBitmap(IDB_FAIL);
            ::Sleep(3000);
        }
        else
        {
            m_bmpStatus.ReLoadBitmap(IDB_PASS);
        }
        m_bRunSuccess = TRUE;       
        break;
        case 21:
        if (!DLmodeDownloadMbn())
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Failed!");
            m_bmpStatus.ReLoadBitmap(IDB_FAIL);
            ::Sleep(3000);
        }
        else
        {
            m_bmpStatus.ReLoadBitmap(IDB_PASS);
        }
        m_bRunSuccess = TRUE;
        break;
        case 22:
        if (!RefurbishmentToDefault())
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Failed!");
            m_bmpStatus.ReLoadBitmap(IDB_FAIL);
            ::Sleep(3000);
        }
        else
        {
            m_bRunSuccess = TRUE;
            //重启手机
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Reset handset.....");
            int Error   = 0;        //search handset
            do
            {
                ::Sleep(500);
                Error++;
                if (Error > 1)
                {
                    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Searching handset.....");
                }
            }
            while ((!ConnectPhone()) && (Error < 40));
            if (Error >= 40)
            {
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Fail! Can't find the handset!");
            }
            ::Sleep(500);
            ConnectPhone();
            ::Sleep(500);
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Success!");
            m_bmpStatus.ReLoadBitmap(IDB_PASS);
        }           
        break;
        case 23:
        if (!RestoreDMUkeyFile())
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Failed!");
            m_bmpStatus.ReLoadBitmap(IDB_FAIL);
            ::Sleep(3000);
        }
        else
        {
            m_bRunSuccess = TRUE;
            //重启手机
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Reset handset.....");
            int Error   = 0;        //search handset
            do
            {
                ::Sleep(500);
                Error++;
                if (Error > 1)
                {
                    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Searching handset.....");
                }
            }
            while ((!ConnectPhone()) && (Error < 40));
            if (Error >= 40)
            {
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Fail! Can't find the handset!");
            }
            ::Sleep(500);
            ConnectPhone();
            ::Sleep(500);
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Success!");
            m_bmpStatus.ReLoadBitmap(IDB_PASS);
        }
        break;
        case 40:
        break;
        default:
        break;
    }

    ShowRatio(0);
    m_PageDL.GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
    m_PageDL.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
    //  m_PageNV.GetDlgItem(IDC_BUTTON_BKS)->EnableWindow(TRUE);
    //  m_PageNV.GetDlgItem(IDC_BUTTON_RSS)->EnableWindow(TRUE);
    //  m_PageReF.GetDlgItem(IDC_BUTTON_REFUR)->EnableWindow(TRUE);
    //  m_PageDmu.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_COMBO_COM)->EnableWindow(TRUE);  
    GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_REFRESH)->EnableWindow(TRUE);
    m_tabCtrl.EnableWindow(TRUE);
    PageActiveNum = 0;

    m_pPacket->m_log.SetLogRenew();
    return TRUE;
}

BOOL CSST_F350Dlg::InitPort(int nPort, int nBaud)
{
    if (m_pPacket->IsOpened())
    {
        m_pPacket->Close();
    }
    if (!(m_pPacket->Open(nPort,nBaud)))
    {
        return FALSE;
    }
    return TRUE;
}

void CSST_F350Dlg::GoStartPoint()
{
    m_bRunSuccess = FALSE;
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("There is no phone connected to the selected port.");
    b_ComOpen = FALSE;
    Conect0OK = FALSE;
    m_bmpStatus.ReLoadBitmap(IDB_SEARCHING);
    m_clPort.ReLoadIcon(IDR_PORT_DISCONNECT);
    HandsetInfo = "";
    SetDlgItemText(IDC_STATIC_INFO,HandsetInfo);
    SetDlgItemText(IDC_STATIC_COM,HandsetInfo);
    m_PageDL.GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
}

BOOL CSST_F350Dlg::StartPortMonitor()
{
    if (pStateThread != NULL)
    {
        pStateThread->SuspendThread();
    }
    pStateThread = AfxBeginThread(PhoneStatusThread,this,THREAD_PRIORITY_HIGHEST);
    return TRUE;
}

UINT CSST_F350Dlg::PhoneStatusThread(LPVOID pParam)      //启动第一个下载线程
{
    CSST_F350Dlg   *pDlg    = (CSST_F350Dlg *) pParam;
    for (; ;)
    {
        BOOL    bSuccess    = pDlg->MonitorPhoneState();
        ::Sleep(800);
    }
    return 0;
}

BOOL CSST_F350Dlg::DLBootFile1(void)
{
    CFile   hReadFile;
    if (!hReadFile.Open(m_strFlashPath,CFile::modeRead | CFile::shareDenyWrite))
    {
        CString buf;
        buf.Format("Can not open file: %s",m_strFlashPath);
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(buf);
        Sleep(1000);
        return FALSE;
    }

    int     nTotalLen       = hReadFile.GetLength();
    int     nBlockLen = 0, nWrittenLen = 0;

    WORD    ProgressStep    = 0;        //发送步进数
    m_ProgressCtrl.SetPos(ProgressStep);    //进度条
    m_ProgressCtrl.SetRange(0,(nTotalLen / SRAM_WRITE_BLOCK_SIZE)); //进度条
    m_ProgressCtrl.SetForeColour(RGB(64,196,64));
    m_ProgressCtrl.SetTextBkColour(RGB(196,0,196));
    m_ProgressCtrl.SetShowText(TRUE);

    byte    writeBuf[SRAM_WRITE_BLOCK_SIZE];
    BOOL    bMore   = FALSE;

    if (nTotalLen > SRAM_WRITE_BLOCK_SIZE)
    {
        nBlockLen = SRAM_WRITE_BLOCK_SIZE;
        bMore = TRUE;
    }
    else
    {
        nBlockLen = nTotalLen;
        bMore = FALSE;
    }
    hReadFile.ReadHuge(writeBuf,nBlockLen);

    CString kiuy    = "";
    CString kiuyyy  = "";
    if (!(m_pPacket->Write32(writeBuf,nWrittenLen + 0X800000,nBlockLen)))
    {
        hReadFile.Close();
        return FALSE;
    }
    else
    {
        kiuyyy = "Writing RAM page: ";
        nWrittenLen += nBlockLen;
        ProgressStep++;
        m_ProgressCtrl.SetPos(ProgressStep);    //进度条
        kiuy.Format("%d",ProgressStep);
        kiuyyy += kiuy;
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(kiuyyy);
    }

    while (bMore)
    {
        Sleep(10);
        nBlockLen = nTotalLen - nWrittenLen;
        if (nBlockLen > SRAM_WRITE_BLOCK_SIZE)
        {
            nBlockLen = SRAM_WRITE_BLOCK_SIZE;
            bMore = TRUE;
        }
        else
        {
            bMore = FALSE;
        }
        hReadFile.ReadHuge(writeBuf,nBlockLen);
        if (!(m_pPacket->Write32(writeBuf,nWrittenLen + 0X800000,nBlockLen)))
        {
            hReadFile.Close();
            return FALSE;
        }
        else
        {
            kiuyyy = "Writing RAM page: ";
            nWrittenLen += nBlockLen;
            m_ProgressCtrl.SetPos(ProgressStep++);  //进度条
            kiuy.Format("%d",ProgressStep);
            kiuyyy += kiuy;
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText(kiuyyy);
        }
    }
    m_ProgressCtrl.SetPos(0);   
    hReadFile.Close();

    return TRUE;
}

BOOL CSST_F350Dlg::ReadMeid()
{
    nv_item_type    items;
    word            status  = 0;
    CString         tmps;

    m_strMeid = "";
    memset(&items,0,sizeof(nv_item_type));
    if (!m_pPacket->NvItemRead(NV_MEID_I,items,status))
    {
        return FALSE;
    }
    for (int i = 6; i >= 0; i--)
    {
        tmps.Format("%2x",items.temp[i]);
        tmps.Replace(' ','0');
        m_strMeid += tmps;
    }
    m_strMeid.MakeUpper();

    return TRUE;
}


BOOL CSST_F350Dlg::DiagConnect()
{
    CString str_Disply  = "";
    CString str_dd1     = "";
    CString str_OldVer  = "";

    word    status      = 0;
    memset(&item_data,0,sizeof(nv_item_type));

    if (m_pPacket->DmssCommand(DIAG_STATUS_F))
    {
        if (m_nConnectType == DIAG_CONNECT)
        {
            return TRUE;
        }
    }
    else
    {
        m_nConnectType = DLAD_CONNECT;
        return FALSE;
    }

    //端口
    //  HandsetInfo.Format("COM%d",m_ComPort1);

    //MEID/ESN
    str_Disply = "";
    //60x5平台通过DIAG_STATUS_F读取ESN不准确
    //  if(m_pPacket->DmssCommand(DIAG_STATUS_F))   //0C 14 3A 7E
    //  {
    //      m_nESN = m_pPacket->m_RspPacket.rsp.stat.esn;
    //      str_Disply.Format("   ESN:%08x   ", m_nESN);
    //      HandsetInfo += str_Disply;
    //  }

    if (m_pPacket->NvItemRead(NV_ESN_I,item_data,status))
    {
        m_nESN = item_data.esn;
        str_Disply.Format("   ESN:%08x   ",m_nESN);
    }
    if (ReadMeid())
    {
        str_Disply.Format(" MEID:(%s) ",m_strMeid);
    }
    HandsetInfo = str_Disply;
    //版本号
    str_Disply = "";
    if (!m_pPacket->GetConnectState())
    {
        SetDlgItemText(IDC_STATIC_COM,"");
        return FALSE;
    }
    diag_sw_ver_rsp_type    sw_ver;
    if (!m_pPacket->GetExtSoftwareVersion(sw_ver))
    {
        if (m_pPacket->GetSoftwareVersion(sw_ver))
        {
            str_Disply.Format("%s",sw_ver.sw_ver_str);
        }
        else
        {
			m_nConnectType = DIAG_CONNECT;  
            return TRUE;
        }
    }
    else
    {
        str_Disply.Format("%s",sw_ver.sw_ver_str);
    }
    m_strPhoneVersion = str_Disply;//   给m_nPhoneVersion赋值
    HandsetInfo += str_Disply;

    //设置当前连接状态为diag连通
    m_nConnectType = DIAG_CONNECT;  
    return TRUE;
}

BOOL CSST_F350Dlg::DownloadConnect()
{
    if (!m_pPacket->DoNoop())
    {
        m_nConnectType = SDLD_CONNECT;
        return FALSE;
    }

    HandsetInfo = "Phone in DownLoad Mode!";    
    m_nConnectType = DLAD_CONNECT;  
    return TRUE;
}

BOOL CSST_F350Dlg::ConnectPhone()
{
    if (PageActiveNum == 14)
    {
        while (!RepConnect())
        {
            Conect0OK = FALSE;
        }
        Conect0OK = TRUE;
    }
    else
    {
        switch (m_nConnectType)
        {
            case UNKN_CONNECT:
            m_pPacket->AT_QCDMG();
            case DIAG_CONNECT:
            Conect0OK = DiagConnect();
            break;
            case DLAD_CONNECT:
            Conect0OK = DownloadConnect();
            break;
            case SDLD_CONNECT:
            Conect0OK = StreamDownloadConnect();
            break;
            case REPR_CONNECT:
            Conect0OK = RepConnect();
            break;
            default:
            Conect0OK = DiagConnect();
        }
    }

    SetShowState();

    return Conect0OK;
}

void CSST_F350Dlg::OnSelchangeTabPage(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: Add your control notification handler code here
    int CurSel; 
    CurSel = m_tabCtrl.GetCurSel(); 
    switch (CurSel)
    {
        case 0:
        {
            m_PageDL.ShowWindow(TRUE);
            break;
        }

        default:
        {
            m_PageDL.ShowWindow(TRUE);
            break;
        }
    }

    *pResult = 0;
}

void CSST_F350Dlg::OnSelchangeComboCom()
{
    // TODO: Add your control notification handler code here
    if (m_bSuspend)
    {
        pStateThread->ResumeThread();
        m_bSuspend = FALSE;
    }

    CString buf;
    char    comBuf[20];

    if (b_ComOpen)
    {
        m_pPacket->Close();     //先关闭
        GoStartPoint();
    }
    m_combCom.GetWindowText(comBuf,20);
    sscanf(comBuf,_T("COM%d"),&m_ComPort1);
    m_PageDL.m_strSelPort = comBuf;
}

void CSST_F350Dlg::OnButtonAbout()
{
    // TODO: Add your control notification handler code here
    /*  About对话框类型说明
    1=PST Service Programing
    2=PST Software Download
    3=PST Service Support Tool
    4=PST Cellebrite Tool
    6=PST Phone Book Programming
    */
    typedef void (FAR __cdecl *ShowAboutDlg)(int);  //定义函数类型
    int             dlgType = 8;    //指定当前显示对话框标题为PST Software Download 
    HINSTANCE       hmod;
    ShowAboutDlg    lpproc;

    hmod = ::LoadLibrary("ZPST_About.dll");     //装载dll文件
    if (hmod == NULL)
    {
        AfxMessageBox("Fail");
    }
    //给函数指针赋值
    lpproc = (ShowAboutDlg) GetProcAddress(hmod,"ShowAbout");
    if (lpproc != (ShowAboutDlg) NULL)
    {
        (*lpproc) (dlgType);        //执行显示函数
    }
    FreeLibrary(hmod);          //释放实例
}

bool CSST_F350Dlg::OnButtonExit()
{
    // TODO: Add your control notification handler code here
    if (IDYES != MessageBox("Are you sure to quit?","Quit",MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2))
    {
        return false;
    }

    m_PageDL.SaveIniFile();

    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Close...... "); 
    UpdateData(FALSE);      

    TerminateThread(pStateThread->m_hThread,1);
    //  Sleep(200);
    EndDialog(1);
    return true;
}

void CSST_F350Dlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    /*  if(PageActiveNum == 11)
        {
            return;
        }
    */  
    if (!OnButtonExit())
    {
        return;
    }
    CDialog::OnClose();
}

void CSST_F350Dlg::OnCancel()
{
    // TODO: Add your message handler code here and/or call default
    if (!OnButtonExit())
    {
        return;
    }
    CDialog::OnClose();
}

void CSST_F350Dlg::OnOK()
{
    // TODO: Add your message handler code here and/or call default
    return;
}

BOOL CSST_F350Dlg::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class

    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == 0xd || pMsg->wParam == VK_ESCAPE)
        {
            return true;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CSST_F350Dlg::SlideWnd(int *buf, int &len, int *NoRspbuf, int &NoRsplen)
{
    int nRetAddress;
    /* 判断返回的地址和buf中地址是否一致，如果一致消除比较 */
    if (m_pPacket->m_retPacket.len != 0)
    {
        for (int i = 0 ; i < m_pPacket->m_retPacket.len ; i++)
        {
            nRetAddress = m_pPacket->m_retPacket.ret[i];

            for (int j = 0 ; j < len ; j++)
            {
                if (nRetAddress == buf[j])
                {
                    //将j前面的地址归入无响应的NoRspbuf，最后重发
                    for (int k = 0 ; k < j ; k++)
                    {
                        NoRspbuf[NoRsplen++] = buf[k];
                    }

                    //将buf中j后面的地址前移, BUFSIZE为分配给数组的空间长度
                    int m   = 0;
                    for (k = j + 1 ; k < BUFSIZE ; k++)
                    {
                        buf[m++] = buf[k];

                        if (k == (BUFSIZE - 1))
                        {
                            buf[k] = 0;
                        }
                    }

                    len = len - j - 1;
                    break;
                }
            }
        }//for
    }
}

void CSST_F350Dlg::ShowProgress(double data)
{
    int     nper;
    CString buf;

    nper = (int) (data * 100);
    buf.Format(" Write Flash Complete: %d%%",nper);
    m_ProgressCtrl.SetPos(nper);
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText(buf);
}


BOOL CSST_F350Dlg::StreamDownloadConnect()
{
    if (!(m_pPacket->Hello()))
    {
        m_nConnectType = REPR_CONNECT;
        return FALSE;
    }

    HandsetInfo = "Phone in Stream Download Mode!";     
    m_nConnectType = SDLD_CONNECT;
    return TRUE;
}

BOOL CSST_F350Dlg::RepConnect()
{
    if (m_pPacket->Down())
    {
        HandsetInfo = "Phone in Repair Mode!";
        return TRUE;
    }

    if (PageActiveNum != 14)
    {
        m_nConnectType = UNKN_CONNECT;
    }

    return FALSE;
}

void CSST_F350Dlg::SetShowState()
{
    if (m_bRunSuccess)
    {
        SetDlgItemText(IDC_STATIC_COM,HandsetInfo);
        return;
    }
    if (!Conect0OK)
    {
        b_ComOpen = FALSE;          
        HandsetInfo = "";
        m_bmpStatus.ReLoadBitmap(IDB_SEARCHING);            
        m_clPort.ReLoadIcon(IDR_PORT_DISCONNECT);
        SetDlgItemText(IDC_STATIC_COM,HandsetInfo);
        return ;
    }

    m_ProgressCtrl.SetPos(0);
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Ready!");
    m_bmpStatus.ReLoadBitmap(IDB_READY);

    switch (m_nConnectType)
    {
        case DIAG_CONNECT:
        m_clPort.ReLoadIcon(IDR_PORT_CONNECT);
        m_PageDL.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
        break;
        case DLAD_CONNECT:
        m_clPort.ReLoadIcon(IDI_ICON_DOWNLOAD);
        m_PageDL.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
        break;
        case SDLD_CONNECT:
        m_clPort.ReLoadIcon(IDI_ICON_SDLOAD);   
        m_PageDL.GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
        break;
        case REPR_CONNECT:
        m_clPort.ReLoadIcon(IDI_ICON_REPAIR);
        m_PageDL.GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
        break;
        default:
        m_clPort.ReLoadIcon(IDR_PORT_DISCONNECT);
        m_PageDL.GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
        break;
    }
    SetDlgItemText(IDC_STATIC_COM,HandsetInfo);
}


BOOL CSST_F350Dlg::SDLModeSoftWare()
{
    m_ProgressCtrl.SetForeColour(RGB(64,64,255));
    m_ProgressCtrl.SetTextBkColour(RGB(255,255,64));
    if (!IsDownFileSelect())
    {
        return FALSE;
    }

    //握手Hello
    int Error   = 0;
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Hello connect!");   
    CString DLtimes;
    do
    {
        ::Sleep(1000);
        DLtimes.Format("%d",Error);
        if (Error > 0)
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("It will retry hello command......");
        }
        Error++;
    }
    while ((!(m_pPacket->Hello())) && (Error < 5));
    if (Error >= 5)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Hello command error. Please retry!");
        return FALSE;
    }
    ::Sleep(3000);

    //准备输出Nop
    if (!m_pPacket->Nop())
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Nop error!");
        return FALSE;
    }

    //Flash读写、下载
    CString DLTimes = "";
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Start download file, please wait ...");
    m_bEnableEraser = TRUE;
    Error = 0;  
    do
    {
        ::Sleep(1000);
        DLTimes.Format("%d",Error);
        if (Error > 0)
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("It will retry Download Image file 60 times. This is No. "
                                                     + DLTimes
                                                     + " ......");
        }
        Error++;
    }
    while (!(DLUserFile()) && (Error < 6));
    if (Error >= 6)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download the image file fail!");
        return FALSE;
    }
    ::Sleep(500);

    //在此对StreamDownload模式复位
    if (m_pPacket->ResetSDP())
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download the image file complete! Reseting......");
    }
    else
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Please reset the handset!");
    }
    ::Sleep(4000);

    return TRUE;
}

BOOL CSST_F350Dlg::RepairModeSoftware()
{
    DWORD   timeBegin, timeEnd;
    //启动计时
    timeBegin = GetTickCount();
    timeEnd = timeBegin;

    m_strFlashPath = m_PageDL.m_FlashfilePath + "\\Aprg1110.bin";
    m_PageDL.GetDlgItemText(IDC_EDIT_USERFILE_PATH,m_PageDL.m_UserFilePath);
    if (m_PageDL.m_UserFilePath == "")
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Please select the phone image file to be downloaded.");
        MessageBox("Please select the phone image file to be downloaded.","Warning",MB_ICONWARNING);
        return FALSE;
    }
    ConnectPhone();

    int Error   = 0;    
    //  增加读取参数命令
    Error = 0;
    Sleep(1000);
    dload_params_rsp_type   params;
    m_pPacket->ParamRequest(params);

    //  下载APRG文件到Ram中
    Error = 0;
    CString DLtimes = "";
    do
    {
        ::Sleep(1000);
        DLtimes.Format("%d",Error);
        if (Error > 0)
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("It will retry Download bootfile 10 times. This is No. "
                                                     + DLtimes
                                                     + " ......");
        }
        Error++;
    }
    while ((!(DLBootFile1())) && (Error < 10));
    if (Error >= 10)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download boot file fail!");
        return FALSE;
    }

    Error = 0;
    do
    {
        ::Sleep(1000);
        DLtimes.Format("%d",Error);
        if (Error > 0)
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("It will retry go command......");
        }
        Error++;
    }
    while ((!(m_pPacket->Go(0X80,0X00))) && (Error < 5));
    if (Error >= 5)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Go command error. Please retry!");
        return FALSE;
    }

    if (!SDLModeSoftWare())
    {
        return FALSE;
    }

    //  Error = 0;
    //  SetDlgItemText(IDC_STATIC_COM, _T(""));
    //  while(!ConnectPhone()&& Error<50)
    //  {
    //      GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Serching Handset...");      
    //      Error++;
    //      ::Sleep(500);
    //  }
    //  if(Error >=50)
    //  {
    //      return FALSE;
    //  }
    ShowRatio(0) ;
    CString strtemp;
    timeEnd = GetTickCount();
    int nSecond = (timeEnd - timeBegin) / 1000;
    int nMinute = nSecond / 60;
    nSecond = nSecond % 60;
    strtemp.Format("Total time is %d min %d sec",nMinute,nSecond) ;
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
    return TRUE;
}


BOOL CSST_F350Dlg::OnDeviceChange(UINT nEventType, DWORD dwData)
{
    DEV_BROADCAST_HDR  *theHd;
    CString             strport;
    int                 nport;

    //如果没有执行功能,则不执行任何操作
    if (!PageActiveNum)
    {
        return TRUE;
    }
    if (nEventType == DBT_DEVICEARRIVAL)
    {
        theHd = (DEV_BROADCAST_HDR *) dwData;
        if (theHd->dbch_devicetype == DBT_DEVTYP_PORT)
        {
            DEV_BROADCAST_PORT *pPort;
            pPort = (DEV_BROADCAST_PORT *) dwData;
            strport.Format("Device Arrival %s\n",pPort->dbcp_name);
            sscanf(pPort->dbcp_name,"COM%d",&nport);
            TRACE(strport); 
            if (m_bSuspend)
            {
                if (nport == m_ComPort1)
                {
                    m_bHaveRefresh = TRUE;
                    InitPort(m_ComPort1,m_nBoundRate);
                    pStateThread->ResumeThread();   
                    m_bSuspend = FALSE;
                }
            }
        }
    }

    if (nEventType == DBT_DEVICEREMOVECOMPLETE)
    {
        theHd = (DEV_BROADCAST_HDR *) dwData;
        if (theHd->dbch_devicetype == DBT_DEVTYP_PORT)
        {
            DEV_BROADCAST_PORT *pPort;
            pPort = (DEV_BROADCAST_PORT *) dwData;
            strport.Format("Device Remove %s \n",pPort->dbcp_name);
            sscanf(pPort->dbcp_name,"COM%d",&nport);

            if (nport == m_ComPort1)
            {
                pStateThread->SuspendThread();
                m_pPacket->Close(); 
                m_bSuspend = TRUE;
            }
            TRACE(strport);
        }
    }

    return TRUE;
}

BOOL CSST_F350Dlg::EraseEFS()
{
    CFile   hReadFile ;
    int     nTotalLen;                          //要写的文件总长度
    int     nBeginAddress   = 0xffffffffL;      //boot区的起始地址
    CString strtemp;
    byte    writeBuf[FLASH_WRITE_BLOCK_SIZE];   //按块写，块的大小

    nTotalLen = FLASH_WRITE_BLOCK_SIZE;
    for (int i = 0; i < FLASH_WRITE_BLOCK_SIZE; i++)
        writeBuf[i] = 0xFF;

    if (!(m_pPacket->StreamWrite(writeBuf,nBeginAddress,FLASH_WRITE_BLOCK_SIZE)))
    {
        strtemp = m_pPacket->m_strErrInfo;
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
        return FALSE ;
    }
    return TRUE;
}

BOOL CSST_F350Dlg::DownloadPartitionTable()
{
    CFile   hReadFile ;
    int     nTotalLen;                          //要写的文件总长度
    byte    writeBuf[FLASH_WRITE_BLOCK_SIZE];   //按块写，块的大小
    CString strtemp;

    if (!hReadFile.Open(m_PageDL.m_stSetting.partitionPath,CFile::modeRead | CFile::shareDenyWrite))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Open parition file failed!");
        return FALSE ;
    }
    nTotalLen = hReadFile.GetLength();

    if (nTotalLen <= 0)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("File length error!");       
        hReadFile.Close() ;
        return FALSE;
    }

    hReadFile.Read(writeBuf,nTotalLen);

    if (!(m_pPacket->partition(writeBuf,nTotalLen)))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(m_pPacket->m_strErrInfo);
        if (m_pPacket->m_RspPacket.rsp.patition.status == 1)
        {
            if (MessageBox("Partition table differs,do you want override partition table?",
                           "warning",
                           MB_ICONINFORMATION | MB_OKCANCEL) == IDOK)
            {
                m_pPacket->m_bOverridePtl = 1;
                if (m_pPacket->partition(writeBuf,nTotalLen))
                {
                    return TRUE;
                }
            }
        }

        return FALSE;
    }

    return TRUE;
}


BOOL CSST_F350Dlg::DownloadSecondbootloaderheader()
{
    CFile   hReadFile ;
    int     nTotalLen;                          //要写的文件总长度
    int     nBeginAddress   = 0x00000000L;      //boot区的起始地址
    //  int nEndAddress=0xFFFF7FFF;             //boot区的上限
    int     nWrittenLen     = 0;                        //已写长度
    int     nBlockLen       = 0;                        //各块的大小
    byte    writeBuf[FLASH_WRITE_BLOCK_SIZE];   //按块写，块的大小
    BOOL    bMore           = FALSE;
    CString strtemp;

    if (!hReadFile.Open(m_PageDL.m_stSetting.qcsblhd_cfgdataPath,CFile::modeRead | CFile::shareDenyWrite))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Open file failed!");
        return FALSE ;
    }
    nTotalLen = hReadFile.GetLength();

    if (nTotalLen <= 0)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("File length error!");
        hReadFile.Close() ;
        return FALSE;
    }

    if (!m_pPacket->OpenMulti(0x02,NULL,0))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Enter secondbootloaderheader mode failed!");        
        hReadFile.Close() ;
        return FALSE;
    }

    //写文件
    do
    {
        nBlockLen = nTotalLen - nWrittenLen;
        if (nBlockLen > FLASH_WRITE_BLOCK_SIZE)
        {
            nBlockLen = FLASH_WRITE_BLOCK_SIZE;
            bMore = TRUE;
        }
        else
        {
            bMore = FALSE;
        }
        hReadFile.Read(writeBuf,nBlockLen);
        if (!(m_pPacket->StreamWrite(writeBuf,nBeginAddress + nWrittenLen,nBlockLen)))
        {
            strtemp = m_pPacket->m_strErrInfo;
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
            hReadFile.Close() ;
            return FALSE ;
        }
        nWrittenLen += nBlockLen;
    }
    while (bMore);
    hReadFile.Close();
    if (!m_pPacket->ClosePacket())      //关闭下载模式
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Close packet failed");
        return FALSE;
    }

    return TRUE;
}


BOOL CSST_F350Dlg::DownloadSecondbootloader()
{
    CFile   hReadFile ;
    int     nTotalLen;                          //要写的文件总长度
    int     nBeginAddress   = 0x00000000L;      //boot区的起始地址
    //  int nEndAddress=0xFFFF7FFF;             //boot区的上限
    int     nWrittenLen     = 0;                        //已写长度
    int     nBlockLen       = 0;                        //各块的大小
    byte    writeBuf[FLASH_WRITE_BLOCK_SIZE];   //按块写，块的大小
    BOOL    bMore           = FALSE;
    CString strtemp;

    if (!hReadFile.Open(m_PageDL.m_stSetting.qcsblPath,CFile::modeRead | CFile::shareDenyWrite))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Open file failed!");
        return FALSE ;
    }
    nTotalLen = hReadFile.GetLength();

    if (nTotalLen <= 0)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("File length error!");
        hReadFile.Close() ;
        return FALSE;
    }

    if (!m_pPacket->OpenMulti(0x03,NULL,0))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Enter secondbootloader mode failed!");      
        hReadFile.Close() ;
        return FALSE;
    }

    //写文件
    do
    {
        nBlockLen = nTotalLen - nWrittenLen;
        if (nBlockLen > FLASH_WRITE_BLOCK_SIZE)
        {
            nBlockLen = FLASH_WRITE_BLOCK_SIZE;
            bMore = TRUE;
        }
        else
        {
            bMore = FALSE;
        }
        hReadFile.Read(writeBuf,nBlockLen);
        if (!(m_pPacket->StreamWrite(writeBuf,nBeginAddress + nWrittenLen,nBlockLen)))
        {
            strtemp = m_pPacket->m_strErrInfo;
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
            hReadFile.Close() ;
            return FALSE ;
        }
        nWrittenLen += nBlockLen;
    }
    while (bMore);
    hReadFile.Close();

    if (!m_pPacket->ClosePacket())//关闭下载模式
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Close packet failed");
        return FALSE;
    }
    return TRUE;
}



BOOL CSST_F350Dlg::DownloadOEMbootloader()
{
    CFile   hReadFile ;
    int     nTotalLen;                          //要写的文件总长度
    int     nBeginAddress   = 0x00000000L;      //boot区的起始地址
    //  int nEndAddress=0xFFFF7FFF;             //boot区的上限
    int     nWrittenLen     = 0;                        //已写长度
    int     nBlockLen       = 0;                        //各块的大小
    byte    writeBuf[FLASH_WRITE_BLOCK_SIZE];   //按块写，块的大小
    BOOL    bMore           = FALSE;
    CString strtemp;


    if (!hReadFile.Open(m_PageDL.m_stSetting.oemsblhdPath,CFile::modeRead | CFile::shareDenyWrite))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Open file failed!");
        return FALSE ;
    }
    nTotalLen = hReadFile.GetLength();
    if (nTotalLen <= 0)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("File length error!");
        hReadFile.Close() ;
        return FALSE;
    }
    hReadFile.Read(writeBuf,nTotalLen);

    if (!m_pPacket->OpenMulti(0x04,writeBuf,nTotalLen))
    {
        hReadFile.Close() ;
        return FALSE;
    }

    hReadFile.Close();
    if (!hReadFile.Open(m_PageDL.m_stSetting.oemsblPath,CFile::modeRead | CFile::shareDenyWrite))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Open file failed!");
        return FALSE ;
    }
    nTotalLen = hReadFile.GetLength();
    if (nTotalLen <= 0)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("File length error!");
        hReadFile.Close() ;
        return FALSE;
    }

    memset(&writeBuf,0,sizeof(writeBuf));
    //写文件
    do
    {
        nBlockLen = nTotalLen - nWrittenLen;
        if (nBlockLen > FLASH_WRITE_BLOCK_SIZE)
        {
            nBlockLen = FLASH_WRITE_BLOCK_SIZE;
            bMore = TRUE;
        }
        else
        {
            bMore = FALSE;
        }
        hReadFile.Read(writeBuf,nBlockLen);
        if (!(m_pPacket->StreamWrite(writeBuf,nBeginAddress + nWrittenLen,nBlockLen)))
        {
            strtemp = m_pPacket->m_strErrInfo;
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
            hReadFile.Close() ;
            return FALSE ;
        }
        nWrittenLen += nBlockLen;
    }
    while (bMore);
    hReadFile.Close();

    if (!m_pPacket->ClosePacket())      //关闭下载模式
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Close packet failed");
        return FALSE;
    }
    return TRUE;
}

BOOL CSST_F350Dlg::DLUserFile()
{
    //进入安全模式
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Entry safe mode...");   
    if (!m_pPacket->Security())
    {
        Sleep(5000) ;//zhoulei add 070404 保护措施防止下载失败
        return FALSE;
    }
    //下载分区表
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download partition table...");
    if (!DownloadPartitionTable())
    {
        Sleep(5000) ;
        return FALSE;
    }

    //下载Secondbootloaderheader文件
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download secondbootloaderheader...");   
    if (!DownloadSecondbootloaderheader())
    {
        Sleep(5000) ;//zhoulei add 070404 保护措施防止下载失败
        return FALSE;
    }

    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download secondbootloader...");     
    if (!DownloadSecondbootloader())
    {
        Sleep(5000) ;//zhoulei add 070404 保护措施防止下载失败
        return FALSE;
    }

    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download oem...");  
    if (!DownloadOEMbootloader())
    {
        Sleep(5000) ;//zhoulei add 070404 保护措施防止下载失败
        return FALSE;
    }

    //下载软件版本文件
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download amss..."); 
    int nError  = 0;
    //while(!DownloadImage() && nError<3)
    while (!HSDLImage() && nError < 3)
    {
        Sleep(2000);
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download amss...");         
        nError++;
    }
    if (nError >= 3)
    {
        Sleep(5000);
        return FALSE;
    }

    //下载EFS文件
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download efs...");  
    nError = 0;
    //while(!DownloadEFSmbn() && nError<3)
    while (!HSDL_EFSmbn() && nError < 3)
    {
        Sleep(2000);        
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download efs...");          
        nError++;
    }
    if (nError >= 3)
    {
        Sleep(5000);        
        return FALSE;
    }

    Sleep(3000);

    //重新启动手机
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Resetting phone......");
    return TRUE;
}
BOOL CSST_F350Dlg::DownloadEFSmbn()
{
    CFile   hReadFile ;
    int     nTotalLen;                          //要写的文件总长度
    int     nBeginAddress       = 0x00000000L;      //起始地址
    int     nWrittenLen         = 0;                        //已写长度
    int     nBlockLen           = 0;                        //各块的大小
    int     TotalBlock          = 0;                        //总块数
    byte    writeBuf[FLASH_WRITE_BLOCK_SIZE];   //按块写，块的大小
    BOOL    bMore               = FALSE;

    DWORD   tStart, tEnd, dtime;
    LONG    offset;
    int     nAddress;
    int     nAddressbuf[BUFSIZE];               //记录发送地址的数组
    int     nAddressbuflen      = 0;                    //记录发送地址个数
    int     nNoRspAddressbuf[BUFSIZE];          //记录未返回地址的数组
    int     nNoRspAddressbuflen = 0;            //记录未返回地址个数

    int     nTempCount          = 0;                        //计数器，用来显示进度
    int     nOneStep;                           //百分之一进度的块数
    int     MinRate             = 4;
    int     MaxRate             = 100;
    int     CurRate             = MinRate;//lichenyang

    ShowRatio(0);   
    CString strtemp = _T("");
    if (!hReadFile.Open(m_PageDL.m_stSetting.efsmbnPath,CFile::modeRead | CFile::shareDenyWrite))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Open file failed!");
        return FALSE ;
    }
    nTotalLen = hReadFile.GetLength();

    if (nTotalLen <= 0)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("File length error!");
        hReadFile.Close() ;
        return FALSE;
    }

    if (!m_pPacket->OpenMulti(0x09,NULL,0))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Entry efs mode failed!");       
        hReadFile.Close() ;
        return FALSE;
    }

    memset(&writeBuf,0,sizeof(writeBuf));
    //显示当前百分比
    ShowRatio(0);

    nTempCount = 0;
    TotalBlock = nTotalLen / FLASH_WRITE_BLOCK_SIZE;
    nOneStep = TotalBlock / (MaxRate - MinRate);
    do
    {
        nBlockLen = nTotalLen - nWrittenLen;
        if (nBlockLen > FLASH_WRITE_BLOCK_SIZE)
        {
            nBlockLen = FLASH_WRITE_BLOCK_SIZE;
            bMore = TRUE;
        }
        else
        {
            bMore = FALSE;
        }
        hReadFile.Read(writeBuf,nBlockLen);
        nAddress = nBeginAddress + nWrittenLen;

        //发送数据多次，根据滑动窗口的长度接收
        //      tStart = GetTickCount();
        if (!(m_pPacket->StreamWriteAsy(writeBuf,nAddress,nBlockLen)))
        {
            strtemp = m_pPacket->m_strErrInfo;
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp); 
            hReadFile.Close() ;
            return FALSE ;
        }

        nWrittenLen += nBlockLen;
        nTempCount++;
        if (nTempCount >= nOneStep)   //每下载百分之一，进行一次进度显示
        {
            CurRate++ ;         
            ShowRatio(CurRate) ;                        
            nTempCount = 0;
            //
            //          if(!pDoc->m_bRunning[Index])
            //          {
            //              ShowInfo(Index,"下载线程被人为终止!"); 
            //              return FALSE ;
            //          }
        }

        nAddressbuf[nAddressbuflen++] = nAddress;

        /* 判断是否连续发送了多块数据 */
        /* 超过滑动窗口的尺寸，需要等待返回正确地址，才能继续 */
        tStart = GetTickCount();
        while (nAddressbuflen >= TXLIMIT)
        {
            /* 读取返回地址 */
            m_pPacket->DLRxPacket();
            /* 对返回地址进行判断 */
            SlideWnd(nAddressbuf,nAddressbuflen,nNoRspAddressbuf,nNoRspAddressbuflen);
            /* 无响应超时，认为写操作失败 */
            tEnd = GetTickCount();
            dtime = tEnd - tStart;
            if (dtime >= 10000)
            {
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Write command failed!"); 
                hReadFile.Close() ;
                return FALSE ;
            }
        }   
        Sleep(5);
    }
    while (bMore);

    /* 判断发送地址是否返回 */
    if (nAddressbuflen)
    {
        /* 查询是否还有地址返回 */
        for (int i = 0 ; i < 3 ; i++)
        {
            ::Sleep(50);
            m_pPacket->DLRxPacket();
            SlideWnd(nAddressbuf,nAddressbuflen,nNoRspAddressbuf,nNoRspAddressbuflen);
            if (!nAddressbuflen)
            {
                break;
            }
        }
        /* 如果还有发送的地址未返回,则归入无响应buf */
        if (nAddressbuflen)
        {
            for (i = 0 ; i < nAddressbuflen ; i++)
            {
                nNoRspAddressbuf[nNoRspAddressbuflen++] = nAddressbuf[i];
            }
        }
    }

    /* 判断是否重发数据块 */    
    if (nNoRspAddressbuflen)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Rewrite data block!"); 
        //重写buf中对应地址的数据
        for (int i = 0 ; i < nNoRspAddressbuflen ; i++)
        {
            nAddress = nNoRspAddressbuf[i];
            offset = nAddress - nBeginAddress;

            nBlockLen = nTotalLen - offset;
            if (nBlockLen > FLASH_WRITE_BLOCK_SIZE)
            {
                nBlockLen = FLASH_WRITE_BLOCK_SIZE;
            }

            hReadFile.Seek(offset,CFile::begin);
            hReadFile.Read(writeBuf,nBlockLen);
            if (!(m_pPacket->StreamWrite(writeBuf,nAddress,nBlockLen)))
            {
                strtemp = m_pPacket->m_strErrInfo;
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp); 
                hReadFile.Close();
                return FALSE ;
            }
            //          nAddressbuf[nAddressbuflen++] = nAddress;
        }

        //      nNoRspAddressbuflen = 0;
        //
        //      /* 对返回地址进行判断 */
        //      for(i = 0 ; i < 3 ; i++)
        //      {
        //          ::Sleep(50);
        //          pDoc->m_Packet[Index].DLRxPacket();
        //          SlideWnd(Index, nAddressbuf, nAddressbuflen, nNoRspAddressbuf, nNoRspAddressbuflen);
        //          
        //          if(!nAddressbuflen)
        //              break;
        //      }
    }

    hReadFile.Close();

    //  if(nAddressbuflen||nNoRspAddressbuflen)
    //  {
    //      ShowInfo(Index,"重写文件失败");
    //      return FALSE;
    //  }

    if (!m_pPacket->ClosePacket())      //关闭下载模式
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Close packet failed");
        return FALSE;
    }
    ShowRatio(MaxRate) ;
    return TRUE;
}

BOOL CSST_F350Dlg::DownloadImage()
{
    CFile   hReadFile ;
    int     nTotalLen;                          //要写的文件总长度
    int     nBeginAddress       = 0x00000000L;      //起始地址
    int     nWrittenLen         = 0;                        //已写长度
    int     nBlockLen           = 0;                        //各块的大小
    int     TotalBlock          = 0;                        //总块数
    byte    writeBuf[FLASH_WRITE_BLOCK_SIZE];   //按块写，块的大小
    BOOL    bMore               = FALSE;

    DWORD   tStart, tEnd, dtime;
    LONG    offset;
    int     nAddress;
    int     nAddressbuf[BUFSIZE];               //记录发送地址的数组
    int     nAddressbuflen      = 0;                    //记录发送地址个数
    int     nNoRspAddressbuf[BUFSIZE];          //记录未返回地址的数组
    int     nNoRspAddressbuflen = 0;            //记录未返回地址个数

    int     nTempCount          = 0;                        //计数器，用来显示进度
    int     nOneStep;                           //百分之一进度的块数
    int     MinRate             = 4;
    int     MaxRate             = 98;
    int     CurRate             = MinRate;

    CString strtemp             = _T("");
    if (!hReadFile.Open(m_PageDL.m_stSetting.amsshdPath,CFile::modeRead | CFile::shareDenyWrite))
    {
        Sleep(1000);
        if (!hReadFile.Open(m_PageDL.m_stSetting.amsshdPath,CFile::modeRead | CFile::shareDenyWrite))
        {
            Sleep(2000);
            if (!hReadFile.Open(m_PageDL.m_stSetting.amsshdPath,CFile::modeRead | CFile::shareDenyWrite))
            {
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Open file failed!"); 
                return FALSE ;
            }
        }
    }
    nTotalLen = hReadFile.GetLength();

    if (nTotalLen <= 0)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("File length error!");
        hReadFile.Close() ;
        return FALSE;
    }

    hReadFile.Read(writeBuf,nTotalLen);
    if (!m_pPacket->OpenMulti(0x05,writeBuf,nTotalLen))
    {
        Sleep(1000);
        if (!m_pPacket->OpenMulti(0x05,writeBuf,nTotalLen))
        {
            Sleep(2000);
            if (!m_pPacket->OpenMulti(0x05,writeBuf,nTotalLen))
            {
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Entry down amss mode failed!");
                hReadFile.Close() ;
                return FALSE;
            }
        }
    }

    hReadFile.Close();
    memset(&writeBuf,0,sizeof(writeBuf));

    if (!hReadFile.Open(m_PageDL.m_stSetting.amssPath,CFile::modeRead | CFile::shareDenyWrite))
    {
        Sleep(1000);
        if (!hReadFile.Open(m_PageDL.m_stSetting.amssPath,CFile::modeRead | CFile::shareDenyWrite))
        {
            Sleep(1000);
            if (!hReadFile.Open(m_PageDL.m_stSetting.amssPath,CFile::modeRead | CFile::shareDenyWrite))
            {
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Open file failed!"); 
                return FALSE ;
            }
        }
    }
    nTotalLen = hReadFile.GetLength();
    //显示当前百分比
    ShowRatio(0) ;

    nTempCount = 0;
    TotalBlock = nTotalLen / FLASH_WRITE_BLOCK_SIZE;
    nOneStep = TotalBlock / (MaxRate - MinRate);
    do
    {
        nBlockLen = nTotalLen - nWrittenLen;
        if (nBlockLen > FLASH_WRITE_BLOCK_SIZE)
        {
            nBlockLen = FLASH_WRITE_BLOCK_SIZE;
            bMore = TRUE;
        }
        else
        {
            bMore = FALSE;
        }
        hReadFile.Read(writeBuf,nBlockLen);
        nAddress = nBeginAddress + nWrittenLen;

        //发送数据多次，根据滑动窗口的长度接收
        //      tStart = GetTickCount();

        if (!(m_pPacket->StreamWriteAsy(writeBuf,nAddress,nBlockLen)))
        {
            Sleep(1000);
            if (!(m_pPacket->StreamWriteAsy(writeBuf,nAddress,nBlockLen)))
            {
                Sleep(3000);
                if (!(m_pPacket->StreamWriteAsy(writeBuf,nAddress,nBlockLen)))
                {
                    strtemp = m_pPacket->m_strErrInfo;
                    GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp); 
                    hReadFile.Close() ;
                    return FALSE ;
                }
            }
        }

        nWrittenLen += nBlockLen;
        nTempCount++;
        if (nTempCount >= nOneStep)   //每下载百分之一，进行一次进度显示
        {
            CurRate++ ;         
            ShowRatio(CurRate) ;                        
            nTempCount = 0;
        }
        nAddressbuf[nAddressbuflen++] = nAddress;
        /* 判断是否连续发送了多块数据 */
        /* 超过滑动窗口的尺寸，需要等待返回正确地址，才能继续 */
        tStart = GetTickCount();
        while (nAddressbuflen >= TXLIMIT)
        {
            /* 读取返回地址 */
            m_pPacket->DLRxPacket();
            /* 对返回地址进行判断 */
            SlideWnd(nAddressbuf,nAddressbuflen,nNoRspAddressbuf,nNoRspAddressbuflen);
            /* 无响应超时，认为写操作失败 */
            tEnd = GetTickCount();
            dtime = tEnd - tStart;
            if (dtime >= 20000)//10000
            {
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Write operation fail, because no response"); 
                hReadFile.Close() ;
                return FALSE ;
            }
            strtemp.Format("Buffer Len %d\n",nAddressbuflen);
            TRACE(strtemp);
        }
        Sleep(5);//test
    }
    while (bMore);

    /* 判断发送地址是否返回 */
    if (nAddressbuflen)
    {
        /* 查询是否还有地址返回 */
        for (int i = 0 ; i < 3 ; i++)
        {
            ::Sleep(50);
            m_pPacket->DLRxPacket();
            //pDoc->LogCurrentPacketError(Index);
            SlideWnd(nAddressbuf,nAddressbuflen,nNoRspAddressbuf,nNoRspAddressbuflen);
            if (!nAddressbuflen)
            {
                break;
            }
        }
        /* 如果还有发送的地址未返回,则归入无响应buf */
        if (nAddressbuflen)
        {
            for (i = 0 ; i < nAddressbuflen ; i++)
            {
                nNoRspAddressbuf[nNoRspAddressbuflen++] = nAddressbuf[i];
            }
        }
    }

    /* 判断是否重发数据块 */    
    if (nNoRspAddressbuflen)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Write data repeat!"); 
        //重写buf中对应地址的数据
        for (int i = 0 ; i < nNoRspAddressbuflen ; i++)
        {
            nAddress = nNoRspAddressbuf[i];
            offset = nAddress - nBeginAddress;

            nBlockLen = nTotalLen - offset;
            if (nBlockLen > FLASH_WRITE_BLOCK_SIZE)
            {
                nBlockLen = FLASH_WRITE_BLOCK_SIZE;
            }

            hReadFile.Seek(offset,CFile::begin);
            hReadFile.Read(writeBuf,nBlockLen);

            //  if(!(pDoc->m_Packet[Index].StreamWriteAsy(writeBuf, nAddress, nBlockLen)))
            if (!(m_pPacket->StreamWrite(writeBuf,nAddress,nBlockLen)))//lichenyang 20070405
            {
                Sleep(1000);
                if (!(m_pPacket->StreamWrite(writeBuf,nAddress,nBlockLen)))//lichenyang 20070405
                {
                    Sleep(2000);
                    if (!(m_pPacket->StreamWrite(writeBuf,nAddress,nBlockLen)))//lichenyang 20070405
                    {
                        strtemp = m_pPacket->m_strErrInfo;
                        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp); 
                        hReadFile.Close();
                        return FALSE ;
                    }
                }
            }
            //          nAddressbuf[nAddressbuflen++] = nAddress;
        }

        //      nNoRspAddressbuflen = 0;
        //
        //      /* 对返回地址进行判断 */
        //      for(i = 0 ; i < 3 ; i++)
        //      {
        //          ::Sleep(50);
        //          pDoc->m_Packet[Index].DLRxPacket();
        //          pDoc->LogCurrentPacketError(Index);
        //          SlideWnd(Index, nAddressbuf, nAddressbuflen, nNoRspAddressbuf, nNoRspAddressbuflen);
        //          
        //          if(!nAddressbuflen)
        //              break;
        //      }
    }

    hReadFile.Close();

    //  if(nAddressbuflen||nNoRspAddressbuflen)
    //  {
    //      ShowInfo(Index,"repeat writing file fail");
    //      return FALSE;
    //  }

    if (!m_pPacket->ClosePacket())      //关闭下载模式
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Close packet error");
        return FALSE;
    }

    ShowRatio(MaxRate);
    return TRUE;
}
void CSST_F350Dlg::ShowRatio(int ratioValue, int max)
{
    if (ratioValue > 100)
    {
        ratioValue = 100;
    }
    else if (ratioValue < 0)
    {
        ratioValue = 0;
    }

    //显示当前百分比和进度条
    if (ratioValue == 0)
    {
        //初始化进度条
        m_ProgressCtrl.SetRange(0,max);
        m_ProgressCtrl.SetPos(ratioValue);
    }
    else
    {
        m_ProgressCtrl.SetPos(ratioValue);
    }
}

BOOL CSST_F350Dlg::BackupNV(CString NVFileName, int nBackupType)
{
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Backup nv item....");
    //CString strNVInfo = _T("");
    BOOL    btemp       = FALSE;
    DWORD   timeBegin, timeEnd;
    CString NVFileTemp  = "";
    CString strNVMFile  = "";
    CString strnv       = "";
    NVFileTemp.Format("%08x",m_nESN);
    strNVMFile = NVFileName + "\\" + "NV_" + NVFileTemp + ".NVM";
    m_strBackupNvPath = strNVMFile;
    //启动计时
    CString strtemp ;
    timeBegin = GetTickCount();
    timeEnd = timeBegin;

    nv_item_list_info   rfcallistCur[MAX_NV_LIST_NUM]   =
    {
        {NV_MEID_I},            //      1943            
        {NV_ESN_I},//=0
        {5494},//NV_PPPPASSWORD_CONT2_I chenhongyu
        {5489},//NV_PPPPASSWORD_CONT1_I
        {5551},//NV_WAPGATEPASSWORD_CONT1_I
        {5553},//NV_WAPGATEPASSWORD_CONT2_I
        {5555},//NV_MMSPASSWORD_CONT1_I
        {5558},//NV_MMSPASSWORD_CONT2_I
        {5501},//NV_BREWPASSWORD_CONT1_I
        {5503},//NV_BREWPASSWORD_CONT2_I
        {5452}, //新增C79的NV备份参数,Wap Password
        {5455}, //
        {5386}, //
        {5391}, //
        {4388}, {4371}, {4374}, {85}, // NV 产线不备份SPC
        {NV_MANUFACTORY_INFO_I}, //chenhongyu 20080729
        {447}, //蓝牙地址   
        //GPS
        //NV449,NV555,NV736,NV1624,NV1713,NV4272
        {449}, {555}, {736}, {4272},
        //BC0, BC1 for D90
        {1713}, //NV_BC0_GPS1_RF_DELAY_I //C88 dont backup this item
        {1715}, //NV_BC0_TX_LIM_VS_TEMP_I
        {1716}, //NV_BC0_TX_LIN_MASTER0_I 
        {1717}, //NV_BC0_TX_LIN_MASTER1_I 
        {1718}, //NV_BC0_TX_LIN_MASTER2_I 
        {1719}, //NV_BC0_TX_LIN_MASTER3_I
        {1720}, //NV_BC0_TX_COMP0_I
        {1721}, //NV_BC0_TX_COMP1_I
        {1722}, //NV_BC0_TX_COMP2_I
        {1723}, //NV_BC0_TX_COMP3_I
        {1724}, //NV_BC0_TX_LIM_VS_FREQ_I
        {1725}, //NV_BC0_PA_R1_RISE_I
        {1726}, //NV_BC0_PA_R1_FALL_I
        {1727}, //NV_BC0_PA_R2_RISE_I
        {1728}, //NV_BC0_PA_R2_FALL_I
        {1729}, //NV_BC0_PA_R3_RISE_I
        {1730}, //NV_BC0_PA_R3_FALL_I
        {1731}, //NV_BC0_HDET_OFF_I
        {1732}, //NV_BC0_HDET_SPN_I
        {1733}, //NV_BC0_EXP_HDET_VS_AGC_I
        {1734}, //NV_BC0_ENC_BTF_I //C88 dont backup this item
        {1735}, //NV_BC0_VCO_COARSE_TUNE_TABLE_I
        {1736}, //NV_BC0_P1_RISE_FALL_OFF_I //C88 dont backup this item
        {1879}, //NV_BC0_HDR_IM_FALL_I
        {1880}, //NV_BC0_HDR_IM_RISE_I
        {1890}, //NV_BC0_HDR_IM_LEVEL_I
        {2080}, //NV_BC0_VCO_COARSE_TUNE_2_I
        {2814}, //NV_BC0_HDR_P1_RISE_FALL_OFF_I
        {3035}, //NV_BC0_PA_DVS_VOLTAGE_I
        {3237}, //NV_BC0_VCO_TUNE_2_I
        {3386}, //NV_BC0_HDR_G0_IM_FALL_I
        {3387}, //NV_BC0_HDR_G0_IM_RISE_I
        {3388}, //NV_BC0_HDR_G0_IM_LEVEL_I
        {3420}, //NV_BC0_ANT_QUALITY_I  
        {1737}, //NV_C0_BC0_TX_CAL_CHAN_I //C88 dont backup this item
        {1738}, //NV_C0_BC0_RX_CAL_CHAN_I //C88 dont backup this item
        {1739}, //NV_C0_BC0_LNA_1_OFFSET_VS_FREQ_I
        {1740}, //NV_C0_BC0_LNA_2_OFFSET_VS_FREQ_I
        {1741}, //NV_C0_BC0_LNA_3_OFFSET_VS_FREQ_I
        {1742}, //NV_C0_BC0_LNA_4_OFFSET_VS_FREQ_I
        {1743}, //NV_C0_BC0_LNA_1_OFFSET_I
        {1744}, //NV_C0_BC0_LNA_2_OFFSET_I
        {1745}, //NV_C0_BC0_LNA_3_OFFSET_I
        {1746}, //NV_C0_BC0_LNA_4_OFFSET_I
        {1747}, //NV_C0_BC0_IM2_I_VALUE_I
        {1748}, //NV_C0_BC0_IM2_Q_VALUE_I
        {1749}, //NV_C0_BC0_VGA_GAIN_OFFSET_I
        {1750}, //NV_C0_BC0_VGA_GAIN_OFFSET_VS_FREQ_I
        {1751}, //NV_C0_BC0_IM2_TRANSCONDUCTOR_VALUE_I
        {1752}, //NV_C0_BC0_LNA_1_RISE_I
        {1753}, //NV_C0_BC0_LNA_1_FALL_I
        {1754}, //NV_C0_BC0_LNA_2_RISE_I
        {1755}, //NV_C0_BC0_LNA_2_FALL_I
        {1756}, //NV_C0_BC0_LNA_3_RISE_I
        {1757}, //NV_C0_BC0_LNA_3_FALL_I
        {1758}, //NV_C0_BC0_LNA_4_RISE_I
        {1759}, //NV_C0_BC0_LNA_4_FALL_I
        {1760}, //NV_C0_BC0_IM_LEVEL1_I
        {1761}, //NV_C0_BC0_IM_LEVEL2_I
        {1762}, //NV_C0_BC0_IM_LEVEL3_I
        {1763}, //NV_C0_BC0_IM_LEVEL4_I     
        {1624}, //NV_BC1_GPS1_RF_DELAY_I  //C88 dont backup this item
        //{1625}, //NV_BC1_RF_TUNE_RESERVED_I
        {1626}, //NV_BC1_TX_LIM_VS_TEMP_I
        {1627}, //NV_BC1_TX_LIN_MASTER0_I
        {1628}, //NV_BC1_TX_LIN_MASTER1_I
        {1629}, //NV_BC1_TX_LIN_MASTER2_I
        {1630}, //NV_BC1_TX_LIN_MASTER3_I
        {1631}, //NV_BC1_TX_COMP0_I
        {1632}, //NV_BC1_TX_COMP1_I
        {1633}, //NV_BC1_TX_COMP2_I
        {1634}, //NV_BC1_TX_COMP3_I
        {1635}, //NV_BC1_TX_LIM_VS_FREQ_I
        {1636}, //NV_BC1_PA_R1_RISE_I
        {1637}, //NV_BC1_PA_R1_FALL_I
        {1638}, //NV_BC1_PA_R2_RISE_I
        {1639}, //NV_BC1_PA_R2_FALL_I
        {1640}, //NV_BC1_PA_R3_RISE_I
        {1641}, //NV_BC1_PA_R3_FALL_I
        {1642}, //NV_BC1_HDET_OFF_I
        {1643}, //NV_BC1_HDET_SPN_I
        {1644}, //NV_BC1_EXP_HDET_VS_AGC_I
        {1645}, //NV_BC1_ENC_BTF_I  //C88 dont backup this item
        {1646}, //NV_BC1_VCO_COARSE_TUNE_TABLE_I
        {1647}, //NV_BC1_P1_RISE_FALL_OFF_I  //C88 dont backup this item
        {2081}, //NV_BC1_VCO_COARSE_TUNE_2_I
        {2815}, //NV_BC1_HDR_P1_RISE_FALL_OFF_I
        {3036}, //NV_BC1_PA_DVS_VOLTAGE_I
        {3238}, //NV_BC1_VCO_TUNE_2_I
        {3421}, //NV_BC1_ANT_QUALITY_I  
        //NV_C0_BC1
        {1648}, //NV_C0_BC1_TX_CAL_CHAN_I  //C88 dont backup this item
        {1649}, //NV_C0_BC1_RX_CAL_CHAN_I  //C88 dont backup this item
        {1650}, //NV_C0_BC1_LNA_1_OFFSET_VS_FREQ_I
        {1651}, //NV_C0_BC1_LNA_2_OFFSET_VS_FREQ_I
        {1652}, //NV_C0_BC1_LNA_3_OFFSET_VS_FREQ_I
        {1653}, //NV_C0_BC1_LNA_4_OFFSET_VS_FREQ_I
        {1654}, //NV_C0_BC1_LNA_1_OFFSET_I
        {1655}, //NV_C0_BC1_LNA_2_OFFSET_I
        {1666}, //NV_C0_BC1_LNA_3_OFFSET_I
        {1667}, //NV_C0_BC1_LNA_4_OFFSET_I
        {1668}, //NV_C0_BC1_IM2_I_VALUE_I
        {1669}, //NV_C0_BC1_IM2_Q_VALUE_I
        {1670}, //NV_C0_BC1_VGA_GAIN_OFFSET_I
        {1671}, //NV_C0_BC1_VGA_GAIN_OFFSET_VS_FREQ_I
        {1672}, //NV_C0_BC1_IM2_TRANSCONDUCTOR_VALUE_I
        {1673}, //NV_C0_BC1_LNA_1_RISE_I
        {1674}, //NV_C0_BC1_LNA_1_FALL_I
        {1675}, //NV_C0_BC1_LNA_2_RISE_I
        {1676}, //NV_C0_BC1_LNA_2_FALL_I
        {1677}, //NV_C0_BC1_LNA_3_RISE_I
        {1678}, //NV_C0_BC1_LNA_3_FALL_I
        {1679}, //NV_C0_BC1_LNA_4_RISE_I
        {1680}, //NV_C0_BC1_LNA_4_FALL_I
        {1681}, //NV_C0_BC1_IM_LEVEL1_I
        {1682}, //NV_C0_BC1_IM_LEVEL2_I
        {1683}, //NV_C0_BC1_IM_LEVEL3_I
        {1684}, //NV_C0_BC1_IM_LEVEL4_I
        {2793}, //NV_C0_BC1_RX_CAL_CHAN_LRU_I
        {3254}, //NV_C0_BC1_IM2_LPM_1_I
        {3260}, //NV_C0_BC1_IM2_LPM_2_I
        {3266}, //NV_C0_BC1_INTELLICEIVER_CAL_I
        {3272}, //NV_C0_BC1_LNA_SWITCHPOINTS_LPM_1_I
        {3278}, //NV_C0_BC1_LNA_SWITCHPOINTS_LPM_2_I
        {2815}, //NV_BC1_HDR_P1_RISE_FALL_OFF_I
        {3036}, //NV_BC1_PA_DVS_VOLTAGE_I
        {3238}, //NV_BC1_VCO_TUNE_2_I
        {3421}, //NV_BC1_ANT_QUALITY_I
        {2791}, //NV_C0_BC0_RX_CAL_CHAN_LRU_I
        {3253}, //NV_C0_BC0_IM2_LPM_1_I
        {3259}, //NV_C0_BC0_IM2_LPM_2_I
        {3265}, //NV_C0_BC0_INTELLICEIVER_CAL_I
        {3271}, //NV_C0_BC0_LNA_SWITCHPOINTS_LPM_1_I
        {3277}, //NV_C0_BC0_LNA_SWITCHPOINTS_LPM_2_I
        //{3469}, //NV_C0_BC0_INTELLICEIVER_DET_THRESH_I
        //BC15
        {3265},//NV_C0_BC0_INTELLICEIVER_CAL_I           
        {3266},//NV_C0_BC1_INTELLICEIVER_CAL_I           
        //{3469},//NV_C0_BC0_INTELLICEIVER_DET_THRESH_I    
        //{3470},//NV_C0_BC1_INTELLICEIVER_DET_THRESH_I    
        //BC15
        {4273},//NV_BC15_TX_LIM_VS_TEMP_I                
        {4274},//NV_BC15_TX_COMP0_I                      
        {4275},//NV_BC15_TX_COMP1_I                      
        {4276},//NV_BC15_TX_COMP2_I                      
        {4277},//NV_BC15_TX_COMP3_I                      
        {4278},//NV_BC15_TX_LIM_VS_FREQ_I                
        {4279},//NV_BC15_PA_R1_RISE_I                    
        {4280},//NV_BC15_PA_R1_FALL_I                    
        {4281},//NV_BC15_PA_R2_RISE_I                    
        {4282},//NV_BC15_PA_R2_FALL_I                    
        {4283},//NV_BC15_PA_R3_RISE_I                    
        {4284},//NV_BC15_PA_R3_FALL_I                    
        {4285},//NV_BC15_HDET_OFF_I                      
        {4286},//NV_BC15_HDET_SPN_I                      
        {4287},//NV_BC15_EXP_HDET_VS_AGC_I               
        {4288},//NV_BC15_ENC_BTF_I                       
        {4289},//NV_BC15_VCO_COARSE_TUNE_TABLE_I         
        {4290},//NV_BC15_VCO_TUNE_2_I                    
        {4291},//NV_BC15_P1_RISE_FALL_OFF_I              
        {4292},//NV_BC15_HDR_P1_RISE_FALL_OFF_I          
        {4293},//NV_C0_BC15_TX_CAL_CHAN_I                
        {4294},//NV_C0_BC15_RX_CAL_CHAN_I                
        {4295},//NV_C0_BC15_RX_CAL_CHAN_LRU_I            
        {4296},//NV_C0_BC15_LNA_1_OFFSET_I               
        {4297},//NV_C0_BC15_LNA_2_OFFSET_I               
        {4298},//NV_C0_BC15_LNA_3_OFFSET_I               
        {4299},//NV_C0_BC15_LNA_4_OFFSET_I               
        {4300},//NV_C0_BC15_LNA_1_OFFSET_VS_FREQ_I       
        {4301},//NV_C0_BC15_LNA_2_OFFSET_VS_FREQ_I       
        {4302},//NV_C0_BC15_LNA_3_OFFSET_VS_FREQ_I       
        {4303},//NV_C0_BC15_LNA_4_OFFSET_VS_FREQ_I       
        {4304},//NV_C0_BC15_IM2_I_VALUE_I                
        {4305},//NV_C0_BC15_IM2_Q_VALUE_I                
        {4306},//NV_C0_BC15_VGA_GAIN_OFFSET_I            
        {4307},//NV_C0_BC15_VGA_GAIN_OFFSET_VS_FREQ_I    
        {4308},//NV_C0_BC15_IM2_TRANSCONDUCTOR_VALUE_I   
        {4309},//NV_C0_BC15_LNA_1_RISE_I                 
        {4310},//NV_C0_BC15_LNA_1_FALL_I                 
        {4311},//NV_C0_BC15_LNA_2_RISE_I                 
        {4312},//NV_C0_BC15_LNA_2_FALL_I                 
        {4313},//NV_C0_BC15_LNA_3_RISE_I                 
        {4314},//NV_C0_BC15_LNA_3_FALL_I                 
        {4315},//NV_C0_BC15_LNA_4_RISE_I                 
        {4316},//NV_C0_BC15_LNA_4_FALL_I                 
        {4317},//NV_C0_BC15_IM_LEVEL1_I                  
        {4318},//NV_C0_BC15_IM_LEVEL2_I                  
        {4319},//NV_C0_BC15_IM_LEVEL3_I                  
        {4320},//NV_C0_BC15_IM_LEVEL4_I                  
        {4351},//NV_C0_BC15_LNA_SWITCHPOINTS_LPM_1_I     
        {4352},//NV_C0_BC15_LNA_SWITCHPOINTS_LPM_2_I     
        {4353},//NV_C0_BC15_IM2_LPM_1_I                  
        {4354},//NV_C0_BC15_IM2_LPM_2_I                  
        {4355},//NV_C0_BC15_INTELLICEIVER_CAL_I          
        //{4356},//NV_C0_BC15_INTELLICEIVER_DET_THRESH_I   
        {4795},//NV_BC0_TX_PDM_0_I                       
        {4796},//NV_BC0_TX_PDM_1_I                       
        {4799},//NV_BC1_TX_PDM_0_I                       
        {4800},//NV_BC1_TX_PDM_1_I                       
        {4827},//NV_BC15_TX_PDM_0_I                      
        {4828},//NV_BC15_TX_PDM_1_I                      
        {4831},//NV_BC0_TX_LIN_0_I                       
        {4832},//NV_BC0_TX_LIN_1_I                          
        {4835},//NV_BC1_TX_LIN_0_I                       
        {4836},//NV_BC1_TX_LIN_1_I                       
        {4863},//NV_BC15_TX_LIN_0_I                      
        {4864},//NV_BC15_TX_LIN_1_I
        {4880},//NV_BC0_PA_R_MAP
        {4881},//NV_BC1_PA_R_MAP
        {4888},//NV_BC15_PA_R_MAP
        //ruiju add 091120 //F350备份保密参数 NV906 NV466 NV889
        {906},//NV_PPP_PASSWORD_I
        {466},//NV_DS_MIP_SS_USER_PROF_I
        {889},//NV_DS_MIP_DMU_MN_AUTH_I
        {197},//NV_WDC_I
        {NV_MAX_I}//这个值的大小要注意，确保上面的都能覆盖到

    };

    m_ProgressCtrl.SetForeColour(RGB(64,128,255));
    m_ProgressCtrl.SetTextBkColour(RGB(196,0,0));
    ShowRatio(0,100);
    m_nSelectMEID = TRUE;
    for (int i = 0 ; i < MAX_NV_LIST_NUM ; i++)
    {
        if (i % 2 == 0)
        {
            ShowRatio(i / 2);
        }  
        if (rfcallistCur[i].item >= NV_MAX_I)
        {
            break;
        }

        for (int j = 0 ; j < 1 ; j++)
        {
            if ((rfcallistCur[i].item == 906)
             || (rfcallistCur[i].item == 466)
             || (rfcallistCur[i].item == 889))//F350备份保密参数 使用扩展的Diag指令0x92读取
            {
                m_pPacket->NvItemReadPSW(rfcallistCur[i].item,rfcallistCur[i].item_data,rfcallistCur[i].status);
                continue;
            }
            btemp = m_pPacket->NvItemRead(rfcallistCur[i].item,
                                          rfcallistCur[i].item_data,
                                          rfcallistCur[i].status);
            if (btemp)
            {
                //备份成功一项
                strtemp.Format(" Backup nv Item = %d",rfcallistCur[i].item);
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
                break;
                //  ::Sleep(10);
            }
        }//for(j)
        if (!btemp)
        {
            if (rfcallistCur[i].item == NV_MEID_I)
            {
                m_nSelectMEID = FALSE;
                continue;
            }
            strtemp.Format(" Do not backup nv Item = %d",rfcallistCur[i].item);
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
            //::Sleep(1000);
            //return FALSE;
        }
    }
    TRY
    {
        CFile   hFile   (strNVMFile,CFile::modeCreate | CFile::modeWrite);
        hFile.WriteHuge((const void *) rfcallistCur,sizeof(rfcallistCur));
        hFile.Close();
    }
    CATCH(CFileException,e)
    {
        e->ReportError();
        strtemp.Format("Save file %s failed)",e->m_strFileName);
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
        return FALSE;
    }
    END_CATCH   ShowRatio   (0);

    timeEnd = GetTickCount();
    int nSecond = (timeEnd - timeBegin) / 1000;
    int nMinute = nSecond / 60;
    nSecond = nSecond % 60;
    strtemp.Format("Total time is %d min %d sec",nMinute,nSecond) ;
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
    return TRUE;
}

BOOL CSST_F350Dlg::RestoreNV(CString NVFilePathIfo, int nBackupType)
{
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Restore nv item......");
    // CString strNVInfo = _T("");
    CString strNVMFile  = "";
    strNVMFile = NVFilePathIfo;
    BOOL    btemp   = FALSE;
    dword   esn     = 0 ;
    DWORD   timeBegin, timeEnd;
    //启动计时
    CString strtemp ;
    timeBegin = GetTickCount();
    timeEnd = timeBegin;

    //  if(!SendSPC())
    //  {
    //      return FALSE;
    //  }
    nv_item_list_info   rfcallistCur[MAX_NV_LIST_NUM]   =
    {
        {NV_MEID_I},            //      1943            
        {NV_ESN_I},//=0
        {5494},//NV_PPPPASSWORD_CONT2_I chenhongyu
        {5489},//NV_PPPPASSWORD_CONT1_I
        {5551},//NV_WAPGATEPASSWORD_CONT1_I
        {5553},//NV_WAPGATEPASSWORD_CONT2_I
        {5555},//NV_MMSPASSWORD_CONT1_I
        {5558},//NV_MMSPASSWORD_CONT2_I
        {5501},//NV_BREWPASSWORD_CONT1_I
        {5503},//NV_BREWPASSWORD_CONT2_I
        {5452}, //新增C79的NV备份参数,Wap Password
        {5455}, //
        {5386}, //
        {5391}, //
        {4388}, {4371}, {4374}, {85}, // NV 产线不备份SPC
        {NV_MANUFACTORY_INFO_I}, //chenhongyu 20080729
        {447}, //蓝牙地址   
        //GPS
        //NV449,NV555,NV736,NV1624,NV1713,NV4272
        {449}, {555}, {736}, {4272},
        //BC0, BC1 for D90
        {1713}, //NV_BC0_GPS1_RF_DELAY_I //C88 dont backup this item
        {1715}, //NV_BC0_TX_LIM_VS_TEMP_I
        {1716}, //NV_BC0_TX_LIN_MASTER0_I 
        {1717}, //NV_BC0_TX_LIN_MASTER1_I 
        {1718}, //NV_BC0_TX_LIN_MASTER2_I 
        {1719}, //NV_BC0_TX_LIN_MASTER3_I
        {1720}, //NV_BC0_TX_COMP0_I
        {1721}, //NV_BC0_TX_COMP1_I
        {1722}, //NV_BC0_TX_COMP2_I
        {1723}, //NV_BC0_TX_COMP3_I
        {1724}, //NV_BC0_TX_LIM_VS_FREQ_I
        {1725}, //NV_BC0_PA_R1_RISE_I
        {1726}, //NV_BC0_PA_R1_FALL_I
        {1727}, //NV_BC0_PA_R2_RISE_I
        {1728}, //NV_BC0_PA_R2_FALL_I
        {1729}, //NV_BC0_PA_R3_RISE_I
        {1730}, //NV_BC0_PA_R3_FALL_I
        {1731}, //NV_BC0_HDET_OFF_I
        {1732}, //NV_BC0_HDET_SPN_I
        {1733}, //NV_BC0_EXP_HDET_VS_AGC_I
        {1734}, //NV_BC0_ENC_BTF_I //C88 dont backup this item
        {1735}, //NV_BC0_VCO_COARSE_TUNE_TABLE_I
        {1736}, //NV_BC0_P1_RISE_FALL_OFF_I //C88 dont backup this item
        {1879}, //NV_BC0_HDR_IM_FALL_I
        {1880}, //NV_BC0_HDR_IM_RISE_I
        {1890}, //NV_BC0_HDR_IM_LEVEL_I
        {2080}, //NV_BC0_VCO_COARSE_TUNE_2_I
        {2814}, //NV_BC0_HDR_P1_RISE_FALL_OFF_I
        {3035}, //NV_BC0_PA_DVS_VOLTAGE_I
        {3237}, //NV_BC0_VCO_TUNE_2_I
        {3386}, //NV_BC0_HDR_G0_IM_FALL_I
        {3387}, //NV_BC0_HDR_G0_IM_RISE_I
        {3388}, //NV_BC0_HDR_G0_IM_LEVEL_I
        {3420}, //NV_BC0_ANT_QUALITY_I  
        {1737}, //NV_C0_BC0_TX_CAL_CHAN_I //C88 dont backup this item
        {1738}, //NV_C0_BC0_RX_CAL_CHAN_I //C88 dont backup this item
        {1739}, //NV_C0_BC0_LNA_1_OFFSET_VS_FREQ_I
        {1740}, //NV_C0_BC0_LNA_2_OFFSET_VS_FREQ_I
        {1741}, //NV_C0_BC0_LNA_3_OFFSET_VS_FREQ_I
        {1742}, //NV_C0_BC0_LNA_4_OFFSET_VS_FREQ_I
        {1743}, //NV_C0_BC0_LNA_1_OFFSET_I
        {1744}, //NV_C0_BC0_LNA_2_OFFSET_I
        {1745}, //NV_C0_BC0_LNA_3_OFFSET_I
        {1746}, //NV_C0_BC0_LNA_4_OFFSET_I
        {1747}, //NV_C0_BC0_IM2_I_VALUE_I
        {1748}, //NV_C0_BC0_IM2_Q_VALUE_I
        {1749}, //NV_C0_BC0_VGA_GAIN_OFFSET_I
        {1750}, //NV_C0_BC0_VGA_GAIN_OFFSET_VS_FREQ_I
        {1751}, //NV_C0_BC0_IM2_TRANSCONDUCTOR_VALUE_I
        {1752}, //NV_C0_BC0_LNA_1_RISE_I
        {1753}, //NV_C0_BC0_LNA_1_FALL_I
        {1754}, //NV_C0_BC0_LNA_2_RISE_I
        {1755}, //NV_C0_BC0_LNA_2_FALL_I
        {1756}, //NV_C0_BC0_LNA_3_RISE_I
        {1757}, //NV_C0_BC0_LNA_3_FALL_I
        {1758}, //NV_C0_BC0_LNA_4_RISE_I
        {1759}, //NV_C0_BC0_LNA_4_FALL_I
        {1760}, //NV_C0_BC0_IM_LEVEL1_I
        {1761}, //NV_C0_BC0_IM_LEVEL2_I
        {1762}, //NV_C0_BC0_IM_LEVEL3_I
        {1763}, //NV_C0_BC0_IM_LEVEL4_I     
        {1624}, //NV_BC1_GPS1_RF_DELAY_I  //C88 dont backup this item
        //{1625}, //NV_BC1_RF_TUNE_RESERVED_I
        {1626}, //NV_BC1_TX_LIM_VS_TEMP_I
        {1627}, //NV_BC1_TX_LIN_MASTER0_I
        {1628}, //NV_BC1_TX_LIN_MASTER1_I
        {1629}, //NV_BC1_TX_LIN_MASTER2_I
        {1630}, //NV_BC1_TX_LIN_MASTER3_I
        {1631}, //NV_BC1_TX_COMP0_I
        {1632}, //NV_BC1_TX_COMP1_I
        {1633}, //NV_BC1_TX_COMP2_I
        {1634}, //NV_BC1_TX_COMP3_I
        {1635}, //NV_BC1_TX_LIM_VS_FREQ_I
        {1636}, //NV_BC1_PA_R1_RISE_I
        {1637}, //NV_BC1_PA_R1_FALL_I
        {1638}, //NV_BC1_PA_R2_RISE_I
        {1639}, //NV_BC1_PA_R2_FALL_I
        {1640}, //NV_BC1_PA_R3_RISE_I
        {1641}, //NV_BC1_PA_R3_FALL_I
        {1642}, //NV_BC1_HDET_OFF_I
        {1643}, //NV_BC1_HDET_SPN_I
        {1644}, //NV_BC1_EXP_HDET_VS_AGC_I
        {1645}, //NV_BC1_ENC_BTF_I  //C88 dont backup this item
        {1646}, //NV_BC1_VCO_COARSE_TUNE_TABLE_I
        {1647}, //NV_BC1_P1_RISE_FALL_OFF_I  //C88 dont backup this item
        {2081}, //NV_BC1_VCO_COARSE_TUNE_2_I
        {2815}, //NV_BC1_HDR_P1_RISE_FALL_OFF_I
        {3036}, //NV_BC1_PA_DVS_VOLTAGE_I
        {3238}, //NV_BC1_VCO_TUNE_2_I
        {3421}, //NV_BC1_ANT_QUALITY_I  
        //NV_C0_BC1
        {1648}, //NV_C0_BC1_TX_CAL_CHAN_I  //C88 dont backup this item
        {1649}, //NV_C0_BC1_RX_CAL_CHAN_I  //C88 dont backup this item
        {1650}, //NV_C0_BC1_LNA_1_OFFSET_VS_FREQ_I
        {1651}, //NV_C0_BC1_LNA_2_OFFSET_VS_FREQ_I
        {1652}, //NV_C0_BC1_LNA_3_OFFSET_VS_FREQ_I
        {1653}, //NV_C0_BC1_LNA_4_OFFSET_VS_FREQ_I
        {1654}, //NV_C0_BC1_LNA_1_OFFSET_I
        {1655}, //NV_C0_BC1_LNA_2_OFFSET_I
        {1666}, //NV_C0_BC1_LNA_3_OFFSET_I
        {1667}, //NV_C0_BC1_LNA_4_OFFSET_I
        {1668}, //NV_C0_BC1_IM2_I_VALUE_I
        {1669}, //NV_C0_BC1_IM2_Q_VALUE_I
        {1670}, //NV_C0_BC1_VGA_GAIN_OFFSET_I
        {1671}, //NV_C0_BC1_VGA_GAIN_OFFSET_VS_FREQ_I
        {1672}, //NV_C0_BC1_IM2_TRANSCONDUCTOR_VALUE_I
        {1673}, //NV_C0_BC1_LNA_1_RISE_I
        {1674}, //NV_C0_BC1_LNA_1_FALL_I
        {1675}, //NV_C0_BC1_LNA_2_RISE_I
        {1676}, //NV_C0_BC1_LNA_2_FALL_I
        {1677}, //NV_C0_BC1_LNA_3_RISE_I
        {1678}, //NV_C0_BC1_LNA_3_FALL_I
        {1679}, //NV_C0_BC1_LNA_4_RISE_I
        {1680}, //NV_C0_BC1_LNA_4_FALL_I
        {1681}, //NV_C0_BC1_IM_LEVEL1_I
        {1682}, //NV_C0_BC1_IM_LEVEL2_I
        {1683}, //NV_C0_BC1_IM_LEVEL3_I
        {1684}, //NV_C0_BC1_IM_LEVEL4_I
        {2793}, //NV_C0_BC1_RX_CAL_CHAN_LRU_I
        {3254}, //NV_C0_BC1_IM2_LPM_1_I
        {3260}, //NV_C0_BC1_IM2_LPM_2_I
        {3266}, //NV_C0_BC1_INTELLICEIVER_CAL_I
        {3272}, //NV_C0_BC1_LNA_SWITCHPOINTS_LPM_1_I
        {3278}, //NV_C0_BC1_LNA_SWITCHPOINTS_LPM_2_I
        {2815}, //NV_BC1_HDR_P1_RISE_FALL_OFF_I
        {3036}, //NV_BC1_PA_DVS_VOLTAGE_I
        {3238}, //NV_BC1_VCO_TUNE_2_I
        {3421}, //NV_BC1_ANT_QUALITY_I
        {2791}, //NV_C0_BC0_RX_CAL_CHAN_LRU_I
        {3253}, //NV_C0_BC0_IM2_LPM_1_I
        {3259}, //NV_C0_BC0_IM2_LPM_2_I
        {3265}, //NV_C0_BC0_INTELLICEIVER_CAL_I
        {3271}, //NV_C0_BC0_LNA_SWITCHPOINTS_LPM_1_I
        {3277}, //NV_C0_BC0_LNA_SWITCHPOINTS_LPM_2_I
        //{3469}, //NV_C0_BC0_INTELLICEIVER_DET_THRESH_I
        //BC15
        {3265},//NV_C0_BC0_INTELLICEIVER_CAL_I           
        {3266},//NV_C0_BC1_INTELLICEIVER_CAL_I           
        //{3469},//NV_C0_BC0_INTELLICEIVER_DET_THRESH_I    
        //{3470},//NV_C0_BC1_INTELLICEIVER_DET_THRESH_I    
        //BC15
        {4273},//NV_BC15_TX_LIM_VS_TEMP_I                
        {4274},//NV_BC15_TX_COMP0_I                      
        {4275},//NV_BC15_TX_COMP1_I                      
        {4276},//NV_BC15_TX_COMP2_I                      
        {4277},//NV_BC15_TX_COMP3_I                      
        {4278},//NV_BC15_TX_LIM_VS_FREQ_I                
        {4279},//NV_BC15_PA_R1_RISE_I                    
        {4280},//NV_BC15_PA_R1_FALL_I                    
        {4281},//NV_BC15_PA_R2_RISE_I                    
        {4282},//NV_BC15_PA_R2_FALL_I                    
        {4283},//NV_BC15_PA_R3_RISE_I                    
        {4284},//NV_BC15_PA_R3_FALL_I                    
        {4285},//NV_BC15_HDET_OFF_I                      
        {4286},//NV_BC15_HDET_SPN_I                      
        {4287},//NV_BC15_EXP_HDET_VS_AGC_I               
        {4288},//NV_BC15_ENC_BTF_I                       
        {4289},//NV_BC15_VCO_COARSE_TUNE_TABLE_I         
        {4290},//NV_BC15_VCO_TUNE_2_I                    
        {4291},//NV_BC15_P1_RISE_FALL_OFF_I              
        {4292},//NV_BC15_HDR_P1_RISE_FALL_OFF_I          
        {4293},//NV_C0_BC15_TX_CAL_CHAN_I                
        {4294},//NV_C0_BC15_RX_CAL_CHAN_I                
        {4295},//NV_C0_BC15_RX_CAL_CHAN_LRU_I            
        {4296},//NV_C0_BC15_LNA_1_OFFSET_I               
        {4297},//NV_C0_BC15_LNA_2_OFFSET_I               
        {4298},//NV_C0_BC15_LNA_3_OFFSET_I               
        {4299},//NV_C0_BC15_LNA_4_OFFSET_I               
        {4300},//NV_C0_BC15_LNA_1_OFFSET_VS_FREQ_I       
        {4301},//NV_C0_BC15_LNA_2_OFFSET_VS_FREQ_I       
        {4302},//NV_C0_BC15_LNA_3_OFFSET_VS_FREQ_I       
        {4303},//NV_C0_BC15_LNA_4_OFFSET_VS_FREQ_I       
        {4304},//NV_C0_BC15_IM2_I_VALUE_I                
        {4305},//NV_C0_BC15_IM2_Q_VALUE_I                
        {4306},//NV_C0_BC15_VGA_GAIN_OFFSET_I            
        {4307},//NV_C0_BC15_VGA_GAIN_OFFSET_VS_FREQ_I    
        {4308},//NV_C0_BC15_IM2_TRANSCONDUCTOR_VALUE_I   
        {4309},//NV_C0_BC15_LNA_1_RISE_I                 
        {4310},//NV_C0_BC15_LNA_1_FALL_I                 
        {4311},//NV_C0_BC15_LNA_2_RISE_I                 
        {4312},//NV_C0_BC15_LNA_2_FALL_I                 
        {4313},//NV_C0_BC15_LNA_3_RISE_I                 
        {4314},//NV_C0_BC15_LNA_3_FALL_I                 
        {4315},//NV_C0_BC15_LNA_4_RISE_I                 
        {4316},//NV_C0_BC15_LNA_4_FALL_I                 
        {4317},//NV_C0_BC15_IM_LEVEL1_I                  
        {4318},//NV_C0_BC15_IM_LEVEL2_I                  
        {4319},//NV_C0_BC15_IM_LEVEL3_I                  
        {4320},//NV_C0_BC15_IM_LEVEL4_I                  
        {4351},//NV_C0_BC15_LNA_SWITCHPOINTS_LPM_1_I     
        {4352},//NV_C0_BC15_LNA_SWITCHPOINTS_LPM_2_I     
        {4353},//NV_C0_BC15_IM2_LPM_1_I                  
        {4354},//NV_C0_BC15_IM2_LPM_2_I                  
        {4355},//NV_C0_BC15_INTELLICEIVER_CAL_I          
        //{4356},//NV_C0_BC15_INTELLICEIVER_DET_THRESH_I   
        {4795},//NV_BC0_TX_PDM_0_I                       
        {4796},//NV_BC0_TX_PDM_1_I                       
        {4799},//NV_BC1_TX_PDM_0_I                       
        {4800},//NV_BC1_TX_PDM_1_I                       
        {4827},//NV_BC15_TX_PDM_0_I                      
        {4828},//NV_BC15_TX_PDM_1_I                      
        {4831},//NV_BC0_TX_LIN_0_I                       
        {4832},//NV_BC0_TX_LIN_1_I                          
        {4835},//NV_BC1_TX_LIN_0_I                       
        {4836},//NV_BC1_TX_LIN_1_I                       
        {4863},//NV_BC15_TX_LIN_0_I                      
        {4864},//NV_BC15_TX_LIN_1_I
        {4880},//NV_BC0_PA_R_MAP
        {4881},//NV_BC1_PA_R_MAP
        {4888},//NV_BC15_PA_R_MAP
        //ruiju add 091120 //F350备份保密参数 NV906 NV466 NV889
        {906},//NV_PPP_PASSWORD_I
        {466},//NV_DS_MIP_SS_USER_PROF_I
        {889},//NV_DS_MIP_DMU_MN_AUTH_I
        {197},//NV_WDC_I
        {NV_MAX_I}//这个值的大小要注意，确保上面的都能覆盖到

    };
    TRY
    {
        // CFile hFile( strNVInfo, CFile::modeRead | CFile::shareDenyWrite);
        CFile   hFile   (strNVMFile,CFile::modeRead | CFile::shareDenyWrite);
        if (hFile.GetLength() != sizeof(rfcallistCur))
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("The nv file is not match the handset.");
            MessageBox("The nv file is not match the handset.","Warning",MB_ICONWARNING);
            return FALSE;
        }
        hFile.ReadHuge((void *) rfcallistCur,sizeof(rfcallistCur));
        hFile.Close();
    }
    CATCH(CFileException,e)
    {
        e->ReportError();
        CString strError    = _T("");
        strtemp.Format("Save file %s failed)",e->m_strFileName);
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
        return FALSE;
    }
    END_CATCH

    //offline
    int Error = 0;
    do
    {
        ::Sleep(200);
        Error++;
    }
    while ((!m_pPacket->EnterOfflineMode()) && (Error < 10));
    if (Error >= 10)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Into Download mode error. please retry!");
        return FALSE;
    } 
    ::Sleep(2000);

    m_ProgressCtrl.SetForeColour(RGB(128,128,0));
    m_ProgressCtrl.SetTextBkColour(RGB(0,0,255));
    ShowRatio(0,100);

    for (int i = 0 ; i < MAX_NV_LIST_NUM ; i++)
    {
        if (i % 2 == 0)
        {
            ShowRatio(i / 2);
        } 
        if (m_nSelectMEID && rfcallistCur[i].item == NV_ESN_I)
        {
            continue;
        }
        if (!m_nSelectMEID && rfcallistCur[i].item == NV_MEID_I)
        {
            continue;
        }


        if (rfcallistCur[i].item >= NV_MAX_I)
        {
            break;
        }
        if (rfcallistCur[i].status == NV_DONE_S)
        {
            if (!m_pPacket->NvItemWrite(rfcallistCur[i].item,rfcallistCur[i].item_data,rfcallistCur[i].status))
            {
                if (rfcallistCur[i].status == NV_NOTACTIVE_S || rfcallistCur[i].status == NV_READONLY_S)
                {
                    continue;
                }
                else
                {
                    //失败了再试一次，如果还不行就彻底失败
                    if (!m_pPacket->NvItemWrite(rfcallistCur[i].item,
                                                rfcallistCur[i].item_data,
                                                rfcallistCur[i].status))
                    {
                        strtemp.Format(" Don not restore nv Item = %d",rfcallistCur[i].item);
                        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
                        //                      return FALSE;
                    }
                }
            }
        }
        Sleep(10);
        //备份成功一项
        strtemp.Format(" Restore nv Item = %d",rfcallistCur[i].item);
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
    }

    ShowRatio(0);

    timeEnd = GetTickCount();
    int nSecond = (timeEnd - timeBegin) / 1000;
    int nMinute = nSecond / 60;
    nSecond = nSecond % 60;
    strtemp.Format("Total time is %d min %d sec",nMinute,nSecond) ;
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);

    return TRUE;
}

/************************************************************************
* 函数名称： // GetTotalBlock(int total)                                *
* 功能描述： // 根据用户所选择的下在方式，获取Flash上面要写的区域       *
* 输入参数： // total:bin文件的Size                                     *
* 输出参数： // 无                                                      *
* 返 回 值： // ntmp:如果选择Reserve NV返回3M（可以执行该种操作）       *
*               或者-1（不能执行该操作）;如果其它下在方式返回total      *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-16                                              *
* 修改内容： //                                                         *
************************************************************************/
int CSST_F350Dlg::GetTotalBlock(int total)
{
    int ntmp;
    ntmp = m_phone_info.nvoffset;   //  擦除区域变小

    if ((ntmp == 0) || (ntmp > total))
    {
        ntmp = total;
    }   
    return ntmp;
}

BOOL CSST_F350Dlg::DownloadMbn()
{
    //启动计时
    DWORD   timeBegin, timeEnd;
    CString strtemp ;
    timeBegin = GetTickCount();
    timeEnd = timeBegin;
    int Error   = 0;

	if(m_nConnectType == DIAG_CONNECT)
	{

		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Send spc code......");  
		if (!SendSPC())
		{
			return FALSE;
		}
		//备份NV
		if (!BackNVofDL())
		{
			return FALSE;
		}

		int     i       = 0;
		BOOL    btemp   = FALSE;
		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Enter offline mode......");
		for (i = 0 ; i < 3 ; i++)
		{
			btemp = m_pPacket->EnterOfflineMode();
			if (btemp)
			{
				break;
			}
			::Sleep(100);
		}
		if (!btemp)
		{
			GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Enter offline mode Error");
			return FALSE;
		}
		Sleep(2000);

		m_bHaveRefresh = FALSE;
		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Switch download......");    
		for (i = 0 ; i < 3 ; i++)
		{
			btemp = m_pPacket->SwitchDownloader();
			if (btemp)
			{
				break;
			}
			::Sleep(500);//chenjie 20080229//(100)
		}
		if (!btemp)
		{
			GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Switch download Error");
			return FALSE;
		}

		Sleep(2000);
		Error = 0;
		while (!m_bHaveRefresh && Error < 10)
		{
			Sleep(1000);
			Error++;
		}
		if (Error >= 10)
		{
			if (!m_bHaveRefresh)
			{
				MessageBox("Switch download mode failed,Please reset the handset and download again!");
				return FALSE;
			}
		}
	}

    //下载版本
    if (!DLmodeDownload())
    {
        return FALSE;
    }

    SetDlgItemText(IDC_EDIT_SPCCODE,"000000");
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Send spc code......");  
    if (!SendSPC())
    {
        return FALSE;
    }
    //还原NV
    RestoreNVofDL();

    ShowRatio(0) ;
    timeEnd = GetTickCount();
    int nSecond = (timeEnd - timeBegin) / 1000;
    int nMinute = nSecond / 60;
    nSecond = nSecond % 60;
    strtemp.Format("Total time is %d min %d sec",nMinute,nSecond) ;
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);

    return TRUE;
}

BOOL CSST_F350Dlg::DLmodeDownloadMbn()
{
    if (IDYES != MessageBox("This may lose some NV data. Will you Continue?",
                            "Info",
                            MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2))
    {
        return FALSE;
    }

    //下载
    BOOL    bDLmode;
    bDLmode = DLmodeDownload();

    if (!bDLmode)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CSST_F350Dlg::SendSPC()
{
    GetDlgItemText(IDC_EDIT_SPCCODE,m_strSpcCode);
    if (m_strSpcCode.GetLength() != 6)
    {
        MessageBox("SPC length is 6!","Warning",MB_ICONWARNING);        
        return FALSE;
    }
    if (!m_pPacket->SendSpc(m_strSpcCode))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("SPC send error, please retry!");
        MessageBox("SPC send error, please retry!","Error",MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}

BOOL CSST_F350Dlg::SpecialVersionTest()
{
    int length;

    length = m_strPhoneVersion.ReverseFind('V');
    //length = m_strPhoneVersion.Find('V');

    if (length == -1)
    {
        return FALSE;
    }

    m_strSoftWareVersion = m_strPhoneVersion.Left(length);

    return TRUE;
}

BOOL CSST_F350Dlg::RefreshPort()
{
    CString strPort;
    CString strLastPort;
    int     nPos    = 0;

    if (pStateThread)
    {
        pStateThread->SuspendThread();
     }
    m_combCom.ResetContent();
    for (int i = 1; i <= 256; i++)
    {
        if (m_pPacket->IsOpened())
        {
            m_pPacket->Close();
        }
        if ((m_pPacket->Open(i,m_nBoundRate)))
        {
            strPort.Format(_T("COM%d"),i);
            m_combCom.AddString(strPort);
            m_pPacket->Close();
            if (i == m_ComPort1)
            {
                strLastPort = strPort;
            }
            if (strLastPort.GetLength() < 1)
            {
                nPos++;
            }
        }
    }

    if (strLastPort.GetLength() == 0)
    {
        m_combCom.SetCurSel(0);
        OnSelchangeComboCom();
    }
    else
    {
        m_combCom.SetCurSel(nPos);
    }


    if (pStateThread)
    {
        pStateThread->ResumeThread();
     }
    return TRUE;
}

void CSST_F350Dlg::OnButtonRefresh()
{
    // TODO: Add your control notification handler code here
    RefreshPort();
}


BOOL CSST_F350Dlg::GetSoftWareVer(CString bufdef)
{
    CFile   hReadFile ;
    int     ReadLen = 0;
    int     nTotalLen;
    char   *writeBuf;
    int     len     = bufdef.GetLength();

    CString strSoftwareFile;
    strSoftwareFile.Format("%s\\amss.mbn",m_PageDL.m_UserFilePath);
    if (!hReadFile.Open(strSoftwareFile,CFile::modeRead | CFile::shareDenyWrite))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Can not find the file!");
        return FALSE;
    }
    nTotalLen = hReadFile.GetLength();
    writeBuf = new char[nTotalLen]; 
    hReadFile.ReadHuge(writeBuf,nTotalLen);

    for (int i = 0; i < nTotalLen; i++)
    {
        for (int j = 0; j < len; j++)
        {
            if (writeBuf[i + j] != bufdef.GetAt(j))
            {
                break;
            }
        }
        if (j == len)
        {
            for (; j < m_strPhoneVersion.GetLength(); j++)
            {
                m_strSoftWareVersion += writeBuf[i + j];
            }

            return true;
        }
    }

    return false;
}
BOOL CSST_F350Dlg::DLmodeDownload()
{
    m_strFlashPath = m_PageDL.m_FlashfilePath + "\\nandprg.bin";    
    m_PageDL.GetDlgItemText(IDC_EDIT_USERFILE_PATH,m_PageDL.m_UserFilePath);
    if (m_PageDL.m_UserFilePath == "")
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Please select the phone image file to be downloaded.");
        MessageBox("Please select the phone image file to be downloaded.","Warning",MB_ICONWARNING);
        return FALSE;
    }

    int Error   = 0;
	CString DLtimes = "";

	if(m_nConnectType == DIAG_CONNECT)
	{
		::Sleep(500);//chenjie 20080229//(100)
		//增加读取参数命令
		Error = 0;
		dload_params_rsp_type   params;
		do
		{
			Sleep(100);
			InitPort(m_ComPort1,m_nBoundRate);
			m_pPacket->ParamRequest(params);
			Error++;
		}
		while (!m_pPacket->DoNoop() && Error < 10);
		//下载aprg
		Error = 0;
	}

	if(m_nConnectType != SDLD_CONNECT)
	{
		
		while ((!(DLBootFile1())) && (Error < 10))
		{
			::Sleep(1000);
			DLtimes.Format("%d",Error);
			if (Error > 0)
			{
				GetDlgItem(IDC_STATIC_INFO)->SetWindowText("It will retry download bootfile 10 times. This is No. "
					+ DLtimes
					+ " ......");
			}
			Error++;
		}
		if (Error >= 10)
		{
			GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download boot file fail!");
			return FALSE;
		}
		Sleep(500);
		Error = 0;
		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Enter to stream mode!");
		while ((!(m_pPacket->Go(0X80,0X00))) && (Error < 5))
		{
			::Sleep(1000);
			DLtimes.Format("%d",Error);
			if (Error > 0)
			{
				GetDlgItem(IDC_STATIC_INFO)->SetWindowText("It will retry go command......");
			}
			Error++;
		}
		if (Error >= 5)
		{
			GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Go command error. Please retry!");
			return FALSE;
		}
		Sleep(5000);
	}

    if (!SDLModeSoftWare())
    {
        return FALSE;
    }

    Error = 0;
    SetDlgItemText(IDC_STATIC_COM,_T(""));
    while (!ConnectPhone() && Error < 50)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Serching handset......");       
        Error++;
        ::Sleep(500);
    }
    if (Error >= 50)
    {
        return FALSE;
    }

    return TRUE;
}


BOOL CSST_F350Dlg::SearchComPort(BOOL bSuspend)
{
    int     index   = 0;
    CString strPort;

    if (pStateThread && bSuspend)
    {
        pStateThread->SuspendThread();
    }
    m_combCom.ResetContent();
    for (int i = 1; i <= 256; i++)
    {
        if (m_pPacket->IsOpened())
        {
            m_pPacket->Close();
        }
        if ((m_pPacket->Open(i,m_nBoundRate)))
        {
            strPort.Format(_T("COM%d"),i);

            m_combCom.AddString(strPort);
            if (i == m_ComPort1)
            {
                index = m_combCom.GetCount() - 1;
            }
            m_pPacket->Close();
        }
    }

    m_combCom.SetCurSel(index);
    if (pStateThread && bSuspend)
    {
        pStateThread->ResumeThread();
    }
    return TRUE;
}

BOOL CSST_F350Dlg::RepairMbn()
{
    if (!RepairModeSoftware())
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CSST_F350Dlg::IsDLMatch(BOOL &IsResv8K)
{
    nv_item_type    item_data;
    word            status  = 0;
    CFile           hReadFile;
    memset(&item_data,0,sizeof(nv_item_type));
    memset(&m_phone_info,0,sizeof(phone_interface_info));
    memset(&bin_info,0,sizeof(phone_interface_info));

    IsResv8K = FALSE;

    if (m_pPacket->NvItemRead(NV_MANUFACTORY_INFO_I,item_data,status))
    {
        m_phone_info.bootInfo = item_data.manufactory_info.bootInfo;
        m_phone_info.last8kInfo = item_data.manufactory_info.last8kInfo;
        m_phone_info.netlockInfo = item_data.manufactory_info.netlockInfo;
        memcpy(&m_phone_info.nvoffset,&item_data.manufactory_info.nvoffset[0],4);
        m_phone_info.phonetypeInfo = item_data.manufactory_info.phonetypeInfo;
        m_phone_info.operatorInfo = item_data.manufactory_info.operatorInfo;
        m_phone_info.bandclass = item_data.manufactory_info.bandclass;
        m_phone_info.flatInfo = item_data.manufactory_info.flatInfo;
    }
    else
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Read the 22 nv item failed!");
        //MessageBox("Read number 22 NV item failed!","Information",MB_ICONINFORMATION);
        return FALSE;
    }

    return TRUE ;
}

BOOL CSST_F350Dlg::BackNVofDL()
{
    if (m_PageDL.m_NVfilePath == "")
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Please select the direction of the nv_backup file.");
        MessageBox("Please select the direction of the nv_backup file.","Information",MB_ICONINFORMATION);
        return FALSE;
    }

    int Error   = 0;

    do
    {
        Error++;
    }
    while ((!BackupNV(m_PageDL.m_NVfilePath)) && (Error < 6));
    if (Error >= 6)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Failed");
        return FALSE;
    }
    ::Sleep(50);
    return TRUE;
}

BOOL CSST_F350Dlg::RestoreNVofDL()
{
    int     Error       = 0;    

    //还原NV
    CString strNVMFile0 = "";
    strNVMFile0 = m_strBackupNvPath;//m_PageDL.m_NVfilePath + "\\" + "NVTemp.nvm";
    Error = 0;
    do
    {
        ::Sleep(1000);
        Error++;
    }
    while ((!RestoreNV(strNVMFile0)) && (Error < 6));
    if (Error < 6)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Reset handset......");
        m_pPacket->ResetDMP();
        Sleep(2000);
        m_nConnectType = DLAD_CONNECT;//此处实现从新读取ESN
        int Error   = 0;        //search handset
        do
        {
            ::Sleep(500);
            Error++;
            if (Error > 1)
            {
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Searching handset.....");
            }
        }
        while ((!ConnectPhone()) && (Error < 40));
        if (Error >= 40)
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Fail to find the handset!");
        }
        ::Sleep(500);

        ConnectPhone();
        ::Sleep(500);
    }
    else
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Download the image file complete! Restore nv failed.");
        MessageBox("Download the image file complete! Restore nv failed.","Information",MB_ICONINFORMATION);
    }
    return TRUE;
}

BOOL CSST_F350Dlg::RestoreEriFile()
{
    //  CString strFilePath;
    //  SetDlgItemText(IDC_STATIC_INFO,"Update ERI file, please wait ......");
    //  m_PageNV.GetDlgItemText(IDC_STATIC_NVFILE,strFilePath); 
    //  m_ProgressCtrl.SetRange(0,3);   //进度条
    // 
    //  if(!SendSPC())
    //  {
    //      return FALSE;
    //  }
    //  m_ProgressCtrl.SetPos(0);
    //  m_pPacket->EnterOfflineMode();
    //  ::Sleep(1000) ;
    //  
    //  BOOL btemp = FALSE;
    //  CString WritePath = _T("");//手机里的wap存放位置 !! 
    //  diag_fs_filename_type fname;
    //  char PathNameContacts[256];//存放本地路径
    //  CString HS_Path=STR_VERIZON_ERI_FILE; 
    //  CString Delete_path = STR_VERIZON_ERI_FILE;
    //  
    //  //取得路径
    //  strcpy(PathNameContacts,strFilePath);
    //  m_ProgressCtrl.SetPos(1);
    //  //删除ERI文件
    //  strcpy(fname.name,HS_Path.GetBuffer(1));
    //  fname.len = HS_Path.GetLength() +1;
    //  m_pPacket->RemoveFile(fname);
    //  Sleep(500);
    //  
    //  for(int j = 0 ; j < 3 ; j++)
    //  {
    //      btemp = WriteFileToPhone(PathNameContacts, HS_Path);
    //      if(btemp)
    //          break;
    //      else
    //          ::Sleep(200);
    //  }
    //  if(!btemp)
    //  {       
    //      return false;
    //  }
    //  Sleep(1000);
    //  m_ProgressCtrl.SetPos(2);
    //  m_pPacket->ResetDMP();
    //  Sleep(3000);
    //  m_ProgressCtrl.SetPos(3);
    //  
    return TRUE;
}

BOOL CSST_F350Dlg::RestoreDMUkeyFile()
{
    //  CString strFilePath;
    //  SetDlgItemText(IDC_STATIC_INFO,"Update DMU Key file, please wait ......");
    //  strFilePath = m_PageDmu.m_strDMUPath; 
    //  
    //  m_ProgressCtrl.SetRange(0,3);   //进度条
    //  if(!SendSPC())
    //  {
    //      return FALSE;
    //  }
    //  m_ProgressCtrl.SetPos(0);
    //  m_pPacket->EnterOfflineMode();
    //  ::Sleep(1000) ;
    //  
    //  BOOL btemp = FALSE;
    //  CString WritePath = _T("");//   
    //  diag_fs_filename_type fname;
    //  char PathNameContacts[256];//存放本地路径
    //  CString HS_Path=STR_VERIZON_DMU_KEY_FILE; 
    //  CString Delete_path = STR_VERIZON_DMU_KEY_FILE;
    //  
    //  //取得路径
    //  strcpy(PathNameContacts,strFilePath);
    //  m_ProgressCtrl.SetPos(1);
    //  //删除ERI文件
    //  strcpy(fname.name,HS_Path.GetBuffer(1));
    //  fname.len = HS_Path.GetLength() + 1;
    //  m_pPacket->RemoveFile(fname);
    //  Sleep(500);
    //  
    //  for(int j = 0 ; j < 3 ; j++)
    //  {
    //      btemp = WriteFileToPhone(PathNameContacts, HS_Path);
    //      if(btemp)
    //          break;
    //      else
    //          ::Sleep(200);
    //  }
    //  if(!btemp)
    //  {       
    //      return false;
    //  }
    //  Sleep(1000);
    //  m_ProgressCtrl.SetPos(2);
    //  m_pPacket->ResetDMP();
    //  Sleep(3000);
    //  m_ProgressCtrl.SetPos(3);

    return TRUE;
}

BOOL CSST_F350Dlg::WriteFileToPhone(CString PathName, CString strSource)
{
    CFile   hReadFile;
    BOOL    bMore           = FALSE;
    int     nBlockLen = 0, nTotalLen = 0, seqnum = 0;
    byte    fs_status       = 0;
    CString strErr          = _T("");
    int     npos            = 0;

    CString strSourcePath   = strSource;
    CString strTargetPath   = PathName;

    if (!hReadFile.Open(strTargetPath,CFile::modeRead | CFile::shareDenyWrite))
    {
        strErr.Format("Can not Open the File %s, please Check If the file path exists!",strTargetPath);
        SetDlgItemText(IDC_STATIC_INFO,strErr);
        return FALSE;
    }

    nTotalLen = hReadFile.GetLength(); 
    if (nTotalLen > DIAG_FS_MAX_FILE_BLOCK_SIZE)
    {
        nBlockLen = DIAG_FS_MAX_FILE_BLOCK_SIZE;
        bMore = TRUE;
    }
    else
    {
        nBlockLen = nTotalLen;
        bMore = FALSE;
    }
    diag_fs_write_begin_req_type    begin;
    diag_fs_write_rsp_type          fs_write;

    begin.mode = 1;
    begin.total_length = nTotalLen;
    begin.attrib.attribute_mask = 0x00ff;
    begin.attrib.buffering_option = 1;
    begin.attrib.cleanup_option = 0;
    begin.name_info.len = strSourcePath.GetLength() + 1;
    strncpy(begin.name_info.name,(LPCSTR) strSourcePath,begin.name_info.len);
    begin.blockdata.len = nBlockLen;
    hReadFile.Read(begin.blockdata.data,nBlockLen);

    if (!m_pPacket->WriteFileFirstBlock(bMore,begin,fs_write,fs_status))
    {
        hReadFile.Close();
        return FALSE;
    }

    byte    inputseqnum = 0;    // for file too big and overrun

    while (bMore)
    {
        if (inputseqnum == 0xff)
        {
            inputseqnum = 1;
        }
        else
        {
            inputseqnum += 1;
        }

        seqnum += 1;
        nBlockLen = nTotalLen - seqnum * DIAG_FS_MAX_FILE_BLOCK_SIZE;
        if (nBlockLen > DIAG_FS_MAX_FILE_BLOCK_SIZE)
        {
            nBlockLen = DIAG_FS_MAX_FILE_BLOCK_SIZE;
            bMore = TRUE;
        }
        else
        {
            bMore = FALSE;
        }

        diag_fs_data_block_type blockdata;
        blockdata.len = nBlockLen;
        hReadFile.Read(blockdata.data,nBlockLen);
        if (!m_pPacket->WriteFileSequenceBlock(inputseqnum,bMore,blockdata,fs_write,fs_status))
        {
            hReadFile.Close();
            return FALSE;
        }
    }

    hReadFile.Close();
    return TRUE;
}

BOOL CSST_F350Dlg::RestorePrlFile()
{
    //  SetDlgItemText(IDC_STATIC_INFO,"Update PRL file, please wait ......");
    // 
    //  if(!SendSPC())
    //  {
    //      return FALSE;
    //  }
    //  m_ProgressCtrl.SetRange(0,3);   //进度条
    //  m_ProgressCtrl.SetPos(1);
    // 
    //  m_pPacket->EnterOfflineMode();
    //  ::Sleep(1000) ;
    //  m_ProgressCtrl.SetPos(2);
    //  if(!m_pPacket->PRLwrite(m_PageNV.m_NVfilePath,m_PageNV.m_nNam))
    //      return FALSE;
    //  
    //  m_pPacket->ResetDMP();
    //  Sleep(3000);
    //  m_ProgressCtrl.SetPos(3);
    return TRUE;
}

BOOL CSST_F350Dlg::RefurbishmentToDefault()
{
    diag_fs_filename_type   fname;
    BOOL                    m_End   = TRUE;

    if (!SendSPC())
    {
        return FALSE;
    }

    m_pPacket->EnterOfflineMode();
    ::Sleep(1000);

    //清除短信
    CString strfilePhone;
    char   *filephone[] =
    {
        "conf", "draftbox", "inbox", "outbox", "sentbox", "templatebox", "tmp", "tmpmediafile", "userbox"
    };
    for (int i = 0; i < sizeof(filephone) / sizeof(char*); i++)
    {
        strfilePhone.Format("brew/mod/umbapp/umb/%s/",filephone[i]);
        DelDirectoryinPhone(strfilePhone);  //目录最后加/
    }

    ShowRatio(10);
    //清除电话本
    //删除电话本修改记录文件
    strfilePhone = _T("pbm_delete_file.dat");
    strcpy(fname.name,strfilePhone.GetBuffer(1));   
    fname.len = strfilePhone.GetLength() + 1;
    m_pPacket->RemoveFile(fname);
    //删除电话本文件
    strfilePhone = _T("pbm_master_v_20.dat"); 
    strcpy(fname.name,strfilePhone.GetBuffer(1));
    fname.len = strfilePhone.GetLength() + 1;
    m_pPacket->RemoveFile(fname);
    //
    ShowRatio(20);
    strfilePhone = _T("brew/mod/contactapp/groupinfo.dat"); 
    strcpy(fname.name,strfilePhone.GetBuffer(1));
    fname.len = strfilePhone.GetLength() + 1;
    m_pPacket->RemoveFile(fname);
    //清除通话记录
    strfilePhone = _T("brew/shared/CHTimerFile.txt"); 
    strcpy(fname.name,strfilePhone.GetBuffer(1));
    fname.len = strfilePhone.GetLength() + 1;
    m_pPacket->RemoveFile(fname);

    strfilePhone = _T("brew/shared/Callhist"); 
    strcpy(fname.name,strfilePhone.GetBuffer(1));
    fname.len = strfilePhone.GetLength() + 1;
    m_pPacket->RemoveFile(fname);

    ShowRatio(30);
    CString strfilePC   = strCurrentDirectory;
    strfilePC = strCurrentDirectory + "\\Callhist";//取得路径
    CFile   file    (strfilePC,CFile::modeCreate);
    file.Close();
    CString WritePath   = _T("brew/shared/Callhist");//手机里的存放位置 !!  
    char    PathNameContacts[256];//存放本地路径
    CString HS_Path     = _T("Callhist");
    strcpy(PathNameContacts,strCurrentDirectory);
    strcat(PathNameContacts,"\\Callhist");
    WriteFileToPhone(PathNameContacts,WritePath);
    //清除闹钟
    strfilePhone.Format("brew/mod/aclkapp/");
    DelDirectoryinPhone(strfilePhone);  //目录最后加/
    //清除相册
    strfilePhone.Format("brew/root/mypicture/");
    DelDirectoryinPhone(strfilePhone);  //目录最后加/
    //清除语音记录
    strfilePhone.Format("brew/root/mysound/");
    DelDirectoryinPhone(strfilePhone);  //目录最后加/
    //清除My Tunes
    strfilePhone.Format("brew/root/myringtone/");
    DelDirectoryinPhone(strfilePhone);  //目录最后加/
    //恢复参数
    ShowRatio(40);
    RestoreHidemenu();
    Sleep(100);
    RestoreBrewFile();
    Sleep(100);
    ShowRatio(50);
    RestoreWap();
    Sleep(100);
    RestoreMms();
    Sleep(100);
    ShowRatio(60);
    RestoreBrewFile();
    Sleep(100);
    RestoreServiceNumbersFile();
    Sleep(100);
    ShowRatio(70);
    RestoreSndSetFile();
    Sleep(100);
    RestoreDMFile();
    Sleep(100);
    ShowRatio(80);
    RestoreCameraFile();
    Sleep(100);
    RestoreCallFile();
    Sleep(100);
    ShowRatio(90);
    RestoreLightFile();
    //reset mobile
    Sleep(500);
    m_pPacket->ResetDMP();
    SetDlgItemText(IDC_STATIC_INFO,"Refurbishment success!");   

    ShowRatio(100);
    return TRUE;
}


BOOL CSST_F350Dlg::DelDirectoryinPhone(CString strPhone)
{
    BOOL                        m_bEnd  = TRUE;
    CString                     strPc;
    CString                     strPhoneFile;
    CString                     strPcFile;
    int                         nFileNums;
    int                         nStep   = 1;
    diag_fs_disp_files_rsp_type fs_disp_files;
    diag_fs_filename_type       filename;
    efs2_diag_readdir_rsp_type  rspData;    
    char                        buf[MAX_PATH];
    CStringArray                fileArray;

    //读取文件数量
    strcpy(filename.name,strPhone.GetBuffer(1));
    filename.len = strPhone.GetLength() + 1;    
    if (!m_pPacket->DisplayFileList(filename,fs_disp_files))
    {
        return TRUE;
    }
    nFileNums = fs_disp_files.num_files;
    if (nFileNums == 0)
    {
        return TRUE;
    }

    //读取文件
    if (!m_pPacket->EFS2OpenDir(strPhone))
    {
        return FALSE;
    }
    while (m_pPacket->EFS2ReadDir(buf,nStep,rspData))
    {
        strPhoneFile.Format("%s%s",strPhone,buf);
        fileArray.Add(strPhoneFile);

        m_pPacket->EFS2CloseDir();  

        if (!m_pPacket->EFS2OpenDir(strPhone))
        {
            return FALSE;
        }
        nStep++;
    }
    m_pPacket->EFS2CloseDir();

    for (int i = 0; i < fileArray.GetSize(); i++)
    {
        strPhone = fileArray.GetAt(i);
        strcpy(filename.name,strPhone.GetBuffer(1));
        filename.len = strPhone.GetLength() + 1;    
        BOOL    delFlag = m_pPacket->RemoveFile(filename);
    }

    return TRUE;
}


BOOL CSST_F350Dlg::RestoreWap()
{
    BOOL    btemp       = FALSE;
    CString WritePath   = _T("brew/ini/");//手机里的wap存放位置 !!  

    char    PathNameContacts[256];//存放本地路径
    CString HS_Path     = "browser_oemdata.ini"; 

    //取得路径
    strcpy(PathNameContacts,strCurrentDirectory);
    strcat(PathNameContacts,"\\");

    for (int j = 0 ; j < 3 ; j++)
    {
        btemp = _WriteFileToPhone(PathNameContacts,HS_Path,WritePath);
        if (btemp)
        {
            break;
        }
        else
        {
            ::Sleep(200);
        }
    }

    return true;
}
BOOL CSST_F350Dlg::RestoreMms()
{
    BOOL    btemp       = FALSE;
    CString WritePath   = _T("brew/mod/umbapp/umb/conf/");//手机里的mms存放位置 !!  

    char    PathNameContacts[256];//存放本地路径
    CString HS_Path     = _T("mms.ini");                         

    //取得路径
    strcpy(PathNameContacts,strCurrentDirectory);
    strcat(PathNameContacts,"\\");

    for (int j = 0 ; j < 3 ; j++)
    {
        btemp = _WriteFileToPhone(PathNameContacts,HS_Path,WritePath);
        if (btemp)
        {
            break;
        }
        else
        {
            ::Sleep(200);
        }
    }

    return true;
}

BOOL CSST_F350Dlg::RestoreBrewFile()
{
    BOOL    btemp       = FALSE;
    CString WritePath   = _T("brew/ini/");//手机里的mms存放位置 !!  

    char    PathNameContacts[256];//存放本地路径
    CString HS_Path     = _T("brewset_app.ini");
    //取得路径
    strcpy(PathNameContacts,strCurrentDirectory);
    strcat(PathNameContacts,"\\");

    for (int j = 0 ; j < 3 ; j++)
    {
        btemp = _WriteFileToPhone(PathNameContacts,HS_Path,WritePath);
        if (btemp)
        {
            break;
        }
        else
        {
            ::Sleep(200);
        }
    }
    if (!btemp)
    {
        return false;
    }

    return true;
}

BOOL CSST_F350Dlg::RestoreSndSetFile()
{
    BOOL    btemp       = FALSE;
    CString WritePath   = _T("brew/ini/");//手机里的mms存放位置 !!  

    char    PathNameContacts[256];//存放本地路径
    CString HS_Path     = _T("sndset_app.ini");
    //取得路径
    strcpy(PathNameContacts,strCurrentDirectory);
    strcat(PathNameContacts,"\\");

    for (int j = 0 ; j < 3 ; j++)
    {
        btemp = _WriteFileToPhone(PathNameContacts,HS_Path,WritePath);
        if (btemp)
        {
            break;
        }
        else
        {
            ::Sleep(200);
        }
    }
    if (!btemp)
    {
        return false;
    }

    return true;
}

BOOL CSST_F350Dlg::RestoreServiceNumbersFile()
{
    BOOL    btemp       = FALSE;
    CString WritePath   = _T("brew/mod/contactapp/");//手机里的mms存放位置 !!   

    char    PathNameContacts[256];//存放本地路径
    CString HS_Path     = _T("servicenumber.dat");
    //取得路径
    strcpy(PathNameContacts,strCurrentDirectory);
    strcat(PathNameContacts,"\\");

    for (int j = 0 ; j < 3 ; j++)
    {
        btemp = _WriteFileToPhone(PathNameContacts,HS_Path,WritePath);
        if (btemp)
        {
            break;
        }
        else
        {
            ::Sleep(200);
        }
    }
    if (!btemp)
    {
        return false;
    }

    return true;
}

BOOL CSST_F350Dlg::RestoreDMFile()
{
    BOOL    btemp       = FALSE;
    CString WritePath   = _T("brew/ini/");//手机里的mms存放位置 !!  

    char    PathNameContacts[256];//存放本地路径
    CString HS_Path     = _T("dmapp.ini");
    //取得路径
    strcpy(PathNameContacts,strCurrentDirectory);
    strcat(PathNameContacts,"\\");

    for (int j = 0 ; j < 3 ; j++)
    {
        btemp = _WriteFileToPhone(PathNameContacts,HS_Path,WritePath);
        if (btemp)
        {
            break;
        }
        else
        {
            ::Sleep(200);
        }
    }
    if (!btemp)
    {
        return false;
    }

    return true;
}

BOOL CSST_F350Dlg::RestoreCameraFile()
{
    BOOL    btemp       = FALSE;
    CString WritePath   = _T("brew/ini/");//手机里的mms存放位置 !!  

    char    PathNameContacts[256];//存放本地路径
    CString HS_Path     = _T("camera_app.ini");
    //取得路径
    strcpy(PathNameContacts,strCurrentDirectory);
    strcat(PathNameContacts,"\\");

    for (int j = 0 ; j < 3 ; j++)
    {
        btemp = _WriteFileToPhone(PathNameContacts,HS_Path,WritePath);
        if (btemp)
        {
            break;
        }
        else
        {
            ::Sleep(200);
        }
    }
    if (!btemp)
    {
        return false;
    }

    return true;
}

BOOL CSST_F350Dlg::RestoreCallFile()
{
    BOOL    btemp       = FALSE;
    CString WritePath   = _T("brew/ini/");//手机里的mms存放位置 !!  

    char    PathNameContacts[256];//存放本地路径
    CString HS_Path     = _T("callset_app.ini");
    //取得路径
    strcpy(PathNameContacts,strCurrentDirectory);
    strcat(PathNameContacts,"\\");

    for (int j = 0 ; j < 3 ; j++)
    {
        btemp = _WriteFileToPhone(PathNameContacts,HS_Path,WritePath);
        if (btemp)
        {
            break;
        }
        else
        {
            ::Sleep(200);
        }
    }
    if (!btemp)
    {
        return false;
    }

    return true;
}

BOOL CSST_F350Dlg::RestoreLightFile()
{
    BOOL    btemp       = FALSE;
    CString WritePath   = _T("brew/ini/");//手机里的mms存放位置 !!  

    char    PathNameContacts[256];//存放本地路径
    CString HS_Path     = _T("dispset_app.ini");
    //取得路径
    strcpy(PathNameContacts,strCurrentDirectory);
    strcat(PathNameContacts,"\\");

    for (int j = 0 ; j < 3 ; j++)
    {
        btemp = _WriteFileToPhone(PathNameContacts,HS_Path,WritePath);
        if (btemp)
        {
            break;
        }
        else
        {
            ::Sleep(200);
        }
    }
    if (!btemp)
    {
        return false;
    }

    return true;
}

BOOL CSST_F350Dlg::RestoreHidemenu()
{
    BOOL    btemp       = FALSE;
    CString WritePath   = _T("brew/ini/");//手机里的mms存放位置 !!  

    char    PathNameContacts[256];//存放本地路径
    CString HS_Path     = _T("hidmenuapp.ini");
    //取得路径
    strcpy(PathNameContacts,strCurrentDirectory);
    strcat(PathNameContacts,"\\");

    for (int j = 0 ; j < 3 ; j++)
    {
        btemp = _WriteFileToPhone(PathNameContacts,HS_Path,WritePath);
        if (btemp)
        {
            break;
        }
        else
        {
            ::Sleep(200);
        }
    }
    if (!btemp)
    {
        return false;
    }

    return true;
}


BOOL CSST_F350Dlg::_WriteFileToPhone(CString PathName, CString strSource, CString SourcePathName)
{
    CFile   hReadFile;
    BOOL    bMore           = FALSE;
    int     nBlockLen = 0, nTotalLen = 0, seqnum = 0;
    byte    fs_status       = 0;
    CString strErr          = _T("");
    int     npos            = 0;

    CString strSourcePath   = SourcePathName + strSource;
    CString strTargetPath   = PathName + strSource;

    if (!hReadFile.Open(strTargetPath,CFile::modeRead | CFile::shareDenyWrite))
    {
        strErr.Format("Can not open the file %s, please check If the file path exists!",strTargetPath);
        SetDlgItemText(IDC_STATIC_INFO,strErr);
        return FALSE;
    }

    nTotalLen = hReadFile.GetLength(); 
    if (nTotalLen > DIAG_FS_MAX_FILE_BLOCK_SIZE)
    {
        nBlockLen = DIAG_FS_MAX_FILE_BLOCK_SIZE;
        bMore = TRUE;
    }
    else
    {
        nBlockLen = nTotalLen;
        bMore = FALSE;
    }
    diag_fs_write_begin_req_type    begin;
    diag_fs_write_rsp_type          fs_write;


    begin.mode = 1;
    begin.total_length = nTotalLen;
    begin.attrib.attribute_mask = 0x00ff;
    begin.attrib.buffering_option = 1;
    begin.attrib.cleanup_option = 0;
    begin.name_info.len = strSourcePath.GetLength() + 1;
    strncpy(begin.name_info.name,(LPCSTR) strSourcePath,begin.name_info.len);
    begin.blockdata.len = nBlockLen;
    hReadFile.Read(begin.blockdata.data,nBlockLen);

    if (!m_pPacket->WriteFileFirstBlock(bMore,begin,fs_write,fs_status))
    {
        hReadFile.Close();

        //      strErr.Format("Error Code (%d),%s",fs_status,m_Packet[Index].EFSOperError(fs_status));
        //      AfxMessageBox(strErr);
        return FALSE;
    }

    byte    inputseqnum = 0;    // for file too big and overrun

    while (bMore)
    {
        if (inputseqnum == 0xff)
        {
            inputseqnum = 1;
        }
        else
        {
            inputseqnum += 1;
        }

        seqnum += 1;
        nBlockLen = nTotalLen - seqnum * DIAG_FS_MAX_FILE_BLOCK_SIZE;
        if (nBlockLen > DIAG_FS_MAX_FILE_BLOCK_SIZE)
        {
            nBlockLen = DIAG_FS_MAX_FILE_BLOCK_SIZE;
            bMore = TRUE;
        }
        else
        {
            bMore = FALSE;
        }

        diag_fs_data_block_type blockdata;
        blockdata.len = nBlockLen;
        hReadFile.Read(blockdata.data,nBlockLen);
        if (!m_pPacket->WriteFileSequenceBlock(inputseqnum,bMore,blockdata,fs_write,fs_status))
        {
            hReadFile.Close();
            return FALSE;
        }
    }

    hReadFile.Close();
    return TRUE;
}


BOOL CSST_F350Dlg::RestoreDefaultNV(CString strNVFilePath)
{
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Restore nv item....");
    // CString strNVInfo = _T("");
    CString strNVMFile  = "";
    strNVMFile = strNVFilePath;
    BOOL                btemp                           = FALSE;
    dword               esn                             = 0 ;
    //启动计时
    CString             strtemp ;


    nv_item_list_info   rfcallistCur[MAX_NV_LIST_NUM]   =
    {
        {NV_MEID_I},            //      1943            
        {NV_ESN_I},//=0
        {NV_MANUFACTORY_INFO_I},//=22       
        {NV_C0_BC0_VGA_GAIN_OFFSET_VS_FREQ_I},//    1750
        {NV_C0_BC1_VGA_GAIN_OFFSET_VS_FREQ_I},//    1671
        {NV_C1_BC0_VGA_GAIN_OFFSET_VS_FREQ_I},//    1777
        {NV_C1_BC1_VGA_GAIN_OFFSET_VS_FREQ_I},//    1699
        {NV_C0_BC0_VGA_GAIN_OFFSET_I},//    1749
        {NV_C0_BC1_VGA_GAIN_OFFSET_I},//    1670
        {NV_C1_BC0_VGA_GAIN_OFFSET_I},//    1776
        {NV_C1_BC1_VGA_GAIN_OFFSET_I},//    1698
        {NV_C0_BC0_LNA_1_OFFSET_VS_FREQ_I},//   1739
        {NV_C0_BC0_LNA_2_OFFSET_VS_FREQ_I},//   1740
        {NV_C0_BC0_LNA_3_OFFSET_VS_FREQ_I},//   1741
        {NV_C0_BC0_LNA_4_OFFSET_VS_FREQ_I},//   1742
        {NV_C0_BC1_LNA_1_OFFSET_VS_FREQ_I},//   1650
        {NV_C0_BC1_LNA_2_OFFSET_VS_FREQ_I},//   1651
        {NV_C0_BC1_LNA_3_OFFSET_VS_FREQ_I},//   1652
        {NV_C0_BC1_LNA_4_OFFSET_VS_FREQ_I},//   1653
        {NV_C1_BC0_LNA_1_OFFSET_VS_FREQ_I},//   1766
        {NV_C1_BC0_LNA_2_OFFSET_VS_FREQ_I},//   1767
        {NV_C1_BC0_LNA_3_OFFSET_VS_FREQ_I},//   1768
        {NV_C1_BC0_LNA_4_OFFSET_VS_FREQ_I},//   1769
        {NV_C1_BC1_LNA_1_OFFSET_VS_FREQ_I},//   1687
        {NV_C1_BC1_LNA_2_OFFSET_VS_FREQ_I},//   1689
        {NV_C1_BC1_LNA_3_OFFSET_VS_FREQ_I},//   1690
        {NV_C1_BC1_LNA_4_OFFSET_VS_FREQ_I},//   1691
        {NV_C0_BC0_LNA_1_OFFSET_I},//   1743
        {NV_C0_BC0_LNA_2_OFFSET_I},//   1744
        {NV_C0_BC0_LNA_3_OFFSET_I},//   1745
        {NV_C0_BC0_LNA_4_OFFSET_I},//   1746
        {NV_C0_BC1_LNA_1_OFFSET_I},//   1654
        {NV_C0_BC1_LNA_2_OFFSET_I},//   1655
        {NV_C0_BC1_LNA_3_OFFSET_I},//   1666
        {NV_C0_BC1_LNA_4_OFFSET_I},//   1667
        {NV_C1_BC0_LNA_1_OFFSET_I},//   1770
        {NV_C1_BC0_LNA_2_OFFSET_I},//   1771
        {NV_C1_BC0_LNA_3_OFFSET_I},//   1772
        {NV_C1_BC0_LNA_4_OFFSET_I},//   1773
        {NV_C1_BC1_LNA_1_OFFSET_I},//   1692
        {NV_C1_BC1_LNA_2_OFFSET_I},//   1693
        {NV_C1_BC1_LNA_3_OFFSET_I},//   1694
        {NV_C1_BC1_LNA_4_OFFSET_I},//   1695
        {NV_C0_BC0_IM2_I_VALUE_I},//    1747
        {NV_C0_BC0_IM2_Q_VALUE_I},//    1748
        {NV_C0_BC1_IM2_I_VALUE_I},//    1668
        {NV_C0_BC1_IM2_Q_VALUE_I},//    1669
        {NV_C1_BC0_IM2_I_VALUE_I},//    1774
        {NV_C1_BC0_IM2_Q_VALUE_I},//    1775
        {NV_C1_BC1_IM2_I_VALUE_I},//    1696
        {NV_C1_BC1_IM2_Q_VALUE_I},//    1697
        {NV_C0_BC0_IM2_TRANSCONDUCTOR_VALUE_I},//   1751
        {NV_C0_BC1_IM2_TRANSCONDUCTOR_VALUE_I},//   1672
        {NV_C1_BC0_IM2_TRANSCONDUCTOR_VALUE_I},//   1778
        {NV_C1_BC1_IM2_TRANSCONDUCTOR_VALUE_I},//   1700
        {NV_BC0_TX_LIN_MASTER0_I},//    1716
        {NV_BC0_TX_LIN_MASTER1_I},//    1717
        {NV_BC0_TX_LIN_MASTER2_I},//    1718
        {NV_BC0_TX_LIN_MASTER3_I},//    1719
        {NV_BC1_TX_LIN_MASTER0_I},//    1627
        {NV_BC1_TX_LIN_MASTER1_I},//    1628
        {NV_BC1_TX_LIN_MASTER2_I},//    1629
        {NV_BC1_TX_LIN_MASTER3_I},//    1630
        {NV_BC0_TX_COMP0_I},//  1720
        {NV_BC0_TX_COMP1_I},//  1721
        {NV_BC0_TX_COMP2_I},//  1722
        {NV_BC0_TX_COMP3_I},//  1723
        {NV_BC1_TX_COMP0_I},//  1631
        {NV_BC1_TX_COMP1_I},//  1632
        {NV_BC1_TX_COMP2_I},//  1633
        {NV_BC1_TX_COMP3_I},//  1634
        {NV_BC0_TX_LIM_VS_FREQ_I},//    1724
        {NV_BC1_TX_LIM_VS_FREQ_I},//    1635
        {NV_BC0_EXP_HDET_VS_AGC_I},//   1733
        {NV_BC1_EXP_HDET_VS_AGC_I},//   1644
        {NV_BC0_HDET_OFF_I},//  1731
        {NV_BC1_HDET_OFF_I},//  1642
        {NV_BC0_HDET_SPN_I},//  1732
        {NV_BC1_HDET_SPN_I},//  1643
        {NV_C0_BC0_TX_CAL_CHAN_I},//    1737
        {NV_C0_BC1_TX_CAL_CHAN_I},//    1648
        {NV_C1_BC0_TX_CAL_CHAN_I},//    1764
        {NV_C1_BC1_TX_CAL_CHAN_I},//    1685
        {NV_C0_BC0_IM2_LPM_1_I},//  3253        
        {NV_C0_BC1_IM2_LPM_1_I},//  3254
        {NV_C0_BC0_IM2_LPM_2_I},//  3259
        {NV_C0_BC1_IM2_LPM_2_1},//  3260
        {NV_C0_BC0_INTELLICEIVER_CAL_I},//  3265
        {NV_C0_BC1_INTELLICEIVER_CAL_I},//  3266
        {NV_BC0_TX_PDM_0_I},//4795
        {NV_BC0_TX_PDM_1_I},//4796
        {NV_BC1_TX_PDM_0_I},//4799
        {NV_BC1_TX_PDM_1_I},//4800
        {NV_BC0_TX_LIN_0_I},//4831
        {NV_BC0_TX_LIN_1_I},//4832
        {NV_BC1_TX_LIN_0_I},//4835
        {NV_BC1_TX_LIN_1_I},//4836

    };

    TRY
    {
        // CFile hFile( strNVInfo, CFile::modeRead | CFile::shareDenyWrite);
        CFile   hFile   (strNVMFile,CFile::modeRead | CFile::shareDenyWrite);
        if (hFile.GetLength() != sizeof(rfcallistCur))
        {
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText("The nv file is not match the handset.");
            MessageBox("The nv file is not match the handset.","Warning",MB_ICONWARNING);
            return FALSE;
        }
        hFile.ReadHuge((void *) rfcallistCur,sizeof(rfcallistCur));
        hFile.Close();
    }
    CATCH(CFileException,e)
    {
        e->ReportError();
        CString strError    = _T("");
        strtemp.Format("Save file %s failed)",e->m_strFileName);
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
        return FALSE;
    }
    END_CATCH

    //offline
    int Error = 0;
    do
    {
        ::Sleep(200);
        Error++;
    }
    while ((!m_pPacket->EnterOfflineMode()) && (Error < 10));
    if (Error >= 10)
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Into download mode error. Please retry!");
        return FALSE;
    } 
    ::Sleep(2000);

    m_ProgressCtrl.SetForeColour(RGB(128,128,0));
    m_ProgressCtrl.SetTextBkColour(RGB(0,0,255));
    ShowRatio(0,MAX_NV_LIST_NUM);

    for (int i = 0 ; i < MAX_NV_LIST_NUM ; i++)
    {
        ShowRatio(i + 1);  
        if (rfcallistCur[i].item >= NV_MAX_I)
        {
            break;
        }
        if (rfcallistCur[i].status == NV_DONE_S)
        {
            if (!m_pPacket->NvItemWrite(rfcallistCur[i].item,rfcallistCur[i].item_data,rfcallistCur[i].status))
            {
                if (rfcallistCur[i].item == NV_ESN_I || rfcallistCur[i].item == NV_MEID_I)
                {
                    continue;
                }
                if (rfcallistCur[i].status == NV_NOTACTIVE_S || rfcallistCur[i].status == NV_READONLY_S)
                {
                    continue;
                }
                else
                {
                    //失败了再试一次，如果还不行就彻底失败
                    if (!m_pPacket->NvItemWrite(rfcallistCur[i].item,
                                                rfcallistCur[i].item_data,
                                                rfcallistCur[i].status))
                    {
                        strtemp.Format(" Don not restore nv Item = %d",rfcallistCur[i].item);
                        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
                        //                      return FALSE;
                    }
                }
            }
        }
        Sleep(10);
        //备份成功一项
        strtemp.Format(" Restore nv Item = %d",rfcallistCur[i].item);
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
    }
    ShowRatio(0);

    return TRUE;
}

BOOL CSST_F350Dlg::IsDownFileSelect()
{
    m_PageDL.GetDlgItemText(IDC_EDIT_USERFILE_PATH,m_PageDL.m_UserFilePath);

    if (m_PageDL.m_UserFilePath == "")
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Please select the phone image file to be downloaded.");
        MessageBox("Please select the phone image file to be downloaded.","Warning",MB_ICONWARNING);
        return FALSE;
    }

    if (!m_PageDL.SetDownloadFile(m_PageDL.m_UserFilePath))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Please select the phone image file to be downloaded.");
        MessageBox("Please select the phone image file to be downloaded.","Warning",MB_ICONWARNING);
        return FALSE;
    }

    return TRUE;
}



BOOL CSST_F350Dlg::HSDLmainFunc(char FilePath[], int nMinRate, int nMaxRate, CString strActionID)
{
    CFile   hReadFile ;
    if (!hReadFile.Open(FilePath,CFile::modeRead | CFile::shareDenyWrite))
    {
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText("open file fail!");
        return FALSE ;
    }   
    int     nTotalLen       = hReadFile.GetLength();
    byte    writeBuf[HS_TRANSMIT_BLOCK_SIZE] ;
    char    mywriteBuf[HS_TRANSMIT_BLOCK_SIZE] ;
    int     TotalBlock      = 0;    
    int     nBlockLen       = 0;    
    int     nWrittenLen     = 0;
    BOOL    bMore           = FALSE;

    int     nTempCount      = 0;                        //计数器，用来显示进度
    int     nOneStep;                           //百分之一进度的块数
    int     MinRate         = nMinRate ;
    int     MaxRate         = nMaxRate ;
    int     CurRate         = nMinRate ;
    BOOL    m_bCRCReturn    = TRUE ;

    ShowRatio(0);
    //计算CRC
    byte    CRC1, CRC2, CRC1_r, CRC2_r ;
    CRC1 = 0;
    CRC2 = 0;
    CRC1_r = 0;
    CRC2_r = 0;
    m_pPacket->MakeCRC2bytes(FilePath,CRC1,CRC2);

    CString strtemp = _T("");
    //发送高速下载协议启动指令。
    if (!m_pPacket->TransHSDLstartCommand(nTotalLen,CRC1,CRC2))
    {
        strtemp.Format("Begin high speed donwload fail!");
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp) ;
        hReadFile.Close() ; 
        return FALSE ;
    }

    //循环主体，开始直接不停传送文件数据
    int     nWritelen   = 0 ;
    CString strshow     = _T("");
    strshow.Format("(Send Data)...");
    strshow = strActionID + strshow ;
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strshow) ;

    MinRate = nMinRate ;
    MaxRate = nMinRate + (nMaxRate - nMinRate) * 7 / 10 ;
    CurRate = MinRate ;
    ShowRatio(CurRate) ;
    nTempCount = 0;
    TotalBlock = nTotalLen / HS_TRANSMIT_BLOCK_SIZE;
    nOneStep = TotalBlock / (MaxRate - MinRate);
    do
    {
        nBlockLen = nTotalLen - nWrittenLen;
        if (nBlockLen > HS_TRANSMIT_BLOCK_SIZE)
        {
            nBlockLen = HS_TRANSMIT_BLOCK_SIZE;
            bMore = TRUE;
        }
        else
        {
            bMore = FALSE;
        }
        hReadFile.Read(writeBuf,nBlockLen);
        memcpy(mywriteBuf,writeBuf,nBlockLen) ;
        nWritelen = nBlockLen ;

        nTempCount++;
        if (nTempCount >= nOneStep)   //每下载百分之一，进行一次进度显示
        {
            CurRate++ ; 
            if (CurRate > MaxRate)
            {
                CurRate = MaxRate;
            }
            ShowRatio(CurRate) ;                        
            nTempCount = 0;
        }
        TRACE("%Write Address:%X,Length:%X\n",nWrittenLen,nBlockLen);
        if (nWritelen != m_pPacket->WriteCommBytes(mywriteBuf,nBlockLen))
        {
            Sleep(100) ;
            if (nWritelen != m_pPacket->WriteCommBytes(mywriteBuf,nBlockLen))
            {
                strtemp.Format("High speed donwload fail!");
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp) ;
                hReadFile.Close() ;
                return FALSE ;
            }
        }
        nWrittenLen += nBlockLen;
    }
    while (bMore) ;
    hReadFile.Close();

    Sleep(2000);
    BOOL    CrcReturn   = FALSE;
    strtemp.Format("Wait for CRC!");
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
    for (int count = 0; count < 600; ++count)
    {
        ::Sleep(100);
        m_pPacket->DLRxPacket(); //接收数据包
        if (m_pPacket->m_retPacket.len != 0) //说明本次接收到包了
        {
            CrcReturn = TRUE;
            //做解包处理
            //第一个包为CRC返回，格式为7E 08 CRC1 CRC2 CRC1_r CRC2_r 
            int CRC_r   = (m_pPacket->m_retPacket.ret[0] >> 16);
            CRC1_r = (byte) CRC_r ;
            CRC2_r = (byte) (CRC_r >> 8) ;  
            if ((CRC1 == CRC1_r) && (CRC2 == CRC2_r))
            {
                strtemp.Format("CRC check OK");
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
                break;
            }
            else
            {
                strtemp.Format("CRC check fail，CRC1=%d CRC2=%d;CRC1_r=%d CRC2_r=%d",CRC1,CRC2,CRC1_r,CRC2_r);
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
                return FALSE ;
            }
        }
    }
    if (!CrcReturn)
    {
        strtemp.Format("NO CRC return within 60s!");
        GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
        AfxMessageBox(strtemp);
        return FALSE ;
    }

    //开始接收返回包，判断实际写入进度。
    int nWriteMaxAddr   = 0x00000000L ; //已写入的最大地址
    int nWriteMaxLength = nWriteMaxAddr *HS_RESPONSE_BLOCK_SIZE ;
    int i               = 0 ;
    int nRetAddress     = 0;
    int nCountNoRsp     = 0 ;
    MinRate = nMinRate + (nMaxRate - nMinRate) * 7 / 10 ;
    MaxRate = nMaxRate ;
    CurRate = MinRate ;
    ShowRatio(CurRate) ;
    nTempCount = 0;
    TotalBlock = nTotalLen / HS_RESPONSE_BLOCK_SIZE;
    nOneStep = TotalBlock / (MaxRate - MinRate);

    strshow = strActionID + "(Read Data)..." ;
    GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strshow) ;
    CString strlinshi   = "" ;
    while (nRetAddress < nTotalLen - 1) //说明还有需要等待响应的包
    {
        m_pPacket->DLRxPacket(); //接收数据包，并且分解为一条条的响应
        if (m_pPacket->m_retPacket.len != 0) //说明本次接收到包了
        {
            //做解包处理
            for (i = 0 ; i < m_pPacket->m_retPacket.len ; i++)
            {
                nTempCount++;
                if (nTempCount >= nOneStep)   //每下载百分之一，进行一次进度显示
                {
                    CurRate++ ; 
                    if (CurRate > MaxRate)
                    {
                        CurRate = MaxRate;
                    }
                    ShowRatio(CurRate) ;                        
                    nTempCount = 0;
                }
                nRetAddress = m_pPacket->m_retPacket.ret[i];
                strlinshi.Format(":0x%X",nRetAddress) ;
                strlinshi = strshow + strlinshi ;
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strlinshi) ;

                if (nRetAddress == nWriteMaxAddr + HS_RESPONSE_BLOCK_SIZE)
                {
                    //说明正常的一个一个按顺序回包
                    nWriteMaxAddr = nRetAddress ;
                }
                else if (nRetAddress >= nWriteMaxAddr + HS_RESPONSE_BLOCK_SIZE)
                {
                    //说明有丢包
                    nWriteMaxAddr = nRetAddress ;

                    //要做丢包处理，把丢的包放进缓冲区准备重发，此处暂时没做
                }
            }                           
            nCountNoRsp = 0 ;
            //  nWriteMaxLength = nWriteMaxAddr * HS_RESPONSE_BLOCK_SIZE ;
        }
        else
        {
            nCountNoRsp ++ ;
        }

        if (nCountNoRsp >= 10000)
        {
            strtemp.Format("No return within 10s!");
            GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
            return FALSE ;
        }
        Sleep(1) ;
    }

    m_pPacket->ClosePacket();
    m_pPacket->Hello();
    //  if(m_pPacket->ClosePacket())//关闭下载模式
    //  {
    //      strtemp.Format("Close Packet fail!");
    //      GetDlgItem(IDC_STATIC_INFO)->SetWindowText(strtemp);
    //      return FALSE;
    //  }

    ShowRatio(MaxRate) ;    
    return TRUE ;
}

BOOL CSST_F350Dlg::HSDLImage()
{
    int     MinRate     = 4;
    int     MaxRate     = 98;
    int     CurRate     = MinRate;
    int     nTotalLen   = 0;    
	byte type = 5;
	byte writeBuf[HS_TRANSMIT_BLOCK_SIZE_NEW];	//按块写，块的大小128K
	CFile hReadFile;
	
	if(!hReadFile.Open(m_PageDL.m_stSetting.amsshdPath, CFile::modeRead | CFile::shareDenyWrite))
	{
		//ShowInfo(Index,"Open file fail!");
		return FALSE ;
	}
	nTotalLen = hReadFile.GetLength();
	if(nTotalLen <= 0) 
	{
		//ShowInfo(Index, "file length wrong!");
		hReadFile.Close() ;
		return FALSE;
	}
	hReadFile.Read(writeBuf, nTotalLen);
	
	if (!m_pPacket->OpenMulti(type, writeBuf, nTotalLen)) 
	{
		Sleep(2000) ;
		if (!m_pPacket->OpenMulti(type, writeBuf, nTotalLen)) 
		{
			Sleep(2000) ;
			if (!m_pPacket->OpenMulti(type, writeBuf, nTotalLen)) 
			{
                GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Entry down amss mode failed!");
                hReadFile.Close() ;
				return FALSE;
			}
		}
	}
	hReadFile.Close();//add
	
	CString strID = _T("");
	strID = _T("Download amss.mbn ") ;

	char sPath[_MAX_DIR] ;
	strcpy(sPath, m_PageDL.m_stSetting.amsshdPath) ;
	if (!HSDLMainFuncNew(sPath, MinRate, MaxRate, strID) ) 
	{
		return FALSE ;
	}

    return TRUE;
}

BOOL CSST_F350Dlg::HSDL_EFSmbn()
{
    //要写的文件总长度
    int MinRate = 4;
    int MaxRate = 98;
    int CurRate = MinRate;
	byte type = 9;

    ShowRatio(0);   
	if (!m_pPacket->OpenMulti(/*0x09*/type,NULL,0)) 
	{
		Sleep(1000);
		if (!m_pPacket->OpenMulti(/*0x09*/type,NULL,0)) 
		{
			Sleep(3000);
			if (!m_pPacket->OpenMulti(/*0x09*/type,NULL,0)) 
			{
				GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Entry efs mode failed!");
				return FALSE;
			}
		}
	}
	
	char szPath[_MAX_DIR] ;
	strcpy(szPath, m_PageDL.m_stSetting.efsmbnPath) ;
	if (!HSDLMainFuncNew(szPath, MinRate, MaxRate, "download cefs.mbn ") )
	{
		return FALSE ;
	}	
	
    return TRUE;
}


BOOL CSST_F350Dlg::HSDLMainFuncNew(char FilePath[], int nMinRate, int nMaxRate, CString strActionID)
{	//文件大小 〉= 内存buffer 128K，CRC校验
	CFile hReadFile ;
	if(!hReadFile.Open(FilePath, CFile::modeRead | CFile::shareDenyWrite))
	{
		//ShowInfo(Index,"open file fail!");
		return FALSE ;
	}
	int nTotalLen = hReadFile.GetLength();
	int TotalBlock = 0;		
    int nPackatNum =(int)ceil(nTotalLen/HS_DEVICE_MEMORY_SIZE); //整个文件可以分成128K大小数据块的数量
	int nOneStep;//百分之一进度的块数
	int MinRate = nMinRate ;
	int MaxRate = nMaxRate ;
	int CurRate = nMinRate ;
	
	/*   m_pPacket->m_bCRC = pDoc->bIsCRC;*/
	
	CString strtemp = _T("");
	//发送高速下载协议启动指令 0x1D data[1]~[4] data[5]~[8] data[9]
	if ( !m_pPacket->TransHSDLstartCommandNew(nTotalLen) )
	{
		strtemp.Format("%s", "Send High Speed Download Start Command Fail!");
		//ShowInfo(Index, strtemp) ;
		hReadFile.Close();	
		return FALSE ;
	}
	
	//等待接收手机发回的准备好接收数据的消息	
	if (ReceiveHSDLRsp() != TRANS_DATA_RSP) 
	{
		strtemp.Format("%s", "Haven't Received Response to Receive Data!");
		//ShowInfo(Index, strtemp) ;
		hReadFile.Close() ;
		return FALSE ;
	}
	
	int iRepeat = 0;
	CString strshow = _T("");
	strshow.Format("%s", "(Transmit Data in High Speed)...");
	strshow = strActionID + strshow ;
	//ShowInfo(Index, strshow) ;
    for (int i = 0; i< nPackatNum; ++i) 
	{				
		//传送一块文件数据
		//每个块长度为128K, 最后一块根据剩余实际长度来计算
		int nBlockTotalLen = ((nTotalLen-i*HS_DEVICE_MEMORY_SIZE) > HS_DEVICE_MEMORY_SIZE) ? HS_DEVICE_MEMORY_SIZE: (nTotalLen-i*HS_DEVICE_MEMORY_SIZE);	
		MinRate = nMinRate + ( nMaxRate - nMinRate ) * i / nPackatNum;
		MaxRate= MinRate + ( nMaxRate - nMinRate ) * 3 / 10 / nPackatNum;
		if (MaxRate == MinRate )
		{
			MaxRate++ ;
		}
		//ShowRatio( Index, CurRate) ;
		
		TotalBlock = nBlockTotalLen/HS_DEVICE_MEMORY_SIZE ;	
		nOneStep = TotalBlock/(MaxRate-MinRate);
		
		byte crc[2] = {0};
		int checksum=0;
		checksum=m_pPacket->MakeCheckSum(FilePath,HS_DEVICE_MEMORY_SIZE*i,nBlockTotalLen);
		
		while (1) 
		{
			//CString strshow = "(高速协议下传送数据"+strFilename+"...)" ;
			//             CString strshow;
			// 			strshow.Format("(高速协议下传送数据第%d/%d部分...)",i+1,nPackatNum);
			// 			strshow = strActionID + strshow ;
			// 			ShowInfo(Index, strshow) ;
			//传输该块文件数据
			//if (!WritePacketData(Index,hReadFile,i*HS_DEVICE_MEMORY_SIZE,nBlockTotalLen,nOneStep,MinRate,pDoc->bIsCRC,crc)) 
			if (!WritePacketDataChecksum(hReadFile,i*HS_DEVICE_MEMORY_SIZE,nBlockTotalLen,nOneStep,MinRate,TRUE,&checksum)) 
			{
				strtemp.Format("%s", "Transmit Data in High Speed Fail!");
				//ShowInfo(Index, strtemp) ;
				hReadFile.Close() ;
				return FALSE ;
			}
			
			int ret;
			//	Sleep(30000);
			//ShowInfo(Index, "正进行CRC校验，请等待。。。") ;
			//接收手机返回的crc校验结果,格式为7E 1d armprg_qdld_rsp_type  armprg_qdld_ret_type  crc1 crc2			
            ret = ReceiveHSDLRsp();   //ret = TRANS_DATA_RSP  CRC_SUCCESS_RSP CRC_FAIL_RSP FALSE
			if (ret != FALSE) //是手机回复
			{
                if (ret == CRC_FAIL_RSP)//CRC校验失败，重发该块数据
                {					
					iRepeat++;
					if (iRepeat >= 10) //重发次数大于10，发送失败
					{
						hReadFile.Close() ;
						return FALSE;
					}
					strtemp.Format("%s", "CRC Check Fail, Tansmitting Data again!");
					//ShowInfo(Index, strtemp) ;
					continue;
                }
				if (ret == CRC_SUCCESS_RSP)
				{
					break;
				}				
			}
			else
			{  
				ret = ReceiveHSDLRsp(); //没有收到手机回复，再收一次
				if (ret != CRC_SUCCESS_RSP)
                {
					strtemp.Format("Line Haven't Received CRC Response");
					//ShowInfo(Index, strtemp) ;
					hReadFile.Close() ;
					return FALSE ;
				}
			}
		}
		MinRate=MaxRate;
		MaxRate=MinRate + ( nMaxRate - nMinRate ) * 7 / 10/ nPackatNum ;
		if ( MinRate == MaxRate )
			MinRate-- ;
		//ShowRatio( Index, MinRate) ;
		strshow = strActionID + "(Receiving Write Response)..." ;
		//strshow = strActionID + "(接收写入响应数据中)..." ;
		//ShowInfo(Index, strshow) ;
		//有时板测会将两包数据合成一包返回，判断checksum校验后接着有实际返回地址也认为正确，需要把数据清零，否则会影响下次的判断
		if ((m_pPacket->m_RxPacket.buf[7] == 0x7e)&&(m_pPacket->m_RxPacket.buf[8] == 0x08)&&((m_pPacket->m_RxPacket.buf[15] == 0x7e)||(m_pPacket->m_RxPacket.buf[16] == 0x7e)) )//有可能返回太快，两包数据被PC侧 一次读完
		{
			m_pPacket->m_RxPacket.buf[7] = 0;
			m_pPacket->m_RxPacket.buf[8] = 0;
			m_pPacket->m_RxPacket.buf[15] = 0;
			m_pPacket->m_RxPacket.buf[16] = 0;			
		}
		else
		{
			//接收返回数据
			if (ReadPacketData(i*HS_DEVICE_MEMORY_SIZE, nBlockTotalLen, MaxRate, MinRate) == 0) 
			{	
				CString aa;
				aa.Format("Haven't Received Write Reponse");
				//ShowInfo(Index, aa) ;
				hReadFile.Close() ;
				return FALSE;
			}
		}
	}
	  
	hReadFile.Close() ;//文件传送完毕，可以关闭文件
	if(!m_pPacket->ClosePacket())		//关闭下载模式
	{
		GetDlgItem(IDC_STATIC_INFO)->SetWindowText("HSDLmainFunc ClosePacket Fail");
		return FALSE;
	}
	
	ShowRatio(nMaxRate) ;
	
	return TRUE ;
}

int CSST_F350Dlg::ReceiveHSDLRsp()
{
	int nTry = 0;
	
	while (nTry<6000)//等待60s
		//while (nTry<12000)//等待60s,间隔修改为5ms
	{
		m_pPacket->DLRxPacket(); //接收数据包并做解包处理
		if(m_pPacket->m_RspPacket.rsp.hdlcrsp.cmd_code == SDP_HSDL_RSP_F_NEW) //说明本次接收到手机返回的数据包了
		{   
			int iRspType = m_pPacket->m_RspPacket.rsp.hdlcrsp.retType;//该RSP指令的类型:握手指令/CRC校验结果响应
			int iRetResult = m_pPacket->m_RspPacket.rsp.hdlcrsp.retResult;//该RSP指令为CRC校验结果响应时起作用，为0表示CRC校验失败，为1表示CRC校验成功
			switch (iRspType) 
			{
			case ARMPRG_QDLD_RSP_BUF_HANDSHAKE://该包是armprg返回的握手指令，告诉PC armrpg已经做好接收数据的准备
				{
					memset(&(m_pPacket->m_RspPacket.rsp), 0, sizeof(m_pPacket->m_RspPacket.rsp));//清空数据
					return TRANS_DATA_RSP;
					break;
				}
            case ARMPRG_QDLD_RSP_BUF_CHECK://该包是CRC校验结果响应
				{
					if (iRetResult == ARMPRG_QDLD_RET_FAIL) //0表示CRC校验失败
					{    
						//	ShowInfo(iIndex, "下载传输CRC校验错误");
						memset(&(m_pPacket->m_RspPacket.rsp), 0, sizeof(m_pPacket->m_RspPacket.rsp));//清空数据
						return CRC_FAIL_RSP;
					}
					else //1表示CRC校验成功
					{
						memset(&(m_pPacket->m_RspPacket.rsp), 0, sizeof(m_pPacket->m_RspPacket.rsp));//清空数据
						return CRC_SUCCESS_RSP;
					}
					break;
				}
			default:
				{
					return FALSE;
					break;
				}
			}			
		}
		else
		{   
			Sleep(10);//未收到包等待10ms，总共60s
			//	Sleep(5);
		}
		++nTry;
	}  
	return FALSE;	
}

BOOL CSST_F350Dlg::WritePacketDataChecksum(CFile&hReadFile,int iOffset,int nBlockTotalLen,int nOneStep,int CurRate,BOOL bCRC, int *checksum)
{
	BOOL bMore = FALSE;
	int nBlockLen = 0;	//待写入的块的大小
	byte writeBuf[HS_TRANSMIT_BLOCK_SIZE_NEW+4]={0};	//按块写，块的大小
	char mywriteBuf[HS_TRANSMIT_BLOCK_SIZE_NEW+4]={0} ;
	int nTempCount = 0;						//计数器，用来显示进度
	int nWritelen=0;
	int nWrittenLen = 0;//已经写入的块的大小
	int nWrittenLen_shiji = 0;//add by mengyanyan 
	
	hReadFile.Seek(iOffset,0);
	do
	{
		nBlockLen=nBlockTotalLen - nWrittenLen;
		if( nBlockLen> HS_TRANSMIT_BLOCK_SIZE_NEW)
		{
			nBlockLen = HS_TRANSMIT_BLOCK_SIZE_NEW;
			bMore = TRUE;
		}
		else
		{
			bMore = FALSE;
		}
		hReadFile.Read(writeBuf,nBlockLen);
		memcpy(mywriteBuf, writeBuf, nBlockLen) ;
		nWritelen = nBlockLen ;
		
		nTempCount++;
		if(nTempCount >= nOneStep)   //每下载百分之一，进行一次进度显示
		{
			CurRate++ ;			
			ShowRatio(CurRate ) ;						
			nTempCount=0;
		}				
		
		if(!bMore&&bCRC)
		{
			
			//最后一块，附加上crc
			//	mywriteBuf[nBlockLen]=crc[0];
			
			//   mywriteBuf[nBlockLen+1]=crc[1];
			mywriteBuf[nBlockLen++]=((byte *)checksum)[0];			
			mywriteBuf[nBlockLen++]=((byte *)checksum)[1];
			mywriteBuf[nBlockLen++]=((byte *)checksum)[2];			
			mywriteBuf[nBlockLen++]=((byte *)checksum)[3];
			
		}
		nWritelen=nBlockLen;
		nWrittenLen_shiji = m_pPacket->WriteCommBytes(mywriteBuf, nBlockLen);
		//	if ( nWritelen != m_pPacket->WriteCommBytes(mywriteBuf, nBlockLen) )
		if ( nWritelen !=nWrittenLen_shiji)
		{
			Sleep(100) ;
            InitPort(m_ComPort1,m_nBoundRate);
			nWrittenLen_shiji = m_pPacket->WriteCommBytes(mywriteBuf+nWrittenLen_shiji, nBlockLen-nWrittenLen_shiji);
			//		if ( nWritelen != m_pPacket->WriteCommBytes(mywriteBuf, nBlockLen) )
			if ( nWritelen !=nWrittenLen_shiji)
			{
				return FALSE ;
			}
		}
		nWrittenLen += nBlockLen;
	}while(bMore) ;
	return TRUE;
}

int CSST_F350Dlg::ReadPacketData(int iOffset,int nBlockTotalLen,int nMaxRate,int nMinRate)
{
	int nTempCount = 0;
	int nRetAddress = 0;
	int nCountNoRsp = 0 ;
	int nWriteMaxAddr = iOffset ;
	int nOneStep = 0;
	int TotalBlock = nBlockTotalLen / HS_RESPONSE_BLOCK_SIZE;
	
	nOneStep = TotalBlock/(nMaxRate-nMinRate);
	int CurRate = nMinRate;
	
	while(nRetAddress < iOffset+nBlockTotalLen - 1 ) //说明还有需要等待响应的包
	{
		m_pPacket->DLRxPacket(); //接收数据包，且解包
		
		if(m_pPacket->m_retPacket.len != 0) //说明本次接收到包了
		{
			//				CString tmp;
			//				tmp.Format(" 收到的响应包数量%d", m_pPacket->m_retPacket.len) ;
			//				ShowInfo(iIndex, tmp) ;
			//				tmp.Format(" 收到的响应包长度%d", m_pPacket->m_RxPacket.length) ;
			//				ShowInfo(iIndex, tmp) ;
			
			//				CString tmp1;
			//				tmp.Format("收到的相应包内容：");
			//				for (int aa=0;aa<m_pPacket->m_RxPacket.length;aa++) {
			//					tmp1.Format("%X ",m_pPacket->m_RxPacket.buf[aa]);
			//					tmp+=tmp1;
			//				}
			//				ShowInfo(iIndex, tmp) ;
			for (int i = 0 ; i < m_pPacket->m_retPacket.len ; ++i)
			{
				nTempCount++;
				if(nTempCount >= nOneStep)   //每下载百分之一，进行一次进度显示
				{
					CurRate++ ;	
					if (CurRate >= nMaxRate)
					{
						CurRate = nMaxRate;							
					}
					ShowRatio( CurRate ) ;						
					nTempCount = 0;
				}
				nRetAddress = m_pPacket->m_retPacket.ret[i];
				
				// 				CString tmp;
				// 				tmp.Format(" 已写入地址块 0x%X", nRetAddress) ;
				// 				//tmp = strshow + tmp ;
				// 				ShowInfo(iIndex, tmp) ;
				
				if ( nRetAddress == nWriteMaxAddr + HS_RESPONSE_BLOCK_SIZE )
				{ //说明正常的一个一个按顺序回包
					nWriteMaxAddr = nRetAddress ;
				}
				else if ( nRetAddress >= nWriteMaxAddr + HS_RESPONSE_BLOCK_SIZE )
				{//说明有丢包
					nWriteMaxAddr = nRetAddress ;
					
					//要做丢包处理，把丢的包放进缓冲区准备重发，此处暂时没做
				}
			}
			nCountNoRsp = 0 ;
			m_pPacket->m_retPacket.len = 0;
		}
		else
		{
			nCountNoRsp ++ ;
		}
		Sleep(10);
		if ( nCountNoRsp >= 10000 )//10000-20000
		{
			return 0 ;
		}		
	}
	
	return 1;
}