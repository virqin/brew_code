/*=============================================================================

FILE: DialerDiag.c

SERVICES: Dialer Applet

GENERAL DESCRIPTION: Dialer Diagnostic functions

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:


(c) COPYRIGHT 2004 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Dialer/DialerDiag.c#4 $
$DateTime: 2005/02/03 15:55:24 $
$Author: carls $
$Change: 162355 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what: where, why
--------   ---     ---------------------------------------------------------
02/03/05   cvs     Fix RVCT compiler error
03/03/04   JAS     Created Dialer Diag file; added functions to handle
                   "Get Call Status" and "Get Call Detail" diag requests.

=============================================================================*/
//lint -save -e611 -e740  Suppress lint complaints about suspicious ptr casts

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"
#include "AEEAppGen.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEText.h"
#include "AEE.h"
#include "AppComFunc.h"
#include "AEECM.h"
#include "DialerUtils.h"
#include "DialerApp.h"
#include "dialerapp_res.h"
#include "uiutils.h"
#include "AEEAlert.h"
#include "DialerApp.bid"

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //FEATURE_APP_TEST_AUTOMATION
