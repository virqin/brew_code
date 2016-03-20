// EStatic.cpp : implementation file
//

#include "stdafx.h"
#include "EStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEStatic

CEStatic::CEStatic()
{
	// Set default foreground text
	m_crTextColor = ::GetSysColor(COLOR_BTNTEXT);
	// Set default background text
	m_crBkColor = ::GetSysColor(COLOR_BTNFACE);
	// Set default background brush
	m_brBkgnd.CreateSolidBrush(m_crBkColor);
}

CEStatic::~CEStatic()
{
}


BEGIN_MESSAGE_MAP(CEStatic, CStatic)
	//{{AFX_MSG_MAP(CEStatic)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEStatic message handlers

void CEStatic::ReLoadIcon(int nIconID)
{
	if(nIconID == -1)
		return;

	HICON hIcon = AfxGetApp()->LoadIcon(nIconID);

	if( hIcon == NULL )
		return;

	hIcon=SetIcon(hIcon);
	if(hIcon!=NULL)
		::DeleteObject(hIcon);
}

void CEStatic::ReLoadBitmap(int nImageID)
{
	if(nImageID == -1)
		return;

	HBITMAP hBmp = (HBITMAP)::LoadImage( AfxGetInstanceHandle(), 
            MAKEINTRESOURCE(nImageID), IMAGE_BITMAP, 0,0, LR_LOADMAP3DCOLORS );

	if( hBmp == NULL )
		return;

	hBmp = SetBitmap(hBmp);
	if(hBmp != NULL)
		::DeleteObject(hBmp);
}

HBRUSH CEStatic::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// Set foreground color
	pDC->SetTextColor(m_crTextColor);

	// Set background color & brush
	pDC->SetBkColor(m_crBkColor);

	// Return a non-NULL brush if the parent's handler should not be called
    return (HBRUSH)m_brBkgnd;
} // End of CtlColor


void CEStatic::OnDestroy() 
{
	CStatic::OnDestroy();
	
	// Destroy resources
    m_brBkgnd.DeleteObject();
} // End of OnDestroy


void CEStatic::SetTextColor(COLORREF crTextColor)
{
	// Set new foreground color
	if (crTextColor != 0xffffffff)
	{
		m_crTextColor = crTextColor;
	}
	else // Set default foreground color
	{
		m_crTextColor = ::GetSysColor(COLOR_BTNTEXT);
	}

	// Repaint control
	Invalidate();
} // End of SetTextColor


COLORREF CEStatic::GetTextColor()
{
	return m_crTextColor;
} // End of GetTextColor


void CEStatic::SetBkColor(COLORREF crBkColor)
{
	// Set new background color
	if (crBkColor != 0xffffffff)
	{
		m_crBkColor = crBkColor;
	}
	else // Set default background color
	{
		m_crBkColor = ::GetSysColor(COLOR_BTNFACE);
	}

    m_brBkgnd.DeleteObject();
    m_brBkgnd.CreateSolidBrush(m_crBkColor);

	// Repaint control
	Invalidate();
} // End of SetBkColor


COLORREF CEStatic::GetBkColor()
{
	return m_crBkColor;
} // End of GetBkColor
