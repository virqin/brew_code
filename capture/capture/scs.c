#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions  
#include "aeestdlib.h"


#include "AEEFile.h"
#include "AEEHtmlViewer.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "AEEText.h"
#include "AEEWeb.h"

#include "scs.h"
#include "inc/capture.h"

//Defines for the Scs
#define MAX_HIST                                9
#define FOR_ALL_WEBACTIONS(pApp, var, exp)     {WebAction *var; FORALL(var, (pApp)->m_awa) { exp; } }
#define WEBBER_USERAGENT   "NetDiagnostics/1.0 (built on "__DATE__")\r\n"
#define STREQ(a,b)         (!STRCMP(a,b))
#define FORALL(p,a)        for (p = (a) + ARRAY_SIZE(a) - 1; p >= (a); --p)
#define JUMP_ECHOTEST      "test:echo"                // starts echo test (expects form data)
#define JUMP_HTTPTEST      "test:http"                // starts HTTP test (expects form data)
#define JUMP_MAIN          "file:///ND_MainMenu.htm"
#define BOUNDARY		   "###echat_brew###"
#define SNAPSHOT_FILE_NAME "Snapshot.jpg"

#define  SCS_INI			"capture.ini"

#define	 SCS_UPLOAD_URL		"http://222.216.247.125:9000/scs/v1/api/upload"

typedef void (*PFNCLEANUP)(CScs *);


typedef struct WebAction
{
	CScs *				 pParent;       // my parent
	AEECallback          cb;            // how to call me back
	
	IWebResp *           piWResp;       // the answer I get from IWeb
	IGetLine *           piGetLine;     // the body of the response, if any
	int                  nLines;        // count of lines in response
	int                  nBytes;        // count of bytes in response
	int					 nResCode;		// response code
	
	uint32               uStart;
	uint32               uRecvStart;
	
	IPeek                *pipPostData;  // POST stream, if any
	char                 *pszPostData;  // POST string
	
	char				 m_pic_full_name[NORMAL_STR_LEN];		 //the picture full name with it's path
	char				 m_pic_short_name[NORMAL_STR_LEN];		 // the pic's name
	echat_camer_upload_info  m_upload_info;
} WebAction;

struct _scs
{
	// HTTP test state
	void *				 m_pApp;
	IShell*				 m_pIShell;				 // pointer to IShell
	IWeb *               m_pIWeb;                // Pointer to IWeb for HTTP Tests
	WebAction            m_awa[1];               // array of WebActions.  By changing the '1' to any 
	char                 m_pszURL[NORMAL_STR_LEN*3];               // "URL" field
	char*				 m_pic_path;			 //the picture full path
	PFNCLEANUP           m_pfnHttpCleanup;       // function to clean up after test (NULL => no test)
	IFileMgr *           m_pFileMgr;             // Pointer to the apps IFileMgr object
};

static boolean CScs_InitAppData(CScs * pApp);

static int    WebAction_Start(WebAction *pwa, char *pszUrl);
static void    WebAction_GotResp(void *p);
static void    WebAction_Header(void *p, const char *cpszName, GetLine *pglVal);
static void    WebAction_Status(void *p, WebStatus ws, void *pVal);
static void    WebAction_Stop(WebAction *pwa);
static void    WebAction_ReadLines(void *p);
static int	   WebAction_GetUploadURL(WebAction *pwa, char *pszUploadDesUrl);

static void ReleaseObj(void ** ppObj);
static void ND_Print(CScs *pApp, char *pszFmt, ...);
static void StrReplace(char **ppsz, const char *pszNew);
static void clean_string(char *str);
//////////////////////////////////////////////////////////////
// SCS
/////////////////////////////////////////////////////////////

static void CScs_HTTPCleanup(CScs* pApp)
{
	FOR_ALL_WEBACTIONS(pApp, p, WebAction_Stop(p));
}


CScs* CScs_CreateInstance(IShell* pIShell, void *pParentApp)
{
	CScs* pCtl = NULL;
	
	if (!pIShell)
	{
		return NULL;
	}
	
	pCtl = (CScs*) MALLOC( sizeof( CScs ) );
	DBGPRINTF("pCtl = %x",pCtl);
	if( pCtl )
	{
		pCtl->m_pIShell = pIShell;
		if(CScs_InitAppData(pCtl) != TRUE)
		{
			FREE(pCtl);
			return NULL;
		}
	}
	
	pCtl->m_pApp = pParentApp;
	
	return pCtl;
}

void CScs_Release(CScs* pApp)
{
	if (pApp->m_awa->pszPostData)
	{
		FREE(pApp->m_awa->pszPostData);
		pApp->m_awa->pszPostData = NULL;
	}
	
	ReleaseObj(&pApp->m_pFileMgr);
	ReleaseObj(&pApp->m_pIWeb);
}


static boolean CScs_InitAppData(CScs * pApp)
{
	int    i = 0;
	WebOpt awo[10]; 
	
	if( (ISHELL_CreateInstance(pApp->m_pIShell, AEECLSID_FILEMGR, (void**)(&pApp->m_pFileMgr)) != SUCCESS) ||
		(ISHELL_CreateInstance(pApp->m_pIShell, AEECLSID_WEB, (void **)(&pApp->m_pIWeb)) != SUCCESS) )
	{
		return FALSE;
	}
	
	// set the IWeb connect timeout to 10 seconds.  this also sets the 
	// failover timeout, if unset, or set to 0, IWeb uses the system 
	// default (30 seconds unless an OEM changes it)
	awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
	awo[i].pVal = (void *)10000;
	i++;
	
	
	// test nowaitconn, this only comes into effect if you build webber 
	// with multiple WebActions (see the definition of struct Webber)
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
	//awo[i].pVal = (void *)"*:///http://192.168.2.172:8888";
	i++;
	
	
	// Marks the end of the array of WebOpts
	awo[i].nId  = WEBOPT_END;
	
	// Add Options
	IWEB_AddOpt(pApp->m_pIWeb,awo);
	
	// Initialize all my WebActions to point to NetDiagnostics applet
	FOR_ALL_WEBACTIONS(pApp, p, p->pParent = pApp);
	
	MEMSET(&pApp->m_awa[0].m_upload_info, 0, sizeof(echat_camer_upload_info));
	
	STRCPY(pApp->m_awa[0].m_upload_info.userid, "1");
	STRCPY(pApp->m_awa[0].m_upload_info.lat, "0");
	STRCPY(pApp->m_awa[0].m_upload_info.lon, "0");
	pApp->m_awa[0].m_upload_info.coordinate = 0;
	return TRUE;
}


int CScs_UploadPic(CScs* pApp, const char* pic_full_path)
{
	AEERect rc;
	IStatic * pStatic = NULL;
	char *pszIter;
	char temp_pic_full_name[AEE_MAX_FILE_NAME];
	char *temp_name = NULL;
	
	//pic
	MEMSET(temp_pic_full_name,0,sizeof(temp_pic_full_name));
	MEMSET(pApp->m_awa[0].m_pic_full_name , 0 , sizeof(pApp->m_awa[0].m_pic_full_name));
	MEMSET(pApp->m_awa[0].m_pic_short_name , 0 , sizeof(pApp->m_awa[0].m_pic_short_name));
	
	MEMCPY(temp_pic_full_name,pic_full_path,STRLEN(pic_full_path));
	MEMCPY(pApp->m_awa[0].m_pic_full_name,pic_full_path,STRLEN(pic_full_path));
	
	if (STRRCHR(temp_pic_full_name,'/') == NULL) {
		temp_name = temp_pic_full_name;
	}else{
		temp_name = STRRCHR(temp_pic_full_name,'/') + 1;
	}
	
	if (temp_name == NULL)
		return ;
	
	MEMCPY(pApp->m_awa[0].m_pic_short_name,temp_name,STRLEN(temp_name));
	
	pApp->m_pfnHttpCleanup = CScs_HTTPCleanup;
	
	
	if (STRLEN(pApp->m_pszURL) == 0)
	{
		int urlRes = -1;
		urlRes = WebAction_GetUploadURL(pApp->m_awa , pApp->m_pszURL);
		if (urlRes == -1)
		{
			return -3;
		}
	}
	
	return WebAction_Start(pApp->m_awa , pApp->m_pszURL);
}

void CScs_CancelUpload(CScs* pApp)
{
	DBGPRINTF("CScs_CancelUpload");
	WebAction_Stop(pApp->m_awa);
}


// Free the old pointer and fill with STRDUP(pszNew)
//
static void StrReplace(char **ppsz, const char *pszNew)
{
	FREE(*ppsz);   // FREE(NULL) is okay in BREW
	*ppsz = (pszNew ? STRDUP(pszNew) : (char*)pszNew);
}


//////////////////////////////////////////////////////////////
// IWeb
/////////////////////////////////////////////////////////////

/*===========================================================================
===========================================================================*/
static int WebAction_Start(WebAction *pwa, char *pszUrl)
{
	ISourceUtil *pisu;
	IFile *picFile;
	FileInfo picFileInfo;
	WebOpt awo[20]; 
	CScs * pApp = pwa->pParent;
	//CameraApp* pCamera =pApp->m_pApp; 
	int iCnt = 0;
	
	int ibodyLen = 0;
	int ibodyHeadLen = 0;
	int ibodyDataLen = 0;
	int ibodyEndLen = 0;
	char bodyHeadStr[1024];
	//char* bodyData = NULL;
	char bodyEnd[128];
	
	MEMSET(bodyHeadStr,0,1024);
	MEMSET(bodyEnd,0,128);
	
	//上传文件校验
	if (NULL == pwa->m_pic_full_name)
		return -1;

	if (SUCCESS != IFILEMGR_Test(pApp->m_pFileMgr, pwa->m_pic_full_name))
		return -1;

	if( STRCMP(pwa->m_upload_info.userid,"0")==0 || STRCMP(pwa->m_upload_info.userid,"1")==0)
	{
		//CameraApp_ShowUploadRes(pApp->m_pApp, FALSE);
		//CameraApp_ShowText(pApp->m_pApp, IDS_STRING_UID_NOT_GET, pCamera->piwMText, pCamera->rcContainer.dx / 3 - 20 , pCamera->rcContainer.dy - 39 + 10);
		return -2;
	}

	// cat the head string
	SPRINTF(bodyHeadStr , "--%s\r\n%s\r\n\r\n%s\r\n--%s\r\n%s\r\n\r\n%s%s%s%s%s%s%s%d\r\n--%s\r\n%s\r\n\r\n%s\r\n--%s\r\n%s\r\n\r\n%s\r\n--%s\r\n%s\r\n\r\n%s\r\n--%s\r\n%s%s%s\r\n%s\r\n\r\n",
		BOUNDARY,"Content-Disposition: form-data; name=\"file_name\"",pwa->m_pic_short_name,
		BOUNDARY,"Content-Disposition: form-data; name=\"context\"","filetype:0,userid:",pwa->m_upload_info.userid,
		",lon:",pwa->m_upload_info.lon,",lat:",pwa->m_upload_info.lat,",coordinate:",pwa->m_upload_info.coordinate,
		BOUNDARY,"Content-Disposition: form-data; name=\"terminal\"","G180",
		BOUNDARY,"Content-Disposition: form-data; name=\"appname\"","echat",
		BOUNDARY,"Content-Disposition: form-data; name=\"token\"","c2hhbmxpdGVjaC1lY2hhdC1neGx6",
		BOUNDARY,"Content-Disposition: form-data; name=\"file\"; filename=\"",pwa->m_pic_short_name,"\"","Content-Type: image/jpeg"
		);
	
	DBGPRINTF("Upload Info: userid:%s lon:%s lat:%s coordinate:%d",
		pwa->m_upload_info.userid,  pwa->m_upload_info.lon, pwa->m_upload_info.lat, pwa->m_upload_info.coordinate);
	
	SPRINTF( bodyEnd , "\r\n--%s--\r\n",BOUNDARY);
	
	ibodyHeadLen = STRLEN(bodyHeadStr) ;
	ibodyEndLen = STRLEN(bodyEnd);
	ibodyLen = ibodyHeadLen + ibodyEndLen;
	
	//获得文件大小信息
	IFILEMGR_GetInfo(pApp->m_pFileMgr, pwa->m_pic_full_name, &picFileInfo);
	ibodyDataLen = picFileInfo.dwSize;
	ibodyLen += picFileInfo.dwSize;

	// if there's post data, construct a stream for IWeb to consume
// 	if ((char *)0 != pwa->m_pic_full_name) {
// 		picFile = IFILEMGR_OpenFile(pApp->m_pFileMgr, pwa->m_pic_full_name , _OFM_READ);
// 		DBGPRINTF("picFile = %x",picFile);
// 		IFILE_GetInfo(picFile,&picFileInfo);
// 		
// 		bodyData = MALLOC((int)picFileInfo.dwSize * (sizeof(char) + 1));
// 		DBGPRINTF("bodyData = %x",bodyData);
// 		MEMSET(bodyData,0,(int)picFileInfo.dwSize * (sizeof(char) + 1));
// 		
// 		IFILE_Read(picFile , bodyData, picFileInfo.dwSize);
// 		ibodyDataLen = picFileInfo.dwSize;
// 		ibodyLen += picFileInfo.dwSize;
// 		
// 		IFILE_Release(picFile);
// 	}
// 	else{
// 		return -1;
// 	}
	
	if (SUCCESS ==  ISHELL_CreateInstance(pApp->m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) {
		if (pwa->pszPostData)
		{
			FREE(pwa->pszPostData);
		}

		pwa->pszPostData =  MALLOC( ibodyLen * (sizeof(char) + 1));
		DBGPRINTF("pwa->pszPostData = %x",pwa->pszPostData);
		if (NULL == pwa->pszPostData)
		{
			DBGPRINTF("Malloc pwa->pszPostData Failed!");
			return -1;
		}
		
		//Header
		MEMCPY( pwa->pszPostData,bodyHeadStr,ibodyHeadLen );

		//Body
		//MEMCPY( pwa->pszPostData+ibodyHeadLen, bodyData , ibodyDataLen);
		picFile = IFILEMGR_OpenFile(pApp->m_pFileMgr, pwa->m_pic_full_name, _OFM_READ);
		if (picFile != NULL)
		{
			IFILE_Read(picFile , pwa->pszPostData+ibodyHeadLen, ibodyDataLen);
		}
		else
		{
			DBGPRINTF("IFILEMGR_OpenFile %s Failed!", pwa->m_pic_full_name);
			return -1;
		}

		//End
		MEMCPY( pwa->pszPostData+ibodyHeadLen+ibodyDataLen ,bodyEnd ,ibodyEndLen );
		
		ISOURCEUTIL_PeekFromMemory(pisu, pwa->pszPostData ,  ibodyLen , 0, 0,  &pwa->pipPostData);
		ISOURCEUTIL_Release(pisu);
	}
	else{
		return -4;
	}
	
	//FREE(bodyData);
	
	awo[iCnt].nId = WEBOPT_HANDLERDATA;
	awo[iCnt].pVal = pwa;
	iCnt++;
	
	awo[iCnt].nId = WEBOPT_METHOD;
	awo[iCnt].pVal = "POST";
	iCnt++;
	
	awo[iCnt].nId = WEBOPT_HEADER;
	awo[iCnt].pVal = "Content-Type: multipart/form-data; boundary=###echat_brew###";
	iCnt++;
	
	awo[iCnt].nId = WEBOPT_CONTENTLENGTH;
	awo[iCnt].pVal = ibodyLen;
	iCnt++;
	
	awo[iCnt].nId = WEBOPT_BODY;
	awo[iCnt].pVal = pwa->pipPostData;
	iCnt++;
	
	awo[iCnt].nId = WEBOPT_HEADERHANDLER;
	awo[iCnt].pVal = WebAction_Header;
	iCnt++;
	
	awo[iCnt].nId = WEBOPT_STATUSHANDLER;
	awo[iCnt].pVal = WebAction_Status;
	iCnt++;
	
	awo[iCnt].nId = WEBOPT_END;
	
	IWEB_AddOpt(pApp->m_pIWeb,awo);
	
	
	// initialize the callback, where I'll be called when the request
	// completes
	CALLBACK_Init(&pwa->cb, WebAction_GotResp, pwa);
	
	pwa->uStart = GETUPTIMEMS();
	
	// start transaction, pass callbacks for web status, web headers
	// the extra WEBOPT_HEADER is used to help test what's sent 
	// (snoop.sh above shows all the headers)
	if ((IPeek *)0 != pwa->pipPostData) {
		IWEB_GetResponseV( pApp->m_pIWeb,&pwa->piWResp, &pwa->cb, pszUrl,awo);
	} else {
		IWEB_GetResponse(pApp->m_pIWeb,
			(pApp->m_pIWeb, &pwa->piWResp, &pwa->cb, pszUrl, 
			WEBOPT_HANDLERDATA, pwa, 
			WEBOPT_HEADER, "X-Method: GET\r\n",
			WEBOPT_HEADERHANDLER, WebAction_Header, 
			WEBOPT_STATUSHANDLER, WebAction_Status, 
			WEBOPT_END));
	}

	return 0;
}

/*===========================================================================

  ===========================================================================*/
  static void WebAction_GotResp(void *p)
  {
	  WebAction *pwa = (WebAction *)p;
	  CScs * pApp = pwa->pParent; 
	  WebRespInfo *pwri;
	  int upload_result = -1;
	  
	  // get information about how the web transaction went
	  // pwa->piWResp is ***NEVER NULL***, even though the transaction may fail
	  // for wont of memory
	  pwri = IWEBRESP_GetInfo(pwa->piWResp);
	  
	  ND_Print(pApp, "** got response...\n** info code: %d\n", pwri->nCode);
	  pwa->nResCode = pwri->nCode;
	  
	  // body may be NULL
	  if ((ISource *)0 != pwri->pisMessage) {
		  ISourceUtil *pisu;
		  
		  // read the body by creating an IGetLine to help parse into lines
		  
		  // This step is not actually necessary if you don't need line-by-line
		  // output.  In that case, use pisMessage directly.
		  ISHELL_CreateInstance(pApp->m_pIShell,AEECLSID_SOURCEUTIL,
			  (void **)&pisu);
		  
		  if ((ISourceUtil *)0 != pisu) {
			  ISOURCEUTIL_GetLineFromSource(pisu, pwri->pisMessage,1100, 
				  &pwa->piGetLine);
			  ISOURCEUTIL_Release(pisu);
		  }
	  }
	  
	  if ((IGetLine *)0 != pwa->piGetLine) {
		  
		  pwa->uRecvStart = GETUPTIMEMS(); // record that I started reading
		  
		  // got an IGetLine, go ahead and read it
		  pwa->nBytes = 0;
		  WebAction_ReadLines(pwa);
		  
	  } else {
		  // ug, tranaction failed or couldn't get an IGetLine, oh well, bail
		  ND_Print(pApp, "** no response\n");
		  WebAction_Stop(pwa); // stop cleans up

		  //当断线时提示上传失败, 并返回
		  CameraApp_ShowUploadRes(pApp->m_pApp, FALSE);
	  }
	  
	  if (pwa->pszPostData)
	  {
		  FREE(pwa->pszPostData);
		  pwa->pszPostData = NULL;
	  }   
  }
  
  
  /*===========================================================================
  ===========================================================================*/
  static void WebAction_Header(void *p, const char *cpszName, GetLine *pglVal)
  {
	  WebAction *pwa = (WebAction *)p;
	  CScs * pApp = pwa->pParent;
  }
  
  /*===========================================================================
  
  ===========================================================================*/
  static void WebAction_Status(void *p, WebStatus ws, void *pVal)
  {
	  char *      pszStatus = 0;
	  WebAction * pwa = (WebAction *)p;
	  
	  (void)pVal;
	  
	  switch (ws) {
	  case WEBS_CANCELLED:
		  pszStatus = "** cancelled...\n";
		  break;
	  case WEBS_GETHOSTBYNAME:
		  pszStatus = "** finding host...\n";
		  break;          
	  case WEBS_CONNECT:
		  pszStatus = "** connecting...\n";
		  break;
	  case WEBS_SENDREQUEST:
		  pszStatus = "** sending...\n";
		  break;
	  case WEBS_READRESPONSE:
		  pszStatus = "** receiving...\n";
		  break;
	  case WEBS_GOTREDIRECT:
		  pszStatus = "** redirect...\n";
		  break;
	  case WEBS_CACHEHIT:
		  pszStatus = "** cache hit...\n";
		  break;
	  }
	  
	  // show that status!
	  if ((char *)0 != pszStatus) {
		  ND_Print(pwa->pParent, "%s", pszStatus);
	  }
  }
  
  /*===========================================================================
  ===========================================================================*/
  static void WebAction_Stop(WebAction *pwa)
  {
	  // this cancels any pending web transaction, or readable on the 
	  // response body.  if nothing is pending, this has no effect
	  CALLBACK_Cancel(&pwa->cb);
	  
	  // then clean up, if necessary
	  if ((char *)0 != pwa->pszPostData) {
		  //*pwa->pszPostData = 1; // write delimiter back in, if any
		  //pwa->pszPostData = 0;
		  FREE(pwa->pszPostData);
		  pwa->pszPostData = NULL;
	  }
	  
	  ReleaseObj((void **)&pwa->pipPostData);
	  ReleaseObj((void **)&pwa->piGetLine);  // let body go
	  ReleaseObj((void **)&pwa->piWResp);    // let response go
  }
  
  
  /*===========================================================================
  ===========================================================================*/
  static void WebAction_ReadLines(void *p)
  {
	  int        rv;
	  GetLine    gl;
	  WebAction *pwa = (WebAction *)p;
	  CScs * pApp = pwa->pParent;
	  
	  WebRespInfo *pwri;
	  char code[10] = {0};
	  char *temp;
	  char *tempEnd;
	  
	  // read a line
	  rv = IGETLINE_GetLine(pwa->piGetLine, &gl, IGETLINE_LF);
	  if (rv == IGETLINE_WAIT) {
		  CALLBACK_Init(&pwa->cb, WebAction_ReadLines, pwa);
		  IGETLINE_Peekable(pwa->piGetLine, &pwa->cb);
		  return;
	  }
	  
	  // process line
	  
	  pwa->nBytes += gl.nLen + IGETLINE_EOLSize(rv);
	  
	  ND_Print(pApp, "%s\n", gl.psz, NULL, TRUE);
	  DBGPRINTF(gl.psz);
	  
	  temp = STRSTR(gl.psz, "code");
	  
	  
	  if (temp)
	  {
		  temp += STRLEN("code:") + 1;
	  }
	  
	  if (temp)
	  {
		  tempEnd = STRCHR(temp, ',');
	  }
	  
	  if (tempEnd)
	  {
		  MEMCPY(code, temp, tempEnd-temp);
	  }
	  
	  // gl.psz = {"response":{"result":{"code":200,"uuid":"b34b9d351e4846dbb1d9ac7b99b67855"}}}
	  
	  // if stream has not reached EOF or ERROR
	  if (!IGETLINE_Exhausted(rv)) {
		  // wait for more data
		  CALLBACK_Init(&pwa->cb, WebAction_ReadLines, pwa);
		  IGETLINE_Readable(pwa->piGetLine, &pwa->cb);
		  return;
	  }
	  
	  // no more data
	  
	  if (rv == IGETLINE_ERROR) {
		  ND_Print(pApp, "\n** error\n");
	  } else {
		  ND_Print(pApp, "\n** end\n");
	  }
	  
	  {
		  uint32 uNow = GETUPTIMEMS();
		  ND_Print(pApp,
			  "** %d bytes\n"
			  "** %dB/s\n"
			  "** %dms et (recv)\n"
			  "** %dms et (total)\n",
			  pwa->nBytes, 
			  pwa->nBytes * 1000 / MAX(1,uNow - pwa->uRecvStart),
			  uNow - pwa->uRecvStart,
			  uNow - pwa->uStart);
	  }
	  
	  // am done, _Stop cleans up
	  WebAction_Stop(pwa);
	  
	  
	  if (STRCMP(code, "200") == 0)
	  {
		  //通知上层显示上传结果
		  if (pwa->nResCode == 200)
		  {
			  CameraApp_ShowUploadRes(pApp->m_pApp, TRUE);
		  }
		  else
		  {
			  CameraApp_ShowUploadRes(pApp->m_pApp, FALSE);
		  }
	  }
	  else
	  {
		  CameraApp_ShowUploadRes(pApp->m_pApp, FALSE);
	  }
  }
  
  
  static void ReleaseObj(void ** ppObj)
  {
	  if (*ppObj) {
		  (void)IBASE_Release(((IBase *)*ppObj));
		  *ppObj = NULL;
	  }
  }
  
  static void ND_Print(CScs *pApp, char *pszFmt, ...)
  {
	  char buf[1200];
	  va_list argptr;
	  va_start(argptr, pszFmt);
	  
	  if (VSNPRINTF(buf, sizeof(buf), pszFmt, argptr) >= 0) {
		  //DBGPRINTF(buf);
	  }
	  va_end(argptr);
  }
  
  void Cscs_SetUploadInfo( CScs* pApp , echat_camer_upload_info* pInfo){
	  MEMCPY(&pApp->m_awa[0].m_upload_info, pInfo, sizeof(echat_camer_upload_info));
  }
  

  static int WebAction_GetUploadURL(WebAction *pwa, char *pszUploadDesUrl)
  {
	  char* iniData = NULL;
	  IFile *picFile;
	  FileInfo picFileInfo;
	  CScs * pApp = pwa->pParent;
	  boolean bDefault = TRUE;
	  int result = -1;

	  //当配置文件不存在或者没有URL项时, 使用使用默认的URL, 如果存在则使用配置文件中的
	  picFile = IFILEMGR_OpenFile(pApp->m_pFileMgr, SCS_INI, _OFM_READ);
	  if (picFile != NULL)
	  {
		  DBGPRINTF("picFile = %x",picFile);
		  IFILE_GetInfo(picFile,&picFileInfo);

		  iniData = MALLOC((int)picFileInfo.dwSize * (sizeof(char) + 1));
		  MEMSET(iniData,0,(int)picFileInfo.dwSize * (sizeof(char) + 1));

		  IFILE_Read(picFile , iniData, picFileInfo.dwSize);
		  IFILE_Release(picFile);
		  picFile = NULL;

		  if (STRRCHR(iniData,'=') != NULL) {
			  char* temp = STRRCHR(iniData,'=') + 1;
			  clean_string(temp);

			  if (STRLEN(temp) > 0)
			  {
				  STRCPY(pszUploadDesUrl,temp);
				  bDefault = FALSE;
				  result = 0;
			  }
		  }

		  FREE(iniData);
		  iniData = NULL;
	  }

	  if (bDefault)
	  {
		  //char szIniBuff[512];

		  //DBGPRINTF("User Default Upload URL:%s", pszUploadDesUrl);
		  //STRCPY(pszUploadDesUrl, SCS_UPLOAD_URL);

		  //picFile = IFILEMGR_OpenFile(pApp->m_pFileMgr, SCS_INI, _OFM_CREATE);
		  //if (picFile != NULL)
		  //{
			 // SNPRINTF(szIniBuff, sizeof(szIniBuff), "url=%s", pszUploadDesUrl);
			 // IFILE_Write(picFile , szIniBuff, STRLEN(szIniBuff));
			 // IFILE_Release(picFile);
			 // picFile = NULL;
		  //}
		  //pszUploadDesUrl = NULL;
		  result = -1;
	  }

	  DBGPRINTF("scs upload destination is : %s!  size is %d",pszUploadDesUrl,STRLEN(pszUploadDesUrl) );
	  return result;
  }


  /*
  去掉字符串首尾的 \x20 \r \n 字符
  by sincoder
  */
  static void clean_string(char *str)
  {
	  char *start = str - 1;
	  char *end = str;
	  char *p = str;
	  while(*p)
	  {
		  switch(*p)
		  {
		  case ' ':
		  case '\r':
		  case '\n':
			  {
				  if(start + 1==p)
					  start = p;
			  }
			  break;
		  default:
			  break;
		  }
		  ++p;
	  }
	  //现在来到了字符串的尾部 反向向前
	  --p;
	  ++start;
	  if(*start == 0)
	  {
		  //已经到字符串的末尾了 
		  *str = 0 ;
		  return;
	  }
	  end = p + 1;
	  while(p > start)
	  {
		  switch(*p)
		  {
		  case ' ':
		  case '\r':
		  case '\n':
			  {
				  if(end - 1 == p)
					  end = p;
			  }
			  break;
		  default:
			  break;
		  }
		  --p;
	  }
	  MEMMOVE(str,start,end-start);
	  *(str + (int)end - (int)start) = 0;
  }
