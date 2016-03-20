/*===========================================================================

FILE: ifileusage.c

SERVICES: Sample applet using AEE

DESCRIPTION
  This file contains usage examples of the IFileMgr and IFile interfaces. The IFileMgr
  interface functions include the creation and deletion of files and directories. 
  IFile interface functions consist of operations performed on files opened by the IFileMgr 
  interface.  The following functions are illustrated in this usage example:

    -IFILEMGR_EnumInit
    -IFILEMGR_EnumNext
    -IFILEMGR_GetFreeSpace
    -IFILEMGR_GetInfo
    -IFILEMGR_GetLastError
    -IFILEMGR_MkDir
    -IFILEMGR_OpenFile
    -IFILEMGR_Remove
    -IFILEMGR_Rename
    -IFILEMGR_RmDir
    -IFILEMGR_Test
    -IFILE_GetInfo
    -IFILE_Read
    -IFILE_Release
    -IFILE_Seek
    -IFILE_Truncate
    -IFILE_Write

PUBLIC CLASSES:  
   N/A


       Copyright © 2000-2001 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
// Mandatory includes
#include "AEEAppGen.h"  // AEEApplet declaration
#include "AEEUsageAppIDs.h" // Class IDs of usage apps (including this one)
#include "AEE.h"      // Standard AEE Declarations
#include "AEEShell.h" // AEE Shell Services
#include "AEEDisp.h"    // AEE Display Services

// Services used by app
#include "AEEFile.h"    //  AEEFile Services
#include "AEEMenu.h"    // Menu Services
#include "AEEStdLib.h"   // AEE stdlib services
#include "AEEMCF.h"   // AEE stdlib services

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/
// IFile app struct. This is the main struct for this applet. This will hold 
// all the data members that need to be remembered throughout the life of
// the applet. 
// THE FIRST DATA MEMBER OF THIS STRUCT MUST BE AN AEEApplet OBJECT. 
// This is critical requirement which is related to an applet being
// dynamically loaded. The app developers should add their data members
// following the AEEApplet data member.
typedef struct _CIFileApp {
   AEEApplet        a;
   IMenuCtl *  m_pIMenu;
   IFileMgr * m_pIFileMgr;
   ISoundPlayer *  m_pISoundPlayer;
   boolean m_bSPActive;
   AEERect     m_rc;
   int m_cLineNum; // screen line to be written to next
   int m_cMaxLine; // max line number of screen on which output can be written
} CIFileApp;

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/
// Mandatory function:
// App Handle Event function
static boolean FileApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// App specific data alloc-init/free functions
static boolean IFile_InitAppData(IApplet* pMe);
static void IFile_FreeAppData(IApplet* pMe);

static void BuildMainMenu(CIFileApp *pMe);
static void IFileUsage (CIFileApp *pMe, uint16 wParam);
static void DisplayEvent (CIFileApp *pMe, uint16 wParam);
static int DisplayOutput(IShell *pIShell, IDisplay *pIDisplay, int nline, char *pszStr);
static void WriteLine (CIFileApp *pMe, char *pszStr);
static void SoundPlayerCBFn( void * pUser, AEESoundPlayerCmd eCBType, AEESoundPlayerStatus eStatus, uint32 dwParam );

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/

// App specific constants
#define USAGE_BASIC_USAGE       100
#define USAGE_FMGR_GETFREESPACE 101
#define USAGE_PLAY_MUSIC 102
#define USAGE_MUSIC_MCF 103
#define USAGE_MUSIC_MCF_ACCESS_FILE_DEMO_APP 104
#define USAGE_ERASE_SCREEN      119   // Special case to erase screen

#define AEEFS_OTHER_APP_DIR "fs:/~0x0100900F/my_aaa/"

// App Resource File
#define APP_RES_FILE AEEFS_CARD0_DIR"piano.mp3"

// Size of buffer used to hold text strings
#define TEXT_BUFFER_SIZE (100 * sizeof(AECHAR))

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
   if(ClsId == AEECLSID_FILE_APP){
      if (AEEApplet_New(sizeof(CIFileApp), ClsId, pIShell,po,(IApplet**)ppObj,
         (AEEHANDLER)FileApp_HandleEvent,(PFNFREEAPPDATA)IFile_FreeAppData) == TRUE)
      {
         DBGPRINTF("lintao new passed");
         if (IFile_InitAppData((IApplet*)*ppObj))
            {
                     DBGPRINTF("lintao IFile_InitAppData passed");

             return(AEE_SUCCESS);
            }
      }
   }
  return (EFAILED);
}

/*===========================================================================

FUNCTION FileApp_HandleEvent

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

static boolean FileApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
   CIFileApp * pMe = (CIFileApp*)pi;

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
        if (pMe->m_pIMenu)
        {
          if ((IMENUCTL_IsActive(pMe->m_pIMenu) == FALSE) &&
             ((wParam == AVK_UP) || (wParam == AVK_DOWN)))
          {
            IMENUCTL_SetActive(pMe->m_pIMenu, TRUE);
            IMENUCTL_Redraw(pMe->m_pIMenu);
          }

          // stop music if it's active
          if(pMe->m_bSPActive == TRUE)
          {
              pMe->m_bSPActive = FALSE;
              if (pMe->m_pISoundPlayer)
              {
                 // First deregister the notify. This way no callbacks 
                 // will be posted for the rest of the ISoundPlayer operations.
                 ISOUNDPLAYER_RegisterNotify (pMe->m_pISoundPlayer, NULL, NULL);
                 // Stop any songs being played at this time
                 ISOUNDPLAYER_Stop (pMe->m_pISoundPlayer);
                 ISOUNDPLAYER_Release (pMe->m_pISoundPlayer);
                 pMe->m_pISoundPlayer = NULL;
              }
          }

          return IMENUCTL_HandleEvent(pMe->m_pIMenu, EVT_KEY, wParam, 0);
        }
        else
          return FALSE;

    case EVT_COMMAND:
      switch(wParam)
      {
        // The following commands are generated by user selections
        // from the main usage app menu.
          case USAGE_BASIC_USAGE:
          case USAGE_FMGR_GETFREESPACE:
          case USAGE_PLAY_MUSIC:
          case USAGE_MUSIC_MCF:
          case USAGE_MUSIC_MCF_ACCESS_FILE_DEMO_APP:
             // Deactivate main menu to turn off horizontal scrolling
             // of long menu items
             IMENUCTL_SetActive(pMe->m_pIMenu, FALSE);

             // Execute the usage example the user has selected
             IFileUsage (pMe, wParam);
             return TRUE;
          default:
            return FALSE;
      }
    default:
      break;
   }
   return FALSE;
}


/*===========================================================================

FUNCTION IFile_InitAppData

DESCRIPTION
   This function initializes app specific data.

PROTOTYPE:
  static boolean IFile_InitAppData(IApplet* pi);

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
static boolean IFile_InitAppData(IApplet* pi)
{
   CIFileApp * pMe = (CIFileApp*)pi;
   int charHeight, pnAscent, pnDescent;
   AEEDeviceInfo di;

   // Make sure the pointers we'll be using are valid
   if (pMe == NULL || pMe->a.m_pIShell == NULL)
     return FALSE;

   pMe->m_pIMenu = NULL;
   pMe->m_pIFileMgr = NULL;
   pMe->m_pISoundPlayer = NULL;
   pMe->m_bSPActive = FALSE;

   // Determine the amount of available screen space
   ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);

   // Determine the height of a line of text
   charHeight = IDISPLAY_GetFontMetrics (pMe->a.m_pIDisplay, AEE_FONT_NORMAL,
      &pnAscent, &pnDescent);

   // Number of available lines equals the available screen
   // space divided by the height of a line, minus 3 for the
   // lines we always print at the top and bottom of the screen
   pMe->m_cMaxLine = (di.cyScreen / charHeight) - 3;

   return TRUE;
}

/*===========================================================================

FUNCTION IFile_FreeAppData

DESCRIPTION
    Frees any data and interfaces used by this app.

PROTOTYPE:
  static void IFile_FreeAppData(IApplet* pi);

PARAMETERS:
  pi [in]: Pointer to the IApplet structure.

DEPENDENCIES
    None.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
static void IFile_FreeAppData(IApplet* pi)
{
   CIFileApp * pMe = (CIFileApp*)pi;

   if(pMe->m_pIMenu != NULL)
   {
     IMENUCTL_Release(pMe->m_pIMenu);
     pMe->m_pIMenu = NULL;
   }

   if(pMe->m_pIFileMgr != NULL)
   {
     IFILEMGR_Release(pMe->m_pIFileMgr);
     pMe->m_pIFileMgr = NULL;
   }

   // Release SoundPlayer object if it is not released
   if (pMe->m_pISoundPlayer != NULL)
   {
      // Deregister Notify so that 
      ISOUNDPLAYER_RegisterNotify (pMe->m_pISoundPlayer, NULL, NULL);
      // Stop any songs being played at this time
      ISOUNDPLAYER_Stop (pMe->m_pISoundPlayer);
      ISOUNDPLAYER_Release (pMe->m_pISoundPlayer);
      pMe->m_pISoundPlayer = NULL;
   }
}

/*===============================================================================
                     APP HELPER FUNCTION DEFINITIONS
=============================================================================== */
/*===========================================================================

FUNCTION: IFileUsage

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
   static void IFileUsage(CIFileApp * pMe, uint16 wParam)

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
static void IFileUsage (CIFileApp * pMe, uint16 wParam)
{
  uint32    rights = 0;
  int       retval;
  
	
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
      case USAGE_BASIC_USAGE:
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
          IFile * pIFile1, * pIFile2, * pIFile3;
          char szFirstStr[]  = "First Sentence";
          char szSecondStr[] = "Second Sentence";
          char szThirdStr[]  = "Third Sentence";
          char szBuffer[30];

          if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,(void **)&pMe->m_pIFileMgr))
              break;

          // Create a new directory.  It will be created as a sub-directory of
          // the directory that contains this app's files.
          if (IFILEMGR_MkDir(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"xxxdir") != SUCCESS)
          { 
            IFILEMGR_Release(pMe->m_pIFileMgr);
            pMe->m_pIFileMgr = NULL;
            break;
          }

          // Create two new files in this directory, specifying the directory
          // and file names
          if ((pIFile1 = IFILEMGR_OpenFile(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"xxxdir/xx1.fil", _OFM_CREATE)) == NULL)
          { 
            IFILEMGR_Release(pMe->m_pIFileMgr);
            pMe->m_pIFileMgr = NULL;
            break;
          }

          if ((pIFile2 = IFILEMGR_OpenFile(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"xxxdir/xx2.fil", _OFM_CREATE)) == NULL)
          { 
            IFILEMGR_Release(pMe->m_pIFileMgr);
            pMe->m_pIFileMgr = NULL;
            IFILE_Release(pIFile1);
            break;
          }

          // To verify that the directory was created, list it
          // (TRUE parm means list directories only)
          IFILEMGR_EnumInit(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"xxxdir", TRUE);
          while (IFILEMGR_EnumNext(pMe->m_pIFileMgr, &fi))
              WriteLine(pMe, fi.szName);

          // To verify that the files were created, list them
          // (FALSE parm means list files only)
          IFILEMGR_EnumInit(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"xxxdir", FALSE);
          while (IFILEMGR_EnumNext(pMe->m_pIFileMgr, &fi))
              WriteLine(pMe, fi.szName);

          // Write the three test sentences to the file we just opened.  IFILE_Write
          // returns the number of bytes it wrote, which we check to make sure the 
          // write operations were successful.
          if (IFILE_Write(pIFile1, szFirstStr, sizeof(szFirstStr)) != sizeof(szFirstStr))
              WriteLine(pMe, "1st Write failed");
          if (IFILE_Write(pIFile1, szSecondStr, sizeof(szSecondStr)) != sizeof(szSecondStr))
              WriteLine(pMe, "2nd Write failed");
          if (IFILE_Write(pIFile1, szThirdStr, sizeof(szThirdStr)) != sizeof(szThirdStr))
              WriteLine(pMe, "3rd Write failed");

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
          if (IFILEMGR_Remove(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"xxxdir/xx1.fil") != SUCCESS)
            WriteLine(pMe, "1st Remove Failed");

          IFILE_Release(pIFile2);
          if (IFILEMGR_Remove(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"xxxdir/xx2.fil") != SUCCESS)
            WriteLine(pMe, "2nd Remove Failed");
          // The directory we created is now empty, so we can remove it
          // as well (note that we don't need to open and close directories)
          if (IFILEMGR_RmDir(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"xxxdir") != SUCCESS)
            WriteLine(pMe, "Dir Remove Failed");
 
          // To verify that our removals really happened, we can test if the
          // directory and its files are still there
          if (IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"xxxdir") == SUCCESS)
            WriteLine(pMe, "Dir Still There!");

          if (IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"xxxdir/xx1.fil") == SUCCESS)
            WriteLine(pMe, "xxxdir/xx1.fil Still There!");

          if (IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"xxxdir/xx2.fil") == SUCCESS)
            WriteLine(pMe, AEEFS_CARD0_DIR"xxxdir/xx2.fil Still There!");

          // We conclude by releasing the IFileMgr interface we created initially
          IFILEMGR_Release(pMe->m_pIFileMgr);
          pMe->m_pIFileMgr = NULL;
         }
         break;


      case USAGE_FMGR_GETFREESPACE:
         {
          // 2. Usage: IFILEMGR_GetFreeSpace

          // IFILEMGR_GetFreeSpace returns the amount of free space available
          // in the file system.  Here we use this function to retrieve the
          // number of free and total file system bytes and echo the values
          // back to the screen.

          uint32 dwFreeSpace, dwTotalFileSystemSize;
          int nRet;
          char szBuf[100];

          if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,(void **)&pMe->m_pIFileMgr))
            break;

          // Get file system information.  Note that the amount of free space is the
          // return value of this function, and the total file system size is passed
          // back in the function's second parameter.
          nRet = IFILEMGR_GetFreeSpaceEx(pMe->m_pIFileMgr, AEEFS_CARD0_DIR, &dwTotalFileSystemSize, &dwFreeSpace);
          if (nRet )
          {
              SPRINTF(szBuf, "IFILEMGR_GetFreeSpaceEx return Error: %u", nRet); 
              WriteLine(pMe, szBuf);
              break;
          }
          
          // Echo amount of free space
          WriteLine(pMe, "Path point to \"fs:/card0/\"");
          WriteLine(pMe, "File Space Avail:");
          SPRINTF(szBuf, "  %u bytes", dwFreeSpace); 
          WriteLine(pMe, szBuf);

          // Echo total file system size
          WriteLine(pMe, "Total File Sys Size:");
          SPRINTF(szBuf, "  %u bytes", dwTotalFileSystemSize); 
          WriteLine(pMe, szBuf);

          IFILEMGR_Release(pMe->m_pIFileMgr);
          pMe->m_pIFileMgr = NULL;
         }
         break;

      case USAGE_PLAY_MUSIC:
         {
           // 3. Usage: Play music on card0

           // search music in card0 directory and play
           AEESoundPlayerInput input;
           FileInfo    fi;
           boolean isSuc;

           // Enumerate music files in the card0 directory
           if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,(void **)&pMe->m_pIFileMgr))
           {
               WriteLine(pMe, "FILEMGR create fail");
               break;
           }

           // Initialize the file pointer to the first file in the "fs:/card0/music" directory.
           if(IFILEMGR_EnumInit(pMe->m_pIFileMgr, AEEFS_CARD0_DIR"music", FALSE) != SUCCESS)
           {
               WriteLine(pMe, "IFILEMGR_EnumInit fail");
           }

           isSuc = IFILEMGR_EnumNext(pMe->m_pIFileMgr, &fi);

           IFILEMGR_Release(pMe->m_pIFileMgr);
           pMe->m_pIFileMgr = NULL;
           
           if(isSuc != TRUE)
           {
               WriteLine(pMe, "Can't find file in card0 directory");
               break;
           }


           // Create ISoundPlayer interface object. This step needs to be done only
           // if the ISoundPlayer interface object have not already been created. 
           ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOUNDPLAYER, (void **)&pMe->m_pISoundPlayer); 

           if (pMe->m_pISoundPlayer == NULL)
           {
               WriteLine(pMe, "sound play create fail");
               break;
           }

           // Register a notify function.
           ISOUNDPLAYER_RegisterNotify(pMe->m_pISoundPlayer, &SoundPlayerCBFn, pMe);

           pMe->m_bSPActive = TRUE;

           // Set input
           input = SDT_FILE;

           // Set the first file in the array.
           ISOUNDPLAYER_Set(pMe->m_pISoundPlayer, input,  fi.szName);

           // ISOUNDPLAYER_Play: issues a command to play a MIDI/MP3 audio 
           ISOUNDPLAYER_Play (pMe->m_pISoundPlayer);
         }
         break;
         
      case USAGE_MUSIC_MCF:
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
          IFile * pIFile1, * pIFile2, * pIFile3;
          char szFirstStr[]  = "First Sentence";
          char szSecondStr[] = "Second Sentence";
          char szThirdStr[]  = "Third Sentence";
          char szBuffer[30];

          if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,(void **)&pMe->m_pIFileMgr))
              break;

          // Create a new directory.  It will be created as a sub-directory of
          // the directory that contains this app's files.
          if (IFILEMGR_MkDir(pMe->m_pIFileMgr, AEEFS_MCF_MUSIC_DIR"xxxdir") != SUCCESS)
          { 
            IFILEMGR_Release(pMe->m_pIFileMgr);
            pMe->m_pIFileMgr = NULL;
            break;
          }

          // Create two new files in this directory, specifying the directory
          // and file names
          if ((pIFile1 = IFILEMGR_OpenFile(pMe->m_pIFileMgr, AEEFS_MCF_MUSIC_DIR"xxxdir/xx1.fil", _OFM_CREATE)) == NULL)
          { 
            IFILEMGR_Release(pMe->m_pIFileMgr);
            pMe->m_pIFileMgr = NULL;
            break;
          }

          if ((pIFile2 = IFILEMGR_OpenFile(pMe->m_pIFileMgr, AEEFS_MCF_MUSIC_DIR"xxxdir/xx2.fil", _OFM_CREATE)) == NULL)
          { 
            IFILEMGR_Release(pMe->m_pIFileMgr);
            pMe->m_pIFileMgr = NULL;
            IFILE_Release(pIFile1);
            break;
          }

          // To verify that the directory was created, list it
          // (TRUE parm means list directories only)
          IFILEMGR_EnumInit(pMe->m_pIFileMgr, AEEFS_MCF_MUSIC_DIR"xxxdir", TRUE);
          while (IFILEMGR_EnumNext(pMe->m_pIFileMgr, &fi))
              WriteLine(pMe, fi.szName);

          // To verify that the files were created, list them
          // (FALSE parm means list files only)
          IFILEMGR_EnumInit(pMe->m_pIFileMgr, AEEFS_MCF_MUSIC_DIR"xxxdir", FALSE);
          while (IFILEMGR_EnumNext(pMe->m_pIFileMgr, &fi))
              WriteLine(pMe, fi.szName);

          // Write the three test sentences to the file we just opened.  IFILE_Write
          // returns the number of bytes it wrote, which we check to make sure the 
          // write operations were successful.
          if (IFILE_Write(pIFile1, szFirstStr, sizeof(szFirstStr)) != sizeof(szFirstStr))
              WriteLine(pMe, "1st Write failed");
          if (IFILE_Write(pIFile1, szSecondStr, sizeof(szSecondStr)) != sizeof(szSecondStr))
              WriteLine(pMe, "2nd Write failed");
          if (IFILE_Write(pIFile1, szThirdStr, sizeof(szThirdStr)) != sizeof(szThirdStr))
              WriteLine(pMe, "3rd Write failed");

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
          if (IFILEMGR_Remove(pMe->m_pIFileMgr, AEEFS_MCF_MUSIC_DIR"xxxdir/xx1.fil") != SUCCESS)
            WriteLine(pMe, "1st Remove Failed");

          IFILE_Release(pIFile2);
          if (IFILEMGR_Remove(pMe->m_pIFileMgr, AEEFS_MCF_MUSIC_DIR"xxxdir/xx2.fil") != SUCCESS)
            WriteLine(pMe, "2nd Remove Failed");
          // The directory we created is now empty, so we can remove it
          // as well (note that we don't need to open and close directories)
          if (IFILEMGR_RmDir(pMe->m_pIFileMgr, AEEFS_MCF_MUSIC_DIR"xxxdir") != SUCCESS)
            WriteLine(pMe, "Dir Remove Failed");
 
          // To verify that our removals really happened, we can test if the
          // directory and its files are still there
          if (IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_MCF_MUSIC_DIR"xxxdir") == SUCCESS)
            WriteLine(pMe, "Dir Still There!");

          if (IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_MCF_MUSIC_DIR"xxxdir/xx1.fil") == SUCCESS)
            WriteLine(pMe, "xxxdir/xx1.fil Still There!");

          if (IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_MCF_MUSIC_DIR"xxxdir/xx2.fil") == SUCCESS)
            WriteLine(pMe, AEEFS_MCF_MUSIC_DIR"xxxdir/xx2.fil Still There!");

          // We conclude by releasing the IFileMgr interface we created initially
          IFILEMGR_Release(pMe->m_pIFileMgr);
          pMe->m_pIFileMgr = NULL;
         }
         break;

      case USAGE_MUSIC_MCF_ACCESS_FILE_DEMO_APP:
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
          IFile * pIFile1, * pIFile2, * pIFile3;
          char szFirstStr[]  = "First Sentence";
          char szSecondStr[] = "Second Sentence";
          char szThirdStr[]  = "Third Sentence";
          char szBuffer[30];

          if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR,(void **)&pMe->m_pIFileMgr))
              break;

          // Create a new directory.  It will be created as a sub-directory of
          // the directory that contains this app's files.
          if (IFILEMGR_MkDir(pMe->m_pIFileMgr, AEEFS_OTHER_APP_DIR"xxxdir") != SUCCESS)
          {
            DBGPRINTF("IFILEMGR_MkDir error");
            IFILEMGR_Release(pMe->m_pIFileMgr);
            pMe->m_pIFileMgr = NULL;
            break;
          }

          // Create two new files in this directory, specifying the directory
          // and file names
          if ((pIFile1 = IFILEMGR_OpenFile(pMe->m_pIFileMgr, AEEFS_OTHER_APP_DIR"xxxdir/xx1.fil", _OFM_CREATE)) == NULL)
          { 
            DBGPRINTF("IFILEMGR_OpenFile error");
            IFILEMGR_Release(pMe->m_pIFileMgr);
            pMe->m_pIFileMgr = NULL;
            break;
          }

          if ((pIFile2 = IFILEMGR_OpenFile(pMe->m_pIFileMgr, AEEFS_OTHER_APP_DIR"xxxdir/xx2.fil", _OFM_CREATE)) == NULL)
          { 
            DBGPRINTF("IFILEMGR_OpenFile error");
            IFILEMGR_Release(pMe->m_pIFileMgr);
            pMe->m_pIFileMgr = NULL;
            IFILE_Release(pIFile1);
            break;
          }

          // To verify that the directory was created, list it
          // (TRUE parm means list directories only)
          IFILEMGR_EnumInit(pMe->m_pIFileMgr, AEEFS_OTHER_APP_DIR"xxxdir", TRUE);
          while (IFILEMGR_EnumNext(pMe->m_pIFileMgr, &fi))
              WriteLine(pMe, fi.szName);

          // To verify that the files were created, list them
          // (FALSE parm means list files only)
          IFILEMGR_EnumInit(pMe->m_pIFileMgr, AEEFS_OTHER_APP_DIR"xxxdir", FALSE);
          while (IFILEMGR_EnumNext(pMe->m_pIFileMgr, &fi))
              WriteLine(pMe, fi.szName);

          // Write the three test sentences to the file we just opened.  IFILE_Write
          // returns the number of bytes it wrote, which we check to make sure the 
          // write operations were successful.
          if (IFILE_Write(pIFile1, szFirstStr, sizeof(szFirstStr)) != sizeof(szFirstStr))
              WriteLine(pMe, "1st Write failed");
          if (IFILE_Write(pIFile1, szSecondStr, sizeof(szSecondStr)) != sizeof(szSecondStr))
              WriteLine(pMe, "2nd Write failed");
          if (IFILE_Write(pIFile1, szThirdStr, sizeof(szThirdStr)) != sizeof(szThirdStr))
              WriteLine(pMe, "3rd Write failed");

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
              DBGPRINTF("IFILE_Seek error");
              // Read the second sentence and echo it back to the screen
              // (when successful, IFile_Read returns the number of bytes read)
              if (IFILE_Read(pIFile1, szBuffer, sizeof(szSecondStr)) == sizeof(szSecondStr))
                WriteLine(pMe, szBuffer);
          }

          // Clean up--first remove the two files we created
          // (must close them first--second file is still open from when 
          // we created it)
          IFILE_Release(pIFile1);
          if (IFILEMGR_Remove(pMe->m_pIFileMgr, AEEFS_OTHER_APP_DIR"xxxdir/xx1.fil") != SUCCESS)
            WriteLine(pMe, "1st Remove Failed");

          IFILE_Release(pIFile2);
          if (IFILEMGR_Remove(pMe->m_pIFileMgr, AEEFS_OTHER_APP_DIR"xxxdir/xx2.fil") != SUCCESS)
            WriteLine(pMe, "2nd Remove Failed");
          // The directory we created is now empty, so we can remove it
          // as well (note that we don't need to open and close directories)
          if (IFILEMGR_RmDir(pMe->m_pIFileMgr, AEEFS_OTHER_APP_DIR"xxxdir") != SUCCESS)
            WriteLine(pMe, "Dir Remove Failed");
 
          // To verify that our removals really happened, we can test if the
          // directory and its files are still there
          if (IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_OTHER_APP_DIR"xxxdir") == SUCCESS)
            WriteLine(pMe, "Dir Still There!");

          if (IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_OTHER_APP_DIR"xxxdir/xx1.fil") == SUCCESS)
            WriteLine(pMe, "xxxdir/xx1.fil Still There!");

          if (IFILEMGR_Test(pMe->m_pIFileMgr, AEEFS_OTHER_APP_DIR"xxxdir/xx2.fil") == SUCCESS)
            WriteLine(pMe, AEEFS_OTHER_APP_DIR"xxxdir/xx2.fil Still There!");

          // We conclude by releasing the IFileMgr interface we created initially
          IFILEMGR_Release(pMe->m_pIFileMgr);
          pMe->m_pIFileMgr = NULL;
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
   static void DisplayEvent(CIFileApp * pMe, uint16 wParam)

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
static void DisplayEvent (CIFileApp *pMe, uint16 wParam)
{
   AEERect qrc;
   AEEDeviceInfo di; // Device Info
   AECHAR * szBuf;
   AEERect rc; 

   // Make sure the pointers we'll be using are valid
   if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->a.m_pIDisplay == NULL)
     return;

   if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)
     return;

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
      case USAGE_BASIC_USAGE:
         STR_TO_WSTR("Basic Usage", szBuf, TEXT_BUFFER_SIZE);
         break; 
      case USAGE_FMGR_GETFREESPACE:
         STR_TO_WSTR("GetFreeSpace", szBuf, TEXT_BUFFER_SIZE);
         break; 
      case USAGE_PLAY_MUSIC:
         STR_TO_WSTR("Play Music on card0", szBuf, TEXT_BUFFER_SIZE);
         break; 
      case USAGE_MUSIC_MCF:
         STR_TO_WSTR("Music MCF", szBuf, TEXT_BUFFER_SIZE);
         break; 
      case USAGE_MUSIC_MCF_ACCESS_FILE_DEMO_APP:
         STR_TO_WSTR("Music Access Other App", szBuf, TEXT_BUFFER_SIZE);
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
   static void BuildMainMenu(CIFileApp * pMe)

PARAMETERS:
   pMe:   [in]: Contains a pointer to the usage app data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void BuildMainMenu(CIFileApp *pMe)
{
   AEERect qrc;
   AEEDeviceInfo di;
   AECHAR * szBuf;

   // Make sure the pointers we'll be using are valid
   if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->m_pIMenu == NULL)
     return;

   if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)
     return;

   // Set Title
   STR_TO_WSTR("IFile Examples", szBuf, TEXT_BUFFER_SIZE);
   IMENUCTL_SetTitle(pMe->m_pIMenu, NULL, 0, szBuf);

   //Set the Rectangle for the Menu
   ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);
   qrc.x = 0;
   qrc.y = 0;
   qrc.dx = di.cxScreen;
   qrc.dy = di.cyScreen;
   IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);  

   //Add individual entries to the Menu

   // Add Basic Usage to menu
   STR_TO_WSTR("1. Basic Usage", szBuf, TEXT_BUFFER_SIZE);
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_BASIC_USAGE, szBuf, 0);

   // Add GetFreeSpace to menu
   STR_TO_WSTR("2. GetFreeSpace", szBuf, TEXT_BUFFER_SIZE);
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_FMGR_GETFREESPACE, szBuf, 0);

   // Add PlayMusic to menu
   STR_TO_WSTR("3. PlayMusic", szBuf, TEXT_BUFFER_SIZE);
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_PLAY_MUSIC, szBuf, 0);

   // Add PlayMusic to menu
   STR_TO_WSTR("4. Music File Access", szBuf, TEXT_BUFFER_SIZE);
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_MUSIC_MCF, szBuf, 0);

   STR_TO_WSTR("5. Access Other App", szBuf, TEXT_BUFFER_SIZE);
   IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, USAGE_MUSIC_MCF_ACCESS_FILE_DEMO_APP, szBuf, 0);

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
   int DisplayOutput(IShell *pIShell, IDisplay *pDisplay, int nline, char *pszStr)

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
static int DisplayOutput(IShell *pIShell, IDisplay *pIDisplay, int nline, char *pszStr)
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
     return 0;

   if ((szBuf = (AECHAR *) MALLOC(TEXT_BUFFER_SIZE)) == NULL)
     return 0;

   // Get device information
   ISHELL_GetDeviceInfo(pIShell,&di);

   // Get the font metrics info
   charHeight = IDISPLAY_GetFontMetrics (pIDisplay, AEE_FONT_NORMAL,
      &pnAscent, &pnDescent);

   // Convert to wide string (unicode)
   STR_TO_WSTR ((char *)pszStr, szBuf, TEXT_BUFFER_SIZE);

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
   totalCh = STRLEN ((char *)pszStr);

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
   static void WriteLine(CIFileApp * pMe,  char *pszStr)

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
static void WriteLine(CIFileApp * pMe, char *pszStr)
{
   
  // If all available lines used, return
  if (pMe->m_cLineNum > pMe->m_cMaxLine)
    return;

  // If we've reached last available line, print an
  // ellipsis indicator
  if (pMe->m_cLineNum == pMe->m_cMaxLine)
  {
    DisplayOutput(pMe->a.m_pIShell, pMe->a.m_pIDisplay, pMe->m_cLineNum++, "...");
    return;
  }

  // There are lines available for printing.  Print the string passed as
  // input and update the count of lines available
  pMe->m_cLineNum += DisplayOutput(pMe->a.m_pIShell, pMe->a.m_pIDisplay, pMe->m_cLineNum, pszStr);
  return;

} // End of WriteLine

/*===========================================================================

FUNCTION SoundPlayerCBFn

DESCRIPTION
	This is the callback function that is registered with ISoundPlayer interface.
   ISoundPlayer communicates with the applet via this callback only.

PROTOTYPE:
   void SoundPlayerCBFn
   (
      AEE_SOUNDCBIndType  eCBType,
      AEESoundPlayerStatus eSPStatus,
      const void *              pClientData,
      void *                    dwParam
   );

PARAMETERS:
   eCBType: Type of ISoundPlayer callback.
   eSPStatus: Status contained in callback. The status depends on the 
            eCBType.
   pClientData: A reference to correlate this transaction
   dwParam: Additional info specific to callback type and status type

   Refer to BREW Reference Guide for more details.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void SoundPlayerCBFn( void * pUser, AEESoundPlayerCmd eCBType, AEESoundPlayerStatus eStatus, uint32 dwParam )
{
   CIFileApp* pMe = (CIFileApp*)pUser;
   char szBuf [50] = {0};
   AEESoundPlayerCmdData * pData = (AEESoundPlayerCmdData *) dwParam;

   if (pMe == NULL || pMe->m_pIMenu == NULL || pMe->a.m_pIShell == NULL)
      return;

   switch ( eStatus )
   {
      case AEE_SOUNDPLAYER_SUCCESS:
         {
            // If the callback was invoked as a result of the
            // GetVolume function print the volume value on the
            // screen.
            if (pData && eCBType == AEE_SOUNDPLAYER_VOLUME_CB)
            {
               SPRINTF (szBuf, "Current Vol: %u", pData->wVolume); 
               WriteLine(pMe, szBuf);
            }
            else if ((eCBType == AEE_SOUNDPLAYER_PLAY_CB) && (pMe->m_bSPActive))
            {
               WriteLine(pMe, "Status: Playing");
            }
            else if ((eCBType == AEE_SOUNDPLAYER_STATUS_CB || eCBType == AEE_SOUNDPLAYER_SOUND_CB) && !pMe->m_bSPActive)
            {
               WriteLine(pMe, "Status: Success");
            }
         }
         break;
         
      case AEE_SOUNDPLAYER_DONE:
         if (pData && eCBType == AEE_SOUNDPLAYER_TIME_CB)
         {
            SPRINTF (szBuf, "Total (ms): %u", (uint16)pData->dwTotalTime); 
            WriteLine(pMe, szBuf);
         }
         else if (pMe->m_bSPActive)
         {
            WriteLine(pMe, "Status: Stopped");
         }
         break;
         
      case AEE_SOUNDPLAYER_AUDIO_SPEC:
         if (pMe->m_bSPActive)
         {
            WriteLine(pMe, "Status: Playing");
         }
         break;
      case AEE_SOUNDPLAYER_REWIND:
         break;
      case AEE_SOUNDPLAYER_FFORWARD:
         break;
      case AEE_SOUNDPLAYER_PAUSE:
         break;
      case AEE_SOUNDPLAYER_RESUME:
         break;
      case AEE_SOUNDPLAYER_TEMPO:
         break;
      case AEE_SOUNDPLAYER_TUNE:
         break;
      case AEE_SOUNDPLAYER_TICK_UPDATE:
         break;
      case AEE_SOUNDPLAYER_FAILURE:
         WriteLine(pMe, "Music player failure");
         break;
      case AEE_SOUNDPLAYER_ABORTED:
         WriteLine(pMe, "Music player aborted");
         break;
      default:
         WriteLine(pMe, "Music player: Unknown status");
         break;
   }
}


