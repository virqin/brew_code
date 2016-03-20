/*
*  �ļ����ƣ�helpfunc.cpp

*  ժҪ��
		����ļ�������һЩ���õĶ��������ֺ�����

*  �汾��1.0.0
*  ���ߣ�����è 
*  ������ڣ�2008��8��21�� 
* 
*  ȡ���汾��N/A 
*  ԭ����  ��N/A 
*  ������ڣ�N/A 

*/ 


#include "fk.h"

///////////////////////////////
//����ʵ�ֲ���


/*
*	��������: 
		 WriteIntoFile(IShell* pIShell,char * pszName,byte * pData,uint32 nLen)
*  ���ܽ���:
		������ΪpszName���ļ���д��pDataָ��Ļ������е�nLen���ֽڵ����ݡ�
		��ָ���ļ��Ѵ��ڣ���ɾ�����½���
*  ���������   
		pszName:ָ���ļ�����
		pData:���ݻ�����
		nLen:��д����ֽ���
*  ���������  
		�� 
*  ����ֵ��   
		��
*	���ܳ��ֵ����⣺
		ʣ����ܲ��㽫����д��ʧ��
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
*	��������: 
		LoadDataFromFile(IShell * pIShell, char * pszFileName, int * pLen)
*  ���ܽ���:
		��ȡ����ΪpszFileName���ļ��е�������һ����������
*  ���������   
		pszFileName:ָ���ļ�����
*  ���������  
		pLen:������ȡ�ֽ����Ļ�����
*  ����ֵ��   
		ָ�����Ѷ�ȡ���ݵĻ�������ָ��
*	���ܳ��ֵ����⣺
		�ļ����ݹ��󣬿��ܵ��»������ռ����ʧ��
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
*	��������: 
		IShell_TestFile(IShell * pIShell,char * pszFileName)
*  ���ܽ���:
		����һ���ļ��Ƿ����
*  ���������   
		pszFileName:ָ���ļ�����
*  ���������  
		�� 
*  ����ֵ��   
		����ָ���ļ��Ƿ����
*	���ܳ��ֵ����⣺
		��
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




