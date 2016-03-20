/*===========================================================================

FILE: mytext.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEMenu.h"
#include "AEEText.h"
#include "AEEStdLib.h"

#include "mytext.bid"
#include "mytext_res.h"

#define ID_SINGLE         1
#define ID_MULTI          2

#define ID_DISPLAY        3
#define ID_OK             4
#define ID_INPUTMODE      5

#define STATUS_MENU       6
#define STATUS_SINGLE     7
#define STATUS_MULTI      8
#define STATUS_INPUTMENU  9
 
#define FOCUS_TEXT1       10
#define FOCUS_TEXT2       11

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
typedef struct _mytext {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface

    // add your own variables here...
	IMenuCtl      *pMenu;
	ITextCtl      *pSingleText;
	IMenuCtl      *pSoftkey1;

	ITextCtl      *pMultiText1;
	AECHAR        *pContent1;
	ITextCtl      *pMultiText2;
	AECHAR        *pContent2;
	IMenuCtl      *pSoftkey2;
	IMenuCtl      *pInputMenu;
	uint16         textctl_focus;
	uint16         input_mode;
	uint16         input_flag;
	int            status;

	int m_PreTextInputMode; // 前一个输入法
} mytext;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean mytext_HandleEvent(mytext* pMe, AEEEvent eCode, 
                                             uint16 wParam, uint32 dwParam);
boolean mytext_InitAppData(mytext* pMe);
void    mytext_FreeAppData(mytext* pMe);
static void BuildMainMenu(mytext *pMe);
static void DisplaySingleText(mytext *pMe);
static void DisplayMultiText(mytext *pMe);
static void BuildInputMenu(mytext *pMe);
static void GetContent(mytext *pMe);
static void SetContent(mytext *pMe);
/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;

	if( ClsId == AEECLSID_MYTEXT )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(mytext),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)mytext_HandleEvent,
                          (PFNFREEAPPDATA)mytext_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(mytext_InitAppData((mytext*)*ppObj))
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
static boolean mytext_HandleEvent(mytext* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{ 
	uint16  item;
	AECHAR  input_info[12];

	if(pMe->pMenu && IMENUCTL_HandleEvent(pMe->pMenu,eCode,wParam,dwParam))
		return TRUE;

	if(pMe->pSingleText && ITEXTCTL_HandleEvent(pMe->pSingleText,eCode,wParam,dwParam) )
		return TRUE;

	if(pMe->pSoftkey1 && IMENUCTL_HandleEvent(pMe->pSoftkey1,eCode,wParam,dwParam))
	  return TRUE;

	if(pMe->pMultiText1 && ITEXTCTL_HandleEvent(pMe->pMultiText1,eCode,wParam,dwParam) )
		return TRUE;

	if(pMe->pMultiText2 && ITEXTCTL_HandleEvent(pMe->pMultiText2,eCode,wParam,dwParam) )
		return TRUE;

	if(pMe->pSoftkey2 && IMENUCTL_HandleEvent(pMe->pSoftkey2,eCode,wParam,dwParam))
	  return TRUE;

	if(pMe->pInputMenu && IMENUCTL_HandleEvent(pMe->pInputMenu,eCode,wParam,dwParam))
	  return TRUE;

    switch (eCode) 
	{
        // App is told it is starting up
        case EVT_APP_START:                        
		    // Add your code here...
			BuildMainMenu(pMe);
            return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...

      		return(TRUE);

#if 0
		case EVT_CTL_TEXT_MODECHANGED:
		  DBGPRINTF("EVT_CTL_TEXT_MODECHANGED");
		  if(pMe->input_mode == AEE_TM_SYMBOLS)
		  {
			  if((!pMe->input_flag) && pMe->pSoftkey2)
			  {
				  ITEXTCTL_Redraw(pMe->pMultiText1);
				  ITEXTCTL_Redraw(pMe->pMultiText2);
				  IMENUCTL_Redraw(pMe->pSoftkey2);
				  pMe->input_flag = 1;
		                DBGPRINTF("Redraw for SYMBOLS mode flag");
			  }
			  else
				  pMe->input_flag = 0;
		  }
		  return TRUE;
#endif
		case EVT_CTL_TEXT_MODECHANGED:
		  {
		         int CurTextInputMode;
        		  CurTextInputMode = wParam;
        		  if(CurTextInputMode != pMe->m_PreTextInputMode)
        		  {
        		      DBGPRINTF("EVT_CTL_TEXT_MODECHANGED %d-->%d", pMe->m_PreTextInputMode, CurTextInputMode);
        		      if(pMe->m_PreTextInputMode == AEE_TM_SYMBOLS)
        		      {
                		      if(pMe->pMultiText1 && pMe->pMultiText2 && pMe->pSoftkey2)
                		      {
                		          ITEXTCTL_Redraw(pMe->pMultiText1);
                		          ITEXTCTL_Redraw(pMe->pMultiText2);
                			   IMENUCTL_Redraw(pMe->pSoftkey2);
                		      }
        		      }
        		      pMe->m_PreTextInputMode = CurTextInputMode;
        		  }
		  }
		  return TRUE;
		  
        // App is being suspended 
        case EVT_APP_RESUME:
		    // Add your code here...
			switch(pMe->status)
			{
			case STATUS_MENU:
				BuildMainMenu(pMe);
				break;
			case STATUS_SINGLE:
				DisplaySingleText(pMe);
				break;
			case STATUS_MULTI:
				DisplayMultiText(pMe);
				SetContent(pMe);
				if(pMe->textctl_focus == FOCUS_TEXT2)
				{
				  ITEXTCTL_Redraw(pMe->pMultiText1);
				  ITEXTCTL_SetActive(pMe->pMultiText2,TRUE);
				}
				else 
				{
				  ITEXTCTL_Redraw(pMe->pMultiText2);
				  ITEXTCTL_SetActive(pMe->pMultiText1,TRUE);
				}
				break;
			case STATUS_INPUTMENU:
				BuildInputMenu(pMe);
				break;
			}
      		return(TRUE);


        // App is being resumed
		    // Add your code here...
        case EVT_APP_SUSPEND:
			switch(pMe->status)
			{
			case STATUS_MENU:
				IMENUCTL_Release(pMe->pMenu);
				pMe->pMenu = NULL;
				break;
			case STATUS_SINGLE:
				ITEXTCTL_Release(pMe->pSingleText);
				pMe->pSingleText = NULL;
				break;
			case STATUS_MULTI:
				GetContent(pMe);
				ITEXTCTL_Release(pMe->pMultiText1);
				ITEXTCTL_Release(pMe->pMultiText2);
				IMENUCTL_Release(pMe->pSoftkey2);
				pMe->pMultiText1 = NULL;
				pMe->pMultiText2 = NULL;
				pMe->pSoftkey2 = NULL;
				break;
			case STATUS_INPUTMENU:
				IMENUCTL_Release(pMe->pInputMenu);
				pMe->pInputMenu = NULL;
				break;
			}

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
			switch(wParam)
			{
			case AVK_CLR:
				if(pMe->status != STATUS_MENU) 
				{
					switch(pMe->status)
					{
					case STATUS_INPUTMENU:
						IMENUCTL_Release(pMe->pInputMenu);
						pMe->pInputMenu = NULL;
						DisplayMultiText(pMe);
						SetContent(pMe);
						if(pMe->textctl_focus == FOCUS_TEXT2)
						{
						  ITEXTCTL_Redraw(pMe->pMultiText1);
						  ITEXTCTL_SetActive(pMe->pMultiText2,TRUE);
						}
						else 
						{
						  ITEXTCTL_Redraw(pMe->pMultiText2);
						  ITEXTCTL_SetActive(pMe->pMultiText1,TRUE);
						}
						return TRUE;
					case STATUS_SINGLE:
						ITEXTCTL_Release(pMe->pSingleText);
						pMe->pSingleText = NULL;
						IMENUCTL_Release(pMe->pSoftkey1);
						pMe->pSoftkey1 = NULL;
						break;
					case STATUS_MULTI:
						ITEXTCTL_Release(pMe->pMultiText1);
						pMe->pMultiText1 = NULL;
						ITEXTCTL_Release(pMe->pMultiText2);
						pMe->pMultiText2 = NULL;
						IMENUCTL_Release(pMe->pSoftkey2);
						pMe->pSoftkey2 = NULL;
						break;
					}
					pMe->status = STATUS_MENU;
					IMENUCTL_SetActive(pMe->pMenu,TRUE);
					return TRUE;
				}
				else //When the user press clear key and the current page is main menu, the application should exit.
				  return FALSE;
				break;
			case AVK_SELECT:
				if(pMe->status = STATUS_MULTI)
				{
					GetContent(pMe);
					IMENUCTL_SetActive(pMe->pSoftkey2,TRUE);
					return TRUE;
				}
				else
					return FALSE;
			}
			return(TRUE);

		case EVT_COMMAND:
			if(pMe->pMenu && IMENUCTL_IsActive(pMe->pMenu))
			{
				item = IMENUCTL_GetSel (pMe->pMenu);
				switch(item)
				{
				case ID_SINGLE:
				  DisplaySingleText(pMe);
				  break;
				case ID_MULTI:
				  DisplayMultiText(pMe);
				  ITEXTCTL_SetActive(pMe->pMultiText1,TRUE);
				  break;
				default:
				  break;
				}
			}
			else if(pMe->pSoftkey1 && IMENUCTL_IsActive(pMe->pSoftkey1))
			{
				item = IMENUCTL_GetSel(pMe->pSoftkey1);
				switch(item)
				{
				case ID_DISPLAY:
					ITEXTCTL_GetText(pMe->pSingleText,input_info,sizeof(input_info));
					IDISPLAY_DrawText(pMe->pIDisplay,AEE_FONT_BOLD, input_info,-1,0,50,NULL,0);
					IDISPLAY_Update(pMe->pIDisplay);
					break;
				case ID_OK:
					ITEXTCTL_Release(pMe->pSingleText);
					IMENUCTL_Release(pMe->pSoftkey1);
					pMe->pSingleText = NULL;
					pMe->pSoftkey1 = NULL;
					IMENUCTL_SetActive(pMe->pMenu,TRUE);
					pMe->status = STATUS_MENU;
					break;
				default:
					break;
				}
			}
			else if(pMe->pSoftkey2 && IMENUCTL_IsActive(pMe->pSoftkey2))
			{
				item = IMENUCTL_GetSel(pMe->pSoftkey2);
				switch(item)
				{
				case ID_INPUTMODE:
					IMENUCTL_Release(pMe->pSoftkey2);
					pMe->pSoftkey2 = NULL;
					ITEXTCTL_Release(pMe->pMultiText1);
					pMe->pMultiText1 = NULL;
					ITEXTCTL_Release(pMe->pMultiText2);
					pMe->pMultiText2 = NULL;
					BuildInputMenu(pMe);
					break;
				case ID_OK:
					IMENUCTL_Release(pMe->pSoftkey2);
					pMe->pSoftkey2 = NULL;
					ITEXTCTL_Release(pMe->pMultiText1);
					pMe->pMultiText1 = NULL;
					ITEXTCTL_Release(pMe->pMultiText2);
					pMe->pMultiText2 = NULL;
					IMENUCTL_SetActive(pMe->pMenu,TRUE);
					pMe->status = STATUS_MENU;
					break;
				}
			}
			else if(pMe->pInputMenu && IMENUCTL_IsActive(pMe->pInputMenu))
			{
				item = IMENUCTL_GetSel(pMe->pInputMenu);
				pMe->input_mode = item;
				if(pMe->input_mode == AEE_TM_SYMBOLS)
				  pMe->input_flag = 1;
				IMENUCTL_SetActive(pMe->pInputMenu,FALSE);
				IMENUCTL_Release(pMe->pInputMenu);
				pMe->pInputMenu = NULL;
				DisplayMultiText(pMe);
				SetContent(pMe);
				if(pMe->textctl_focus == FOCUS_TEXT2)
				{
				  ITEXTCTL_Redraw(pMe->pMultiText1);
				  ITEXTCTL_SetActive(pMe->pMultiText2,TRUE);
				  ITEXTCTL_SetInputMode(pMe->pMultiText2,pMe->input_mode);
				}
				else 
				{
				  ITEXTCTL_Redraw(pMe->pMultiText2);
				  ITEXTCTL_SetActive(pMe->pMultiText1,TRUE);
				  ITEXTCTL_SetInputMode(pMe->pMultiText1,pMe->input_mode);
				}
				return TRUE;
			}
			return TRUE;

		case EVT_CTL_TAB:
			if(pMe->status == STATUS_MULTI)
			{
				if(ITEXTCTL_IsActive(pMe->pMultiText1))
				{
				  ITEXTCTL_SetActive(pMe->pMultiText1,FALSE);
				  ITEXTCTL_SetActive(pMe->pMultiText2,TRUE);
				}
				else if(ITEXTCTL_IsActive(pMe->pMultiText2))
				{
				  ITEXTCTL_SetActive(pMe->pMultiText2,FALSE);
				  ITEXTCTL_SetActive(pMe->pMultiText1,TRUE);
				}
				return TRUE;
			}
			else
			  return FALSE;

        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}


// this function is called when your application is starting up
boolean mytext_InitAppData(mytext* pMe)
{
    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&(pMe->DeviceInfo));

    // The display and shell interfaces are always created by
    // default, so we'll asign them so that you can access
    // them via the standard "pMe->" without the "a."
    pMe->pIDisplay = pMe->a.m_pIDisplay;
    pMe->pIShell   = pMe->a.m_pIShell;

    // Insert your code here for initializing or allocating resources...
	pMe->pMenu = NULL;
	pMe->pSingleText = NULL;
	pMe->pSoftkey1 = NULL;
	pMe->pMultiText1 = NULL;
	pMe->pMultiText2 = NULL;
	pMe->pSoftkey2 = NULL;
	pMe->pInputMenu = NULL;
	pMe->pContent1 = NULL;
	pMe->pContent2 = NULL;

    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void mytext_FreeAppData(mytext* pMe)
{
    // insert your code here for freeing any resources you have allocated...

    // example to use for releasing each interface:
    // if ( pMe->pIMenuCtl != NULL )         // check for NULL first
    // {
    //    IMENUCTL_Release(pMe->pIMenuCtl)   // release the interface
    //    pMe->pIMenuCtl = NULL;             // set to NULL so no problems trying to free later
    // }
    //
	if(pMe->pMenu)
	{
		IMENUCTL_Release(pMe->pMenu);
		pMe->pMenu = NULL;
	}
	if(pMe->pSingleText)
	{
		ITEXTCTL_Release(pMe->pSingleText);
		pMe->pSingleText = NULL;
	}
	if(pMe->pSoftkey1)
	{
		IMENUCTL_Release(pMe->pSoftkey1);
		pMe->pSoftkey1 = NULL;
	}
	if(pMe->pMultiText1)
	{
		ITEXTCTL_Release(pMe->pMultiText1);
		pMe->pMultiText1 = NULL;
	}
	if(pMe->pMultiText2)
	{
		ITEXTCTL_Release(pMe->pMultiText2);
		pMe->pMultiText2 = NULL;
	}
	if(pMe->pSoftkey2)
	{
		IMENUCTL_Release(pMe->pSoftkey2);
		pMe->pSoftkey2 = NULL;
	}
	if(pMe->pInputMenu)
	{
		IMENUCTL_Release(pMe->pInputMenu);
		pMe->pInputMenu = NULL;
	}
	if(pMe->pContent1)
	{
		FREE(pMe->pContent1);
		pMe->pContent1 = NULL;
	}
	if(pMe->pContent2)
	{
		FREE(pMe->pContent2);
		pMe->pContent2 = NULL;
	}
}

static void BuildMainMenu(mytext *pMe)
{
	if(!pMe->pMenu)
	{
		if(ISHELL_CreateInstance(pMe->pIShell,AEECLSID_MENUCTL,(void**)&pMe->pMenu) != SUCCESS)
			return ;
	}

	IMENUCTL_SetTitle(pMe->pMenu,MYTEXT_RES_FILE,IDS_TITLE,NULL);

	IMENUCTL_AddItem(pMe->pMenu,MYTEXT_RES_FILE,IDS_SINGLE,ID_SINGLE,NULL,0);
	IMENUCTL_AddItem(pMe->pMenu,MYTEXT_RES_FILE,IDS_MULTI,ID_MULTI,NULL,0);

	IMENUCTL_SetActive(pMe->pMenu,TRUE);
	pMe->status = STATUS_MENU;
}

static void DisplaySingleText(mytext *pMe)
{
	AEERect    rect;

	if(!pMe->pSingleText)
	{
		if(ISHELL_CreateInstance(pMe->pIShell,AEECLSID_TEXTCTL,(void**)&pMe->pSingleText) != SUCCESS)
			return;
		if(ISHELL_CreateInstance(pMe->pIShell,AEECLSID_SOFTKEYCTL,(void**)&pMe->pSoftkey1) != SUCCESS)
		{
			ITEXTCTL_Release(pMe->pSingleText);
			pMe->pSingleText = NULL;
			return ;
		}
	}

	IMENUCTL_SetActive(pMe->pMenu,FALSE);
	IDISPLAY_ClearScreen(pMe->pIDisplay);

	ITEXTCTL_SetTitle(pMe->pSingleText,MYTEXT_RES_FILE,IDS_INPUT_INFO,NULL);
	ITEXTCTL_SetProperties(pMe->pSingleText,TP_FRAME);
	ITEXTCTL_SetMaxSize(pMe->pSingleText,5);
	SETAEERECT(&rect,0,20,pMe->DeviceInfo.cxScreen,pMe->DeviceInfo.cyScreen/2);
	ITEXTCTL_SetRect(pMe->pSingleText,&rect);

	ITEXTCTL_SetSoftKeyMenu(pMe->pSingleText,pMe->pSoftkey1);
	ITEXTCTL_SetInputMode(pMe->pSingleText,AEE_TM_FIRST_OEM+1);
	IMENUCTL_AddItem(pMe->pSoftkey1,MYTEXT_RES_FILE,IDS_DISPLAY,ID_DISPLAY,NULL,0);
	IMENUCTL_AddItem(pMe->pSoftkey1,MYTEXT_RES_FILE,IDS_OK,ID_OK,NULL,0);

	ITEXTCTL_SetActive(pMe->pSingleText,TRUE);
	pMe->status = STATUS_SINGLE;
}

static void DisplayMultiText(mytext *pMe)
{
	AEERect       rect;
	int           ascent;
	int           descent;

	pMe->status = STATUS_MULTI;

	if(!pMe->pMultiText1)
	{
		if(ISHELL_CreateInstance(pMe->pIShell,AEECLSID_TEXTCTL,(void**)&pMe->pMultiText1) != SUCCESS)
			return;
		if(ISHELL_CreateInstance(pMe->pIShell,AEECLSID_TEXTCTL,(void**)&pMe->pMultiText2) != SUCCESS)
		{
			ITEXTCTL_Release(pMe->pMultiText1);
			pMe->pMultiText1 = NULL;
			return;
		}
		if(ISHELL_CreateInstance(pMe->pIShell,AEECLSID_SOFTKEYCTL,(void**)&pMe->pSoftkey2) != SUCCESS)
		{
			ITEXTCTL_Release(pMe->pMultiText1);
			pMe->pMultiText1 = NULL;
			ITEXTCTL_Release(pMe->pMultiText2);
			pMe->pMultiText2 = NULL;
			return ;
		}
	}

	IMENUCTL_SetActive(pMe->pMenu,FALSE);
	IDISPLAY_ClearScreen(pMe->pIDisplay);

	IDISPLAY_GetFontMetrics(pMe->pIDisplay,AEE_FONT_NORMAL,&ascent,&descent);

	ITEXTCTL_SetTitle(pMe->pMultiText1,MYTEXT_RES_FILE,IDS_INPUT_INFO,NULL);
	ITEXTCTL_SetProperties(pMe->pMultiText1,TP_FRAME);
	ITEXTCTL_SetMaxSize(pMe->pMultiText1,5);
	SETAEERECT(&rect,0,20,pMe->DeviceInfo.cxScreen,ascent + descent +2);
	ITEXTCTL_SetRect(pMe->pMultiText1,&rect);

	ITEXTCTL_SetTitle(pMe->pMultiText2,MYTEXT_RES_FILE,IDS_TITLE_ADDED,NULL);
	ITEXTCTL_SetProperties(pMe->pMultiText2,TP_FRAME);
	ITEXTCTL_SetMaxSize(pMe->pMultiText2,5);
	SETAEERECT(&rect,0,60,pMe->DeviceInfo.cxScreen,ascent + descent + 2);
	ITEXTCTL_SetRect(pMe->pMultiText2,&rect);

	IMENUCTL_AddItem(pMe->pSoftkey2,MYTEXT_RES_FILE,IDS_INPUTMODE,ID_INPUTMODE,NULL,0);
	IMENUCTL_AddItem(pMe->pSoftkey2,MYTEXT_RES_FILE,IDS_OK,ID_OK,NULL,0);
	IMENUCTL_Redraw(pMe->pSoftkey2);
}

static void BuildInputMenu(mytext *pMe)
{
	AEETextInputMode     i;
	AEETextInputModeInfo info;
	ITextCtl             *pTextCtl;

	if(!pMe->pInputMenu)
	{
		if(ISHELL_CreateInstance(pMe->pIShell,AEECLSID_MENUCTL,(void**)&pMe->pInputMenu) != SUCCESS)
			return ;
	}

	pMe->status = STATUS_INPUTMENU;

	IDISPLAY_ClearScreen(pMe->pIDisplay);
	IMENUCTL_SetTitle(pMe->pInputMenu,MYTEXT_RES_FILE,IDS_INPUTMODE,NULL);
	ISHELL_CreateInstance(pMe->pIShell,AEECLSID_TEXTCTL,(void**)&pTextCtl);

	ITEXTCTL_EnumModeInit(pTextCtl);
	i = ITEXTCTL_EnumNextMode(pTextCtl,&info);
	while(i != AEE_TM_NONE)
	{
		switch(i)
		{
		case AEE_TM_PINYIN:
			IMENUCTL_AddItem(pMe->pInputMenu,MYTEXT_RES_FILE,IDS_PINYIN,AEE_TM_PINYIN,NULL,0);
			break;
		case AEE_TM_STROKE:
			IMENUCTL_AddItem(pMe->pInputMenu,MYTEXT_RES_FILE,IDS_STROKE,AEE_TM_STROKE,NULL,0);
			break;
		case AEE_TM_EZTEXT:
			IMENUCTL_AddItem(pMe->pInputMenu,MYTEXT_RES_FILE,IDS_EZTEXT,AEE_TM_EZTEXT,NULL,0);
			break;
		case AEE_TM_NUMBERS:
			IMENUCTL_AddItem(pMe->pInputMenu,MYTEXT_RES_FILE,IDS_NUMBERS,AEE_TM_NUMBERS,NULL,0);
			break;
		case AEE_TM_LETTERS:
			IMENUCTL_AddItem(pMe->pInputMenu,MYTEXT_RES_FILE,IDS_ENGLISH,AEE_TM_LETTERS,NULL,0);
			break;
		case AEE_TM_SYMBOLS:
			IMENUCTL_AddItem(pMe->pInputMenu,MYTEXT_RES_FILE,IDS_SYMBOLS,AEE_TM_SYMBOLS,NULL,0);
			break;
		case AEE_TM_RAPID:
			IMENUCTL_AddItem(pMe->pInputMenu,MYTEXT_RES_FILE,IDS_RAPID,AEE_TM_RAPID,NULL,0);
			break;
		case AEE_TM_T9:
			IMENUCTL_AddItem(pMe->pInputMenu,MYTEXT_RES_FILE,IDS_T9,AEE_TM_T9,NULL,0);
			break;
		}
		i = ITEXTCTL_EnumNextMode(pTextCtl,&info);
	}
	IMENUCTL_SetActive(pMe->pInputMenu,TRUE);
	ITEXTCTL_Release(pTextCtl);
}

static void GetContent(mytext *pMe)
{
	if(ITEXTCTL_IsActive(pMe->pMultiText1))
		pMe->textctl_focus = FOCUS_TEXT1;
	if(ITEXTCTL_IsActive(pMe->pMultiText2))
		pMe->textctl_focus = FOCUS_TEXT2;

	if(!pMe->pContent1)
		pMe->pContent1 = (AECHAR*)MALLOC(12);
	if(!pMe->pContent1)
	  return;
	ITEXTCTL_GetText(pMe->pMultiText1,pMe->pContent1,6);

	if(!pMe->pContent2)
		pMe->pContent2 = (AECHAR*)MALLOC(12);
	if(!pMe->pContent2)
	  return;
	ITEXTCTL_GetText(pMe->pMultiText2,pMe->pContent2,6);

	ITEXTCTL_SetActive(pMe->pMultiText1,FALSE);
	ITEXTCTL_SetActive(pMe->pMultiText2,FALSE);
}

static void SetContent(mytext *pMe)
{
	if(pMe->pContent1)
	{
		ITEXTCTL_SetText(pMe->pMultiText1,pMe->pContent1,-1);
		ITEXTCTL_SetCursorPos(pMe->pMultiText1,TC_CURSOREND);
	}
	if(pMe->pContent2)
	{
		ITEXTCTL_SetText(pMe->pMultiText2,pMe->pContent2,-1);
		ITEXTCTL_SetCursorPos(pMe->pMultiText2,TC_CURSOREND);
	}
}
