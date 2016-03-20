/*********************************************************************
* 版权所有 (C)2001, 深圳市中兴通讯股份有限公司。
* 
* 文件名称： // opporatepage.cpp
* 文件标识： // 
* 内容摘要： // 属性页面CPageDL和CPageNV的类成员函数在此实现    
* 其它说明： // 
* 当前版本： // SST_F350_P300V1.00.08
* 作    者： // 
* 完成日期： // 
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
#include "operatepage.h"
#include "SST_F350Dlg.h"
#include "CIniFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageDL dialog

/************************************************************************
* 函数名称： // CPageDL(CWnd* pParent )                                 *
* 功能描述： // CPageDL的构造函数，完成对成员变量的初始化               *
* 输入参数： // 无                                                      *
* 返 回 值： // 无                                                      *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-21                                              *
* 修改内容： //                                                         *
************************************************************************/
CPageDL::CPageDL(CWnd *pParent /*=NULL*/):CDialog(CPageDL::IDD,pParent)
{
    //{{AFX_DATA_INIT(CPageDL)
    m_UserFilePath = _T("");
    m_FlashfilePath = _T("");
    m_CurrentDir = "";
    m_strSelPort = _T("");
    m_NVfilePath = _T("");
    //}}AFX_DATA_INIT

}

void CPageDL::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPageDL)
    DDX_Text(pDX, IDC_EDIT_USERFILE_PATH, m_UserFilePath);
    //}}AFX_DATA_MAP

}

BEGIN_MESSAGE_MAP(CPageDL, CDialog)
    //{{AFX_MSG_MAP(CPageDL)
    ON_BN_CLICKED(IDC_BUTTON2, OnButtonUserFilePath)
    ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageDL message handlers

/************************************************************************
* 函数名称： // OnInitDialog()                                          *
* 功能描述： // 在这里对控件变量赋初值（不能在构造函数中进行初始化)     *
* 输入参数： // 无                                                      *
* 返 回 值： // 正常运行的时候会返回TRUE                                *
* 其它说明： // 无                                                      *
* 修改日期： // 20006-09-21                                             *
* 修改内容： //                                                         *
************************************************************************/
BOOL CPageDL::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    CString StrCurrent; 
    char    CurrentDir[MAX_PATH];
    //  GetCurrentDirectory(MAX_PATH,CurrentDir);
    GetModuleFileName(NULL,CurrentDir,MAX_PATH);
    StrCurrent = CurrentDir;
    StrCurrent = StrCurrent.Left(StrCurrent.ReverseFind('\\'));

    m_NVfilePath.Format("%s\\NV_BackUp",StrCurrent);
    m_FlashfilePath.Format("%s",StrCurrent);
    m_CurrentDir.Format("%s",StrCurrent);

    ReadIniFile();

    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

/************************************************************************
* 函数名称： // OnButtonUserFilePath()                                  *
* 功能描述： // 选择下载文件的路径                                      *
* 输入参数： // 无                                                      *
* 返 回 值： // 无                                                      *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-21                                              *
* 修改内容： //                                                         *
************************************************************************/
void CPageDL::OnButtonUserFilePath()
{
    // TODO: Add your control notification handler code here
    // 目录选择
    BROWSEINFO  bi;
    char        dispname[MAX_PATH], path[MAX_PATH];
    ITEMIDLIST *pidl;
    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = NULL;//specify the path
    bi.pszDisplayName = dispname;
    bi.lpszTitle = "Please select software version directory";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_DONTGOBELOWDOMAIN ;
    bi.lpfn = 0;
    bi.lParam = 0;
    bi.iImage = 0;
    if (pidl = SHBrowseForFolder(&bi))
    {
        SHGetPathFromIDList(pidl,path);
    }
    else
    {
        return ;
    }
    UpdateData(TRUE) ;
    SetDownloadFile(path);
}

BOOL CPageDL::FolderOrFileIsExist(CString szPath)
{
    WIN32_FIND_DATA fd;

    CString         szFindPath  = szPath;
    int             nSlash      = szFindPath.ReverseFind('\\');
    int             nLength     = szFindPath.GetLength();

    if (nSlash == nLength - 1)
    {
        if (nLength == 3)
        {
            if (szFindPath.GetAt(1) == ':')
            {
                return true;
            }
        }
        else
        {
            szFindPath = szFindPath.Left(nSlash);
        }
    }

    HANDLE  hFind   = FindFirstFile(szFindPath,&fd);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
    }

    return hFind != INVALID_HANDLE_VALUE;
}

/************************************************************************
* 函数名称： // OnButtonStart()                                         *
* 功能描述： // 开始下载软件的激发事件，在此设定pDlg->PageActiveNum     *
*               等于11而在与手机通讯的线程中实现对该事件的处理          *
* 输入参数： // 无                                                      *
* 返 回 值： // 无                                                      *
* 其它说明： // 无                                                      *
* 修改日期： //                                                         *
* 修改内容： //                                                         *
************************************************************************/
void CPageDL::OnButtonStart()
{
    // TODO: Add your control notification handler code here
    UpdateData();

    CSST_F350Dlg   *pDlg    = (CSST_F350Dlg *) AfxGetApp()->GetMainWnd();

    if (!pDlg->IsDownFileSelect())
    {
		return;
        pDlg->PageActiveNum = 13;
    }
	switch (pDlg->m_nConnectType)
	{
		case DIAG_CONNECT:
			pDlg->PageActiveNum = 13;
			break;
		case DLAD_CONNECT:
			pDlg->PageActiveNum = 21;
			break;
		default:
			pDlg->PageActiveNum = 13;
	}
}

/************************************************************************
* 函数名称： // PreTranslateMessage(MSG* pMsg)                          *
* 功能描述： // 该函数实现对键盘事件的屏蔽                              *
* 输入参数： // 无                                                      *
* 返 回 值： // 无                                                      *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-21                                              *
* 修改内容： //                                                         *
************************************************************************/
BOOL CPageDL::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class  
    //  if(pMsg->message == WM_KEYDOWN)
    //  {
    //      return true;
    //  }   
    return CDialog::PreTranslateMessage(pMsg);
}


void CPageDL::SetUserFilePath(CString strPath)
{
    m_UserFilePath = strPath;
}

CString CPageDL::GetCurrentDir()
{
    return m_CurrentDir;
}

BOOL CPageDL::SetDownloadFile(CString path)
{
    CString str = path;

    // if select root directory, must delete the '\'
    int     len = str.GetLength();
    int     pos = str.ReverseFind('\\');
    if (pos == (len - 1) && len > 1)
    {
        str.Delete(pos,1);
    }
    sprintf(m_stSetting.strSoftStructDir,str) ;

    CString strtemp = _T("");
    strtemp.Format("%s\\partition.mbn",m_stSetting.strSoftStructDir);
    if (!FolderOrFileIsExist(strtemp))
    {
        strtemp += " is not exist, please select again";
        AfxMessageBox(strtemp);
        return FALSE;
    }
    strcpy(m_stSetting.partitionPath,strtemp);


    strtemp.Format("%s\\qcsblhd_cfgdata.mbn",m_stSetting.strSoftStructDir);
    if (!FolderOrFileIsExist(strtemp))
    {
        strtemp += " is not exist, please select again";
        AfxMessageBox(strtemp);
        return FALSE;
    }
    strcpy(m_stSetting.qcsblhd_cfgdataPath,strtemp);

    strtemp.Format("%s\\qcsbl.mbn",m_stSetting.strSoftStructDir);
    if (!FolderOrFileIsExist(strtemp))
    {
        strtemp += " is not exist, please select again";
        AfxMessageBox(strtemp);
        return FALSE;
    }

    strcpy(m_stSetting.qcsblPath,strtemp);

    strtemp.Format("%s\\oemsblhd.mbn",m_stSetting.strSoftStructDir);
    if (!FolderOrFileIsExist(strtemp))
    {
        strtemp += " is not exist, please select again";
        AfxMessageBox(strtemp);
        return FALSE;
    }
    strcpy(m_stSetting.oemsblhdPath,strtemp);

    strtemp.Format("%s\\oemsbl.mbn",m_stSetting.strSoftStructDir);
    if (!FolderOrFileIsExist(strtemp))
    {
        strtemp += " is not exist, please select again";
        AfxMessageBox(strtemp);
        return FALSE;
    }
    strcpy(m_stSetting.oemsblPath,strtemp);

    strtemp.Format("%s\\amsshd.mbn",m_stSetting.strSoftStructDir);
    if (!FolderOrFileIsExist(strtemp))
    {
        strtemp += " is not exist, please select again";
        AfxMessageBox(strtemp);
        return FALSE;
    }
    strcpy(m_stSetting.amsshdPath,strtemp);

    strtemp.Format("%s\\amss.mbn",m_stSetting.strSoftStructDir);
    if (!FolderOrFileIsExist(strtemp))
    {
        strtemp += " is not exist, please select again";
        AfxMessageBox(strtemp);
        return FALSE;
    }
    strcpy(m_stSetting.amssPath,strtemp);

    //EFS mbn
    strtemp.Format("%s\\cefs.mbn",m_stSetting.strSoftStructDir);
    if (!FolderOrFileIsExist(strtemp))
    {
        strtemp += " is not exist, the Efs file will not be downloaded!";
        //AfxMessageBox(strtemp);
    }
    strcpy(m_stSetting.efsmbnPath,strtemp);

    // pic and tone
    strtemp.Format("%s",m_stSetting.strSoftStructDir);
    if (!FolderOrFileIsExist(strtemp))
    {
        strtemp += " is not exist";
        AfxMessageBox(strtemp);
    }
    strcpy(m_stSetting.strEFSDir,strtemp);

    // show the dir 
    GetDlgItem(IDC_EDIT_USERFILE_PATH)->SetWindowText(m_stSetting.strSoftStructDir) ;
    return TRUE;
}


BOOL CPageDL::SaveIniFile()
{
    CIniFile    fread;
    CString     strPath;
    CString     filePath;
    char        CurrentDir[MAX_PATH];

    GetCurrentDirectory(MAX_PATH,CurrentDir);
    filePath.Format("%s\\setting.ini",CurrentDir);
    if (!fread.Create(filePath))
    {
        return FALSE;
    }
    GetDlgItemText(IDC_EDIT_USERFILE_PATH,strPath);

    fread.SetVarStr("SET","Port",m_strSelPort);
    fread.SetVarStr("SET","PATH_NAME",strPath);

    return TRUE;
}

BOOL CPageDL::ReadIniFile()
{
    CIniFile    fread;
    CString     strPath;
    CString     filePath;
    char        CurrentDir[MAX_PATH];

    GetCurrentDirectory(MAX_PATH,CurrentDir);
    filePath.Format("%s\\setting.ini",CurrentDir);
    if (!fread.Create(filePath))
    {
        return FALSE;
    }

    fread.GetVarStr("SET","Port",m_strSelPort);
    fread.GetVarStr("SET","PATH_NAME",strPath);
    m_UserFilePath = strPath;
    //  m_PageDL.SetDlgItemText(IDC_EDIT_USERFILE_PATH,strPath);
    if (strPath != "")
    {
        if (!SetDownloadFile(strPath))
        {
            m_UserFilePath = "";
        }
    }

    return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CPageNV dialog

/************************************************************************
* 函数名称： // CPageNV(CWnd* pParent)                                  *
* 功能描述： // CPageNV类的构造函数，对成员变量赋初始值                 *
* 输入参数： // 无                                                      *
* 返 回 值： // 无                                                      *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-22                                              *
* 修改内容： //                                                         *
************************************************************************/
CPageNV::CPageNV(CWnd *pParent /*=NULL*/):CDialog(CPageNV::IDD,pParent)
{
    //{{AFX_DATA_INIT(CPageNV)
    m_NVfile = _T("");
    m_NVfilePath = _T("");
    m_nNam = 0;
    //}}AFX_DATA_INIT

}


void CPageNV::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPageNV)
    DDX_Text(pDX, IDC_STATIC_NVFILE, m_NVfile);
    DDX_Text(pDX, IDC_STATIC_NVPATH, m_NVfilePath);
    DDX_Radio(pDX, IDC_RADIO_NAM, m_nNam);
    //}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CPageNV, CDialog)
    //{{AFX_MSG_MAP(CPageNV)
    ON_BN_CLICKED(IDC_BUTTON_BKB, OnButtonBkbFilePath)
    ON_BN_CLICKED(IDC_BUTTON_RSB, OnButtonRsbFile)
    ON_BN_CLICKED(IDC_BUTTON_BKS, OnButtonBksStart)
    ON_BN_CLICKED(IDC_BUTTON_RSS, OnButtonRssStart)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageNV message handlers

/************************************************************************
* 函数名称： // OnButtonBkbFilePath()                                   *
* 功能描述： // 执行此函数来设置备份NV文件的路径                        *
* 输入参数： // 无                                                      *
* 返 回 值： // 无                                                      *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-22                                              *
* 修改内容： //                                                         *
************************************************************************/
void CPageNV::OnButtonBkbFilePath()
{
    int         nLen;
    // TODO: Add your control notification handler code here
    CFileDialog dlg (TRUE,
                     "PRL",
                     NULL,
                     OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "PRL Files(*.prl) | *.prl; ||",
                     NULL);
    if (dlg.DoModal() == IDOK)
    {
        m_NVfilePath = dlg.GetPathName();
        GetDlgItem(IDC_STATIC_NVPATH)->SetWindowText(m_NVfilePath);
        nLen = m_NVfilePath.GetLength();
        ((CEdit *) GetDlgItem(IDC_STATIC_NVPATH))->SetSel(nLen,nLen);
    }
}

/************************************************************************
* 函数名称： // OnButtonRsbFile()                                       *
* 功能描述： // 选择要用来恢复手机NV项的文件                            *
* 输入参数： // 无                                                      *
* 返 回 值： // 无                                                      *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-24                                              *
* 修改内容： //                                                         *
************************************************************************/
void CPageNV::OnButtonRsbFile()
{
    // TODO: Add your control notification handler code here
    int         nLen;
    CFileDialog dlg (TRUE,
                     "ERI",
                     NULL,
                     OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "ERI Files(*.bin) | *.bin; ||",
                     NULL);
    if (dlg.DoModal() == IDOK)
    {
        m_NVfile = dlg.GetPathName();
        SetDlgItemText(IDC_STATIC_NVFILE,m_NVfile);
        nLen = m_NVfile.GetLength();
        ((CEdit *) GetDlgItem(IDC_STATIC_NVFILE))->SetSel(nLen,nLen);
    }
}

/************************************************************************
* 函数名称： // OnButtonBkStart()                                       *
* 功能描述： // 触发备份NV的事件，在此设定pDlg->PageActiveNum等于21，   *
*               而在与手机通讯的线程中实现对该事件的处理                *
* 输入参数： // 无                                                      *
* 返 回 值： // 无                                                      *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-22                                              *
* 修改内容： //                                                         *
************************************************************************/
void CPageNV::OnButtonBksStart()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    CSST_F350Dlg   *pDlg    = (CSST_F350Dlg *) AfxGetApp()->GetMainWnd();
    GetDlgItemText(IDC_STATIC_NVPATH,m_NVfilePath);
    if (m_NVfilePath == "")
    {
        pDlg->GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Please select the PRL file to be updated!");
        MessageBox("Please select the PRL file to be updated!","Warning",MB_ICONWARNING);
        return;
    }
    pDlg->PageActiveNum = 11;
}

/************************************************************************
* 函数名称： // OnButtonRssStart()                                      *
* 功能描述： // 触发恢复NV的事件，在此设定pDlg->PageActiveNum           *
*               等于22，在与手机通讯的线程中实现对该事件的处理          *
* 输入参数： // 无                                                      *
* 返 回 值： // 无                                                      *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-22                                              *
* 修改内容： //                                                         *
************************************************************************/
void CPageNV::OnButtonRssStart()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    CSST_F350Dlg   *pDlg    = (CSST_F350Dlg *) AfxGetApp()->GetMainWnd();
    GetDlgItemText(IDC_STATIC_NVFILE,m_NVfile);
    if (m_NVfile == "")
    {
        pDlg->GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Please select the ERI file to be updated!");
        MessageBox("Please select the ERI file to be updated!","Warning",MB_ICONWARNING);
        return;
    }   

    pDlg->PageActiveNum = 12;
}

/************************************************************************
* 函数名称： // PreTranslateMessage(MSG* pMsg)                          *
* 功能描述： // 该函数实现对键盘事件的屏蔽                              *
* 输入参数： // 无                                                      *
* 返 回 值： // 如果捕获按键事件，就返回TRUE                            *
* 其它说明： // 无                                                      *
* 修改日期： // 2006-09-22                                              *
* 修改内容： //                                                         *
************************************************************************/
BOOL CPageNV::PreTranslateMessage(MSG *pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if (pMsg->message == WM_KEYDOWN)
    {
        return true;
    }   
    return CDialog::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CRefurDlg dialog


CRefurDlg::CRefurDlg(CWnd *pParent /*=NULL*/):CDialog(CRefurDlg::IDD,pParent)
{
    //{{AFX_DATA_INIT(CRefurDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

}


void CRefurDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRefurDlg)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CRefurDlg, CDialog)
//{{AFX_MSG_MAP(CRefurDlg)
ON_BN_CLICKED(IDC_BUTTON_REFUR, OnButtonRefur)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRefurDlg message handlers

void CRefurDlg::OnButtonRefur()
{
    // TODO: Add your control notification handler code here
    CSST_F350Dlg   *pDlg    = (CSST_F350Dlg *) AfxGetApp()->GetMainWnd();

    pDlg->ShowRatio(0);
    pDlg->PageActiveNum = 22;
}


/////////////////////////////////////////////////////////////////////////////
// CDlgDMU dialog


CDlgDMU::CDlgDMU(CWnd *pParent /*=NULL*/):CDialog(CDlgDMU::IDD,pParent)
{
    //{{AFX_DATA_INIT(CDlgDMU)
    m_strDMUPath = _T("");
    //}}AFX_DATA_INIT

}


void CDlgDMU::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgDMU)
    DDX_Text(pDX, IDC_EDIT_DMU_PATH, m_strDMUPath);
    //}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CDlgDMU, CDialog)
//{{AFX_MSG_MAP(CDlgDMU)
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
    ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDMU message handlers

void CDlgDMU::OnButtonBrowse()
{
    // TODO: Add your control notification handler code here
    int         nLen;
    // TODO: Add your control notification handler code here
    CFileDialog dlg (TRUE,
                     "Key",
                     NULL,
                     OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                     "Key Files(*.key) | *.key; ||",
                     NULL);
    if (dlg.DoModal() == IDOK)
    {
        m_strDMUPath = dlg.GetPathName();
        GetDlgItem(IDC_EDIT_DMU_PATH)->SetWindowText(m_strDMUPath);
        nLen = m_strDMUPath.GetLength();
        ((CEdit *) GetDlgItem(IDC_EDIT_DMU_PATH))->SetSel(nLen,nLen);
    }
}

void CDlgDMU::OnButtonStart()
{
    // TODO: Add your control notification handler code here
    CSST_F350Dlg   *pDlg    = (CSST_F350Dlg *) AfxGetApp()->GetMainWnd();

    GetDlgItemText(IDC_EDIT_DMU_PATH,m_strDMUPath);
    if (m_strDMUPath == "")
    {
        pDlg->GetDlgItem(IDC_STATIC_INFO)->SetWindowText("Please select the *.key file to be updated!");
        MessageBox("Please select the *.key file to be updated!","Warning",MB_ICONWARNING);
        return;
    }   

    pDlg->ShowRatio(0);
    pDlg->PageActiveNum = 23;
}
