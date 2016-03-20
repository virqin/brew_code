// SST_F350.h : main header file for the SST_F350 application
//

#if !defined(AFX_SST_F350_H__249F7C06_BC83_454E_AF62_E29414B12EE8__INCLUDED_)
#define AFX_SST_F350_H__249F7C06_BC83_454E_AF62_E29414B12EE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSST_F350App:
// See SST_F350.cpp for the implementation of this class
//

class CSST_F350App : public CWinApp
{
public:

	CSST_F350App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSST_F350App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSST_F350App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SST_F350_H__249F7C06_BC83_454E_AF62_E29414B12EE8__INCLUDED_)
