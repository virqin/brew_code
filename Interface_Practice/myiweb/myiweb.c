/*===========================================================================

FILE: myiweb.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions

#include "myiweb.bid"
#include "myiweb_res.h"
#include "AEEFile.h"			// File interface definitions
#include "AEENet.h"				// Socket interface definitions
#include "AEEHtmlViewer.h"
#include "AEEStdLib.h"
#include "AEEMenu.h" 
#define TEST_HOST  "http://192.168.84.40/" //"http://10.10.10.84/"
#define TEST_URL  "http://192.168.84.40/brewtest.htm" //"http://10.10.10.84/brewtest.htm"
#define FIND_STRING(a, b) if (STRISTR(a, b) != NULL)
/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _myiweb {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface
	AEERect              m_rc; 
    // add your own variables here...
	
	IWeb*	m_pIWeb;
	IMenuCtl *        m_pIMenu;
	IStatic *		m_pIStatic;
	IHtmlViewer *        m_pHTMLViewer;  
	IFileMgr * m_pIFileMgr;
	IMenuCtl *        m_pIFileMenu;
	IMenuCtl *        m_pIMediaMenu;

	IPeek                *pipPostData;  // POST stream, if any
	char                 *pszPostData;  // POST string
	char	* m_pszFileName;
	AEECallback          cb;            // how to call me back
	
	IWebResp *           piWResp;       // the answer I get from IWeb
	
	byte *		m_BodyBuffer;		// Buffer to hold HTML data
	int			m_BodySize;			// Size of HTML data in buffer
	int			m_BodyAllocSize;	// Allocated size of HTML data buffer

	int					m_iAppStatus;
	uint32               uStart;
	uint32               uRecvStart;
	int m_iMediaType;
	char targetUrl[128];
} myiweb;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean myiweb_HandleEvent(myiweb* pMe, AEEEvent eCode, 
                                             uint16 wParam, uint32 dwParam);
boolean myiweb_InitAppData(myiweb* pMe);
void    myiweb_FreeAppData(myiweb* pMe);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */
static void	 myiweb_DisplayInfo(myiweb* pMe,AECHAR* pInfo);
static void  myiweb_ShowMenu(myiweb* pMe);
static void myiwebhtml_NotifyCB( void* pvUser, HViewNotify* pNotify );

static void myiweb_Start(myiweb *pMe, char *pszUrl,int iPage);
static void    myiweb_GotResp(void *p);
static void    myiweb_Header(void *p, const char *cpszName, GetLine *pglVal);
static void    myiweb_Status(void *p, WebStatus ws, void *pVal);
static void    myiweb_Stop(myiweb *pwa);
static void myiweb_DownloadFile(void *p);
static void myiweb_download(myiweb *pMe, char *pszUrl);
static void  myiweb_ShowFileMenu(myiweb* pMe);

static char * myiweb_GetFileName(const char * psz);
static boolean myiweb_AddMenuItem(IMenuCtl * pMenu, uint16 wTextID, AECHAR * pText, uint16 wImageID, uint16 wItemID, uint32 dwData);
/*===========================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION
	This function is invoked while the app is being loaded. All Modules must provide this 
	function. Ensure to retain the same name and parameters for this function.
	In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
	that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IDisplay_InitAppData(). 
   This is done as follows: InitAppData() is called to initialize AppletData 
   instance. It is app developers responsibility to fill-in app data initialization 
   code of InitAppData(). App developer is also responsible to release memory 
   allocated for data contained in AppletData -- this can be done in 
   IDisplay_FreeAppData().

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

	if( ClsId == AEECLSID_MYIWEB )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(myiweb),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)myiweb_HandleEvent,
                          (PFNFREEAPPDATA)myiweb_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(myiweb_InitAppData((myiweb*)*ppObj))
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
FUNCTION SampleAppWizard_HandleEvent

DESCRIPTION
	This is the EventHandler for this app. All events to this app are handled in this
	function. All APPs must supply an Event Handler.

PROTOTYPE:
	boolean SampleAppWizard_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

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
static boolean myiweb_HandleEvent(myiweb* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	if (pMe->m_pHTMLViewer && IHTMLVIEWER_HandleEvent(pMe->m_pHTMLViewer, eCode, wParam, dwParam)&&(pMe->m_iAppStatus==1))
       return TRUE;
	if(pMe->m_pIMenu && IMENUCTL_HandleEvent(pMe->m_pIMenu, eCode, wParam, dwParam) && pMe->m_iAppStatus ==0 )
					return TRUE;
	if(pMe->m_pIFileMenu && IMENUCTL_HandleEvent(pMe->m_pIFileMenu, eCode, wParam, dwParam) )
					return TRUE;
    switch (eCode) 
	{
        // App is told it is starting up
        case EVT_APP_START:                        
		    // Add your code here...
			myiweb_ShowMenu(pMe);
            return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...
			myiweb_Stop(pMe);
      		return(TRUE);


        // App is being suspended 
        case EVT_APP_SUSPEND:
		    // Add your code here...

      		return(TRUE);


        // App is being resumed
        case EVT_APP_RESUME:
		    // Add your code here...
			
      		return(TRUE);


        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
			// Add your code here...
			
			return(TRUE);
			
			// A key was pressed. Look at the wParam above to see which key was pressed. The key
		case EVT_KEY:
			// Add your code here...			
			if(wParam ==AVK_CLR)
			{
				if(pMe->m_pIStatic)
				{
					ISTATIC_Release(pMe->m_pIStatic);
					pMe->m_pIStatic = NULL;
				}
				if(pMe->m_pIMediaMenu)
				{
					IMENUCTL_Release(pMe->m_pIMediaMenu);
					pMe->m_pIMediaMenu =NULL;
					myiweb_ShowFileMenu(pMe);
					return TRUE;

				}
				if(pMe->m_iAppStatus >0)
				{
					IHTMLVIEWER_SetNotifyFn(pMe->m_pHTMLViewer, NULL, pMe);
					
				}else if(pMe->m_iAppStatus ==0)
				{
					ISHELL_CloseApplet(pMe->pIShell,FALSE);
					return TRUE;
				}
				if(pMe->m_pIFileMenu)
				{
					IFILEMGR_Release(pMe->m_pIFileMenu);
					pMe->m_pIFileMenu = NULL;
				}
				myiweb_ShowMenu(pMe);
				return TRUE;
			}
			
			
		case EVT_COMMAND:
			if( pMe->m_pIMenu )
			{
				switch(IMENUCTL_GetSel(pMe->m_pIMenu))
				{
				case IDS_WCONNECT:
					myiweb_Start(pMe,TEST_URL,0);
					break;
				case IDS_FILE_MGR:
					myiweb_ShowFileMenu(pMe);		
					break;
				case IDS_FAVOURITE:
					break;
				case IDS_ABOUT:
					break;
				}
				IMENUCTL_Release(pMe->m_pIMenu);
				pMe->m_pIMenu = NULL;
				
			}else if(pMe->m_pIFileMenu)
			{
				char foldername[8];
				
				switch(IMENUCTL_GetSel(pMe->m_pIFileMenu))
				{
				case IDS_MEDIA_PIC:
					STRCPY(foldername,"PIC");
					break;
				case IDS_MEDIA_MUSIC:
					STRCPY(foldername,"MUSIC");		
					break;
				}
				IMENUCTL_Release(pMe->m_pIFileMenu);
				pMe->m_pIFileMenu = NULL;

		if(!pMe->m_pIMediaMenu)
				ISHELL_CreateInstance(pMe->pIShell, AEECLSID_MENUCTL, (void **)&pMe->m_pIMediaMenu) ;
				if(!pMe->m_pIFileMgr)
					ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_FILEMGR, (void **)&pMe->m_pIFileMgr);
				if(IFILEMGR_Test(pMe->m_pIFileMgr,foldername) == SUCCESS)
				{
					
					uint16      wItemID = 0;
					AECHAR wszBuf[32];
					FileInfo    fi;
					IFILEMGR_EnumInit(pMe->m_pIFileMgr, foldername, FALSE);
					while (IFILEMGR_EnumNext(pMe->m_pIFileMgr, &fi))
					{
						char *   szName;				
						
						// Add only the file name
						szName = myiweb_GetFileName(fi.szName);
						if (szName)
						{
							uint16   wIconID = IDB_MENU;
							if (wIconID)
							{
								STRTOWSTR(szName, wszBuf, 32);
								myiweb_AddMenuItem(pMe->m_pIMediaMenu, 0, wszBuf, wIconID, (uint16)(100 + wItemID), 0);
							}
						}
						
						wItemID++;
					}
					if(pMe->m_pIMediaMenu)
						IMENUCTL_SetActive(pMe->m_pIMediaMenu,TRUE);
					IFILEMGR_Release(pMe->m_pIFileMgr);
					pMe->m_pIFileMgr = NULL;
					
				}else{
					AECHAR szEmpty[16];
					IFILEMGR_Release(pMe->m_pIFileMgr);
					pMe->m_pIFileMgr = NULL;
					IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
					ISHELL_LoadResString(pMe->a.m_pIShell, MYIWEB_RES_FILE, IDS_NO_MEDIA, szEmpty, sizeof(szEmpty));
					myiweb_DisplayInfo(pMe,szEmpty);
				}
				
				
			}
			return(TRUE);
			// If nothing fits up to this point then we'll just break out
        default:
            break;
	}
	
	return FALSE;
}


// this function is called when your application is starting up
boolean myiweb_InitAppData(myiweb* pMe)
{
    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

    // The display and shell interfaces are always created by
    // default, so we'll asign them so that you can access
    // them via the standard "pMe->" without the "a."
    pMe->pIDisplay = pMe->a.m_pIDisplay;
    pMe->pIShell   = pMe->a.m_pIShell;

    // Insert your code here for initializing or allocating resources...
 // Create each of the controls used by the application.
   if(//(ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void**)(&pMe->m_pFileMgr)) != SUCCESS) ||
      (ISHELL_CreateInstance(pMe->pIShell, AEECLSID_HTML, (void**)(&pMe->m_pHTMLViewer)) != SUCCESS) ||
    //  (ISHELL_CreateInstance(pIShell, AEECLSID_NET, (void**)(&pMe->m_pINetMgr)) != SUCCESS) ||
      (ISHELL_CreateInstance(pMe->pIShell, AEECLSID_WEB, (void **)(&pMe->m_pIWeb)) != SUCCESS))
   {
      IAPPLET_Release((IApplet*)pMe);
      return FALSE;
   }
   IHTMLVIEWER_SetNotifyFn(pMe->m_pHTMLViewer, (PFNHVIEWNOTIFY)myiwebhtml_NotifyCB, pMe);
   IHTMLVIEWER_SetProperties(pMe->m_pHTMLViewer, HVP_SCROLLBAR);

   // Get device screen rect
   SETAEERECT(&pMe->m_rc, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen);

    // Initialize the IWeb with a few options
   {
      int    i = 0;
      WebOpt awo[10]; 

      // set the IWeb connect timeout to 10 seconds.  this also sets the 
      // failover timeout, if unset, or set to 0, IWeb uses the system 
      // default (30 seconds unless an OEM changes it)
      awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
      awo[i].pVal = (void *)10000;
      i++;

      // test user-agent, uncomment this section to ship your own user-agent 
      // string. if unset, IWeb will send a default.  If set to NULL, no 
      // user agent header will be sent */

      // Set TEST_USER_AGENT in the NetDiagnostics project settings to all
      // shipping of your own user agent.

#ifdef TEST_USER_AGENT
      awo[i].nId  = WEBOPT_USERAGENT;
      awo[i].pVal = (void *)WEBBER_USERAGENT;
      i++;
#endif 

      // test nowaitconn, this only comes into effect if you build webber 
      // with multiple myiwebs (see the definition of struct Webber)
      awo[i].nId  = WEBOPT_FLAGS;
      awo[i].pVal = (void *)WEBREQUEST_NOWAITCONN;
      i++;
      

      // test forcenew, uncomment this section to try multiple simultaneous
      // "forced" new connections. Forced new connections are not kept alive
      // unless they are the first forced new connection to a host
#ifdef TEST_FORCENEWCONN
      awo[i].nId  = WEBOPT_FLAGS;
      awo[i].pVal = (void *)WEBREQUEST_FORCENEWCONN;
      i++;
#endif 

#ifdef TEST_USER_AGENT
      // turn off HTTP over HTTP proxying
      awo[i].nId  = WEBOPT_PROXYSPEC;
      awo[i].pVal = (void *)"http:///";
      i++;

      // turn on ALL proxying.  Proxyspecs are examined in WebOpt
      // order, so in this list, with the above and below PROXYSPECs,
      // everything except HTTP will be proxied through
      // http://webproxy.yourdomain.com:8080, (which you'll have to
      // set up to test, sorry
      awo[i].nId  = WEBOPT_PROXYSPEC;
      awo[i].pVal = (void *)"*:///http://webproxy.yourdomain.com:8080";
      i++;
#endif

      // Marks the end of the array of WebOpts
      awo[i].nId  = WEBOPT_END;
      
      // Add Options
      IWEB_AddOpt(pMe->m_pIWeb,awo);
   }

   	pMe->m_BodyBuffer = NULL;
	pMe->m_BodySize = 0;
	pMe->m_BodyAllocSize = 0;
    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void myiweb_FreeAppData(myiweb* pMe)
{
    // insert your code here for freeing any resources you have allocated...
	
	if( pMe->m_pIMenu )
	{
		IMENUCTL_Release(pMe->m_pIMenu);
		pMe->m_pIMenu = NULL;
	}
	if(pMe->m_pIStatic)
	{
		ISTATIC_Release(pMe->m_pIStatic);
		pMe->m_pIStatic = NULL;
	}

	if(pMe->m_pHTMLViewer)
	{
		IHTMLVIEWER_SetNotifyFn(pMe->m_pHTMLViewer, NULL, pMe);
		IHTMLVIEWER_Release(pMe->m_pHTMLViewer);
		pMe->m_pHTMLViewer = NULL;
	}
	if(pMe->m_pIWeb)
	{
		IWEB_Release(pMe->m_pIWeb);
		pMe->m_pIWeb = NULL;
	}
}
static void  myiweb_ShowMenu(myiweb* pMe)
{	
	CtlAddItem  ai;
	if (! pMe->m_pIMenu)
		ISHELL_CreateInstance(pMe->pIShell, AEECLSID_MENUCTL, (void **)&pMe->m_pIMenu) ;
	
	IMENUCTL_SetTitle(pMe->m_pIMenu,MYIWEB_RES_FILE,IDS_TITLE,NULL);

	ai.pText = NULL;
	ai.pImage = NULL;
	ai.pszResImage = MYIWEB_RES_FILE;
	ai.pszResText = MYIWEB_RES_FILE;
	ai.wFont = AEE_FONT_NORMAL;
	ai.dwData = NULL;
	
	ai.wText = IDS_WCONNECT;   
	ai.wImage = IDB_MENU;
	ai.wItemID = IDS_WCONNECT;
	
	
	// Add the item to the menu control
	if(IMENUCTL_AddItemEx(pMe->m_pIMenu, &ai )==FALSE)
	{
		IMENUCTL_Release(pMe->m_pIMenu);
		return;
	}
	
	ai.wText = IDS_FILE_MGR;
	ai.wItemID = IDS_FILE_MGR;
	
	// Add the item to the menu control
	IMENUCTL_AddItemEx( pMe->m_pIMenu, &ai );

	ai.wText = IDS_FAVOURITE;
	ai.wItemID = IDS_FAVOURITE;
	
	// Add the item to the menu control
	IMENUCTL_AddItemEx( pMe->m_pIMenu, &ai );

	ai.wText = IDS_ABOUT;
	ai.wItemID = IDS_ABOUT;
	
	// Add the item to the menu control
	IMENUCTL_AddItemEx( pMe->m_pIMenu, &ai );
	
	IMENUCTL_SetActive(pMe->m_pIMenu,TRUE);
	pMe->m_iAppStatus = 0;
}
static void	 myiweb_DisplayInfo(myiweb* pMe,AECHAR* pInfo)
{
		AEERect rct;
	
	if(!pMe->m_pIStatic)
		ISHELL_CreateInstance(pMe->pIShell, AEECLSID_STATIC, (void **)&pMe->m_pIStatic);
	SETAEERECT(&rct, 16, 16, pMe->DeviceInfo.cxScreen-32, pMe->DeviceInfo.cyScreen-32);
	ISTATIC_SetRect(pMe->m_pIStatic, &rct);
	ISTATIC_SetText(pMe->m_pIStatic, NULL,pInfo, AEE_FONT_BOLD, AEE_FONT_NORMAL);
	ISTATIC_Redraw(pMe->m_pIStatic);
}
static void myiwebhtml_NotifyCB( void* pvUser, HViewNotify* pNotify )
{
   myiweb* pMe = (myiweb*) pvUser;   

   switch( pNotify->code )
   {

   case HVN_REDRAW_SCREEN:
      IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
      IHTMLVIEWER_Redraw(pMe->m_pHTMLViewer);
      break;

   case HVN_JUMP:
   case HVN_SUBMIT:
	   STRCPY(pMe->targetUrl,TEST_HOST);
	   pMe->m_pszFileName = (char*)MALLOC(16);
	   FIND_STRING(pNotify->u.jump.pszURL, "PIC")
	   {
			STRCAT(pMe->targetUrl,pNotify->u.jump.pszURL+4);
			STRCPY(pMe->m_pszFileName,pNotify->u.jump.pszURL+4);
			pMe->m_iMediaType = 1;
	   }
	   FIND_STRING(pNotify->u.jump.pszURL, "MUSIC")
	   {
		   STRCAT(pMe->targetUrl,pNotify->u.jump.pszURL+6);
		   STRCPY(pMe->m_pszFileName,pNotify->u.jump.pszURL+6);
		   pMe->m_iMediaType = 2;
	   }
	 //  IHTMLVIEWER_SetNotifyFn(pMe->m_pHTMLViewer, NULL, pMe);
	   myiweb_Stop(pMe);
		myiweb_Start(pMe,pMe->targetUrl,1);

      break;

   case HVN_DONE:
      IHTMLVIEWER_SetRect(pMe->m_pHTMLViewer, &pMe->m_rc);
      IHTMLVIEWER_Redraw( pMe->m_pHTMLViewer );
      break;

   }
}
static void myiweb_Start(myiweb *pMe, char *pszUrl,int iPage)
{
   ISourceUtil *pisu;
	pMe->m_iAppStatus = 1;
   // look to see if there's POST data, this is totally non-standard, but 
   // easy to put into tests
   pMe->pszPostData = STRCHR(pszUrl, 1);
   
   // if there's post data, construct a stream for IWeb to consume
   if ((char *)0 != pMe->pszPostData) 
   {
      *pMe->pszPostData = 0;
      if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) 
	  {
         
         ISOURCEUTIL_PeekFromMemory(pisu, pMe->pszPostData + 1, 
                                    STRLEN(pMe->pszPostData + 1), 0, 0,
                                    &pMe->pipPostData);
         ISOURCEUTIL_Release(pisu);
      }
   }

   // initialize the callback, where I'll be called when the request
   // completes
   if(iPage == 0)
   {
	   CALLBACK_Cancel(&pMe->cb);
	   CALLBACK_Init(&pMe->cb, myiweb_GotResp, pMe);
   }else if(iPage ==1)
   {
	   CALLBACK_Cancel(&pMe->cb);
	   CALLBACK_Init(&pMe->cb, myiweb_DownloadFile, pMe);
   }
   pMe->uStart = GETUPTIMEMS();
   // start transaction, pass callbacks for web status, web headers
   // the extra WEBOPT_HEADER is used to help test what's sent 
   // (snoop.sh above shows all the headers)
   if(!pMe->m_pIWeb)
	   ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)(&pMe->m_pIWeb));
   if ((IPeek *)0 != pMe->pipPostData) {
      IWEB_GetResponse(pMe->m_pIWeb,
                       (pMe->m_pIWeb, &pMe->piWResp, &pMe->cb, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe,
                        WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
                        WEBOPT_HEADERHANDLER, myiweb_Header, 
                        WEBOPT_STATUSHANDLER, myiweb_Status, 
                        WEBOPT_METHOD, "POST",
                        WEBOPT_BODY, pMe->pipPostData,
                        WEBOPT_CONTENTLENGTH, STRLEN(pMe->pszPostData + 1),
                        WEBOPT_END));
   } else {
      IWEB_GetResponse(pMe->m_pIWeb,
                       (pMe->m_pIWeb, &pMe->piWResp, &pMe->cb, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe, 
                        WEBOPT_HEADER, "X-Method: GET \r\n",
                        WEBOPT_HEADERHANDLER, myiweb_Header, 
                        WEBOPT_STATUSHANDLER, myiweb_Status, 
                        WEBOPT_END));
   }
}

static void myiweb_GotResp(void *p)
{
	myiweb *pwa = (myiweb *)p;
	
	WebRespInfo *pwri;
	
	// get information about how the web transaction went
	// pwa->piWResp is ***NEVER NULL***, even though the transaction may fail
	// for wont of memory
	pwri = IWEBRESP_GetInfo(pwa->piWResp);
	
	//   ND_Print(pMe, "** got response...\n** info code: %d\n", pwri->nCode);
	
	// body may be NULL
	if ((ISource *)0 != pwri->pisMessage) 
	{
		IHTMLVIEWER_LoadSource(pwa->m_pHTMLViewer, pwri->pisMessage);
	}
	
}

/*===========================================================================

FUNCTION: myiweb_Header

DESCRIPTION:
   Received header callback for a web transaction. cpszName is NULL in the case 
   of continuation header line parts.

PARAMETERS:
   p: a myiweb (the subscriber)
   cpszName: the name of the web header (like "Content-Type")
   pglVal: the value of the header, like "text/html"

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Header(void *p, const char *cpszName, GetLine *pglVal)
{
   myiweb *pwa = (myiweb *)p;
  
  
 //  if (pMe->m_bRS) { // If response is to be displayed
      if ((char *)0 != cpszName) {
        // ND_Print(pMe, "%s:", cpszName);
      }
      //ND_Print(pMe, "%s\n", pglVal->psz);
  // }
}

/*===========================================================================

FUNCTION: myiweb_Status

DESCRIPTION:
   Web status callback for a Web transaction 

PARAMETERS:
   p: a myiweb (the subscriber)
   ws: type of status
   pVal: unused as of yet

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Status(void *p, WebStatus ws, void *pVal)
{
   AECHAR     szText[32];
   myiweb * pMe = (myiweb *)p;
   
   (void)pVal;

   switch (ws) {
   case WEBS_CANCELLED:
	  ISHELL_LoadResString(pMe->a.m_pIShell, MYIWEB_RES_FILE, IDS_WEBS_CANCELLED, szText, sizeof(szText));
      break;
   case WEBS_GETHOSTBYNAME:
      ISHELL_LoadResString(pMe->a.m_pIShell, MYIWEB_RES_FILE, IDS_WEBS_GETHOSTBYNAME,szText, sizeof(szText));
      break;          
   case WEBS_CONNECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, MYIWEB_RES_FILE, IDS_WEBS_CONNECT, szText, sizeof(szText));
      break;
   case WEBS_SENDREQUEST:
      ISHELL_LoadResString(pMe->a.m_pIShell, MYIWEB_RES_FILE, IDS_WEBS_SENDREQUEST, szText, sizeof(szText));
      break;
   case WEBS_READRESPONSE:
      ISHELL_LoadResString(pMe->a.m_pIShell, MYIWEB_RES_FILE, IDS_WEBS_READRESPONSE, szText, sizeof(szText));
      break;
   case WEBS_GOTREDIRECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, MYIWEB_RES_FILE, IDS_WEBS_GOTREDIRECT, szText, sizeof(szText));
      break;
   case WEBS_CACHEHIT:
      ISHELL_LoadResString(pMe->a.m_pIShell, MYIWEB_RES_FILE, IDS_WEBS_CACHEHIT, szText, sizeof(szText));
      break;
   }

   // show that status!
   if ((AECHAR *)0 != szText) 
   {
      myiweb_DisplayInfo(pMe,szText);
   }
}

/*===========================================================================

FUNCTION: myiweb_Stop

DESCRIPTION:
   Halts a web transaction, wrapped/represented by a myiweb

PARAMETERS:
   pwa: the myiweb

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Stop(myiweb *pwa)
{
   // this cancels any pending web transaction, or readable on the 
   // response body.  if nothing is pending, this has no effect
   CALLBACK_Cancel(&pwa->cb);
   
   // then clean up, if necessary
   if ((char *)0 != pwa->pszPostData) {
      *pwa->pszPostData = 1; // write delimiter back in, if any
      pwa->pszPostData = 0;
   }
	if (pwa->piWResp)
	{
		IWEBRESP_Release(pwa->piWResp);
		pwa->piWResp= NULL;
	}

	if (pwa->m_pIWeb) 
	{
		IWEB_Release(pwa->m_pIWeb);
		pwa->m_pIWeb = NULL;
	}

	if (pwa->m_BodyBuffer)
	{
		FREE(pwa->m_BodyBuffer);
		pwa->m_BodyBuffer = NULL;
	}
	pwa->m_BodySize = 0;
	pwa->m_BodyAllocSize = 0;

}
static void myiweb_DownloadFile(void * p)
{
	myiweb *pMe = (myiweb *)p;
	
	WebRespInfo *pwri;
	char targetFile[24];
	byte buf[1024];
	int ByteCount;
	IFile * pFile;
	   
	   // get information about how the web transaction went
	   // pwa->piWResp is ***NEVER NULL***, even though the transaction may fail
	   // for wont of memory

	   pwri = IWEBRESP_GetInfo(pMe->piWResp);
	   
	   //   ND_Print(pMe, "** got response...\n** info code: %d\n", pwri->nCode);
	   if(WEB_ERROR_SUCCEEDED(pwri->nCode))
		   DBGPRINTF("request successful!");
	   else{
		   WEB_ERROR_MAP(pwri->nCode);
		   DBGPRINTF("request successful %i",pwri->nCode);
	   }
	   // body may be NULL
	   if ((ISource *)0 != pwri->pisMessage) 
	   {
		   ISource * pISource = pwri->pisMessage;
		   ByteCount =	ISOURCE_Read(pISource, (byte*)buf, sizeof(buf));

		   switch(ByteCount)
		   {
		   case ISOURCE_END:
			   if (pMe->m_BodySize < pMe->m_BodyAllocSize)
				   pMe->m_BodyBuffer[pMe->m_BodySize] = 0;
			   else
				   pMe->m_BodyBuffer[pMe->m_BodyAllocSize - 1] = 0;
			   
			   if(!pMe->m_pIFileMgr)
				   ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_FILEMGR, (void **)&pMe->m_pIFileMgr);
			   if(pMe->m_iMediaType == 1)
			   {
				   if (IFILEMGR_MkDir(pMe->m_pIFileMgr, "pic") != SUCCESS)
				   {	
					   IFILEMGR_Release(pMe->m_pIFileMgr);
					   pMe->m_pIFileMgr = NULL;
					   return;
				   }
				   STRCPY(targetFile,"pic\\");
			   }else if(pMe->m_iMediaType == 2)
			   {
				   if (IFILEMGR_MkDir(pMe->m_pIFileMgr, "music") != SUCCESS)
				   {	
					   IFILEMGR_Release(pMe->m_pIFileMgr);
					   pMe->m_pIFileMgr = NULL;
					   return;
				   }
				   STRCPY(targetFile,"music\\");
			   }
			   STRCAT(targetFile,pMe->m_pszFileName);
			   if(IFILEMGR_Test(pMe->m_pIFileMgr,targetFile)==SUCCESS)
				   IFILEMGR_Remove(pMe->m_pIFileMgr,targetFile);
			   pFile= IFILEMGR_OpenFile(pMe->m_pIFileMgr,targetFile,_OFM_CREATE);
			   IFILE_Write(pFile,(void*)pMe->m_BodyBuffer,pMe->m_BodyAllocSize);
			   IFILE_Release(pFile);
			   IFILEMGR_Release(pMe->m_pIFileMgr);
			   pMe->m_pIFileMgr = NULL;
			   
			   {
				   AECHAR szFile[64];
				   AECHAR szText[64];
				   STRTOWSTR(pMe->m_pszFileName,szFile,64);
				   ISHELL_LoadResString(pMe->a.m_pIShell, MYIWEB_RES_FILE, IDS_DOWNLOAD_OK, szText, sizeof(szText));
				   WSTRCAT(szFile,szText);
				   myiweb_DisplayInfo(pMe,szFile);
			   }
			   FREEIF(pMe->m_pszFileName);
			   break;
		   case ISOURCE_ERROR:
			   break;
		   case ISOURCE_WAIT:
			   ISOURCE_Readable(pISource, &pMe->cb);
			   break;
		   default:
			   
			   if (ByteCount)
			   {
				   if (pMe->m_BodySize + ByteCount > pMe->m_BodyAllocSize)
				   {
					   const int NewSize = pMe->m_BodyAllocSize + 1024;
					   byte* NewBuf = (byte*)REALLOC(pMe->m_BodyBuffer, NewSize);
					   
					   if (NewBuf)
					   {
						   pMe->m_BodyBuffer = NewBuf;
						   pMe->m_BodyAllocSize = NewSize;
					   }
				   }
				   if(pMe->m_BodySize + ByteCount <= pMe->m_BodyAllocSize)
				   {
					   MEMCPY(pMe->m_BodyBuffer + pMe->m_BodySize, buf, ByteCount);
					   pMe->m_BodySize += ByteCount;
				   }
			   }
			   
			   ISOURCE_Readable(pISource, &pMe->cb);
			   break;
		   } 
	   }	   
   
}
static void  myiweb_ShowFileMenu(myiweb* pMe)
{
	CtlAddItem  ai;
	if (! pMe->m_pIFileMenu)
		ISHELL_CreateInstance(pMe->pIShell, AEECLSID_MENUCTL, (void **)&pMe->m_pIFileMenu) ;
	
	IMENUCTL_SetTitle(pMe->m_pIFileMenu,MYIWEB_RES_FILE,IDS_TITLE,NULL);

	ai.pText = NULL;
	ai.pImage = NULL;
	ai.pszResImage = MYIWEB_RES_FILE;
	ai.pszResText = MYIWEB_RES_FILE;
	ai.wFont = AEE_FONT_NORMAL;
	ai.dwData = NULL;
	
	ai.wText = IDS_MEDIA_PIC;   
	ai.wImage = IDB_MENU;
	ai.wItemID = IDS_MEDIA_PIC;
	
	
	// Add the item to the menu control
	if(IMENUCTL_AddItemEx(pMe->m_pIFileMenu, &ai )==FALSE)
	{
		IMENUCTL_Release(pMe->m_pIFileMenu);
		pMe->m_pIFileMenu = NULL;
		return;
	}
	
	ai.wText = IDS_MEDIA_MUSIC;
	ai.wItemID = IDS_MEDIA_MUSIC;
	
	// Add the item to the menu control
	IMENUCTL_AddItemEx( pMe->m_pIFileMenu, &ai );

	IMENUCTL_SetActive(pMe->m_pIFileMenu,TRUE);
	pMe->m_iAppStatus = 10;
}
static char * myiweb_GetFileName(const char * psz)
{
   char *   pszName = STRRCHR(psz, (int)DIRECTORY_CHAR);

   if (pszName)
      pszName++;
   else
      pszName = (char *)psz;

   return pszName;
}

static boolean myiweb_AddMenuItem(IMenuCtl * pMenu, uint16 wTextID, AECHAR * pText, uint16 wImageID, uint16 wItemID, uint32 dwData)
{
   CtlAddItem  ai;

   // Fill in the CtlAddItem structure values
   ai.pText = pText;
   ai.pImage = NULL;
   ai.pszResImage = MYIWEB_RES_FILE;
   ai.pszResText = MYIWEB_RES_FILE;
   ai.wText = wTextID;
   ai.wFont = AEE_FONT_NORMAL;
   ai.wImage = wImageID;
   ai.wItemID = wItemID;
   ai.dwData = dwData;

   // Add the item to the menu control
   return IMENUCTL_AddItemEx( pMenu, &ai );
}