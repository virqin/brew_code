/*===========================================================================

FILE: myfile.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEStdLib.h"

#include "myfile.bid"
#include "AEEMenu.h"
#include "AEEFILE.h"
#include "myfile.brh"

#define ID_CREATE_FILE   1
#define ID_READ_FILE     2
#define ID_FS_PREFIX     3

#define STATUS_CREATE    5
#define STATUS_READ      6
#define STATUS_MENU      7
#define STATUS_FS_PREFIX     8

#define TEXT_BUFFER_SIZE 200
/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _myfile {
    AEEApplet      a ;         // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface

    // add your own variables here...
    IMenuCtl      *pMenu;
    IFileMgr      *pMyFileMgr;
    int            status;
    int         m_cLineNum;
} myfile;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean myfile_HandleEvent(myfile* pMe, AEEEvent eCode, 
                                             uint16 wParam, uint32 dwParam);
boolean myfile_InitAppData(myfile* pMe);
void    myfile_FreeAppData(myfile* pMe);
static void BuildMainMenu(myfile *pMe);
static void CreateFile(myfile *pMe);
static void ReadFile(myfile  *pMe);
static void FsPrefixTest(myfile  *pMe);

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

    if( ClsId == AEECLSID_MYFILE )
    {
        // Create the applet and make room for the applet structure
        if( AEEApplet_New(sizeof(myfile),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)myfile_HandleEvent,
                          (PFNFREEAPPDATA)myfile_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
        {
            //Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
            if(myfile_InitAppData((myfile*)*ppObj))
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
static boolean myfile_HandleEvent(myfile* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    uint16   item;

    if(pMe->pMenu && IMENUCTL_HandleEvent(pMe->pMenu,eCode,wParam,dwParam))
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


        // App is being suspended 
        case EVT_APP_SUSPEND:
            // Add your code here...
            switch(pMe->status)
            {
                case STATUS_MENU:
                    IMENUCTL_Release(pMe->pMenu);
                    pMe->pMenu = NULL;
                break;
                
                case STATUS_CREATE:
                case STATUS_READ:
                case STATUS_FS_PREFIX:
                    IFILEMGR_Release(pMe->pMyFileMgr);
                    pMe->pMyFileMgr = NULL;
                break;
            }

            return(TRUE);


        // App is being resumed
        case EVT_APP_RESUME:
            // Add your code here...
            switch(pMe->status)
            {
                case STATUS_MENU:
                    BuildMainMenu(pMe);
                break;
                
                case STATUS_CREATE:
                    CreateFile(pMe);
                break;
                
                case STATUS_READ:
                    ReadFile(pMe);
                break;

                case STATUS_FS_PREFIX:
                    FsPrefixTest(pMe);
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
            if((wParam == AVK_CLR) && (pMe->status != STATUS_MENU)) 
            {
                if(pMe->pMyFileMgr)
                {
                    IFILEMGR_Release(pMe->pMyFileMgr);
                }
                pMe->pMyFileMgr = NULL;
                IMENUCTL_Redraw(pMe->pMenu);
                IMENUCTL_SetActive(pMe->pMenu,TRUE);
                pMe->status = STATUS_MENU;
                return TRUE;
            }
            else //When the user press clear key and the current page is main menu, the application should exit.
                return FALSE;

            return(TRUE);

         case EVT_COMMAND:
             item = IMENUCTL_GetSel (pMe->pMenu);
             switch(item)
             {
                case ID_CREATE_FILE:
                  CreateFile(pMe);
                  break;
                  
                case ID_READ_FILE:
                  ReadFile(pMe);
                  break;

                case ID_FS_PREFIX:
                  FsPrefixTest(pMe);
                  break;


                default:
                  break;
             }
             return TRUE;


        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}


// this function is called when your application is starting up
boolean myfile_InitAppData(myfile* pMe)
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
    pMe->pMenu = NULL;
    pMe->pMyFileMgr = NULL;

    pMe->m_cLineNum = 0;
    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void myfile_FreeAppData(myfile* pMe)
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
    if(pMe->pMyFileMgr)
    {
        IFILEMGR_Release(pMe->pMyFileMgr);
        pMe->pMyFileMgr = NULL;
    }
}

static void BuildMainMenu(myfile *pMe)
{
    if(!pMe->pMenu)
    {
        if(ISHELL_CreateInstance(pMe->pIShell,AEECLSID_MENUCTL,(void**)&pMe->pMenu) != SUCCESS)
            return ;
    }

    IMENUCTL_SetTitle(pMe->pMenu,MYFILE_RES_FILE,IDS_TITLE,NULL);

    IMENUCTL_AddItem(pMe->pMenu,MYFILE_RES_FILE,IDS_CREATE_FILE, ID_CREATE_FILE,NULL,0);
    IMENUCTL_AddItem(pMe->pMenu,MYFILE_RES_FILE,IDS_READ_FILE,    ID_READ_FILE,       NULL,0);
    IMENUCTL_AddItem(pMe->pMenu,MYFILE_RES_FILE,IDS_FS_PREFIX,    ID_FS_PREFIX,       NULL,0);

    IMENUCTL_SetActive(pMe->pMenu,TRUE);
    pMe->status = STATUS_MENU;
}

static void CreateFile(myfile *pMe)
{
    IFile   *pMyFile;
    AECHAR  w_str[100];
    char    str[100];
    /*
    AECHAR  str_open_fail[] = {'O','p','e','n',' ','f','i','l','e',' ','f','a','i','l','\0'};
    AECHAR  str_success[] = {'W','r','i','t','e',' ','S','u','c','c','e','s','s','\0'};
    AECHAR  str_fail[] = {'W','r','i','t','e',' ','F','a','i','l','\0'};
    */
    AECHAR  str_open_fail[] = L"Open File Fail";
    AECHAR  str_success[] = L"Write Success";
    AECHAR  str_fail[] = L"Write Fail";
    
    if(ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR,(void **)&pMe->pMyFileMgr))
        return;

    IMENUCTL_SetActive(pMe->pMenu,FALSE);
    pMyFile = IFILEMGR_OpenFile(pMe->pMyFileMgr,"mydir/myfile.txt",_OFM_CREATE);
    if(pMyFile == NULL)
    {
         IDISPLAY_ClearScreen(pMe->pIDisplay);
         IDISPLAY_DrawText(pMe->pIDisplay,    // Display instance
                           AEE_FONT_BOLD,       // Use BOLD font
                           str_open_fail,        // Text - Normally comes from resource
                           -1,                  // -1 = Use full string length
                           0,                   // Ignored - IDF_ALIGN_CENTER
                           0,                   // Ignored - IDF_ALIGN_MIDDLE
                           NULL,                // No clipping
                           IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
         IDISPLAY_Update(pMe->pIDisplay);
         pMe->status = STATUS_CREATE;
         return;
    }
    
    ISHELL_LoadResString(pMe->pIShell,MYFILE_RES_FILE,IDS_FILE_DATA,w_str,sizeof(w_str));
    WSTRTOSTR(w_str,str,sizeof(str));
    if(IFILE_Write(pMyFile,str,STRLEN(str)) == 0)
    {
        IDISPLAY_ClearScreen(pMe->pIDisplay);
         IDISPLAY_DrawText(pMe->pIDisplay,    // Display instance
                           AEE_FONT_BOLD,       // Use BOLD font
                           str_fail,        // Text - Normally comes from resource
                           -1,                  // -1 = Use full string length
                           0,                   // Ignored - IDF_ALIGN_CENTER
                           0,                   // Ignored - IDF_ALIGN_MIDDLE
                           NULL,                // No clipping
                           IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
    }
    else
    {
         IDISPLAY_ClearScreen(pMe->pIDisplay);
         IDISPLAY_DrawText(pMe->pIDisplay,    // Display instance
                           AEE_FONT_BOLD,       // Use BOLD font
                           str_success,        // Text - Normally comes from resource
                           -1,                  // -1 = Use full string length
                           0,                   // Ignored - IDF_ALIGN_CENTER
                           0,                   // Ignored - IDF_ALIGN_MIDDLE
                           NULL,                // No clipping
                           IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
    }
    IDISPLAY_Update(pMe->pIDisplay);
    IFILE_Release(pMyFile);
    pMe->status = STATUS_CREATE;
}

static void ReadFile(myfile  *pMe)
{
    IFile   *pMyFile;
    //AECHAR   str_open_fail[] = {'N','o',' ','S','u','c','h',' ','f','i','l','e','\0'};
    AECHAR   str_open_fail[] = L"No Such File";
    char     buf[100];
    char     *pDest;
    AECHAR  w_buf[100];

    if(ISHELL_CreateInstance(pMe->pIShell, AEECLSID_FILEMGR,(void **)&pMe->pMyFileMgr))
        return;

    IMENUCTL_SetActive(pMe->pMenu,FALSE);
    pMyFile = IFILEMGR_OpenFile(pMe->pMyFileMgr,"mydir/myfile.txt",_OFM_READ);
    if(pMyFile == NULL)
    {
         IDISPLAY_ClearScreen(pMe->pIDisplay);
         IDISPLAY_DrawText(pMe->pIDisplay,    // Display instance
                           AEE_FONT_BOLD,       // Use BOLD font
                           str_open_fail,        // Text - Normally comes from resource
                           -1,                  // -1 = Use full string length
                           0,                   // Ignored - IDF_ALIGN_CENTER
                           0,                   // Ignored - IDF_ALIGN_MIDDLE
                           NULL,                // No clipping
                           IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
         IDISPLAY_Update(pMe->pIDisplay);
         pMe->status = STATUS_READ;
         return;
    }
    
    IFILE_Read(pMyFile,buf,sizeof(buf));
    pDest = STRCHR(buf,'.');
    pDest ++;
    *pDest = 0;
    STRTOWSTR(buf,w_buf,sizeof(w_buf));
    IDISPLAY_ClearScreen(pMe->pIDisplay);
    IDISPLAY_DrawText(pMe->pIDisplay,    // Display instance
                       AEE_FONT_BOLD,       // Use BOLD font
                       w_buf,        // Text - Normally comes from resource
                       -1,                  // -1 = Use full string length
                       0,                   // Ignored - IDF_ALIGN_CENTER
                       0,                   // Ignored - IDF_ALIGN_MIDDLE
                       NULL,                // No clipping
                       IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
    IDISPLAY_Update(pMe->pIDisplay);
    IFILE_Release(pMyFile);
    pMe->status = STATUS_READ;
}

static void WriteLine(myfile * pMe, char *pszStr)
{

   int charHeight = 0;      // Stores the char height in pixels for given font
   int pnAscent = 0;        // Stores the ascent in number of pixels
   int pnDescent = 0;       // Stores the descent in number of pixels
   AECHAR * szBuf;     // a buffer that supports 200 char string

   // Make sure the pointers we'll be using are valid
   if (pMe==NULL || pMe->pIDisplay == NULL || pszStr == NULL)
       return;

   if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)
     return;

   // Get the font metrics info
   charHeight = IDISPLAY_GetFontMetrics (pMe->pIDisplay, AEE_FONT_BOLD, &pnAscent, &pnDescent);

   // Convert to wide string (unicode)
   STR_TO_WSTR ((char *)pszStr, szBuf, TEXT_BUFFER_SIZE);

   IDISPLAY_DrawText(pMe->pIDisplay,    // Display instance
                       AEE_FONT_BOLD,       // Use BOLD font
                       szBuf,        // Text - Normally comes from resource
                       -1,                  // -1 = Use full string length
                       0,                   // Ignored - IDF_ALIGN_CENTER
                       charHeight*pMe->m_cLineNum,                   // Ignored - IDF_ALIGN_MIDDLE
                       NULL,                // No clipping
                       0);

  pMe->m_cLineNum ++;
  FREE(szBuf);
  return;

} // End of WriteLine 

static void FsPrefixTest(myfile  *pMe)
{
    // 1. Usage: Basic File Usage

    // This example illustrates several of the functions in the IFileMgr
    // and IFile interfaces.  We first create a directory and two files within
    // the directory, using IFileMgr's EnumInit and EnumNext functions to verify
    // that the creates succeeded.  We then open one of the files for reading and
    // writing, and write three sentences to it with IFile's Write function.  We then
    // use IFile's Seek and Read functions to read back the second sentence we wrote
    // to the file and echo it to the screen.  We then clean up by deleting the directory
    // and files we created, using IFileMgr's Test function to verify that they no longer
    // exist.

    FileInfo fi;
    IFile * pIFile1, * pIFile2;
    char szFirstStr[]  = "First Sentence";
    char szSecondStr[] = "Second Sentence";
    char szThirdStr[]  = "Third Sentence";
    char szBuffer[30];

    // prepare screen for out text
    IMENUCTL_SetActive(pMe->pMenu,FALSE);
    IDISPLAY_ClearScreen(pMe->pIDisplay);
    pMe->m_cLineNum = 0;

    if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,(void **)&pMe->pMyFileMgr))
        return;

    // Create a new directory.  It will be created as a sub-directory of
    // the directory that contains this app's files.
    if (IFILEMGR_MkDir(pMe->pMyFileMgr, AEEFS_HOME_DIR"xxxdir") != SUCCESS)
    { 
      return;
    }

    // Create two new files in this directory, specifying the directory
    // and file names
    if ((pIFile1 = IFILEMGR_OpenFile(pMe->pMyFileMgr, AEEFS_HOME_DIR"xxxdir/xx1.fil", _OFM_CREATE)) == NULL)
    { 
      return;
    }

    if ((pIFile2 = IFILEMGR_OpenFile(pMe->pMyFileMgr, AEEFS_HOME_DIR"xxxdir/xx2.fil", _OFM_CREATE)) == NULL)
    { 
      IFILE_Release(pIFile1);
      return;
    }

    // To verify that the directory was created, list it
    // (TRUE parm means list directories only)
    IFILEMGR_EnumInit(pMe->pMyFileMgr, AEEFS_HOME_DIR"xxxdir", TRUE);
    while (IFILEMGR_EnumNext(pMe->pMyFileMgr, &fi))
        WriteLine(pMe, fi.szName);

    // To verify that the files were created, list them
    // (FALSE parm means list files only)
    IFILEMGR_EnumInit(pMe->pMyFileMgr, AEEFS_HOME_DIR"xxxdir", FALSE);
    while (IFILEMGR_EnumNext(pMe->pMyFileMgr, &fi))
        WriteLine(pMe, fi.szName);

    // Write the three test sentences to the file we just opened.  IFILE_Write
    // returns the number of bytes it wrote, which we check to make sure the 
    // write operations were successful.
    if (IFILE_Write(pIFile1, szFirstStr, sizeof(szFirstStr)) != sizeof(szFirstStr))
    {
        WriteLine(pMe, "1st Write failed");
    }
    else
    {
        WriteLine(pMe, "1st Write OK");
    }
    
    if (IFILE_Write(pIFile1, szSecondStr, sizeof(szSecondStr)) != sizeof(szSecondStr))
    {
        WriteLine(pMe, "2nd Write failed");
    }
    else
    {
        WriteLine(pMe, "2nd Write OK");
    }
    
    if (IFILE_Write(pIFile1, szThirdStr, sizeof(szThirdStr)) != sizeof(szThirdStr))
    {
        WriteLine(pMe, "3rd Write failed");
    }
    else
    {
        WriteLine(pMe, "3rd Write OK");
    }

    // To verify that we wrote to the file successfully,
    // retrieve its size in bytes using GetInfo.
    IFILE_GetInfo(pIFile1, &fi);
    SPRINTF(szBuffer, "Wrote %d bytes", fi.dwSize); 
    WriteLine(pMe, szBuffer);

    // Read back the second sentence from the file.
    // First, we seek to the start position of the sentence
    // by hopping over the first sentence
    if (IFILE_Seek(pIFile1, _SEEK_START, sizeof(szFirstStr)) == SUCCESS)
    {
        // Read the second sentence and echo it back to the screen
        // (when successful, IFile_Read returns the number of bytes read)
        if (IFILE_Read(pIFile1, szBuffer, sizeof(szSecondStr)) == sizeof(szSecondStr))
          WriteLine(pMe, szBuffer);
    }

    // Clean up--first remove the two files we created
    // (must close them first--second file is still open from when 
    // we created it)
    IFILE_Release(pIFile1);
    if (IFILEMGR_Remove(pMe->pMyFileMgr, AEEFS_HOME_DIR"xxxdir/xx1.fil") != SUCCESS)
    {
      WriteLine(pMe, "1st Remove Failed");
    }
    else
    {
      WriteLine(pMe, "1st Remove OK");
    }

    IFILE_Release(pIFile2);
    if (IFILEMGR_Remove(pMe->pMyFileMgr, AEEFS_HOME_DIR"xxxdir/xx2.fil") != SUCCESS)
    {
      WriteLine(pMe, "2st Remove Failed");
    }
    else
    {
      WriteLine(pMe, "2st Remove OK");
    }
    
    // The directory we created is now empty, so we can remove it
    // as well (note that we don't need to open and close directories)
    if (IFILEMGR_RmDir(pMe->pMyFileMgr, AEEFS_HOME_DIR"xxxdir") != SUCCESS)
    {
        WriteLine(pMe, "Dir Remove Failed");
    }
    else
    {
        WriteLine(pMe, "Dir Remove OK");
    }

    // To verify that our removals really happened, we can test if the
    // directory and its files are still there
    if (IFILEMGR_Test(pMe->pMyFileMgr, AEEFS_HOME_DIR"xxxdir") == SUCCESS)
      WriteLine(pMe, "Dir Still There!");

    if (IFILEMGR_Test(pMe->pMyFileMgr, AEEFS_HOME_DIR"xxxdir/xx1.fil") == SUCCESS)
      WriteLine(pMe, "xxxdir/xx1.fil Still There!");

    if (IFILEMGR_Test(pMe->pMyFileMgr, AEEFS_HOME_DIR"xxxdir/xx2.fil") == SUCCESS)
      WriteLine(pMe, AEEFS_HOME_DIR"xxxdir/xx2.fil Still There!");
    
    IDISPLAY_Update(pMe->pIDisplay);

    pMe->status = STATUS_FS_PREFIX;
 }
