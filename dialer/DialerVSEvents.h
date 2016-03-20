#ifndef DIALERVSEVENTS_H
#define DIALERVSEVENTS_H

/*=============================================================================

FILE: DialerVSEvents.h

GENERAL DESCRIPTION: VideoShare Events to Dialer app

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2002 - 2006 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE


This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
05/31/06   RI      Initial revision.
-------------------------------------------------------------------------------
05/28/07   Michele Moved BREW specific values from IpcMessages.h to this file
-------------------------------------------------------------------------------
16/11/07   Michele Revert to previous version to avoid header conflict on 6800.
-------------------------------------------------------------------------------
21/03/08   Michele Added new events for notifications about registration events.
-------------------------------------------------------------------------------
Dec 15,08  Julie   Added new events for notifications about link up/down,  
                   options forbidden, & options attempt stopped, requested by MOT.
                   (ECR166978, ECR166979)
=============================================================================*/

#include <AEE.h>

// Events to Dialer for IMS registration and deregistration.
#define EVT_QP_IMS_REGISTRATION                       EVT_USER + 2000
#define EVT_QP_IMS_DEREGISTRATION                     EVT_USER + 2001

// Events to DialerApp about VideoShare session.
#define EVT_QP_IMS_VIDEOSHARE_INVITING                EVT_USER + 2002
#define EVT_QP_IMS_VIDEOSHARE_RINGING                 EVT_USER + 2003
#define EVT_QP_IMS_VIDEOSHARE_ACTIVE                  EVT_USER + 2004
#define EVT_QP_IMS_VIDEOSHARE_CLOSING                 EVT_USER + 2005
#define EVT_QP_IMS_VIDEOSHARE_END                     EVT_USER + 2006
#define EVT_QP_IMS_VIDEOSHARE_AVAILABLE               EVT_USER + 2007
#define EVT_QP_IMS_VIDEOSHARE_UNAVAILABLE             EVT_USER + 2008

// Events to Dialer to indicate what happens in RegManager
#define EVT_QP_IMS_REGISTRATION_STARTED               EVT_USER + 2010
#define EVT_QP_IMS_REGISTRATION_OK                    EVT_USER + 2011
#define EVT_QP_IMS_REGISTRATION_FAILURE               EVT_USER + 2012
#define EVT_QP_IMS_DEREGISTRATION_STARTED             EVT_USER + 2013
#define EVT_QP_IMS_DEREGISTRATION_OK                  EVT_USER + 2014
#define EVT_QP_IMS_REGISTRATION_ATTEMPTS_STOPPED      EVT_USER + 2015

// Events to Dialer to indicate link up / link down, requested by MOT
#define EVT_QP_IMS_LINKUP                             EVT_USER + 2016
#define EVT_QP_IMS_LINKDOWN                           EVT_USER + 2017

// Events to Dialer to indicate what happened in options, requested by MOT
#define EVT_QP_IMS_OPTIONS_FORBIDDEN                  EVT_USER + 2018
#define EVT_QP_IMS_OPTIONS_ATTEMPTS_STOPPED           EVT_USER + 2019

#endif // DIALERVSEVENTS_H

