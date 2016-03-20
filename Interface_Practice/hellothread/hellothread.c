/*===========================================================================

FILE: hellothread.c

SERVICES: Sample applet using AEE

DESCRIPTION
  This file contains usage examples of following interfaces. 
  AEECLSID_THREAD

PUBLIC CLASSES:  
   N/A

       Copyright © 2000-2008
                    All Rights Reserved.
          
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
// Mandatory includes
#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEAppGen.h"        // Applet helper file
#include "nmdef.h"
#include "AEEThread.h"
#include "AEEGraphics.h"


// Services used by app
#include "hellothread.bid"
#include "hellothread.brh"
#include "hellothread.h"

/*-------------------------------------------------------------------
Static function prototypes
-------------------------------------------------------------------*/
static void Thread1Start( IThread *pThread, SThreadCtx *pMe );
static void Thread2Start( IThread *pThread, SThreadCtx *pMe );
static void Thread1Main( SThreadCtx *pMe );
static void Thread2Main( SThreadCtx *pMe );
static void Thread2Done( HelloThreadApp *pMe );
static boolean HelloWorld_HandleEvent(AEEApplet * pme, AEEEvent eCode,uint16 wParam, uint32 dwParam);

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/

// Size of buffer used to hold text strings for display
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
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
{
	*ppObj = NULL;
	
	if(AEEApplet_New( sizeof(HelloThreadApp),                  // Size of our private class
		ClsId,                              // Our class ID
		pIShell,                            // Shell interface
		pMod,                               // Module instance
		(IApplet**)ppObj,                   // Return object
		(AEEHANDLER)HelloWorld_HandleEvent, // Our event handler
		NULL))                              // No special "cleanup" function
		return(AEE_SUCCESS);
	
	return (EFAILED);
}

static void Thread1Start( IThread *pThread, SThreadCtx *pMe )
{
	Thread1Main( pMe );
}

static void Thread1Main( SThreadCtx *pMe )
{
	AECHAR wsz[ 16 ];
	
	while( 1 )
	{
		pMe->n++;
		
		WSPRINTF( wsz, sizeof( wsz ), L"t1=%d", pMe->n );
		
		IDISPLAY_DrawText(pMe->pid,    // Display instance
			AEE_FONT_NORMAL,     // Use BOLD font
			wsz,         // Text - Normally comes from resource
			-1,                  // -1 = Use full string length
			0,                   // Ignored - IDF_ALIGN_CENTER
			0,                   // Ignored - IDF_ALIGN_MIDDLE
			NULL,                // No clipping
			IDF_ALIGN_CENTER | IDF_ALIGN_TOP);
		IDISPLAY_Update (pMe->pid);
		
		ISHELL_Resume( pMe->pis, ITHREAD_GetResumeCBK( pMe->pit ) );
		ITHREAD_Suspend( pMe->pit );
	}
}

static void Thread2Start( IThread *pit, SThreadCtx *pMe )
{
	Thread2Main( pMe );
}

static void Thread2Main( SThreadCtx *pMe )
{
	AECHAR wsz[ 16 ];
	
	while( pMe->n < 256 )
	{
		AEECircle circle;
		
		pMe->n++;
		//circle.cx = 176/2;
		circle.cx = 128/2;
		circle.cy = 120;
		circle.r = pMe->n % 20;
		
		WSPRINTF( wsz, sizeof( wsz ), L"t2=%d", pMe->n );
		
		IDISPLAY_DrawText(pMe->pid,    // Display instance
			AEE_FONT_NORMAL,     // Use BOLD font
			wsz,         // Text - Normally comes from resource
			-1,                  // -1 = Use full string length
			0,                   // Ignored - IDF_ALIGN_CENTER
			0,                   // Ignored - IDF_ALIGN_MIDDLE
			NULL,                // No clipping
			IDF_ALIGN_CENTER | IDF_ALIGN_BOTTOM);
		
		IDISPLAY_EraseRgn( pMe->pid, 0, 70, 128, 90 );
		IGRAPHICS_DrawCircle( pMe->pig, &circle );
		
		IDISPLAY_Update (pMe->pid);
		ISHELL_Resume( pMe->pis, ITHREAD_GetResumeCBK( pMe->pit ) );
		ITHREAD_Suspend( pMe->pit );
	}
	ITHREAD_Exit( pMe->pit, SUCCESS );
}

static void Thread2Done( HelloThreadApp *pMe )
{
	DBGPRINTF( pMe->nThread2Result == SUCCESS ? "Thread2 exit SUCCESS" : "Thread2 exit FAILED" );
}

static boolean HelloWorld_HandleEvent(AEEApplet * p, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	AECHAR szText[] = {'H','e','l','l','o',' ','W','o', 'r', 'l', 'd', '\0'};
	HelloThreadApp *pMe = (HelloThreadApp *)p;
	
	switch (eCode)
    {
	case EVT_APP_START:                        
		{
			int r;
			r = ISHELL_CreateInstance( p->m_pIShell, AEECLSID_THREAD, (void **)&pMe->stc1.pit );
			r = ISHELL_CreateInstance( p->m_pIShell, AEECLSID_THREAD, (void **)&pMe->stc2.pit );
			r = ISHELL_CreateInstance( p->m_pIShell, AEECLSID_GRAPHICS, (void **)&pMe->stc2.pig );
			pMe->stc1.pis = p->m_pIShell;
			pMe->stc1.pid = p->m_pIDisplay;
			pMe->stc2.pis = p->m_pIShell;
			pMe->stc2.pid = p->m_pIDisplay;
			
			CALLBACK_Init( &pMe->cbThread2Done, (PFNNOTIFY)Thread2Done, (void *)pMe );
			ITHREAD_HoldRsc( pMe->stc2.pit, (IBase *)pMe->stc2.pig );
			ITHREAD_Join( pMe->stc2.pit, &pMe->cbThread2Done, &pMe->nThread2Result );
			ITHREAD_Start( pMe->stc1.pit, 128, (PFNTHREAD)Thread1Start, (void *)&pMe->stc1 );
			ITHREAD_Start( pMe->stc2.pit, 128, (PFNTHREAD)Thread2Start, (void *)&pMe->stc2 );
			
			IDISPLAY_DrawText(p->m_pIDisplay,    // Display instance
				AEE_FONT_BOLD,       // Use BOLD font
				szText,              // Text - Normally comes from resource
				-1,                  // -1 = Use full string length
				0,                   // Ignored - IDF_ALIGN_CENTER
				0,                   // Ignored - IDF_ALIGN_MIDDLE
				NULL,                // No clipping
				IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
			IDISPLAY_Update (p->m_pIDisplay);
		}
		return(TRUE);
		
    case EVT_APP_STOP:
		{
			IGRAPHICS_Release( pMe->stc2.pig );
			ITHREAD_Release( pMe->stc1.pit );
			ITHREAD_Release( pMe->stc2.pit );
		}
		return(TRUE);
		
		
    default:
		break;
	}
	return(FALSE);
}



