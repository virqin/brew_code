/*==================================================================
=  文件名:CIniFile类定义文件                                       =  
=  主要功能:可以读取.修改变量数值,可以设置新的组,新的变量          =
=  修改日期:2002-12-28                                             = 
=  作者:阿皮                                                       =
=  E_Mail:peijikui@sd365.com  QQ:122281932                         =                            = 
====================================================================*/
#ifndef _CINIFILE_H_
#define _CINIFILE_H_
//用户接口说明:在成员函数SetVarStr和SetVarInt函数中,当iType等于零,则如果用户制定的参数在ini文件中不存在,
//则就写入新的变量.当iType不等于零,则如果用户制定的参数在ini文件中不存在,就不写入新的变量，而是直接返回FALSE;
#include <afxtempl.h>

class CIniFile
{
public:
	CIniFile();
	virtual ~CIniFile();
private:
	CIniFile(const CIniFile &);
	CIniFile & operator = (const CIniFile &);
public:
	//创建函数
	BOOL Create(const CString &strFileName);
	//得到变量整数型数值
	BOOL GetVarInt(const CString &,const CString & ,int &);
	//得到变量字符串型数值
	BOOL GetVarStr(const CString &,const CString & ,CString & );
	//重新设置变量整数型数值
	BOOL SetVarInt(const CString &,const CString & ,const int &,const int iType = 1);
	//重新设置变量字符串型数值
	BOOL SetVarStr(const CString &,const CString &, const CString &,const int iType = 1);
	
private:
	BOOL GetVar(const CString &,const CString &,CString &);
	BOOL SetVar(const CString &,const CString &,const CString &,const int iType = 1);
	int  SearchLine(const CString &,const CString &);

private:
//	vector <CString>  FileContainer;
	CArray <CString,CString> FileContainer;
	BOOL  bFileExsit;
	CStdioFile stfFile;
	CString strInIFileName;
	
};

#endif