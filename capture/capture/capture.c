
#include "AEE.h"              // Standard AEE Declarations
#include "AEEShell.h"         // AEE Shell Services
#include "AEEFont.h"          // AEE Font Services
#include "AEEDisp.h"          // AEE Display Services
#include "AEEStdLib.h"        // AEE StdLib Services
#include "AEEWidget.h"
#include "AEERootContainer.h"
#include "AEEDisplayCanvas.h"
#include "AEEFrameWidget.h"
#include "AEECameraFrameModel.h"
#include "AEEImageWidget.h"
#include "AEEBitmapWidget.h"
#include "AEEImageWidget.h"
#include "AEEListWidget.h"
#include "AEEStaticWidget.h"
#include "AEEMimeTypes.h"
#include "AEEVectorModel.h"
#include "AEEProgressWidget.h"
#include "AEEStaticWidget.h"
#include "AEEFile.h"
#include "inc/capture.bid"
#include "inc/capture_res.h"
#include "AEEBitmap.h"

#include "inc/capture.h"
#include "scs.h"

//SLPTT CLSD
#define AEECLSID_SLPTT 0x01009FF0

#define G500_AVK_PTT 0xE082

//------------------------------------------------------------------------
//    Types and Macros
//------------------------------------------------------------------------
#define EVT_CAPTURE_EVENT		(EVT_USER + 500)

#define MAX_UPLOAD_FILE_SIZE	1024*1024		//文件大小最大1M
#define MIN_STORAGE_SIZE		1024*1024*5		//存储空间最小5M

#define RESFILE					"capture.bar"
#define AEEFS_FILEMGR_DIR		"fs:/filemgr/"

#define TITLE_HEIGHT			16	//标题高
#define BUTTON_WIDTH			39	//菜单条高度
#define ALERT_WIDTH				128	//提示图标位置和宽
#define ALERT_OFFSET			56	//提示图标坐标偏移

#ifdef AEE_SIMULATOR
#define DEF_PIC_WIDTH			240
#define DEF_PIC_HEIGHT			320
#else
#define DEF_PIC_WIDTH			600
#define DEF_PIC_HEIGHT			800
#endif

//////////////////////////////////////////////////////////////////////////
#define IDX_PICTURE_QUALITY		0
#define IDX_PICTURE_SIZE		1
#define IDX_CAPTURE				2
#define IDX_STORAGE			    3
#define IDX_FOLDER				4

#define NUM_BUTTONS				5
//////////////////////////////////////////////////////////////////////////

#define MODE_PREVIEWING			0
#define MODE_SELECTING			1
#define MODE_VIEWING			2
#define MODE_CAPTURING			3
#define MODE_PICTURE_QUALITY	4
#define MODE_PICTURE_SIZE		5
#define MODE_STORAGE			6

//文件浏览状态
typedef enum
{
	FOLDER_STATE_ROOT,
	FOLDER_STATE_LIST,
	FOLDER_STATE_VIEWING,
	FOLDER_STATE_UPLOADING,
	FOLDER_STATE_UPLOAD_RES
} EFolderStateType;


//存储类型
typedef enum _EStorageType
{
	STOTAGE_PONE,
	STOTAGE_SDCARD
} EStorageType;



//提示类型
typedef enum _EAlertType
{
	ALERT_LOW_MEM,
	ALERT_BIG_FILE
} EAlertType;

/**
 * @brief
 */
typedef struct hal_walltime_t {
	uint16			year;
	uint8			month;
	uint8			day;
	uint8			hour;
	uint8			minute;
	uint8			second;
	uint16			millisecond;
} hal_walltime_t;



#define ALERT_TIMER			2200 // Time Close The Alert Dialog

#define UPLOAD_TIMER		1200 // Time Close The Alert Dialog

#define CAMERA_TIMER		300 // Time between Camera Stop and Capture

#ifndef REMOVEWIDGET
#define REMOVEWIDGET(r, w) if (w) IROOTCONTAINER_Remove(r, w); 
#endif

#ifndef RELEASEIF
#define RELEASEIF(p)       ReleaseIf((IBase**)&p)
static void ReleaseIf(IBase **ppif) {
	if (*ppif) {
		IBASE_Release(*ppif); 
		*ppif = 0;
	}
}
#endif

#ifndef ADDREFIF
#define ADDREFIF(p)        do { if (p) IBASE_AddRef((IBase*) (void *) p); } while(0)
#endif

#define IWIDGET_SetExtentEx(p,w,h)  \
do { WExtent we; we.width = (w); we.height=(h); IWIDGET_SetExtent((p),&we); } while(0)


static __inline int IROOTCONTAINER_InsertEx(IRootContainer *me, IWidget *piw, int x, int y) {
	WidgetPos wp;
	wp.x = x;
	wp.y = y;
	wp.bVisible = 1;
	return IROOTCONTAINER_Insert(me, piw, WIDGET_ZNORMAL, &wp);
}


static uint32  CameraApp_AddRef   (IApplet *po);
static uint32  CameraApp_Release  (IApplet *po);
static int     CameraApp_Start    (CameraApp *me);
static void    CameraApp_Notify   (void * pUser, AEECameraNotify * pNotify);
static void    CameraApp_Snapshot (void * pUser);

static void	   CameraApp_ShowButtons (void * pUser, const boolean bShow);
static void	   CameraApp_ShowSnap (void * pUser, const char* imagePath);
static void	   CameraApp_ShowPreview (void * pUser);
static void    CameraApp_Capture (void * pUser);
static void	   CameraApp_ShowFolder (void * pUser);
static void	   CameraApp_ShowFolderList (void * pUser);
static void	   CameraApp_ShowStorage (void * pUser);
static void	   CameraApp_ShowPicQuality (void * pUser);
static void	   CameraApp_ShowPicSize (void * pUser);
static void	   CameraApp_RemoveAll( void * pUser);
static void	   CameraApp_ShowUpload (void * pUser);
void		   CameraApp_ShowUploadRes( void * pUser, const boolean bRes);

void		   CameraApp_DrawNotify(void *pUser, IImage *pImage, AEEImageInfo *pi, int nErr);	//显示图片通知消息
void		   CameraApp_ShowText(void * pUser, int16 resid, IWidget * piw, int x, int y);		//显示Text, 可复用
static void	   CameraApp_UpdateButton(void * pUser, int16 nButtonIdx, boolean bSel);			//更新BUTTON图标状态

static void	   CameraApp_ShowAlert(void * pUser, EAlertType type);									//显示提示

static uint16  findPicType( const char* imagePath );
static void    CameraApp_ShowUploadFailInfo (void * pUser, int errNo);
void		   CameraApp_ShowTextRaw(void * pUser, const char*locInfo, IWidget * piw, int x, int y);

//------------------------------------------------------------------------
//    CameraApp struct
//------------------------------------------------------------------------

typedef struct _CameraApp{
	
	IApplet              ia;
	IAppletVtbl          iavt;  
	
	
	IShell *             piShell;
	IModule *            piModule;
	IFileMgr *           piFileMgr;
	IVectorModel *       piListModel;
	IVectorModel *       piListModelPicSize;
	IVectorModel *       piListModelPicQuality;
	IValueModel *		 piValueModel;
	ICameraFrameModel *	 piModel;

	ICamera *            piCamera;
	CScs *				 scs;
	AEERect              rcContainer;
	IRootContainer *     pwcRoot;
	IWidget *            piwRoot;
	IWidget *            piwFrame;					//Camera预览显示widget
	IWidget *            piwList;					//图片列表
	IWidget *            piwListPicSize;			//图片大小列表
	IWidget *            piwListPicQuality;			//图片质量列表

	IWidget *            piwImage;					//显示图片
	IWidget *            piwFtImage;				//底部图片
	IWidget *            piwUpImage;				//上传图片
	IWidget *            piwButtons[NUM_BUTTONS];	//命令按钮
	IWidget *			 piwProgress;				//上传进度条
	IWidget *            piwTitle;					//标题
	IWidget *            piwLText;					//左侧显示文本
	IWidget *            piwMText;					//中间显示文本
	IWidget *            piwRText;					//右侧键显示文本
	IWidget *            piwExText;					//提示文本


	int                  nRefs;
	int16                nActiveButton;				//当前激活按钮
	int16				 btnResId[NUM_BUTTONS][2];	//按钮状态图标ID

	int                  nMode;						//运行模式
	int                  nPreMode;					//上次运行模式
	int					 nRootMode;					//根模式

	char				 szSnapDir[AEE_MAX_FILE_NAME];	//抓拍图片目录
	char                 szImage[AEE_MAX_FILE_NAME];	//抓拍图片完整路径

	int					 nQuality;					//图片质量
	AEESize				 picSize;					//图片大小
	EStorageType		 eStorageType;				//存储类型(手机或SD卡)
	EFolderStateType	 eFolderStateType;			//文件夹浏览状态

	int					 nPreIndex;					//上次选择的索引
	int					 nRetryCount;				//打开图片尝试次数
	boolean				 bUploading;				//正在上传
	boolean				 bUploadRes;				//正在上传
	boolean				 bSnaping;					//正在拍照
	boolean				 bStartSuccess;			    //启动照相机是否成功

	IWidget *            piwErrorImage ;			//启动失败时显示启动错误图片
	IWidget *            piwErrorText;				//启动失败时提示启动错误提示

}CameraApp;


int hal_walltime_now(hal_walltime_t* tw) {
	uint32 secs;
	JulianType julian;
	
	if( NULL == tw )
		return -1;
	
	secs = GETTIMESECONDS();
	GETJULIANDATE(secs,&julian);
	
	tw->year = julian.wYear;
	tw->month = (uint8)julian.wMonth;
	tw->day = (uint8)julian.wDay;
	tw->hour = (uint8)julian.wHour;
	tw->minute = (uint8)julian.wMinute;
	tw->second = (uint8)julian.wSecond;
	tw->millisecond = 0;
	return 0;
}

void ShowTitle(void * pUser, const char* szTitle, int x, int y)
{
	CameraApp *me = (CameraApp *)pUser;
	int nErr = 0;
	int nSize = 0;
	AECHAR *wstr = NULL;
	
	nSize = STRLEN(szTitle)*sizeof(AECHAR)+1;
	wstr = (AECHAR*)MALLOC(nSize);
	STRTOWSTR(szTitle, wstr, nSize);

	{
		char szBuf[20];
		WSTRTOSTR(wstr, szBuf, nSize/2);
		DBGPRINTF(szBuf);
	}
	
	IWIDGET_SetText(me->piwTitle, wstr, FALSE);
	
	IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwTitle, me->rcContainer.dx/3, TITLE_HEIGHT);
	
	//FREE(wstr);
	return ;

}

void CameraApp_ShowText(void * pUser, int16 resid, IWidget * piw, int x, int y)
{
	CameraApp *me = (CameraApp *)pUser;
	int nErr = 0;
	int nSize = 20, size = 0;
	char szBuf[10] = {""};
	
	AECHAR *wstr = (AECHAR*)MALLOC(nSize);
	
	nErr = ISHELL_LoadResString(me->piShell, RESFILE, resid, wstr, nSize);
	size = (WSTRLEN(wstr))*sizeof(char);
	WSTRTOSTR(wstr, szBuf, size);
	
	DBGPRINTF(szBuf);
	
	IWIDGET_SetText(piw, wstr, FALSE);
	
	IROOTCONTAINER_InsertEx(me->pwcRoot, piw, x, y);
	
	//FREE(wstr);
	return ;
}


void CameraApp_ShowTextRaw(void * pUser, const char*locInfo, IWidget * piw, int x, int y)
{
	CameraApp *me = (CameraApp *)pUser;
	int nErr = 0;
	int size = 0;
	char szBuf[20] = {""};

	AECHAR *wstr = NULL;

	size = STRLEN(locInfo)*sizeof(AECHAR)+2;
	wstr = (AECHAR*)MALLOC(size);
	
	STRTOWSTR(locInfo, wstr, size);

	size = (WSTRLEN(wstr))*sizeof(char);
	WSTRTOSTR(wstr, szBuf, size);
	
	DBGPRINTF(szBuf);
	
	IWIDGET_SetText(piw, wstr, FALSE);
	
	IROOTCONTAINER_InsertEx(me->pwcRoot, piw, x, y);
	
	//FREE(wstr);
	return ;
}


void ShowSubFiles(void * pUser, const char* dir, IVectorModel *piListModel)
{
	CameraApp *me = (CameraApp *)pUser;
	FileInfo fi;
	AECHAR *wstr = 0;
	int nSize = 0;

	if (dir == NULL || piListModel == NULL)
		return;

	if (SUCCESS == IFILEMGR_Test(me->piFileMgr, AEEFS_CARD0_DIR))
	{
		DBGPRINTF("%s exists", AEEFS_CARD0_DIR);
	}

	{
	FileInfo fi;
	IFILEMGR_EnumInit(me->piFileMgr, AEEFS_CARD0_DIR, TRUE);
	while (IFILEMGR_EnumNext(me->piFileMgr, &fi)) {
		DBGPRINTF(fi.szName);
	} 
	}

	IFILEMGR_EnumInit(me->piFileMgr, dir, FALSE);
	while (IFILEMGR_EnumNext(me->piFileMgr, &fi)) 
	{
		char *fileName = STRRCHR(fi.szName, '/') + 1;

		//检查是否为支持的图像格式
		if (findPicType(fileName) != 0)
		{
			DBGPRINTF("Dir:%s %s", fi.szName, fileName);
			nSize = (STRLEN(fileName) + 1) * sizeof(AECHAR);
			wstr = (AECHAR*)MALLOC(nSize);
			IVECTORMODEL_Add(me->piListModel, STRTOWSTR(fileName, wstr, nSize));  
		}
	}

	IWIDGET_SetBGImage(me->piwList,NULL);
}

//--------------------------------------------------------------------
//   CameraApp Event Handler
//--------------------------------------------------------------------

static boolean CameraApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	int nErr = 0;
	CameraApp *me = (CameraApp *)pi;
	
	switch (eCode) {
		
	case EVT_APP_START:
		CameraApp_Start(me);
		return TRUE;
		
	case EVT_APP_STOP:
		DBGPRINTF("EVT_APP_STOP!");
		CameraApp_Release(me);

		return TRUE;   
		
	case EVT_APP_SUSPEND:
		CameraApp_RemoveAll(me);
		return TRUE;
		
	case EVT_APP_RESUME:
		CameraApp_ShowPreview(me);
		return TRUE;
	
	case EVT_CAPTURE_EVENT:
		{
			echat_camer_upload_info camera_upload_info;

			if (wParam == sizeof(echat_camer_upload_info))
			{
				MEMCPY(&camera_upload_info, (echat_camer_upload_info*)(dwParam), sizeof(echat_camer_upload_info));

				if (STRLEN(camera_upload_info.userid) == 0 || camera_upload_info.userid[0] == '0')
				{
					STRCPY(camera_upload_info.userid, "1");
				}
				if (STRLEN(camera_upload_info.lon) == 0)
				{
					STRCPY(camera_upload_info.lon, "0");
				}
				if (STRLEN(camera_upload_info.lat) == 0)
				{
					STRCPY(camera_upload_info.lat, "0");
				}
				if (camera_upload_info.coordinate != 0 || camera_upload_info.coordinate != 1)
				{
					camera_upload_info.coordinate = 0;
				}

				DBGPRINTF("Recieve Upload Info: userid:%s lon:%s lat:%s coordinate:%d",camera_upload_info.userid,  camera_upload_info.lon, camera_upload_info.lat, camera_upload_info.coordinate);
				
				Cscs_SetUploadInfo(me->scs , &camera_upload_info);


#if 0
				//显示经纬度[测试使用]
				{
					int size = 0;
					char locInfo[40];
					SPRINTF(locInfo, "userid:%s lon:%s lat:%s", camera_upload_info.userid, camera_upload_info.lon, camera_upload_info.lat);
		
					CameraApp_ShowTextRaw(me, locInfo, me->piwExText, 10, 10);
					
				}
#endif
				
			}
		}
		break;

	case EVT_KEY:
		
		DBGPRINTF("EVT_KEY: %x", wParam);

		//按PTT键和关机键都退出拍照程序
		if (G500_AVK_PTT == wParam || AVK_END == wParam)
		{
			ISHELL_CloseApplet((IShell*)me->piShell, FALSE);
		}

		//add by yao 处理上传
		if (me->nMode == MODE_VIEWING) 
		{
			if (AVK_SELECT == wParam)
			{
				if (!me->bUploading && !me->bUploadRes)
				{
					DBGPRINTF("@@@ Upload @@@!!");
					CameraApp_ShowUpload(me);
				}
				else
				{
					DBGPRINTF("@@@ Uploading... @@@!!");
				}
				
				return TRUE;
			}
			else if (AVK_SOFT2 == wParam)
			{
				//正在上传过程可以取消
				if (me->bUploading)
				{
					CScs_CancelUpload(me->scs);
					CameraApp_ShowUploadRes(me, FALSE);
					return TRUE;
				}			
			}
		}
		
		//处理返回值
		if (AVK_SOFT2 == wParam)
		{
			if(me->bStartSuccess == FALSE)
			{
				ISHELL_CloseApplet((IShell*)me->piShell, FALSE);
				return TRUE;
			}

			if (me->nRootMode == MODE_SELECTING)
			{
				if (me->eFolderStateType != FOLDER_STATE_ROOT)
				{
					me->nPreMode = me->nMode;
					me->nMode = MODE_SELECTING;
				}
				else
				{
					me->nRootMode = MODE_PREVIEWING;
					CameraApp_ShowPreview(me);
					return TRUE;
				}
			}
			else
			{
				if (me->nMode == MODE_PREVIEWING)
				{
					//CameraApp_RemoveAll(me);
					ISHELL_CloseApplet((IShell*)me->piShell, FALSE);
				}
				else
				{
					CameraApp_ShowPreview(me);
					return TRUE;
				}
				
			}			
		}		

		if(me->bStartSuccess == FALSE)
		{
			return TRUE;
		}

		// Press CLEAR key to go back to Preview
		if (AVK_CLR == wParam) 
		{
			CameraApp_RemoveAll(me);
			CameraApp_ShowPreview(me);

			return TRUE;
		}
		
		//文件浏览
		if (me->nMode == MODE_SELECTING) 
		{
			//返回
			if (AVK_SOFT2 == wParam)
			{
				if (me->eFolderStateType == FOLDER_STATE_UPLOADING)
				{
					DBGPRINTF("UPLOADING....");
				}

				if (me->eFolderStateType == FOLDER_STATE_UPLOAD_RES)
				{
					me->eFolderStateType = FOLDER_STATE_VIEWING;
				}

				if (me->eFolderStateType == FOLDER_STATE_LIST)
				{					
					CameraApp_ShowFolder(me);
				}

				if (me->eFolderStateType == FOLDER_STATE_VIEWING)
				{
					CameraApp_ShowFolderList(me);
				}
			}

			if (AVK_SELECT == wParam || AVK_SOFT1 == wParam) 
			{
 				AECHAR *wstr = 0;
 				int nIndex; 
				char	szPath[AEE_MAX_FILE_NAME];
				char	szSection[20];

				
				if (SUCCESS == IWIDGET_GetFocusIndex(me->piwList, &nIndex))
				{
					if (SUCCESS == IVECTORMODEL_GetAt(me->piListModel, nIndex, (void**)&wstr))
					{
						int nSize;
						nSize = (WSTRLEN(wstr)+1)*sizeof(char);
						WSTRTOSTR(wstr, szSection, nSize);
						DBGPRINTF("Index:%d Text:%s", nIndex, szSection);
					}
					else
					{
						//目录为空
						return 0;
					}


					//根目录则判断是否存在
					if (me->eFolderStateType == FOLDER_STATE_ROOT)
					{
						switch (nIndex)
						{
						case 0:
							CameraApp_ShowFolderList(me);

							break;
						case 1:
							
							break;
						case 2:
							
							break;
						default:
							
							break;
						}
					}
					else
					{
						if (me->eStorageType == STOTAGE_PONE)
						{
							SNPRINTF(szPath, sizeof(szPath), "%simage/My Photo/%s", AEEFS_FILEMGR_DIR, szSection);
						}
						else
						{
							SNPRINTF(szPath, sizeof(szPath), "%simage/My Photo/%s", AEEFS_CARD0_DIR, szSection);
						}
						
						me->nPreIndex = nIndex;	//记录上次的索引

						me->eFolderStateType = FOLDER_STATE_VIEWING;
						STRCPY(me->szImage, szPath);
						CameraApp_ShowSnap(me, szPath);
					}
				}

			}
			else
			{
				IWIDGET_HandleEvent(me->piwList, eCode, wParam, dwParam);
			}
		}
		else if (me->nMode == MODE_STORAGE)			//存储位置
		{		
			if (AVK_SELECT == wParam || AVK_SOFT1 == wParam) 
			{
				AECHAR *wstr = 0;
				int nIndex;
				char	szSection[20];
				
				
				if (SUCCESS == IWIDGET_GetFocusIndex(me->piwList, &nIndex))
				{
					if (SUCCESS == IVECTORMODEL_GetAt(me->piListModel, nIndex, (void**)&wstr))
					{
						int nSize;
						nSize = (WSTRLEN(wstr)+1)*sizeof(char);
						WSTRTOSTR(wstr, szSection, nSize);
						DBGPRINTF("Index:%d Text:%s", nIndex, szSection);
					}

					switch (nIndex)
					{
					case 0:
						me->eStorageType = STOTAGE_PONE;
						break;
					
					case 1:
						me->eStorageType = STOTAGE_SDCARD;
						break;

					default:
						me->eStorageType = STOTAGE_PONE;
						break;
					}

					if (me->eStorageType == STOTAGE_PONE)
					{
						if (SUCCESS == IFILEMGR_Test(me->piFileMgr, AEEFS_FILEMGR_DIR))
						{
							SNPRINTF(me->szSnapDir, sizeof(me->szSnapDir), "%simage/My Photo/", AEEFS_FILEMGR_DIR);
						}
						else
						{
							DBGPRINTF("Index:%d Dir %s Not Exist!", nIndex, me->szSnapDir);
						}
					}
					else
					{
						if (SUCCESS == IFILEMGR_Test(me->piFileMgr, AEEFS_CARD0_DIR))
						{
							SNPRINTF(me->szSnapDir, sizeof(me->szSnapDir), "%simage/My Photo/", AEEFS_CARD0_DIR);
						}
						else if (SUCCESS == IFILEMGR_Test(me->piFileMgr, AEEFS_FILEMGR_DIR))
						{
							DBGPRINTF("Index:%d Dir %s Not Exist!", nIndex, me->szSnapDir);
							SNPRINTF(me->szSnapDir, sizeof(me->szSnapDir), "%simage/My Photo/", AEEFS_FILEMGR_DIR);
						}
						else
						{
							DBGPRINTF("Index:%d FileMg && SD Not Exist!", nIndex);
						}
					}
			
					DBGPRINTF("Index:%d Text:%s storageType:%s", nIndex, szSection, (me->eStorageType == STOTAGE_PONE)?"STOTAGE_PONE":"STOTAGE_SDCARD");
					CameraApp_ShowPreview(me);

				}
			}
			else
			{
				IWIDGET_HandleEvent(me->piwList, eCode, wParam, dwParam);
			}
		}
		else if (me->nMode == MODE_PICTURE_QUALITY)		//设置图片质量
		{
			if (AVK_SELECT == wParam || AVK_SOFT1 == wParam)
			{		
				int nIndex;
				int quality = 0;
				AECHAR *wstr = 0;

				if (SUCCESS == IWIDGET_GetFocusIndex(me->piwListPicQuality, &nIndex))
				{	
					if (SUCCESS == IVECTORMODEL_GetAt(me->piListModelPicQuality, nIndex, (void**)&wstr))
					{
						int nSize;
						char szInfo[10] = {""};
						nSize = (WSTRLEN(wstr)+1)*sizeof(char);
						WSTRTOSTR(wstr, szInfo, nSize);
						DBGPRINTF("Index:%d Text:%s", nIndex, szInfo);
					}

					switch (nIndex)
					{
					case 0:
						quality = 90;
						break;
					case 1:
						quality = 60;
						break;
					case 2:
						quality = 30;
						break;
					default:
						quality = 60;
						break;
					}
					
					nErr = ICAMERA_SetParm(me->piCamera, CAM_PARM_QUALITY, quality, 0);
					DBGPRINTF("Index:%d Set QUALITY:%d nErr:%d", nIndex, quality, nErr);
					nErr = ICAMERA_GetParm(me->piCamera, CAM_PARM_QUALITY, (int32*)(&quality), 0);
					DBGPRINTF("Index:%d Get QUALITY:%d nErr:%d", nIndex, quality, nErr);

					me->nQuality = quality;

					CameraApp_ShowPreview(me);

				}
				
			} 
			else
			{
				IWIDGET_HandleEvent(me->piwListPicQuality, eCode, wParam, dwParam);
			}
		}
		else if (me->nMode == MODE_PICTURE_SIZE)	//设置图片大小
		{
			if (AVK_SELECT == wParam || AVK_SOFT1 == wParam)
			{
				int nIndex; 
				AEESize picSize;
				AECHAR *wstr = 0;

				if (SUCCESS == IWIDGET_GetFocusIndex(me->piwListPicSize, &nIndex))
				{
					if (SUCCESS == IVECTORMODEL_GetAt(me->piListModelPicSize, nIndex, (void**)&wstr))
					{
						int nSize;
						char szInfo[20] = {""};
						nSize = (WSTRLEN(wstr)+1)*sizeof(char);
						WSTRTOSTR(wstr, szInfo, nSize);
						DBGPRINTF("Index:%d Text:%s", nIndex, szInfo);
					}

					switch (nIndex)
					{
					case 0:
						picSize.cx = 240;
						picSize.cy = 320;
						break;
					case 1:
						picSize.cx = 480;
						picSize.cy = 640;
						break;
					case 2:
						picSize.cx = 600;
						picSize.cy = 800;
						break;
					case 3:
						picSize.cx = 960;
						picSize.cy = 1280;
						break;
					default:
						picSize.cx = DEF_PIC_WIDTH;
						picSize.cy = DEF_PIC_HEIGHT;
						break;
					}

					nErr = ICAMERA_SetParm(me->piCamera, CAM_PARM_SIZE, (int32)(&picSize), 0);
					DBGPRINTF("Index:%d Set Size: %d %d nErr:%d", nIndex, picSize.cx, picSize.cy, nErr);
					nErr = ICAMERA_GetParm(me->piCamera, CAM_PARM_SIZE, (int32*)(&picSize), 0);					
					DBGPRINTF("Index:%d Get Size: %d %d nErr:%d", nIndex, picSize.cx, picSize.cy, nErr);

					me->picSize = picSize;
					CameraApp_ShowPreview(me);
				}
				
			} 
			else
			{
				IWIDGET_HandleEvent(me->piwListPicSize, eCode, wParam, dwParam);
			}
		}
		else 
		{
			if (AVK_RIGHT == wParam) {
				CameraApp_UpdateButton(me, me->nActiveButton, 0);
				me->nActiveButton = (me->nActiveButton+1) % (NUM_BUTTONS);
				CameraApp_UpdateButton(me, me->nActiveButton, 1);
				
			} else if (AVK_LEFT == wParam) {				
				CameraApp_UpdateButton(me, me->nActiveButton, 0);
				me->nActiveButton = (me->nActiveButton-1 + (NUM_BUTTONS)) % (NUM_BUTTONS);
				CameraApp_UpdateButton(me, me->nActiveButton, 1);
				
			} else if (AVK_SELECT == wParam) {
				
				switch (me->nActiveButton) 
				{
				case IDX_CAPTURE:
					if (!me->bSnaping)
						CameraApp_Capture(me);
					break;

				case IDX_PICTURE_QUALITY:
					CameraApp_ShowPicQuality(me);
					break;

				case IDX_PICTURE_SIZE:
					CameraApp_ShowPicSize(me);
					break;

				case IDX_STORAGE:
					CameraApp_ShowStorage(me);
					break;

				case IDX_FOLDER:
					CameraApp_ShowFolder(me);
					break;
				default:
					break;
				}
			}
		}
		
		return TRUE;
		
	}  // switch(eCode)
	
	return FALSE;
}

static void CameraApp_Snapshot(void * pUser)
{
	CameraApp *me = (CameraApp*)pUser;
	int16 nMode;
	boolean bPaused;
	int nErr = 0;
	
	// Check for ready state
	if ((SUCCESS == ICAMERA_GetMode(me->piCamera, &nMode, &bPaused)) && nMode == CAM_MODE_READY) {
		// Take snapshot
		
		me->nPreMode = me->nMode;

		me->nMode = MODE_CAPTURING;
		me->nRootMode = MODE_CAPTURING;
		nErr = ICAMERA_RecordSnapshot(me->piCamera);

		DBGPRINTF("ICAMERA_RecordSnapshot Over");
	}
	else{
		ISHELL_SetTimer(me->piShell, CAMERA_TIMER, CameraApp_Snapshot, (void*)me);
	}
}

static void CameraApp_ShowButtons(void * pUser, const boolean bShow)
{
	int i = 0;
	int nErr = 1;
	CameraApp *me = (CameraApp*)pUser;

	if (bShow)
	{
		// Insert Button widgets
		for (i=0; i<NUM_BUTTONS; i++) {
			IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwButtons[i], 3 + i*(BUTTON_WIDTH+10), me->rcContainer.dy - BUTTON_WIDTH);
		}
	}
	else
	{
		//Remove Buttons
		for (i = 0; i < NUM_BUTTONS; i ++)
		{
			if (me->piwButtons[i])
			{
				REMOVEWIDGET(me->pwcRoot, me->piwButtons[i]);
			}
		}
	}

}


static void CameraApp_ShowSnap(void * pUser, const char* imagePath)
{
	CameraApp *me = (CameraApp *)pUser;
	
	int nErr = 1;
	FileInfo pi;
	IImage *piImage = NULL;
	char*     cpszMIME = NULL;
	AEECLSID cls=0;
	uint32 dwSize = 0;

	me->bUploadRes = FALSE;

	//////////////////////////////////////////////////////////////////////////
	//NEW_ISSUE 添加文件大小校验, 如果超过设定大小则提示错误
	nErr = IFILEMGR_GetInfo(me->piFileMgr, me->szImage, &pi);
	if (pi.dwSize > MAX_UPLOAD_FILE_SIZE)
	{
		DBGPRINTF("CameraApp_ShowUpload:%s pi.dwSize:%d MAX:%d", me->szImage, pi.dwSize, MAX_UPLOAD_FILE_SIZE);
		CameraApp_ShowAlert(pUser, ALERT_BIG_FILE);
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	piImage = ISHELL_LoadImage(me->piShell, imagePath);

	me->nRetryCount = 0;
	IIMAGE_Notify(piImage, CameraApp_DrawNotify, pUser);

}

static void CameraApp_ShowPreview(void * pUser)
{
	int nErr = 0;
	CameraApp *me = (CameraApp *)pUser;
	int nMode = 0;

	//设定抓拍图片大小
	nErr = ICAMERA_SetSize(me->piCamera, &me->picSize);

	//设定预览
	nErr = ICAMERA_Preview(me->piCamera);
	
	me->nPreMode = me->nMode;
	me->nMode = MODE_PREVIEWING;
	
	CameraApp_UpdateButton(me, me->nActiveButton, 0);
	me->nActiveButton = IDX_CAPTURE;	// Set active button to Play
	CameraApp_UpdateButton(me, me->nActiveButton, 1);

	CameraApp_RemoveAll(pUser);
	CameraApp_ShowButtons(me, TRUE);
	
	IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(25,25,25));

 	IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwFrame, 0, 0);
}


static void CameraApp_Capture (void * pUser)
{
	int i = 1;
	int nErr = 1;
	AEEMediaData md;
	CameraApp *me = (CameraApp *)pUser;
	char szTime[16];

	hal_walltime_t walltime;
	
	hal_walltime_now(&walltime);

	SNPRINTF(szTime, sizeof(szTime), "%04%02d%02d%02d", walltime.year, walltime.month, walltime.day, walltime.hour);

#ifndef AEE_SIMULATOR
	//////////////////////////////////////////////////////////////////////////
	//NEW_ISSUE 添加内存剩余空间校验和提示, 当内存空间低于设定的空间时提示
	{
		int nErr = 0;
		uint32 dwTotal = 0;
		uint32 dwFree = 0;
		char *dir = NULL;

		if (me->eStorageType == STOTAGE_PONE)
		{
			dir = AEEFS_FILEMGR_DIR;
		}
		else
		{
			dir = AEEFS_CARD0_DIR;
		}

		nErr = IFILEMGR_GetFreeSpaceEx(me->piFileMgr, dir, &dwTotal, &dwFree);
		if (nErr == SUCCESS)
		{
			DBGPRINTF("Dir:%s Space Total:%d Free:%d", dir, dwTotal, dwFree);

			if (dwFree < MIN_STORAGE_SIZE)
			{
				DBGPRINTF("Dir:%s Space Total:%d Free:%d Min:%d Low Memory", dir, dwTotal, dwFree, MIN_STORAGE_SIZE);

				CameraApp_ShowAlert(me, ALERT_LOW_MEM);
				return ;
			}
		}

	}
#endif	//AEE_SIMULATOR
	
	// Find suitable file name
	do {
		SNPRINTF(me->szImage, sizeof(me->szImage), "%s%s%03i.jpg", me->szSnapDir, szTime, i++);
	} while (IFILEMGR_Test(me->piFileMgr, me->szImage) == SUCCESS);
	
	md.clsData = MMD_FILE_NAME;
	md.pData = (void *)me->szImage;
	md.dwSize = 0;
	
	// Camera must be in ready state
	nErr = ICAMERA_Stop(me->piCamera);
	
	me->bSnaping = TRUE;

#ifdef AEE_SIMULATOR
	nErr = ICAMERA_SetParm(me->piCamera, CAM_PARM_VIDEO_ENCODE, AEECLSID_JPEG, 0);
	nErr = ICAMERA_SetMediaData(me->piCamera, &md, MT_JPG);
	nErr = ICAMERA_DeferEncode(me->piCamera, FALSE);
#else
#ifdef CAM_PARM_PREVIEW_TYPE
#ifdef CAM_PREVIEW_SNAPSHOT
	{
		int nMode = 0;
		ICAMERA_GetMode(me->piCamera, &nMode, NULL);
		DBGPRINTF("ICAMERA_GetMode:%d", nMode);
	}
	nErr = ICAMERA_SetParm(me->piCamera, CAM_PARM_PREVIEW_TYPE, CAM_PREVIEW_SNAPSHOT, 0);
	DBGPRINTF("ICAMERA_SetParm CAM_PREVIEW_SNAPSHOT ret:%d", nErr);
#endif // CAM_PREVIEW_SNAPSHOT
#endif // CAM_PARM_PREVIEW_TYPE
	nErr = ICAMERA_SetParm(me->piCamera, CAM_PARM_VIDEO_ENCODE, AEECLSID_JPEG, 0);
	DBGPRINTF("ICAMERA_SetParm CAM_PARM_VIDEO_ENCODE PNG ret:%d", nErr);
	nErr = ICAMERA_SetMediaData(me->piCamera, &md, MT_JPG);
	DBGPRINTF("ICAMERA_SetMediaData PNG ret:%d", nErr);
	nErr = ICAMERA_DeferEncode(me->piCamera, FALSE);
	DBGPRINTF("ICAMERA_DeferEncode PNG ret:%d", nErr);
#endif
	// Set timer so camera has a chance to get to ready state
	// Check for ready state
	ISHELL_SetTimer(me->piShell, CAMERA_TIMER, CameraApp_Snapshot, (void*)me);
}

static void CameraApp_ShowFolder (void * pUser)
{
	int nErr = 1;
	IImage *piImage;
	CameraApp *me = (CameraApp *)pUser;
	
	int i = 0;
	char *fileFolder[3];
// 	fileFolder[0] = "Image";
// 	fileFolder[1] = "Video";
// 	fileFolder[2] = "Music";
	fileFolder[0] = "";
	fileFolder[1] = "";
	fileFolder[2] = "";

	IVECTORMODEL_DeleteAll(me->piListModel);

	for (i = 0; i < 3; i ++)
	{
		int nSize = (STRLEN(fileFolder[i]) * sizeof(AECHAR) + 2);
		AECHAR *wstr = (AECHAR*)MALLOC(nSize);
		STRTOWSTR(fileFolder[i], wstr, nSize);
		IVECTORMODEL_Add(me->piListModel, wstr);
	}
	
	// Camera must be in ready state
	ICAMERA_Stop(me->piCamera);
	
	CameraApp_RemoveAll(pUser);
	
	me->nPreMode = me->nMode;
	me->nMode = MODE_SELECTING;
	me->nRootMode = MODE_SELECTING;

	me->eFolderStateType = FOLDER_STATE_ROOT;

	IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(32,160,192));
	CameraApp_ShowText(me, IDS_STRING_SELECT, me->piwLText, 5, me->rcContainer.dy - BUTTON_WIDTH + 10);
	CameraApp_ShowText(me, IDS_STRING_RETURN, me->piwRText, me->rcContainer.dx - 40, me->rcContainer.dy - BUTTON_WIDTH + 10);
	
	piImage = ISHELL_LoadResImage(me->piShell, RESFILE, IDB_FOLDER_BG);
	if (piImage)
	{
		IWIDGET_SetBGImage(me->piwList, piImage);
		RELEASEIF(piImage);
	}

	IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwList, 0, 0);

}

static void CameraApp_ShowFolderList (void * pUser)
{
	char	szPath[AEE_MAX_FILE_NAME];
	CameraApp *me = (CameraApp *)pUser;

	CameraApp_RemoveAll(me);
	
	IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(32,160,192));
	CameraApp_ShowText(me, IDS_STRING_SELECT, me->piwLText, 5, me->rcContainer.dy - BUTTON_WIDTH + 10);
	CameraApp_ShowText(me, IDS_STRING_RETURN, me->piwRText, me->rcContainer.dx - 40, me->rcContainer.dy - BUTTON_WIDTH + 10);

	IWIDGET_SetItemHeight(me->piwList, 30);
	IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwList, 0, 0);
	

	if (me->eStorageType == STOTAGE_PONE)
	{
		SNPRINTF(szPath, sizeof(szPath), "%simage/My Photo", AEEFS_FILEMGR_DIR);
	}
	else
	{
		SNPRINTF(szPath, sizeof(szPath), "%simage/My Photo", AEEFS_CARD0_DIR);
	}
	
	IVECTORMODEL_DeleteAll(me->piListModel);
	
	ShowSubFiles(me, szPath, me->piListModel);

	IWIDGET_SetFocusIndex(me->piwList, me->nPreIndex);
	me->eFolderStateType = FOLDER_STATE_LIST;
	me->nMode = MODE_SELECTING;
}

static void CameraApp_ShowStorage (void * pUser)
{
	CameraApp *me = (CameraApp *)pUser;
	int nErr = 0;
	int nSize = 20;
	AECHAR *wstr[2];
	
	IVECTORMODEL_DeleteAll(me->piListModel);

	//if (SUCCESS == IFILEMGR_Test(me->piFileMgr, AEEFS_FILEMGR_DIR))
	{
		wstr[0] = (AECHAR*)MALLOC(nSize);
		nErr = ISHELL_LoadResString(me->piShell, RESFILE, IDS_STRING_PHONE, wstr[0], nSize);
		IVECTORMODEL_Add(me->piListModel, wstr[0]);
	}
	
	if (SUCCESS == IFILEMGR_Test(me->piFileMgr, AEEFS_CARD0_DIR))
	{
		wstr[1] = (AECHAR*)MALLOC(nSize);
		nErr = ISHELL_LoadResString(me->piShell, RESFILE, IDS_STRING_SDCARD, wstr[1], nSize);
		IVECTORMODEL_Add(me->piListModel, wstr[1]);
	}
	
	// Camera must be in ready state
	ICAMERA_Stop(me->piCamera);
	
	CameraApp_RemoveAll(pUser);
	
	me->nPreMode = me->nMode;
	me->nMode = MODE_STORAGE;
	me->nRootMode = MODE_STORAGE;

	IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(32,160,192));
	CameraApp_ShowText(me, IDS_STRING_SELECT, me->piwLText, 5, me->rcContainer.dy - BUTTON_WIDTH + 10);
	CameraApp_ShowText(me, IDS_STRING_RETURN, me->piwRText, me->rcContainer.dx - 40, me->rcContainer.dy - BUTTON_WIDTH + 10);


	IWIDGET_SetBGImage(me->piwList, NULL);
	IWIDGET_SetItemHeight(me->piwList, 30);
	IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwList, 0, 0);

	if (me->eStorageType == STOTAGE_PONE)
	{
		IWIDGET_SetFocusIndex(me->piwList, 0);
	}
	else
	{
		IWIDGET_SetFocusIndex(me->piwList, 1);
	}

}

static void CameraApp_ShowPicQuality (void * pUser)
{
	CameraApp *me = (CameraApp *)pUser;
  
	// Camera must be in ready state
	ICAMERA_Stop(me->piCamera);

	CameraApp_RemoveAll(pUser);

	me->nPreMode = me->nMode;
	me->nMode = MODE_PICTURE_QUALITY;
	me->nRootMode = MODE_PICTURE_QUALITY;

	IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(32,160,192));
	CameraApp_ShowText(me, IDS_STRING_SET, me->piwLText, 5, me->rcContainer.dy - BUTTON_WIDTH + 10);
	CameraApp_ShowText(me, IDS_STRING_RETURN, me->piwRText, me->rcContainer.dx - 40, me->rcContainer.dy - BUTTON_WIDTH + 10);
	
	IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwListPicQuality, 0, 0);
}

static void	   CameraApp_ShowPicSize( void * pUser)
{
	CameraApp *me = (CameraApp *)pUser;
	int index = 0;

	// Camera must be in ready state
	ICAMERA_Stop(me->piCamera);
	
	CameraApp_RemoveAll(pUser);
	
	me->nPreMode = me->nMode;
	me->nMode = MODE_PICTURE_SIZE;
	me->nRootMode = MODE_PICTURE_SIZE;
	
	//根据当前记录尺寸设定选择的条目
	if (me->picSize.cx == 240)
	{
		index = 0;
	}
	else if (me->picSize.cx == 480)
	{
		index = 1;
	}
	else if (me->picSize.cx == 600)
	{
		index = 2;
	}
	else if (me->picSize.cx == 960)
	{
		index = 3;
	}

	IWIDGET_SetFocusIndex(me->piwListPicSize, index);

	IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(32,160,192));
	CameraApp_ShowText(me, IDS_STRING_SET, me->piwLText, 5, me->rcContainer.dy - BUTTON_WIDTH + 10);
	CameraApp_ShowText(me, IDS_STRING_RETURN, me->piwRText, me->rcContainer.dx - 40, me->rcContainer.dy - BUTTON_WIDTH + 10);

	IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwListPicSize, 0, 0);
	
}


static void CameraApp_ShowUpload (void * pUser)
{
	int uploadRes = -1;
	CameraApp *me = (CameraApp *)pUser;
	
	IImage *piImage = NULL;

	me->nPreMode = me->nMode;
	if (me->nRootMode == MODE_SELECTING)
	{
		me->eFolderStateType = FOLDER_STATE_UPLOADING;
	}

	//CameraApp_RemoveAll(pUser);
	
	REMOVEWIDGET(me->pwcRoot, me->piwFtImage);
	REMOVEWIDGET(me->pwcRoot, me->piwExText);
	REMOVEWIDGET(me->pwcRoot, me->piwMText);


	IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(32,160,192));

	piImage = ISHELL_LoadResImage(me->piShell, RESFILE, IDB_UPLOAD);
	if (piImage) {
		//IImage_SetParm(piImage, IPARM_SCALE, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
		IWIDGET_SetImage(me->piwUpImage, piImage);
		
		IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwUpImage, ALERT_OFFSET, ALERT_OFFSET);
		//CameraApp_ShowText(me, IDS_STRING_UPLOADING, me->piwMText, ALERT_OFFSET + 40, ALERT_WIDTH+ALERT_OFFSET-20);
		CameraApp_ShowText(me, IDS_STRING_UPLOADING, me->piwMText, me->rcContainer.dx / 3 - 20 , me->rcContainer.dy - BUTTON_WIDTH + 10);

		RELEASEIF(piImage);
	}

	//////////////////////////////////////////////////////////////////////////
	IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwProgress, ALERT_OFFSET, ALERT_WIDTH+ALERT_OFFSET);
	//////////////////////////////////////////////////////////////////////////

	me->bUploading = TRUE;
	uploadRes = CScs_UploadPic(me->scs,me->szImage);
	if (uploadRes != 0)
	{
		CameraApp_ShowUploadFailInfo(me,uploadRes);
	}

	//For Test
	//ISHELL_SetTimer(me->piShell, UPLOAD_TIMER, CameraApp_ShowUploadRes, (void*)me);

}


void CameraApp_ShowUploadRes (void * pUser, const boolean bRes)
{
	CameraApp *me = (CameraApp *)pUser;
	
	IImage *piImage = NULL;
	
	//CameraApp_RemoveAll(pUser);
	
	REMOVEWIDGET(me->pwcRoot, me->piwFtImage);
	REMOVEWIDGET(me->pwcRoot, me->piwProgress);
	REMOVEWIDGET(me->pwcRoot, me->piwExText);
	REMOVEWIDGET(me->pwcRoot, me->piwMText);
	REMOVEWIDGET(me->pwcRoot, me->piwFrame);

	if (bRes)
	{
		piImage = ISHELL_LoadResImage(me->piShell, RESFILE, IDB_SUCCESS);
		CameraApp_ShowText(me, IDS_STRING_UPLOAD_SUCCESS, me->piwMText, me->rcContainer.dx / 2 - 30 , me->rcContainer.dy - BUTTON_WIDTH + 10);
	}
	else
	{
		piImage = ISHELL_LoadResImage(me->piShell, RESFILE, IDB_FAILED);
		CameraApp_ShowText(me, IDS_STRING_UPLOAD_FAILED, me->piwMText, me->rcContainer.dx / 2 - 30 , me->rcContainer.dy - BUTTON_WIDTH + 10);
	}
	
	IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(32,160,192));

	me->nPreMode = me->nMode;

	if (me->nRootMode == MODE_SELECTING)
	{
		me->eFolderStateType = FOLDER_STATE_UPLOAD_RES;
	}

	if (piImage) {
		IWIDGET_SetImage(me->piwUpImage, piImage);
		IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwUpImage, ALERT_OFFSET, ALERT_OFFSET);
		RELEASEIF(piImage);
	}

	//!!暂时注掉自动返回前一个页面!!
// 	if (me->nRootMode == MODE_SELECTING)
// 	{
// 		ISHELL_SetTimer(me->piShell, UPLOAD_TIMER, CameraApp_ShowFolderList, (void*)me);
// 	}
// 	else
// 	{
// 		ISHELL_SetTimer(me->piShell, UPLOAD_TIMER, CameraApp_ShowPreview, (void*)me);
// 	}

// 	if (bRes)
// 	{
// 		ISHELL_SetTimer(me->piShell, UPLOAD_TIMER, CameraApp_ShowPreview, (void*)me);
// 	}

	me->bUploadRes = bRes;	//记录上传结果值
	me->bUploading = FALSE;	//记录上传状态
}


static void CameraApp_ShowUploadFailInfo (void * pUser, int errNo)
{
	CameraApp *me = (CameraApp *)pUser;

	IImage *piImage = NULL;

	//CameraApp_RemoveAll(pUser);

	REMOVEWIDGET(me->pwcRoot, me->piwFtImage);
	REMOVEWIDGET(me->pwcRoot, me->piwProgress);
	REMOVEWIDGET(me->pwcRoot, me->piwMText);
	REMOVEWIDGET(me->pwcRoot, me->piwFrame);

	piImage = ISHELL_LoadResImage(me->piShell, RESFILE, IDB_FAILED);

	if (errNo == -1)
	{
	}
	else if(errNo == -2)
	{
		CameraApp_ShowText(me, IDS_STRING_UID_NOT_GET, me->piwMText, 65 , me->rcContainer.dy - BUTTON_WIDTH + 10);
	}
	else if( errNo == -3)
	{
		CameraApp_ShowText(me, IDS_STRING_SCS_INI_NOT_FOUND, me->piwMText,  75 , me->rcContainer.dy - BUTTON_WIDTH + 10);
	}

	IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(32,160,192));

	me->nPreMode = me->nMode;

	if (me->nRootMode == MODE_SELECTING)
	{
		me->eFolderStateType = FOLDER_STATE_UPLOAD_RES;
	}

	if (piImage) {
		IWIDGET_SetImage(me->piwUpImage, piImage);
		IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwUpImage, ALERT_OFFSET, ALERT_OFFSET);
		RELEASEIF(piImage);
	}

	//!!暂时注掉自动返回前一个页面!!
	// 	if (me->nRootMode == MODE_SELECTING)
	// 	{
	// 		ISHELL_SetTimer(me->piShell, UPLOAD_TIMER, CameraApp_ShowFolderList, (void*)me);
	// 	}
	// 	else
	// 	{
	// 		ISHELL_SetTimer(me->piShell, UPLOAD_TIMER, CameraApp_ShowPreview, (void*)me);
	// 	}

	// 	if (bRes)
	// 	{
	// 		ISHELL_SetTimer(me->piShell, UPLOAD_TIMER, CameraApp_ShowPreview, (void*)me);
	// 	}

	me->bUploadRes = FALSE;	//记录上传结果值
	me->bUploading = FALSE;	//记录上传状态
}



static void	CameraApp_RemoveAll( void * pUser)
{
	CameraApp *me = (CameraApp *)pUser;


	IWIDGET_SetImage(me->piwImage, NULL);

	REMOVEWIDGET(me->pwcRoot, me->piwImage);
	REMOVEWIDGET(me->pwcRoot, me->piwFtImage);
	REMOVEWIDGET(me->pwcRoot, me->piwUpImage);
	REMOVEWIDGET(me->pwcRoot, me->piwFrame);
	REMOVEWIDGET(me->pwcRoot, me->piwList);
	REMOVEWIDGET(me->pwcRoot, me->piwListPicSize);
	REMOVEWIDGET(me->pwcRoot, me->piwListPicQuality);

	REMOVEWIDGET(me->pwcRoot, me->piwTitle);
	REMOVEWIDGET(me->pwcRoot, me->piwLText);
	REMOVEWIDGET(me->pwcRoot, me->piwMText);
	REMOVEWIDGET(me->pwcRoot, me->piwRText);
	REMOVEWIDGET(me->pwcRoot, me->piwExText);
	

	CameraApp_ShowButtons(pUser, FALSE);

}

// Draw Notify Callback
void CameraApp_DrawNotify(void *pUser, IImage *pImage, AEEImageInfo *pi, int nErr)
{
    CameraApp *me = (CameraApp*)pUser;
	
    if( NULL == me || NULL == pImage)
		return;
	
	DBGPRINTF("CameraApp_DrawNotify nErr:%d", nErr);

    if (nErr == AEE_SUCCESS)
    {
		me->nPreMode = me->nMode;
		me->nMode = MODE_VIEWING;
		
		CameraApp_RemoveAll(pUser);
	
		IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(32,160,192));
		CameraApp_ShowText(me, IDS_STRING_UPLOAD, me->piwMText, me->rcContainer.dx / 2 - 15, me->rcContainer.dy - BUTTON_WIDTH + 10);
		CameraApp_ShowText(me, IDS_STRING_RETURN, me->piwRText, me->rcContainer.dx - 40, me->rcContainer.dy - BUTTON_WIDTH + 10);

		if (pImage != NULL) {
			IImage_SetParm(pImage, IPARM_SCALE, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
			IWIDGET_SetImage(me->piwImage, pImage);
			
			IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwImage, 0, 0);
			
			RELEASEIF(pImage);
		}
		else
		{
			IWIDGET_SetImage(me->piwImage, NULL);
			IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwImage, 0, 0);
		}
		
		//显示 上传 界面时，发送查询经纬度和uid的消息
		ISHELL_PostEvent(me->piShell, AEECLSID_SLPTT, EVT_CAPTURE_EVENT, STRLEN(CAPTURE_EVENT_MSG), CAPTURE_EVENT_MSG);
    }
	else
	{
		if (me->nRetryCount == 0)
		{
			IImage *piImage = ISHELL_LoadImage(me->piShell, me->szImage);
			
			IIMAGE_Notify(piImage, CameraApp_DrawNotify, pUser);

			me->nRetryCount ++;
			DBGPRINTF("IIMAGE_Notify Retry:%d", me->nRetryCount);
		}
		else
		{
			IWIDGET_SetImage(me->piwImage, NULL);
			IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwImage, 0, 0);
			
			DBGPRINTF("IIMAGE_Notify Retry:%d Failed", me->nRetryCount);
		}
	}

	RELEASEIF(pImage);
}


static void CameraApp_Notify(void * pUser, AEECameraNotify * pNotify)
{
	int nErr = 0;
	int i = 0;
	CameraApp *me = (CameraApp*)pUser;
	
	if (pNotify->nCmd == CAM_CMD_ENCODESNAPSHOT) {
		
		if (pNotify->nStatus == CAM_STATUS_DONE) {
			
			me->bSnaping = FALSE;
			CameraApp_ShowSnap(me, me->szImage);
			
		}else { // Failure
			me->nPreMode = me->nMode;
			me->nMode = MODE_PREVIEWING;
			me->bSnaping = FALSE;

			DBGPRINTF("ICAMERA_RecordSnapshot Failure , Start Preview");
			nErr = ICAMERA_Preview(me->piCamera);
		}
	}

	DBGPRINTF("CameraApp_Notify cmd:%d nStatus:%d", pNotify->nCmd, pNotify->nStatus);

}

static void CameraApp_UpdateButton(void * pUser, int16 nButtonIdx, boolean bSel)
{
	CameraApp *me = (CameraApp *)pUser;
	int nErr = 0;
	IImage *piImage = NULL;
	int16 nButtonBmp = 0;

	if ( !bSel )
	{
		nButtonBmp = me->btnResId[nButtonIdx][0];
	}
	else
	{
		nButtonBmp = me->btnResId[nButtonIdx][1];
	}

	piImage = ISHELL_LoadResImage(me->piShell, RESFILE, nButtonBmp);
	
	if (piImage) {
		nErr = IWIDGET_SetImage(me->piwButtons[nButtonIdx], piImage);
		
	} else {
		nErr = ERESOURCENOTFOUND;
	}
	
	RELEASEIF(piImage);
}

//显示提示信息
static void CameraApp_ShowAlert(void *pUser, EAlertType type)
{
	CameraApp *me = (CameraApp *)pUser;
	IImage *piImage = NULL;

	// Camera must be in ready state
	ICAMERA_Stop(me->piCamera);
	
	CameraApp_RemoveAll(pUser);

	IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(32,160,192));
	IWIDGET_SetBGColor(me->piwImage, MAKE_RGB(147,227,255));
	IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwImage, 0, 0);

	switch (type)
	{
	case ALERT_LOW_MEM:	//内存不足

		CameraApp_ShowText(me, IDS_STRING_ALERT_MEM_SIZE1, me->piwLText, me->rcContainer.dx / 3 + 5, ALERT_WIDTH+ALERT_OFFSET);
		CameraApp_ShowText(me, IDS_STRING_ALERT_MEM_SIZE2, me->piwMText, me->rcContainer.dx / 3 - 12, ALERT_WIDTH+ALERT_OFFSET + 20);
		CameraApp_ShowText(me, IDS_STRING_RETURN, me->piwRText, me->rcContainer.dx - 40, me->rcContainer.dy - BUTTON_WIDTH + 10);
		break;

	case ALERT_BIG_FILE://文件太大

		CameraApp_ShowText(me, IDS_STRING_ALERT_FILE_SIZE1, me->piwLText, me->rcContainer.dx / 3 - 10, ALERT_WIDTH+ALERT_OFFSET);
		CameraApp_ShowText(me, IDS_STRING_ALERT_FILE_SIZE2, me->piwMText, me->rcContainer.dx / 3 - 10, ALERT_WIDTH+ALERT_OFFSET + 20);
		CameraApp_ShowText(me, IDS_STRING_RETURN, me->piwRText, me->rcContainer.dx - 40, me->rcContainer.dy - BUTTON_WIDTH + 10);
		break;

	default:
		return;
	}

	piImage = ISHELL_LoadResImage(me->piShell, RESFILE, IDB_ALERT);
	if (piImage) {
		IWIDGET_SetImage(me->piwUpImage, piImage);
		IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwUpImage, ALERT_OFFSET, ALERT_OFFSET);
		RELEASEIF(piImage);
	}

	if (me->nRootMode == MODE_SELECTING)
	{
		ISHELL_SetTimer(me->piShell, ALERT_TIMER, CameraApp_ShowFolderList, (void*)me);
	}
	else
	{
		ISHELL_SetTimer(me->piShell, ALERT_TIMER, CameraApp_ShowPreview, (void*)me);
	}
	
}

static uint16 findPicType( const char* imagePath )
{
	char type[32];
	MEMSET(type,0,32);
	
	if (imagePath == NULL)
	{
		return 0;
	}
	
	if (STRRCHR(imagePath,'.') != NULL) {
		char* temp = STRRCHR(imagePath,'.') + 1;
		
		if (temp == NULL)
		{
			return 0;
		}

		if (STRLEN(temp) <=0)
		{
			return (uint16)0;
		}
		
		if ( STRCMP(temp , "JPG") == 0 || STRCMP(temp , "JPEG") == 0 || STRCMP(temp , "jpg" )==0 || STRCMP(temp , "jpeg") == 0 )
		{
			return (uint16)AEECLSID_JPEG;
		}
		else if (STRCMP(temp , "PNG") == 0 || STRCMP(temp , "png" )==0)
		{
			return (uint16)AEECLSID_PNG;
		}
		else if (STRCMP(temp , "BMP") == 0 || STRCMP(temp , "bmp" )==0)
		{
			return (uint16)AEECLSID_WINBMP;
		}
// 		else if (STRCMP(temp , "GIF") == 0 || STRCMP(temp , "gif" )==0)	//2015.06.18 yao del
// 		{
// 			return (uint16)AEECLSID_GIF;
// 		}
		else{
			return (uint16)0;
		}
	}
}

//--------------------------------------------------------------------
//   CameraApp Creation
//--------------------------------------------------------------------

static int CameraApp_CreateButton(CameraApp *me, int16 nButtonIdx, int16 nButtonBmp)
{
	int nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_IMAGEWIDGET, (void**)&me->piwButtons[nButtonIdx]);
	
	// Load image
	if (!nErr) {
		IImage *piImage = ISHELL_LoadResImage(me->piShell, RESFILE, nButtonBmp);
		
		if (piImage) {
			nErr = IWIDGET_SetImage(me->piwButtons[nButtonIdx], piImage);
			
		} else {
			nErr = ERESOURCENOTFOUND;
		}
		
		RELEASEIF(piImage);
	}
	
	// Configure the button
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwButtons[nButtonIdx], BUTTON_WIDTH, BUTTON_WIDTH);
		IWIDGET_SetBorderColor(me->piwButtons[nButtonIdx], MAKE_RGB(255,255,255));
	}
	
	return nErr;
}


static int CameraApp_CreateWidgets(CameraApp *me)
{
	int nErr = 0;
	
	// Create frame widget to display the video
	nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_FRAMEWIDGET, (void**)&me->piwFrame);
	
	// Configure the frame widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwFrame, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
		IWIDGET_SetBGColor(me->piwFrame, MAKE_RGB(0,0,0));
	}
	
	// Create ICamera object
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_CAMERA, (void**)&me->piCamera);
		DBGPRINTF("AEECLSID_CAMERA nErr = %d",nErr);
	}
		
   // Configure ICamera
   if (!nErr) {
      AEESize size;
 	 
	  me->picSize.cx = DEF_PIC_WIDTH;	
	  me->picSize.cy = DEF_PIC_HEIGHT;
	  
      size.cx = me->rcContainer.dx;
      size.cy = me->rcContainer.dy - BUTTON_WIDTH;
	  //size.cy = (size.cy >> 4)<<4;
	  ICAMERA_SetSize(me->piCamera, &size);
      ICAMERA_SetDisplaySize(me->piCamera, &size);
   }
	// Create camera frame model instance to hold the ICamera object
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_CAMERAFRAMEMODEL, (void**)&me->piModel);
		DBGPRINTF("AEECLSID_CAMERAFRAMEMODEL Over");
	}
	
	// Configure the camera frame model
	if (!nErr) {
		ICAMERAFRAMEMODEL_RegisterNotify(me->piModel, CameraApp_Notify, me);
	}
	
	// Set the ICamera into the ICameraFrameModel
	if (!nErr) {
		nErr = ICAMERAFRAMEMODEL_SetICamera(me->piModel, me->piCamera);
		DBGPRINTF("ICAMERAFRAMEMODEL_SetICamera Over");
	}
	
	// Set the model on the frame widget
	if (!nErr) {
		IWIDGET_SetModel(me->piwFrame, (IModel*)me->piModel);
		DBGPRINTF("IWIDGET_SetModel Over");
	}
	
	// Create the File List widget for image selection
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_LISTWIDGET, (void**)&me->piwList);
	}
	
	// Configure the File List widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwList, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
		IWIDGET_SetBGColor(me->piwList, MAKE_RGB(147,227,255));
		IWIDGET_SetBorderWidth(me->piwList, 1);
		IWIDGET_SetItemHeight(me->piwList, 30);
		//IWIDGET_SetBGColor(me->piwList, MAKE_RGB(166,202,240));
		IWIDGET_SetModel(me->piwList, (IModel*)me->piListModel);
	}
	
	// Use Static widget to draw list items
	if (!nErr) {
		
		IWidget *pisw = 0;
		
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_STATICWIDGET, (void**)&pisw);
		
		if (!nErr) {
			
			IWIDGET_SetLeftPadding(pisw, 1);
			IWIDGET_SetRightPadding(pisw, 1);
			IWIDGET_SetBorderWidth(pisw, 1);
			IWIDGET_SetSelectedBorderColor(pisw, MAKE_RGBA(0,0,255,255));
			IDECORATOR_SetWidget((IDecorator *)me->piwList, pisw);
		}
		
		RELEASEIF(pisw);
	}

	//////////////////////////////////////////////////////////////////////////

	// Create the Picture Size List widget for image selection
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_LISTWIDGET, (void**)&me->piwListPicSize);
	}
	
	// Configure the Picture Size List widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwListPicSize, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
		IWIDGET_SetBGColor(me->piwListPicSize, MAKE_RGB(147,227,255));
		IWIDGET_SetBorderWidth(me->piwListPicSize, 1);
		IWIDGET_SetItemHeight(me->piwListPicSize, 30);
		IWIDGET_SetModel(me->piwListPicSize, (IModel*)me->piListModelPicSize);
	}

	// Use Static widget to draw list items
	if (!nErr) {
		
		IWidget *pisw = 0;
		
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_STATICWIDGET, (void**)&pisw);
		
		if (!nErr) {
			
			IWIDGET_SetLeftPadding(pisw, 1);
			IWIDGET_SetRightPadding(pisw, 1);
			IWIDGET_SetBorderWidth(pisw, 1);
			IWIDGET_SetSelectedBorderColor(pisw, MAKE_RGBA(0,0,255,255));
			//IWIDGET_SetSelectedActiveBGColor(pisw, MAKE_RGBA(51,102,255,255));
			IDECORATOR_SetWidget((IDecorator *)me->piwListPicSize, pisw);
		}
		
		RELEASEIF(pisw);
	}

	// Create the Picture Quality List widget for image selection
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_LISTWIDGET, (void**)&me->piwListPicQuality);
	}
	
	// Configure the Picture Quality List widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwListPicQuality, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
		IWIDGET_SetBGColor(me->piwListPicQuality, MAKE_RGB(147,227,255));
		IWIDGET_SetBorderWidth(me->piwListPicQuality, 1);
		IWIDGET_SetItemHeight(me->piwListPicQuality, 30);
		IWIDGET_SetModel(me->piwListPicQuality, (IModel*)me->piListModelPicQuality);
	}

	// Use Static widget to draw list items
	if (!nErr) {
		
		IWidget *pisw = 0;
		
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_STATICWIDGET, (void**)&pisw);
		
		if (!nErr) {
			
			IWIDGET_SetLeftPadding(pisw, 1);
			IWIDGET_SetRightPadding(pisw, 1);
			IWIDGET_SetBorderWidth(pisw, 1);
			IWIDGET_SetSelectedBorderColor(pisw, MAKE_RGBA(0,0,255,255));
			IDECORATOR_SetWidget((IDecorator *)me->piwListPicQuality, pisw);
		}
		
		RELEASEIF(pisw);
	}
	//////////////////////////////////////////////////////////////////////////

		// Create the BG Image widget
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_IMAGEWIDGET, (void**)&me->piwImage);
	}
	
	// Configure the BG Image widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwImage, me->rcContainer.dx, me->rcContainer.dy - BUTTON_WIDTH);
		//IWIDGET_SetBorderWidth(me->piwImage, 1);
		IWIDGET_SetBGColor(me->piwImage, MAKE_RGB(147,227,255));
	}

	// Create the Upload Image widget
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_IMAGEWIDGET, (void**)&me->piwUpImage);
	}
	
	// Configure the Upload Image widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwUpImage, ALERT_WIDTH, ALERT_WIDTH);
		//IWIDGET_SetBorderWidth(me->piwImage, 1);
	}
	
	// Create the BG Image widget
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_IMAGEWIDGET, (void**)&me->piwFtImage);
	}
	
	// Configure the BG Image widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwFtImage, me->rcContainer.dx, BUTTON_WIDTH);
		//IWIDGET_SetBorderWidth(me->piwFtImage, 1);
	}
	
	// Configure the Progress widget
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_PROGRESSWIDGET, (void**)&me->piwProgress);
	}

	if (!nErr)
	{
		IWIDGET_GetModel(me->piwProgress, AEEIID_VALUEMODEL, (IModel **)&me->piValueModel);
		IVALUEMODEL_SetValue(me->piValueModel, -1, 4, NULL);
	}

	// Configure the Progress widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwProgress, ALERT_WIDTH, BUTTON_WIDTH/2);
		IWIDGET_SetBorderWidth(me->piwProgress, 1);
		IWIDGET_SetBorderColor(me->piwProgress, MAKE_RGB(255,255,255));
		IWIDGET_SetFGColor(me->piwProgress, MAKE_RGB(166,202,240));
	}

	
	//////////////////////////////////////////////////////////////////////////
	// Configure the Text widget
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_STATICWIDGET, (void**)&me->piwTitle);
	}
	
	// Configure the Text widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwTitle, me->rcContainer.dx, TITLE_HEIGHT);
	}


	// Configure the LeftText widget
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_STATICWIDGET, (void**)&me->piwLText);
	}
	
	// Configure the LeftText widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwLText, me->rcContainer.dx/2, TITLE_HEIGHT);
	}

	// Configure the MidText widget
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_STATICWIDGET, (void**)&me->piwMText);
	}
	
	// Configure the MidText widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwMText, me->rcContainer.dx/2, TITLE_HEIGHT);
	}

	// Configure the RightText widget
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_STATICWIDGET, (void**)&me->piwRText);
	}
	
	// Configure the RightText widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwRText, me->rcContainer.dx/2, TITLE_HEIGHT);
	}

	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_STATICWIDGET, (void**)&me->piwExText);
	}
	
	// Configure the Text widget
	if (!nErr) {
		IWIDGET_SetExtentEx(me->piwExText, me->rcContainer.dx, TITLE_HEIGHT);
	}
	//////////////////////////////////////////////////////////////////////////

	// Create the Capture button
	if (!nErr) {
		nErr = CameraApp_CreateButton(me, IDX_CAPTURE, IDB_CAPTURE_SEL);
		me->btnResId[IDX_CAPTURE][0] = IDB_CAPTURE;
		me->btnResId[IDX_CAPTURE][1] = IDB_CAPTURE_SEL;
	}
	
	// Create the Pictute Quality button
	if (!nErr) {
		nErr = CameraApp_CreateButton(me, IDX_PICTURE_QUALITY, IDB_PICTURE_QUALITY);
		me->btnResId[IDX_PICTURE_QUALITY][0] = IDB_PICTURE_QUALITY;
		me->btnResId[IDX_PICTURE_QUALITY][1] = IDB_PICTURE_QUALITY_SEL;
	}
	
	// Create the Pictute Size button
	if (!nErr) {
		nErr = CameraApp_CreateButton(me, IDX_PICTURE_SIZE, IDB_PICTURE_SIZE);
		me->btnResId[IDX_PICTURE_SIZE][0] = IDB_PICTURE_SIZE;
		me->btnResId[IDX_PICTURE_SIZE][1] = IDB_PICTURE_SIZE_SEL;
	}
	
	// Create the Storage button
	if (!nErr) {
		nErr = CameraApp_CreateButton(me, IDX_STORAGE, IDB_STORAGE);
		me->btnResId[IDX_STORAGE][0] = IDB_STORAGE;
		me->btnResId[IDX_STORAGE][1] = IDB_STORAGE_SEL;
	}

	// Create the Folder Button
	if (!nErr) {
		nErr = CameraApp_CreateButton(me, IDX_FOLDER, IDB_FOLDER);
		me->btnResId[IDX_FOLDER][0] = IDB_FOLDER;
		me->btnResId[IDX_FOLDER][1] = IDB_FOLDER_SEL;
	}
	
	return nErr;
}


static int CameraApp_Start(CameraApp *me)
{
	int nErr = -1;
	
	nErr = CameraApp_CreateWidgets(me);
	
	if (!nErr)
	{
		me->bStartSuccess = TRUE;
		DBGPRINTF("CameraApp_CreateWidgets Over!");
		IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwFrame, 0, 0);
		CameraApp_ShowPreview(me);
		DBGPRINTF("ICAMERA_Preview Over!");
	}
	else {
		IImage *			 piImage = NULL;
		IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(32,160,192));

		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_IMAGEWIDGET, (void**)&me->piwErrorImage);
		if (!nErr) {
			IWIDGET_SetExtentEx(me->piwErrorImage, ALERT_WIDTH, ALERT_WIDTH);
			piImage = ISHELL_LoadResImage(me->piShell, RESFILE, IDB_CAMAREA);

			if (piImage) {
				nErr = IWIDGET_SetImage(me->piwErrorImage , piImage);
				IROOTCONTAINER_InsertEx(me->pwcRoot, me->piwErrorImage, ALERT_OFFSET, ALERT_OFFSET);
				RELEASEIF(piImage);
			}
		}	
		
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_STATICWIDGET, (void**)&me->piwErrorText);
		if (!nErr) {
			IWIDGET_SetExtentEx(me->piwErrorText,150, 30);
			CameraApp_ShowText(me, IDS_STRING_CAMERA_ERROR, me->piwErrorText ,85, 200);
		}
		DBGPRINTF("CameraApp_CreateWidgets fail !");
	}
	return nErr;
}


static int CameraApp_Construct(CameraApp *me, IModule * piModule, IShell * piShell)
{
	IBitmap *pb = 0;
	int nErr = 0;
	IDisplay *piDisplay = 0;
	ICanvas *piCanvas = 0;

	me->ia.pvt           = &me->iavt;
	me->iavt.AddRef      = CameraApp_AddRef;
	me->iavt.Release     = CameraApp_Release;
	me->iavt.HandleEvent = CameraApp_HandleEvent;

	me->nRefs = 1;
	me->nMode = MODE_PREVIEWING;
	me->nPreMode = me->nMode;
	me->nRetryCount = 0;
	me->bUploading = FALSE;
	me->bUploadRes = FALSE;
	me->bSnaping = FALSE;
	me->bStartSuccess = FALSE;

	me->piShell = piShell;
	ISHELL_AddRef(piShell);

	me->piModule = piModule;
	IMODULE_AddRef(piModule);

	if (!nErr) {
		nErr = ISHELL_CreateInstance(piShell, AEECLSID_DISPLAY, (void **)&piDisplay);
	}

	if (!nErr) {
		nErr = IDISPLAY_GetDeviceBitmap(piDisplay, &pb);
	}

	if (!nErr) {
		AEEBitmapInfo  biDevice;
		IBITMAP_GetInfo(pb, &biDevice, sizeof(biDevice));
		IBITMAP_Release(pb);
		SETAEERECT(&me->rcContainer, 0, 0, (short) biDevice.cx, (short) biDevice.cy);
		nErr = ISHELL_CreateInstance(piShell, AEECLSID_ROOTCONTAINER, (void*)&me->pwcRoot);
	}

	if (!nErr) {
		nErr = ISHELL_CreateInstance(piShell, AEECLSID_DISPLAYCANVAS, (void**)&piCanvas);
	}

	if (!nErr) {
		nErr = IDISPLAYCANVAS_SetDisplay((IDisplayCanvas*)piCanvas, piDisplay);
	}

	if (!nErr) {
		IROOTCONTAINER_SetCanvas(me->pwcRoot, piCanvas, &me->rcContainer);
		nErr = IROOTCONTAINER_QueryInterface(me->pwcRoot, AEEIID_WIDGET, (void**)&me->piwRoot);
		IWIDGET_SetBGColor(me->piwRoot, MAKE_RGB(25,25,25));
	}

	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_FILEMGR, (void **)&me->piFileMgr);
	}

	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_VECTORMODEL, (void**)&me->piListModel);
	}

	if (!nErr) {
		IVECTORMODEL_SetPfnFree(me->piListModel, WidgetAEEFree);
	}
	
	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_VECTORMODEL, (void**)&me->piListModelPicQuality);
	}

	if (!nErr) {
		IVECTORMODEL_SetPfnFree(me->piListModelPicQuality, WidgetAEEFree);
	}

	if (!nErr) {
		nErr = ISHELL_CreateInstance(me->piShell, AEECLSID_VECTORMODEL, (void**)&me->piListModelPicSize);
	}

	if (!nErr) {
		IVECTORMODEL_SetPfnFree(me->piListModelPicSize, WidgetAEEFree);
	}

	//优先配置保存到SD卡, 如果SD卡不存在则使用文件管理目录	
	if (SUCCESS == IFILEMGR_Test(me->piFileMgr, AEEFS_CARD0_DIR))
	{
		me->eStorageType = STOTAGE_SDCARD;
		SNPRINTF(me->szSnapDir, sizeof(me->szSnapDir), "%simage/My Photo/", AEEFS_CARD0_DIR);
	}
	else if (SUCCESS == IFILEMGR_Test(me->piFileMgr, AEEFS_FILEMGR_DIR))
	{
		me->eStorageType = STOTAGE_PONE;
		SNPRINTF(me->szSnapDir, sizeof(me->szSnapDir), "%simage/My Photo/", AEEFS_FILEMGR_DIR);
	}
	
	
	// Add  picture quality to list model
	if (!nErr) {
		int nErr = 0;
		int nSize = 10, size = 0;
		char szBuf[20] = {""};

		AECHAR *wstr[3];
		wstr[0] = (AECHAR*)MALLOC(nSize);
		wstr[1] = (AECHAR*)MALLOC(nSize);
		wstr[2] = (AECHAR*)MALLOC(nSize);
		
		nErr = ISHELL_LoadResString(me->piShell, RESFILE, IDS_STRING_HIGH, wstr[0], nSize);
		IVECTORMODEL_Add(me->piListModelPicQuality, wstr[0]);
		size = (WSTRLEN(wstr)+6)*sizeof(char);
		WSTRTOSTR(wstr, szBuf, size);

		nErr = ISHELL_LoadResString(me->piShell, RESFILE, IDS_STRING_MIDDLE, wstr[1], nSize);
		IVECTORMODEL_Add(me->piListModelPicQuality, wstr[1]);
		
		nErr = ISHELL_LoadResString(me->piShell, RESFILE, IDS_STRING_LOW, wstr[2], nSize);
		IVECTORMODEL_Add(me->piListModelPicQuality, wstr[2]);
		//FREE(wstr);
	}

	// Add  picture size to list model
	if (!nErr) {
		int nErr = 0;
		int nSize = 20;
		AECHAR *wstr[4];

		wstr[0] = (AECHAR*)MALLOC(nSize);
		wstr[1] = (AECHAR*)MALLOC(nSize);
		wstr[2] = (AECHAR*)MALLOC(nSize);
		wstr[3] = (AECHAR*)MALLOC(nSize);
		
		nErr = ISHELL_LoadResString(me->piShell, RESFILE, IDS_STRING_SIZE1, wstr[0], nSize);
		IVECTORMODEL_Add(me->piListModelPicSize, wstr[0]);
		
		nErr = ISHELL_LoadResString(me->piShell, RESFILE, IDS_STRING_SIZE2, wstr[1], nSize);
		IVECTORMODEL_Add(me->piListModelPicSize, wstr[1]);
		
		nErr = ISHELL_LoadResString(me->piShell, RESFILE, IDS_STRING_SIZE3, wstr[2], nSize);
		IVECTORMODEL_Add(me->piListModelPicSize, wstr[2]);

		nErr = ISHELL_LoadResString(me->piShell, RESFILE, IDS_STRING_SIZE4, wstr[3], nSize);
		IVECTORMODEL_Add(me->piListModelPicSize, wstr[3]);
		//FREE(wstr);
	}
	
	me->scs = CScs_CreateInstance(me->piShell, me);

	DBGPRINTF("CScs_CreateInstance :%x", me->scs);
	if (me->scs == NULL)
	{
		DBGPRINTF("CScs_CreateInstance :%x FAILED", me->scs);
	}
	RELEASEIF(piDisplay);
	RELEASEIF(piCanvas);

	return nErr;
}


// Destroy CameraApp object
//
static void CameraApp_Dtor(CameraApp *me)
{
	int i;

	if (me->scs)
	{
		CScs_Release(me->scs);
		FREE(me->scs);
		me->scs = NULL;
	}

	RELEASEIF(me->piwFrame);
	RELEASEIF(me->piwList);
	RELEASEIF(me->piwListPicSize);
	RELEASEIF(me->piwListPicQuality);
	RELEASEIF(me->piwFtImage);
	RELEASEIF(me->piwProgress);
	RELEASEIF(me->piwTitle);
	RELEASEIF(me->piwLText);
	RELEASEIF(me->piwMText);
	RELEASEIF(me->piwRText);
	RELEASEIF(me->piwExText);
	RELEASEIF(me->piwList);
	RELEASEIF(me->piwListPicSize);
	RELEASEIF(me->piwListPicQuality);
	RELEASEIF(me->piwImage);
	RELEASEIF(me->piwUpImage);
	RELEASEIF(me->piwRoot);
	RELEASEIF(me->pwcRoot);

	RELEASEIF( me->piwErrorImage ) ;
	RELEASEIF( me->piwErrorText );

	RELEASEIF(me->piListModel);
	RELEASEIF(me->piListModelPicQuality);
	RELEASEIF(me->piListModelPicSize);
	RELEASEIF(me->piValueModel);
	RELEASEIF(me->piModel);

	RELEASEIF(me->piShell);
	RELEASEIF(me->piModule);
	RELEASEIF(me->piCamera);
	RELEASEIF(me->piFileMgr);

	for (i=0; i<NUM_BUTTONS; i++) {
		RELEASEIF(me->piwButtons[i]);
	}
}


static uint32 CameraApp_AddRef(IApplet *po)
{
	CameraApp *me = (CameraApp *)po;
	return ++me->nRefs;
}


static uint32 CameraApp_Release(IApplet *po)
{
	CameraApp *me = (CameraApp *)po;
	uint32 nRefs = --me->nRefs;
	int i = 0;

	if (nRefs == 0) {
		CameraApp_Dtor(me);
		FREE(me);
		me = NULL;
	}
	return nRefs;
}


#include "xmod.h"

int xModule_CreateInstance(IModule *module, IShell *shell, AEECLSID idClass, void **pp)
{
	//DBGPRINTF("%s in", __FUNCTION__);
	if (idClass == AEECLSID_CAPTURE) {
		CameraApp *me = MALLOCREC(CameraApp);

		if (me == 0) {
			return ENOMEMORY;
		} else {
			*pp = &me->ia;
			return CameraApp_Construct(me, module, shell);
		}
	}

	*pp = 0;
	return ECLASSNOTSUPPORT;
}


