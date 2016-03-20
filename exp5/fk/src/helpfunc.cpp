/*
*  文件名称：helpfunc.cpp

*  摘要：
		这个文件包含了一些常用的独立的助手函数。

*  版本：1.0.0
*  作者：酷酷的猫 
*  完成日期：2008年8月21日 
* 
*  取代版本：N/A 
*  原作者  ：N/A 
*  完成日期：N/A 

*/ 


#include "fk.h"

///////////////////////////////
//函数实现部分


/*
*	函数名称: 
		 WriteIntoFile(IShell* pIShell,char * pszName,byte * pData,uint32 nLen)
*  功能介绍:
		往名称为pszName的文件中写入pData指向的缓冲区中的nLen个字节的内容。
		若指定文件已存在，则删除后新建。
*  输入参数：   
		pszName:指定文件名称
		pData:数据缓冲区
		nLen:欲写入的字节数
*  输出参数：  
		无 
*  返回值：   
		无
*	可能出现的意外：
		剩余空能不足将导致写入失败
*/ 
void WriteIntoFile(IShell* pIShell,char * pszName,byte * pData,uint32 nLen)
{
	IFileMgr   * pMgr       = NULL;
	IFile      * pFile      = NULL;


	if ( SUCCESS != ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void * *) &pMgr) )
	{
		RELEASEIF(pMgr);
		return;
	}
	IFILEMGR_Remove(pMgr, pszName);

	pFile = IFILEMGR_OpenFile(pMgr, pszName, _OFM_CREATE);

	IFILE_Write(pFile, (void *) pData, nLen);
	
	IFILE_Release(pFile);
	RELEASEIF(pMgr);
}

/*
*	函数名称: 
		LoadDataFromFile(IShell * pIShell, char * pszFileName, int * pLen)
*  功能介绍:
		读取名称为pszFileName的文件中的内容至一个缓冲区。
*  输入参数：   
		pszFileName:指定文件名称
*  输出参数：  
		pLen:包含读取字节数的缓冲区
*  返回值：   
		指向存放已读取内容的缓冲区的指针
*	可能出现的意外：
		文件内容过大，可能导致缓冲区空间分配失败
*/ 

byte * LoadDataFromFile(IShell * pIShell, char * pszFileName, int * pLen)
{
   IFile   * pFile   = NULL;
   FileInfo fi;
   byte    * pbtData = NULL;
   IFileMgr * pMgr=NULL;

   ISHELL_CreateInstance(pIShell,AEECLSID_FILEMGR,(void**)&pMgr);
  

   if (NULL!=(pFile=IFILEMGR_OpenFile(pMgr,pszFileName,_OFM_READ)))
   {
      if ( SUCCESS == IFILE_GetInfo(pFile, &fi)  && NULL != (pbtData = (byte *) MALLOC(fi.dwSize + 1)) )
      {
		  IFILE_Read(pFile, pbtData, fi.dwSize);		   
		  *pLen = fi.dwSize;
      }	  
	  RELEASEIF(pFile);  
   }
   RELEASEIF(pMgr);
   return pbtData;
}

/*
*	函数名称: 
		IShell_TestFile(IShell * pIShell,char * pszFileName)
*  功能介绍:
		测试一个文件是否存在
*  输入参数：   
		pszFileName:指定文件名称
*  输出参数：  
		无 
*  返回值：   
		返回指定文件是否存在
*	可能出现的意外：
		无
*/ 
boolean IShell_TestFile(IShell * pIShell,char * pszFileName)
{
	IFileMgr * pFileMgr=NULL;
	boolean br=FALSE;

	if(NULL==pIShell||NULL==pszFileName||
		SUCCESS!=ISHELL_CreateInstance(pIShell,AEECLSID_FILEMGR,(void **)&pFileMgr))
	{
		return FALSE;
	}
	br=(SUCCESS==IFILEMGR_Test(pFileMgr,pszFileName));
	RELEASEIF(pFileMgr);
	return br;	
}




