/*===========================================================================

FILE: addrbookruim.c
ABR: Address Book on Ruim
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEStdLib.h"
#include "AEEKeysConfig.h"
#include "AEEAddrbook.h"

#include "addrbookruim.h"
#include "addrbookruim.bid"
#include "addrbookruim.brh"
#include "abr_function.h"


/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean ABR_HandleEvent(CAddrBookRuim* pMe, AEEEvent eCode, 
                                             uint16 wParam, uint32 dwParam);
boolean ABR_InitAppData(CAddrBookRuim* pMe);
static boolean ABR_CreateMainControls(CAddrBookRuim* pMe);

void    ABR_FreeAppData(CAddrBookRuim* pMe);
static void ABR_ReleaseMainControls(CAddrBookRuim* pMe);
static void ReleaseObj(void ** ppObj);
static void ABR_DisplayMainMenu( CAddrBookRuim* pMe );
static void ABR_DisplayAddScreen( CAddrBookRuim* pMe );
static void ABR_DisplayViewMenu( CAddrBookRuim* pMe );
static void ABR_DisplayEditScreen( CAddrBookRuim* pMe, uint16 wItemID );
static void ABR_DisplayTotalRec( CAddrBookRuim* pMe );
static void ABR_DisplayStoreDevice( CAddrBookRuim* pMe );

static void ABR_Reset(CAddrBookRuim * pMe );

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION
  This function is invoked while the app is being loaded. All Modules must provide this 
  function. Ensure to retain the same name and parameters for this function.
  In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
  that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IDisplay_InitAppData(). 
   This is done as follows: InitAppData() is called to initialize AppletData 
   instance. It is app developers responsibility to fill-in app data initialization 
   code of InitAppData(). App developer is also responsible to release memory 
   allocated for data contained in AppletData -- this can be done in 
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
  clsID: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell object. 

  pIModule: pin]: Contains pointer to the IModule object to the current module to which
  this app belongs

  ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
  of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
  *ppObj = NULL;

  if( ClsId == AEECLSID_ADDRBOOKRUIM )
  {
    // Create the applet and make room for the applet structure
    if( AEEApplet_New(sizeof(CAddrBookRuim),
                      ClsId,
                      pIShell,
                      po,
                      (IApplet**)ppObj,
                      (AEEHANDLER)ABR_HandleEvent,
                      (PFNFREEAPPDATA)ABR_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
    {
      //Initialize applet data, this is called before sending EVT_APP_START
      // to the HandleEvent function
      if(ABR_InitAppData((CAddrBookRuim*)*ppObj))
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


// this function is called when your application is starting up
boolean ABR_InitAppData(CAddrBookRuim* pMe)
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

    // Create all of the common GUI controls that will be used for all
    // interfaces except the Add/Edit and View dialogs (these will be 
    // created and deleted as needed).
    if(!ABR_CreateMainControls(pMe))
    {
      return FALSE;
    }
    pMe->m_nFontHeight = IDISPLAY_GetFontMetrics( pMe->pIDisplay, AEE_FONT_NORMAL, NULL, NULL ) + 1;

    SETAEERECT( &pMe->m_rScreenRect, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen);
    pMe->m_ContactCLS = AEECLSID_ADDRBOOK_RUIM;
    // if there have been no failures up to this point then return success
    return TRUE;
}

/*===========================================================================

FUNCTION: ABR_CreateMainControls

DESCRIPTION:
  Creates controls used by the applet

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
   Assumes pMe is not NULL

RETURN VALUE:
   TRUE - Controls created successfully
   FALSE - Control creation failed

SIDE EFFECTS:
   None
===========================================================================*/
static boolean ABR_CreateMainControls( CAddrBookRuim* pMe )
{
   AEERect rc;
   AEEItemStyle rNormalStyle;
   AEEItemStyle rSelStyle;

   // Create the main application controls.
   if((ISHELL_CreateInstance(pMe->pIShell, AEECLSID_MENUCTL, (void**)(&pMe->m_pMenu)) != SUCCESS) ||
      (ISHELL_CreateInstance(pMe->pIShell, AEECLSID_SOFTKEYCTL, (void**)(&pMe->m_pSK)) != SUCCESS) ||
      (ISHELL_CreateInstance(pMe->pIShell, AEECLSID_TEXTCTL, (void**)(&pMe->m_pText1)) != SUCCESS) ||
      (ISHELL_CreateInstance(pMe->pIShell, AEECLSID_TEXTCTL, (void**)(&pMe->m_pText2)) != SUCCESS ))
      return FALSE;

  // Let's reset text control size so on subsequent this won't misbehave by erasing screen
  SETAEERECT(&rc, 0, 0, 0, 0);
  ITEXTCTL_SetRect(pMe->m_pText1, &rc);
  ITEXTCTL_SetRect(pMe->m_pText2, &rc);

  // Set the style of the menu control so that all the icons are displayed as
  // transparent.  This is set based on the default menu styles.
  ISHELL_GetItemStyle( pMe->pIShell, AEE_IT_MENU, &rNormalStyle, &rSelStyle );
  rNormalStyle.roImage = AEE_RO_TRANSPARENT;
  rSelStyle.roImage = AEE_RO_TRANSPARENT;
  IMENUCTL_SetStyle( pMe->m_pMenu, &rNormalStyle, &rSelStyle );

  return TRUE;
}

// this function is called when your application is exiting
void ABR_FreeAppData(CAddrBookRuim* pMe)
{
    // insert your code here for freeing any resources you have allocated...

    // example to use for releasing each interface:
    // if ( pMe->pIMenuCtl != NULL )         // check for NULL first
    // {
    //    IMENUCTL_Release(pMe->pIMenuCtl)   // release the interface
    //    pMe->pIMenuCtl = NULL;             // set to NULL so no problems trying to free later
    // }
    //
   // Release each of the  common controls.
   ABR_ReleaseMainControls(pMe);

}

/*===========================================================================

FUNCTION: ABR_ReleaseMainControls

DESCRIPTION:
  Frees each of the controls used by the application.

PARAMETERS:
  pMe [in] - Applet Structure

DEPENDENCIES:
   Assumes pMe is not NULL

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void ABR_ReleaseMainControls(CAddrBookRuim* pMe)
{   
   // Release each of the controls
   ReleaseObj((void**)&pMe->m_pMenu);
   ReleaseObj((void**)&pMe->m_pSK);
   ReleaseObj((void**)&pMe->m_pText1);
   ReleaseObj((void**)&pMe->m_pText2);
}

/*===========================================================================

FUNCTION: ReleaseObj

DESCRIPTION:
  This is a convenience function which releases BREW controls.  It also
  checks that the pointer is not NULL before attempting to release the
  control.

PARAMETERS:
  ReleaseObj [in/out] - Pointer to the data pointer of the control.

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void ReleaseObj(void ** ppObj)
{
    if ( ppObj && *ppObj ) 
    {
        (void)IBASE_Release(((IBase *)*ppObj));
        *ppObj = NULL;
    }
}

/*===========================================================================
FUNCTION SampleAppWizard_HandleEvent

DESCRIPTION
  This is the EventHandler for this app. All events to this app are handled in this
  function. All APPs must supply an Event Handler.

PROTOTYPE:
  boolean SampleAppWizard_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
  pi: Pointer to the AEEApplet structure. This structure contains information specific
  to this applet. It was initialized during the AEEClsCreateInstance() function.

  ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean ABR_HandleEvent(CAddrBookRuim* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    const uint16 wButtonIDs[] = {IDC_OK_PROMPT, 0};
    int32 nRet;
    AEEPromptInfo pInfo;
    uint16 wTextID;
      
    // Let each of the controls have a crack at the event.  If one of them is able to handle
    // the event then return TRUE indicating that the event has been handled.
    if (IMENUCTL_HandleEvent(pMe->m_pMenu, eCode, wParam, dwParam))
        return TRUE;
    if (IMENUCTL_HandleEvent(pMe->m_pSK, eCode, wParam, dwParam))
        return TRUE;

    /* text控件处理DOWN键后，后面程序继续处理*/
    if (ITEXTCTL_HandleEvent(pMe->m_pText1, eCode, wParam, dwParam))
    {
        if ( eCode==EVT_KEY && wParam==AVK_DOWN)
        {
            // 不返回TRUE，使后面程序继续处理
        }
        else
        {
            return TRUE;
        }
    }

    /* text控件处理DOWN键后，后面程序继续处理*/
    if (ITEXTCTL_HandleEvent(pMe->m_pText2, eCode, wParam, dwParam))
    {
        if ( eCode==EVT_KEY && wParam==AVK_DOWN)
        {
            // 不返回TRUE，使后面程序继续处理
        }
        else
        {
            return TRUE;
        }
    }

   
    switch (eCode) 
        {
        // App is told it is starting up
        case EVT_APP_START:                        
            // Add your code here...

            ABR_DisplayMainMenu( pMe );
            return(TRUE);

        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...
            return(TRUE);


        // App is being suspended 
        case EVT_APP_SUSPEND:
            // Add your code here...
            // Set all controls to InActive
            IMENUCTL_SetActive( pMe->m_pMenu, FALSE );
            IMENUCTL_SetActive( pMe->m_pSK, FALSE );
            ITEXTCTL_SetActive( pMe->m_pText1, FALSE );
            ITEXTCTL_SetActive( pMe->m_pText2, FALSE );
            return(TRUE);


        // App is being resumed
        case EVT_APP_RESUME:
            // Add your code here...
            switch( pMe->m_eAppState )
            {
                case APP_STATE_MAIN:
                    ABR_DisplayMainMenu( pMe );
                    return TRUE;

                case APP_STATE_ADD:
                    ABR_DisplayAddScreen(pMe);
                    return TRUE;

                case APP_STATE_EDIT:
                    ABR_DisplayEditScreen(pMe, pMe->m_wSelRecId+ABR_REC_LIST_ID);
                    return TRUE;

                case APP_STATE_VIEW:
                    ABR_DisplayViewMenu(pMe);
                    return TRUE;

                case APP_STATE_TOTAL_REC:
                    ABR_DisplayTotalRec( pMe );
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

    case EVT_CTL_SEL_CHANGED:
        // If the record menu is showing and the user changes the selected
        // record in the menu control, get the record id and save it to m_wSelRecId
        if (pMe->m_eAppState == APP_STATE_VIEW)
        {            
            IMenuCtl * pMenu = (IMenuCtl*)dwParam;
            // IMENUCTL_GetSel()得到menu的ID，减去ABR_REC_LIST_ID得到address book record的ID
            pMe->m_wSelRecId = IMENUCTL_GetSel(pMenu) - ABR_REC_LIST_ID;
            return TRUE;
        }
        break;

    // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
    // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
    case EVT_APP_MESSAGE:
       // Add your code here...
  return(TRUE);
        
    case EVT_COMMAND:  // Handle 'SELECT' button events
        switch(wParam)
        {
            case IDC_ADD: // User pressed the 'Add' menu
                ABR_DisplayAddScreen(pMe);
                return TRUE;

            case IDC_VIEW: // User pressed the 'View' menu
                ABR_DisplayViewMenu(pMe);
                return TRUE;

            case IDC_TOTAL_REC: // User pressed the 'Total Rec' menu
                ABR_DisplayTotalRec(pMe);
                return TRUE;

            case IDC_SWITCH_STORE_DEV:
                ABR_DisplayStoreDevice(pMe);
                return TRUE;

            case IDC_ADD_SK: // User pressed the 'Add soft key' menu
                // 只可能在APP_STATE_ADD状态下发生
                if( pMe->m_eAppState == APP_STATE_ADD  && pMe->m_pText1 && pMe->m_pText2 )
                {
                    AECHAR * pTextName=NULL;
                    AECHAR * pTextNum=NULL;
                    pTextName= ITEXTCTL_GetTextPtr( pMe->m_pText1 );
                    pTextNum = ITEXTCTL_GetTextPtr( pMe->m_pText2 );
                    // 将名字和电话作为一个记录增加到address book
                    nRet = ABR_AddContactRec( pMe, pMe->m_ContactCLS, pTextName, pTextNum);
                    if(nRet == AEE_SUCCESS)
                    {
                      wTextID = IDS_PROMPT_TEXT_SUC;
                    }
                    else
                    {
                      wTextID = IDS_PROMPT_TEXT_FAIL;
                    }

                    pInfo.pszRes = ADDRBOOKRUIM_RES_FILE;
                    pInfo.pTitle = NULL;
                    pInfo.pText = NULL;
                    pInfo.wTitleID = 0;
                    pInfo.wTextID = wTextID;
                    pInfo.wDefBtn = IDC_OK_PROMPT;
                    pInfo.pBtnIDs = wButtonIDs;
                    pInfo.dwProps = ST_CENTERTITLE;
                    pInfo.fntTitle = AEE_FONT_BOLD;
                    pInfo.fntText = AEE_FONT_NORMAL;
                    pInfo.dwTimeout = 0;
                    // 显示成功或失败消息，同时prompt  OK按钮
                    // 用户按按钮后产生IDC_OK_PROMPT command
                    ISHELL_Prompt(pMe->pIShell, &pInfo);
                }
                else
                {
                    ABR_DisplayMainMenu(pMe);
                }
                return TRUE;

            case IDC_UPDATE: // User pressed the 'Update' soft key in Edit screen
                if( pMe->m_eAppState == APP_STATE_EDIT  && pMe->m_pText1 && pMe->m_pText2 )
                {
                    AECHAR * pTextName=NULL;
                    AECHAR * pTextNum=NULL;
                    pTextName= ITEXTCTL_GetTextPtr( pMe->m_pText1 );
                    pTextNum = ITEXTCTL_GetTextPtr( pMe->m_pText2 );
                    // 以新的名字(pTextName)和电话(pTextNum)更新电话本中记录id为pMe->m_wSelRecId的记录
                    nRet = ABR_UpdateContactRec( pMe, pMe->m_ContactCLS, pMe->m_wSelRecId, pTextName, pTextNum);
                    if(nRet == AEE_SUCCESS)
                    {
                        wTextID = IDS_PROMPT_TEXT_SUC;
                    }
                    else
                    {
                        wTextID = IDS_PROMPT_TEXT_FAIL;
                    }

                    pInfo.pszRes = ADDRBOOKRUIM_RES_FILE;
                    pInfo.pTitle = NULL;
                    pInfo.pText = NULL;
                    pInfo.wTitleID = 0;
                    pInfo.wTextID = wTextID;
                    pInfo.wDefBtn = IDC_OK_PROMPT;
                    pInfo.pBtnIDs = wButtonIDs;
                    pInfo.dwProps = ST_CENTERTITLE;
                    pInfo.fntTitle = AEE_FONT_BOLD;
                    pInfo.fntText = AEE_FONT_NORMAL;
                    pInfo.dwTimeout = 0;  
                    // 显示成功或失败消息，同时prompt  OK按钮
                    // 用户按按钮后产生IDC_OK_PROMPT command
                    ISHELL_Prompt(pMe->pIShell, &pInfo);
                }
                else
                {
                    ABR_DisplayMainMenu(pMe);
                }

                return TRUE;

            case IDC_DELETE: // User pressed the 'Delete' soft key on Edit screen
                if( pMe->m_eAppState == APP_STATE_EDIT)
                {
                    // 删除电话本中记录id为pMe->m_wSelRecId的记录
                    nRet = ABR_DeleteContactRec( pMe, pMe->m_ContactCLS, pMe->m_wSelRecId);
                    if(nRet == AEE_SUCCESS)
                    {
                        wTextID = IDS_PROMPT_TEXT_SUC;
                    }
                    else
                    {
                         wTextID = IDS_PROMPT_TEXT_FAIL;
                    }

                    pInfo.pszRes = ADDRBOOKRUIM_RES_FILE;
                    pInfo.pTitle = NULL;
                    pInfo.pText = NULL;
                    pInfo.wTitleID = 0;
                    pInfo.wTextID = wTextID;
                    pInfo.wDefBtn = IDC_OK_PROMPT;
                    pInfo.pBtnIDs = wButtonIDs;
                    pInfo.dwProps = ST_CENTERTITLE;
                    pInfo.fntTitle = AEE_FONT_BOLD;
                    pInfo.fntText = AEE_FONT_NORMAL;
                    pInfo.dwTimeout = 0;  
                    ISHELL_Prompt(pMe->pIShell, &pInfo);
                }
                else
                {
                    ABR_DisplayMainMenu(pMe);
                }
                return TRUE;

            case IDC_OK_SK: // User pressed the 'OK' soft key
                ABR_DisplayMainMenu( pMe );
                return TRUE;

            case IDC_OK_PROMPT: // User pressed the 'OK' soft key
                 if( pMe->m_eAppState != APP_STATE_ADD && pMe->m_eAppState != APP_STATE_EDIT )
                 {
                   DBGPRINTF("Wrong condition IDC_OK_SK");
                 }
                 ABR_DisplayMainMenu( pMe );
                return TRUE;

            case IDC_SEL_DEV_HANDSET: // 选择手机中的电话薄
                 if( pMe->m_eAppState == APP_STATE_SWITCH_STORE_DEV)
                 {
                    pMe->m_ContactCLS = AEECLSID_ADDRBOOK;
                 }
                 else
                 {
                   DBGPRINTF("Wrong condition IDC_SEL_DEV_HANDSET");
                 }
                 ABR_DisplayMainMenu( pMe );
                return TRUE;

            case IDC_SEL_DEV_RUIM: // 选择RUIM 中的电话薄
                 if( pMe->m_eAppState == APP_STATE_SWITCH_STORE_DEV)
                 {
                    pMe->m_ContactCLS = AEECLSID_ADDRBOOK_RUIM;
                 }
                 else
                 {
                   DBGPRINTF("Wrong condition IDC_SEL_DEV_RUIM");
                 }
                 ABR_DisplayMainMenu( pMe );
                return TRUE;

            default:
                // If the wParam ID matches the ID of a contact record 
                // load the record and display it on the screen.
                if (wParam >= ABR_REC_LIST_ID)
                {
                    ABR_DisplayEditScreen(pMe, wParam);
                    return TRUE;
                }
                break;
            }         
            break;

        // A key was pressed. Look at the wParam above to see which key was pressed. The key
        // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
        case EVT_KEY:
            // Add your code here...
            switch(wParam)
            {
                case AVK_SOFT2:
                case AVK_CLR:
                    // If the clear button is pressed, back up one screen depending on the
                    // current application state
                    switch (pMe->m_eAppState)
                    {
                        case APP_STATE_ADD:
                        case APP_STATE_VIEW:
                        case APP_STATE_TOTAL_REC:
                            ABR_DisplayMainMenu(pMe);
                            return TRUE;

                        case APP_STATE_EDIT:
                            ABR_DisplayViewMenu(pMe);
                            return TRUE;

                        default:
                            break;
                    }

                case AVK_UP:  // Handle 'UP' button presses if the text control has focus.
                    if(pMe->m_eAppState == APP_STATE_ADD ||pMe->m_eAppState == APP_STATE_EDIT)
                    {
                        // the focus switch from soft key to TEXT2
                        if (IMENUCTL_IsActive(pMe->m_pSK))
                        {
                            IMENUCTL_SetActive(pMe->m_pSK, FALSE);
                            IMENUCTL_Redraw(pMe->m_pSK);

                            ITEXTCTL_SetActive(pMe->m_pText2, TRUE);
                            ITEXTCTL_Redraw(pMe->m_pText2);
                            return TRUE;
                        }

                        // the focus switch from TEXT2 to TEXT1
                        if (ITEXTCTL_IsActive(pMe->m_pText2))
                        {
                            ITEXTCTL_SetActive(pMe->m_pText2, FALSE);
                            ITEXTCTL_Redraw(pMe->m_pText2);

                            ITEXTCTL_SetActive(pMe->m_pText1, TRUE);
                            ITEXTCTL_Redraw(pMe->m_pText1);
                            return TRUE;
                        }
                    }
                    break;

                        
                case AVK_DOWN:  // Handle 'DOwN' button presses if the text control has focus.
                case AVK_SELECT:  // Handle 'SELECT' button presses if the text control has focus.
                    if(pMe->m_eAppState == APP_STATE_ADD ||pMe->m_eAppState == APP_STATE_EDIT)
                    {
                        // the focus switch from TEXT1 to TEXT2
                        if (ITEXTCTL_IsActive(pMe->m_pText1))
                        {
                            ITEXTCTL_SetActive(pMe->m_pText1, FALSE);
                            ITEXTCTL_Redraw(pMe->m_pText1);

                            ITEXTCTL_SetActive(pMe->m_pText2, TRUE);
                            ITEXTCTL_Redraw(pMe->m_pText2);
                            return TRUE;
                        }

                        // the focus switch from TEXT2 to soft key
                        if (ITEXTCTL_IsActive(pMe->m_pText2))
                        {
                            ITEXTCTL_SetActive(pMe->m_pText2, FALSE);
                            ITEXTCTL_Redraw(pMe->m_pText2);

                            IMENUCTL_SetActive(pMe->m_pSK, TRUE);
                            IMENUCTL_Redraw(pMe->m_pSK);
                            return TRUE;
                        }
                    }
                    break;

                case AVK_POUND:  // Handle '#' button presses if the text control has focus.
                    if((pMe->m_eAppState == APP_STATE_ADD ||pMe->m_eAppState == APP_STATE_EDIT)
                        && pMe->m_pText1 && pMe->m_pText2)
                    {
                        IKeysConfig KeysConfig;
                        ITextCtl*     pTextActive;

                        if (ITEXTCTL_IsActive(pMe->m_pText1))
                        {
                            pTextActive = pMe->m_pText1;
                        }
                        else
                        {
                            pTextActive = pMe->m_pText2;
                        }
                        // 变换输入法
                        #if 0
                        switch(pMe->m_nInputMode)
                        {
                            case IMT_NUMBER:  // 数字变小写
                                ITEXTCTL_SetInputMode( pTextActive, AEE_TM_LETTERS );
                                if(!ISHELL_CreateInstance(pMe->pIShell, AEEIID_KEYSCONFIG, (void **)(&pMe->KeysConfig)))
                                {
                                    //IKEYSCONFIG_SetKeyState(&KeysConfig, KB_CAPSLOCK, FALSE);
                                    //IKEYSCONFIG_Release(&KeysConfig);
                                }
                                pMe->m_nInputMode = IMT_LITTLE_CASE_LETTER;
                                break;
                                
                            case IMT_LITTLE_CASE_LETTER:  // 小写变大写
                                if(!ISHELL_CreateInstance(pMe->pIShell, AEEIID_KEYSCONFIG, (void **)&KeysConfig))
                                {
                                    IKEYSCONFIG_SetKeyState(&KeysConfig, KB_CAPSLOCK, TRUE);
                                    IKEYSCONFIG_Release(&KeysConfig);
                                }
                                pMe->m_nInputMode = IMT_BIG_CASE_LETTER;
                                break;

                            case IMT_BIG_CASE_LETTER:  // 大写变汉字
                                if(!ISHELL_CreateInstance(pMe->pIShell, AEEIID_KEYSCONFIG, (void **)&KeysConfig))
                                {
                                    IKEYSCONFIG_SetKeyState(&KeysConfig, KB_CAPSLOCK, FALSE);
                                    IKEYSCONFIG_Release(&KeysConfig);
                                }
                                ITEXTCTL_SetInputMode( pTextActive, AEE_TM_RAPID);
                                pMe->m_nInputMode = IMT_RAPID;
                                break;

                            case IMT_RAPID:  // 汉字 变数字
                                if(!ISHELL_CreateInstance(pMe->pIShell, AEEIID_KEYSCONFIG, (void **)&KeysConfig))
                                {
                                    IKEYSCONFIG_SetKeyState(&KeysConfig, KB_CAPSLOCK, FALSE);
                                    IKEYSCONFIG_Release(&KeysConfig);
                                }
                                ITEXTCTL_SetInputMode(pTextActive, AEE_TM_NUMBERS);
                                pMe->m_nInputMode = IMT_NUMBER;
                                break;

                            default:
                                ITEXTCTL_SetInputMode( pTextActive, AEE_TM_LETTERS );
                                if(!ISHELL_CreateInstance(pMe->pIShell, AEEIID_KEYSCONFIG, (void **)&KeysConfig))
                                {
                                    IKEYSCONFIG_SetKeyState(&KeysConfig, KB_CAPSLOCK, FALSE);
                                    IKEYSCONFIG_Release(&KeysConfig);
                                }
                                pMe->m_nInputMode = IMT_LITTLE_CASE_LETTER;
                                break;
                        }
                        #endif
                        return TRUE;
                    }
                    break;
                default:
                    break;
            }
        }

    return(FALSE);
}


/*===========================================================================

FUNCTION: ET_DisplayMainMenu

DESCRIPTION:
  This function displays the application Main Menu.

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
  Causes the phone display to be updated.
  Set application state to APP_STATE_MAIN.
===========================================================================*/
static void ABR_DisplayMainMenu( CAddrBookRuim* pMe )
{
    // Make sure the pointers we'll be using are valid
    if (pMe == NULL || pMe->pIShell == NULL || pMe->m_pMenu == NULL)
        return;

    ABR_Reset(pMe);

    IMENUCTL_SetRect( pMe->m_pMenu, &pMe->m_rScreenRect );

    // Set Title
    IMENUCTL_SetTitle( pMe->m_pMenu, ADDRBOOKRUIM_RES_FILE, IDS_MAIN_MENU, NULL);
    IMENUCTL_AddItem(pMe->m_pMenu, ADDRBOOKRUIM_RES_FILE, IDC_ADD,  IDC_ADD, NULL, NULL);
    IMENUCTL_AddItem(pMe->m_pMenu, ADDRBOOKRUIM_RES_FILE, IDC_VIEW, IDC_VIEW, NULL, NULL);
    IMENUCTL_AddItem(pMe->m_pMenu, ADDRBOOKRUIM_RES_FILE, IDC_TOTAL_REC, IDC_TOTAL_REC, NULL, NULL);
    IMENUCTL_AddItem(pMe->m_pMenu, ADDRBOOKRUIM_RES_FILE, IDC_SWITCH_STORE_DEV, IDC_SWITCH_STORE_DEV, NULL, NULL);

    IMENUCTL_SetActive( pMe->m_pMenu, TRUE );
    pMe->m_eAppState = APP_STATE_MAIN;
}

/*===========================================================================

FUNCTION: ABR_DisplayAddScreen

DESCRIPTION:
  This function displays the application Add Screen.

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
  Causes the phone display to be updated.
  Set application state to APP_STATE_MAIN.
===========================================================================*/

static void ABR_DisplayAddScreen( CAddrBookRuim* pMe )
{
    uint16 nControlYLoc = 2;
    AECHAR* psTextBuf;
    AEERect rRect;

    // Make sure the pointers we'll be using are valid
    if (pMe == NULL || pMe->pIShell == NULL || pMe->pIDisplay==NULL || pMe->m_pSK == NULL || pMe->m_pText1 == NULL || pMe->m_pText2 == NULL)
        return;

    ABR_Reset(pMe);

    psTextBuf = (AECHAR*) MALLOC( MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

    // display add menu title
    if( psTextBuf )
    {
        ISHELL_LoadResString( pMe->pIShell, ADDRBOOKRUIM_RES_FILE, IDS_ADD_MENU, psTextBuf, MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

        // Set the position of the label's daring rectangle
        SETAEERECT( &rRect, 2, nControlYLoc, IDISPLAY_MeasureText( pMe->pIDisplay, AEE_FONT_BOLD, psTextBuf ), pMe->m_nFontHeight );
        IDISPLAY_DrawText( pMe->pIDisplay, AEE_FONT_BOLD, psTextBuf, -1, 0, 0, &rRect, IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT);
        IDISPLAY_Update( pMe->pIDisplay);
        FREEIF( psTextBuf );
    }


    nControlYLoc += pMe->m_nFontHeight + 3;
    psTextBuf = (AECHAR*) MALLOC( MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

    // display add name head
    if( psTextBuf )
    {
        ISHELL_LoadResString( pMe->pIShell, ADDRBOOKRUIM_RES_FILE, IDC_ADD_NAME, psTextBuf, MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

        // Set the position of the label's daring rectangle
        SETAEERECT( &rRect, 2, nControlYLoc, IDISPLAY_MeasureText( pMe->pIDisplay, AEE_FONT_NORMAL, psTextBuf ), pMe->m_nFontHeight );
        IDISPLAY_DrawText( pMe->pIDisplay, AEE_FONT_NORMAL, psTextBuf, -1, rRect.x, rRect.y, &rRect, IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT );
        IDISPLAY_Update( pMe->pIDisplay);
        FREEIF( psTextBuf );
    }

   
    // Display add name input text
    ITEXTCTL_Reset( pMe->m_pText1 );
    ITEXTCTL_SetProperties( pMe->m_pText1, TP_FRAME );
    // Set the starting X coordinate position of the control and its width (screen width
    // minus the width of the label string).
    rRect.x = 2 + rRect.dx;
    rRect.dx = pMe->m_rScreenRect.dx - rRect.x -2;
    ITEXTCTL_SetRect(pMe->m_pText1, &rRect );
    ITEXTCTL_SetInputMode( pMe->m_pText1, AEE_TM_LETTERS );
    pMe->m_nInputMode = IMT_LITTLE_CASE_LETTER;
    ITEXTCTL_SetMaxSize( pMe->m_pText1, MAX_CONTACT_NAME_SIZE+1 );
    ITEXTCTL_SetActive( pMe->m_pText1, TRUE );
    ITEXTCTL_Redraw( pMe->m_pText1);

    nControlYLoc += pMe->m_nFontHeight + 3;


    psTextBuf = (AECHAR*) MALLOC( MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

    // display add num head
    if( psTextBuf )
    {
        ISHELL_LoadResString( pMe->pIShell, ADDRBOOKRUIM_RES_FILE, IDC_ADD_NUM, psTextBuf, MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

        // Set the position of the label's daring rectangle
        SETAEERECT( &rRect, 2, nControlYLoc, IDISPLAY_MeasureText( pMe->pIDisplay, AEE_FONT_NORMAL, psTextBuf ), pMe->m_nFontHeight );
        IDISPLAY_DrawText( pMe->pIDisplay, AEE_FONT_NORMAL, psTextBuf, -1, rRect.x, rRect.y, &rRect, IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT );
        IDISPLAY_Update( pMe->pIDisplay);
        FREEIF( psTextBuf );
    }
   
    // Display add num input text
    ITEXTCTL_Reset( pMe->m_pText2 );
    ITEXTCTL_SetProperties( pMe->m_pText2, TP_FRAME );
    // Set the starting X coordinate position of the control and its width (screen width
    // minus the width of the label string).
    rRect.x = 2 + rRect.dx;
    rRect.dx = pMe->m_rScreenRect.dx - rRect.x -2;
    ITEXTCTL_SetRect(pMe->m_pText2, &rRect );
    ITEXTCTL_SetInputMode( pMe->m_pText2, AEE_TM_NUMBERS);
    pMe->m_nInputMode = IMT_NUMBER;
    ITEXTCTL_SetMaxSize( pMe->m_pText2, MAX_CONTACT_NUM_SIZE );
    ITEXTCTL_SetActive( pMe->m_pText2, FALSE );
    ITEXTCTL_Redraw( pMe->m_pText2);

    // soft key
    IMENUCTL_AddItem( pMe->m_pSK, ADDRBOOKRUIM_RES_FILE, IDC_ADD_SK, IDC_ADD_SK, NULL, 0 );
    IMENUCTL_Redraw( pMe->m_pSK);

    pMe->m_eAppState = APP_STATE_ADD;
}


/*===========================================================================

FUNCTION: ABR_DisplayEditScreen

DESCRIPTION:
  This function displays the application Edit Menu.

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 
  wItemID[in] - record ID of address book

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
  Causes the phone display to be updated.
  Set application state to APP_STATE_MAIN.
===========================================================================*/

static void ABR_DisplayEditScreen( CAddrBookRuim* pMe, uint16 wItemID )
{
    uint16 nControlYLoc = 2;
    AECHAR* psTextBuf;
    AEERect rRect;
    AECHAR psNameForItemID[MAX_CONTACT_NAME_SIZE+1];
    AECHAR psNumForItemID[MAX_CONTACT_NUM_SIZE+1];

    // Make sure the pointers we'll be using are valid
    if (pMe == NULL || pMe->pIShell == NULL || pMe->pIDisplay==NULL || pMe->m_pSK == NULL || pMe->m_pText1 == NULL || pMe->m_pText2 == NULL)
        return;

    ABR_Reset(pMe);

    // wItemID 是menu的ID，减去ABR_REC_LIST_ID变成其对应的address book record的ID
    // 根据record  ID获得名字和电话号码
    ABR_GetContactFieldByItemID(pMe, pMe->m_ContactCLS, wItemID-ABR_REC_LIST_ID, psNameForItemID,psNumForItemID);

    // display view edit title
    psTextBuf = (AECHAR*) MALLOC( MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );
    if( psTextBuf )
    {
        ISHELL_LoadResString( pMe->pIShell, ADDRBOOKRUIM_RES_FILE, IDS_EDIT_TITLE, psTextBuf, MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

        // Set the position of the label's daring rectangle
        SETAEERECT( &rRect, 2, nControlYLoc, IDISPLAY_MeasureText( pMe->pIDisplay, AEE_FONT_BOLD, psTextBuf ), pMe->m_nFontHeight );
        IDISPLAY_DrawText( pMe->pIDisplay, AEE_FONT_BOLD, psTextBuf, -1, 0, 0, &rRect, IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT);
        IDISPLAY_Update( pMe->pIDisplay);
        FREEIF( psTextBuf );
    }

    nControlYLoc += pMe->m_nFontHeight + 3;
    psTextBuf = (AECHAR*) MALLOC( MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

    // display add name head
    if( psTextBuf )
    {
        ISHELL_LoadResString( pMe->pIShell, ADDRBOOKRUIM_RES_FILE, IDC_ADD_NAME, psTextBuf, MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

        // Set the position of the label's daring rectangle
        SETAEERECT( &rRect, 2, nControlYLoc, IDISPLAY_MeasureText( pMe->pIDisplay, AEE_FONT_NORMAL, psTextBuf ), pMe->m_nFontHeight );
        IDISPLAY_DrawText( pMe->pIDisplay, AEE_FONT_NORMAL, psTextBuf, -1, rRect.x, rRect.y, &rRect, IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT );
        IDISPLAY_Update( pMe->pIDisplay);
        FREEIF( psTextBuf );
    }

    // Display add name input text
    ITEXTCTL_Reset( pMe->m_pText1 );
    ITEXTCTL_SetProperties( pMe->m_pText1, TP_FRAME );
    // Set the starting X coordinate position of the control and its width (screen width
    // minus the width of the label string).
    rRect.x = 2 + rRect.dx;
    rRect.dx = pMe->m_rScreenRect.dx - rRect.x -2;
    ITEXTCTL_SetRect(pMe->m_pText1, &rRect );
    ITEXTCTL_SetInputMode( pMe->m_pText1, AEE_TM_LETTERS );
    pMe->m_nInputMode = IMT_LITTLE_CASE_LETTER;
    ITEXTCTL_SetMaxSize( pMe->m_pText1, MAX_CONTACT_NAME_SIZE+1 );
    ITEXTCTL_SetText( pMe->m_pText1, psNameForItemID, MAX_CONTACT_NAME_SIZE );
    ITEXTCTL_SetActive( pMe->m_pText1, TRUE );
    ITEXTCTL_Redraw( pMe->m_pText1);

    nControlYLoc += pMe->m_nFontHeight + 3;


    psTextBuf = (AECHAR*) MALLOC( MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

    // display add num head
    if( psTextBuf )
    {
        ISHELL_LoadResString( pMe->pIShell, ADDRBOOKRUIM_RES_FILE, IDC_ADD_NUM, psTextBuf, MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

        // Set the position of the label's daring rectangle
        SETAEERECT( &rRect, 2, nControlYLoc, IDISPLAY_MeasureText( pMe->pIDisplay, AEE_FONT_NORMAL, psTextBuf ), pMe->m_nFontHeight );
        IDISPLAY_DrawText( pMe->pIDisplay, AEE_FONT_NORMAL, psTextBuf, -1, rRect.x, rRect.y, &rRect, IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT );
        IDISPLAY_Update( pMe->pIDisplay);
        FREEIF( psTextBuf );
    }
   
    // Display add num input text
    ITEXTCTL_Reset( pMe->m_pText2 );
    ITEXTCTL_SetProperties( pMe->m_pText2, TP_FRAME );
    // Set the starting X coordinate position of the control and its width (screen width
    // minus the width of the label string).
    rRect.x = 2 + rRect.dx;
    rRect.dx = pMe->m_rScreenRect.dx - rRect.x -2;
    ITEXTCTL_SetRect(pMe->m_pText2, &rRect );
    ITEXTCTL_SetInputMode( pMe->m_pText2, AEE_TM_NUMBERS);
    pMe->m_nInputMode = IMT_NUMBER;
    ITEXTCTL_SetMaxSize( pMe->m_pText2, MAX_CONTACT_NUM_SIZE );
    ITEXTCTL_SetText( pMe->m_pText2,psNumForItemID, MAX_CONTACT_NUM_SIZE );
    ITEXTCTL_SetActive( pMe->m_pText2, FALSE );
    ITEXTCTL_Redraw( pMe->m_pText2);

    // soft key
    IMENUCTL_AddItem( pMe->m_pSK, ADDRBOOKRUIM_RES_FILE, IDC_UPDATE, IDC_UPDATE, NULL, 0 );
    IMENUCTL_AddItem( pMe->m_pSK, ADDRBOOKRUIM_RES_FILE, IDC_DELETE, IDC_DELETE, NULL, 0 );
    IMENUCTL_Redraw( pMe->m_pSK);

    pMe->m_eAppState = APP_STATE_EDIT;
}



/*===========================================================================

FUNCTION: ABR_DisplayViewMenu

DESCRIPTION:
  This function displays the application View Menu.

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
  Causes the phone display to be updated.
  Set application state to APP_STATE_MAIN.
===========================================================================*/

static void ABR_DisplayViewMenu( CAddrBookRuim* pMe )
{
    uint32 nTotal = 0; // record count of address book
    AEERect rRect;

    // Make sure the pointers we'll be using are valid
    if (pMe == NULL || pMe->pIShell == NULL || pMe->m_pMenu==NULL )
        return;

    ABR_Reset(pMe);

    SETAEERECT( &rRect, 0, 0, pMe->m_rScreenRect.dx, pMe->m_rScreenRect.dy);
    IMENUCTL_SetRect( pMe->m_pMenu, &rRect );

    // Get contact record and show it in menu item
    ABR_GetContactRec( pMe, pMe->m_ContactCLS, pMe->m_pMenu, &nTotal);

    if(nTotal)
    {
        IMENUCTL_SetTitle(pMe->m_pMenu, ADDRBOOKRUIM_RES_FILE, IDS_VIEW_MENU, NULL);
    }
    else
    {
        IMENUCTL_SetTitle(pMe->m_pMenu, ADDRBOOKRUIM_RES_FILE, IDS_VIEW_NO_REC, NULL);
    }

    // Active Menu
    IMENUCTL_SetActive( pMe->m_pMenu, TRUE);
    pMe->m_wSelRecId = IMENUCTL_GetSel(pMe->m_pMenu) - ABR_REC_LIST_ID;
   
    pMe->m_eAppState = APP_STATE_VIEW;
}

/*===========================================================================

FUNCTION: ABR_DisplayTotalRec

DESCRIPTION:
  This function displays the total record count of address book

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void ABR_DisplayTotalRec( CAddrBookRuim* pMe )
{
    uint16 nControlYLoc = 2;
    AECHAR* psTextBuf=NULL;
    AEERect rRect;
    char* psTextBufAscii=NULL;
    AECHAR* pwsBuf=NULL;

    // Make sure the pointers we'll be using are valid
    if (pMe == NULL || pMe->pIShell == NULL || pMe->pIDisplay==NULL || pMe->m_pSK==NULL)
        return;

    ABR_Reset(pMe);

    psTextBuf = (AECHAR*) MALLOC( MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

    // display total record title
    if( psTextBuf )
    {
        ISHELL_LoadResString( pMe->pIShell, ADDRBOOKRUIM_RES_FILE, IDS_TOTAL_SCREEN_TITLE, psTextBuf, MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

        // Set the position of the label's daring rectangle
        SETAEERECT( &rRect, 2, nControlYLoc, IDISPLAY_MeasureText( pMe->pIDisplay, AEE_FONT_BOLD, psTextBuf ), pMe->m_nFontHeight );
        IDISPLAY_DrawText( pMe->pIDisplay, AEE_FONT_BOLD, psTextBuf, -1, 0, 0, &rRect, IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT);
        IDISPLAY_Update( pMe->pIDisplay);
        FREEIF( psTextBuf );
    }


    nControlYLoc += pMe->m_nFontHeight + 3;
    // display add name head
    psTextBuf = (AECHAR*) MALLOC( MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );
    if( psTextBuf )
    {
        ISHELL_LoadResString( pMe->pIShell, ADDRBOOKRUIM_RES_FILE, IDS_TOTAL_REC_STR, psTextBuf, MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

        psTextBufAscii = (char*) MALLOC( MAX_RES_STRING_BUF_SIZE * sizeof( char ) );
        pwsBuf      = (AECHAR*) MALLOC( MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ) );

        if(psTextBufAscii && pwsBuf)
        {
            // get string of record count
            SPRINTF(psTextBufAscii, "%d", ARB_GetTotalRecNum(pMe, pMe->m_ContactCLS));
            STRTOWSTR(psTextBufAscii, pwsBuf, MAX_RES_STRING_BUF_SIZE * sizeof( AECHAR ));
            WSTRCAT(psTextBuf, pwsBuf);
            // Set the position of the label's daring rectangle
            SETAEERECT( &rRect, 2, nControlYLoc, IDISPLAY_MeasureText( pMe->pIDisplay, AEE_FONT_NORMAL, psTextBuf ), pMe->m_nFontHeight );
            IDISPLAY_DrawText( pMe->pIDisplay, AEE_FONT_NORMAL, psTextBuf, -1, 0, nControlYLoc-2, &rRect, IDF_ALIGN_BOTTOM | IDF_ALIGN_LEFT );
            IDISPLAY_Update( pMe->pIDisplay);
        }
        FREEIF( pwsBuf );
        FREEIF( psTextBufAscii );
        FREEIF( psTextBuf );
    }

    // soft key
    IMENUCTL_AddItem( pMe->m_pSK, ADDRBOOKRUIM_RES_FILE, IDC_OK_SK, IDC_OK_SK, NULL, 0 );
    IMENUCTL_SetActive( pMe->m_pSK, TRUE);
    IMENUCTL_Redraw( pMe->m_pSK);

    pMe->m_eAppState = APP_STATE_TOTAL_REC;
}

/*===========================================================================

FUNCTION: ABR_DisplayStoreDevice

DESCRIPTION:
  This function displays screen of store device switch

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void ABR_DisplayStoreDevice( CAddrBookRuim* pMe )
{
    AEEPromptInfo pInfo;
    const uint16 wButtonIDs[] = {IDC_SEL_DEV_HANDSET, IDC_SEL_DEV_RUIM, 0};

    // Make sure the pointers we'll be using are valid
    if (pMe == NULL || pMe->pIShell == NULL || pMe->pIDisplay==NULL )
        return;

    ABR_Reset(pMe);

    pInfo.pszRes = ADDRBOOKRUIM_RES_FILE;
    pInfo.pTitle = NULL;
    pInfo.pText = NULL;
    pInfo.wTitleID = 0;
    pInfo.wTextID = IDS_SEL_DEV_TITLE;
    pInfo.wDefBtn = IDC_SEL_DEV_HANDSET;
    pInfo.pBtnIDs = wButtonIDs;
    pInfo.dwProps = ST_CENTERTITLE;
    pInfo.fntTitle = AEE_FONT_BOLD;
    pInfo.fntText = AEE_FONT_NORMAL;
    pInfo.dwTimeout = 0;
    // 选择"手机" 还是"RUIM"存储电话薄, 
    // 用户确认后产生IDC_SEL_DEV_HANDSET or IDC_SEL_DEV_RUIM
    ISHELL_Prompt(pMe->pIShell, &pInfo);

    pMe->m_eAppState = APP_STATE_SWITCH_STORE_DEV;
}

/*===========================================================================

FUNCTION: ABR_Reset

DESCRIPTION:
  Reset controls
  
PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void ABR_Reset(CAddrBookRuim * pMe )
{
    // Reset Menu Controls
    IMENUCTL_Reset(pMe->m_pMenu);
    IMENUCTL_Reset( pMe->m_pSK);
   
    ITEXTCTL_SetActive( pMe->m_pText1, FALSE );
    ITEXTCTL_SetActive( pMe->m_pText2, FALSE );

    // Clear Screen
    IDISPLAY_ClearScreen(pMe->pIDisplay);
}



