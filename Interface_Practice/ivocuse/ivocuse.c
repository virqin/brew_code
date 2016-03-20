/*===========================================================================

FILE: ivocuse.c

DESCRIPTION

  This app demonstrates BREW's ability to stream audio with the IVocoder interface.  
  In order to simulate streaming, the application takes inputed data from the
  IVocoder interface and then plays that back through the interface.

===========================================================================*/

/*===============================================================================
                                   INCLUDES 
=============================================================================== */

#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AEEShell.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "AEEVocoder.h"
#include "AEESound.h"
#include "ivocuse.bid"
#include "ivocuse_res.h"
#include "AEEFILE.h"

/*===============================================================================
                            GLOBAL CONSTANT DEFINITIONS
=============================================================================== */

//The buffer must be large enough to support the maximum frame rate for the 
//vocoder, not merely the maximum used by the current rate limits
#define MAXFRAMELEN 256 // Size of input buffer to VocInRead


/*===============================================================================
                                   STRUCTURES
=============================================================================== */

// TODO: add comments
typedef struct _CVocApp {
   AEEApplet            a;                       // Applet Structure
   byte                 frameData[MAXFRAMELEN];  // Buffer for reading and writing frames
   AECHAR               pNameBuffer[256];        // Buffer for display text
   AEEDeviceInfo        deviceInfo;              // Holds Device Info

   IMenuCtl            *pIMenu;                  // Pointer to Main Menu
   IMenuCtl            *pISoft;                  // Pointer to Soft Menu
   IMenuCtl            *pIBack;                  // This menu holds the back button
   IVocoder            *pIVocoder;               // Pointer to the IVocoder interface
   ISound              *pISound;                 // Sound pointer used for setting sound variables 
   IStatic             *pIStatic;                // A Static box used for displaying text

   IVocoderConfigType   vocConfig;               // Holds Vocoder config data
   IVocoderInfoType     vocInfo;                 // Configuration information about the selected vocoder 
   AEESoundInfo         soundInfo;               // Holds info used for ISound_SetDevice

   boolean              bActive;                 // TRUE if active
   boolean              bVocOn;                  // TRUE if on        
}CVocApp;



/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/
// App Handle Event function
static boolean VocUseApp_HandleEvent(CVocApp* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// App specific data alloc-init/free functions
static boolean VocUse_InitAppData(CVocApp* pMe);
static void VocUse_FreeAppData(CVocApp* pMe);
static void HaveDataCB(uint16 numFrames, void * usrPtr);
static void NeedDataCB(uint16 numFrames, void * usrPtr);
static void ReadyCB(void * usrPtr);

// Display related helper functions
void DisplayOutput(CVocApp * pMe, uint16 event);
static void DisplayMessageToScreen(CVocApp* pMe, AECHAR* message);

// Menu manipulation
void BuildMainMenu(CVocApp *pMe);
void BuildSoftMenu(CVocApp *pMe);
static void ReplaceMenuItem(IMenuCtl *pIMenu, uint16 remove, uint16 add);


/*===============================================================================
                     FUNCTION DEFINITIONS
=============================================================================== */

int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
{
   *ppObj = NULL;

   // We want to load this App. So, invoke AEEApplet_New().To it, pass the
   // address of the app-specific handle event function.
   if (ClsId == AEECLSID_IVOCUSE)
   {
      if (AEEApplet_New(sizeof(CVocApp), ClsId, pIShell,po,(IApplet**)ppObj,
                        (AEEHANDLER)VocUseApp_HandleEvent,(PFNFREEAPPDATA)VocUse_FreeAppData)
          == TRUE)
      {
         if (VocUse_InitAppData((CVocApp*)*ppObj) == TRUE)
         {
            return(AEE_SUCCESS);
         }
      }
   }
   return(EFAILED);
}

/*===========================================================================

FUNCTION VocUse_SetActive

DESCRIPTION
   This function activates or deactivates the IVocoder.  On activation
   all parameters of the IVocoder as well as settings for sound info.  
   Info is available in the API on correct congigurations for the IVocoder.

PROTOTYPE:
	static void VocUse_SetActive(CVocApp* pMe, boolean active);

PARAMETERS:
   pMe: [in]: Pointer to CVocApp  struct
   active: [in]: TRUE for activation, FALSE for deactivation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void VocUse_SetActive(CVocApp* pMe, boolean active)
{
   int status;
   // Check to see if activity is already set
   if (active == pMe->bActive)
   {
      return;
   }

   // If we are setting the IVocoder active...
   if (active) {
      // Data Integrity checks
      if (pMe->pIVocoder && pMe->pISound) {
         // Unmute the microphone and earpiece
         pMe->soundInfo.eDevice     = AEE_SOUND_DEVICE_CURRENT; //AEE_SOUND_DEVICE_CURRENT;
         pMe->soundInfo.eMethod     = AEE_SOUND_METHOD_VOICE;
         pMe->soundInfo.eAPath      = AEE_SOUND_APATH_BOTH;
         pMe->soundInfo.eEarMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;
         pMe->soundInfo.eMicMuteCtl = AEE_SOUND_MUTECTL_UNMUTED;

         // Set configurations via ISOUND interface
         ISOUND_Set(pMe->pISound, &pMe->soundInfo);
         ISOUND_SetDevice(pMe->pISound);

         // Set necessary config data, Description in API
         pMe->vocConfig.needDataCB = NeedDataCB;
         pMe->vocConfig.haveDataCB = HaveDataCB;
         pMe->vocConfig.playedDataCB = NULL;
         pMe->vocConfig.readyCB = ReadyCB;
         pMe->vocConfig.usrPtr = pMe;
         pMe->vocConfig.max = HALF_RATE;
         pMe->vocConfig.min = EIGHTH_RATE;
         pMe->vocConfig.overwrite = TRUE;
         pMe->vocConfig.txReduction = 0;
         pMe->vocConfig.vocoder = VOC_IS127;
         pMe->vocConfig.watermark = 24;

         // Configure IVocoder
         status = IVOCODER_VocConfigure(pMe->pIVocoder, pMe->vocConfig, &pMe->vocInfo);

         // check status
         if(status != SUCCESS) {
             /*  PUSH_LOG("Problem w/ Voc %d",status);*/
         }

         
         //Set buffer
        /* PUSH_LOG("buffersize %d", pMe->vocInfo.maxFrameSize);*/

         // if on and configured
         if(pMe->bVocOn) {
            ReadyCB(pMe);
         }
      }

      // Display status to screen
      DisplayOutput(pMe, IDS_STREAM);
   }

   // If we are deactivating the IVocoder
   else if (!active) {

         // Data Integrity Checks
         if (pMe->pIVocoder && pMe->pISound) {

            // We stop incoming and outgoing data
            IVOCODER_VocInStop(pMe->pIVocoder);
            IVOCODER_VocOutStop(pMe->pIVocoder);

            // Reset data in Vocoder
            IVOCODER_VocOutReset(pMe->pIVocoder); 
            IVOCODER_VocInReset(pMe->pIVocoder); 
         }
   }

   // Set boolean value
   pMe->bActive = active;
}

/*===========================================================================

FUNCTION VocUseApp_HandleEvent

DESCRIPTION
   This function ahandles events sent to the application.

PROTOTYPE:
	static boolean VocUseApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);

PARAMETERS:
	pMe: Pointer to app data

	ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static boolean VocUseApp_HandleEvent(CVocApp* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam) {

   switch (eCode) {
      // Start Applet
      case EVT_APP_START:

         // Set Main menu active
         IMENUCTL_SetActive(pMe->pIMenu,TRUE);

         return(TRUE);

      // End Applet
      case EVT_APP_STOP:
         VocUse_SetActive(pMe, FALSE);
         return(TRUE);

      // Handle Key events
      case EVT_KEY:

         // Handle events in the main menu
         if(pMe->pIMenu && IMENUCTL_IsActive(pMe->pIMenu)) {
            return IMENUCTL_HandleEvent (pMe->pIMenu, eCode, wParam, dwParam);
         }
         // Handle events in the soft menu
         if(pMe->pISoft && IMENUCTL_IsActive(pMe->pISoft)) {

            // If AVK_CLR then go to main menu
            if(wParam == AVK_CLR) {
               ISHELL_PostEvent(pMe->a.m_pIShell, AEECLSID_IVOCUSE, EVT_COMMAND, IDSSOFT_EXIT, 0);
               return TRUE;
            }
            IMENUCTL_HandleEvent (pMe->pISoft, eCode, wParam, dwParam);
 
            // Reset Sound configurations to keep sound playing after key tone
            ISOUND_Set(pMe->pISound, &pMe->soundInfo);
            ISOUND_SetDevice(pMe->pISound);

            return TRUE;
         }
         // Handle events in the back menu
         if(pMe->pIBack && IMENUCTL_IsActive(pMe->pIBack)) {
            if(wParam == AVK_CLR) {
               IMENUCTL_SetActive(pMe->pIBack, FALSE);
               IMENUCTL_SetActive(pMe->pIMenu, TRUE);
               return TRUE;
            }
            return IMENUCTL_HandleEvent (pMe->pIBack, eCode, wParam, dwParam);
         }
         break;

      // Events from menus
      case EVT_COMMAND:
          switch(wParam) {

          // Set menu inactive and begin simulating Streaming
          case IDSMAIN_STREAM:
             BuildSoftMenu(pMe);
             IMENUCTL_SetActive(pMe->pIMenu, FALSE);
             VocUse_SetActive(pMe, TRUE);
             IMENUCTL_SetActive(pMe->pISoft, TRUE);
             break;

          // Exits Application
          case IDSMAIN_EXIT:
             ISHELL_CloseApplet(pMe->a.m_pIShell, FALSE);
             break;

          // Displays basic info about the application 
          case IDSMAIN_INFO:
             IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
             IDISPLAY_UpdateEx(pMe->a.m_pIDisplay, TRUE);
             ISHELL_LoadResString(pMe->a.m_pIShell, IVOCUSE_RES_FILE, IDS_ABOUT, pMe->pNameBuffer, sizeof(pMe->pNameBuffer));
             DisplayMessageToScreen(pMe, pMe->pNameBuffer);
             IMENUCTL_SetActive(pMe->pIMenu, FALSE);
             IMENUCTL_SetActive(pMe->pIBack, TRUE);
             break;

          // Exit back to main menu
          case IDSSOFT_EXIT:
             VocUse_SetActive(pMe, FALSE);
             IVOCODER_VocOutReset(pMe->pIVocoder); 
             IVOCODER_VocInReset(pMe->pIVocoder); 
             IMENUCTL_SetActive(pMe->pISoft, FALSE);
             IMENUCTL_SetActive(pMe->pIMenu, TRUE);
             return TRUE;

          // This will start the streaming 
          case IDSSOFT_ACTIVATE:
             VocUse_SetActive(pMe, TRUE);
             ReplaceMenuItem(pMe->pISoft, IDSSOFT_ACTIVATE, IDSSOFT_DEACTIVATE);
             return TRUE;

          // This will pause the streaming
          case IDSSOFT_DEACTIVATE:
             VocUse_SetActive(pMe, FALSE);
             ReplaceMenuItem(pMe->pISoft, IDSSOFT_DEACTIVATE, IDSSOFT_ACTIVATE);
             return TRUE;

          // This event is received when user is done viewing ivocuse info
          case IDS_BACK:
             ISTATIC_SetActive(pMe->pIStatic, FALSE);
             IMENUCTL_SetActive(pMe->pIBack, FALSE);
             IMENUCTL_SetActive(pMe->pIMenu, TRUE); 
             return TRUE;

          default:
             break;
          }
      default:
         break;
   }

   return FALSE;
}


/*===========================================================================

FUNCTION VocUse_InitAppData

DESCRIPTION
   This function initializes all data necesary for the application.

PROTOTYPE:
	static boolean VocUse_InitAppData(IApplet* pi);

PARAMETERS:
   pMe: [in]: Pointer to CVocApp  struct

DEPENDENCIES
  None

RETURN VALUE
  TRUE: if all data initialized correctly
  FALSE: If function failed to intialize data

SIDE EFFECTS
  None
===========================================================================*/
static boolean VocUse_InitAppData(CVocApp* pMe) {
   IFileMgr *pFileMgr;
   // Get Device information
   pMe->deviceInfo.wStructSize = sizeof(pMe->deviceInfo);
   ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->deviceInfo);

   pMe->pIMenu = NULL;
   pMe->bActive = FALSE;
   pMe->pIVocoder = NULL;
   pMe->pISound = NULL;
   pMe->bVocOn = FALSE;

   // Create and build menus
   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MENUCTL, (void **)&pMe->pIMenu) != SUCCESS)
   {
      return FALSE;
   }
   BuildMainMenu(pMe);

   // This function will create the menu ctl
   BuildSoftMenu(pMe);

   // Create back menu and add back item
   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pMe->pIBack) != SUCCESS)
   {
      return FALSE;
   }
   IMENUCTL_AddItem(pMe->pIBack, IVOCUSE_RES_FILE, IDS_BACK, IDS_BACK, NULL, NULL);

   // Create IStatic Text Box
   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_STATIC, (void **)&pMe->pIStatic) != SUCCESS)
   {
      return FALSE;
   }

   // Create IVocoder Interface
   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_VOCODER, (void **)&pMe->pIVocoder) != SUCCESS)
   {
      return FALSE;
   }

   // Create ISound interface
   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOUND, (void **)&pMe->pISound) != SUCCESS)
   {
      return FALSE;
   }
   
   return TRUE;
}

/*===========================================================================

FUNCTION VocUse_FreeAppData

DESCRIPTION
   This function ireleases all resources used by the application.

PROTOTYPE:
	static void VocUse_FreeAppData(CVocApp * pMe);

PARAMETERS:
   pi: [in]: Pointer to CVocApp struct

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void VocUse_FreeAppData(CVocApp * pMe) {
   // Release all menu items
   if(pMe->pIMenu) {
      IMENUCTL_Release (pMe->pIMenu);
      pMe->pIMenu = NULL;
   }
   if(pMe->pISoft) {
      IMENUCTL_Release (pMe->pISoft);
      pMe->pISoft = NULL;
   }
   if(pMe->pIBack) {
      IMENUCTL_Release(pMe->pIBack);
      pMe->pIBack = NULL;
   }

   // Release IVocoder
   if(pMe->pIVocoder) {
      IVOCODER_Release(pMe->pIVocoder);
      pMe->pIVocoder = NULL;
   }

   // Release Sound interface
   if(pMe->pISound) {
      ISOUND_Release(pMe->pISound);
      pMe->pISound = NULL;
   }

   // Release Static box
   if(pMe->pIStatic) {
      ISTATIC_Release(pMe->pIStatic);
      pMe->pIStatic = NULL;
   }

}

/*===========================================================================

FUNCTION DisplayOutput

DESCRIPTION
   This function displays a specified message to the screen.  The uint16 on input
   correlates to the bar file needed for this app.

PROTOTYPE:
	void DisplayOutput(CVocApp * pMe, uint16 event);

PARAMETERS:
   pMe: [in]: Pointer to cVocApp structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void DisplayOutput(CVocApp * pMe, uint16 event) {

   // Do not update screen if on main menu
   if(IMENUCTL_IsActive(pMe->pIMenu)) {
      return;
   }

   // Clear the screen
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

   //Load the string to memory
   if(SUCCESS != ISHELL_LoadResString(pMe->a.m_pIShell, IVOCUSE_RES_FILE, event, pMe->pNameBuffer, sizeof(pMe->pNameBuffer))) {
      ISHELL_LoadResString(pMe->a.m_pIShell, IVOCUSE_RES_FILE, event, pMe->pNameBuffer, sizeof(pMe->pNameBuffer));
   } else {
      ISHELL_LoadResString(pMe->a.m_pIShell, IVOCUSE_RES_FILE, IDS_ERROR, pMe->pNameBuffer, sizeof(pMe->pNameBuffer));
   }

   // Draw text
   IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, pMe->pNameBuffer, -1, 0, 0, NULL,                // No clipping
                           IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
   IDISPLAY_Update(pMe->a.m_pIDisplay);

   // Redraw Menu if active
   if(IMENUCTL_IsActive(pMe->pISoft)) {
      IMENUCTL_Redraw(pMe->pISoft);
   }
}


/*===========================================================================

FUNCTION HaveCB

DESCRIPTION
	Callback function invoked when decoder has more frames. 

PROTOTYPE:
	static void HaveCB(uint16 numFrames, void * usrPtr);

PARAMETERS:
   numFrames [in]: number of rames that ivocoder can use
   pMe: [in]: Pointer to CVocApp  struct

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void HaveDataCB(uint16 numFrames, void * usrPtr)
{
   CVocApp* pMe = (CVocApp*)usrPtr;
   DataRateType rate;
   uint16 length;
   int i;
   int status;

   // Data integrity checks
   if (!pMe || !pMe->pIVocoder)
   {
      return;
   }

   // Read in each frame and send to network
   for (i = 0; i < numFrames; i++) {
      status = IVOCODER_VocInRead(pMe->pIVocoder,
                                  &rate, &length, pMe->frameData);

      // If we succesfully read in data, then write to IVocoder
      if (status == SUCCESS) {
         // Send recived data to IVocoder
         IVOCODER_VocOutWrite(pMe->pIVocoder, rate, length, pMe->frameData);
      }
   }
}


/*===========================================================================

FUNCTION NeedCB

DESCRIPTION
	Callback function invoked when decoder requires more frames. This function
   will get invoked when the IVocoder is paused (deactivated)

PROTOTYPE:
	static void NeedCB(uint16 numFrames, void * usrPtr);

PARAMETERS:
   numFrames [in]: number of rames that ivocoder can use
   pMe: [in]: Pointer to CVocApp  struct

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void NeedDataCB(uint16 numFrames, void * usrPtr) {
   CVocApp* pMe = (CVocApp*)usrPtr;

   // Display pause message if ivocoder needs data
   if(pMe->bVocOn) {
      DisplayOutput(pMe, IDS_PAUSE);
   }
}


/*===========================================================================

FUNCTION ReadyCB

DESCRIPTION
	Callback function invoked when vocoder is ready for use. Called after
   IVOCODER_VocConfigure has finished.

PROTOTYPE:
	static void ReadyCB(void * usrPtr);

PARAMETERS:
   pMe: [in]: Pointer to CVocApp  struct

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ReadyCB(void * usrPtr) {
   CVocApp* pMe = (CVocApp*)usrPtr;

   // Start Reading in
   IVOCODER_VocInStart(pMe->pIVocoder);

   // Allow for playing
   IVOCODER_VocOutStart(pMe->pIVocoder);

   // Set Vocoder variable to on
   pMe->bVocOn = TRUE;
}


/*===========================================================================

FUNCTION DisplayMessageToScreen

DESCRIPTION
	This Clears the Screen and displays the inputed text

PROTOTYPE:
	static void DisplayMessageToScreen(CVocApp* pMe, char* message)

PARAMETERS:
   pMe: [in]: Pointer to CMaxFileTest  struct
   message: [in]: pointer to a string

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void DisplayMessageToScreen(CVocApp* pMe, AECHAR* message) {
	AEERect	rec;

   // Set All menus to inactive
   IMENUCTL_SetActive(pMe->pIMenu, FALSE);

   // Set properties of text box
	SETAEERECT(&rec, 0, 0, pMe->deviceInfo.cxScreen, pMe->deviceInfo.cyScreen);
	ISTATIC_SetRect(pMe->pIStatic, &rec);
	ISTATIC_SetProperties(pMe->pIStatic, ST_MIDDLETEXT | ST_CENTERTEXT);
	ISTATIC_SetText(pMe->pIStatic, NULL, message, AEE_FONT_NORMAL, AEE_FONT_NORMAL);

   // Prepare text box and update screen Update screen
	ISTATIC_SetActive(pMe->pIStatic, TRUE);
	ISTATIC_Redraw(pMe->pIStatic);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

/*===========================================================================

FUNCTION BuildMainMenu

DESCRIPTION
	This function adds all menu items to the main menu. 

PROTOTYPE:
	void BuildMainMenu(CVocApp *pMe);

PARAMETERS:
   pMe: [in]: Pointer to CVocApp  struct

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void BuildMainMenu(CVocApp *pMe) {

    // Set Title
   IMENUCTL_SetTitle(pMe->pIMenu, IVOCUSE_RES_FILE, IDSMAIN_TITLE, NULL); 

   // Add Streaming to IVocuse
   IMENUCTL_AddItem(pMe->pIMenu, IVOCUSE_RES_FILE, IDSMAIN_STREAM, IDSMAIN_STREAM, NULL, NULL); 

   // Add An info page
   IMENUCTL_AddItem(pMe->pIMenu, IVOCUSE_RES_FILE, IDSMAIN_INFO, IDSMAIN_INFO, NULL, NULL); 

   // Add an exit option
   IMENUCTL_AddItem(pMe->pIMenu, IVOCUSE_RES_FILE, IDSMAIN_EXIT, IDSMAIN_EXIT, NULL, NULL);    
}



/*===========================================================================

FUNCTION BuildSoftMenu

DESCRIPTION
	This function adds all menu items to the soft menu.  This menu is displayed
   when the application is streaming audio.

PROTOTYPE:
	void BuildSoftMenu(CVocApp *pMe);

PARAMETERS:
   pMe: [in]: Pointer to CVocApp  struct

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void BuildSoftMenu(CVocApp *pMe) {

   // Avoid memory leak
   if(pMe->pISoft) {
      IMENUCTL_Release (pMe->pISoft);
      pMe->pISoft = NULL;
   }

   // Create interface
   if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pMe->pISoft) != SUCCESS)
   {
      return;
   }

   // Add Activation to IVocuse - This must change when deactivated
   IMENUCTL_AddItem(pMe->pISoft, IVOCUSE_RES_FILE, IDSSOFT_DEACTIVATE, IDSSOFT_DEACTIVATE, NULL, NULL); 

   // Add an exit option
   IMENUCTL_AddItem(pMe->pISoft, IVOCUSE_RES_FILE, IDSSOFT_EXIT, IDSSOFT_EXIT, NULL, NULL); 
}


/*===========================================================================

FUNCTION ReplaceMenuItem

DESCRIPTION
	This function will remove the second parameter from a menu and add
   the third parameter to the menu.  The parameter corresponds to the 
   resource file supplied with this application.  If either parameter
   is 0, then it's corresponding removal ar addition is ignored.

PROTOTYPE:
	ReplaceMenuItem(IMenuCtl *pIMenu, uint16 remove, uint16 add);

PARAMETERS:
   pIMenu: [in]: Pointer to working menu
   remove: [in]: remove this item from menu, if 0 remove nothing
   add: [in]: add this item to the menu, if 0 add nothing

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
static void ReplaceMenuItem(IMenuCtl *pIMenu, uint16 remove, uint16 add) {
   
   // Remove Item if one is given
   if(remove) {
      IMENUCTL_DeleteItem(pIMenu, remove); 
   }
   
   // Add Item if one is given
   if(add) {
      IMENUCTL_AddItem(pIMenu, IVOCUSE_RES_FILE, add, add, NULL, NULL); 
   }

   // Move item up to beginning
   IMENUCTL_MoveItem(pIMenu, add, -1);

   // Set Cursor focus to added elem
   IMENUCTL_SetFocus(pIMenu, add);

   // Redraw Menu
   IMENUCTL_Redraw(pIMenu);
}
