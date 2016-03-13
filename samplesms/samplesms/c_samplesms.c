/*===========================================================================
FILE: c_samplesms.c

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

   This file represents the main program providing the application 
   initialization, application event handler, and application user interface.


                  Copyright ?2008 QUALCOMM Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================*/


#include "c_samplesms.h"
#include "c_smsstorage.h"
#include "c_sendreceive.h"
#include "c_conversionEmsObj.h"

/*===========================================================================
                            FUNCTION PROTOTYPES
============================================================================= */
boolean c_samplesms_InitAppData(CSampleSMS* pMe);
void    c_samplesms_FreeAppData(CSampleSMS* pMe);
static  boolean c_samplesms_HandleEvent(CSampleSMS* pMe, AEEEvent eCode, 
                                      uint16 wParam, uint32 dwParam);
int c_samplesms_InitializeMessaging(CSampleSMS *pMe);
void c_samplesms_MainMenu(CSampleSMS*pMe);
void c_samplesms_SetOptionsMenu(CSampleSMS *pMe);
void c_samplesms_ShowOptionsMenu(CSampleSMS *pMe);
void c_samplesms_ShowMOEncodingsMenu(CSampleSMS *pMe);
void c_samplesms_SetEMSObjectsMenu(CSampleSMS *pMe);
void c_samplesms_ShowEMSObjectsMenu(CSampleSMS *pMe);
void c_samplesms_ListInboxMessages(CSampleSMS *pMe);
void c_samplesms_ListOutboxMessages(CSampleSMS *pMe);
void c_samplesms_ListDrafts(CSampleSMS *pMe);


/*============================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
   This function is invoked while the app is being loaded. All Modules must provide this 
   function. Ensure to retain the same name and parameters for this function.
   In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
   that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. 
   c_samplesms_InitAppData() is called to initialize AppletData instance. 
   When exiting the application, c_samplesms_FreeAppData will be called
   to release any memory allocated (for data contained in AppletData)
   during application lifetime.

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
   clsID: [in]: Specifies the ClassID of the applet which is being loaded

   pIShell: [in]: Contains pointer to the IShell object. 

   pIModule: pin]: Contains pointer to the IModule object to the current module to which
   this app belongs

   ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
   of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
   none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
   *ppObj = NULL;

   if( ClsId == AEECLSID_C_SAMPLESMS )
   {
      // Create the applet and make room for the applet structure
      if( AEEApplet_New(sizeof(CSampleSMS),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)c_samplesms_HandleEvent,
                          (PFNFREEAPPDATA)c_samplesms_FreeAppData) ) 
                          
      {
         //Initialize applet data, this is called before sending EVT_APP_START
         // to the HandleEvent function
         if(c_samplesms_InitAppData((CSampleSMS*)*ppObj))
         {
            //Data initialized successfully
            return(AEE_SUCCESS);
         }
         else
         {
            //Release the applet. This will free the memory allocated for the applet when
            // AEEApplet_New was called.
            IAPPLET_Release((IApplet*)*ppObj);
            return EFAILED;
         }

        } // end AEEApplet_New

    }

   return(EFAILED);
}


/*===========================================================================

FUNCTION c_samplesms_HandleEvent

DESCRIPTION
   This is the EventHandler for this app. All events to this app are handled in this
   function. 

PROTOTYPE:
   boolean c_samplesms_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
   pi: Pointer to the AEEApplet structure. This structure contains information specific
   to this applet. It was initialized during the AEEClsCreateInstance() function.

   ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean c_samplesms_HandleEvent(CSampleSMS* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	int ret = FALSE;

    switch (eCode) 
   {
        case EVT_APP_START:
         // ***************************************************************
         // If application uses SMSStorage model listener,
         // it may choose to call the procedure to obtain
         // - message counters until EVT_MDL_SMSSTORAGE_READY is received
         // ****************************************************************
         c_samplesms_MainMenu(pMe);
         return(TRUE);


        case EVT_APP_STOP:
         // *****************************************************************
         // When a user is exiting the app while in Compose Screen,
         // - the app will save the message draft
         // *****************************************************************
         if ((pMe->m_appstate == APPSTATE_INPUT_MSG) ||
            (pMe->m_appstate == APPSTATE_INPUT_DEST1) ||
            (pMe->m_appstate == APPSTATE_INPUT_DEST2)) {
            if (pMe->m_bEditMode) {
               c_samplesms_SaveDraftData(pMe);
               c_samplesms_UpdateMessage(pMe);
               pMe->m_bEditMode = FALSE;
            } else {
               // ********************************************************
               // Check whether user has entered at least a char to 
               // - the input fields (dest# or msg body); 
               // whether or not should proceed with saving the draft
               // ********************************************************
               AECHAR temp1[3], temp2[3];
               MEMSET(temp1, 0, sizeof(temp1));
               MEMSET(temp2, 0, sizeof(temp2));
               ITEXTCTL_GetText(pMe->m_pITextDest1, temp1, 
                  (sizeof(temp1)/sizeof(AECHAR))-1);   
               ITEXTCTL_GetText(pMe->m_pITextMsg, temp2, 
                  (sizeof(temp2)/sizeof(AECHAR))-1);   
               if (( WSTRLEN(temp1) != 0 ) || ( WSTRLEN(temp2) != 0 )) {
                  if (AEE_SUCCESS == c_samplesms_CreateISMSMsgInstance(pMe)) {
                     c_samplesms_SetMessageOptions(pMe);
                     c_samplesms_SaveDraftData(pMe);
                     c_samplesms_SetMessageTag(pMe->m_pISMSMsg, AEESMS_TAG_MO_NOT_SENT);
                     c_samplesms_StoreMessage(pMe);
                  }
               }
            }
         }
         return(TRUE);

        case EVT_APP_SUSPEND:
            DBGPRINTF("App is suspended..");
            return(TRUE);

        case EVT_APP_RESUME:
          switch (pMe->m_appstate) {
            case APPSTATE_INPUT_MSG:
            case APPSTATE_EDIT_DRAFT:
               c_samplesms_BackToCompose(pMe);
               break;
            case APPSTATE_OPTIONS:
               c_samplesms_ShowOptionsMenu(pMe);
               break;
            case APPSTATE_SET_MOENC:
               c_samplesms_ShowMOEncodingsMenu(pMe);
               break;
            case APPSTATE_INBOX:
               c_samplesms_ListInboxMessages(pMe);
               break;
            case APPSTATE_OUTBOX:
               c_samplesms_ListOutboxMessages(pMe);
               break;
            case APPSTATE_READ_INBOX:
            case APPSTATE_READ_OUTBOX:
               break;
            case APPSTATE_DRAFTS:
               c_samplesms_ListDrafts(pMe);
               break;
            default:
               c_samplesms_BuildMainMenu(pMe);
               break;
          }
          return(TRUE);


        case EVT_APP_MESSAGE:
          return(TRUE);

      case EVT_NOTIFY:
      {
         // Cast dwParam as an AEENotify structure
         AEENotify *temp = (AEENotify *) dwParam;
      
         DBGPRINTF("evt_notify from %u...", (uint32) (temp->cls));
         
         // Check for proper mask and type of notify event
         if (AEECLSID_SMSNOTIFIER == temp->cls &&
            ( (((AEESMS_TYPE_TEXT << 16)| NMASK_SMS_TYPE) == temp->dwMask) ||
              (((AEESMS_TYPE_EMS << 16) | NMASK_SMS_TYPE) == temp->dwMask) ))   {
            DBGPRINTF("you've got a message!");
            c_samplesms_ReceiveSMSMessage(pMe, (uint32) temp->pData);
            return(TRUE);

            }

         // We received an EVT_NOTIFY that we did not intend to handle 
         return FALSE;
      }

        case EVT_KEY:
         if(pMe->m_pIMenuMain && IMENUCTL_IsActive(pMe->m_pIMenuMain)) {
            IMENUCTL_HandleEvent(pMe->m_pIMenuMain, eCode, wParam, dwParam);
         }

         if(pMe->m_pIMenuOptions && IMENUCTL_IsActive(pMe->m_pIMenuOptions)) {
            IMENUCTL_HandleEvent(pMe->m_pIMenuOptions, eCode, wParam, dwParam);
         }

         if(pMe->m_pIMenuEMSObjects && IMENUCTL_IsActive(pMe->m_pIMenuEMSObjects)) {
            IMENUCTL_HandleEvent(pMe->m_pIMenuEMSObjects, eCode, wParam, dwParam);
         }

         if(pMe->m_pIMenuInboxMsgs && IMENUCTL_IsActive(pMe->m_pIMenuInboxMsgs)) {
            IMENUCTL_HandleEvent(pMe->m_pIMenuInboxMsgs, eCode, wParam, dwParam);
         }

         if(pMe->m_pIMenuOutboxMsgs && IMENUCTL_IsActive(pMe->m_pIMenuOutboxMsgs)) {
            IMENUCTL_HandleEvent(pMe->m_pIMenuOutboxMsgs, eCode, wParam, dwParam);
         }

         if(pMe->m_pIMenuDrafts && IMENUCTL_IsActive(pMe->m_pIMenuDrafts)) {
            IMENUCTL_HandleEvent(pMe->m_pIMenuDrafts, eCode, wParam, dwParam);
         }

         if(pMe->m_pIMenuMOEncodings && IMENUCTL_IsActive(pMe->m_pIMenuMOEncodings)) {
            IMENUCTL_HandleEvent(pMe->m_pIMenuMOEncodings, eCode, wParam, dwParam);
         }
         
         if(pMe->m_pITextDest1 && ITEXTCTL_IsActive(pMe->m_pITextDest1)) {
            if ((wParam == AVK_UP) || (wParam == AVK_DOWN) || (wParam == AVK_SELECT)) { 
               ITEXTCTL_HandleEvent(pMe->m_pITextDest1, eCode, wParam, dwParam);
            }
            else {
				ret = ITEXTCTL_HandleEvent(pMe->m_pITextDest1, eCode, wParam, dwParam);
				if (ret == TRUE)
					return ret;
            }
         }

         if(pMe->m_pITextDest2 && ITEXTCTL_IsActive(pMe->m_pITextDest2)) {
            if ((wParam == AVK_UP) || (wParam == AVK_DOWN) || (wParam == AVK_SELECT)) { 
               ITEXTCTL_HandleEvent(pMe->m_pITextDest2, eCode, wParam, dwParam);
            }
            else {
               ret = ITEXTCTL_HandleEvent(pMe->m_pITextDest2, eCode, wParam, dwParam);
			   if (ret == TRUE)
					return ret;
            }
         }

         if(pMe->m_pITextMsg && ITEXTCTL_IsActive(pMe->m_pITextMsg)) {
            switch (wParam) {
               case AVK_UP:
               case AVK_DOWN:
               case AVK_SELECT:
               case AVK_SOFT1:
               case AVK_SOFT2:
                  ITEXTCTL_HandleEvent(pMe->m_pITextMsg, eCode, wParam, dwParam);
                  break;
               default:
				   {
						ret = ITEXTCTL_HandleEvent(pMe->m_pITextMsg, eCode, wParam, dwParam);
						if (ret == TRUE)
							return ret;
				   }
            }
         }
         
         switch (wParam) {
            case AVK_SOFT1:
               // ***********************************************************************
               // If user is currently in Inbox/ Outbox/ Drafts
               // - LSK is available for Deleting the selected message
               // ***********************************************************************
               if ((pMe->m_appstate == APPSTATE_INBOX) || 
                  (pMe->m_appstate == APPSTATE_OUTBOX) ||
                  (pMe->m_appstate == APPSTATE_DRAFTS)) {
                  // ### LSK = Delete Msg ###
                  uint16 selectedMsg;
                  if (pMe->m_appstate == APPSTATE_INBOX) {
                     selectedMsg = IMENUCTL_GetSel(pMe->m_pIMenuInboxMsgs);
                  } else if (pMe->m_appstate == APPSTATE_OUTBOX) {
                     selectedMsg = IMENUCTL_GetSel(pMe->m_pIMenuOutboxMsgs);
                  } else {
                     selectedMsg = IMENUCTL_GetSel(pMe->m_pIMenuDrafts);
                  }
                  c_samplesms_DeleteMessage(pMe, selectedMsg);
               }
               // ***********************************************************************
               // If user is in text message field, 
               // - LSK is available for sending the composed message
               // Sending a message requires at least one destination number field 
               // ***********************************************************************
               else if (pMe->m_appstate == APPSTATE_INPUT_MSG) {
                  // ### LSK = Send Msg ###
                  // ********************************************************
                  // Check whether user has entered a destination number 
                  // ********************************************************
                        AECHAR temp[3];
                  MEMSET(temp, 0, sizeof(temp));
                  ITEXTCTL_GetText(pMe->m_pITextDest1, temp, 
                     (sizeof(temp)/sizeof(AECHAR))-1);   
                  if ( WSTRLEN(temp) != 0 ) {
                     if (AEE_SUCCESS == c_samplesms_InitializeSending(pMe) ){
                        c_samplesms_SendSMS(pMe);
                     }
                  } else {
                     // Warn user, then go back to compose screen
                     c_samplesms_ShowStatus(pMe, "Message has no destination number!");
                  }
               }

               break;

            case AVK_SOFT2:
               // ***********************************************************************
               // If user is currently in Main Menu,
               // - RSK is available for accessing Options Menu 
               // ***********************************************************************
               if (pMe->m_appstate == APPSTATE_MAIN) {
                  pMe->m_appstate = APPSTATE_OPTIONS;
                  // ### RSK = Options ###
                  c_samplesms_ShowOptionsMenu(pMe);
               }

               // ***********************************************************************
               // If user is currently in Inbox/Outbox/Drafts,
               // - RSK is available for reading the message from the SMSStorage
               // Reading a message from SMSStorage is asychronous call,
               // once the callback fires:
               // 1) (from inbox/outbox) application will display the message
               // 2) (from drafts) application will let the user to edit the message
               //    (bring the user to Compose Screen
               // ***********************************************************************

               else if ((pMe->m_appstate == APPSTATE_INBOX) || 
                        (pMe->m_appstate == APPSTATE_OUTBOX)||
                        (pMe->m_appstate == APPSTATE_DRAFTS)) {
                  // ### RSK = Read Message ###
                  uint16 selectedMsg;
                  if (pMe->m_appstate == APPSTATE_INBOX) {
                     selectedMsg = IMENUCTL_GetSel(pMe->m_pIMenuInboxMsgs);
                  } else if (pMe->m_appstate == APPSTATE_OUTBOX) {
                     selectedMsg = IMENUCTL_GetSel(pMe->m_pIMenuOutboxMsgs);
                  } else {
                     selectedMsg = IMENUCTL_GetSel(pMe->m_pIMenuDrafts);
                  }

                  c_samplesms_ReadMessage(pMe, selectedMsg);
               }

               else if (pMe->m_appstate == APPSTATE_INPUT_MSG) {
                  // ### RSK = Insert Object ###
                  // Let user pick the EMS object to insert
                  if (pMe->m_objPos < 0) {   
                     c_samplesms_ShowEMSObjectsMenu(pMe);
                  } else {
                     // ### RSK = Delete Object ###
                     c_samplesms_DeleteEMSObject(pMe);
                  }
               }

               break;

            case AVK_DOWN:
               {
               // ********************************************************
               // If user has entered dest#1, pressing AVK_DOWN
               // - while in dest#1 input field 
               // - will bring the user to enter dest#2,
               // otherwise it will bring the user to message input field
               // ********************************************************
               AECHAR temp[3];
               if (pMe->m_appstate == APPSTATE_INPUT_DEST1) {
                  MEMSET(temp, 0, sizeof(temp));
                  ITEXTCTL_GetText(pMe->m_pITextDest1, temp, 
                     (sizeof(temp)/sizeof(AECHAR))-1); 
                  if ( WSTRLEN(temp) != 0 ) {
                     pMe->m_appstate = APPSTATE_INPUT_DEST2;
                     ITEXTCTL_SetActive(pMe->m_pITextDest2, TRUE);
                     ITEXTCTL_SetCursorPos(pMe->m_pITextDest2, TC_CURSOREND);
                     ITEXTCTL_SetActive(pMe->m_pITextDest1, FALSE);
                  }
                  else {
                     pMe->m_appstate = APPSTATE_INPUT_MSG;
                     c_samplesms_DrawSoftKey(pMe, IDS_SEND, IDS_INS_OBJECT); 
                     ITEXTCTL_SetActive(pMe->m_pITextMsg, TRUE);
                     ITEXTCTL_SetCursorPos(pMe->m_pITextMsg, TC_CURSOREND);
                     ITEXTCTL_SetActive(pMe->m_pITextDest1, FALSE);
                     
                  }
               } else if (pMe->m_appstate == APPSTATE_INPUT_DEST2) {
                  pMe->m_appstate = APPSTATE_INPUT_MSG;
                  c_samplesms_DrawSoftKey(pMe, IDS_SEND, IDS_INS_OBJECT);   
                  ITEXTCTL_SetActive(pMe->m_pITextMsg, TRUE);
                  ITEXTCTL_SetCursorPos(pMe->m_pITextMsg, TC_CURSOREND);
                  ITEXTCTL_SetActive(pMe->m_pITextDest2, FALSE);
               }
               break;
               }
               
            case AVK_UP:
               {
               AECHAR temp[3];
               if (pMe->m_appstate == APPSTATE_INPUT_DEST2) {
                  pMe->m_appstate = APPSTATE_INPUT_DEST1;
                  ITEXTCTL_SetActive(pMe->m_pITextDest1, TRUE);
                  ITEXTCTL_SetCursorPos(pMe->m_pITextDest1, TC_CURSOREND);
                  ITEXTCTL_SetActive(pMe->m_pITextDest2, FALSE);
               } else if (pMe->m_appstate == APPSTATE_INPUT_MSG) {
                  IDISPLAY_ClearScreen(pMe->m_pIDisplay);
                  IDISPLAY_Update(pMe->m_pIDisplay);
                  MEMSET(temp, 0, sizeof(temp));
                  ITEXTCTL_GetText(pMe->m_pITextDest1, temp, 
                     (sizeof(temp)/sizeof(AECHAR))-1);
               
                  // ********************************************************
                  // If user has entered dest#1, pressing AVK_UP
                  // - while in message input field
                  // - will bring the user to enter dest#2,
                  // otherwise it will bring the user to dest#2 input field
                  // ********************************************************
                  if (WSTRLEN(temp) != 0) {
                     pMe->m_appstate = APPSTATE_INPUT_DEST2;
                     ITEXTCTL_SetActive(pMe->m_pITextDest2, TRUE);
                     ITEXTCTL_SetCursorPos(pMe->m_pITextDest2, TC_CURSOREND);
                     ITEXTCTL_Redraw(pMe->m_pITextDest1);
                  } else {
                     pMe->m_appstate = APPSTATE_INPUT_DEST1;
                     ITEXTCTL_SetActive(pMe->m_pITextDest1, TRUE);
                     ITEXTCTL_SetCursorPos(pMe->m_pITextDest1, TC_CURSOREND);
                  }
                  ITEXTCTL_SetActive(pMe->m_pITextMsg, FALSE);
                  c_samplesms_DrawComposeScreenAttribute(pMe);
               }
               break;
               } 

            case AVK_SELECT:
               {
                  if (pMe->m_appstate == APPSTATE_INSERT_OBJ) {
                     IMENUCTL_SetActive(pMe->m_pIMenuEMSObjects, FALSE);
                     c_samplesms_InsertEMSObject(pMe);
                  } else if (pMe->m_appstate == APPSTATE_SET_MOENC) {
                     uint16 selectedMOEnc = IMENUCTL_GetSel(pMe->m_pIMenuMOEncodings);
                     IMENUCTL_SetActive(pMe->m_pIMenuMOEncodings, FALSE);
                     c_samplesms_SetMOEncoding(pMe, selectedMOEnc);
                  } else if ((pMe->m_appstate == APPSTATE_INBOX) || 
                             (pMe->m_appstate == APPSTATE_OUTBOX)||
                             (pMe->m_appstate == APPSTATE_DRAFTS)) {
                     // ### RSK = Read Message ###
                     uint16 selectedMsg;
                     if (pMe->m_appstate == APPSTATE_INBOX) {
                        selectedMsg = IMENUCTL_GetSel(pMe->m_pIMenuInboxMsgs);
                     } else if (pMe->m_appstate == APPSTATE_OUTBOX) {
                        selectedMsg = IMENUCTL_GetSel(pMe->m_pIMenuOutboxMsgs);
                     } else {
                        selectedMsg = IMENUCTL_GetSel(pMe->m_pIMenuDrafts);
                     }
                     c_samplesms_ReadMessage(pMe, selectedMsg);
                  }
                  break;
               }
         
            case AVK_CLR:
               switch (pMe->m_appstate) {
                  case APPSTATE_MAIN:
                     ISHELL_CloseApplet(pMe->m_pIShell, FALSE);
                     break;
                  case APPSTATE_INSERT_OBJ:
                     IMENUCTL_SetActive(pMe->m_pIMenuEMSObjects, FALSE);
                     c_samplesms_BackToCompose(pMe);   
                     break;
                  case APPSTATE_SET_MOENC:
                     pMe->m_appstate = APPSTATE_OPTIONS;
                     c_samplesms_ShowOptionsMenu(pMe);
                     break;
                  case APPSTATE_READ_INBOX:
                     //FREEIF(pMe->m_pReadMsgBuffer);
                     c_samplesms_ResetControls(pMe);
                     pMe->m_appstate = APPSTATE_INBOX;
                     if (pMe->m_curTag == AEESMS_TAG_MT_NOT_READ) {
                        c_samplesms_SetMessageTag(pMe->m_pISMSMsg, AEESMS_TAG_MT_READ);
                        c_samplesms_UpdateMessage(pMe);
                     } else {
                        IMENUCTL_SetActive(pMe->m_pIMenuInboxMsgs, TRUE);
                        c_samplesms_DrawSoftKey(pMe, IDS_DELETE_MSG, IDS_READ_MSG);
                     }
                     if (pMe->m_pIImage) {
                        IImage_Release(pMe->m_pIImage); 
                     }
                     break;
                  case APPSTATE_READ_OUTBOX:
                     FREEIF(pMe->m_pReadMsgBuffer);
                     pMe->m_appstate = APPSTATE_OUTBOX;
                     c_samplesms_ResetControls(pMe);
                     IMENUCTL_SetActive(pMe->m_pIMenuOutboxMsgs, TRUE);
                     c_samplesms_DrawSoftKey(pMe, IDS_DELETE_MSG, IDS_READ_MSG);   
                     if (pMe->m_pIImage) {
                        IImage_Release(pMe->m_pIImage); 
                     }
                     break;
                  case APPSTATE_EDIT_DRAFT:
                     pMe->m_appstate = APPSTATE_DRAFTS;
                     c_samplesms_ResetControls(pMe);
                     IMENUCTL_SetActive(pMe->m_pIMenuDrafts, TRUE);
                     c_samplesms_DrawSoftKey(pMe, IDS_DELETE_MSG, IDS_READ_MSG);   
                     break;
                  default:
                     c_samplesms_MainMenu(pMe);
                     break;
               }
         }

         return(TRUE);

      case EVT_COMMAND:
         switch (wParam) {
            case IDS_COMPOSE_MSG:
               pMe->m_appstate = APPSTATE_INPUT_DEST1;
               c_samplesms_ComposeScreen(pMe);
               break;

            case IDS_INBOX:
               pMe->m_appstate = APPSTATE_INBOX;
               /* The code below is commented out...
                  See note on c_samplesms_StoreRcvdMessageCB
                  in smsstorage.c
               ====================================================
               if (!pMe->m_bInboxEnumerated) {
                  pMe->m_bInboxEnumerated = TRUE;
                  c_samplesms_AccessStorage(pMe, AEESMS_TAG_MT_NOT_READ);   
               } else {
                  c_samplesms_ListInboxMessages(pMe);
               }
               */
               c_samplesms_AccessStorage(pMe, AEESMS_TAG_MT_NOT_READ);
               break;

            case IDS_OUTBOX:
               pMe->m_appstate = APPSTATE_OUTBOX;
               if (!pMe->m_bOutboxEnumerated) {
                  pMe->m_bOutboxEnumerated = TRUE;
                  c_samplesms_AccessStorage(pMe, AEESMS_TAG_MO_SENT);
               } else {
                  c_samplesms_ListOutboxMessages(pMe);
               }
                 break;

            case IDS_DRAFTS:
               pMe->m_appstate = APPSTATE_DRAFTS;
               if (!pMe->m_bDraftsEnumerated) {
                  pMe->m_bDraftsEnumerated = TRUE;
                  c_samplesms_AccessStorage(pMe, AEESMS_TAG_MO_NOT_SENT);
               } else {
                  c_samplesms_ListDrafts(pMe);
               }
               break;

            case IDS_ERASE_INBOX:
               c_samplesms_DeleteMessageTag(pMe, AEESMS_TAG_MT_NOT_READ);
               break;

            case IDS_ERASE_OUTBOX:
               c_samplesms_DeleteMessageTag(pMe, AEESMS_TAG_MO_SENT);
               break;

            case IDS_ERASE_DRAFTS:
               c_samplesms_DeleteMessageTag(pMe, AEESMS_TAG_MO_NOT_SENT);
               break;

            case IDS_ERASE_ALL:
               c_samplesms_DeleteMessageAll(pMe);
               break;

            case IDS_SET_MOSMS_ENC:
               pMe->m_appstate = APPSTATE_SET_MOENC;
               c_samplesms_ShowMOEncodingsMenu(pMe);
               break;
         }
         return TRUE;


      case EVT_STSSTORAGE:
         switch (pMe->m_curTag) {
            // Note: in APPSTATE_PREPMAIN, get storage counter for each category, then build main menu
            case AEESMS_TAG_MT_NOT_READ:
               //Update message tag (storage) counter
               pMe->m_nUnread = dwParam;
               if (pMe->m_appstate == APPSTATE_PREPMAIN) {
                  c_samplesms_GetStorageStatus(pMe, AEESMS_TAG_MT_READ);
               }
               break;
            case AEESMS_TAG_MT_READ:
               //Update message tag (storage) counter
               pMe->m_nRead = dwParam;
               if (pMe->m_appstate == APPSTATE_PREPMAIN) {
                  c_samplesms_GetStorageStatus(pMe, AEESMS_TAG_MO_SENT);
               } 
               break;
            case AEESMS_TAG_MO_SENT:
               //Update message tag (storage) counter
               pMe->m_nSent = dwParam;
               if (pMe->m_appstate == APPSTATE_PREPMAIN) {
                  c_samplesms_GetStorageStatus(pMe, AEESMS_TAG_MO_NOT_SENT);
               } 
               break;
            case AEESMS_TAG_MO_NOT_SENT:
               //Update message tag (storage) counter
               pMe->m_nDraft = dwParam;
               if (pMe->m_appstate == APPSTATE_PREPMAIN) {
                  ISHELL_SendEvent(pMe->m_pIShell, AEECLSID_C_SAMPLESMS, EVT_COUNTERS_READY, 0, 0);
               } 
               break;
         }
         return TRUE;


      case EVT_COUNTERS_READY:
         pMe->m_bGetCountersInProgress = FALSE;
         c_samplesms_BuildMainMenu(pMe);
         return (TRUE);


      case EVT_ENUM_TAG_DONE:
         switch (pMe->m_curTag) {
            case AEESMS_TAG_MT_NOT_READ:
               c_samplesms_AccessStorage(pMe, AEESMS_TAG_MT_READ);
               break;   
            case AEESMS_TAG_MT_READ:
               pMe->m_bEnumerating = FALSE;
               c_samplesms_ListInboxMessages(pMe);
               break;
            case AEESMS_TAG_MO_SENT:
               pMe->m_bEnumerating = FALSE;
               c_samplesms_ListOutboxMessages(pMe);
               break;
            case AEESMS_TAG_MO_NOT_SENT:
               pMe->m_bEnumerating = FALSE;
               c_samplesms_ListDrafts(pMe);
               break;
         }
         return (TRUE);

      case EVT_SENDING_DONE:
         pMe->m_appstate = APPSTATE_MSG_SENT;
         FREEIF(pMe->m_pszMessage);
         FREEIF(pMe->m_pwszMessage);
         FREEIF(pMe->m_pszDestNum);
         c_samplesms_ShowStatus(pMe, "Message sent");
         return (TRUE);

        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}

/*===========================================================================

FUNCTION c_samplesms_InitAppData

DESCRIPTION
   This function initializes app specific data allocates memory for app data 
   (AppletData) and sets it to pAppData of AEEApplet. 

PROTOTYPE:
   void c_samplesms_InitAppData(CSampleSMS* pi);

PARAMETERS:
   pi [in]: Pointer to the CSampleSMS structure. This structure contains 
   information specific to this applet. It was initialized during the 
   AEEClsCreateInstance().

DEPENDENCIES
   Assumes pi is not NULL

RETURN VALUE
  TRUE: If the app is able to initialize application data successfully 
  FALSE: If the app fails to initialize any application data 

SIDE EFFECTS
  none
===========================================================================*/
boolean c_samplesms_InitAppData(CSampleSMS* pMe)
{
   IModel *pIModel = NULL;
   AEERect   rect;
   
   pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
   ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

   pMe->m_pIDisplay = pMe->a.m_pIDisplay;
   pMe->m_pIShell   = pMe->a.m_pIShell;

   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_MENUCTL, 
                        (void **)&pMe->m_pIMenuMain) != SUCCESS) {
      pMe->m_pIMenuMain = NULL;
      return FALSE;
   }
                                
   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_MENUCTL, 
                        (void **)&pMe->m_pIMenuInboxMsgs) != SUCCESS) {
      pMe->m_pIMenuInboxMsgs = NULL;
      return FALSE;
   }

   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_MENUCTL, 
                        (void **)&pMe->m_pIMenuOutboxMsgs) != SUCCESS) {
      pMe->m_pIMenuOutboxMsgs = NULL;
      return FALSE;
   }

   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_MENUCTL, 
                        (void **)&pMe->m_pIMenuDrafts) != SUCCESS) {
      pMe->m_pIMenuDrafts = NULL;
      return FALSE;
   }

   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_MENUCTL, 
                        (void **)&pMe->m_pIMenuMOEncodings) != SUCCESS) {
      pMe->m_pIMenuMOEncodings = NULL;
      return FALSE;
   }

   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_MENUCTL, 
                        (void **)&pMe->m_pIMenuOptions) != SUCCESS) {
      pMe->m_pIMenuOptions = NULL;
      return FALSE;
   }

   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_MENUCTL, 
                        (void **)&pMe->m_pIMenuEMSObjects) != SUCCESS) {
      pMe->m_pIMenuEMSObjects = NULL;
      return FALSE;
   }

   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_STATIC, 
                        (void **)&pMe->m_pIStatic) != SUCCESS) {
      pMe->m_pIStatic = NULL;
      return FALSE;
   }

   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_SMS, 
                        (void **)&pMe->m_pISMS) != SUCCESS) {
      pMe->m_pISMS = NULL;
      return FALSE;
   }
   
   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_SMSSTORAGE, 
                        (void **)&pMe->m_pISMSStorage) != SUCCESS) {
      pMe->m_pISMSStorage = NULL;
      return FALSE;
   }
   
   if (pMe->m_pISMSStorage &&
      (SUCCESS == ISMSSTORAGE_QueryInterface(pMe->m_pISMSStorage, 
                     AEEIID_MODEL, (void**)&pIModel))) {
      IMODEL_AddListenerEx(pIModel, &pMe->m_SMSStorageListener, 
         (PFNLISTENER)c_samplesms_SMSStorage_ModelListener, pMe);
      IMODEL_Release(pIModel);
    }

   if(ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_TEXTCTL, 
                        (void **)&pMe->m_pITextDest1) != SUCCESS ) {
      pMe->m_pITextDest1 = NULL;
      return FALSE;
   }

   if(ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_TEXTCTL, 
                        (void **)&pMe->m_pITextDest2) != SUCCESS ) {
      pMe->m_pITextDest2 = NULL;
      return FALSE;
   }

   if(ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_TEXTCTL, 
                        (void **)&pMe->m_pITextMsg) != SUCCESS ) {
      pMe->m_pITextMsg = NULL;
      return FALSE;
   }

   if (c_samplesms_InitializeMessaging(pMe) != AEE_SUCCESS) {
      return FALSE;
   }

   rect.x = 0;
   rect.y = 0;
   rect.dx = pMe->DeviceInfo.cxScreen;
   rect.dy = pMe->DeviceInfo.cyScreen-20;

   IMENUCTL_SetRect(pMe->m_pIMenuInboxMsgs, &rect);
   IMENUCTL_SetRect(pMe->m_pIMenuOutboxMsgs, &rect);
   IMENUCTL_SetRect(pMe->m_pIMenuDrafts, &rect);
   IMENUCTL_SetTitle(pMe->m_pIMenuInboxMsgs, C_SAMPLESMS_RES_FILE, IDS_INBOX_TITLE, NULL);
   IMENUCTL_SetTitle(pMe->m_pIMenuOutboxMsgs, C_SAMPLESMS_RES_FILE, IDS_OUTBOX_TITLE, NULL);
   IMENUCTL_SetTitle(pMe->m_pIMenuDrafts, C_SAMPLESMS_RES_FILE, IDS_DRAFTS_TITLE, NULL);
   IMENUCTL_SetTitle(pMe->m_pIMenuMOEncodings, C_SAMPLESMS_RES_FILE, IDS_SET_MOENC_TITLE, NULL);
   
    // if there have been no failures up to this point then return success
    return TRUE;
}

/*===========================================================================
FUNCTION c_samplesms_FreeAppData

DESCRIPTION
   This function will be called while application is exiting.
   This frees data and any allocated memory during application lifetime.
 
PROTOTYPE:
   void c_samplesms_FreeAppData(CSampleSMS * pi);

PARAMETERS:
   pi [in]: Pointer to the void CSampleSMS structure. This structure contains 
   information specific to this applet. It was initialized during the 
   AEEClsCreateInstance().

DEPENDENCIES
   Assumes pi is not NULL

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void c_samplesms_FreeAppData(CSampleSMS* pMe)
{
   if (pMe->m_pIMenuMain) {
      IMENUCTL_Release(pMe->m_pIMenuMain);
      pMe->m_pIMenuMain = NULL;
   }
   if (pMe->m_pIMenuInboxMsgs) {
      IMENUCTL_Release(pMe->m_pIMenuInboxMsgs);
      pMe->m_pIMenuInboxMsgs = NULL;
   }
   if (pMe->m_pIMenuOutboxMsgs) {
      IMENUCTL_Release(pMe->m_pIMenuOutboxMsgs);
      pMe->m_pIMenuOutboxMsgs = NULL;
   }
   if (pMe->m_pIMenuDrafts) {
      IMENUCTL_Release(pMe->m_pIMenuDrafts);
      pMe->m_pIMenuDrafts = NULL;
   }
   if (pMe->m_pIMenuMOEncodings) {
      IMENUCTL_Release(pMe->m_pIMenuMOEncodings);
      pMe->m_pIMenuMOEncodings = NULL;
   }
   if (pMe->m_pIMenuOptions) {
      IMENUCTL_Release(pMe->m_pIMenuOptions);
      pMe->m_pIMenuOptions = NULL;
   }
   if (pMe->m_pIMenuEMSObjects) {
      IMENUCTL_Release(pMe->m_pIMenuEMSObjects);
      pMe->m_pIMenuEMSObjects = NULL;
   }
   if (pMe->m_pIStatic) {
      ISTATIC_Release(pMe->m_pIStatic);
      pMe->m_pIStatic = NULL;
   }
   if (pMe->m_pISMS) {
      ISMS_Release(pMe->m_pISMS);
      pMe->m_pISMS = NULL;
   }      
   if (pMe->m_pISMSMsg) {
      ISMSMSG_Release(pMe->m_pISMSMsg);
      pMe->m_pISMSMsg = NULL;
   }
   if (pMe->m_pISMSMsgRcv) {
      ISMSMSG_Release(pMe->m_pISMSMsgRcv);
      pMe->m_pISMSMsgRcv = NULL;
   }
   if (pMe->m_pISMSStorage) {
      ISMSSTORAGE_Release(pMe->m_pISMSStorage);
      pMe->m_pISMSStorage = NULL;
   }
   if (pMe->m_pITextDest1) {
      ITEXTCTL_Release(pMe->m_pITextDest1);
      pMe->m_pITextDest1 = NULL;
   }
   if (pMe->m_pITextDest2) {
      ITEXTCTL_Release(pMe->m_pITextDest2);
      pMe->m_pITextDest2 = NULL;
   }
   if (pMe->m_pITextMsg) {
      ITEXTCTL_Release(pMe->m_pITextMsg);
      pMe->m_pITextMsg = NULL;
   }
   if(pMe->m_pReadMsgBuffer) {
      FREE(pMe->m_pReadMsgBuffer);
      pMe->m_pReadMsgBuffer = NULL;
   }
   if(pMe->m_pszDestNum) {
      FREE(pMe->m_pszDestNum);
      pMe->m_pszDestNum = NULL;
   }
   if(pMe->m_pszMessage) {
      FREE(pMe->m_pszMessage);
      pMe->m_pszMessage = NULL;
   }
   if(pMe->m_pwszMessage) {
      FREE(pMe->m_pwszMessage);
      pMe->m_pwszMessage = NULL;
   }
   if(pMe->m_pMOEncodings) {
      FREE(pMe->m_pMOEncodings);
      pMe->m_pMOEncodings = NULL;
   }
   if(pMe->m_pPlEncodings) {
      FREE(pMe->m_pPlEncodings);
      pMe->m_pPlEncodings = NULL;
   }
   if(pMe->m_pIMemAStream) {
      IMEMASTREAM_Release(pMe->m_pIMemAStream);
      pMe->m_pIMemAStream = NULL;
   }
   if(pMe->m_pIImage) {
      IImage_Release(pMe->m_pIImage);
      pMe->m_pIImage = NULL;
   }
}


/*===========================================================================
FUNCTION c_samplesms_InitializeMessaging

DESCRIPTION
   This function initializes application specific data required for 
   the messaging, including querying the device specific info for encoding,
   and registering for SMS notification.
   This function also sets up several application sub-menus.

PROTOTYPE:
   void c_samplesms_InitializeMessaging(CSampleSMS* pi);

PARAMETERS:
   pi [in]: Pointer to the CSampleSMS structure. This structure contains 
   information specific to this applet. It was initialized during the 
   AEEClsCreateInstance().

DEPENDENCIES
   Assumes pi is not NULL

RETURN VALUE
  AEE_SUCCESS: If the app succesfully initializes data/parameter needed for
    performing messaging operations
  EFAILED: If the app couldn't retrieve any particular information/data 
    needed for performing messaging operations from the device 

SIDE EFFECTS
  none
===========================================================================*/
int c_samplesms_InitializeMessaging(CSampleSMS *pMe) {
   int temp;
   int nSize, nPadding;

   if (!c_samplesms_SetStorageType(pMe)) {
      return EFAILED;
   }

   // Initialize counters
   pMe->m_nUnread = 0;
   pMe->m_nRead = 0;
   pMe->m_nSent = 0;
   pMe->m_nDraft = 0;
   pMe->m_bGetCountersInProgress = FALSE;

   // Variables needed for message segmentation 
   pMe->m_seqNum = 1;
   pMe->m_totalMsg = 0;
   GETRAND(&pMe->m_refNum, 1);
   pMe->m_pText = NULL;
   pMe->m_pwText = NULL;
    pMe->m_calculationFactor = 1;
   
   // No object has been inserted 
   pMe->m_objPos = -1;
   
   // Message edit mode off 
   pMe->m_bEditMode = FALSE;

   // None of storage category has been enumerated
   pMe->m_bInboxEnumerated = FALSE;
   pMe->m_bOutboxEnumerated = FALSE;
   pMe->m_bDraftsEnumerated = FALSE;
   pMe->m_bEnumerating = FALSE;
   nPadding = 0;
   // Determine max payload bytes for one single message
   if (SUCCESS == ISMS_GetBytesAvailableForPayload(pMe->m_pISMS, pMe->m_pISMSMsg, 
                                                    &pMe->m_nMaxPayloadBytes, &nPadding)) {
      DBGPRINTF("max payload bytes = %d", pMe->m_nMaxPayloadBytes);
      if (pMe->m_nMaxPayloadBytes > 140) {
         DBGPRINTF("WARNING!! max payload bytes exceeds 140 bytes");
      }
   } else {
      DBGPRINTF("WARNING!! Couldn't retrieve max payload bytes");
      DBGPRINTF("set max payload bytes to 140");
      pMe->m_nMaxPayloadBytes = 140;   // set to 3GPP spec 
   }

   nSize = 0;
   //Get device encodings (used for payload encodings)
   if (SUCCESS == ISHELL_GetDeviceInfoEx(pMe->m_pIShell, 
         AEE_DEVICEITEM_TAPI_SMS_ENCODING, NULL, (int*)&nSize)) {
      DBGPRINTF("Device has %d enc options", nSize/sizeof(int));
      pMe->m_nPlEncodings = nSize/sizeof(int);
      if ( NULL == (pMe->m_pPlEncodings = (uint32 *) MALLOC(nSize)) ) {
         DBGPRINTF("Couldn't allocate memory");
         return ENOMEMORY;
      } else {
         if (SUCCESS == ISHELL_GetDeviceInfoEx(pMe->m_pIShell, 
               AEE_DEVICEITEM_TAPI_SMS_ENCODING, pMe->m_pPlEncodings, (int*)&nSize)) {
            for (temp = 0; temp < (nSize/sizeof(int)); temp++) {
               DBGPRINTF("enc %d = %d", temp, *(pMe->m_pPlEncodings+temp));
            }
            c_samplesms_SetPayloadEncoding(pMe);
         } else {
            DBGPRINTF("Couldn't retrieve device encoding(s)");
            if(!c_samplesms_SetDefaultPayloadEncoding(pMe)) {
               return EFAILED;
            }
         }
      }
   }
   else {
      DBGPRINTF("Couldn't retrieve device encoding(s)");
      if(!c_samplesms_SetDefaultPayloadEncoding(pMe)) {
         return EFAILED;
      }
   }

   nSize = 0;
   //Get MOSMS Encodings available for the device
   if (SUCCESS == ISMS_GetEncodingsAvailableForMOSMS(pMe->m_pISMS, NULL, (uint32*) &nSize)) {
      DBGPRINTF("Device has %d mo enc options", nSize/sizeof(int));
      pMe->m_nMOEncodings = nSize/sizeof(int);
      if ( NULL == (pMe->m_pMOEncodings = (uint32 *) MALLOC(nSize))  ) {
         DBGPRINTF("Couldn't allocate memory");
         return ENOMEMORY;
      } else {
         if (SUCCESS == ISMS_GetEncodingsAvailableForMOSMS(pMe->m_pISMS, pMe->m_pMOEncodings, (uint32*) &nSize)) {
            for (temp = 0; temp < (nSize/sizeof(int)); temp++) {
               DBGPRINTF("enc %d = %d", temp, *(pMe->m_pMOEncodings+temp));
            }
            c_samplesms_InitMOSMSEncoding(pMe);
         } else {
            DBGPRINTF("Couldn't retrieve device mo encoding(s)");
            //pMe->m_moEncoding = AEESMS_ENC_ASCII;
            return EFAILED;
         }
      }
   }
   else {
      DBGPRINTF("Couldn't retrieve device mo encoding(s)");
      //pMe->m_moEncoding = AEESMS_ENC_ASCII;
      return EFAILED;
   }


   // Build Options Menu
   c_samplesms_SetOptionsMenu(pMe);

   // Build EMS Objects Menu
   c_samplesms_SetEMSObjectsMenu(pMe);

   // Register for Text and EMS 
   ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_C_SAMPLESMS, 
         AEECLSID_SMSNOTIFIER, (AEESMS_TYPE_TEXT << 16) | NMASK_SMS_TYPE);
   ISHELL_RegisterNotify(pMe->a.m_pIShell, AEECLSID_C_SAMPLESMS, 
         AEECLSID_SMSNOTIFIER, (AEESMS_TYPE_EMS << 16) | NMASK_SMS_TYPE);
   
   return AEE_SUCCESS;
}


/*===========================================================================
FUNCTION c_samplesms_CreateISMSMsgInstance

DESCRIPTION
   This function is creating a fresh ISMSMsg instance to encapsulate a message.
 
PROTOTYPE:
   int c_samplesms_CreateISMSMsgInstance(CSampleSMS * pi);

PARAMETERS:
   pi [in]: Pointer to the void CSampleSMS structure. This structure contains 
   information specific to this applet. It was initialized during the 
   AEEClsCreateInstance().

DEPENDENCIES
   Assumes pi is not NULL

RETURN VALUE
   AEE_SUCCESS: If the app successfully creates ISMSMsg instance 
   EFAILED: If the app couldn't create ISMSMsg instance

SIDE EFFECTS
   None
===========================================================================*/
int c_samplesms_CreateISMSMsgInstance(CSampleSMS *pMe) {
   if (pMe->m_pISMSMsg) {
      ISMSMSG_Release(pMe->m_pISMSMsg);
      pMe->m_pISMSMsg = NULL;
   }
   if (SUCCESS != ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_SMSMSG, 
                        (void **)&pMe->m_pISMSMsg)) {
      DBGPRINTF("Couldn't create ISMSMsg instance");
      return EFAILED;
   }

    return AEE_SUCCESS;
}


/*===========================================================================
FUNCTION c_samplesms_MainMenu

DESCRIPTION
   This function is preparing the Main Menu for the application.
   It collects storage storage status (count messages) of 
   Inbox, Outbox and Drafts.
 
PROTOTYPE:
   void c_samplesms_MainMenu(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_MainMenu(CSampleSMS *pMe) {
   pMe->m_appstate = APPSTATE_PREPMAIN;
   // In APPSTATE_PREPMAIN, get counters for each category, then build main menu
   if (!pMe->m_bGetCountersInProgress) {
      c_samplesms_GetMessageCounters(pMe);
   }
}


/*===========================================================================
FUNCTION c_samplesms_BuildMainMenu

DESCRIPTION
   This function will provide the Main Menu of the application
   
PROTOTYPE:
   void c_samplesms_BuildMainMenu(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_BuildMainMenu(CSampleSMS *pMe) {
   AECHAR tempBuff[64];

   c_samplesms_ResetControls(pMe);

   if (pMe->m_appstate == APPSTATE_PREPMAIN) {
      IMENUCTL_DeleteAll(pMe->m_pIMenuMain);
      IMENUCTL_SetTitle(pMe->m_pIMenuMain, C_SAMPLESMS_RES_FILE, IDS_MESSAGING, NULL);
   
      IMENUCTL_AddItem(pMe->m_pIMenuMain, C_SAMPLESMS_RES_FILE, IDS_COMPOSE_MSG, IDS_COMPOSE_MSG, 0, 0);

      ISHELL_LoadResString(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDS_INBOX, tempBuff, sizeof(tempBuff));
      WSPRINTF(pMe->m_wszStringBuffer, sizeof(pMe->m_wszStringBuffer), tempBuff, pMe->m_nUnread, pMe->m_nRead);
      IMENUCTL_AddItem(pMe->m_pIMenuMain, 0, 0, IDS_INBOX, pMe->m_wszStringBuffer, 0);
   
      ISHELL_LoadResString(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDS_OUTBOX, tempBuff, sizeof(tempBuff));
      WSPRINTF(pMe->m_wszStringBuffer, sizeof(pMe->m_wszStringBuffer), tempBuff, pMe->m_nSent);
      IMENUCTL_AddItem(pMe->m_pIMenuMain, 0, 0, IDS_OUTBOX, pMe->m_wszStringBuffer, 0);
   
      ISHELL_LoadResString(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDS_DRAFTS, tempBuff, sizeof(tempBuff));
      WSPRINTF(pMe->m_wszStringBuffer, sizeof(pMe->m_wszStringBuffer), tempBuff, pMe->m_nDraft);
      IMENUCTL_AddItem(pMe->m_pIMenuMain, 0, 0, IDS_DRAFTS, pMe->m_wszStringBuffer, 0);
   }
   
   pMe->m_appstate = APPSTATE_MAIN;
   IMENUCTL_SetActive(pMe->m_pIMenuMain, TRUE);
   IMENUCTL_Redraw(pMe->m_pIMenuMain);
   c_samplesms_DrawSoftKey(pMe, 0, IDS_OPTIONS);   
   
   return;
}


/*===========================================================================
FUNCTION c_samplesms_ResetControls

DESCRIPTION
   This function resets any control objects in the application.
   This is normally called when application changes its state, 
   where there is a need to change the app UI.
 
PROTOTYPE:
   void c_samplesms_ResetControls(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_ResetControls(CSampleSMS *pMe) {
   IMENUCTL_SetActive(pMe->m_pIMenuMain, FALSE);
   IMENUCTL_SetActive(pMe->m_pIMenuOptions, FALSE);
   IMENUCTL_SetActive(pMe->m_pIMenuEMSObjects, FALSE);
   IMENUCTL_SetActive(pMe->m_pIMenuInboxMsgs, FALSE);
   IMENUCTL_SetActive(pMe->m_pIMenuOutboxMsgs, FALSE);
   IMENUCTL_SetActive(pMe->m_pIMenuDrafts, FALSE);
   IMENUCTL_SetActive(pMe->m_pIMenuMOEncodings, FALSE);

   ITEXTCTL_SetActive(pMe->m_pITextDest1, FALSE);
   ITEXTCTL_SetActive(pMe->m_pITextDest2, FALSE);
   ITEXTCTL_SetActive(pMe->m_pITextMsg, FALSE);
   ISTATIC_SetActive(pMe->m_pIStatic, FALSE);

   // Clear Screen
    IDISPLAY_ClearScreen(pMe->m_pIDisplay);
   IDISPLAY_Update(pMe->m_pIDisplay);
}


/*===========================================================================
FUNCTION c_samplesms_SetOptionsMenu

DESCRIPTION
   This function builds 'Options' sub-menu for the application.
   With this sub-menu, application allows the user to change the 
   MO SMS encoding, erase Inbox, Outbox, Drafts, or all stored messages.
 
PROTOTYPE:
   void c_samplesms_SetOptionsMenu(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_SetOptionsMenu(CSampleSMS *pMe) {
   IMENUCTL_SetTitle(pMe->m_pIMenuOptions, C_SAMPLESMS_RES_FILE, IDS_OPTIONS, NULL);
   IMENUCTL_AddItem(pMe->m_pIMenuOptions, C_SAMPLESMS_RES_FILE, IDS_ERASE_INBOX, IDS_ERASE_INBOX, 0, 0);
   IMENUCTL_AddItem(pMe->m_pIMenuOptions, C_SAMPLESMS_RES_FILE, IDS_ERASE_OUTBOX, IDS_ERASE_OUTBOX, 0, 0);
   IMENUCTL_AddItem(pMe->m_pIMenuOptions, C_SAMPLESMS_RES_FILE, IDS_ERASE_DRAFTS, IDS_ERASE_DRAFTS, 0, 0);   
   IMENUCTL_AddItem(pMe->m_pIMenuOptions, C_SAMPLESMS_RES_FILE, IDS_ERASE_ALL, IDS_ERASE_ALL, 0, 0);
   IMENUCTL_AddItem(pMe->m_pIMenuOptions, C_SAMPLESMS_RES_FILE, IDS_SET_MOSMS_ENC, IDS_SET_MOSMS_ENC, 0, 0);
}


/*===========================================================================
FUNCTION c_samplesms_ShowOptionsMenu

DESCRIPTION
   This function will enable the user to perform additional options provided
   by the application, i.e. erasing Inbox/Outbox/Drafts, and changing the
   MO Encoding (if several MO encoding are available on a device)
   This menu is accessible through application Main Menu.
   
PROTOTYPE:
   void c_samplesms_ShowOptionsMenu(CSampleSMS * pi);
===========================================================================*/

void c_samplesms_ShowOptionsMenu(CSampleSMS *pMe) {
   c_samplesms_ResetControls(pMe);
   if (pMe->m_appstate == APPSTATE_SET_MOENC) {
      IMENUCTL_SetActive(pMe->m_pIMenuMOEncodings, FALSE);
      pMe->m_appstate = APPSTATE_OPTIONS;
   }
   IMENUCTL_SetActive(pMe->m_pIMenuOptions, TRUE);
}

/*===========================================================================
FUNCTION c_samplesms_SetMOEncodingsMenu

DESCRIPTION
   This function builds sub-menu listing available MO Encoding on the device, 
   that was queried during initialization.
   This menu will be accessible through 'Options' submenu.
 
PROTOTYPE:
   void c_samplesms_SetMOEncodingsMenu(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_SetMOEncodingsMenu(CSampleSMS *pMe) {
   int temp;

   for(temp=0; temp<pMe->m_nMOEncodings; temp++) {
      switch ( *(pMe->m_pMOEncodings+temp) ) {
         case AEESMS_ENC_OCTET:
            IMENUCTL_AddItem(pMe->m_pIMenuMOEncodings, C_SAMPLESMS_RES_FILE, 
                  IDS_MOENC_OCTET, IDS_MOENC_OCTET, 0, 0);
            break;
         case AEESMS_ENC_ASCII:
            IMENUCTL_AddItem(pMe->m_pIMenuMOEncodings, C_SAMPLESMS_RES_FILE, 
                  IDS_MOENC_ASCII, IDS_MOENC_ASCII, 0, 0);
            break;
         case AEESMS_ENC_UNICODE:
            IMENUCTL_AddItem(pMe->m_pIMenuMOEncodings, C_SAMPLESMS_RES_FILE, 
                  IDS_MOENC_UNICODE, IDS_MOENC_UNICODE, 0, 0);   
            break;
         case AEESMS_ENC_LATIN:
            IMENUCTL_AddItem(pMe->m_pIMenuMOEncodings, C_SAMPLESMS_RES_FILE, 
                  IDS_MOENC_LATIN, IDS_MOENC_LATIN, 0, 0);   
            break;
         case AEESMS_ENC_GSM_7_BIT_DEFAULT:
            IMENUCTL_AddItem(pMe->m_pIMenuMOEncodings, C_SAMPLESMS_RES_FILE, 
                  IDS_MOENC_GSM7BIT, IDS_MOENC_GSM7BIT, 0, 0);   
            break;
         default:
            DBGPRINTF("non common mo encoding - not added!");
            break;
      }
   }
}

/*===========================================================================
FUNCTION c_samplesms_ShowMOEncodingsMenu

DESCRIPTION
   This function will enable the user to change the MO Encoding,
   depending on available MO Encodings on the device.
   
PROTOTYPE:
   void c_samplesms_ShowMOEncodingsMenu(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_ShowMOEncodingsMenu(CSampleSMS *pMe) {
   c_samplesms_ResetControls(pMe);
   IMENUCTL_SetActive(pMe->m_pIMenuMOEncodings, TRUE);
   IMENUCTL_Redraw(pMe->m_pIMenuMOEncodings);
}


/*===========================================================================
FUNCTION c_samplesms_SetEMSObjectsMenu

DESCRIPTION
   This function will build a list of EMS Objects (small pics & large pics)
   that can be added to a message.
   
PROTOTYPE:
   void c_samplesms_SetEMSObjectsMenu(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_SetEMSObjectsMenu(CSampleSMS *pMe) {

   CtlAddItem msgItem;
   AECHAR tempBuff[64];

   IMENUCTL_SetTitle(pMe->m_pIMenuEMSObjects, C_SAMPLESMS_RES_FILE, IDS_OBJECTS_LIST, NULL);

   MEMSET(&msgItem, 0, sizeof(CtlAddItem));
   msgItem.pImage = ISHELL_LoadResImage(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDI_SMALL_PIC1);
   if (NULL != msgItem.pImage) {
      msgItem.wItemID = IDI_SMALL_PIC1;   
      ISHELL_LoadResString(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDS_SMALL_PIC1, tempBuff, sizeof(tempBuff));
      msgItem.pText = tempBuff;
      IMENUCTL_AddItemEx(pMe->m_pIMenuEMSObjects, &msgItem);
      if (msgItem.pImage) {
         IImage_Release(msgItem.pImage);
      }
   } else {
      DBGPRINTF("Couldn't load image from resource file!");
   }

   MEMSET(&msgItem, 0, sizeof(CtlAddItem));
   msgItem.pImage = ISHELL_LoadResImage(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDI_LARGE_PIC1);
   if (NULL != msgItem.pImage) {
      msgItem.wItemID = IDI_LARGE_PIC1;   
      ISHELL_LoadResString(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDS_LARGE_PIC1, tempBuff, sizeof(tempBuff));
      msgItem.pText = tempBuff;
      IMENUCTL_AddItemEx(pMe->m_pIMenuEMSObjects, &msgItem);
      if (msgItem.pImage) {
         IImage_Release(msgItem.pImage);
      }
   } else {
      DBGPRINTF("Couldn't load image from resource file!");
   }
}

/*===========================================================================
FUNCTION c_samplesms_ShowEMSObjectsMenu

DESCRIPTION
   This function will enable the user to pic an EMS object to be inserted
   to a message
   
PROTOTYPE:
   void c_samplesms_ShowEMSObjectsMenu(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_ShowEMSObjectsMenu(CSampleSMS *pMe) {
   IDISPLAY_ClearScreen(pMe->m_pIDisplay);
   ITEXTCTL_SetActive(pMe->m_pITextMsg, FALSE);
   IMENUCTL_SetActive(pMe->m_pIMenuEMSObjects, TRUE);
   pMe->m_appstate = APPSTATE_INSERT_OBJ;
}

/*===========================================================================
FUNCTION c_samplesms_DrawSoftKey

DESCRIPTION
   This function will draw soft key functionality, which vary based on
   which state the application is in.
   
PROTOTYPE:
   void c_samplesms_DrawSoftKey(CSampleSMS *pMe, int16 lskID, int16 rskID);
===========================================================================*/
void c_samplesms_DrawSoftKey(CSampleSMS *pMe, int16 lskID, int16 rskID) {
   AEERect skRect;
   AECHAR tempBuff[35];

   skRect.x = 0;
   skRect.y = pMe->DeviceInfo.cyScreen - 20;
   skRect.dx = pMe->DeviceInfo.cxScreen;
   skRect.dy = 20; 
   IDisplay_DrawRect(pMe->m_pIDisplay, &skRect, 0, MAKE_RGB(0xA8,0xA8,0xA8), IDF_RECT_FILL);

   // Set text background
   IDISPLAY_SetColor(pMe->m_pIDisplay, CLR_USER_BACKGROUND, MAKE_RGB(0xA8,0xA8,0xA8));

   if (lskID != 0) {
      ISHELL_LoadResString(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, lskID, tempBuff, sizeof(tempBuff));
      IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_BOLD, tempBuff, -1, 
                     0, 0, NULL, IDF_ALIGN_LEFT | IDF_ALIGN_BOTTOM );
   }
   if (rskID != 0) {
      ISHELL_LoadResString(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, rskID, tempBuff, sizeof(tempBuff));
      IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_BOLD, tempBuff, -1, 
                     0, 0, NULL, IDF_ALIGN_RIGHT | IDF_ALIGN_BOTTOM);
   }
   IDISPLAY_Update(pMe->m_pIDisplay);

   // Reset background color to white
   IDISPLAY_SetColor(pMe->m_pIDisplay, CLR_USER_BACKGROUND, MAKE_RGB(0xFF, 0xFF, 0xFF));
   
}

/*===========================================================================
FUNCTION c_samplesms_ShowStatus

DESCRIPTION
   This function will pop up a message to the user, informing status of the
   executed operation.   
   
PROTOTYPE:
   void c_samplesms_ShowStatus(CSampleSMS *pMe, char *pText);
===========================================================================*/
void c_samplesms_ShowStatus(CSampleSMS *pMe, char *pText){
   AEERect rect;
   AECHAR *wText; 

   if (NULL == (wText = (AECHAR *) MALLOC((STRLEN(pText)+1) * sizeof (AECHAR)))) {
      DBGPRINTF("Couldn't allocate memory!");
      return;
   }
   
   rect.x = 10;
   rect.y = pMe->DeviceInfo.cyScreen/2-25;
   rect.dx = pMe->DeviceInfo.cxScreen-20;
   rect.dy = 50;
   
   c_samplesms_ResetControls(pMe);

   STRTOWSTR(pText, wText, (STRLEN(pText)+1)*sizeof(AECHAR));
   ISTATIC_SetRect(pMe->m_pIStatic, &rect);
   ISTATIC_SetText(pMe->m_pIStatic, NULL, wText, 0, AEE_FONT_BOLD);
   FREE(wText);
   ISTATIC_SetProperties(pMe->m_pIStatic, ST_CENTERTEXT|ST_MIDDLETEXT);
   ISTATIC_SetActive(pMe->m_pIStatic, TRUE);
   ISTATIC_Redraw(pMe->m_pIStatic);

   switch (pMe->m_appstate) {
      case APPSTATE_SENDING:
         break;
      case APPSTATE_INPUT_MSG:
         ISHELL_SetTimer(pMe->m_pIShell, 2000, (PFNNOTIFY)c_samplesms_BackToCompose, (void *)pMe);
         break;
      case APPSTATE_OPTIONS:
      case APPSTATE_SET_MOENC:
         ISHELL_SetTimer(pMe->m_pIShell, 2000, (PFNNOTIFY)c_samplesms_ShowOptionsMenu, (void *)pMe);
         break;
      case APPSTATE_INBOX:
      case APPSTATE_READ_INBOX:
         ISHELL_SetTimer(pMe->m_pIShell, 2000, (PFNNOTIFY)c_samplesms_ListInboxMessages, (void *)pMe);
         break;
      case APPSTATE_OUTBOX:
      case APPSTATE_READ_OUTBOX:
         ISHELL_SetTimer(pMe->m_pIShell, 2000, (PFNNOTIFY)c_samplesms_ListOutboxMessages, (void *)pMe);
         break;
      case APPSTATE_DRAFTS:
      case APPSTATE_EDIT_DRAFT:
         ISHELL_SetTimer(pMe->m_pIShell, 2000, (PFNNOTIFY)c_samplesms_ListDrafts, (void *)pMe);
         break;
      default:
         ISHELL_SetTimer(pMe->m_pIShell, 2000, (PFNNOTIFY)c_samplesms_MainMenu, (void *)pMe);
         break;
   }   
}


/*=======================================================================================
FUNCTION c_samplesms_AddMessageToList

DESCRIPTION
   This function will add a stored message to the corresponding 
   Menu Control(Inbox/Outbox/Drafts) for message listing.
   
PROTOTYPE:
   void c_samplesms_AddMessageToList(CSampleSMS * pi, MyMessage *msg, AEESMSTag msgTag);
=========================================================================================*/
void c_samplesms_AddMessageToList(CSampleSMS *pMe, MyMessage *msg, AEESMSTag msgTag) {

   CtlAddItem   msgItem;
   AECHAR wszTempBuff[64], wszStringBuffer[64];
   char szTempBuff[64], szStringBuffer[64];

   MEMSET(&msgItem, 0, sizeof(msgItem));   

   msgItem.wItemID = msg->msgIndex;
   
   ISHELL_LoadResString(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDS_MESSAGE, wszTempBuff, sizeof(wszTempBuff));
   WSTRTOSTR(wszTempBuff, szTempBuff, sizeof(szTempBuff));
   switch (msgTag) {
      case AEESMS_TAG_MT_NOT_READ:
         SPRINTF(szStringBuffer, szTempBuff, msg->number.srcNumber, msg->msgAttribute.timeStamp);
         msgItem.pImage = ISHELL_LoadResImage(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDI_ENV_CLOSED);
		 msgItem.pText = STRTOWSTR(szStringBuffer, wszStringBuffer, sizeof(wszStringBuffer));
         IMENUCTL_AddItemEx(pMe->m_pIMenuInboxMsgs, &msgItem);
         break;
      case AEESMS_TAG_MT_READ:
         SPRINTF(szStringBuffer, szTempBuff, msg->number.srcNumber, msg->msgAttribute.timeStamp);
         msgItem.pImage = ISHELL_LoadResImage(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDI_ENV_OPENED);
         msgItem.pText = STRTOWSTR(szStringBuffer, wszStringBuffer, sizeof(wszStringBuffer));
		 IMENUCTL_AddItemEx(pMe->m_pIMenuInboxMsgs, &msgItem);
         break;
      case AEESMS_TAG_MO_SENT:
         SPRINTF(szStringBuffer, szTempBuff, msg->number.srcNumber, msg->msgAttribute.msgString);
         msgItem.pImage = ISHELL_LoadResImage(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDI_ENV_SENT);
         msgItem.pText = STRTOWSTR(szStringBuffer, wszStringBuffer, sizeof(wszStringBuffer));
         IMENUCTL_AddItemEx(pMe->m_pIMenuOutboxMsgs, &msgItem);
         break;
      case AEESMS_TAG_MO_NOT_SENT:
         SPRINTF(szStringBuffer, szTempBuff, msg->number.srcNumber, msg->msgAttribute.msgString);
         msgItem.pImage = ISHELL_LoadResImage(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDI_ENV_DRAFT);
         msgItem.pText = STRTOWSTR(szStringBuffer, wszStringBuffer, sizeof(wszStringBuffer));
		 IMENUCTL_AddItemEx(pMe->m_pIMenuDrafts, &msgItem);
         break;
   }

   if (msgItem.pImage) {
      IImage_Release(msgItem.pImage);
   }
   FREEIF(msgItem.pText);
}


/*===========================================================================
FUNCTION c_samplesms_ListInboxMessages

DESCRIPTION
   This function will list any Inbox messages stored in SMSStorage.
   The stored message is tagged as AEESMS_TAG_MT_NOT_READ or AEESMS_TAG_MT_READ
   
PROTOTYPE:
   void c_samplesms_ListInboxMessages(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_ListInboxMessages(CSampleSMS *pMe) {
   DBGPRINTF("list inbox msgs");

   c_samplesms_ResetControls(pMe);
   IMENUCTL_SetActive(pMe->m_pIMenuMain, FALSE);
   IMENUCTL_SetActive(pMe->m_pIMenuInboxMsgs, TRUE);
   c_samplesms_DrawSoftKey(pMe, IDS_DELETE_MSG, IDS_READ_MSG); 
   IDISPLAY_Update(pMe->m_pIDisplay);
}

/*===========================================================================
FUNCTION c_samplesms_ListOutboxMessages

DESCRIPTION
   This function will list any Outbox messages stored in SMSStorage.
   The stored message is tagged as AEESMS_TAG_MO_SENT
   
PROTOTYPE:
   void c_samplesms_ListOutboxMessages(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_ListOutboxMessages(CSampleSMS *pMe) {
   DBGPRINTF("list outbox msgs");
   
   c_samplesms_ResetControls(pMe);
   IMENUCTL_SetActive(pMe->m_pIMenuMain, FALSE);
   IMENUCTL_SetActive(pMe->m_pIMenuOutboxMsgs, TRUE);
   IMENUCTL_Redraw(pMe->m_pIMenuOutboxMsgs);
   c_samplesms_DrawSoftKey(pMe, IDS_DELETE_MSG, IDS_READ_MSG);      
   IDISPLAY_Update(pMe->m_pIDisplay);
}

/*===========================================================================
FUNCTION c_samplesms_ListDrafts

DESCRIPTION
   This function will list any draft messages stored in SMSStorage.
   The stored message is tagged as AEESMS_TAG_MO_NOT_SENT
   
PROTOTYPE:
   void c_samplesms_ListDrafts(CSampleSMS * pi);
===========================================================================*/
void c_samplesms_ListDrafts(CSampleSMS *pMe) {
   DBGPRINTF("list drafts");
   
   c_samplesms_ResetControls(pMe);
   IMENUCTL_SetActive(pMe->m_pIMenuMain, FALSE);
   IMENUCTL_SetActive(pMe->m_pIMenuDrafts, TRUE);
   IMENUCTL_Redraw(pMe->m_pIMenuDrafts);
   c_samplesms_DrawSoftKey(pMe, IDS_DELETE_MSG, IDS_EDIT_MSG);      
   IDISPLAY_Update(pMe->m_pIDisplay);
}


/*===========================================================================
FUNCTION c_samplesms_DisplayMessage

DESCRIPTION
   This function will display the stored (inbox/outbox) message.
   
PROTOTYPE:
   void c_samplesms_DisplayMessage(CSampleSMS *pi, MyMessage message);
===========================================================================*/
void c_samplesms_DisplayMessage(CSampleSMS *pMe, MyMessage message) {
   AEERect rect;
   BITMAPFILEHEADER *hdr;
   BITMAPINFO *bmpInfo;
   byte *imageBuf;
   
   char *dataBuffer; 
   
   if (pMe->m_pReadMsgBuffer != NULL) {
      if (NULL == (dataBuffer = MALLOC(100 + STRLEN(pMe->m_pReadMsgBuffer) + 1))) {
         DBGPRINTF("Couldn't allocate memory!");
         return;
      }
   } else {
      if (NULL == (dataBuffer = MALLOC(100))) {
         DBGPRINTF("Couldn't allocate memory!");
         return;
      }
   }

   c_samplesms_ResetControls(pMe);

   if (message.emsObj.header_id != 0) {
      imageBuf = MALLOC(IMAGE_SIZE);
      if(imageBuf == NULL)  {
         DBGPRINTF("Buffer could Not be Allocated");
         return;
      }
   
      bmpInfo = MALLOC(sizeof(BITMAPINFO));
      if(bmpInfo == NULL)
      {
         DBGPRINTF("Buffer Could Not be Allocated");
         return;
      }

      switch(message.emsObj.header_id) {
         case AEESMS_UDH_SMALL_PICTURE:
            bmpInfo->bmiHeader.biWidth = SMALL_PIC_SIZE;
            bmpInfo->bmiHeader.biHeight = SMALL_PIC_SIZE;
            
            c_samplesms_ConvertEmsPicToBmp(imageBuf, &hdr, bmpInfo,
               message.emsObj.u.small_picture.data, 
               sizeof(message.emsObj.u.small_picture.data));   
            pMe->m_objPos = message.emsObj.u.small_picture.position;
            break;
         case AEESMS_UDH_LARGE_PICTURE:
            bmpInfo->bmiHeader.biWidth = LARGE_PIC_SIZE;
            bmpInfo->bmiHeader.biHeight = LARGE_PIC_SIZE;
      
            c_samplesms_ConvertEmsPicToBmp(imageBuf, &hdr, bmpInfo,
               message.emsObj.u.large_picture.data, 
               sizeof(message.emsObj.u.large_picture.data));   
            pMe->m_objPos = message.emsObj.u.large_picture.position;
            break;
      }

      c_samplesms_DisplayEMSObject(pMe, imageBuf, IMAGE_SIZE);
      FREEIF(imageBuf);
      FREEIF(bmpInfo);
   } 


   // Filling up data buffer with formatted message content 
   if (pMe->m_appstate == APPSTATE_INBOX) {
      pMe->m_appstate = APPSTATE_READ_INBOX;
      if (!message.bIsConcatMsg) {
         SPRINTF(dataBuffer, "%s\nFrom: %s\n\0", 
            message.msgAttribute.timeStamp, message.number.srcNumber);
      } else {
         SPRINTF(dataBuffer, "%s\nFrom: %s\nMessage %d of %d\n\0", 
            message.msgAttribute.timeStamp, message.number.srcNumber, 
            message.msgSeqNum, message.msgTotal);
      }

      if (pMe->m_pReadMsgBuffer) {
         if (message.bIsConcatMsg && (message.msgSeqNum > 1)) {
            STRCAT(dataBuffer, "+");
         }
         if (pMe->m_objPos == -1) {
            STRCAT(dataBuffer, pMe->m_pReadMsgBuffer);
         } else {
            STRNCPY(dataBuffer+STRLEN(dataBuffer), 
               pMe->m_pReadMsgBuffer, pMe->m_objPos);
            if (AEESMS_UDH_SMALL_PICTURE == message.emsObj.header_id) {
               STRCAT(dataBuffer, "      ");
            } else {
               STRCAT(dataBuffer, "            ");
            }
            STRCAT(dataBuffer, (pMe->m_pReadMsgBuffer + pMe->m_objPos));
         }
         if (message.bIsConcatMsg && (message.msgSeqNum < message.msgTotal)) {
            STRCAT(dataBuffer, "+");
         }
      }
   } else {
      pMe->m_appstate = APPSTATE_READ_OUTBOX;
      if (!message.bIsConcatMsg) {
         SPRINTF(dataBuffer, "To: %s\n %s\n\0", 
            message.number.destNumber, message.msgAttribute.msgString);
      } else {
         SPRINTF(dataBuffer, "To: %s\n %s\nMessage %d of %d\n\0", 
            message.number.destNumber, message.msgAttribute.msgString,
            message.msgSeqNum, message.msgTotal);
      }
      
      if (pMe->m_pReadMsgBuffer) {
         if (message.bIsConcatMsg && (message.msgSeqNum > 1)) {
            STRCAT(dataBuffer, "+");
         }
         if (pMe->m_objPos == -1) {
            STRCAT(dataBuffer, pMe->m_pReadMsgBuffer);
         } else {
            STRNCPY(dataBuffer+STRLEN(dataBuffer), 
               pMe->m_pReadMsgBuffer, pMe->m_objPos);
            if (AEESMS_UDH_SMALL_PICTURE == message.emsObj.header_id) {
               STRCAT(dataBuffer, "      ");
            } else {
               STRCAT(dataBuffer, "            ");
            }
            STRCAT(dataBuffer, (pMe->m_pReadMsgBuffer + pMe->m_objPos));
         }
         if (message.bIsConcatMsg && (message.msgSeqNum < message.msgTotal)) {
            STRCAT(dataBuffer, "+");
         }
      }
   }
   
   rect.x = 0;
   rect.y = 0;
   rect.dx = pMe->DeviceInfo.cxScreen;
   rect.dy = pMe->DeviceInfo.cyScreen;
   
   ISTATIC_SetRect(pMe->m_pIStatic, &rect);
   ISTATIC_Reset(pMe->m_pIStatic);
   ISTATIC_SetProperties(pMe->m_pIStatic, ST_ASCII);
   ISTATIC_SetText(pMe->m_pIStatic, NULL, (AECHAR *)dataBuffer, AEE_FONT_NORMAL, AEE_FONT_NORMAL);
   ISTATIC_SetActive(pMe->m_pIStatic, TRUE);
   ISTATIC_Redraw(pMe->m_pIStatic);

   FREEIF(hdr);
   FREEIF(dataBuffer);

}

/*===========================================================================
FUNCTION c_samplesms_EditMessage

DESCRIPTION
   This function will bring the user to the Compose Screen 
   allowing the user to edit(and/or send) the selected draft message.
   
PROTOTYPE:
   void c_samplesms_EditMessage(CSampleSMS *pi, MyMessage message);
===========================================================================*/
void c_samplesms_EditMessage(CSampleSMS *pMe, MyMessage message) {
   c_samplesms_ResetControls(pMe);

   IDISPLAY_ClearScreen(pMe->m_pIDisplay);
   IDISPLAY_Update(pMe->m_pIDisplay);
   c_samplesms_ComposeScreen(pMe);
   
   STRTOWSTR(message.number.destNumber, pMe->m_wszStringBuffer, 
      (STRLEN(message.number.destNumber)+1)*sizeof(AECHAR));
   ITEXTCTL_SetText(pMe->m_pITextDest1, pMe->m_wszStringBuffer, STRLEN(message.number.destNumber)+1);
   ITEXTCTL_Redraw(pMe->m_pITextDest1);
   
   if (pMe->m_pReadMsgBuffer) {
      STRTOWSTR(pMe->m_pReadMsgBuffer, pMe->m_wszStringBuffer, 
         (STRLEN(pMe->m_pReadMsgBuffer)+1)*sizeof(AECHAR));
      ITEXTCTL_SetText(pMe->m_pITextMsg, pMe->m_wszStringBuffer, 
         STRLEN(pMe->m_pReadMsgBuffer)+1);
      ITEXTCTL_Redraw(pMe->m_pITextMsg);
   }

   ITEXTCTL_SetActive(pMe->m_pITextMsg, TRUE);
   ITEXTCTL_SetCursorPos(pMe->m_pITextMsg, TC_CURSOREND);
   ITEXTCTL_SetActive(pMe->m_pITextDest1, FALSE);
   ITEXTCTL_SetActive(pMe->m_pITextDest2, FALSE);

   pMe->m_bEditMode = TRUE;      // editing draft
   pMe->m_appstate = APPSTATE_INPUT_MSG;
   if (pMe->m_objPos >=0) {
      c_samplesms_DrawSoftKey(pMe, IDS_SEND, IDS_DEL_OBJECT);
   } else {
      c_samplesms_DrawSoftKey(pMe, IDS_SEND, IDS_INS_OBJECT);
   }
   IDISPLAY_Update(pMe->m_pIDisplay);
}

/*===========================================================================
FUNCTION c_samplesms_MarkAsRead

DESCRIPTION
   This function will change/update the icon of the message as a read message
   
PROTOTYPE:
   void c_samplesms_MarkAsRead(CSampleSMS * pi, uint16 itemID);
===========================================================================*/
void c_samplesms_MarkAsRead(CSampleSMS *pMe, uint16 itemID) {
   CtlAddItem msgItem;
         
   MEMSET(&msgItem, 0, sizeof(msgItem));
   msgItem.pImage = ISHELL_LoadResImage(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, IDI_ENV_OPENED);
   
   IMENUCTL_SetItem(pMe->m_pIMenuInboxMsgs, itemID, MSIF_IMAGE, &msgItem); 
   if (msgItem.pImage) {
      IImage_Release(msgItem.pImage);
   }   
}

