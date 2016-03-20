/****************************************************************************** 
FILE:  isms.h 
 
SERVICES:  Header file 
 
GENERAL DESCRIPTION: 
  isms demo 
 
PUBLIC CLASSES AND STATIC FUNCTIONS: 
  Classes: 
 
INITIALIZATION & SEQUENCING REQUIREMENTS: 
 
  See Exported Routines 
 
       Copyright ? 2008 unicom-brew 
               All Rights Reserved. 
******************************************************************************/ 
#ifndef ISMS_H 
#define ISMS_H 
 
// MO, 接收号码 
#define DESTINATION_NUMBER "13641081369" 
 
// ascii 短信内容，对于unicode短信内容，必须由资源文件bar中获取，否则编码不对 
#define MO_TEXT_ASCII "Your friend recommend a mobile phone application - China Daily Paper, you can visit mshop to download and play if you are interested it, hope you like it" 
 
/*=========================================================================== 
 
                      PUBLIC DATA DECLARATIONS 
 
===========================================================================*/ 
 
/*------------------------------------------------------------------- 
            Type Declarations 
-------------------------------------------------------------------*/ 
// IFile app struct. This is the main struct for this applet. This will hold  
// all the data members that need to be remembered throughout the life of 
// the applet.  
// THE FIRST DATA MEMBER OF THIS STRUCT MUST BE AN AEEApplet OBJECT.  
// This is critical requirement which is related to an applet being 
// dynamically loaded. The app developers should add their data members 
// following the AEEApplet data member. 
typedef struct _ISmsApp { 
   AEEApplet        a; 
   IMenuCtl *  m_pIMenu; 
   AEERect     m_rc; 
   int m_cLineNum; // screen line to be written to next 
   int m_cMaxLine; // max line number of screen on which output can be written 
 
    ISMSMsg *m_pISMSMsg; 
    ISMS       *m_pISMS; 
    AEECallback  m_cb; 
 
    ISMSStorage *m_pISMSStorage;      // ISMSStorage Interface 
    ModelListener  m_SMSStorageModelListener; // Model Listener for ISMSStorage 
    AEESMSStorageStatus m_ss;           // Storage Status 
    uint32         m_dwStatus;          // Status 
    AEECallback EnumMsgInitCb; 
    AEECallback EnumMsgNextCb; 
    uint32         m_dwIndex;           // Message Index 
    AEESMSTag m_tag; 
    AEESMSType m_mt; 
} ISmsApp; 
 
#endif // ISMS_H 
