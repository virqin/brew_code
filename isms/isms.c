
/*===========================================================================  
  
FILE: isms.c  
  
SERVICES: Sample applet using AEE  
  
DESCRIPTION  
  This file contains usage examples of the ISMSMSG , ISMS and ISMSNotifier interfaces.   
  
PUBLIC CLASSES:    
   N/A  
  
       Copyright ? 2000-2008 Unicom-brew.  
                    All Rights Reserved.  
                   Unicom-brew  
===========================================================================*/   
   
/*===============================================================================  
                     INCLUDES AND VARIABLE DEFINITIONS  
=============================================================================== */   
// Mandatory includes   
#include "AEEAppGen.h"  // AEEApplet declaration   
#include "AEE.h"      // Standard AEE Declarations   
#include "AEEShell.h" // AEE Shell Services   
#include "AEEStdlib.h"    // AEE Display Services   
#include "AEEDisp.h"    // AEE Display Services   
#include "AEEMenu.h"    // AEE Display Services   
   
// Services used by app   
#include "AEETAPI.h"   
#include "AEESMS.h"   
#include "isms.bid"   
#include "isms.brh"   
#include "isms.h"   
   
/*-------------------------------------------------------------------  
            Function Prototypes  
-------------------------------------------------------------------*/   
// Mandatory function:   
// App Handle Event function   
static boolean ISmsApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);   
   
// App specific data alloc-init/free functions   
static boolean ISmsApp_InitAppData(IApplet* pMe);   
static void ISmsApp_FreeAppData(IApplet* pMe);   
   
static void BuildMainMenu(ISmsApp *pMe);   
static void ISmsAppUsage (ISmsApp *pMe, uint16 wParam);   
static void DisplayEvent (ISmsApp *pMe, uint16 wParam);   
static int DisplayOutput(IShell *pIShell, IDisplay *pIDisplay, int nline, char *pszStr, AECHAR *pszwStr, boolean isWideChar);   
static void WriteLine (ISmsApp *pMe, char *pszStr, AECHAR *pszwStr, boolean isWideChar);   
static void SMSCallBack_Send(void *p);   
static void OATSMS_StoreReadMsgStorageStatusCb(void * po);   
static void OATSMS_EnumMsgInitCb(void * po);   
static void OATSMS_EnumMsgNextCb(void * po);   
static void showSMS(ISmsApp * pMe,ISMSMsg *pRxMsg);   
static void OATSMSStorage_ModelListener(ISMSMsg * pMe, ModelEvent *pEvent);   
/*-------------------------------------------------------------------  
            Global Constant Definitions  
-------------------------------------------------------------------*/   
   
// App specific constants   
#define USAGE_SMS_TX_ASCII         100   
#define USAGE_SMS_TX_UNICODE    101   
#define USAGE_SMS_TX_UTF8           102   
#define USAGE_SMS_RX_TAPI_METHOD       103   
#define USAGE_SMS_RX_ISMS_METHOD       104   
#define USAGE_SMS_STORE_STATUS     105   
#define USAGE_SMS_STORE_ENUM_READ     106   
#define USAGE_ERASE_SCREEN      119   // Special case to erase screen   
   
// Size of buffer used to hold text strings   
#define TEXT_BUFFER_SIZE (100 * sizeof(AECHAR))   
   
/*===============================================================================  
                     FUNCTION DEFINITIONS  
=============================================================================== */   
   
/*===========================================================================  
  
FUNCTION: AEEClsCreateInstance  
  
DESCRIPTION  
  This function is invoked while the app is being loaded. All Modules must provide this   
  function. Ensure to retain the same name and parameters for this function.  
  In here, the module must verify the ClassID and then invoke the AEEApplet_New() function  
  that has been provided in AEEAppGen.c.   
  
   After invoking AEEApplet_New(), this function can do app specific initialization. In this  
   example, a generic structure is provided so that app developers need not change app specific  
   initialization section every time except for a call to IFile_InitAppData(). This is done as follows:  
   IFile_InitAppData() is called to initialize the applet's data. It is app developer's   
   responsibility to fill-in app data initialization code of IFile_InitAppData(). App developer  
   is also responsible to release memory allocated for data -- this can be  
   done in IFile_FreeAppData().  
  
PROTOTYPE:  
  boolean AEEClsCreateInstance(AEECLSID ClsId, IShell* pIShell, IModule* pIModule,   
                      void** ppObj)  
  
PARAMETERS:  
  ClsId: [in]: Specifies the ClassID of the applet which is being loaded  
  
  pIShell: [in]: Contains pointer to the IShell interface.   
  
  pIModule: [in]: Contains pointer to the IModule interface to the current module to which  
  this app belongs  
  
  ppObj: [out]: On return, *ppObj must point to a valid AEEApplet structure. Allocation  
  of memory for this structure and initializing the base data members is done by AEEApplet_New().  
  
DEPENDENCIES  
  none  
  
RETURN VALUE  
  TRUE: If the app needs to be loaded and if AEEApplet_New() invocation was successful  
  FALSE: If the app does not need to be loaded or if errors occurred in AEEApplet_New().  
  If this function returns FALSE, the app will not be loaded.  
  
SIDE EFFECTS  
  none  
===========================================================================*/   
int AEEClsCreateInstance(AEECLSID ClsId, IShell* pIShell, IModule* po, void** ppObj)   
{   
    *ppObj = NULL;   
       
    // We want to load this App. So, invoke AEEApplet_New().To it, pass the   
    // address of the app-specific handle event function.   
    if(ClsId == AEECLSID_ISMS)   
    {   
        if (AEEApplet_New(sizeof(ISmsApp), ClsId, pIShell,po,(IApplet**)ppObj,   
            (AEEHANDLER)ISmsApp_HandleEvent,(PFNFREEAPPDATA)ISmsApp_FreeAppData) == TRUE)   
        {   
            if (ISmsApp_InitAppData((IApplet*)*ppObj))   
            {   
                return(AEE_SUCCESS);   
            }   
        }   
    }   
    return (EFAILED);   
}   
   
/*===========================================================================  
  
FUNCTION ISmsApp_HandleEvent  
  
DESCRIPTION  
  This is the EventHandler for this app. All events to this app are handled in this  
  function.  
  
PROTOTYPE:  
  static boolean App_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)  
  
PARAMETERS:  
  pi: Pointer to the AEEApplet structure. This structure contains information specific  
  to this applet.  
  
  ecode: Specifies the Event sent to this applet  
  
    wParam, dwParam: Event-specific data.  
  
DEPENDENCIES  
    none  
  
RETURN VALUE  
    TRUE: If the app has processed the event  
    FALSE: If the app did not process the event  
  
SIDE EFFECTS  
  none  
===========================================================================*/   
   
static boolean ISmsApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)   
{     
    ISmsApp * pMe = (ISmsApp*)pi;   
   
    switch (eCode)    
    {   
   
        case EVT_APP_START:   
            // When this applet is started, try to create the main menu   
            // that allows the user to select a usage example to execute.   
            // If we cannot create an instance of the menu class, exit and   
            // indicate that we have handled the event by returning TRUE   
            if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MENUCTL,    
                                                (void **)&pMe->m_pIMenu) != SUCCESS)   
                return TRUE;   
   
            // Succeeded in obtaining a menu instance pointer, so   
            // construct the menu and display it on the screen   
            BuildMainMenu(pMe);   
            return(TRUE);   
   
            case EVT_APP_STOP:   
            {   
                int nErr;   
                AEECLSID clsNotifier= AEECLSID_TAPI; // event sender   
                AEECLSID clsNotify = AEECLSID_ISMS; // event listener   
                uint32 dwMask = 0; // for deregister NMASK_TAPI_SMS_TEXT;    
                   
                // De -Register a notification    
                nErr = ISHELL_RegisterNotify (pMe->a.m_pIShell, clsNotify, clsNotifier, dwMask);   
                DBGPRINTF("ISHELL_RegisterNotify ret %d", nErr);         
   
                clsNotifier= AEECLSID_SMSNOTIFIER; // event sender   
                clsNotify = AEECLSID_ISMS; // event listener   
                dwMask = 0; // for deregister event;    
   
                // De - Register a notification    
                nErr = ISHELL_RegisterNotify (pMe->a.m_pIShell, clsNotify, clsNotifier, dwMask);   
                DBGPRINTF("ISHELL_RegisterNotify ret %d", nErr);         
            }   
            return(TRUE);   
   
            case EVT_KEY:    
                // 控制上下键选择菜单   
                if (pMe->m_pIMenu)   
                {   
                    if ((IMENUCTL_IsActive(pMe->m_pIMenu) == FALSE) &&   
                       ((wParam == AVK_UP) || (wParam == AVK_DOWN)))   
                    {   
                        IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);   
                        IMENUCTL_Redraw(pMe->m_pIMenu);   
                    }   
   
                    return IMENUCTL_HandleEvent(pMe->m_pIMenu, EVT_KEY, wParam, 0);   
                }   
                else   
                  return FALSE;   
   
   
            case EVT_NOTIFY:   
                {   
                    uint32 uMsgId;   
                    ISMSMsg *pRxMsg=NULL;   
                    AEENotify* pNotify = (AEENotify*) dwParam;   
   
                    // 使用原TAPI接口接收短信   
                    if (pNotify && (pNotify->cls == AEECLSID_TAPI)) // event sender   
                    {   
                        DBGPRINTF("AEECLSID_TAPI sender");   
   
                        if ((pNotify->dwMask & NMASK_TAPI_SMS_TEXT) == NMASK_TAPI_SMS_TEXT)    
                        {   
                            AEESMSTextMsg *pMsg;   
                            DBGPRINTF("Notify mask ok");   
                            pMsg = (AEESMSTextMsg *)pNotify->pData;   
   
                            // 此处假定短信为ASCII，如果UNICODE需要首先检测，再输出UNICODE字串   
                            WriteLine(pMe,  pMsg->szText, NULL, FALSE);   
                        }   
                    }   
                       
                    // 使用新的SMSNotifier 接口接收短信,此接口BREW 3.1.x 以上版本支持   
                    if (pNotify && (pNotify->cls == AEECLSID_SMSNOTIFIER)) // event sender   
                    {   
                        uint32  MyNMask;   
   
                        DBGPRINTF("AEECLSID_SMSNOTIFIER sender");   
                        // 普通短信的EVENT MASK   
                        MyNMask = ((AEESMS_TYPE_TEXT << 16) | NMASK_SMS_TYPE);   
                        if ((pNotify->dwMask & MyNMask) == MyNMask)    
                        {   
                            DBGPRINTF("Notify mask ok");   
                               
                            uMsgId = (uint32 )pNotify->pData;   
                            if(ISMS_ReceiveMsg(pMe->m_pISMS, uMsgId, &pRxMsg)==AEE_SUCCESS)   
                            {   
                                SMSMsgOpt TmpOpt;   
                                DBGPRINTF("ISMS_ReceiveMsg ret ok");   
   
                                // MSGOPT_FROM_DEVICE_SZ - 发送方号码   
                                if(ISMSMSG_GetOpt(pRxMsg, MSGOPT_FROM_DEVICE_SZ, &TmpOpt)==AEE_SUCCESS)   
                                {   
                                    DBGPRINTF("get MSGOPT_FROM_DEVICE_SZ");   
                                    WriteLine(pMe,  (char *)TmpOpt.pVal, NULL, FALSE);   
                                }   
   
                                // MSGOPT_PAYLOAD_SZ - ASCII的短信内容   
                                if(ISMSMSG_GetOpt(pRxMsg, MSGOPT_PAYLOAD_SZ, &TmpOpt)==AEE_SUCCESS)   
                                {   
                                    DBGPRINTF("get MSGOPT_PAYLOAD_SZ");   
                                    WriteLine(pMe,  (char *)TmpOpt.pVal, NULL, FALSE);   
                                }   
   
                                // MSGOPT_PAYLOAD_WSZ - UNICODE的短信内容   
                                if(ISMSMSG_GetOpt(pRxMsg, MSGOPT_PAYLOAD_WSZ, &TmpOpt)==AEE_SUCCESS)   
                                {   
                                    DBGPRINTF("get MSGOPT_PAYLOAD_WSZ");   
                                    WriteLine(pMe,  NULL, (AECHAR *)TmpOpt.pVal, TRUE);   
                                }   
   
                                // MSGOPT_PAYLOAD_BINARY - UTF-8的短信内容   
                                if(ISMSMSG_GetOpt(pRxMsg, MSGOPT_PAYLOAD_BINARY, &TmpOpt)==AEE_SUCCESS)   
                                {    
                                    unsigned char *ucBuf;   
                                    AECHAR  UnicodeBuf[100];   
                                    DBGPRINTF("PAYLOAD_BINARY(len: %d):", WEBOPT_GETVARBUFFERLENGTH(TmpOpt.pVal));   
                                    ucBuf = WEBOPT_GETVARBUFFERDATA(TmpOpt.pVal);   
                                    UTF8TOWSTR(ucBuf, WEBOPT_GETVARBUFFERLENGTH(TmpOpt.pVal), UnicodeBuf, sizeof(UnicodeBuf));   
                                       
                                    DBGPRINTF("0x%2x, %2x, %2x, %2x", UnicodeBuf[0], UnicodeBuf[1], UnicodeBuf[2], UnicodeBuf[3] );   
                                    DBGPRINTF("0x%2x, %2x, %2x, %2x", UnicodeBuf[4], UnicodeBuf[5], UnicodeBuf[6], UnicodeBuf[7] );   
                                    DBGPRINTF("0x%2x, %2x, %2x, %2x", UnicodeBuf[8], UnicodeBuf[9], UnicodeBuf[10], UnicodeBuf[11] );   
                                    DBGPRINTF("0x%2x, %2x, %2x, %2x", UnicodeBuf[12], UnicodeBuf[13], UnicodeBuf[14], UnicodeBuf[15] );   
                                    WriteLine(pMe,  NULL, (AECHAR *)UnicodeBuf, TRUE);   
                                }   
   
                                // MSGOPT_TIMESTAMP - 短信时间标签   
                                if(ISMSMSG_GetOpt(pRxMsg, MSGOPT_TIMESTAMP, &TmpOpt)==AEE_SUCCESS)   
                                {   
                                    JulianType *pMyJulian;   
                                    char buf[80];   
                                    DBGPRINTF("get MSGOPT_TIMESTAMP");   
   
                                    pMyJulian = (JulianType *)TmpOpt.pVal;   
                                    SPRINTF(buf, "%d/%d/%d %d:%d:%d, weekday:%d", pMyJulian->wMonth, pMyJulian->wDay, pMyJulian->wYear,    
                                                                                              pMyJulian->wHour, pMyJulian->wMinute, pMyJulian->wSecond, pMyJulian->wWeekDay);   
                                    WriteLine(pMe,  buf, NULL, FALSE);   
                                }   
                            }   
   
                        }   
                    }   
                }   
   
                   
                return TRUE;   
               
    case EVT_COMMAND:   
      switch(wParam)   
      {   
          // The following commands are generated by user selections   
          // from the main usage app menu.   
          case USAGE_SMS_TX_ASCII:   
          case USAGE_SMS_TX_UNICODE:   
          case USAGE_SMS_TX_UTF8:   
          case USAGE_SMS_RX_TAPI_METHOD:   
          case USAGE_SMS_RX_ISMS_METHOD:   
          case USAGE_SMS_STORE_STATUS:   
          case USAGE_SMS_STORE_ENUM_READ:   
             // Deactivate main menu to turn off horizontal scrolling of long menu items   
             IMENUCTL_SetActive(pMe->m_pIMenu, FALSE);   
   
             // Execute the usage example the user has selected   
             ISmsAppUsage (pMe, wParam);   
             return TRUE;   
                
          default:   
            return FALSE;   
      }   
    default:   
      break;   
   }   
   return FALSE;   
}   
   
   
/*===========================================================================  
  
FUNCTION ISmsApp_InitAppData  
  
DESCRIPTION  
   This function initializes app specific data.  
  
PROTOTYPE:  
  static boolean ISmsApp_InitAppData(IApplet* pi);  
  
PARAMETERS:  
  pi [in]: Pointer to the IApplet structure.  
  
DEPENDENCIES  
    None.  
  
RETURN VALUE  
    TRUE: If the app has app data is allocated and initialized successfully  
    FALSE: Either app data could not be allocated or initialized  
  
SIDE EFFECTS  
    None.  
===========================================================================*/   
static boolean ISmsApp_InitAppData(IApplet* pi)   
{   
   ISmsApp * pMe = (ISmsApp*)pi;   
   int charHeight, pnAscent, pnDescent;   
   AEEDeviceInfo di;   
   int nErr;   
   int i;   
   AECHAR szwStr[32];   
   char szNumber[32];   
   
   #define MAX_ENC 32   
   uint32 *EncodingMoSms;   
   uint32 nSize;   
   
   IModel * pIModel = NULL;   
   // Make sure the pointers we'll be using are valid   
   if (pMe == NULL || pMe->a.m_pIShell == NULL)   
     return FALSE;   
   
   pMe->m_pIMenu = NULL;   
   pMe->m_pISMSMsg = NULL;   
   pMe->m_pISMS      = NULL;   
   pMe->m_pISMSStorage = NULL;   
   
   
   
   // Determine the amount of available screen space   
   ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);   
   
   // Determine the height of a line of text   
   charHeight = IDISPLAY_GetFontMetrics (pMe->a.m_pIDisplay, AEE_FONT_NORMAL,   
      &pnAscent, &pnDescent);   
   
   // Number of available lines equals the available screen   
   // space divided by the height of a line, minus 3 for the   
   // lines we always print at the top and bottom of the screen   
   pMe->m_cMaxLine = (di.cyScreen / charHeight) - 3;   
   
    nErr =ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SMS, (void **)&pMe->m_pISMS);   
    DBGPRINTF("CreateInstance of AEECLSID_SMS ret %d", nErr);   
    if(nErr != AEE_SUCCESS)   
    {   
        return FALSE;   
    }   
   
   if ((ISMS_GetEncodingsAvailableForMOSMS(pMe->m_pISMS, NULL, (uint32*)&nSize) == SUCCESS) &&   
       ((EncodingMoSms = (uint32*)MALLOC(nSize)) != NULL) &&   
       (ISMS_GetEncodingsAvailableForMOSMS(pMe->m_pISMS, EncodingMoSms, (uint32*)&nSize) == SUCCESS))   
   {   
      nSize = nSize/sizeof(uint32);         
      DBGPRINTF("ISMS_GetEncodingsAvailableForMOSMS");   
      DBGPRINTF("size Encode ret:%d", nSize);   
        for(i=0; i<nSize; i++)   
        {   
            DBGPRINTF("en[%d]=%x", i, EncodingMoSms[i]);   
        }   
   }   
   
  if ((nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SMSSTORAGE, (void**)&pMe->m_pISMSStorage)) != SUCCESS)   
  {   
    DBGPRINTF("CreateInstance SMSSTORAGE ret %d", nErr);   
    return FALSE;   
  }   
   
  if (pMe->m_pISMSStorage &&   
     (SUCCESS == ISMSSTORAGE_QueryInterface(pMe->m_pISMSStorage, AEEIID_MODEL, (void**)&pIModel)))   
   {   
      IMODEL_AddListenerEx(pIModel, &pMe->m_SMSStorageModelListener, (PFNLISTENER)OATSMSStorage_ModelListener, pMe);   
      IMODEL_Release(pIModel);   
      pIModel = NULL;   
   }   
   
   ISHELL_LoadResString(pMe->a.m_pIShell, ISMS_RES_FILE, IDS_SMS_TAG, szwStr, sizeof(szwStr));   
   WSTRTOSTR(szwStr, szNumber, sizeof(szNumber));   
   pMe->m_tag = STRTOUL(szNumber, NULL, 10);   
   
   ISHELL_LoadResString(pMe->a.m_pIShell, ISMS_RES_FILE, IDS_SMS_MT, szwStr, sizeof(szwStr));   
   WSTRTOSTR(szwStr, szNumber, sizeof(szNumber));   
   pMe->m_mt = STRTOUL(szNumber, NULL, 10);   
      
   return TRUE;   
}   
   
/*===========================================================================  
  
FUNCTION ISmsApp_FreeAppData  
  
DESCRIPTION  
    Frees any data and interfaces used by this app.  
  
PROTOTYPE:  
  static void ISmsApp_FreeAppData(IApplet* pi);  
  
PARAMETERS:  
  pi [in]: Pointer to the IApplet structure.  
  
DEPENDENCIES  
    None.  
  
RETURN VALUE  
    None.  
  
SIDE EFFECTS  
    None.  
===========================================================================*/   
static void ISmsApp_FreeAppData(IApplet* pi)   
{   
   ISmsApp * pMe = (ISmsApp*)pi;   
   
   CALLBACK_Cancel(&pMe->m_cb);   
   
   if(pMe->m_pIMenu != NULL)   
   {   
     IMENUCTL_Release(pMe->m_pIMenu);   
     pMe->m_pIMenu = NULL;   
   }   
   
    if(pMe->m_pISMSMsg != NULL)   
    {   
        ISMSMSG_Release(pMe->m_pISMSMsg);   
        pMe->m_pISMSMsg = NULL;   
    }   
   
    if(pMe->m_pISMS != NULL)   
    {   
        ISMS_Release(pMe->m_pISMS);   
        pMe->m_pISMS = NULL;   
    }   
   
}   
   
/*===============================================================================  
                     APP HELPER FUNCTION DEFINITIONS  
=============================================================================== */   
/*===========================================================================  
  
FUNCTION: IFileUsage  
  
DESCRIPTION  
    This function executes the usage example selected from the main menu by the  
  user.  It takes as input the app data pointer and the ID of the example the  
  user selected.  After clearing the screen, the selected example is executed.  
  Each example is a self-contained code block that contains any local variables  
  needed to execute the example.  Each code block also creates and releases any  
  BREW interfaces that are required, and will exit if any errors are detected.    
  Once the example has been executed, this function prints a heading at the top   
  of the screen to indicate which example was selected,   
  and a message at the bottom of the screen instructing the user how to return to  
  the main menu to execute another example.  
      
PROTOTYPE:  
   static void IFileUsage(ISmsApp * pMe, uint16 wParam)  
  
PARAMETERS:  
   pMe:   [in]: Contains a pointer to the usage app data structure.  
   wParam: [in]: Identifier of the selected example (example IDs are specified when the  
                 main usage app menu is created).  
  
DEPENDENCIES  
  None  
  
RETURN VALUE  
  None  
  
SIDE EFFECTS  
  None  
  
===========================================================================*/   
static void ISmsAppUsage (ISmsApp * pMe, uint16 wParam)   
{   
   
       
   // Make sure the pointers we'll be using are valid   
   if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->a.m_pIDisplay == NULL)   
     return;   
   
   // Erase screen first for display output purposes.   
   DisplayEvent (pMe, USAGE_ERASE_SCREEN);   
  IMENUCTL_SetProperties(pMe->m_pIMenu, MP_NO_REDRAW);   
  IMENUCTL_SetActive(pMe->m_pIMenu, FALSE);   
   
   // Initialize line number where we will start the output    
   // of each test   
   pMe->m_cLineNum = 1;   
   
   switch (wParam)   
   {   
        case USAGE_SMS_TX_ASCII:   
            {   
                int nErr;   
                WebOpt awo[6]; /* ***IMPORTANT**** grow this if you add more   
                                 WebOpts here, or shrink it and call AddOpt() multiple times */   
                int    i = 0;   
                uint32 nReturn=0;                                    
   
   
                nErr =ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SMSMSG, (void **)&pMe->m_pISMSMsg);   
                DBGPRINTF("CreateInstance of AEECLSID_SMSMSG ret %d", nErr);   
                if(nErr != AEE_SUCCESS)   
                {   
                    break;   
                }   
   
                /* NULL terminated string providing destination device number.   
                    '+' as first character signifies international number.  */   
                awo[i].nId  = MSGOPT_TO_DEVICE_SZ ;   
                awo[i].pVal = (void *)DESTINATION_NUMBER;   
                i++;   
   
                /* ascii text to be send */   
                awo[i].nId  = MSGOPT_PAYLOAD_SZ ;   
                awo[i].pVal = (void *)MO_TEXT_ASCII;   
                i++;   
   
                /* encoding */   
                awo[i].nId  = MSGOPT_PAYLOAD_ENCODING;   
                awo[i].pVal = (void *)AEE_ENC_ISOLATIN1;   
                i++;   
   
                awo[i].nId  = MSGOPT_MOSMS_ENCODING;   
                awo[i].pVal = (void *)AEESMS_ENC_ASCII;   
                i++;   
#if 0   
                /* user ack */   
                awo[i].nId  = MSGOPT_USER_ACK;   
                awo[i].pVal = (void *)TRUE;   
                i++;   
#endif   
                /* this is absolutely necessary, do not remove, marks the end of the   
                   array of WebOpts */   
                awo[i].nId  = WEBOPT_END;   
                 
                /* add 'em */   
                nErr =ISMSMSG_AddOpt(pMe->m_pISMSMsg, awo);   
                DBGPRINTF("ISMSMSG_AddOpt ret %d", nErr);   
   
                CALLBACK_Init(&pMe->m_cb, SMSCallBack_Send, pMe);   
                ISMS_SendMsg(pMe->m_pISMS, pMe->m_pISMSMsg, &pMe->m_cb, &nReturn);   
   
                // Higher 16 bits specify error type specified as AEESMS_ERRORTYPE_XXX    
                // lower  16 bits specify error specified as AEESMS_ERROR_XXX   
                DBGPRINTF("ISMS_SendMsg ret 0x%x", nReturn);   
            }   
            break;   
   
        case USAGE_SMS_TX_UNICODE:   
            {   
                int nErr;   
                WebOpt awo[6]; /* ***IMPORTANT**** grow this if you add more   
                                 WebOpts here, or shrink it and call AddOpt() multiple times */   
                int    i = 0;   
                uint32 nReturn=0;                                    
                AECHAR pszBuf[100];   
   
                nErr =ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SMSMSG, (void **)&pMe->m_pISMSMsg);   
                DBGPRINTF("CreateInstance of AEECLSID_SMSMSG ret %d", nErr);   
                if(nErr != AEE_SUCCESS)   
                {   
                    break;   
                }   
   
                /* NULL terminated string providing destination device number.   
                    '+' as first character signifies international number.  */   
                awo[i].nId  = MSGOPT_TO_DEVICE_SZ ;   
                awo[i].pVal = (void *)DESTINATION_NUMBER;   
                i++;   
   
                /* unicode text to be send */   
                awo[i].nId  = MSGOPT_PAYLOAD_WSZ ;   
                ISHELL_LoadResString(pMe->a.m_pIShell, ISMS_RES_FILE, IDS_MO_TEXT_UNICODE, pszBuf, sizeof(pszBuf));   
                awo[i].pVal = (void *)pszBuf;   
                i++;   
   
                /* encoding */   
                awo[i].nId  = MSGOPT_PAYLOAD_ENCODING;   
                awo[i].pVal = (void *)AEE_ENC_UNICODE ;   
                i++;   
   
                awo[i].nId  = MSGOPT_MOSMS_ENCODING;   
                awo[i].pVal = (void *)AEESMS_ENC_UNICODE;   
                i++;   
   
#if 0   
                /* user ack */   
                awo[i].nId  = MSGOPT_READ_ACK;   
                awo[i].pVal = (void *)TRUE;   
                i++;   
#endif   
   
                /* this is absolutely necessary, do not remove, marks the end of the   
                   array of WebOpts */   
                awo[i].nId  = MSGOPT_END;   
                 
                /* add 'em */   
                nErr =ISMSMSG_AddOpt(pMe->m_pISMSMsg, awo);   
                DBGPRINTF("ISMSMSG_AddOpt ret %d", nErr);   
   
                CALLBACK_Init(&pMe->m_cb, SMSCallBack_Send, pMe);   
                ISMS_SendMsg(pMe->m_pISMS, pMe->m_pISMSMsg, &pMe->m_cb, &nReturn);   
   
                // Higher 16 bits specify error type specified as AEESMS_ERRORTYPE_XXX    
                // lower  16 bits specify error specified as AEESMS_ERROR_XXX   
                DBGPRINTF("ISMS_SendMsg ret 0x%x", nReturn);   
            }   
            break;   
   
        case USAGE_SMS_TX_UTF8:   
            {   
                int nErr;   
                WebOpt awo[6]; /* ***IMPORTANT**** grow this if you add more   
                                 WebOpts here, or shrink it and call AddOpt() multiple times */   
                int    i = 0;   
                uint32 nReturn=0;                                    
                char utf8_Buf_Sending[302];   
                uint16 count;   
                // in utf-8 "数组19英文English混合" with heading of two bytes for length -0x0010   
                // The length is little-endian, low byte first.   
               char cTextUTF8[142]={0xFF, 0xFE,    
                                                    0x70, 0x65,0x57, 0x5B, 0x31, 0x0, 0x39, 0x0, 0xF1, 0x82, 0x87, 0x65, 0x45, 0x0,   
                                                    0x6E, 0x0,  0x67, 0x0, 0x6c, 0x0, 0x69, 0x0, 0x73, 0x0, 0x68, 0x0, 0xF7, 0x6D, 0x08, 0x54, 0x0, 0x0 };   
               //char cTextUTF8[142]={0x1e, 0x0,    
               //                                    0xEF, 0xBB, 0xBF, 0xE6,0x95, 0xB0, 0xE7, 0xBB, 0x84, 0x31, 0x39, 0xE8, 0x8B, 0x8B, 0xB1, 0xE6, 0x96,   
               //                                     0x87, 0x45,  0x6E, 0x67, 0x6c, 0x69, 0x73, 0x68, 0xE6, 0xB7, 0xB7, 0xE5, 0x90, 0x88 };   
   
                nErr =ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SMSMSG, (void **)&pMe->m_pISMSMsg);   
                DBGPRINTF("CreateInstance of AEECLSID_SMSMSG ret %d", nErr);   
                if(nErr != AEE_SUCCESS)   
                {   
                    break;   
                }   
   
                /* NULL terminated string providing destination device number.   
                    '+' as first character signifies international number.  */   
                awo[i].nId  = MSGOPT_TO_DEVICE_SZ ;   
                awo[i].pVal = (void *)DESTINATION_NUMBER;   
                i++;   
   
                /* unicode text to be send */   
                awo[i].nId  = MSGOPT_PAYLOAD_BINARY;   
                MEMSET(utf8_Buf_Sending, 0, sizeof(utf8_Buf_Sending));   
                WSTRTOUTF8((AECHAR *)cTextUTF8, WSTRLEN((AECHAR *)cTextUTF8), (byte *)&(utf8_Buf_Sending[2]), 300);   
                count = STRLEN(&(utf8_Buf_Sending[2]));   
                DBGPRINTF("WLen(%d)===> count=%d", WSTRLEN((AECHAR *)cTextUTF8),count);   
                                    DBGPRINTF("0x%2x, %2x, %2x, %2x", utf8_Buf_Sending[0], utf8_Buf_Sending[1], utf8_Buf_Sending[2], utf8_Buf_Sending[3] );   
                                    DBGPRINTF("0x%2x, %2x, %2x, %2x", utf8_Buf_Sending[4], utf8_Buf_Sending[5], utf8_Buf_Sending[6], utf8_Buf_Sending[7] );   
                                    DBGPRINTF("0x%2x, %2x, %2x, %2x", utf8_Buf_Sending[8], utf8_Buf_Sending[9], utf8_Buf_Sending[10], utf8_Buf_Sending[11] );   
                                    DBGPRINTF("0x%2x, %2x, %2x, %2x", utf8_Buf_Sending[12], utf8_Buf_Sending[13], utf8_Buf_Sending[14], utf8_Buf_Sending[15] );   
                                    DBGPRINTF("0x%2x, %2x, %2x, %2x", utf8_Buf_Sending[16], utf8_Buf_Sending[17], utf8_Buf_Sending[18], utf8_Buf_Sending[19] );   
                utf8_Buf_Sending[0]=count & 0xff;   
                utf8_Buf_Sending[1]=(count>>8) & 0xff;   
                awo[i].pVal = (void *)utf8_Buf_Sending;   
                i++;   
   
                /* encoding */   
                awo[i].nId  = MSGOPT_PAYLOAD_ENCODING;   
                awo[i].pVal = (void *)AEE_ENC_UTF8 ;   
                i++;   
   
                awo[i].nId  = MSGOPT_MOSMS_ENCODING;   
                awo[i].pVal = (void *)AEESMS_ENC_OCTET;   
                i++;   
   
#if 0   
                /* user ack */   
                awo[i].nId  = MSGOPT_READ_ACK;   
                awo[i].pVal = (void *)TRUE;   
                i++;   
#endif   
   
                /* this is absolutely necessary, do not remove, marks the end of the   
                   array of WebOpts */   
                awo[i].nId  = MSGOPT_END;   
                 
                /* add 'em */   
                nErr =ISMSMSG_AddOpt(pMe->m_pISMSMsg, awo);   
                DBGPRINTF("ISMSMSG_AddOpt ret %d", nErr);   
   
                CALLBACK_Init(&pMe->m_cb, SMSCallBack_Send, pMe);   
                ISMS_SendMsg(pMe->m_pISMS, pMe->m_pISMSMsg, &pMe->m_cb, &nReturn);   
   
                // Higher 16 bits specify error type specified as AEESMS_ERRORTYPE_XXX    
                // lower  16 bits specify error specified as AEESMS_ERROR_XXX   
                DBGPRINTF("ISMS_SendMsg ret 0x%x", nReturn);   
            }   
            break;   
   
       case USAGE_SMS_RX_TAPI_METHOD:   
            {   
                // 使用TAPI的notify机制接收短信   
                AEECLSID clsNotifier= AEECLSID_TAPI; // Event sender   
                AEECLSID clsNotify = AEECLSID_ISMS;  // Event listener   
                // Set the notify event to be NOTIFY_MASK_EVENT. This event is used   
                // as an example.   
                uint32 dwMask = NMASK_TAPI_SMS_TEXT;    
                int nErr;   
   
                // Register a notification    
                nErr = ISHELL_RegisterNotify (pMe->a.m_pIShell, clsNotify, clsNotifier, dwMask);   
                DBGPRINTF("ISHELL_RegisterNotify ret %d", nErr);   
            }   
            
            break;   
   
        case USAGE_SMS_RX_ISMS_METHOD:   
            {   
                // 使用新的AEECLSID_SMSNOTIFIER 的notify机制接收短信   
                AEECLSID clsNotifier= AEECLSID_SMSNOTIFIER; // Event sender   
                AEECLSID clsNotify = AEECLSID_ISMS; // Event listener   
                // Set the notify event to be NOTIFY_MASK_EVENT. This event is used   
                // as an example.   
                uint32 dwMask = ((AEESMS_TYPE_TEXT << 16) | NMASK_SMS_TYPE);    
                int nErr;   
   
                // Register a notification    
                nErr = ISHELL_RegisterNotify (pMe->a.m_pIShell, clsNotify, clsNotifier, dwMask);   
                DBGPRINTF("ISHELL_RegisterNotify ret %d", nErr);   
            }   
            break;   
   
        case USAGE_SMS_STORE_STATUS:   
            {   
               CALLBACK_Cancel(&pMe->m_cb);   
               CALLBACK_Init(&pMe->m_cb, OATSMS_StoreReadMsgStorageStatusCb, (void*)pMe);   
   
               ISMSSTORAGE_GetStorageStatus(pMe->m_pISMSStorage,    
                                            pMe->m_mt,    
                                            pMe->m_tag,    
                                            &pMe->m_cb,    
                                            &pMe->m_ss,    
                                            &pMe->m_dwStatus);   
           }   
           break;   
               
        case USAGE_SMS_STORE_ENUM_READ:   
            {   
              CALLBACK_Cancel(&pMe->EnumMsgInitCb);   
              CALLBACK_Init(&pMe->EnumMsgInitCb, OATSMS_EnumMsgInitCb, (void*)pMe);   
              ISMSSTORAGE_EnumMsgInit(pMe->m_pISMSStorage,    
                    pMe->m_mt,    
                    pMe->m_tag,    
                    &pMe->EnumMsgInitCb,    
                    &pMe->m_dwStatus);   
            }   
           break;   
               
        default:   
             return;   
    }   
               
    // Display above event.    
    DisplayEvent (pMe, wParam);   
   
    return;   
}   
   
/*===========================================================================  
  
FUNCTION: DisplayEvent  
  
DESCRIPTION  
    This function prints a heading at the top   
  of the screen to indicate which usage example was selected by the user,   
  It also prints a message on the bottom two lines of the screen instructing   
  the user to press the UP or DOWN key to return to  
  the main menu to execute another example.  
      
PROTOTYPE:  
   static void DisplayEvent(ISmsApp * pMe, uint16 wParam)  
  
PARAMETERS:  
   pMe:   [in]: Contains a pointer to the usage app data structure.  
   wParam: [in]: Identifier of the selected example (example IDs are specified when the  
                 main usage app menu is created).  If the value USAGE_ERASE_SCREEN is supplied  
         instead, this function clears the screen.  
  
DEPENDENCIES  
  None  
  
RETURN VALUE  
  None  
  
SIDE EFFECTS  
  None  
  
===========================================================================*/   
static void DisplayEvent (ISmsApp *pMe, uint16 wParam)   
{   
   AEERect qrc;   
   AEEDeviceInfo di; // Device Info   
   AECHAR * szBuf;   
   AEERect rc;    
   
   // Make sure the pointers we'll be using are valid   
   if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->a.m_pIDisplay == NULL)   
     return;   
   
   if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)   
     return;   
   
   ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);   
   qrc.x = 0;   
   qrc.y = 0;   
   qrc.dx = di.cxScreen;   
   qrc.dy = di.cyScreen;   
   rc.x = 5;   
   rc.y = 5;   
   rc.dx = di.cxScreen - 5;   
   rc.dy = di.cyScreen - 5;   
   
   // Specify text for heading based on example the user has selected   
   switch (wParam)   
   {   
      case USAGE_ERASE_SCREEN:   
         IDISPLAY_EraseRect(pMe->a.m_pIDisplay,&qrc);  //Clear the display   
         FREE(szBuf);   
         return;   
   
      case USAGE_SMS_TX_ASCII:   
         STR_TO_WSTR("SMS TX ASCII", szBuf, TEXT_BUFFER_SIZE);   
         break;    
   
      case USAGE_SMS_TX_UNICODE:   
         STR_TO_WSTR("SMS TX UNICODE", szBuf, TEXT_BUFFER_SIZE);   
         break;    
   
      case USAGE_SMS_TX_UTF8:   
         STR_TO_WSTR("SMS TX UTF-8", szBuf, TEXT_BUFFER_SIZE);   
         break;    
   
      case USAGE_SMS_RX_TAPI_METHOD:   
         STR_TO_WSTR("SMS RX in TAPI method", szBuf, TEXT_BUFFER_SIZE);   
         break;    
   
      case USAGE_SMS_RX_ISMS_METHOD:   
         STR_TO_WSTR("SMS RX in ISMS method", szBuf, TEXT_BUFFER_SIZE);   
         break;    
   
      case USAGE_SMS_STORE_STATUS:   
         STR_TO_WSTR("SMS Store Status", szBuf, TEXT_BUFFER_SIZE);   
         break;    
   
      case USAGE_SMS_STORE_ENUM_READ:   
         STR_TO_WSTR("SMS Store Read", szBuf, TEXT_BUFFER_SIZE);   
         break;    
   
      default:   
         STR_TO_WSTR("Unsupported fn", szBuf, TEXT_BUFFER_SIZE);   
   }   
   
   // Print the heading at the top of the screen   
   IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szBuf, -1,10, 5, &rc,   
   IDF_ALIGN_NONE|  IDF_TEXT_TRANSPARENT);   
   
   // Print a message at the bottom of the screen telling the user   
   // how to return to the main app menu   
   STR_TO_WSTR("Press Up/Down", szBuf, TEXT_BUFFER_SIZE);   
   IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szBuf, -1,10, qrc.dy - 25, &rc,   
     IDF_ALIGN_NONE | IDF_TEXT_TRANSPARENT);   
   STR_TO_WSTR("keys to continue..", szBuf, TEXT_BUFFER_SIZE);   
   IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szBuf, -1,10, qrc.dy - 15, &rc,   
     IDF_ALIGN_NONE | IDF_TEXT_TRANSPARENT);   
   IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,TRUE);   
   
   FREE(szBuf);   
}   
/*===========================================================================  
  
FUNCTION: BuildMainMenu  
  
DESCRIPTION  
    This function constructs the menu control that allows the user to  
  select a usage example to execute.  The command ID for each menu item  
  is passed to the usage example function above when the user selects the  
  menu item for that example.  
      
PROTOTYPE:  
   static void BuildMainMenu(ISmsApp * pMe)  
  
PARAMETERS:  
   pMe:   [in]: Contains a pointer to the usage app data structure.  
  
DEPENDENCIES  
  None  
  
RETURN VALUE  
  None  
  
SIDE EFFECTS  
  None  
  
===========================================================================*/   
static void BuildMainMenu(ISmsApp *pMe)   
{   
   AEERect qrc;   
   AEEDeviceInfo di;   
   AECHAR * szBuf;   
   
   // Make sure the pointers we'll be using are valid   
   if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->m_pIMenu == NULL)   
     return;   
   
   if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)   
     return;   
   
   // Set Title   
   STR_TO_WSTR("ISMS & ISMSMsg Example", szBuf, TEXT_BUFFER_SIZE);   
   IMENUCTL_SetTitle(pMe->m_pIMenu, NULL, 0, szBuf);   
   
   //Set the Rectangle for the Menu   
   ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);   
   qrc.x = 0;   
   qrc.y = 0;   
   qrc.dx = di.cxScreen;   
   qrc.dy = di.cyScreen;   
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);     
   
   //Add individual entries to the Menu   
   
   // Add SMS Sending to menu   
   STR_TO_WSTR("1. SMS Tx ASCII", szBuf, TEXT_BUFFER_SIZE);   
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_SMS_TX_ASCII, szBuf, 0);   
   
   STR_TO_WSTR("2. SMS Tx UNICODE", szBuf, TEXT_BUFFER_SIZE);   
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_SMS_TX_UNICODE, szBuf, 0);   
   
   STR_TO_WSTR("3. SMS Tx UTF-8", szBuf, TEXT_BUFFER_SIZE);   
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_SMS_TX_UTF8, szBuf, 0);   
   
   // Add SMS Receive  in TAPI method to menu   
   STR_TO_WSTR("4. Rx SMS in method TAPI ", szBuf, TEXT_BUFFER_SIZE);   
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_SMS_RX_TAPI_METHOD, szBuf, 0);   
   
   // Add SMS Receive  in ISMS method to menu   
   STR_TO_WSTR("5. Rx SMS in method ISMSNotifier", szBuf, TEXT_BUFFER_SIZE);   
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_SMS_RX_ISMS_METHOD, szBuf, 0);   
   
   // SMS storage status   
   STR_TO_WSTR("6. SMS storage status", szBuf, TEXT_BUFFER_SIZE);   
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_SMS_STORE_STATUS, szBuf, 0);   
   
   // read SMS from store   
   STR_TO_WSTR("7. SMS store read", szBuf, TEXT_BUFFER_SIZE);   
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_SMS_STORE_ENUM_READ, szBuf, 0);   
   
   // Activate menu   
   IMENUCTL_SetActive(pMe->m_pIMenu,TRUE);   
   
   FREE(szBuf);   
}   
/*===========================================================================  
  
FUNCTION: DisplayOutput  
  
DESCRIPTION  
    This function displays an output string at a given line number on the  
    screen. If the nline parameter is a negative value (-1) the string  
    is displayed in the middle of the screen. If the "nline" value is larger  
    than or equal to zero the "nline" value is multiplied by 15 and the   
    resulting value in pixels is set to the y-coordinate of the start of   
    the string display on the screen. If the string does not fit on one line  
    the string wraps around to the next line (spaced rougly 10-15 pixels apart).  
    By default 5 is used as the starting the x-coordinate of a displayed   
    string.  
  
    How many characters that fit on one line is calculated for each line   
    that is wrapped around to the next line.  
  
    Note: depending on the phone screen size and the fonts used for characters   
          the output might differ on different handsets (devices). Where some   
          handsets will have a smaller screen and large default fonts which will   
          cause partial overlapping of lines. This function does not try to address  
          these issues (this is meant as a simple display function).  
      
PROTOTYPE:  
   int DisplayOutput(IShell *pIShell, IDisplay *pDisplay, int nline, char *pszStr, AECHAR *pszwStr, boolean isWideChar)  
  
PARAMETERS:  
   pIShell:   [in]: Contains a pointer to the IShell interface.  
   pIDisplay: [in]: Contains a pointer to the IDisplay interface.  
   nline:     [in]: Contains the line number to start displaying the text. The line  
        numbers are by default spaced 15 pixels apart along the y-axis.  
   pszStr:    [in]: The character string to be displayed on the screen.  
  
DEPENDENCIES  
  None  
  
RETURN VALUE  
  Number of lines written to the screen.  
  
SIDE EFFECTS  
  None  
  
===========================================================================*/   
static int DisplayOutput(IShell *pIShell, IDisplay *pIDisplay, int nline, char *pszStr, AECHAR *pszwStr, boolean isWideChar)   
{   
   AEEDeviceInfo di; // Device Info   
   AECHAR * szBuf;     // a buffer that supports 200 char string   
   AECHAR * psz = NULL;   
   int charHeight = 0;      // Stores the char height in pixels for given font   
   int pnAscent = 0;        // Stores the ascent in number of pixels   
   int pnDescent = 0;       // Stores the descent in number of pixels   
   int pixelWidth;   
   AEEFont font = AEE_FONT_NORMAL;   
   int pnFits = 0, dy;   
   int totalCh = 0;   
   int numLinesPrinted = 0;   
   
   // Make sure the pointers we'll be using are valid   
   if (pIShell == NULL || pIDisplay == NULL)   
     return 0;   
   
   if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)   
     return 0;   
   
   // Get device information   
   ISHELL_GetDeviceInfo(pIShell,&di);   
   
   // Get the font metrics info   
   charHeight = IDISPLAY_GetFontMetrics (pIDisplay, AEE_FONT_NORMAL,   
      &pnAscent, &pnDescent);   
   if(isWideChar)   
   {   
        WSTRCPY(szBuf,pszwStr);   
   }   
   else   
   {   
    // Convert to wide string (unicode)   
    STR_TO_WSTR ((char *)pszStr, szBuf, TEXT_BUFFER_SIZE);   
   }   
   // If nlines is zero then print this string starting around the middle of    
   // the screen. Or else multiply nlines by charheight to decide the y coordinate of   
   // the start of the string.   
   if (nline < 0) {   
      dy = di.cyScreen*2/5;   
   }   
   else{   
      dy = nline * charHeight + 5;   
   }   
   
   // psz keeps track of the point from which to write from the string buffer   
   // in case the string does not fit one line and needs to wrap around in the   
   // next line.   
   psz = szBuf;   
         
   // Need to calculate the lotal string length to decide if any wrapping   
   // around is needed.   
   if(isWideChar)   
   {   
        totalCh = 2*WSTRLEN (pszwStr);   
   }   
   else   
   {   
        totalCh = STRLEN ((char *)pszStr);   
   }   
   
   // Keep displaying text string on multiple lines if the string can't be displayed   
   // on one single line. Lines are spaced 15 pixels apart.   
   while ((totalCh > 0) && (*psz != NULL))   
   {    
      // Get information on how many characters will fit in a line.   
      // Give the pointer to the buffer to be displayed, and the number of   
      // pixels along the x axis you want to display the string in (max number)   
      // pnFits will have the max number of chars that will fit in the maxWidth   
      // number of pixels (given string can't fit in one line), or the number of    
      // chars in the string (if it does fit in one line). pnWidth gives the   
      // number of pixels that will be used to display pnFits number of chars.   
      pixelWidth = IDISPLAY_MeasureTextEx(pIDisplay,   
                      font,    
                      (AECHAR *) psz,  // Start of the buffer to display,   
                      -1,   
                      di.cxScreen - 5, // maxWidth   
                      &pnFits);         // Number of chars that will fit a line   
   
      // If pnFits is zero there is something wrong in the input to above function.    
      // Normally this scenario should not occur. But, have the check anyway.   
      if (pnFits == 0)   
      {   
        FREE(szBuf);   
        return 0;   
      }   
   
      IDISPLAY_DrawText(pIDisplay, AEE_FONT_NORMAL, psz, pnFits, 5 /*start dx*/,    
         dy, 0 /* use default rectangle coordinates */, 0);   
   
      psz += pnFits;      // move pointer to the next segment to be displayed   
      totalCh -= pnFits;  // reduce the total number of characters to still display   
      dy += charHeight;   // Place next line charHeight pixels below the    
                          // previous line.   
      ++numLinesPrinted;   
   
      IDISPLAY_Update(pIDisplay); //, TRUE);   
      if (totalCh < pnFits)   
         pnFits = totalCh;  // if total number is less than pnFits, adjust pnFits   
   }   
   
   FREE(szBuf);   
   
   return numLinesPrinted;      
} // End of DisplayOutput   
   
/*===========================================================================  
  
FUNCTION: WriteLine  
  
DESCRIPTION  
    This function displays lines of text on the screen, taking into account  
  the number of text lines printed so far and the number of lines available  
  for output.  
  
    If all available lines are exhausted, this function returns without doing  
  anything.  
  
    If the last available line of the screen is reached, this function prints  
  "..." to indicate that some lines may not have been printed due to lack  
  of space.  
  
    Otherwise, this function prints the line on the screen by calling DisplayOutput,  
  and updates the count of lines printed based on how many lines DisplayOutput  
  used to print the text.  
   
      
PROTOTYPE:  
   static void WriteLine(ISmsApp * pMe,  char *pszStr, AECHAR *pszwStr, boolean isWideChar)  
  
PARAMETERS:  
   pMe:   [in]: Contains a pointer to this app's data structure.  
   pszStr:    [in]: The character string to be displayed on the screen.  
  
DEPENDENCIES  
  Assumes the pMe->m_cLineNum gets initialized to the starting line for text display  
  on the screen, and that pMe->m_cMaxLine contains the last available line for   
  output on the screen.  
  
RETURN VALUE  
  None.  
  
SIDE EFFECTS  
  None  
  
===========================================================================*/   
static void WriteLine(ISmsApp * pMe, char *pszStr, AECHAR *pszwStr, boolean isWideChar)   
{   
      
  // If all available lines used, return   
  if (pMe->m_cLineNum > pMe->m_cMaxLine)   
    return;   
   
  // If we've reached last available line, print an   
  // ellipsis indicator   
  if (pMe->m_cLineNum == pMe->m_cMaxLine)   
  {   
    DisplayOutput(pMe->a.m_pIShell, pMe->a.m_pIDisplay, pMe->m_cLineNum++, "...", NULL, FALSE);   
    return;   
  }   
   
  // There are lines available for printing.  Print the string passed as   
  // input and update the count of lines available   
  pMe->m_cLineNum += DisplayOutput(pMe->a.m_pIShell, pMe->a.m_pIDisplay, pMe->m_cLineNum, pszStr, pszwStr, isWideChar);   
  return;   
   
} // End of WriteLine   
   
static void SMSCallBack_Send(void *p)   
{   
   ISmsApp *pMe = (ISmsApp *)p;   
   DBGPRINTF("SMSCallBack_Send called");   
   
   // 短信发送完毕后，释放ISMSMsg，否则它将保留在其中，对下一条短信产生影响   
   if(pMe->m_pISMSMsg != NULL)   
   {   
       ISMSMSG_Release(pMe->m_pISMSMsg);   
       pMe->m_pISMSMsg = NULL;   
   }   
}   
       
//===========================================================================   
//   
// Callback for GetStorageStatus() call from OAT_StoreReadMsg   
//   
//===========================================================================   
static void OATSMS_StoreReadMsgStorageStatusCb(void * po)   
{   
   ISmsApp * pMe = (ISmsApp *)po;   
   
   if ( pMe->m_dwStatus == SUCCESS )    
   {   
      if ( pMe->m_ss.nFreeSlots )   
      {   
         /* Free slots available.  Resume store read as usual */   
         DBGPRINTF("free slots: %d", pMe->m_ss.nFreeSlots);   
         {   
             char buf[80];   
             SPRINTF(buf, "free slots: %d", pMe->m_ss.nFreeSlots);   
             WriteLine(pMe,  buf, NULL, FALSE);   
         }   
         /* used slots available.  Resume store read as usual */   
         DBGPRINTF("Usedslots: %d", pMe->m_ss.nUsedTagSlots);   
         {   
             char buf[80];   
             SPRINTF(buf, "Usedslots: %d", pMe->m_ss.nUsedTagSlots);   
             WriteLine(pMe,  buf, NULL, FALSE);   
         }   
      }   
      else   
      {   
          DBGPRINTF("Number of free slots needed for storage is: %d, cannot proceed with test.", pMe->m_ss.nFreeSlots);   
         {   
             char buf[120];   
             SPRINTF(buf, "Number of free slots needed for storage is: %d, cannot proceed with test.", pMe->m_ss.nFreeSlots);   
             WriteLine(pMe,  buf, NULL, FALSE);   
         }   
      }   
   }   
   else   
   {         
      DBGPRINTF("GetStorageStatus failed (nErr= %d)", pMe->m_dwStatus);    
         {   
             char buf[80];   
             SPRINTF(buf, "GetStorageStatus failed (nErr= %d)", pMe->m_dwStatus);    
             WriteLine(pMe,  buf, NULL, FALSE);   
         }   
   }   
}   
   
static void OATSMS_EnumMsgInitCb(void * po)   
{   
   ISmsApp * pMe = (ISmsApp *)po;   
   
   DBGPRINTF("OATSMS_EnumMsgInitCb");   
   
   pMe->m_pISMSMsg = NULL; // initial to save msg   
   if (SUCCESS == pMe->m_dwStatus)   
   {   
      CALLBACK_Cancel(&pMe->EnumMsgNextCb);   
      CALLBACK_Init(&pMe->EnumMsgNextCb, OATSMS_EnumMsgNextCb, (void*)pMe);   
      ISMSSTORAGE_EnumNextMsg(pMe->m_pISMSStorage,    
          pMe->m_mt,    
          &pMe->EnumMsgNextCb,   
          &pMe->m_dwIndex,    
          &pMe->m_pISMSMsg,    
          &pMe->m_dwStatus);   
   }   
   else   
   {         
      DBGPRINTF("EnumMsgInit failed (nErr=%d)",  pMe->m_dwStatus);   
   }   
}   
   
static void OATSMS_EnumMsgNextCb(void * po)   
{   
   int nErr = SUCCESS;   
   ISmsApp * pMe = (ISmsApp *)po;   
   
   DBGPRINTF("OATSMS_EnumMsgNextCb");   
   
   if (SUCCESS == (nErr = pMe->m_dwStatus))    
   {   
      showSMS(pMe, pMe->m_pISMSMsg);   
      pMe->m_pISMSMsg = NULL; // initial for next time   
      // no item left == mean ending   
      if (0xFFFFFFFF != pMe->m_dwIndex)   
      {   
         CALLBACK_Cancel(&pMe->EnumMsgNextCb);   
         CALLBACK_Init(&pMe->EnumMsgNextCb, OATSMS_EnumMsgNextCb, (void*)pMe);   
         ISMSSTORAGE_EnumNextMsg(pMe->m_pISMSStorage,    
             pMe->m_mt,    
             &pMe->EnumMsgNextCb,    
             &pMe->m_dwIndex,    
             &pMe->m_pISMSMsg,    
             &pMe->m_dwStatus);   
      }   
      else   
      {   
          DBGPRINTF("OATSMS_EnumMsgNextCb end");   
      }   
   }   
   else    
   {   
      DBGPRINTF("EnumMsgNextfailed (nErr=%d)",  pMe->m_dwStatus);   
   }   
}   
   
static void showSMS(ISmsApp * pMe,ISMSMsg *pRxMsg)   
{   
    SMSMsgOpt TmpOpt;   
    DBGPRINTF("showSMS");   
    if(pMe == NULL || pRxMsg == NULL)   
    {   
          DBGPRINTF("Null point");   
          return;   
    }   
                                // MSGOPT_FROM_DEVICE_SZ - 发送方号码   
                                if(ISMSMSG_GetOpt(pRxMsg, MSGOPT_FROM_DEVICE_SZ, &TmpOpt)==AEE_SUCCESS)   
                                {   
                                    DBGPRINTF("get MSGOPT_FROM_DEVICE_SZ");   
                                    WriteLine(pMe,  (char *)TmpOpt.pVal, NULL, FALSE);   
                                }   
   
                                // MSGOPT_PAYLOAD_SZ - ASCII的短信内容   
                                if(ISMSMSG_GetOpt(pRxMsg, MSGOPT_PAYLOAD_SZ, &TmpOpt)==AEE_SUCCESS)   
                                {   
                                    DBGPRINTF("get MSGOPT_PAYLOAD_SZ");   
                                    WriteLine(pMe,  (char *)TmpOpt.pVal, NULL, FALSE);   
                                }   
   
                                // MSGOPT_PAYLOAD_WSZ - UNICODE的短信内容   
                                if(ISMSMSG_GetOpt(pRxMsg, MSGOPT_PAYLOAD_WSZ, &TmpOpt)==AEE_SUCCESS)   
                                {   
                                    DBGPRINTF("get MSGOPT_PAYLOAD_WSZ");   
                                    WriteLine(pMe,  NULL, (AECHAR *)TmpOpt.pVal, TRUE);   
                                }   
   
                                // MSGOPT_TIMESTAMP - 短信时间标签   
                                if(ISMSMSG_GetOpt(pRxMsg, MSGOPT_TIMESTAMP, &TmpOpt)==AEE_SUCCESS)   
                                {   
                                    JulianType *pMyJulian;   
                                    char buf[80];   
                                    DBGPRINTF("get MSGOPT_TIMESTAMP");   
   
                                    pMyJulian = (JulianType *)TmpOpt.pVal;   
                                    SPRINTF(buf, "%d/%d/%d %d:%d:%d, weekday:%d", pMyJulian->wMonth, pMyJulian->wDay, pMyJulian->wYear,    
                                                                                              pMyJulian->wHour, pMyJulian->wMinute, pMyJulian->wSecond, pMyJulian->wWeekDay);   
                                    WriteLine(pMe,  buf, NULL, FALSE);   
                                }   
}   
   
//===========================================================================   
//   
// ModelListener for EVT_MDL_SMSSTORAGE_XXX   
//   
//===========================================================================   
static void OATSMSStorage_ModelListener(ISMSMsg * pMe, ModelEvent *pEvent)   
{    
   DBGPRINTF("OATSMSStorage_ModelListener");   
   
   if ((EVT_MDL_SMSSTORAGE_STORE == pEvent->evCode) ||   
       (EVT_MDL_SMSSTORAGE_UPDATE == pEvent->evCode) ||   
       (EVT_MDL_SMSSTORAGE_DELETE == pEvent->evCode) ||   
       (EVT_MDL_SMSSTORAGE_DELETE_TAG == pEvent->evCode) ||   
       (EVT_MDL_SMSSTORAGE_DELETE_ALL == pEvent->evCode) ||   
       (EVT_MDL_SMSSTORAGE_CLASS_0_AUTOSTORE == pEvent->evCode) )   
   {   
        DBGPRINTF("Event:%x", pEvent->evCode);   
   }   
}   
