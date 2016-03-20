; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSST_F350Dlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "sst_f350.h"
LastPage=0

ClassCount=9
Class1=CEStatic
Class2=CPageDL
Class3=CPageNV
Class4=CRefurDlg
Class5=CDlgDMU
Class6=CSST_F350App
Class7=CAboutDlg
Class8=CSST_F350Dlg
Class9=CTextProgressCtrl

ResourceCount=6
Resource1=IDD_ABOUTBOX (English (U.S.))
Resource2=IDD_DIALOG_REFUR (English (U.S.))
Resource3=IDD_DIALOG_DL (English (U.S.))
Resource4=IDD_DIALOG_NV (English (U.S.))
Resource5=IDD_SST_F350_DIALOG (English (U.S.))
Resource6=IDD_DIALOG_DMU (English (U.S.))

[CLS:CEStatic]
Type=0
BaseClass=CStatic
HeaderFile=EStatic.h
ImplementationFile=EStatic.cpp

[CLS:CPageDL]
Type=0
BaseClass=CDialog
HeaderFile=operatepage.h
ImplementationFile=operatepage.cpp

[CLS:CPageNV]
Type=0
BaseClass=CDialog
HeaderFile=operatepage.h
ImplementationFile=operatepage.cpp

[CLS:CRefurDlg]
Type=0
BaseClass=CDialog
HeaderFile=operatepage.h
ImplementationFile=operatepage.cpp

[CLS:CDlgDMU]
Type=0
BaseClass=CDialog
HeaderFile=operatepage.h
ImplementationFile=operatepage.cpp
Filter=D
VirtualFilter=dWC

[CLS:CSST_F350App]
Type=0
BaseClass=CWinApp
HeaderFile=SST_F350.h
ImplementationFile=SST_F350.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=SST_F350DLG.cpp
ImplementationFile=SST_F350DLG.cpp
LastObject=CAboutDlg

[CLS:CSST_F350Dlg]
Type=0
BaseClass=CDialog
HeaderFile=SST_F350DLG.h
ImplementationFile=SST_F350DLG.cpp
LastObject=CSST_F350Dlg

[CLS:CTextProgressCtrl]
Type=0
BaseClass=CProgressCtrl
HeaderFile=TextProgressCtrl.h
ImplementationFile=TextProgressCtrl.cpp

[DLG:IDD_DIALOG_DL]
Type=1
Class=CPageDL

[DLG:IDD_DIALOG_NV]
Type=1
Class=CPageNV

[DLG:IDD_DIALOG_REFUR]
Type=1
Class=CRefurDlg

[DLG:IDD_DIALOG_DMU]
Type=1
Class=CDlgDMU

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_SST_F350_DIALOG]
Type=1
Class=CSST_F350Dlg

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=6
Control1=IDC_STATIC,static,1342308480
Control2=IDC_STATIC,static,1342308352
Control3=IDOK,button,1342373889
Control4=IDC_STATIC,static,1342177294
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,button,1342210055

[DLG:IDD_SST_F350_DIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=18
Control1=IDC_BUTTON_ABOUT,button,1342242816
Control2=IDC_BUTTON_EXIT,button,1342242816
Control3=IDC_COMBO_COM,combobox,1344339971
Control4=IDC_TAB_PAGE,SysTabControl32,1342177280
Control5=IDC_PROGRESS,msctls_progress32,1342177281
Control6=IDC_STATIC_BMP,static,1350565902
Control7=IDC_STATIC_CBSELECT,button,1342177287
Control8=IDC_STATIC_PROGRESS,static,1342308352
Control9=IDC_STATIC_PORTINFO,static,1342308352
Control10=IDC_STATIC_PORT,static,1342177283
Control11=IDC_STATIC_CSELECT,static,1342308352
Control12=IDC_STATIC,static,1350566158
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC_COM,edit,1342244992
Control15=IDC_STATIC_INFO,static,1342308352
Control16=IDC_BUTTON_REFRESH,button,1342242816
Control17=IDC_STATIC_BSELECT2,static,1073872896
Control18=IDC_EDIT_SPCCODE,edit,1350631552

[DLG:IDD_DIALOG_DL (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_EDIT_USERFILE_PATH,edit,1350633600
Control2=IDC_BUTTON2,button,1342242816
Control3=IDC_BUTTON_START,button,1476460544
Control4=IDC_STATIC,button,1342177287

[DLG:IDD_DIALOG_NV (English (U.S.))]
Type=1
Class=?
ControlCount=10
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC_NVPATH,edit,1350633600
Control4=IDC_BUTTON_BKB,button,1342242816
Control5=IDC_BUTTON_BKS,button,1342242816
Control6=IDC_STATIC_NVFILE,edit,1350633600
Control7=IDC_BUTTON_RSB,button,1342242816
Control8=IDC_BUTTON_RSS,button,1342242816
Control9=IDC_RADIO_NAM,button,1342308361
Control10=IDC_RADIO2,button,1342177289

[DLG:IDD_DIALOG_REFUR (English (U.S.))]
Type=1
Class=?
ControlCount=3
Control1=IDC_BUTTON_REFUR,button,1342242816
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,button,1342177287

[DLG:IDD_DIALOG_DMU (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,button,1342177287
Control2=IDC_EDIT_DMU_PATH,edit,1350633600
Control3=IDC_BUTTON_BROWSE,button,1342242816
Control4=IDC_BUTTON_START,button,1342242816

