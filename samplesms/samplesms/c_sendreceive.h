#ifndef C_SENDRECEIVE_H
#define C_SENDRECEIVE_H
/*=====================================================================
FILE:  c_sendreceive.h

AUTHOR:
   QUALCOMM Incorporated

SERVICES: 
   C_SAMPLESMS applet showing usage of BREW ISMS interfaces.

DESCRIPTION:
   This file provides function prototypes for c_samplesms/c_ssendreceive.c


             Copyright © 2008 QUALCOMM Incorporated.
                    All Rights Reserved.
                 QUALCOMM Proprietary/GTDR
=====================================================================*/

#include "c_samplesms.h"

/*===============================================================================
                        FUNCTION PROTOTYPES
=============================================================================== */
void c_samplesms_SetPayloadEncoding(CSampleSMS *pMe);
boolean c_samplesms_SetDefaultPayloadEncoding(CSampleSMS *pMe);
void c_samplesms_InitMOSMSEncoding(CSampleSMS *pMe);
void c_samplesms_SetMOEncoding(CSampleSMS *pme, uint16 selectedMOEnc);
void c_samplesms_ComposeScreen(CSampleSMS *pMe);
void c_samplesms_DrawComposeScreenAttribute(CSampleSMS *pMe);
void c_samplesms_BackToCompose(CSampleSMS *pMe);
void c_samplesms_InsertEMSObject(CSampleSMS *pMe);
void c_samplesms_DeleteEMSObject(CSampleSMS *pMe);
void c_samplesms_SetMessageOptions(CSampleSMS *pMe);
void c_samplesms_SaveDraftData(CSampleSMS *pMe);
void c_samplesms_SetMessageTag(ISMSMsg *pISMSMsg, AEESMSTag tag);
int c_samplesms_InitializeSending(CSampleSMS *pMe);
void c_samplesms_SendSMS(CSampleSMS *pMe);
void c_samplesms_ReceiveSMSMessage(CSampleSMS *pMe, uint32 messageID);

#endif
