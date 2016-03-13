/*===========================================================================
FILE: c_smsstorage.c

AUTHOR:
   QUALCOMM Incorporated

SERVICES: 
   C_SAMPLESMS applet showing usage of BREW ISMS interfaces.

DESCRIPTION:
   This sample application (C_SAMPLESMS) demonstrates:
   - Using ISMS to send messages encapsulated by ISMSMsg interface
   - Using ISMS to receive incoming message(s) by registering for notification 
   issued by ISMSNotifier (AEECLSID_SMSNOTIFIER).
   - Sending and receiving EMS utilizing User Data Header (UDH). 
   - SMSStorage for messages, including performing operation on messages 
   (store, delete, update, read). The application will also provide 
   message classification as inbox, outbox, or draft messages.

   This file represents the program for any operations on stored messages
   in SMSStorage, including getting storage status (for message counters), 
   read, storing, deleting, or updating a message.


                  Copyright © 2008 QUALCOMM Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "c_smsstorage.h"
#include "c_sendreceive.h"


/*===============================================================================
                        FUNCTION PROTOTYPES
=============================================================================== */
void c_samplesms_GetStorageStatusCB(CSampleSMS *pMe);
void c_samplesms_EnumMessages(CSampleSMS *pMe);
void c_samplesms_EnumMessageCB(CSampleSMS *pMe);
void c_samplesms_DeleteMessageCB(CSampleSMS *pMe);
void c_samplesms_DeleteMessageTagCB(CSampleSMS *pMe);
void c_samplesms_DeleteMessageAllCB(CSampleSMS *pMe);
void c_samplesms_ReadMessageCB(CSampleSMS *pMe);
void c_samplesms_StoreMessageCB(CSampleSMS *pMe);
void c_samplesms_StoreRcvdMessageCB(CSampleSMS *pMe);
void c_samplesms_UpdateMessageCB(CSampleSMS *pMe);

/*===========================================================================
FUNCTION c_samplesms_SetStorageType

DESCRIPTION
   This function sets storage type for use in every SMSStorage operations,
   Storage type will be set to either AEESMS_NV_CDMA or AEESMS_NV_GSM
   according to current active system mode.
 
PROTOTYPE:
   boolean c_samplesms_SetStorageType(CSampleSMS * pi);

RETURN VALUE
  TRUE: If the app can set storage type based on the active system mode 
  FALSE: If the app fails to set storage type based on the active system mode 
===========================================================================*/
boolean c_samplesms_SetStorageType(CSampleSMS *pMe) {
   ITelephone *pITelephone = NULL;
   AEETSSInfo sTSSInfo;
    
   MEMSET(&sTSSInfo, 0, sizeof(sTSSInfo));
   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_TELEPHONE, 
                        (void **)&pITelephone) != SUCCESS) {
      pITelephone = NULL;
      DBGPRINTF("Failed to create ITelephone instance..");
      return FALSE;
   }

   if (ITELEPHONE_GetServingSystemInfo(pITelephone, &sTSSInfo, sizeof(sTSSInfo)) != SUCCESS) {
      DBGPRINTF("Get serving system info failed");
       return FALSE;
    }

   switch (sTSSInfo.sys_mode) {
       case AEET_SYS_MODE_CDMA:
       case AEET_SYS_MODE_HDR:
         DBGPRINTF("storage type = AEESMS_NV_CDMA");
         pMe->m_storageType = AEESMS_NV_CDMA;
         break;

      case AEET_SYS_MODE_GSM:
      case AEET_SYS_MODE_WCDMA:
         DBGPRINTF("storage type = AEESMS_NV_GW");
         pMe->m_storageType = AEESMS_NV_GW;
         break;

      default:
         DBGPRINTF("Couldn't get valid system mode");
         return FALSE;
   }

   if (pITelephone) {
      ITELEPHONE_Release(pITelephone);
      pITelephone = NULL;
   }
   return TRUE;
}


/*===========================================================================
FUNCTION c_samplesms_GetMessageCounters

DESCRIPTION
   This function queries message counters for all message categories
   (Inbox, Outbox, Drafts), as needed information in Main Menu.
 
PROTOTYPE:
   void c_samplesms_GetMessageCounters(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_GetMessageCounters(CSampleSMS *pMe) {
   pMe->m_bGetCountersInProgress = TRUE;
   c_samplesms_GetStorageStatus(pMe, AEESMS_TAG_MT_NOT_READ);
}


/*===========================================================================
FUNCTION c_samplesms_GetStorageStatus

DESCRIPTION
   This function queries count messages of a specified message tag.
   If the query is executed in APPSTATE_PREP_MAIN, sequential queries will 
   be done for each storage category (Inbox/Outbox/Drafts).
 
PROTOTYPE:
   void c_samplesms_GetStorageStatus(CSampleSMS * pi, AEESMSTag msgTag);
===========================================================================*/
void c_samplesms_GetStorageStatus(CSampleSMS *pMe, AEESMSTag msgTag) {
   CALLBACK_Cancel(&pMe->m_cbStorageSts);
   CALLBACK_Init(&pMe->m_cbStorageSts, (PFNNOTIFY)c_samplesms_GetStorageStatusCB, pMe);
   DBGPRINTF("get storage sts of tag %d", (int) msgTag);
   
   pMe->m_curTag = msgTag;
   ISMSSTORAGE_GetStorageStatus(pMe->m_pISMSStorage, pMe->m_storageType, msgTag, 
      &pMe->m_cbStorageSts, &pMe->m_storageSts, &pMe->m_errStorageSts);
}

/*===========================================================================
FUNCTION c_samplesms_GetStorageStatus

DESCRIPTION
   This callback function fires after storage status of a specified tag
   (in this app, the info is used to get message counter) has been retrieved.
   This function will send an application specific user event EVT_STSSTORAGE
   notifying the application that the status/counter is available.
 
PROTOTYPE:
   void c_samplesms_GetStorageStatus(CSampleSMS * pi, AEESMSTag msgTag);
===========================================================================*/
void c_samplesms_GetStorageStatusCB(CSampleSMS *pMe) {
   DBGPRINTF("storage type = %d", pMe->m_storageType);
   DBGPRINTF("getstorage sts = %d", pMe->m_retVal);
   DBGPRINTF("nMaxSlots = %d", pMe->m_storageSts.nMaxSlots);
   DBGPRINTF("nFreeSlots = %d", pMe->m_storageSts.nFreeSlots);
   DBGPRINTF("nUsedTagSlots = %d", pMe->m_storageSts.nUsedTagSlots);

   if (pMe->m_errStorageSts == SUCCESS) {
      ISHELL_SendEvent(pMe->m_pIShell, AEECLSID_C_SAMPLESMS, 
         EVT_STSSTORAGE, 0, pMe->m_storageSts.nUsedTagSlots);
   }
   else {
      // Calling ISMSSTORAGE_GetStorageStatus may be unsuccessful
      // if the function was called before SMSStorage is not ready 
      c_samplesms_GetStorageStatus(pMe, pMe->m_curTag);
   }
}

/*===========================================================================
FUNCTION c_samplesms_AccessStorage

DESCRIPTION
   This function will initialize the storage enumeration of a specified 
   tag. 
 
PROTOTYPE:
   void c_samplesms_AccessStorage(CSampleSMS * pi, AEESMSTag msgTag);
===========================================================================*/
void c_samplesms_AccessStorage(CSampleSMS *pMe, AEESMSTag msgTag) {
   pMe->m_curTag = msgTag;
   pMe->m_bEnumerating = TRUE;
   CALLBACK_Cancel(&pMe->m_callback);
   CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY)c_samplesms_EnumMessages, pMe);   
   ISMSSTORAGE_EnumMsgInit(pMe->m_pISMSStorage, pMe->m_storageType, 
      pMe->m_curTag, &pMe->m_callback, &pMe->m_retVal);
}

/*===========================================================================
FUNCTION c_samplesms_EnumMessages

DESCRIPTION
   This function will retrieve information of next message on the specified
   storage type specified in c_samplesms_AccessStorage function 
   (ISMSSTORAGE_EnumMsgInit).
 
PROTOTYPE:
   void c_samplesms_EnumMessages(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_EnumMessages(CSampleSMS *pMe) {   
   if (SUCCESS == pMe->m_retVal) {
      if (pMe->m_pISMSMsg) {
         ISMSMSG_Release(pMe->m_pISMSMsg);
         pMe->m_pISMSMsg = NULL;
      }

      CALLBACK_Cancel(&pMe->m_callback);
      CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY)c_samplesms_EnumMessageCB, pMe);
      ISMSSTORAGE_EnumNextMsg(pMe->m_pISMSStorage, pMe->m_storageType, &pMe->m_callback, 
                                 &pMe->m_index, &pMe->m_pISMSMsg, &pMe->m_retVal);
   }
   else {
      DBGPRINTF("EnumInit failed!");
      DBGPRINTF("retval = %d", pMe->m_retVal);
      return;
   }
}

/*===========================================================================
FUNCTION c_samplesms_EnumMessageCB

DESCRIPTION
   This function will check whether EnumNextMsg function retrieve 
   a valid index. If so, the procedure is continued with getting the message
   details.

PROTOTYPE:
   void c_samplesms_EnumMessageCB(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_EnumMessageCB(CSampleSMS *pMe) {
   if (SUCCESS != pMe->m_retVal) {
      DBGPRINTF("EnumNextMsg failed!");
      DBGPRINTF("retval = %d", pMe->m_retVal);
   } else if (pMe->m_index == 0xFFFFFFFF) {
      ISHELL_SendEvent(pMe->m_pIShell, AEECLSID_C_SAMPLESMS, EVT_ENUM_TAG_DONE, 0, 0);
      DBGPRINTF("Enum tag-%d done!", pMe->m_curTag);   
   } else {
      c_samplesms_GetMessage(pMe, pMe->m_curTag);
   } 
}


/*===========================================================================
FUNCTION c_samplesms_GetMessage

DESCRIPTION
   This function will extract the information of a message, and
   to add message to appropriate list (Inbox/Outbox/Drafts)

PROTOTYPE:
   void c_samplesms_GetMessage(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_GetMessage(CSampleSMS *pMe, AEESMSTag msgTag) {
   SMSMsgOpt smo;
   JulianType *pJts;
   AEESMSTimestamp ts;
   MyMessage message;
   int nErr;
   int msgLen = 0;
   ISMSMsg *pISMSMsg;
   int msgTimestampOpt;

   if ((!pMe->m_bEnumerating) && (msgTag == AEESMS_TAG_MT_NOT_READ)) {
      pISMSMsg = pMe->m_pISMSMsgRcv;
   } else {
      pISMSMsg = pMe->m_pISMSMsg;
   }

   MEMSET(&message, 0, sizeof(MyMessage));

   message.msgIndex = pMe->m_index;
   
   // For an inbox message: retrieve msg sender number 
   if ( (msgTag == AEESMS_TAG_MT_NOT_READ) || (msgTag == AEESMS_TAG_MT_READ) ) {
      nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_FROM_DEVICE_SZ, &smo);      
      if (SUCCESS == nErr) {
         STRNCPY(message.number.srcNumber, (const char *) smo.pVal, STRLEN((const char *) smo.pVal) + 1);
         DBGPRINTF("msg from = %s", message.number.srcNumber);
      }
      else {
         STRCPY(message.number.srcNumber, "");
         DBGPRINTF("get msg opt error = %d", nErr);
      }
   }
      // For an MO (outbox / draft) message: retrieve msg destination number
   if ( (msgTag == AEESMS_TAG_MO_SENT) || (msgTag == AEESMS_TAG_MO_NOT_SENT) ) {
      nErr = ISMSMSG_GetOpt(pISMSMsg, MSGOPT_TO_DEVICE_SZ, &smo);      
      if (SUCCESS == nErr) {
         STRNCPY(message.number.destNumber, (const char *) smo.pVal, STRLEN((const char *) smo.pVal) + 1);
         DBGPRINTF("msg to = %s", message.number.destNumber);
      }
      else {
         STRCPY(message.number.destNumber, "");
         DBGPRINTF("destination number not specified");
      }
   }
      // For MT message: retrieve message timestamp
   if ( (msgTag == AEESMS_TAG_MT_READ) || (msgTag == AEESMS_TAG_MT_NOT_READ) )  {
      if (pMe->m_storageType == AEESMS_NV_CDMA) {
         msgTimestampOpt = MSGOPT_TIMESTAMP;
      } else {
         msgTimestampOpt = MSGOPT_GW_TIMESTAMP;
	  }
	  if (SUCCESS == ISMSMSG_GetOpt(pISMSMsg, msgTimestampOpt, &smo) ) {
         pJts = (JulianType *) smo.pVal;
         ts.year = (pJts->wYear > 2000) ? pJts->wYear - 2000 : pJts->wYear - 1900;
         ts.month = (byte) pJts->wMonth;
         ts.day = (byte) pJts->wDay;
         ts.hour = (byte) pJts->wHour;
         ts.minute = (byte) pJts->wMinute;
         ts.second = (byte) pJts->wSecond;
            SPRINTF(message.msgAttribute.timeStamp, TIME_STRING, ts.month, ts.day, ts.year, ts.hour, ts.minute);
      }
   }
      
   // For MO message: retrieve message string (part of it, if the msg is long)
   if ((msgTag == AEESMS_TAG_MO_SENT) || (msgTag == AEESMS_TAG_MO_NOT_SENT)) {
      if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PAYLOAD_SZ, &smo) == SUCCESS) {
         DBGPRINTF("payload_sz ...");
         msgLen = STRLEN((const char *) smo.pVal);
         if ( msgLen <= (sizeof(message.msgAttribute.msgString) - 1) ) {
            STRNCPY(message.msgAttribute.msgString, (const char *) smo.pVal, msgLen + 1);   
         }
         else {
            STRNCPY(message.msgAttribute.msgString, (const char *) smo.pVal, sizeof(message.msgAttribute.msgString) - 3);
            message.msgAttribute.msgString[sizeof(message.msgAttribute.msgString) - 3] = 0;
            STRCAT(message.msgAttribute.msgString, "..");
         }
      } else if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PAYLOAD_WSZ, &smo) == SUCCESS) {
              DBGPRINTF("payload_wsz ...");
         msgLen = WSTRLEN((AECHAR *) smo.pVal);
         if ( msgLen <= (sizeof(message.msgAttribute.msgString) - 1) ) {
            WSTRTOSTR((AECHAR *) smo.pVal, message.msgAttribute.msgString, 
               sizeof(message.msgAttribute.msgString));
         }
         else {
            WSTRTOSTR((AECHAR *) smo.pVal, message.msgAttribute.msgString, 
               sizeof(message.msgAttribute.msgString) - 3);
            message.msgAttribute.msgString[sizeof(message.msgAttribute.msgString) - 3] = 0;
            STRCAT(message.msgAttribute.msgString, "..");
         }         
      } else if (ISMSMSG_GetOpt(pISMSMsg, MSGOPT_PAYLOAD_BINARY, &smo) == SUCCESS) {
         DBGPRINTF("binary payload ...");
         msgLen = WEBOPT_GETVARBUFFERLENGTH(smo.pVal);
         if ( msgLen <= (sizeof(message.msgAttribute.msgString) - 1) ) {
            STRNCPY(message.msgAttribute.msgString, 
               (const char *)WEBOPT_GETVARBUFFERDATA(smo.pVal), msgLen + 1);   
         }
         else {
            STRNCPY(message.msgAttribute.msgString, 
               (const char *)WEBOPT_GETVARBUFFERDATA(smo.pVal), sizeof(message.msgAttribute.msgString) - 3);
            message.msgAttribute.msgString[sizeof(message.msgAttribute.msgString) - 3] = 0;
            STRCAT(message.msgAttribute.msgString, "..");
         }
      }
   }

   if (pMe->m_bEnumerating) {
      c_samplesms_AddMessageToList(pMe, &message, msgTag);
      c_samplesms_EnumMessages(pMe);
   }
   else {
      c_samplesms_AddMessageToList(pMe, &message, msgTag);
   }
   
}

/*===========================================================================
FUNCTION c_samplesms_DeleteMessage

DESCRIPTION
   This function will delete a message with the specified index 

PROTOTYPE:
   void c_samplesms_DeleteMessage(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_DeleteMessage(CSampleSMS *pMe, uint16 msgIndex) {
   pMe->m_curIndex = msgIndex;
   CALLBACK_Cancel(&pMe->m_callback);
   CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY)c_samplesms_DeleteMessageCB, pMe);
   ISMSSTORAGE_DeleteMsg(pMe->m_pISMSStorage, pMe->m_storageType, pMe->m_curIndex, &pMe->m_callback, &pMe->m_retVal);
}

/*===========================================================================
FUNCTION c_samplesms_DeleteMessageCB

DESCRIPTION
   This callback function fires after a message with specified index is deleted.
   This function will update the correponding message list (Inbox/Outbox/Drafts)

PROTOTYPE:
   void c_samplesms_DeleteMessageCB(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_DeleteMessageCB(CSampleSMS *pMe) {
   DBGPRINTF("del msg status = %d", pMe->m_retVal);
   if (SUCCESS == pMe->m_retVal) {
      switch (pMe->m_appstate) {
         case APPSTATE_INBOX:
            IMENUCTL_DeleteItem(pMe->m_pIMenuInboxMsgs, pMe->m_curIndex);
            IMENUCTL_Redraw(pMe->m_pIMenuInboxMsgs);
            break;
         case APPSTATE_OUTBOX:
            IMENUCTL_DeleteItem(pMe->m_pIMenuOutboxMsgs, pMe->m_curIndex);
            IMENUCTL_Redraw(pMe->m_pIMenuOutboxMsgs);
            break;
         case APPSTATE_DRAFTS:
            IMENUCTL_DeleteItem(pMe->m_pIMenuDrafts, pMe->m_curIndex);
            IMENUCTL_Redraw(pMe->m_pIMenuDrafts);
            break;
		 default:
			 break;
      }
      DBGPRINTF("msg %d deleted", pMe->m_curIndex);
      c_samplesms_DrawSoftKey(pMe, IDS_DELETE_MSG, IDS_READ_MSG);   
      IDISPLAY_Update(pMe->m_pIDisplay);
   }
   else {
      DBGPRINTF("delete msg error = %d", pMe->m_retVal);
   }

}

/*===========================================================================
FUNCTION c_samplesms_DeleteMessageTag

DESCRIPTION
   This function will delete message(s) with the specified tag 

PROTOTYPE:
   void c_samplesms_DeleteMessageTag(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_DeleteMessageTag(CSampleSMS *pMe, AEESMSTag tag) {
   pMe->m_curTag = tag;
   CALLBACK_Cancel(&pMe->m_callback);
   CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY)c_samplesms_DeleteMessageTagCB, pMe);
   ISMSSTORAGE_DeleteMsgTag(pMe->m_pISMSStorage, pMe->m_storageType, pMe->m_curTag, 
      &pMe->m_callback, &pMe->m_retVal);
}

/*===========================================================================
FUNCTION c_samplesms_DeleteMessageTagCB

DESCRIPTION
   This callback function fires after message(s) with specified tag are deleted.
   This function will update the correponding message list (Inbox/Outbox/Drafts),
   and pop up a message to the user informing the deletion is done.

PROTOTYPE:
   void c_samplesms_DeleteMessageTagCB(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_DeleteMessageTagCB(CSampleSMS *pMe) {
   DBGPRINTF("del tag status = %d", pMe->m_retVal);
   switch (pMe->m_curTag) {
      case AEESMS_TAG_MT_NOT_READ:
         DBGPRINTF("unread msg(s) deleted..");
         c_samplesms_DeleteMessageTag(pMe, AEESMS_TAG_MT_READ);
         break;
      case AEESMS_TAG_MT_READ:
            DBGPRINTF("read msg(s) deleted..");      
         IMENUCTL_DeleteAll(pMe->m_pIMenuInboxMsgs);
         c_samplesms_ShowStatus(pMe, "Inbox deleted...");
         break;
      case AEESMS_TAG_MO_SENT:
         DBGPRINTF("sent msg(s) deleted..");
         IMENUCTL_DeleteAll(pMe->m_pIMenuOutboxMsgs);
         c_samplesms_ShowStatus(pMe, "Outbox deleted...");
         break;
      case AEESMS_TAG_MO_NOT_SENT:
         DBGPRINTF("draft msg(s) deleted..");
         IMENUCTL_DeleteAll(pMe->m_pIMenuDrafts);
         c_samplesms_ShowStatus(pMe, "Drafts deleted...");
         break;
   }
}

/*===========================================================================
FUNCTION c_samplesms_DeleteMessageAll

DESCRIPTION
   This function will delete all messages in the current storage. 

PROTOTYPE:
   void c_samplesms_DeleteMessageAll(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_DeleteMessageAll(CSampleSMS *pMe) {
   CALLBACK_Cancel(&pMe->m_callback);
   CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY)c_samplesms_DeleteMessageAllCB, pMe);
   ISMSSTORAGE_DeleteAll(pMe->m_pISMSStorage, pMe->m_storageType, 
      &pMe->m_callback, &pMe->m_retVal);
}

/*===========================================================================
FUNCTION c_samplesms_DeleteMessageAllCB

DESCRIPTION
   This callback function fires after all messages are deleted.
   This function will remove any messages in the lists (Inbox/Outbox/Drafts),
   and pop up a message to the user informing the deletion is done.
PROTOTYPE:
   void c_samplesms_DeleteMessageAllCB(SampleSMS * pi);
===========================================================================*/
void c_samplesms_DeleteMessageAllCB(CSampleSMS *pMe) {
   DBGPRINTF("Del all msgs status = %d", pMe->m_retVal);
   IMENUCTL_DeleteAll(pMe->m_pIMenuInboxMsgs);
   IMENUCTL_DeleteAll(pMe->m_pIMenuOutboxMsgs);
   IMENUCTL_DeleteAll(pMe->m_pIMenuDrafts);
   c_samplesms_ShowStatus(pMe, "All messages deleted...");
}

/*===========================================================================
FUNCTION c_samplesms_ReadMessage

DESCRIPTION
   This function will read a message with specified index from the storage.

PROTOTYPE:
   void c_samplesms_ReadMessage(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_ReadMessage(CSampleSMS *pMe, uint16 msgIndex) {
   pMe->m_curIndex = msgIndex;

   if (pMe->m_pISMSMsg) {
      ISMSMSG_Release(pMe->m_pISMSMsg);
      pMe->m_pISMSMsg = NULL;
   }

   CALLBACK_Cancel(&pMe->m_callback);
   CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY)c_samplesms_ReadMessageCB, pMe);
   ISMSSTORAGE_ReadMsg(pMe->m_pISMSStorage, pMe->m_storageType, pMe->m_curIndex, 
      &pMe->m_callback, &pMe->m_pISMSMsg, &pMe->m_retVal);
}

/*===========================================================================
FUNCTION c_samplesms_ReadMessageCB

DESCRIPTION
   This callback function fires after message is read from a storage.
   This function will grab the message details to be displayed to the user
   or to be edited by the user.

PROTOTYPE:
   void c_samplesms_ReadMessageCB(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_ReadMessageCB(CSampleSMS *pMe) {
   SMSMsgOpt smo;
   JulianType *pJts;
   AEESMSTimestamp ts;
   MyMessage message;
   int nErr;
   int msgLen = 0;
   int udhDataLen = 0;
   int msgTimestampOpt;

   MEMSET(&message, 0, sizeof(MyMessage));

   if (SUCCESS == pMe->m_retVal) {
      nErr = ISMSMSG_GetOpt(pMe->m_pISMSMsg, MSGOPT_TAG, &smo);
      if (SUCCESS == nErr) {
         pMe->m_curTag = (AEESMSTag) smo.pVal;         
      } else {
         DBGPRINTF("Couldn't retrieve msg tag");
      }   

      // For inbox message: Get message sender number
      if (pMe->m_appstate == APPSTATE_INBOX) {
         nErr = ISMSMSG_GetOpt(pMe->m_pISMSMsg, MSGOPT_FROM_DEVICE_SZ, &smo);      
         if (SUCCESS == nErr) {
            STRNCPY(message.number.srcNumber, (const char *) smo.pVal, STRLEN((const char *) smo.pVal) + 1);
            DBGPRINTF("msg from = %s", message.number.srcNumber);
         }
         else {
            STRCPY(message.number.srcNumber, "N/A");
            DBGPRINTF("get msg opt error = %d", nErr);
         }
      }
   
      // For outbox/draft message: Get message destination number
      if ((pMe->m_appstate == APPSTATE_OUTBOX) || (pMe->m_appstate == APPSTATE_DRAFTS))  {
         nErr = ISMSMSG_GetOpt(pMe->m_pISMSMsg, MSGOPT_TO_DEVICE_SZ, &smo);      
         if (SUCCESS == nErr) {
            STRNCPY(message.number.destNumber, (const char *) smo.pVal, STRLEN((const char *) smo.pVal) + 1);
            DBGPRINTF("msg to = %s", message.number.destNumber);
         }
         else {
            STRCPY(message.number.destNumber, "N/A");
            DBGPRINTF("get msg opt error = %d", nErr);
         }
      }

      // For MT message: Get message time stamp
      if ( (pMe->m_appstate == APPSTATE_INBOX) )  {
         if (pMe->m_storageType == AEESMS_NV_CDMA) {
            msgTimestampOpt = MSGOPT_TIMESTAMP;
         } else {
            msgTimestampOpt = MSGOPT_GW_TIMESTAMP;
         }
         if (SUCCESS == ISMSMSG_GetOpt(pMe->m_pISMSMsg, msgTimestampOpt, &smo) ) {
            pJts = (JulianType *) smo.pVal;
            ts.year = (pJts->wYear > 2000) ? pJts->wYear - 2000 : pJts->wYear - 1900;
            ts.month = (byte) pJts->wMonth;
            ts.day = (byte) pJts->wDay;
            ts.hour = (byte) pJts->wHour;
            ts.minute = (byte) pJts->wMinute;
            ts.second = (byte) pJts->wSecond;

            SPRINTF(message.msgAttribute.timeStamp, TIME_STRING, 
               ts.month, ts.day, ts.year, ts.hour, ts.minute);
         }
      }
      
      // Get concatenation info, if any
      if (SUCCESS == ISMSMSG_GetOpt(pMe->m_pISMSMsg, MSGOPT_UDH_CONCAT, &smo) ) {
         message.bIsConcatMsg = TRUE;
         message.msgSeqNum = *((unsigned char*)(MSGOPT_GETUDHBUFFERDATA(
                           (unsigned char*)smo.pVal)+1));
         message.msgTotal = *((unsigned char*)(MSGOPT_GETUDHBUFFERDATA(
                           (unsigned char*)smo.pVal)+2));
         if (*((unsigned char*)(MSGOPT_GETUDHBUFFERDATA(
                           (unsigned char*)smo.pVal))) == AEESMS_UDH_CONCAT_8)  {
            message.msgRefNum = *((unsigned char*)(MSGOPT_GETUDHBUFFERDATA(
                           (unsigned char*)smo.pVal)+3)); 
         } else {
            message.msgRefNum = *((unsigned char*)(MSGOPT_GETUDHBUFFERDATA(
                           (unsigned char*)smo.pVal)+3)) +
                           (*((unsigned char*)(MSGOPT_GETUDHBUFFERDATA(
                           (unsigned char*)smo.pVal)+4)) << 8);
         }

      }
      
      // Get message payload
      if (ISMSMSG_GetOpt(pMe->m_pISMSMsg, MSGOPT_PAYLOAD_SZ, &smo) == SUCCESS) {
         DBGPRINTF("payloadsz success");
         msgLen = STRLEN((const char *) smo.pVal);
         FREEIF(pMe->m_pReadMsgBuffer);
         if(NULL == (pMe->m_pReadMsgBuffer = (char *) MALLOC(msgLen + 1) )){
            DBGPRINTF("Couldn't allocate memory");
            return;
         } else {
            STRNCPY(pMe->m_pReadMsgBuffer, (const char *) smo.pVal, msgLen+1);
         }
      } else if (ISMSMSG_GetOpt(pMe->m_pISMSMsg, MSGOPT_PAYLOAD_WSZ, &smo) == SUCCESS) {
            DBGPRINTF("payloadwsz success");
         msgLen = WSTRLEN((AECHAR *) smo.pVal);
         FREEIF(pMe->m_pReadMsgBuffer);
         if(NULL == (pMe->m_pReadMsgBuffer = (char *) MALLOC(msgLen + 1) )){
            DBGPRINTF("Couldn't allocate memory");
            return;
         } else {
            WSTRTOSTR((AECHAR *) smo.pVal, pMe->m_pReadMsgBuffer, msgLen+1);
         }
      } else if (ISMSMSG_GetOpt(pMe->m_pISMSMsg, MSGOPT_PAYLOAD_BINARY, &smo) == SUCCESS) {
         DBGPRINTF("binary payload");
         msgLen = WEBOPT_GETVARBUFFERLENGTH(smo.pVal);
         FREEIF(pMe->m_pReadMsgBuffer);
         if(NULL == (pMe->m_pReadMsgBuffer = (char *) MALLOC(msgLen + 1) )){
            DBGPRINTF("Couldn't allocate memory");
            return;
         } else {
            STRNCPY(pMe->m_pReadMsgBuffer, 
                  (const char *)WEBOPT_GETVARBUFFERDATA(smo.pVal), msgLen+1);
         }
      }

      pMe->m_objPos = -1;
      // Retrieve user predefined picture, if any
      if (ISMSMSG_GetOpt(pMe->m_pISMSMsg, MSGOPT_UDH_USERDEFINED, &smo) == SUCCESS) {
         udhDataLen = WEBOPT_GETVARBUFFERLENGTH(smo.pVal);
         DBGPRINTF("userUDH length = %d", udhDataLen);
         pMe->m_objPos = *((byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)smo.pVal)+1));
         if (udhDataLen == UDH_SM_PIC_ANIM_LENGTH - 1) {   
            message.emsObj.header_id = AEESMS_UDH_SMALL_PICTURE;
            message.emsObj.u.small_picture.position = 
                  *((byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)smo.pVal)+1));
            MEMCPY(message.emsObj.u.small_picture.data,
                  (MSGOPT_GETUDHBUFFERDATA((byte*)smo.pVal)+3), 32);
         }
         else if (udhDataLen == UDH_LG_PIC_ANIM_LENGTH - 1) {
            message.emsObj.header_id = AEESMS_UDH_LARGE_PICTURE;
            message.emsObj.u.large_picture.position = 
                  *((byte*)(MSGOPT_GETUDHBUFFERDATA((byte*)smo.pVal)+1));
            MEMCPY(message.emsObj.u.large_picture.data,
                  (MSGOPT_GETUDHBUFFERDATA((byte*)smo.pVal)+3), 128);
         }   
      }

      if (pMe->m_appstate == APPSTATE_DRAFTS) {
         c_samplesms_EditMessage(pMe, message);
      }
      else {
         c_samplesms_DisplayMessage(pMe, message);
      }
   }
   else {
      DBGPRINTF("ReadMsg failed !");
   }
}


/*===========================================================================
FUNCTION c_samplesms_SMSStorage_ModelListener

DESCRIPTION
   This callback function will be invoked when there is an event 
   in SMSStorage model.

PROTOTYPE:
   void c_samplesms_SMSStorage_ModelListener
      (CSampleSMS * pi, ModelEvent *pModelEvent);
===========================================================================*/
void c_samplesms_SMSStorage_ModelListener(CSampleSMS *pMe, ModelEvent *pModelEvent) {
   AEESMSStorageEvtData *pStorageEvt;

   pStorageEvt = (AEESMSStorageEvtData *)pModelEvent->dwParam;
   DBGPRINTF("Event on = %d", pStorageEvt->st);
   DBGPRINTF("Model event = %d", pModelEvent->evCode);
   DBGPRINTF("Tag = %d", pStorageEvt->mt);
   DBGPRINTF("Index = %d", pStorageEvt->nIndex);
   
   if (pStorageEvt->st == pMe->m_storageType) {
      switch (pModelEvent->evCode) {
         case EVT_MDL_SMSSTORAGE_STORE:
            //***********************************************************
            //Get updated storage counter and refresh main menu listing
            //***********************************************************
            if (pStorageEvt->mt == AEESMS_TAG_MT_NOT_READ) {
               pMe->m_nUnread++;
               c_samplesms_ShowStatus(pMe, 
                     "You've got a new message\n Check your inbox");
               if (pMe->m_appstate == APPSTATE_MAIN) {
                  c_samplesms_BuildMainMenu(pMe);         
               }
            }
            //c_samplesms_GetStorageStatus(pMe, pStorageEvt->mt);
            break;
         case EVT_MDL_SMSSTORAGE_READY:
            // **********************************************************
            // Note: 
            // In BREW 3.1.5 Simulator, SMSStorage model doesn't 
            // - consistently send EVT_MDL_SMSSTORAGE_READY
            // 
            // The code below is commented out,
            // - this application choose to call the procedure to obtain
            // - message counters upon receiving EVT_APP_START
            // **********************************************************
            //c_samplesms_MainMenu(pMe);
            break;
         case EVT_MDL_SMSSTORAGE_FULL:
            c_samplesms_ShowStatus(pMe, "Storage full!!");
            break;
      }
   }
   else {
      return;
   }
}

/*===========================================================================
FUNCTION c_samplesms_StoreMessage

DESCRIPTION
   This function will store a message encapsulated in the specified
   ISMSMsg instance
PROTOTYPE:
   void c_samplesms_StoreMessage(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_StoreMessage(CSampleSMS *pMe) {
   DBGPRINTF("storing message..");
   CALLBACK_Cancel(&pMe->m_callback);
   CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY)c_samplesms_StoreMessageCB, pMe);
   ISMSSTORAGE_StoreMsg(pMe->m_pISMSStorage, pMe->m_storageType, pMe->m_pISMSMsg, 
               &pMe->m_callback, &pMe->m_nStoreIndex, &pMe->m_retVal);
}

/*===========================================================================
FUNCTION c_samplesms_StoreRcvdMessage

DESCRIPTION
   This function will store a received encapsulated in the specified
   ISMSMsg instance (see also c_samplesms_ReceiveSMSMessage)
PROTOTYPE:
   void c_samplesms_StoreRcvdMessage(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_StoreRcvdMessage(CSampleSMS *pMe) {
   DBGPRINTF("storing rcvd message..");
   CALLBACK_Cancel(&pMe->m_cbStoreRcvd);
   CALLBACK_Init(&pMe->m_cbStoreRcvd, (PFNNOTIFY)c_samplesms_StoreRcvdMessageCB, pMe);
   ISMSSTORAGE_StoreMsg(pMe->m_pISMSStorage, pMe->m_storageType, pMe->m_pISMSMsgRcv, 
      &pMe->m_cbStoreRcvd, &pMe->m_nStoreIndex, &pMe->m_errStoreRcv);
}

/*===========================================================================
FUNCTION c_samplesms_StoreMessageCB

DESCRIPTION
   This callback function will be invoked when storing a message has finished
   
PROTOTYPE:
   void c_samplesms_StoreMessageCB(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_StoreMessageCB(CSampleSMS *pMe) {
   AEESMSTag storeTag;
   SMSMsgOpt smo;
   int nErr;

   DBGPRINTF("Store status = %d", pMe->m_retVal);
   DBGPRINTF("Store index  = %d", pMe->m_nStoreIndex);

   // *************************************************************************************
   //If a specified storage has been previously visited, directly add message to the list
   // - (message enumeration is only done on the first access)
   // *************************************************************************************
   nErr = ISMSMSG_GetOpt(pMe->m_pISMSMsg, MSGOPT_TAG, &smo);
   if (SUCCESS == nErr) {
      storeTag = (AEESMSTag) smo.pVal;         
   } else {
      DBGPRINTF("Couldn't retrieve msg tag");
      return;
   }   
   
   if (SUCCESS == pMe->m_retVal) {
      switch(storeTag) {
         case AEESMS_TAG_MO_SENT:
            pMe->m_nSent++;
            if (pMe->m_bOutboxEnumerated) {
               pMe->m_index = pMe->m_nStoreIndex;
               c_samplesms_GetMessage(pMe, storeTag);
            }
            if (pMe->m_totalMsg > 1) {
               pMe->m_seqNum++;

               if (pMe->m_seqNum <= pMe->m_totalMsg) {
                  c_samplesms_SendSMS(pMe);
                  return;
               } else {   // reset sequence number for next destination number
                  pMe->m_seqNum = 1;
               }
            }

            pMe->m_destIndex++;
            FREEIF(pMe->m_pszDestNum);
            if (pMe->m_destIndex <= MAX_DESTINATION_INDEX) {
               c_samplesms_SendSMS(pMe);
               return;
            }
            ISHELL_SendEvent(pMe->m_pIShell, AEECLSID_C_SAMPLESMS, 
               EVT_SENDING_DONE, 0, 0);      
            break;

         case AEESMS_TAG_MO_NOT_SENT:
            pMe->m_nDraft++;
            if (pMe->m_bDraftsEnumerated) {
               pMe->m_index = pMe->m_nStoreIndex;
               c_samplesms_GetMessage(pMe, storeTag);
            }
            break;
      }
      if (pMe->m_appstate == APPSTATE_MAIN) {
         c_samplesms_BuildMainMenu(pMe);         
      }
   } else {
      DBGPRINTF("storing not successful!!");
   }
}

/*===========================================================================
FUNCTION c_samplesms_StoreRcvdMessageCB

DESCRIPTION
   This callback function will be invoked when storing 
   a received message has finished
   
PROTOTYPE:
   void c_samplesms_StoreRcvdMessageCB(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_StoreRcvdMessageCB(CSampleSMS *pMe) {
   DBGPRINTF("Store status = %d", pMe->m_errStoreRcv);
   DBGPRINTF("Store index  = %d", pMe->m_nStoreIndex);

   /* NOTE!!!
     Application may receive another message before this callback is invoked.
     Considering this situation (newer message store will overwrite
     the callback), this application utilizes
     the SMSStorage ModelListener to update pMe->m_nUnread counter.
     With similar reason, regardless whether inbox has been previously 
     visited/enumerated, the application will (re-)do inbox enumeration 
     when user is accessing the inbox.

     With above consideration, the code below is commented out...

   // *************************************************************************************
   //If a specified storage has been previously visited, directly add message to the list
   // - (message enumeration is only done on the first access)
   // *************************************************************************************
   pMe->m_nUnread++;
   if (pMe->m_bInboxEnumerated) {
      pMe->m_index = pMe->m_nStoreIndex;
      c_samplesms_GetMessage(pMe);
   }
   if (pMe->m_appstate == APPSTATE_MAIN) {
      c_samplesms_RefreshMainMenu(pMe);         
   }
   c_samplesms_ShowStatus(pMe, "You've got a new message\n Check your inbox");
   */
}


/*===========================================================================
FUNCTION c_samplesms_UpdateMessage

DESCRIPTION
   This function will update a message of a specified index with updated
   ISMSMsg instance
PROTOTYPE:
   void c_samplesms_UpdateMessage(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_UpdateMessage(CSampleSMS *pMe) {
   CALLBACK_Cancel(&pMe->m_callback);
   CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY)c_samplesms_UpdateMessageCB, pMe);
   ISMSSTORAGE_UpdateMsg(pMe->m_pISMSStorage, pMe->m_storageType, pMe->m_curIndex, 
         pMe->m_pISMSMsg, &pMe->m_callback, &pMe->m_retVal);
}

/*===========================================================================
FUNCTION c_samplesms_UpdateMessageCB

DESCRIPTION
   This callback fires after update of a message is done.
   If update message involves updating message tag, list will also be updated
   accordingly
PROTOTYPE:
   void c_samplesms_UpdateMessageCB(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_UpdateMessageCB(CSampleSMS *pMe) {
   AEESMSTag updateTag;
   SMSMsgOpt smo;
   int nErr;

   DBGPRINTF("Update status = %d", pMe->m_retVal);
   DBGPRINTF("Update index  = %d", pMe->m_curIndex);

   nErr = ISMSMSG_GetOpt(pMe->m_pISMSMsg, MSGOPT_TAG, &smo);
   if (SUCCESS == nErr) {
      updateTag = (AEESMSTag) smo.pVal;         
   } else {
      DBGPRINTF("Couldn't retrieve msg tag");
      return;
   }   
   
   if (SUCCESS == pMe->m_retVal) {
      switch(updateTag) {
            case AEESMS_TAG_MO_SENT:
            // **********************************************************
            // This will be resulted after editing draft message, 
            // - and send it.
            // A stored draft message will be updated as a sent message
            // **********************************************************
                pMe->m_nSent++;
            if (pMe->m_bOutboxEnumerated) {
               pMe->m_index = pMe->m_curIndex;
               c_samplesms_GetMessage(pMe, updateTag);
            }
            if (pMe->m_totalMsg > 1) {
               pMe->m_seqNum++;

               if (pMe->m_seqNum <= pMe->m_totalMsg) {
                  c_samplesms_SendSMS(pMe);
                  return;
               } else {   // reset sequence number for next destination number
                  pMe->m_seqNum = 1;
               }
            }

            pMe->m_destIndex++;
            FREEIF(pMe->m_pszDestNum);
            if (pMe->m_destIndex <= MAX_DESTINATION_INDEX) {
               c_samplesms_SendSMS(pMe);
               return;
            }
            ISHELL_SendEvent(pMe->m_pIShell, AEECLSID_C_SAMPLESMS, 
               EVT_SENDING_DONE, 0, 0);      
            break;

         case AEESMS_TAG_MT_READ:
            c_samplesms_MarkAsRead(pMe, pMe->m_curIndex);
            IMENUCTL_Redraw(pMe->m_pIMenuInboxMsgs);
            IMENUCTL_SetActive(pMe->m_pIMenuInboxMsgs, TRUE);
            c_samplesms_DrawSoftKey(pMe, IDS_DELETE_MSG, IDS_READ_MSG);
            break;
         default:
            break;
      }
   }
}


