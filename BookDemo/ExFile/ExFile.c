/*===========================================================================

FILE: ExFile.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions

#include "AEEFile.h"			// File interface definitions
#include "AEEStdLib.h"
#include "ExFile.bid"
#include "AEEDB.h"

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
typedef struct _ExFile {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface

    // add your own variables here...



} ExFile;
typedef struct _UserInfo
{
	char userName[8];
	int score;
}UserInfo;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean ExFile_HandleEvent(ExFile* pMe, 
                                                   AEEEvent eCode, uint16 wParam, 
                                                   uint32 dwParam);
boolean ExFile_InitAppData(ExFile* pMe);
void    ExFile_FreeAppData(ExFile* pMe);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;

	if( ClsId == AEECLSID_EXFILE )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(ExFile),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)ExFile_HandleEvent,
                          (PFNFREEAPPDATA)ExFile_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(ExFile_InitAppData((ExFile*)*ppObj))
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
===========================================================================*/
static boolean ExFile_HandleEvent(ExFile* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  

    switch (eCode) 
	{
        // App is told it is starting up
        case EVT_APP_START:                        
		    // Add your code here...
					/*{
						IDBMgr *pIDBMgr;
						IDatabase *pIDatabase;
						IDBRecord *pIDBRecord;
						AEEDBField field[2];
						char userName[8]={0};
						AEEDBFieldType fType; 
						AEEDBFieldName fName;
						UserInfo userInfo[10];
						int16 len;
						int score;
						int i;
						int recordNum;
						byte *pData;
						ISHELL_CreateInstance(pMe->pIShell,AEECLSID_DBMGR,(void**)&pIDBMgr);
						pIDatabase = IDBMGR_OpenDatabase(pIDBMgr,"history",TRUE);
						//插入数据
						for(i=0;i<10;i++)
						{
							userName[0] = '0'+i;
							field[0].fName = AEEDBFIELD_FULLNAME;
							field[0].fType = AEEDB_FT_STRING;
							field[0].pBuffer = userName;
							field[0].wDataLen  = sizeof(userName);
							field[1].fName = AEEDBFIELD_TEXT;
							field[1].fType = AEEDB_FT_DWORD;
							score = 10-i;
							field[1].pBuffer = &score;
							field[1].wDataLen = sizeof(score);
							pIDBRecord = IDATABASE_CreateRecord(pIDatabase,field,2);
							IDBRECORD_Release(pIDBRecord);
						}
						//读取数据
						recordNum = IDATABASE_GetRecordCount(pIDatabase);
						IDATABASE_Reset(pIDatabase);
						i= 0;
						while((pIDBRecord = IDATABASE_GetNextRecord(pIDatabase))!=NULL)
						{
							IDBRECORD_Reset(pIDBRecord);
							while((fType = IDBRECORD_NextField(pIDBRecord,&fName,&len))!=AEEDB_FT_NONE)
							{
								switch(fName)
								{
								case AEEDBFIELD_FULLNAME:
									pData = IDBRECORD_GetField(pIDBRecord,&fName,&fType,&len);
									//处理用户名
									MEMCPY(userInfo[i].userName,pData,len);
									break;
								case AEEDBFIELD_TEXT:
									pData = IDBRECORD_GetField(pIDBRecord,&fName,&fType,&len);
									score = *(int*)pData;
									//处理分数
									break;
								}
							}
							IDBRECORD_Release(pIDBRecord);
							++i;
						}
						//保存查询的纪录
						//修改数据排序
						//重新写入记录

						IDATABASE_Reset(pIDatabase);
						while((pIDBRecord = IDATABASE_GetNextRecord(pIDatabase))!=NULL)
						{
							IDBRECORD_Remove(pIDBRecord);
						}
						IDATABASE_Reset(pIDatabase);
						for(i=0;i<10;i++)
						{
							field[0].fName = AEEDBFIELD_FULLNAME;
							field[0].fType = AEEDB_FT_STRING;
							field[0].pBuffer = userInfo[i].userName;
							field[0].wDataLen  = sizeof(userInfo[i].userName);
							field[1].fName = AEEDBFIELD_TEXT;
							field[1].fType = AEEDB_FT_DWORD;
							field[1].pBuffer = &userInfo[i].score;
							field[1].wDataLen = sizeof(userInfo[i].score);
							pIDBRecord = IDATABASE_CreateRecord(pIDatabase,field,2);
							IDBRECORD_Release(pIDBRecord);
						}
						IDATABASE_Release(pIDatabase);
						IDBMGR_Release(pIDBMgr);
					}*/
					{
						IFileMgr *pIFileMgr;
						FileInfo fileInfo;
						IFile *pIFile;
						IAStream *pIAStream;
						byte *pBuffer;
						ISHELL_CreateInstance(pMe->pIShell,AEECLSID_FILEMGR,(void**)&pIFileMgr);
						pIFile = IFILEMGR_OpenFile(pIFileMgr,"test.dat",_OFM_READWRITE);
						IFILE_GetInfo(pIFile,&fileInfo);
						pBuffer = (byte*)MALLOC(fileInfo.dwSize);
						pIAStream = (IAStream*)pIFile;
						IASTREAM_Read(pIAStream,pBuffer,fileInfo.dwSize);
						//分析数据

						FREE(pBuffer);
						IFILE_Release(pIFile);
						IFILEMGR_Release(pIFileMgr);
					}

            return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...

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
        // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
        case EVT_KEY:
		    // Add your code here...

      		return(TRUE);


        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}


// this function is called when your application is starting up
boolean ExFile_InitAppData(ExFile* pMe)
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



    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void ExFile_FreeAppData(ExFile* pMe)
{
    // insert your code here for freeing any resources you have allocated...

    // example to use for releasing each interface:
    // if ( pMe->pIMenuCtl != NULL )         // check for NULL first
    // {
    //    IMENUCTL_Release(pMe->pIMenuCtl)   // release the interface
    //    pMe->pIMenuCtl = NULL;             // set to NULL so no problems trying to free later
    // }
    //

}
/*
						IFileMgr *pIFileMgr;
						FileInfo fileInfo;
						uint32 totalSize;
						int i;
						IFile *pIFile;
						UserInfo userInfo[10];
						ISHELL_CreateInstance(pMe->pIShell,AEECLSID_FILEMGR,(void**)&pIFileMgr);
						IFILEMGR_EnumInit(pIFileMgr,"*.c",FALSE);
						while(IFILEMGR_EnumNext(pIFileMgr,&fileInfo))
						{
							//读取文件信息
						}
						IFILEMGR_GetFreeSpace(pIFileMgr,&totalSize);
						if(IFILEMGR_Test(pIFileMgr,"test.dat")==SUCCESS)
						{
							pIFile = IFILEMGR_OpenFile(pIFileMgr,"test.dat",_OFM_READWRITE);
						}
						else
						{
							pIFile = IFILEMGR_OpenFile(pIFileMgr,"test.dat",_OFM_CREATE);
						}
						MEMSET(userInfo,0,sizeof(userInfo));
						for(i=0;i<10;i++)
						{
							userInfo[i].userName[0] = '0' + i;
							userInfo[i].score = 10-i;
							if(IFILE_Write(pIFile,userInfo[i].userName,sizeof(userInfo[i].userName))!=sizeof(userInfo[i].userName))	
							{
								//写入数据出错
							}
							if(IFILE_Write(pIFile,&userInfo[i].score,sizeof(userInfo[i].score))!=sizeof(userInfo[i].userName))
							{
							}
						}
						
						IFILE_Seek(pIFile,_SEEK_START,0);
						for(i=0;i<10;i++)
						{
							IFILE_Read(pIFile,userInfo[i].userName,sizeof(userInfo[i].userName));
							IFILE_Read(pIFile,&userInfo[i].score,sizeof(userInfo[i].score));
						}
						//修改数据，排序，或者更改，重新写入
						IFILE_Seek(pIFile,_SEEK_START,0);
						for(i=0;i<10;i++)
						{
							if(IFILE_Write(pIFile,userInfo[i].userName,sizeof(userInfo[i].userName))!=sizeof(userInfo[i].userName))	
							{
							}
							if(IFILE_Write(pIFile,&userInfo[i].score,sizeof(userInfo[i].score))!=sizeof(userInfo[i].userName))
							{
							}
						}
						IFILE_Release(pIFile);
						IFILEMGR_Release(pIFileMgr);*/