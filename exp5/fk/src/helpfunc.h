#ifndef _H_HELPFUNC
#define _H_HELPFUNC



byte    *LoadDataFromFile(IShell * pIShell, char * pszFileName, int * pLen);
boolean IShell_TestFile(IShell * pIShell,char * pszFileName);
void WriteIntoFile(IShell* pIShell,char * pszName,byte * pData,uint32 nLen);

#endif