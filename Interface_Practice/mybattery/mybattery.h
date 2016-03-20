/****************************************************************************
FILE		: mybattery.h
SERVICES	: Header file for mybattery.c sample application
DESCRIPTION	: Data structure and macro definitions for mybattery.c
AUTHOR		: Tech. Support, UniBrew
DATE		: January 2007
NOTES		: None
****************************************************************************/
#ifndef MYBATTER_H
#define MYBATTER_H


#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEStdLib.h"
#include "AEEMenu.h"
#include "AEEBattery.h"			// IBattery interface definitions
#include "mybattery.bid"

#define TEXT_BUF_SIZE  sizeof(AECHAR)*1024
#define BOTTOM_TEXT_HEIGHT  15

#define RT_BATTERY_INFO		201
#define RT_BATTERY_NOTIFY	    202
#define RT_BATTERY_NOTIFY_CANCEL	203
#define RT_QUERYINT_IMODEL	204
#define RT_QUERYINT_IMODEL_CANCEL	 205

#define SOFTMENU_BACK		300
/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _mybattery {
    AEEApplet		a ;	         // First element of this structure must be AEEApplet
    AEEDeviceInfo	DeviceInfo; // always have access to the hardware device information
	
	IDisplay		*m_pIDisplay;
	IShell			*m_pIShell;
	IStatic			*m_pIStatic;
	IMenuCtl		*m_pIMainMenu;
	IMenuCtl		*m_pISoftMenu;
	AECHAR			m_szTitle[30];	// menu title
	AECHAR			*m_pszText;		// menu text
	
	IBattery		*m_pIBattery;
	IModel            *m_pIModel;
	ModelListener myModelListerner;
} mybattery;

#endif
