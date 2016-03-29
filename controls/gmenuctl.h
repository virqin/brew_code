/******************************************************************************
FILE:  GMenuCtl.h

SERVICES:  Header file for IGMENUCTL

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
	List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

       Copyright © 2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
******************************************************************************/

#define CGMENUCTL_MAX_DIGITS           8  // The max number of digits the control can display
#define CGMENUCTL_DECIMAL_PRECISION    2  // The decimal precision of the control's value

#define CTL_TAB_LEFT                    0  // The wParam value of a left key press in an EVT_CTL_TAB event
#define CTL_TAB_RIGHT                   1  // The wParam value of a right key press in an EVT_CTL_TAB event
#define CTL_TAB_UP						2
#define CTL_TAB_DOWN					3

//×Ô¶¨Òå²Ëµ¥Ïî
typedef struct
{
   const AECHAR * pText;         // Text
   IImage *       pImage;        // Image

   const char * pszResImage;
   const char * pszResText;
   uint16 wText;
   uint16 wImage;

   uint16         wItemID;
   uint32         dwData;

} GMenuItem;

typedef struct
{
    RGBVAL        cText;
    RGBVAL        cSelText;
} GMenuColors; 

// Control Structure
typedef struct
{
	IImage            *m_pImageBk;
	IImage            *m_pImageSe;

	//TQueueList        *m_pDataList;

	int               m_Index;
	int               m_startIndex;
	int               m_pageSize;

	int               m_textPos;

	boolean           m_isActive;

	AEERect           m_Rect;
	GMenuColors       m_Colors;

	uint32            m_Properties;
	boolean           m_bIsActive;
} CGMeuCtl;

extern CGMeuCtl*	CGMENUCTL_CreateInstance( IShell* pIShell );
extern boolean      CGMENUCTL_HandleEvent( CGMeuCtl* pCtl, AEEEvent eCode, uint16 wParam, uint32 dwParam );
extern void         CGMENUCTL_SetActive( CGMeuCtl* pCtl, boolean bActive );
extern boolean      CGMENUCTL_IsActive( CGMeuCtl* pCtl );
extern void         CGMENUCTL_Release( CGMeuCtl* pCtl );
extern void         CGMENUCTL_Reset( CGMeuCtl* pCtl );

extern void         CGMENUCTL_Redraw( CGMeuCtl* pCtl );
extern void         CGMENUCTL_SetRect( CGMeuCtl* pCtl, AEERect* pRect );
extern void			CGMENUCTL_GetRect( CGMeuCtl* pCtl, AEERect* pRect );
