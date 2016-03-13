/*===========================================================================
FILE: c_sendreceive.c

AUTHOR:
   QUALCOMM Incorporated

SERVICES: 
   SampleSMS applet showing usage of BREW ISMS interfaces.

DESCRIPTION:
   This sample application (C_SAMPLESMS) demonstrates:
   - Using ISMS to send messages encapsulated by ISMSMsg interface
   - Using ISMS to receive incoming message(s) by registering for notification 
   issued by ISMSNotifier (AEECLSID_SMSNOTIFIER).
   - Sending and receiving EMS utilizing User Data Header (UDH). 
   - SMSStorage for messages, including performing operation on messages 
   (store, delete, update, read). The application will also provide 
   message classification as inbox, outbox, or draft messages.

   This file represents the program for sending and receiving a message, 
   including setting/getting any parameters needed for sending and receiving 
   a message.


                 Copyright ?2008 QUALCOMM Incorporated.
                        All Rights Reserved.
                     QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "c_sendreceive.h"
#include "c_conversionEmsObj.h"
#include "c_smsstorage.h"

/*===============================================================================
                        FUNCTION PROTOTYPES
=============================================================================== */
void c_samplesms_CalculateTotalMessage(CSampleSMS *pMe);
int c_samplesms_ExtractTextMessage(CSampleSMS *pMe);
void c_samplesms_AddPayloadSzToMessage(CSampleSMS *pMe);
void c_samplesms_AddPayloadWszToMessage(CSampleSMS *pMe);
void c_samplesms_AddPayloadBinaryToMessage(CSampleSMS *pMe);
void c_samplesms_AddObjectToMessage(CSampleSMS *pMe, int objType, int position);
void c_samplesms_AddConcatUDH(CSampleSMS *pMe);
void c_samplesms_SendSMSCB(CSampleSMS *pMe);


/*===========================================================================
FUNCTION c_samplesms_SetPayloadEncoding

DESCRIPTION
   This function set the message payload encoding, based on
   AEE_DEVICEITEM_TAPI_SMS_ENCODING retrieved from a device
 
PROTOTYPE:
   void c_samplesms_SetPayloadEncoding(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_SetPayloadEncoding(CSampleSMS *pMe) {
   switch (*pMe->m_pPlEncodings) {
      case AEE_ENC_ISOLATIN1:
         pMe->m_payloadEncoding = AEE_ENC_ISOLATIN1;
         return;
      case AEE_ENC_UNICODE:
         pMe->m_payloadEncoding = AEE_ENC_UNICODE;
         return;
      default:
         DBGPRINTF("not isolatin1 nor unicode!");
         pMe->m_payloadEncoding = (*pMe->m_pPlEncodings);
         return;
   }
}

/*===========================================================================
FUNCTION c_samplesms_SetDefaultPayloadEncoding

DESCRIPTION
   If AEE_DEVICEITEM_TAPI_SMS_ENCODING value couldn't be retrieved 
   from a device, this function will set payload encoding to 
   a common encoding for CDMA/GSM device. 
   
PROTOTYPE:
   boolean c_samplesms_SetDefaultPayloadEncoding(CSampleSMS * pi);

RETURN VALUE
  TRUE: If the app is able to get default payload encoding 
  FALSE: If the app fails to get default payload encoding

===========================================================================*/
boolean c_samplesms_SetDefaultPayloadEncoding(CSampleSMS *pMe) {
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
         DBGPRINTF("Use default CDMA device encoding");
         pMe->m_payloadEncoding = DEFAULT_CDMA_TAPI_SMS_ENC;
         break;

      case AEET_SYS_MODE_GSM:
      case AEET_SYS_MODE_WCDMA:
         DBGPRINTF("Use default GSM device encoding");
        pMe->m_payloadEncoding = DEFAULT_GSM_TAPI_SMS_ENC;
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
FUNCTION c_samplesms_InitMOSMSEncoding
DESCRIPTION
   This function set MOSMSEncoding, based on the value(s) retrieved from
   ISMS_GetEncodingsAvailableForMOSMS. The first encoding value of
   the retrieved encodings list is used as the init value for MO SMS Encoding.
 
PROTOTYPE:
   void c_samplesms_InitMOSMSEncoding(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_InitMOSMSEncoding(CSampleSMS *pMe) {

   switch (*pMe->m_pMOEncodings) {
      case AEESMS_ENC_OCTET:
      case AEESMS_ENC_ASCII:
      case AEESMS_ENC_UNICODE:
      case AEESMS_ENC_LATIN:
      case AEESMS_ENC_GSM_7_BIT_DEFAULT:
         pMe->m_moEncoding = (*pMe->m_pMOEncodings);
         break;
      default:
         DBGPRINTF("non common mo encoding!");
         DBGPRINTF("WARNING! App may not handle message segmentation correctly");
            pMe->m_moEncoding = (*pMe->m_pMOEncodings);
         break;
   }
   c_samplesms_SetMOEncodingsMenu(pMe);

}

/*===========================================================================
FUNCTION c_samplesms_SetMOEncoding
DESCRIPTION
   If more than one MO Encoding is available on a device, 
   this function set MOSMSEncoding to the user selected encoding 
   (selected from MOEncodings Menu)
   
PROTOTYPE:
   void c_samplesms_SetMOEncoding(CSampleSMS * pi, uint16 selectedMOEnc);
===========================================================================*/
void c_samplesms_SetMOEncoding(CSampleSMS *pMe, uint16 selectedMOEnc) {
   char message[64];

   switch (selectedMOEnc) {
      case IDS_MOENC_OCTET:
         pMe->m_moEncoding = AEESMS_ENC_OCTET;
         pMe->m_payloadEncoding = *pMe->m_pPlEncodings;
         STRCPY(message, "MOEnc changed to 'OCTET'"); 
         break;
      case IDS_MOENC_ASCII:
         pMe->m_moEncoding = AEESMS_ENC_ASCII;
         pMe->m_payloadEncoding = *pMe->m_pPlEncodings;
         STRCPY(message, "MOEnc changed to 'ASCII'");
         break;
      case IDS_MOENC_UNICODE:
         pMe->m_moEncoding = AEESMS_ENC_UNICODE;
         pMe->m_payloadEncoding = *pMe->m_pPlEncodings;
         STRCPY(message, "MOEnc changed to 'UNICODE'");
         break;
      case IDS_MOENC_LATIN:
         pMe->m_moEncoding = AEESMS_ENC_LATIN;
         pMe->m_payloadEncoding = *pMe->m_pPlEncodings;
         STRCPY(message, "MOEnc changed to 'LATIN'");
         break;
      case IDS_MOENC_GSM7BIT:
         pMe->m_moEncoding = AEESMS_ENC_GSM_7_BIT_DEFAULT;
         pMe->m_payloadEncoding = AEE_ENC_GSM_7_BIT_DEFAULT;
         STRCPY(message, "MOEnc changed to 'GSM7BIT'");
         break;
      default:
         STRCPY(message, "MOEnc unchanged");
         break;
   }
   c_samplesms_ShowStatus(pMe, message);

}


/*===========================================================================
FUNCTION c_samplesms_ComposeScreen
DESCRIPTION
   This function will display Compose Screen to get user entries
   for a message to be sent.
   
PROTOTYPE:
   void c_samplesms_ComposeScreen(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_ComposeScreen(CSampleSMS *pMe) {
   AEERect rectDest1;
   AEERect rectDest2;
   AEERect rectMsg;

   rectDest1.x = 25;
   rectDest1.y = 5;
   rectDest1.dx = (pMe->DeviceInfo.cxScreen) - 35;
   rectDest1.dy = 10; 

   rectDest2.x = 25;
   rectDest2.y = 40;   
   rectDest2.dx = (pMe->DeviceInfo.cxScreen) - 35;
   rectDest2.dy = 10; 

   rectMsg.x = 10;
   rectMsg.y = 60;
   rectMsg.dx = (pMe->DeviceInfo.cxScreen) - 20;
   rectMsg.dy = (pMe->DeviceInfo.cyScreen) - 140;   

   c_samplesms_ResetControls(pMe);
   ITEXTCTL_Reset(pMe->m_pITextDest1);
   ITEXTCTL_Reset(pMe->m_pITextDest2);
   ITEXTCTL_Reset(pMe->m_pITextMsg);

   pMe->m_objPos = -1;

   // Set input text box for Destination Number 1
   ITEXTCTL_SetRect(pMe->m_pITextDest1, &rectDest1);
   ITEXTCTL_SetProperties(pMe->m_pITextDest1, TP_FRAME );
   ITEXTCTL_SetMaxSize(pMe->m_pITextDest1, MAX_DEST_NUM_LENGTH    );
   ITEXTCTL_SetTitle(pMe->m_pITextDest1, NULL, 0, 
      STRTOWSTR("To:", pMe->m_wszStringBuffer, (STRLEN("To:")+1)*sizeof(AECHAR)));
   ITEXTCTL_SetActive(pMe->m_pITextDest1, TRUE);
   ITEXTCTL_SetInputMode(pMe->m_pITextDest1, AEE_TM_DIALED_DIGITS);

   // Set input text box for Destination Number 2
   ITEXTCTL_SetRect(pMe->m_pITextDest2, &rectDest2);
   ITEXTCTL_SetProperties(pMe->m_pITextDest2, TP_FRAME );
   ITEXTCTL_SetMaxSize(pMe->m_pITextDest2, MAX_DEST_NUM_LENGTH);
   ITEXTCTL_SetActive(pMe->m_pITextDest2, FALSE);
   ITEXTCTL_SetInputMode(pMe->m_pITextDest2, AEE_TM_DIALED_DIGITS );

   // Set input text box for Message body
   ITEXTCTL_SetRect(pMe->m_pITextMsg, &rectMsg);
   ITEXTCTL_SetProperties(pMe->m_pITextMsg, TP_MULTILINE|TP_FRAME );
   ITEXTCTL_SetMaxSize(pMe->m_pITextMsg, BUFFER_LENGTH );
   ITEXTCTL_SetTitle(pMe->m_pITextMsg, NULL, 0, 
      STRTOWSTR("Text:", pMe->m_wszStringBuffer, (STRLEN("Text:")+1)*sizeof(AECHAR)));
   ITEXTCTL_SetActive(pMe->m_pITextMsg, FALSE);
   ITEXTCTL_SetInputMode(pMe->m_pITextMsg, AEE_TM_LETTERS );

   ITEXTCTL_Redraw(pMe->m_pITextDest1);
   ITEXTCTL_Redraw(pMe->m_pITextDest2);
   ITEXTCTL_Redraw(pMe->m_pITextMsg);
   
   c_samplesms_DrawComposeScreenAttribute(pMe);

   IDISPLAY_Update(pMe->m_pIDisplay);

}

/*===========================================================================
FUNCTION c_samplesms_DrawComposeScreenAttribute
DESCRIPTION
   This function will draw some attributes to compose screen,
   serving as additional information to the user.
   
PROTOTYPE:
   void c_samplesms_DrawComposeScreenAttribute(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_DrawComposeScreenAttribute(CSampleSMS *pMe) {
   AECHAR wszObjPos[] = {'%', 'd', '\0'};
   AECHAR textBuff[15];
   AEERect rect1, rect2;

   // *********************************************************************************
   // Drawing text to attribute fields on the Compose Screen
   // *********************************************************************************
   STRTOWSTR("1. ", textBuff, ((STRLEN("1. ") + 1) * sizeof(AECHAR)));
   IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_BOLD, textBuff, -1, 10, 20, NULL, 0);
      
   STRTOWSTR("2. ", textBuff, ((STRLEN("2. ") + 1) * sizeof(AECHAR)));
   IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_BOLD, textBuff, -1, 10, 42, NULL, 0);

   STRTOWSTR("Inserted: ", textBuff, ((STRLEN("Inserted: ") + 1) * sizeof(AECHAR)));
   IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_BOLD, textBuff, -1, 
      10, (pMe->DeviceInfo.cyScreen) - 70, NULL, 0);

   STRTOWSTR("Pos: ", textBuff, ((STRLEN("Pos: ") + 1) * sizeof(AECHAR)));
   IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_BOLD, textBuff, -1, 
      (pMe->DeviceInfo.cxScreen) - 45 , (pMe->DeviceInfo.cyScreen) - 70, NULL, 0);


   // ********************************************************************************
   // Drawing rectangle for inserted object information, if any
   // ********************************************************************************
   // Rectangle for inserted object's name:
   rect1.x = 10;
   rect1.y = (pMe->DeviceInfo.cyScreen) - 50;
   rect1.dx = (pMe->DeviceInfo.cxScreen) - 70;
   rect1.dy = 20; 
   IDisplay_DrawRect(pMe->m_pIDisplay, &rect1, 0, MAKE_RGB(0xB4,0xF0,0xF0), IDF_RECT_FILL);

   // Rectangle for inserted object's position in the text:
   rect2.x = (pMe->DeviceInfo.cxScreen) - 45;
   rect2.y = (pMe->DeviceInfo.cyScreen) - 50;
   rect2.dx = 35;
   rect2.dy = 20; 
   IDisplay_DrawRect(pMe->m_pIDisplay, &rect2, 0, MAKE_RGB(0xB4,0xF0,0xF0), IDF_RECT_FILL);

   //********************************************************************************

   
   // ***********************************************************************
   // Drawing inserted object's information: name + position in the text msg
   // ***********************************************************************
   if (pMe->m_objPos >=0) { 
      // Write object's name
      IDISPLAY_SetColor(pMe->m_pIDisplay, CLR_USER_BACKGROUND, MAKE_RGB(0xB4,0xF0,0xF0));
      IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, pMe->m_emsObj.objName, -1, 
                     (rect1.x)+2 , (rect1.y)+2, NULL, 0);

      // Write object's position
      WSPRINTF(textBuff, sizeof(textBuff), wszObjPos, pMe->m_objPos);

      IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_NORMAL, textBuff, -1, 
                     (rect2.x)+2, (rect2.y)+2, NULL, 0);

      // Reset background color to white
      IDISPLAY_SetColor(pMe->m_pIDisplay, CLR_USER_BACKGROUND, MAKE_RGB(0xFF, 0xFF, 0xFF));


      if (pMe->m_appstate == APPSTATE_INPUT_MSG) {
         // Allow user to delete the inserted object 
         c_samplesms_DrawSoftKey(pMe, IDS_SEND, IDS_DEL_OBJECT);
      }
   } // end if (pMe->m_objPos >=0)         
   
   else {   
      // While user is in the text message entry field, allow object (pic) insertion
      if (pMe->m_appstate == APPSTATE_INPUT_MSG) {
           c_samplesms_DrawSoftKey(pMe, IDS_SEND, IDS_INS_OBJECT);   
      }
   }
}


/*===========================================================================
FUNCTION c_samplesms_BackToCompose
DESCRIPTION
   This function will return the app to the Compose Screen, if the app is
   interrupted during composing a message.
   
PROTOTYPE:
   void c_samplesms_BackToCompose(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_BackToCompose(CSampleSMS *pMe) {
   pMe->m_appstate = APPSTATE_INPUT_MSG;
   ISTATIC_SetActive(pMe->m_pIStatic, FALSE);
   IDISPLAY_ClearScreen(pMe->m_pIDisplay);
   IDISPLAY_Update(pMe->m_pIDisplay);
   c_samplesms_DrawComposeScreenAttribute(pMe);

   ITEXTCTL_SetActive(pMe->m_pITextDest1, FALSE);
   ITEXTCTL_SetActive(pMe->m_pITextDest2, FALSE);
   ITEXTCTL_SetActive(pMe->m_pITextMsg, TRUE);
   ITEXTCTL_SetCursorPos(pMe->m_pITextMsg, TC_CURSOREND);

   ITEXTCTL_Redraw(pMe->m_pITextDest1);
   ITEXTCTL_Redraw(pMe->m_pITextDest2);
   ITEXTCTL_Redraw(pMe->m_pITextMsg);

}

/*===========================================================================
FUNCTION c_samplesms_InsertEMSObject
DESCRIPTION
   This function will retrieve ems object information from the selected object,
   to be inserted to a message
   
PROTOTYPE:
   void c_samplesms_InsertEMSObject(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_InsertEMSObject(CSampleSMS *pMe) {
   uint16 selectedObj;
   CtlAddItem item;

   MEMSET(&pMe->m_emsObj, 0, sizeof(pMe->m_emsObj));
   selectedObj = IMENUCTL_GetSel(pMe->m_pIMenuEMSObjects);
   if (TRUE == IMENUCTL_GetItem(pMe->m_pIMenuEMSObjects, selectedObj, &item)) {
      WSTRNCOPYN(pMe->m_emsObj.objName, sizeof(pMe->m_emsObj.objName), item.pText, -1);
      if (selectedObj == IDI_SMALL_PIC1) {      // or put range of small pic collection Ids
         pMe->m_emsObj.header_id = AEESMS_UDH_SMALL_PICTURE;
         pMe->m_objPos = ITEXTCTL_GetCursorPos(pMe->m_pITextMsg);
      }
      else if (selectedObj == IDI_LARGE_PIC1) {   // or range of large pic collection Ids
         pMe->m_emsObj.header_id = AEESMS_UDH_LARGE_PICTURE;
         pMe->m_objPos = ITEXTCTL_GetCursorPos(pMe->m_pITextMsg);
      }
      if (TRUE == c_samplesms_ConvertResPicToEMS(pMe, item.wItemID, &pMe->m_emsObj) ) {
         DBGPRINTF("Pic converted to ems format!");
      } else {
         DBGPRINTF("couldn't convert bmp to ems format!");
         return;
      }

      IMENUCTL_SetActive(pMe->m_pIMenuEMSObjects, FALSE);
      c_samplesms_BackToCompose(pMe);
   } else {
      DBGPRINTF("Couldn't retrieve item");
      return;
   }
}

/*===========================================================================
FUNCTION c_samplesms_DeleteEMSObject
DESCRIPTION
   This function will remove ems object from a composed message.
   
PROTOTYPE:
   void c_samplesms_DeleteEMSObject(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_DeleteEMSObject(CSampleSMS *pMe) {
   pMe->m_objPos = -1;
   c_samplesms_BackToCompose(pMe);
}

/*===========================================================================
FUNCTION c_samplesms_SetMessageOptions
DESCRIPTION
   This function will append SMSMsgOpts to the ISMSMsg instance
   
PROTOTYPE:
   void c_samplesms_SetMessageOptions(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_SetMessageOptions(CSampleSMS* pMe) {
   SMSMsgOpt smo[2];

   smo[1].nId = MSGOPT_END;
   smo[1].pVal = NULL;

   smo[0].nId = MSGOPT_COPYOPTS;
   smo[0].pVal = (void *) TRUE;
   ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);

   // ## Set payload encoding ##
   smo[0].nId = MSGOPT_PAYLOAD_ENCODING;
   smo[0].pVal = (void *) pMe->m_payloadEncoding;
   ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);

   // ## Set MO SMS encoding ##
   smo[0].nId = MSGOPT_MOSMS_ENCODING;
   smo[0].pVal = (void *) pMe->m_moEncoding;
   ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);

}

/*===========================================================================
FUNCTION c_samplesms_SaveDraftData
DESCRIPTION
   This function will save a message as a draft if a user is exiting the 
   application while composing a message.
   With the limitations described in the design document, i.e. there is no
   support for inline objects/images, any attached image will not be saved
   as part of a draft. Moreover, since the application doesn't maintain 
   additional database holding information of message indexes 
   (that make up a concatenated message), only first segment of the message 
   will be saved in a draft .
   
PROTOTYPE:
   void c_samplesms_SaveDraftData(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_SaveDraftData(CSampleSMS *pMe) {
   SMSMsgOpt smo[2];

   smo[1].nId = MSGOPT_END;
   smo[1].pVal = NULL;
   
   ISMSMSG_RemoveOpt(pMe->m_pISMSMsg, MSGOPT_TO_DEVICE_SZ);
   ISMSMSG_RemoveOpt(pMe->m_pISMSMsg, MSGOPT_PAYLOAD_SZ);
   ISMSMSG_RemoveOpt(pMe->m_pISMSMsg, MSGOPT_UDH_USERDEFINED);

   // Saving Destination Number
   MEMSET(pMe->m_wszStringBuffer, 0, sizeof(pMe->m_wszStringBuffer));
   ITEXTCTL_GetText(pMe->m_pITextDest1, pMe->m_wszStringBuffer, MAX_DEST_NUM_LENGTH);
   if (NULL == (pMe->m_pszDestNum = (char *) MALLOC( WSTRLEN(pMe->m_wszStringBuffer) + 1)) ) {        
      DBGPRINTF("Couldn't allocate memory");
      return;
   } else {
      WSTRTOSTR(pMe->m_wszStringBuffer, pMe->m_pszDestNum, WSTRLEN(pMe->m_wszStringBuffer) + 1);
   }

   smo[0].nId = MSGOPT_TO_DEVICE_SZ;
   smo[0].pVal = pMe->m_pszDestNum;
   ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);

   
   c_samplesms_CalculateTotalMessage(pMe);
   if (AEE_SUCCESS == c_samplesms_ExtractTextMessage(pMe))  {
       // Saving text message 
       if (pMe->m_payloadEncoding == AEE_ENC_UNICODE) {
          c_samplesms_AddPayloadWszToMessage(pMe);
       } else if (pMe->m_payloadEncoding == AEE_ENC_ISOLATIN1) {
          c_samplesms_AddPayloadSzToMessage(pMe);
       } else {
          c_samplesms_AddPayloadBinaryToMessage(pMe);
       }
    }
    
    return;
}

/*===========================================================================
FUNCTION c_samplesms_SetMessageTag
DESCRIPTION
   This function will put a tag to a message, so that it can be enumerated
   based on message classification.
   
PROTOTYPE:
   void c_samplesms_SetMessageTag(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_SetMessageTag(ISMSMsg *pISMSMsg, AEESMSTag tag) {
   SMSMsgOpt smo[2];
   
   smo[1].nId = MSGOPT_END;
   smo[1].pVal = NULL;
   
   ISMSMSG_RemoveOpt(pISMSMsg, MSGOPT_TAG);
   // ## Set Message tag ##
   smo[0].nId = MSGOPT_TAG;
   smo[0].pVal = (void *) tag;
   ISMSMSG_AddOpt(pISMSMsg, smo);
}


/*===========================================================================
FUNCTION c_samplesms_InitializeSending
DESCRIPTION
   This function will prepare the app to send a finished composed message
   
PROTOTYPE:
   void c_samplesms_InitializeSending(CSampleSMS * pi);
===========================================================================*/
int c_samplesms_InitializeSending(CSampleSMS *pMe) {
   SMSMsgOpt smo[2];

   //Create fresh ISMSMsg instance
   if (AEE_SUCCESS != c_samplesms_CreateISMSMsgInstance(pMe) ) {
       return EFAILED;
   }

   //Reset messaging
   pMe->m_totalMsg = 0;
   pMe->m_seqNum = 1;
   pMe->m_destIndex = 1;
   pMe->m_pText = NULL;
   pMe->m_pwText = NULL;
   
   c_samplesms_CalculateTotalMessage(pMe);
   
   //*****************************************************************************
   // Get reference number neeeded for message segmentation/concatenation 
   // In this application, reference number is initialized to a random number
   // - upon application starts.
   // It is then incremented by one every time application sends a long message
   //*****************************************************************************
   if (pMe->m_totalMsg > 1) {
      pMe->m_refNum++;
   }

   // *******************************************************************************
   // ## Set message type ##
   // *******************************************************************************
   smo[1].nId = MSGOPT_END;
   smo[1].pVal = NULL;

   if ((pMe->m_totalMsg > 1) || (pMe->m_objPos >=0)) {
      smo[0].nId = MSGOPT_MSGTYPE;
      smo[0].pVal = (void *) AEESMS_TYPE_EMS;
      ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);
   } else {
      smo[0].nId = MSGOPT_MSGTYPE;
      smo[0].pVal = (void *) AEESMS_TYPE_TEXT;
      ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);
   }

   /* To send on T-Mobile network */
   smo[0].nId = MSGOPT_TO_NUMBER_PLAN;
   smo[0].pVal = (void *) AEESMS_NUMBER_PLAN_TELEPHONY;
   ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);

    return AEE_SUCCESS;
}

/*===========================================================================
FUNCTION c_samplesms_CalculateTotalMessage
DESCRIPTION
   We can only send one segment message at a time.
   Maximum length of one segment can be retrieved through 
   ISMS_GetBytesAvailableForPayload() query.

   This function will calculate the length of the message,
   to determine whether segmentation is needed.
   
PROTOTYPE:
   void c_samplesms_CalculateTotalMessage(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_CalculateTotalMessage(CSampleSMS *pMe) {
   int msgLength = 0;
   int msgLengthInBytes = 0;
   int objPosInBytes = 0;
   int maxLoad = pMe->m_nMaxPayloadBytes - UDH_CONCAT_LENGTH;

   DBGPRINTF("Max payload bytes = %d", pMe->m_nMaxPayloadBytes);
   MEMSET(pMe->m_wszStringBuffer, 0, sizeof(pMe->m_wszStringBuffer));
   ITEXTCTL_GetText(pMe->m_pITextMsg, pMe->m_wszStringBuffer, BUFFER_LENGTH);

   msgLength = WSTRLEN(pMe->m_wszStringBuffer);

   switch (pMe->m_moEncoding) {
      case AEESMS_ENC_UNICODE:
         pMe->m_calculationFactor = 16;
         break;
      case AEESMS_ENC_ASCII:
      case AEESMS_ENC_GSM_7_BIT_DEFAULT:
         pMe->m_calculationFactor = 7;
         break;
      case AEESMS_ENC_OCTET:
      case AEESMS_ENC_LATIN:
         pMe->m_calculationFactor = 8;
         break;
      default:
         break;
   }

   msgLengthInBytes = msgLength * pMe->m_calculationFactor / 8;
   objPosInBytes = pMe->m_objPos * pMe->m_calculationFactor / 8;
   if ((pMe->m_moEncoding == AEESMS_ENC_ASCII) || 
      (pMe->m_moEncoding == AEESMS_ENC_GSM_7_BIT_DEFAULT)) {
      if (msgLength % 8 != 0) {
         msgLengthInBytes++;
      }
      if (pMe->m_objPos % 8 != 0) {
         objPosInBytes++;
      }
   } 
   
   // Include ems udh in the payload count
   if (pMe->m_objPos >=0) {      
      if (AEESMS_UDH_SMALL_PICTURE == pMe->m_emsObj.header_id)  {
         msgLengthInBytes += UDH_SM_PIC_ANIM_LENGTH;
         if ((maxLoad - (objPosInBytes % maxLoad)) < UDH_SM_PIC_ANIM_LENGTH) {
            // **************************************************************
            // objPos is within segment a,
            // but since the remaining bytes < udh length for the object,
            // object needs to go in the next segment (a+1),
            // leaving some unfilled bytes in message segment a.
            // ***************************************************************
            msgLengthInBytes += maxLoad - (objPosInBytes % maxLoad);
         }
      } else if (AEESMS_UDH_LARGE_PICTURE == pMe->m_emsObj.header_id) {
         msgLengthInBytes += UDH_LG_PIC_ANIM_LENGTH;
         if ((maxLoad - (objPosInBytes % maxLoad)) < UDH_LG_PIC_ANIM_LENGTH) {
            msgLengthInBytes += maxLoad - (objPosInBytes % maxLoad);
         }
      }      

   }
   
   if (msgLengthInBytes <= pMe->m_nMaxPayloadBytes) {
      pMe->m_totalMsg = 1;
   } else {
      pMe->m_totalMsg = (msgLengthInBytes / maxLoad) + 1;
      if (msgLengthInBytes % (pMe->m_nMaxPayloadBytes - UDH_CONCAT_LENGTH) == 0) {
         pMe->m_totalMsg--;
      }
   }
   DBGPRINTF("calc factor = %d", pMe->m_calculationFactor);
   DBGPRINTF("total msg = %d", pMe->m_totalMsg);
}   

/*===========================================================================
FUNCTION c_samplesms_ExtractTextMessage
DESCRIPTION
   This function will extract the text inputted by the user.
   
PROTOTYPE:
   void c_samplesms_ExtractTextMessage(CSampleSMS * pi);
===========================================================================*/
int c_samplesms_ExtractTextMessage(CSampleSMS *pMe) {
   MEMSET(pMe->m_wszStringBuffer, 0, sizeof(pMe->m_wszStringBuffer));
   ITEXTCTL_GetText(pMe->m_pITextMsg, pMe->m_wszStringBuffer, BUFFER_LENGTH);

   if (pMe->m_payloadEncoding == AEE_ENC_UNICODE) {
      FREEIF(pMe->m_pwszMessage);
      if (NULL == (pMe->m_pwszMessage = 
               (AECHAR *) MALLOC((WSTRLEN(pMe->m_wszStringBuffer)+ 1)*sizeof(AECHAR)) )) {
         DBGPRINTF("Couldn't allocate memory");
         return ENOMEMORY;
      } else {
         WSTRNCOPYN(pMe->m_pwszMessage, (WSTRLEN(pMe->m_wszStringBuffer)+ 1) * sizeof(AECHAR), 
                     pMe->m_wszStringBuffer, -1);

         // Initialize text pointer and unsent length (for segmentation)
         pMe->m_nUnsentTextLength = WSTRLEN(pMe->m_pwszMessage);
         pMe->m_pwText = pMe->m_pwszMessage;
      }
   } else {
      FREEIF(pMe->m_pszMessage);
      if (NULL == (pMe->m_pszMessage = 
               (char *) MALLOC( WSTRLEN(pMe->m_wszStringBuffer) + 1)) ) {
         DBGPRINTF("Couldn't allocate memory");
         return ENOMEMORY;
      } else {
         WSTRTOSTR(pMe->m_wszStringBuffer, pMe->m_pszMessage, WSTRLEN(pMe->m_wszStringBuffer) + 1);
         // Initialize text pointer and unsent length (for segmentation)
         pMe->m_nUnsentTextLength = STRLEN(pMe->m_pszMessage);
         pMe->m_pText = pMe->m_pszMessage;

      }   
   }

   return AEE_SUCCESS;
}

/*===========================================================================
FUNCTION c_samplesms_AddPayloadSzToMessage
DESCRIPTION
   When payload encoding is AEE_ENC_ISOLATIN1, 
   this function will add the payload to a message. 
   Message segmentation calculation (if needed) is performed here.

PROTOTYPE:
   void c_samplesms_AddPayloadSzToMessage(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_AddPayloadSzToMessage(CSampleSMS *pMe) {         
   SMSMsgOpt smo[2];
   char szTextBuff[256];
   int maxLoad = pMe->m_nMaxPayloadBytes - UDH_CONCAT_LENGTH;
   int objLength = 0;
   int objPosInBytes = 0;

   MEMSET(szTextBuff, 0, 256);

   // ***************************************************************************
   // If total message = 1; doesn't need to worry about segmentation
   // ***************************************************************************
   if (pMe->m_totalMsg == 1) {
      STRNCPY(szTextBuff, (const char *) pMe->m_pText, pMe->m_nUnsentTextLength);
      smo[0].nId = MSGOPT_PAYLOAD_SZ;
      smo[0].pVal = szTextBuff;
      ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);

      if (pMe->m_objPos >=0) {
         c_samplesms_AddObjectToMessage(pMe, pMe->m_emsObj.header_id, pMe->m_objPos);
         pMe->m_objPos = -1;   // object has been loaded
      }
      return;
   }

   switch (pMe->m_emsObj.header_id) {
      case AEESMS_UDH_SMALL_PICTURE:
         objLength = UDH_SM_PIC_ANIM_LENGTH;
         break;
      case AEESMS_UDH_LARGE_PICTURE:
         objLength = UDH_LG_PIC_ANIM_LENGTH;
         break;
   }

   smo[1].nId = MSGOPT_END;
   smo[1].pVal = NULL;
   
   objPosInBytes = (pMe->m_objPos * pMe->m_calculationFactor)/8;
   if (pMe->m_objPos % 8 != 0) {
      objPosInBytes++;
   }
   
   if (pMe->m_seqNum > 1) {
      ISMSMSG_RemoveOpt(pMe->m_pISMSMsg, MSGOPT_PAYLOAD_SZ);
      ISMSMSG_RemoveOpt(pMe->m_pISMSMsg, MSGOPT_UDH_USERDEFINED);
   }

   if (objPosInBytes > maxLoad) {      
      // ***************************************************************************
      // m_nUnsentLength > maxLoad; object position is beyond maxLoad
      // ***************************************************************************
      STRNCPY(szTextBuff, (const char *) pMe->m_pText, maxLoad * 8/pMe->m_calculationFactor);
      pMe->m_nUnsentTextLength -= maxLoad * 8/pMe->m_calculationFactor;
      pMe->m_pText += maxLoad * 8/pMe->m_calculationFactor;
      pMe->m_objPos -= maxLoad * 8/pMe->m_calculationFactor;
   }
      
   else if ((pMe->m_objPos >=0) && (objLength > (maxLoad - objPosInBytes)) ) {
      // ***************************************************************************
      // maxLoad hasn't been reached, but adding object will overload the payload 
      // ***************************************************************************
      STRNCPY(szTextBuff, (const char *) pMe->m_pText, pMe->m_objPos);
      pMe->m_nUnsentTextLength -= pMe->m_objPos;
      pMe->m_pText += pMe->m_objPos;
      pMe->m_objPos = 0;   // object will go to next sequence with position = 0
         
   } 
   
   else if ((pMe->m_objPos >=0) && (objLength <= (maxLoad - objPosInBytes)) ) {
      // ***************************************************************************
      // sending some text + object
      // ***************************************************************************
      if ((pMe->m_nUnsentTextLength * pMe->m_calculationFactor/8) >= (maxLoad - objLength)) {
         // ***********************************************************************
         // full payload: text + object
         // ***********************************************************************
         STRNCPY(szTextBuff, (const char *) pMe->m_pText, 
            ((maxLoad - objLength) * pMe->m_calculationFactor/8));
         pMe->m_nUnsentTextLength -= ((maxLoad - objLength) * pMe->m_calculationFactor/8);
         pMe->m_pText += ((maxLoad - objLength) * pMe->m_calculationFactor/8);
      
         c_samplesms_AddObjectToMessage(pMe, pMe->m_emsObj.header_id, pMe->m_objPos);
         pMe->m_objPos = -1;   // object has been loaded
      } 
      else {
         // ***********************************************************************
         // total payload < maxLoad -> sending remaining text
         // ***********************************************************************
         STRNCPY(szTextBuff, (const char *) pMe->m_pText, pMe->m_nUnsentTextLength);
         pMe->m_nUnsentTextLength = 0;
         pMe->m_pText += pMe->m_nUnsentTextLength;
      
         c_samplesms_AddObjectToMessage(pMe, pMe->m_emsObj.header_id, pMe->m_objPos);
         pMe->m_objPos = -1;   // object has been loaded
      }
   } 
      
   else {
      // ***************************************************************************
      // no object or object has been loaded; only need to send remaining text
      // ***************************************************************************
      if (pMe->m_nUnsentTextLength >= (maxLoad * 8/pMe->m_calculationFactor) ) {
         STRNCPY(szTextBuff, (const char *) pMe->m_pText, 
            (maxLoad * 8/pMe->m_calculationFactor));
         pMe->m_nUnsentTextLength -= maxLoad * 8/pMe->m_calculationFactor;
         pMe->m_pText += maxLoad * 8/pMe->m_calculationFactor;               
      } else {
         STRNCPY(szTextBuff, (const char *) pMe->m_pText, pMe->m_nUnsentTextLength);
         pMe->m_nUnsentTextLength = 0;
         pMe->m_pText += pMe->m_nUnsentTextLength;
      }
   }

   smo[0].nId = MSGOPT_PAYLOAD_SZ;
   smo[0].pVal = szTextBuff;
   ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);
}

/*===========================================================================
FUNCTION c_samplesms_AddPayloadWszToMessage
DESCRIPTION
   When payload encoding is UNICODE,
   this function will add the payload to a message. 
   Message segmentation calculation (if needed) is performed here.

PROTOTYPE:
   void c_samplesms_AddPayloadWszToMessage(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_AddPayloadWszToMessage(CSampleSMS *pMe)  {
   SMSMsgOpt smo[2];
   AECHAR wszTextBuff[BUFFER_LENGTH];
   int maxLoad = pMe->m_nMaxPayloadBytes - UDH_CONCAT_LENGTH;
   int objLength = 0;
   int objPosInBytes = 0;

   MEMSET(wszTextBuff, 0, BUFFER_LENGTH);

   // ***************************************************************************
   // If total message = 1; doesn't need to worry about segmentation
   // ***************************************************************************
   if (pMe->m_totalMsg == 1) {
      WSTRNCOPYN(wszTextBuff, sizeof(wszTextBuff), pMe->m_pwText, pMe->m_nUnsentTextLength);
      smo[0].nId = MSGOPT_PAYLOAD_WSZ;
      smo[0].pVal = wszTextBuff;
      ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);

      if (pMe->m_objPos >=0) {
         c_samplesms_AddObjectToMessage(pMe, pMe->m_emsObj.header_id, pMe->m_objPos);
         pMe->m_objPos = -1;   // object has been loaded
      }
      return;
   }


   switch (pMe->m_emsObj.header_id) {
      case AEESMS_UDH_SMALL_PICTURE:
         objLength = UDH_SM_PIC_ANIM_LENGTH;
         break;
      case AEESMS_UDH_LARGE_PICTURE:
         objLength = UDH_LG_PIC_ANIM_LENGTH;
         break;
   }

   smo[1].nId = MSGOPT_END;
   smo[1].pVal = NULL;
   
   objPosInBytes = (pMe->m_objPos * pMe->m_calculationFactor)/8;
   if (pMe->m_objPos % 8 != 0) {
      objPosInBytes++;
   }
   
   if (pMe->m_seqNum > 1) {
      ISMSMSG_RemoveOpt(pMe->m_pISMSMsg, MSGOPT_PAYLOAD_WSZ);
      ISMSMSG_RemoveOpt(pMe->m_pISMSMsg, MSGOPT_UDH_USERDEFINED);
   }
   if (objPosInBytes > maxLoad) {      
      // ***************************************************************************
      // m_nUnsentLength > maxLoad; object position is beyond maxLoad
      // ***************************************************************************
      WSTRNCOPYN(wszTextBuff, sizeof(wszTextBuff), pMe->m_pwText, maxLoad * 8/pMe->m_calculationFactor);
      pMe->m_nUnsentTextLength -= maxLoad * 8/pMe->m_calculationFactor;
      pMe->m_pwText += maxLoad * 8/pMe->m_calculationFactor;
      pMe->m_objPos -= maxLoad * 8/pMe->m_calculationFactor;
   }
      
   else if ((pMe->m_objPos >=0) && (objLength > (maxLoad - objPosInBytes)) ) {
      // ***************************************************************************
      // maxLoad hasn't been reached, but adding object will overload the payload 
      // ***************************************************************************
      WSTRNCOPYN(wszTextBuff, sizeof(wszTextBuff), pMe->m_pwText, pMe->m_objPos);
      pMe->m_nUnsentTextLength -= pMe->m_objPos;
      pMe->m_pwText += pMe->m_objPos;
      pMe->m_objPos = 0;   // object will go to next sequence with position = 0
      
   } 
   
   else if ((pMe->m_objPos >=0) && (objLength <= (maxLoad - objPosInBytes)) ) {
      // ***************************************************************************
      // sending some text + object
      // ***************************************************************************
      if ((pMe->m_nUnsentTextLength * pMe->m_calculationFactor/8) >= (maxLoad - objLength) ) {
         // ***********************************************************************
         // full payload: text + object
         // ***********************************************************************
         WSTRNCOPYN(wszTextBuff, sizeof(wszTextBuff), pMe->m_pwText, 
            ((maxLoad - objLength) * pMe->m_calculationFactor/8));
         pMe->m_nUnsentTextLength -= ((maxLoad - objLength) * pMe->m_calculationFactor/8);
         pMe->m_pwText += ((maxLoad - objLength) * pMe->m_calculationFactor/8);
               
         c_samplesms_AddObjectToMessage(pMe, pMe->m_emsObj.header_id, pMe->m_objPos);
         pMe->m_objPos = -1;   // object has been loaded
         } 
      else {
         // ***********************************************************************
         // total payload < maxLoad -> sending remaining text
         // ***********************************************************************
         WSTRNCOPYN(wszTextBuff, sizeof(wszTextBuff), pMe->m_pwText, pMe->m_nUnsentTextLength);
         pMe->m_nUnsentTextLength = 0;
         pMe->m_pwText += pMe->m_nUnsentTextLength;
               
         c_samplesms_AddObjectToMessage(pMe, pMe->m_emsObj.header_id, pMe->m_objPos);
         pMe->m_objPos = -1;   // object has been loaded
      }
   } 
      
   else {
      // ***************************************************************************
      // no object or object has been loaded; only need to send remaining text
      // ***************************************************************************
      if (pMe->m_nUnsentTextLength >= (maxLoad * 8/pMe->m_calculationFactor) ) {
         WSTRNCOPYN(wszTextBuff, sizeof(wszTextBuff), pMe->m_pwText, 
            (maxLoad * 8/pMe->m_calculationFactor));
         pMe->m_nUnsentTextLength -= maxLoad * 8/pMe->m_calculationFactor;
         pMe->m_pwText += maxLoad * 8/pMe->m_calculationFactor;

      } else {
         WSTRNCOPYN(wszTextBuff, sizeof(wszTextBuff), pMe->m_pwText, pMe->m_nUnsentTextLength);
         pMe->m_nUnsentTextLength = 0;
         pMe->m_pwText += pMe->m_nUnsentTextLength;
      }
   }
   smo[0].nId = MSGOPT_PAYLOAD_WSZ;
   smo[0].pVal = wszTextBuff;   
   ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);
}

/*===========================================================================
FUNCTION c_samplesms_AddPayloadBinaryToMessage
DESCRIPTION
   When payload encoding is neither AEE_ENC_ISOLATIN1 or AEE_ENC_UNICODE, 
   this function will add the payload to a message. 
   Message segmentation calculation (if needed) is performed here.

PROTOTYPE:
   void c_samplesms_AddPayloadBinaryToMessage(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_AddPayloadBinaryToMessage(CSampleSMS *pMe) {         
   SMSMsgOpt smo[2];
   char szTextBuff[256];
   int maxLoad = pMe->m_nMaxPayloadBytes - UDH_CONCAT_LENGTH;
   int objLength = 0;
   int objPosInBytes = 0;

   MEMSET(szTextBuff, 0, 256);

   // ***************************************************************************
   // If total message = 1; doesn't need to worry about segmentation
   // ***************************************************************************
   if (pMe->m_totalMsg == 1) {
        WEBOPT_SETVARBUFFER(szTextBuff, pMe->m_pText, pMe->m_nUnsentTextLength);
      smo[0].nId = MSGOPT_PAYLOAD_BINARY;
      smo[0].pVal = (void *)szTextBuff;
      ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);

      if (pMe->m_objPos >=0) {
         c_samplesms_AddObjectToMessage(pMe, pMe->m_emsObj.header_id, pMe->m_objPos);
         pMe->m_objPos = -1;   // object has been loaded
      }
      return;
   }

   switch (pMe->m_emsObj.header_id) {
      case AEESMS_UDH_SMALL_PICTURE:
         objLength = UDH_SM_PIC_ANIM_LENGTH;
         break;
      case AEESMS_UDH_LARGE_PICTURE:
         objLength = UDH_LG_PIC_ANIM_LENGTH;
         break;
   }

   smo[1].nId = MSGOPT_END;
   smo[1].pVal = NULL;
   

   objPosInBytes = (pMe->m_objPos * pMe->m_calculationFactor)/8;
   if (pMe->m_objPos % 8 != 0) {
      objPosInBytes++;
   }
   
   if (pMe->m_seqNum > 1) {
      ISMSMSG_RemoveOpt(pMe->m_pISMSMsg, MSGOPT_PAYLOAD_SZ);
      ISMSMSG_RemoveOpt(pMe->m_pISMSMsg, MSGOPT_UDH_USERDEFINED);
   }

   if (objPosInBytes > maxLoad) {      
      // ***************************************************************************
      // m_nUnsentLength > maxLoad; object position is beyond maxLoad
      // ***************************************************************************
      WEBOPT_SETVARBUFFER(szTextBuff, pMe->m_pText, maxLoad * 8/pMe->m_calculationFactor);
      pMe->m_nUnsentTextLength -= maxLoad * 8/pMe->m_calculationFactor;
      pMe->m_pText += maxLoad * 8/pMe->m_calculationFactor;
      pMe->m_objPos -= maxLoad * 8/pMe->m_calculationFactor;  
   }
      
   else if ((pMe->m_objPos >=0) && (objLength > (maxLoad - objPosInBytes)) ) {
      // ***************************************************************************
      // maxLoad hasn't been reached, but adding object will overload the payload 
      // ***************************************************************************
      WEBOPT_SETVARBUFFER(szTextBuff, pMe->m_pText, pMe->m_objPos);
      pMe->m_nUnsentTextLength -= pMe->m_objPos;
      pMe->m_pText += pMe->m_objPos;
      pMe->m_objPos = 0;   // object will go to next sequence with position = 0
   } 
   
   else if ((pMe->m_objPos >=0) && (objLength <= (maxLoad - objPosInBytes)) ) {
      // ***************************************************************************
      // sending some text + object
      // ***************************************************************************
      if ((pMe->m_nUnsentTextLength * pMe->m_calculationFactor/8) >= (maxLoad - objLength)) {
         // ***********************************************************************
         // full payload: text + object
         // ***********************************************************************
         WEBOPT_SETVARBUFFER(szTextBuff, pMe->m_pText, 
            ((maxLoad - objLength) * pMe->m_calculationFactor/8));
         pMe->m_nUnsentTextLength -= ((maxLoad - objLength) * pMe->m_calculationFactor/8);
         pMe->m_pText += ((maxLoad - objLength) * pMe->m_calculationFactor/8);
      
         c_samplesms_AddObjectToMessage(pMe, pMe->m_emsObj.header_id, pMe->m_objPos);
         pMe->m_objPos = -1;   // object has been loaded
      } 
      else {
         // ***********************************************************************
         // total payload < maxLoad -> sending remaining text
         // ***********************************************************************
         WEBOPT_SETVARBUFFER(szTextBuff, pMe->m_pText, pMe->m_nUnsentTextLength);
         pMe->m_nUnsentTextLength = 0;
         pMe->m_pText += pMe->m_nUnsentTextLength;
            
         c_samplesms_AddObjectToMessage(pMe, pMe->m_emsObj.header_id, pMe->m_objPos);
         pMe->m_objPos = -1;   // object has been loaded
      }
   } 
      
   else {
      // ***************************************************************************
      // no object or object has been loaded; only need to send remaining text
      // ***************************************************************************
      if (pMe->m_nUnsentTextLength >= (maxLoad * 8/pMe->m_calculationFactor) ) {
         WEBOPT_SETVARBUFFER(szTextBuff, pMe->m_pText, 
            (maxLoad * 8/pMe->m_calculationFactor));
         pMe->m_nUnsentTextLength -= maxLoad * 8/pMe->m_calculationFactor;
         pMe->m_pText += maxLoad * 8/pMe->m_calculationFactor;
                  
      } else {
         WEBOPT_SETVARBUFFER(szTextBuff, pMe->m_pText, pMe->m_nUnsentTextLength);
         pMe->m_nUnsentTextLength = 0;
         pMe->m_pText += pMe->m_nUnsentTextLength;
      }
   }

   smo[0].nId = MSGOPT_PAYLOAD_BINARY;
   smo[0].pVal = (void *)szTextBuff;
   ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);
}


/*====================================================================================
FUNCTION c_samplesms_AddObjectToMessage
DESCRIPTION
   This function will add EMS object to the ISMSMsg.

PROTOTYPE:
   void c_samplesms_AddPayObjectToMessage(CSampleSMS * pi, int objType, int position);
======================================================================================*/
void c_samplesms_AddObjectToMessage(CSampleSMS *pMe, int objType, int position) {
   unsigned char smpicbuff[UDH_SM_PIC_ANIM_LENGTH+1];
   unsigned char lgpicbuff[UDH_LG_PIC_ANIM_LENGTH+1];
   SMSMsgOpt smo[2];

   smo[1].nId = MSGOPT_END;
   smo[1].pVal = NULL;
   
   switch (objType) {
      case AEESMS_UDH_SMALL_PICTURE:
         MSGOPT_SETUDHSMALLPICBUFFER(smpicbuff,position,   
               pMe->m_emsObj.u.small_picture.data);
         smo[0].nId = MSGOPT_UDH_USERDEFINED;   
         smo[0].pVal = (void *)smpicbuff;
         ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);
         break;

      case AEESMS_UDH_LARGE_PICTURE:
         MSGOPT_SETUDHLARGEPICBUFFER(lgpicbuff, position,
               pMe->m_emsObj.u.large_picture.data);
         smo[0].nId = MSGOPT_UDH_USERDEFINED;
         smo[0].pVal = (void *)lgpicbuff;
         ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);
         break;
   }
}

/*====================================================================================
FUNCTION c_samplesms_AddConcatUDH
DESCRIPTION
   This function will add concatenation info of a segmented message to the ISMSMsg.

PROTOTYPE:
   void c_samplesms_AddConcatUDH(CSampleSMS * pi);
======================================================================================*/
void c_samplesms_AddConcatUDH(CSampleSMS *pMe) {
   unsigned char concbuff[UDH_CONCAT_LENGTH+1];
   SMSMsgOpt smo[2];

   smo[1].nId = MSGOPT_END;
   smo[1].pVal = NULL;

   DBGPRINTF("sending msg %d of %d", pMe->m_seqNum, pMe->m_totalMsg);
   
   if (pMe->m_payloadEncoding == AEE_ENC_UNICODE) {
      MSGOPT_SETUDHCONCAT16BUFFER(concbuff,pMe->m_seqNum,pMe->m_totalMsg,pMe->m_refNum);      
   } else {
      MSGOPT_SETUDHCONCAT8BUFFER(concbuff,pMe->m_seqNum,pMe->m_totalMsg,pMe->m_refNum);   
   }

   ISMSMSG_RemoveOpt(pMe->m_pISMSMsg, MSGOPT_UDH_CONCAT);

   smo[0].nId = MSGOPT_UDH_CONCAT;
   smo[0].pVal = concbuff;
   ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);

}


/*===========================================================================
FUNCTION c_samplesms_SendSMS
DESCRIPTION
   This function will encapsulate a message in ISMSMsg instance, 
   and execute the message sending
   
PROTOTYPE:
   void c_samplesms_SendSMS(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_SendSMS(CSampleSMS *pMe) {
   SMSMsgOpt smo[2];

   smo[1].nId = MSGOPT_END;
   smo[1].pVal = NULL;

   // ***************************************************************************************
   // Get destination number
   // - for multipart messages, destination number only set once
   // ***************************************************************************************
   // If this is the first part of message, we haven't set destination number
   if (pMe->m_seqNum == 1) {
      //*************************************
      // ## Setting destination number ##
      //*************************************
      MEMSET(pMe->m_wszStringBuffer, 0, sizeof(pMe->m_wszStringBuffer));
      switch (pMe->m_destIndex) {   
         case 1:
            ITEXTCTL_GetText(pMe->m_pITextDest1, pMe->m_wszStringBuffer, MAX_DEST_NUM_LENGTH);
            if (NULL == (pMe->m_pszDestNum = (char *) MALLOC( WSTRLEN(pMe->m_wszStringBuffer) + 1)) ) {
               DBGPRINTF("Couldn't allocate memory");
               return;
            } else {
               WSTRTOSTR(pMe->m_wszStringBuffer, pMe->m_pszDestNum, 
                  WSTRLEN(pMe->m_wszStringBuffer) + 1);
            }   
            break;
         case 2:
            // Check if have another destination to send the message
            ITEXTCTL_GetText(pMe->m_pITextDest2, pMe->m_wszStringBuffer, MAX_DEST_NUM_LENGTH); 
            if ( WSTRLEN(pMe->m_wszStringBuffer) != 0 ) {
               if (NULL == (pMe->m_pszDestNum = 
                        (char *) MALLOC( WSTRLEN(pMe->m_wszStringBuffer) + 1)) ) {
                  DBGPRINTF("Couldn't allocate memory");
                  return;
               } else {
                  WSTRTOSTR(pMe->m_wszStringBuffer, pMe->m_pszDestNum, 
                     WSTRLEN(pMe->m_wszStringBuffer) + 1);
               }
            } else {
               ISHELL_SendEvent(pMe->m_pIShell, AEECLSID_C_SAMPLESMS, 
                  EVT_SENDING_DONE, 0, 0);
               return;
            }
            break;

         default:
            return;
      }

      if (pMe->m_destIndex > 1) {
         ISMSMSG_RemoveOpt(pMe->m_pISMSMsg, MSGOPT_TO_DEVICE_SZ);
      }
      smo[0].nId = MSGOPT_TO_DEVICE_SZ;
      smo[0].pVal = pMe->m_pszDestNum;
      ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);
      // pMe->m_pszDestNum will be freed at StoreMessageCB, 
      // - after sending (+storing) to one destination is done
   }

    DBGPRINTF("payload enc = %d", pMe->m_payloadEncoding);
    DBGPRINTF("mo enc = %d", pMe->m_moEncoding);

   if ( (pMe->m_destIndex) == 1 && (pMe->m_seqNum == 1) ) {
      pMe->m_appstate = APPSTATE_SENDING;
        c_samplesms_SetMessageOptions(pMe);
        // at this point, we haven't extracted the message body
        if (AEE_SUCCESS != c_samplesms_ExtractTextMessage(pMe)) {
         c_samplesms_ShowStatus(pMe, "Couldn't process message body!");  
         return;
        }   
   }

   if (pMe->m_payloadEncoding == AEE_ENC_UNICODE) {
          c_samplesms_AddPayloadWszToMessage(pMe);
      } else if (pMe->m_payloadEncoding == AEE_ENC_ISOLATIN1) {
       c_samplesms_AddPayloadSzToMessage(pMe);
    } else {
       c_samplesms_AddPayloadBinaryToMessage(pMe);
    }

   if (pMe->m_totalMsg > 1) {
      c_samplesms_AddConcatUDH(pMe);
   }
  
   c_samplesms_ShowStatus(pMe, "Sending message...");
   CALLBACK_Cancel(&pMe->m_callback);
   CALLBACK_Init(&pMe->m_callback, (PFNNOTIFY)c_samplesms_SendSMSCB, pMe);
   ISMS_SendMsg(pMe->m_pISMS, pMe->m_pISMSMsg, &pMe->m_callback, &pMe->m_retVal);
   
}

/*===========================================================================
FUNCTION c_samplesms_SendSMSCB
DESCRIPTION
   This is a callback function that fires after the message is sent. 
   Status of message sending can be retrieved here.
   The procedure is continued with storing a message tagged as a sent message
   
PROTOTYPE:
   void c_samplesms_SendSMSCB(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_SendSMSCB(CSampleSMS *pMe) {
   uint16 errType = AEESMS_GETERRORTYPE(pMe->m_retVal);
   uint16 err = AEESMS_GETERROR(pMe->m_retVal);

   SMSMsgOpt smo[2];
   int dwSecs = 0;

   smo[1].nId = MSGOPT_END;
   smo[1].pVal = NULL;

   switch(err) {
      case AEESMS_ERROR_NONE:
         DBGPRINTF("Sent OK");
         break;

      default:
         DBGPRINTF("Error type: %d \n", errType);
         DBGPRINTF("Error = %d \n", err);
         c_samplesms_ShowStatus(pMe, "Error!");
         break;
   }

   if (pMe->m_bEditMode) {
      c_samplesms_SetMessageTag(pMe->m_pISMSMsg, AEESMS_TAG_MO_SENT);
      c_samplesms_UpdateMessage(pMe);
      pMe->m_bEditMode = FALSE;
   } else {
      c_samplesms_SetMessageTag(pMe->m_pISMSMsg, AEESMS_TAG_MO_SENT);
      
	  // ----------------------------------------------
	  // Add the timestamp before storing the message
	  // ----------------------------------------------
      dwSecs = GETUTCSECONDS() + LOCALTIMEOFFSET(NULL);
      GETJULIANDATE(dwSecs, &pMe->date);
	  if (pMe->m_storageType == AEESMS_NV_CDMA) {
	     smo[0].nId = MSGOPT_TIMESTAMP;
	  } else {
	     smo[0].nId = MSGOPT_GW_TIMESTAMP;  
	  }
	  smo[0].pVal = (void *)&pMe->date;
      ISMSMSG_AddOpt(pMe->m_pISMSMsg, smo);
      
	  c_samplesms_StoreMessage(pMe);
   }
}


/*====================================================================================
FUNCTION c_samplesms_ReceiveSMSMessage
DESCRIPTION
   This function retrieves the incoming message after the application receives 
   EVT_NOTIFY issued by AEECLSID_SMSNOTIFIER with NMASK_SMS_TYPE of 
   AEESMS_TYPE_TEXT or AEESMS_TYPE_EMS.
   The procedure is continued with storing a message tagged as AEESMS_TAG_MT_NOT_READ.

PROTOTYPE:
   void c_samplesms_ReceiveSMSMessage(CSampleSMS * pi, uint32 messageID);
======================================================================================*/
void c_samplesms_ReceiveSMSMessage(CSampleSMS *pMe, uint32 messageID) {
   SMSMsgOpt smo;

   DBGPRINTF("receive message with id = %d", messageID);

   if (pMe->m_pISMSMsgRcv) {
      ISMSMSG_Release(pMe->m_pISMSMsgRcv);
      pMe->m_pISMSMsgRcv = NULL;
   }

   // Encapsulate the SMS message in an ISMSMsg interface
   if (SUCCESS == ISMS_ReceiveMsg(pMe->m_pISMS, messageID, &pMe->m_pISMSMsgRcv)) {
      if (ISMSMSG_GetOpt(pMe->m_pISMSMsgRcv, MSGOPT_TAG, &smo) == SUCCESS ) {
         DBGPRINTF("msg recvd with tag: %d", (int) smo.pVal);
      } else {
         DBGPRINTF("Couldn't retrieve msg tag");
      }   
      c_samplesms_SetMessageTag(pMe->m_pISMSMsgRcv, AEESMS_TAG_MT_NOT_READ);
      c_samplesms_StoreRcvdMessage(pMe);

   } else {
      DBGPRINTF("Receive msg - error !");
   }
   return;
}


