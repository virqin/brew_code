#ifndef C_SMSSTORAGE_H
#define C_SMSSTORAGE_H
/*=====================================================================
FILE:  c_smsstorage.h

AUTHOR:
   QUALCOMM Incorporated

SERVICES: 
   C_SAMPLESMS applet showing usage of BREW ISMS interfaces.

DESCRIPTION:
   This file provides function prototypes for c_samplesms/c_smsstorage.c


               Copyright © 2008 QUALCOMM Incorporated.
                      All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
=====================================================================*/

#include "c_samplesms.h"

/*===============================================================================
                        FUNCTION PROTOTYPES
=============================================================================== */
boolean c_samplesms_SetStorageType(CSampleSMS *pMe);
void c_samplesms_GetMessageCounters(CSampleSMS *pMe);
void c_samplesms_GetStorageStatus(CSampleSMS *pMe, AEESMSTag msgTag);
void c_samplesms_AccessStorage(CSampleSMS *pMe, AEESMSTag msgTag);
void c_samplesms_GetMessage(CSampleSMS *pMe, AEESMSTag msgTag);
void c_samplesms_DeleteMessage(CSampleSMS *pMe, uint16 msgIndex);
void c_samplesms_DeleteMessageTag(CSampleSMS *pMe, AEESMSTag tag);
void c_samplesms_DeleteMessageAll(CSampleSMS *pMe);
void c_samplesms_SMSStorage_ModelListener(CSampleSMS *pMe, ModelEvent *pModelEvent);
void c_samplesms_ReadMessage(CSampleSMS *pMe, uint16 msgIndex);
void c_samplesms_StoreMessage(CSampleSMS *pMe);
void c_samplesms_StoreRcvdMessage(CSampleSMS *pMe);
void c_samplesms_UpdateMessage(CSampleSMS *pMe);

#endif
