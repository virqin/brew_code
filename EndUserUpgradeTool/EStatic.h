#if !defined(AFX_ESTATIC_H__83BF90B9_4845_11D4_A7D5_0050DA221C81__INCLUDED_)
#define AFX_ESTATIC_H__83BF90B9_4845_11D4_A7D5_0050DA221C81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEStatic window

class CEStatic : public CStatic
{
// Construction
public:
	CEStatic();

// Attributes
public:
	COLORREF m_crTextColor;
	COLORREF m_crBkColor;
	CBrush m_brBkgnd;

// Operations
public:
	void SetTextColor(COLORREF crTextColor = 0xffffffff);
	COLORREF GetTextColor();
	void SetBkColor(COLORREF crBkColor = 0xffffffff);
	COLORREF GetBkColor();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ReLoadBitmap(int nImageID = -1);
	void ReLoadIcon(int nIconID = -1);
	virtual ~CEStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEStatic)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ESTATIC_H__83BF90B9_4845_11D4_A7D5_0050DA221C81__INCLUDED_)
