/*===========================================================================

FILE: rscpooldemo.c

SERVICES: Sample applet using AEE

DESCRIPTION
  This file contains usage examples of following interfaces. 
  AEECLSID_RANDOM
  ISOURCE

PUBLIC CLASSES:  
   N/A

       Copyright © 2000-2008 Unicom-brew.
                    All Rights Reserved.
                   Unicom-brew
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
// Mandatory includes
#include "AEEAppGen.h"  // AEEApplet declaration
#include "AEE.h"      // Standard AEE Declarations
#include "AEEShell.h" // AEE Shell Services
#include "AEEStdlib.h"    // AEE Display Services
#include "AEEDisp.h"    // AEE Display Services
#include "AEEMenu.h"    // AEE Display Services
#include "AEERscPool.h"    // 
#include "AEEThread.h"    // 

// Services used by app
#include "rscpooldemo.bid"
#include "rscpooldemo.brh"
#include "rscpooldemo.h"

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/
// Mandatory function:
// App Handle Event function
static boolean RscPoolDemoApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// App specific data alloc-init/free functions
static boolean RscPoolDemoApp_InitAppData(IApplet* pMe);
static void RscPoolDemoApp_FreeAppData(IApplet* pMe);

static void BuildMainMenu(RscPoolDemoApp *pMe);
static void RscPoolDemoAppUsage (RscPoolDemoApp *pMe, uint16 wParam);
static void DisplayEvent (RscPoolDemoApp *pMe, uint16 wParam);
static int DisplayOutput(IShell *pIShell, IDisplay *pIDisplay, int nline, char *pszStr, AECHAR *pszwStr, boolean isWideChar);
static void WriteLine (RscPoolDemoApp *pMe, char *pszStr, AECHAR *pszwStr, boolean isWideChar);

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/

// Size of buffer used to hold text strings for display
#define TEXT_BUFFER_SIZE (100 * sizeof(AECHAR))

// App specific constants

#define USAGE_RSC_POOL_MEM         100
#define USAGE_RSC_POOL_IBASE      101
#define USAGE_RSC_POOL_LEAKAGE      102

#define USAGE_ERASE_SCREEN      119   // Special case to erase screen


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
   initialization section every time except for a call to IFile_InitAppData(). This is done as follows:
   IFile_InitAppData() is called to initialize the applet's data. It is app developer's 
   responsibility to fill-in app data initialization code of IFile_InitAppData(). App developer
   is also responsible to release memory allocated for data -- this can be
   done in IFile_FreeAppData().

PROTOTYPE:
  boolean AEEClsCreateInstance(AEECLSID ClsId, IShell* pIShell, IModule* pIModule, 
                      void** ppObj)

PARAMETERS:
  ClsId: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell interface. 

  pIModule: [in]: Contains pointer to the IModule interface to the current module to which
  this app belongs

  ppObj: [out]: On return, *ppObj must point to a valid AEEApplet structure. Allocation
  of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app needs to be loaded and if AEEApplet_New() invocation was successful
  FALSE: If the app does not need to be loaded or if errors occurred in AEEApplet_New().
  If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell* pIShell, IModule* po, void** ppObj)
{
    *ppObj = NULL;
    
    // We want to load this App. So, invoke AEEApplet_New().To it, pass the
    // address of the app-specific handle event function.
    if(ClsId == AEECLSID_RSCPOOLDEMO)
    {
        if (AEEApplet_New(sizeof(RscPoolDemoApp), ClsId, pIShell,po,(IApplet**)ppObj,
            (AEEHANDLER)RscPoolDemoApp_HandleEvent,(PFNFREEAPPDATA)RscPoolDemoApp_FreeAppData) == TRUE)
        {
            if (RscPoolDemoApp_InitAppData((IApplet*)*ppObj))
            {
                return(AEE_SUCCESS);
            }
        }
    }
    return (EFAILED);
}

/*===========================================================================

FUNCTION RscPoolDemoApp_HandleEvent

DESCRIPTION
  This is the EventHandler for this app. All events to this app are handled in this
  function.

PROTOTYPE:
  static boolean App_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
  pi: Pointer to the AEEApplet structure. This structure contains information specific
  to this applet.

  ecode: Specifies the Event sent to this applet

    wParam, dwParam: Event-specific data.

DEPENDENCIES
    none

RETURN VALUE
    TRUE: If the app has processed the event
    FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/

static boolean RscPoolDemoApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    RscPoolDemoApp * pMe = (RscPoolDemoApp*)pi;

    if (pMe->m_pIMenu && IMENUCTL_HandleEvent(pMe->m_pIMenu, eCode, wParam, dwParam))
    {
        return TRUE;
    }


    switch (eCode) 
    {
        case EVT_APP_START:
            // When this applet is started, try to create the main menu
            // that allows the user to select a usage example to execute.
            // If we cannot create an instance of the menu class, exit and
            // indicate that we have handled the event by returning TRUE
            if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MENUCTL, 
                                                (void **)&pMe->m_pIMenu) != SUCCESS)
                return TRUE;

            // Succeeded in obtaining a menu instance pointer, so
            // construct the menu and display it on the screen
            BuildMainMenu(pMe);
            return(TRUE);

        case EVT_APP_STOP:
            return(TRUE);

        case EVT_KEY: 
            // ¿ØÖÆÉÏÏÂ¼üÑ¡Ôñ²Ëµ¥
            if (pMe->m_pIMenu)
            {
                if ((IMENUCTL_IsActive(pMe->m_pIMenu) == FALSE) &&
                   ((wParam == AVK_UP) || (wParam == AVK_DOWN)))
                {
                    IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
                    IMENUCTL_Redraw(pMe->m_pIMenu);
                }

                return IMENUCTL_HandleEvent(pMe->m_pIMenu, EVT_KEY, wParam, 0);
            }
            else
                return FALSE;


        case EVT_NOTIFY:
            break;
            
        case EVT_COMMAND:
            switch(wParam)
            {
                // The following commands are generated by user selections
                // from the main usage app menu.

                case USAGE_RSC_POOL_MEM:
                case USAGE_RSC_POOL_IBASE:
                case USAGE_RSC_POOL_LEAKAGE:

                    // Deactivate main menu to turn off horizontal scrolling of long menu items
                    IMENUCTL_SetActive(pMe->m_pIMenu, FALSE);

                    // Execute the usage example the user has selected
                    RscPoolDemoAppUsage (pMe, wParam);
                    return TRUE;
             
                default:
                    return FALSE;
            } // switch(wPara)
      
        default:
            break;
   } //  switch (eCode) 
   return FALSE;
}


/*===========================================================================

FUNCTION RscPoolDemoApp_InitAppData

DESCRIPTION
   This function initializes app specific data.

PROTOTYPE:
  static boolean RscPoolDemoApp_InitAppData(IApplet* pi);

PARAMETERS:
  pi [in]: Pointer to the IApplet structure.

DEPENDENCIES
    None.

RETURN VALUE
    TRUE: If the app has app data is allocated and initialized successfully
    FALSE: Either app data could not be allocated or initialized

SIDE EFFECTS
    None.
===========================================================================*/
static boolean RscPoolDemoApp_InitAppData(IApplet* pi)
{
   RscPoolDemoApp * pMe = (RscPoolDemoApp*)pi;
   int charHeight, pnAscent, pnDescent;
   AEEDeviceInfo di;

   // Make sure the pointers we'll be using are valid
   if (pMe == NULL || pMe->a.m_pIShell == NULL)
     return FALSE;

   pMe->m_pIMenu = NULL;

   // Determine the amount of available screen space
   ZEROAT(&di);
   di.wStructSize = sizeof(di);
   ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);

   pMe->m_rc.dx = di.cxScreen;
   pMe->m_rc.dy = di.cyScreen;
   // Determine the height of a line of text
   charHeight = IDISPLAY_GetFontMetrics (pMe->a.m_pIDisplay, AEE_FONT_NORMAL,
      &pnAscent, &pnDescent);

   // Number of available lines equals the available screen
   // space divided by the height of a line, minus 3 for the
   // lines we always print at the top and bottom of the screen
   pMe->m_cMaxLine = (di.cyScreen / charHeight) - 3;

   // initialize resource pool
   pMe->m_pIRscPool = NULL;

   return TRUE;
}

/*===========================================================================
   Function
   static void ReleaseIf(IBase **ppif)
  
   Description
   -----------
   releases the interface pointer, sets it to null
  
   Parameters
   ----------
   IBase **ppif: interfacee pointer
  
   Returns
   -------
   nothing
  
===========================================================================*/
static void ReleaseIf(IBase **ppif)
{
    if ((IBase *)0 != *ppif) 
    {
        IBASE_Release(*ppif);
        *ppif = (IBase *)0;
    }
}


/*===========================================================================

FUNCTION RscPoolDemoApp_FreeAppData

DESCRIPTION
    Frees any data and interfaces used by this app.

PROTOTYPE:
  static void RscPoolDemoApp_FreeAppData(IApplet* pi);

PARAMETERS:
  pi [in]: Pointer to the IApplet structure.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
static void RscPoolDemoApp_FreeAppData(IApplet* pi)
{
   RscPoolDemoApp * pMe = (RscPoolDemoApp*)pi;

   ReleaseIf((IBase **)&pMe->m_pIMenu);

   ReleaseIf((IBase **)&pMe->m_pIRscPool);
}

/*===============================================================================
                     APP HELPER FUNCTION DEFINITIONS
=============================================================================== */
/*===========================================================================

FUNCTION: RscPoolDemoAppUsage

DESCRIPTION
    This function executes the usage example selected from the main menu by the
  user.  It takes as input the app data pointer and the ID of the example the
  user selected.  After clearing the screen, the selected example is executed.
  Each example is a self-contained code block that contains any local variables
  needed to execute the example.  Each code block also creates and releases any
  BREW interfaces that are required, and will exit if any errors are detected.  
  Once the example has been executed, this function prints a heading at the top 
  of the screen to indicate which example was selected, 
  and a message at the bottom of the screen instructing the user how to return to
  the main menu to execute another example.
    
PROTOTYPE:
   static void IFileUsage(RscPoolDemoApp * pMe, uint16 wParam)

PARAMETERS:
   pMe:   [in]: Contains a pointer to the usage app data structure.
   wParam: [in]: Identifier of the selected example (example IDs are specified when the
                 main usage app menu is created).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void RscPoolDemoAppUsage (RscPoolDemoApp * pMe, uint16 wParam)
{

    
   // Make sure the pointers we'll be using are valid
   if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->a.m_pIDisplay == NULL)
     return;

   // Erase screen first for display output purposes.
   DisplayEvent (pMe, USAGE_ERASE_SCREEN);
  IMENUCTL_SetProperties(pMe->m_pIMenu, MP_NO_REDRAW);
  IMENUCTL_SetActive(pMe->m_pIMenu, FALSE);

   // Initialize line number where we will start the output 
   // of each test
   pMe->m_cLineNum = 1;

   switch (wParam)
   {
        case USAGE_RSC_POOL_MEM:
            {
                int nErr;
                char *mem1 = NULL;
                char *mem2 = NULL;

 
                nErr =ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_RSCPOOL, (void **)&pMe->m_pIRscPool);
                DBGPRINTF("Create RSCPOOL ret %d", nErr);
                if(nErr != AEE_SUCCESS)
                {
                    break;
                }

                mem1 = IRSCPOOL_Malloc(pMe->m_pIRscPool, 128);
                if(mem1==NULL)
                {
                    break;
                }
                
                mem2 = IRSCPOOL_Malloc(pMe->m_pIRscPool, 64);
                if(mem2==NULL)
                {
                    break;
                }
                
                WriteLine(pMe, "IRscPool allocate mem1/2", NULL, FALSE);

                ReleaseIf((IBase **)&pMe->m_pIRscPool);
                WriteLine(pMe, "Release IRscPool", NULL, FALSE);

                // mem1 and mem2 will released automatically
            }
            break;

        case USAGE_RSC_POOL_IBASE:
            {
                int nErr;
                IThread *pIThread1 = NULL;
                IThread *pIThread2 = NULL;

                nErr =ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_RSCPOOL, (void **)&pMe->m_pIRscPool);
                DBGPRINTF("Create RSCPOOL ret %d", nErr);
                if(nErr != AEE_SUCCESS)
                {
                    break;
                }

                nErr = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_THREAD, (void **)&pIThread1 );
                DBGPRINTF("Create THREAD ret %d", nErr);
                if(nErr != AEE_SUCCESS || pIThread1 ==NULL)
                {
                    break;
                }
                
                nErr = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_THREAD, (void **)&pIThread2 );
                DBGPRINTF("Create THREAD ret %d", nErr);
                if(nErr != AEE_SUCCESS || pIThread2 ==NULL)
                {
                    break;
                }

                nErr = IRSCPOOL_HoldRsc(pMe->m_pIRscPool, (IBase *)pIThread1);
                DBGPRINTF("HoldRsc 1 ret %d", nErr);
                if(nErr != AEE_SUCCESS)
                {
                    break;
                }

                nErr = IRSCPOOL_HoldRsc(pMe->m_pIRscPool, (IBase *)pIThread2);
                DBGPRINTF("HoldRsc 2 ret %d", nErr);
                if(nErr != AEE_SUCCESS)
                {
                    break;
                }

                WriteLine(pMe, "IRscPool hold thread1/2", NULL, FALSE);

                ReleaseIf((IBase **)&pMe->m_pIRscPool);

                WriteLine(pMe, "Release IRscPool", NULL, FALSE);

                // pIThread1 and pIThread2 will released automatically
            }
            break;

        case USAGE_RSC_POOL_LEAKAGE:
            {
                char *mem1 = NULL;
                WriteLine(pMe, "mem allocate but no free", NULL, FALSE);

                // mem1 allocate
                mem1 = MALLOC(512);
                
            }
            break;
            
        default:
             return;
    }
            
    // Display above event. 
    DisplayEvent (pMe, wParam);

    return;
}

/*===========================================================================

FUNCTION: DisplayEvent

DESCRIPTION
    This function prints a heading at the top 
  of the screen to indicate which usage example was selected by the user, 
  It also prints a message on the bottom two lines of the screen instructing 
  the user to press the UP or DOWN key to return to
  the main menu to execute another example.
    
PROTOTYPE:
   static void DisplayEvent(RscPoolDemoApp * pMe, uint16 wParam)

PARAMETERS:
   pMe:   [in]: Contains a pointer to the usage app data structure.
   wParam: [in]: Identifier of the selected example (example IDs are specified when the
                 main usage app menu is created).  If the value USAGE_ERASE_SCREEN is supplied
         instead, this function clears the screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void DisplayEvent (RscPoolDemoApp *pMe, uint16 wParam)
{
  AEERect qrc;
  AEEDeviceInfo di; // Device Info
  AECHAR * szBuf;
  AEERect rc; 

  // Make sure the pointers we'll be using are valid
  if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->a.m_pIDisplay == NULL)
  {
    return;
  }

  if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)
  {
    return;
  }
  
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);
  qrc.x = 0;
  qrc.y = 0;
  qrc.dx = di.cxScreen;
  qrc.dy = di.cyScreen;
  rc.x = 5;
  rc.y = 5;
  rc.dx = di.cxScreen - 5;
  rc.dy = di.cyScreen - 5;

  // Specify text for heading based on example the user has selected
  switch (wParam)
  {
    case USAGE_ERASE_SCREEN:
       IDISPLAY_EraseRect(pMe->a.m_pIDisplay,&qrc);  //Clear the display
       FREE(szBuf);
       return;

    case USAGE_RSC_POOL_MEM:
       STR_TO_WSTR("RSC POOL MEM", szBuf, TEXT_BUFFER_SIZE);
       break; 

    case USAGE_RSC_POOL_IBASE:
       STR_TO_WSTR("RSC POOL IBASE", szBuf, TEXT_BUFFER_SIZE);
       break; 

    case USAGE_RSC_POOL_LEAKAGE:
       STR_TO_WSTR("RSC POOL LEAKAGE", szBuf, TEXT_BUFFER_SIZE);
       break; 

    default:
       STR_TO_WSTR("Unsupported fn", szBuf, TEXT_BUFFER_SIZE);
  }

  // Print the heading at the top of the screen
  IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szBuf, -1,10, 5, &rc,
  IDF_ALIGN_NONE|  IDF_TEXT_TRANSPARENT);

  // Print a message at the bottom of the screen telling the user
  // how to return to the main app menu
  STR_TO_WSTR("Press Up/Down", szBuf, TEXT_BUFFER_SIZE);
  IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szBuf, -1,10, qrc.dy - 25, &rc,
    IDF_ALIGN_NONE | IDF_TEXT_TRANSPARENT);
  STR_TO_WSTR("keys to continue..", szBuf, TEXT_BUFFER_SIZE);
  IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szBuf, -1,10, qrc.dy - 15, &rc,
    IDF_ALIGN_NONE | IDF_TEXT_TRANSPARENT);
  IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,TRUE);

  FREE(szBuf);
}
/*===========================================================================

FUNCTION: BuildMainMenu

DESCRIPTION
    This function constructs the menu control that allows the user to
  select a usage example to execute.  The command ID for each menu item
  is passed to the usage example function above when the user selects the
  menu item for that example.
    
PROTOTYPE:
   static void BuildMainMenu(RscPoolDemoApp * pMe)

PARAMETERS:
   pMe:   [in]: Contains a pointer to the usage app data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void BuildMainMenu(RscPoolDemoApp *pMe)
{
  AEERect qrc;
  AEEDeviceInfo di;
  AECHAR * szBuf;

  // Make sure the pointers we'll be using are valid
  if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->m_pIMenu == NULL)
  {
    return;
  }
  
  if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)
  {
     return;
  }
  
  // Set Title
  STR_TO_WSTR("RSCPOOLDEMO & RSCPOOLDEMOMsg Example", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_SetTitle(pMe->m_pIMenu, NULL, 0, szBuf);

  //Set the Rectangle for the Menu
  ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);
  qrc.x = 0;
  qrc.y = 0;
  qrc.dx = di.cxScreen;
  qrc.dy = di.cyScreen;
  IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);  

  //Add individual entries to the Menu

  // Add SMS Sending to menu
  STR_TO_WSTR("1. RSC POOL MEM", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_RSC_POOL_MEM, szBuf, 0);

  STR_TO_WSTR("2. RSC POOL IBASE", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_RSC_POOL_IBASE, szBuf, 0);

  STR_TO_WSTR("3. RSC POOL LEAKAGE", szBuf, TEXT_BUFFER_SIZE);
  IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_RSC_POOL_LEAKAGE, szBuf, 0);

  // Activate menu
  IMENUCTL_SetActive(pMe->m_pIMenu,TRUE);

  FREE(szBuf);
}
/*===========================================================================

FUNCTION: DisplayOutput

DESCRIPTION
    This function displays an output string at a given line number on the
    screen. If the nline parameter is a negative value (-1) the string
    is displayed in the middle of the screen. If the "nline" value is larger
    than or equal to zero the "nline" value is multiplied by 15 and the 
    resulting value in pixels is set to the y-coordinate of the start of 
    the string display on the screen. If the string does not fit on one line
    the string wraps around to the next line (spaced rougly 10-15 pixels apart).
    By default 5 is used as the starting the x-coordinate of a displayed 
    string.

    How many characters that fit on one line is calculated for each line 
    that is wrapped around to the next line.

    Note: depending on the phone screen size and the fonts used for characters 
          the output might differ on different handsets (devices). Where some 
          handsets will have a smaller screen and large default fonts which will 
          cause partial overlapping of lines. This function does not try to address
          these issues (this is meant as a simple display function).
    
PROTOTYPE:
   int DisplayOutput(IShell *pIShell, IDisplay *pDisplay, int nline, char *pszStr, AECHAR *pszwStr, boolean isWideChar)

PARAMETERS:
   pIShell:   [in]: Contains a pointer to the IShell interface.
   pIDisplay: [in]: Contains a pointer to the IDisplay interface.
   nline:     [in]: Contains the line number to start displaying the text. The line
        numbers are by default spaced 15 pixels apart along the y-axis.
   pszStr:    [in]: The character string to be displayed on the screen.

DEPENDENCIES
  None

RETURN VALUE
  Number of lines written to the screen.

SIDE EFFECTS
  None

===========================================================================*/
static int DisplayOutput(IShell *pIShell, IDisplay *pIDisplay, int nline, char *pszStr, AECHAR *pszwStr, boolean isWideChar)
{
  AEEDeviceInfo di; // Device Info
  AECHAR * szBuf;     // a buffer that supports 200 char string
  AECHAR * psz = NULL;
  int charHeight = 0;      // Stores the char height in pixels for given font
  int pnAscent = 0;        // Stores the ascent in number of pixels
  int pnDescent = 0;       // Stores the descent in number of pixels
  int pixelWidth;
  AEEFont font = AEE_FONT_NORMAL;
  int pnFits = 0, dy;
  int totalCh = 0;
  int numLinesPrinted = 0;

  // Make sure the pointers we'll be using are valid
  if (pIShell == NULL || pIDisplay == NULL)
  {
    return 0;
  }
  
  if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)
  {
    return 0;
  }
  // Get device information
  ISHELL_GetDeviceInfo(pIShell,&di);

  // Get the font metrics info
  charHeight = IDISPLAY_GetFontMetrics (pIDisplay, AEE_FONT_NORMAL,
     &pnAscent, &pnDescent);
  if(isWideChar)
  {
       WSTRCPY(szBuf,pszwStr);
  }
  else
  {
   // Convert to wide string (unicode)
   STR_TO_WSTR ((char *)pszStr, szBuf, TEXT_BUFFER_SIZE);
  }
  // If nlines is zero then print this string starting around the middle of 
  // the screen. Or else multiply nlines by charheight to decide the y coordinate of
  // the start of the string.
  if (nline < 0) {
     dy = di.cyScreen*2/5;
  }
  else{
     dy = nline * charHeight + 5;
  }

  // psz keeps track of the point from which to write from the string buffer
  // in case the string does not fit one line and needs to wrap around in the
  // next line.
  psz = szBuf;
     
  // Need to calculate the lotal string length to decide if any wrapping
  // around is needed.
  if(isWideChar)
  {
       totalCh = 2*WSTRLEN (pszwStr);
  }
  else
  {
       totalCh = STRLEN ((char *)pszStr);
  }

  // Keep displaying text string on multiple lines if the string can't be displayed
  // on one single line. Lines are spaced 15 pixels apart.
  while ((totalCh > 0) && (*psz != NULL))
  { 
     // Get information on how many characters will fit in a line.
     // Give the pointer to the buffer to be displayed, and the number of
     // pixels along the x axis you want to display the string in (max number)
     // pnFits will have the max number of chars that will fit in the maxWidth
     // number of pixels (given string can't fit in one line), or the number of 
     // chars in the string (if it does fit in one line). pnWidth gives the
     // number of pixels that will be used to display pnFits number of chars.
     pixelWidth = IDISPLAY_MeasureTextEx(pIDisplay,
                     font, 
                     (AECHAR *) psz,  // Start of the buffer to display,
                     -1,
                     di.cxScreen - 5, // maxWidth
                     &pnFits);         // Number of chars that will fit a line

     // If pnFits is zero there is something wrong in the input to above function. 
     // Normally this scenario should not occur. But, have the check anyway.
     if (pnFits == 0)
     {
       FREE(szBuf);
       return 0;
     }

     IDISPLAY_DrawText(pIDisplay, AEE_FONT_NORMAL, psz, pnFits, 5 /*start dx*/, 
        dy, 0 /* use default rectangle coordinates */, 0);

     psz += pnFits;      // move pointer to the next segment to be displayed
     totalCh -= pnFits;  // reduce the total number of characters to still display
     dy += charHeight;   // Place next line charHeight pixels below the 
                         // previous line.
     ++numLinesPrinted;

     IDISPLAY_Update(pIDisplay); //, TRUE);
     if (totalCh < pnFits)
        pnFits = totalCh;  // if total number is less than pnFits, adjust pnFits
  }

  FREE(szBuf);

  return numLinesPrinted;   
} // End of DisplayOutput

/*===========================================================================

FUNCTION: WriteLine

DESCRIPTION
    This function displays lines of text on the screen, taking into account
  the number of text lines printed so far and the number of lines available
  for output.

    If all available lines are exhausted, this function returns without doing
  anything.

    If the last available line of the screen is reached, this function prints
  "..." to indicate that some lines may not have been printed due to lack
  of space.

    Otherwise, this function prints the line on the screen by calling DisplayOutput,
  and updates the count of lines printed based on how many lines DisplayOutput
  used to print the text.
 
    
PROTOTYPE:
   static void WriteLine(RscPoolDemoApp * pMe,  char *pszStr, AECHAR *pszwStr, boolean isWideChar)

PARAMETERS:
   pMe:   [in]: Contains a pointer to this app's data structure.
   pszStr:    [in]: The character string to be displayed on the screen.

DEPENDENCIES
  Assumes the pMe->m_cLineNum gets initialized to the starting line for text display
  on the screen, and that pMe->m_cMaxLine contains the last available line for 
  output on the screen.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
static void WriteLine(RscPoolDemoApp * pMe, char *pszStr, AECHAR *pszwStr, boolean isWideChar)
{
   
  // If all available lines used, return
  if (pMe->m_cLineNum > pMe->m_cMaxLine)
    return;

  // If we've reached last available line, print an
  // ellipsis indicator
  if (pMe->m_cLineNum == pMe->m_cMaxLine)
  {
    DisplayOutput(pMe->a.m_pIShell, pMe->a.m_pIDisplay, pMe->m_cLineNum++, "...", NULL, FALSE);
    return;
  }

  // There are lines available for printing.  Print the string passed as
  // input and update the count of lines available
  pMe->m_cLineNum += DisplayOutput(pMe->a.m_pIShell, pMe->a.m_pIDisplay, pMe->m_cLineNum, pszStr, pszwStr, isWideChar);
  return;

} // End of WriteLine


