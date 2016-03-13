#ifndef C_SAMPLESMS_H
#define C_SAMPLESMS_H
/*=====================================================================
FILE:  samplesms.h

AUTHOR:
   QUALCOMM Incorporated

SERVICES: 
   C_SAMPLESMS applet showing usage of BREW ISMS interfaces.

DESCRIPTION:
   This file provides definitions, typedefs and function prototypes
   for c_samplesms/c_samplesms.c


        Copyright ?2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/

/*===============================================================================
                        INCLUDES
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEStdLib.h"
#include "AEESMS.h"
#include "AEETAPI.h"
#include "AEETelephone.h"
#include "AEEMenu.h"
#include "AEEText.h"
#include "AEETime.h"
#include "AEEModel.h"
#include "AEEFile.h"
#include "AEEImage.h"

#include "c_samplesms.bid"
#include "c_samplesms_res.h"

/*===============================================================================
                        APPLICATION SPECIFIC EVENT
=============================================================================== */
#define EVT_SMSTEST_BASE    EVT_USER + 0x200
#define EVT_STSSTORAGE      EVT_SMSTEST_BASE
#define EVT_COUNTERS_READY  EVT_SMSTEST_BASE + 1
#define EVT_ENUM_TAG_DONE   EVT_SMSTEST_BASE + 2
#define EVT_SENDING_DONE    EVT_SMSTEST_BASE + 3

/*===============================================================================
                        CONSTANTS
=============================================================================== */
#define TIME_STRING            "%02u/%02u/%02u %02u:%02u"
#define MAX_DESTINATION_INDEX    2
#define MAX_SEGMENT              3
#define MAX_DEST_NUM_LENGTH     11
#define UDH_CONCAT_LENGTH        6
#define UDH_SM_PIC_ANIM_LENGTH  36   
#define UDH_LG_PIC_ANIM_LENGTH 132
#define BUFFER_LENGTH         1024
#define DEFAULT_CDMA_TAPI_SMS_ENC   AEE_ENC_ISOLATIN1
#define DEFAULT_GSM_TAPI_SMS_ENC    AEE_ENC_UNICODE

/*===============================================================================
                        APPLICATION STATE
=============================================================================== */
typedef enum {
   APPSTATE_PREPMAIN,
   APPSTATE_MAIN,         
   APPSTATE_INPUT_DEST1,
   APPSTATE_INPUT_DEST2,
   APPSTATE_INPUT_MSG,
   APPSTATE_INSERT_OBJ,
   APPSTATE_INBOX,
   APPSTATE_OUTBOX,
   APPSTATE_DRAFTS,
   APPSTATE_OPTIONS,
   APPSTATE_SET_MOENC,
   APPSTATE_READ_INBOX,
   APPSTATE_READ_OUTBOX,
   APPSTATE_EDIT_DRAFT,
   APPSTATE_SENDING,
   APPSTATE_MSG_SENT
   //APPSTATE_RECEIVING
} SMSAppState;

/*===============================================================================
                        APPLICATION DEFINED DATA TYPES
=============================================================================== */

typedef struct SMALLPIC {
   int position;
   byte data[32];
} SMALLPIC;

typedef struct LARGEPIC {
   int position;
   byte data[128];
} LARGEPIC;

typedef struct ems_udh_type {
   AEESMSUDHId      header_id;
   union {
      SMALLPIC   small_picture;                                       
      LARGEPIC   large_picture;
   } u;
   AECHAR   objName[32];
} ems_udh_type;


typedef struct {
   int   msgIndex;
   union {
      char srcNumber[12];      
      char destNumber[12];
   } number;
   union {
      char timeStamp[17];      // mm/dd/yyyy hh:mm
      char msgString[17];
   } msgAttribute;
   boolean bIsConcatMsg;
   unsigned char msgSeqNum;
   unsigned char msgTotal;
   unsigned char msgRefNum;
   ems_udh_type emsObj;
} MyMessage;


/*===============================================================================
   APPLET STRUCTURE
   All variables in here are reference via "pMe->"
=============================================================================== */

typedef struct _CSampleSMS {
   AEEApplet      a ;          
    AEEDeviceInfo  DeviceInfo; 
   
   SMSAppState          m_appstate;
   AEESMSStorageType    m_storageType;     // Storage type that app is dealing with
   AEESMSStorageStatus  m_storageSts;      // Place holder for storage status
   AEESMSTag            m_curTag;          // Sms tag that the app is currently operating on
                                           // -(enumeration, delete-tag, reading from)
   IDisplay             *m_pIDisplay;  
   IShell               *m_pIShell;    
   ISMS                 *m_pISMS;      
   ISMSMsg              *m_pISMSMsg;       // To encapsulate composed/sent message 
   ISMSMsg              *m_pISMSMsgRcv;    // To encapsulate received message
   ISMSStorage          *m_pISMSStorage;
   IStatic              *m_pIStatic;       // Static text-ctl to display message to the user
   IMemAStream          *m_pIMemAStream;   // To stream ems image data buffer  
   IImage               *m_pIImage;        // To display streamed ems data
   IMenuCtl             *m_pIMenuMain;       // Main Menu
   IMenuCtl             *m_pIMenuOptions;    // Options Menu
   IMenuCtl             *m_pIMenuInboxMsgs;  // InboxMsg list
   IMenuCtl             *m_pIMenuOutboxMsgs; // OutboxMsg list
   IMenuCtl             *m_pIMenuDrafts;     // Drafts list
   IMenuCtl             *m_pIMenuMOEncodings;   // MO Encoding options
   IMenuCtl             *m_pIMenuEMSObjects;    // EMS Objects list
   ITextCtl             *m_pITextDest1;         // Dest#1 input field
   ITextCtl             *m_pITextDest2;         // Dest#2 input field
   ITextCtl             *m_pITextMsg;           // Message input field
   AEECallback          m_callback;         // General callback
   AEECallback          m_cbStoreRcvd;      // Callback for storing received msg
   AEECallback          m_cbStorageSts;     // Callback for querying storage status
   AEESMSEncType        m_moEncoding;       // MO encoding used to send msg  
   int                  m_payloadEncoding;  // Payload encoding used to send msg
   uint32               *m_pPlEncodings;    // Supported payload encodings on a device
   int                  m_nPlEncodings;     // Number of supported payload encodings
   uint32               *m_pMOEncodings;    // MO encodings supported on a device
   int                  m_nMOEncodings;     // Number of supported MO encodings
   
   uint32               m_retVal;         // General place holder for error
   uint32               m_errStorageSts;  // Place holder for error on storage status query
   uint32               m_errStoreRcv;    // Place holder for error on storing received msg
   uint32               m_nStoreIndex;    // Store index when storing to the SMS Storage   
   uint16               m_curIndex;       // Msg index that the app is currently operating on
                                          // -(reading, deleting, updating)
   boolean              m_bGetCountersInProgress; // Whether app is querying 
                                                  // - message counters (Inbox/Outbox/Drafts)
   boolean              m_bInboxEnumerated;   // Whether Inbox has been previously enumerated
   boolean              m_bOutboxEnumerated;  // Whether Outbox has been previously enumerated
   boolean              m_bDraftsEnumerated;  // Whether Drafts has been previously enumerated
   boolean              m_bEnumerating;       // Whether the app is in enumeration process

   AECHAR               m_wszStringBuffer[BUFFER_LENGTH+1]; // General wide-string buffer 
   int                  m_nMaxPayloadBytes;  // Max payload bytes for one message segment
   int                  m_calculationFactor; // Calculation factor (depending on MO encoding)
                                             // - to determine message segmentation points
   unsigned char        m_totalMsg;          // Number of message segments to be sent
   unsigned char        m_seqNum;            // Current sequence number while sending 
                                             // - a multipart message
   unsigned char        m_refNum;            // Reference number used while sending
                                             // - a multipart message 
   int                  m_destIndex;         // Destination index to which 
                                             // - app is sending the message
   int                  m_nUnread;       // Number of unread message (inbox)
   int                  m_nRead;         // Number of read message (inbox)
   int                  m_nSent;         // Number or sent message (outbox)
   int                  m_nDraft;        // Number of drafts
   uint32               m_index;         // Message index variable used for
                                         // - enumeration and message listing
   JulianType           date;            // Timestamp of the composed/sent message
   ModelListener        m_SMSStorageListener;   // Model listener for SMS Storage         
   char                 *m_pReadMsgBuffer;      // Array containing currently read message   
   char                 *m_pszDestNum;   // Array containing current destination number
   char                 *m_pszMessage;   // Array containing message (string)   
   AECHAR               *m_pwszMessage;  // Array containing message (widestring)
   char                 *m_pText;        // Start pointer to the message (string)
                                         // - for current segment (in multipart msg)
   AECHAR               *m_pwText;       // Start pointer to the message (widestring)
                                         // - for current segment (in multipart msg)
   int                  m_nUnsentTextLength; // length of remaining message to send   
   boolean              m_bEditMode;   // Whether app is editing a draft
   ems_udh_type         m_emsObj;      // Inserted ems object data
   int                  m_objPos;      // Ems object position in the message
} CSampleSMS;


/*===============================================================================
                        FUNCTION PROTOTYPES
=============================================================================== */
int c_samplesms_CreateISMSMsgInstance(CSampleSMS *pMe);
void c_samplesms_BuildMainMenu(CSampleSMS *pMe);
void c_samplesms_ResetControls(CSampleSMS *pMe);
void c_samplesms_SetMOEncodingsMenu(CSampleSMS *pMe);
void c_samplesms_DrawSoftKey(CSampleSMS *pMe, int16 lskID, int16 rskID);
void c_samplesms_ShowStatus(CSampleSMS *pMe, char *pText);
void c_samplesms_AddMessageToList(CSampleSMS *pMe, MyMessage *msg, AEESMSTag msgTag);
void c_samplesms_DisplayMessage(CSampleSMS *pMe, MyMessage message);
void c_samplesms_EditMessage(CSampleSMS *pMe, MyMessage message);
void c_samplesms_MarkAsRead(CSampleSMS *pMe, uint16 itemID);

#endif
