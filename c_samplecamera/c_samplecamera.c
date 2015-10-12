/*=============================================================================
FILE: c_samplecamera.c
=============================================================================*/

/*=============================================================================
This QUALCOMM Sample Code Disclaimer applies to the sample code of 
QUALCOMM Incorporated (“QUALCOMM”) to which it is attached or in which it is 
integrated (“Sample Code”).  Qualcomm is a trademark of, and may not be used 
without express written permission of, QUALCOMM.

Disclaimer of Warranty. Unless required by applicable law or agreed to in 
writing, QUALCOMM provides the Sample Code on an "as is" basis, without 
warranties or conditions of any kind, either express or implied, including, 
without limitation, any warranties or conditions of title, non-infringement, 
merchantability, or fitness for a particular purpose. You are solely 
responsible for determining the appropriateness of using the Sample Code and 
assume any risks associated therewith. PLEASE BE ADVISED THAT QUALCOMM WILL NOT
SUPPORT THE SAMPLE CODE OR TROUBLESHOOT ANY ISSUES THAT MAY ARISE WITH IT.

Limitation of Liability.  In no event shall QUALCOMM be liable for any direct, 
indirect, incidental, special, exemplary, or consequential damages (including, 
but not limited to, procurement of substitute goods or services; loss of use, 
data, or profits; or business interruption) however caused and on any theory
of liability, whether in contract, strict liability, or tort (including 
negligence or otherwise) arising in any way out of the use of the Sample Code 
even if advised of the possibility of such damage.
=============================================================================*/

/*=============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================*/
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions  
#include "aeestdlib.h"

#include "c_samplecamera.bid"

// add include for camera 
#include "AEECamera.h"
//
// Defines for the camera 
//
// These are for the text boxes
#define TEXT_HEIGHT 14
#define SQCIF_X 128
#define SQCIF_Y 96

// These define the file names used to store movies or snapshots
#define MOVIE_FILE_NAME "Movie.3g2"
#define SNAPSHOT_FILE_NAME "Snapshot.jpg"

//This enum is used to store which command is to be executed
typedef enum _CommandTypes
{
   RECORD_SNAPSHOT,
   RECORD_MOVIE,
   NO_COMMAND
}CommandTypes;
// end camera definitions

/*=============================================================================
Applet structure. All variables in here are referenced via "pMe->"
=============================================================================*/
// Create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _c_samplecamera {
    AEEApplet      Applet; // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo;// Always have access to the hardware device info
    IDisplay *pIDisplay; // Give a standard way to access the Display interface
    IShell   *pIShell;   // Give a standard way to access the Shell interface
    int nWidth;     // Stores the device screen width
    int nHeight;    // Stores the device screen height
    
	// Add your own variables here...
   ICamera           *pICamera;          // ICamera instance
   AEESize           ImageSize;          // size of the image to capture
   int32             nPreviewType;       // current preview type Snapshot/Movie
   boolean           bSupportZoom;       // does this phone support Zoom
   boolean           bCommandToExecute;  // is there a command to execute
   int               nCommandToExecute;  // which command should be executed
   boolean           bAdjustedPreviewRotate;  // Rotation adjustment for camera mount

   // extra defintions needed to store the location of various text blocks
   AEERect           Status1TextRect;   //first line of status "Mode:"
   AEERect           Status2TextRect;   //second line of status "File Name:"
   AEERect           ZoomStatusTextRect;   //zoom status "Zoom %d(%d to %d)
   AEERect           SoftKey1TextRect;  // Command for the first soft key
   AEERect           SoftKey2TextRect;  // Command for the second soft key
   AEERect           ZoomTextRect;      // Directions on setting the zoom

} c_samplecamera;

/*=============================================================================
Function Prototypes
=============================================================================*/
static  boolean c_samplecamera_HandleEvent(c_samplecamera* pMe, 
                                AEEEvent eCode, uint16 wParam, uint32 dwParam);
boolean c_samplecamera_InitAppData(c_samplecamera* pMe);
void    c_samplecamera_FreeAppData(c_samplecamera* pMe);
static void c_SampleCameraApp_Start(c_samplecamera* pMe );
static void c_SampleCameraApp_PrintStatus(c_samplecamera* pMe);
static void c_SampleCameraApp_SetMediaDataParameters(c_samplecamera* pMe,
												   int32 nPreviewType);
/*=============================================================================
FUNCTION DEFINITIONS
=============================================================================*/

/*=============================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION
    This function is invoked while the app is being loaded. All modules must 
    provide this function. Ensure to retain the same name and parameters for 
    this function. In here, the module must verify the ClassID and then invoke 
    the AEEApplet_New() function that has been provided in AEEAppGen.c. 

    After invoking AEEApplet_New(), this function can do app specific 
    initialization. In this example, a generic structure is provided so that 
    app developers need not change app specific initialization section every 
    time except for a call to IDisplay_InitAppData(). This is done as follows:
    InitAppData() is called to initialize AppletData instance. It is app 
    developers responsibility to fill-in app data initialization code of 
    InitAppData(). App developer is also responsible to release memory 
    allocated for data contained in AppletData. This is done in 
    IDisplay_FreeAppData().

PROTOTYPE:
    int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,
                            void ** ppObj)

PARAMETERS:
    clsID: [in]: Specifies the ClassID of the applet which is being loaded

    pIShell: [in]: Contains pointer to the IShell object. 

    pIModule: pin]: Contains pointer to the IModule object to the current 
     module to which this app belongs

    ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. 
     Allocation of memory for this structure and initializing the base data 
     members is done by AEEApplet_New().

DEPENDENCIES:
    none

RETURN VALUE
    AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() 
     invocation was successful
   
   EFAILED: If the app does not need to be loaded or if errors occurred in 
    AEEApplet_New(). If this function returns FALSE, the app will not load.

SIDE EFFECTS:
    none
=============================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, 
						 void **ppObj)
{
    *ppObj = NULL;

    if( AEECLSID_C_SAMPLECAMERA == ClsId ) {
        // Create the applet and make room for the applet structure
	    if( TRUE == AEEApplet_New(sizeof(c_samplecamera),
                        ClsId,
                        pIShell,
                        po,
                        (IApplet**)ppObj,
                        (AEEHANDLER)c_samplecamera_HandleEvent,
                        (PFNFREEAPPDATA)c_samplecamera_FreeAppData) ) {
            // FreeAppData is called after sending EVT_APP_STOP to HandleEvent
                     		
            // Initialize applet data, this is called before sending 
            // EVT_APP_START to the HandleEvent function
		    if(TRUE == c_samplecamera_InitAppData(
				                            (c_samplecamera*)*ppObj)) {
	            // Data initialized successfully
			    return(AEE_SUCCESS);
		    }
		    else {
                // Release the applet. This will free the memory allocated for 
                // the applet when AEEApplet_New was called.
                IApplet_Release((IApplet*)*ppObj);
                return (EFAILED);
            }
        } // End AEEApplet_New
    }
    return(EFAILED);
}

/*=============================================================================
FUNCTION c_samplecamera_HandleEvent

DESCRIPTION:
    This is the EventHandler for this app. All events to this app are handled 
    in this function. All APPs must supply an Event Handler.

PROTOTYPE:
    boolean SampleAppWizard_HandleEvent(IApplet * pi, AEEEvent eCode, 
                                      uint16 wParam, uint32 dwParam)

PARAMETERS:
    pi: Pointer to the AEEApplet structure. This structure contains information 
     specific to this applet. It was initialized during AEEClsCreateInstance().
  
    ecode: Specifies the Event sent to this applet
     wParam, dwParam: Event specific data.

DEPENDENCIES:
    none

RETURN VALUE:
    TRUE: If the app has processed the event
    FALSE: If the app did not process the event

SIDE EFFECTS:
    none
=============================================================================*/
static boolean c_samplecamera_HandleEvent(c_samplecamera* pMe, 
								AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	int nErr = AEE_EFAILED;
   	int32 nMode = 0;
    switch (eCode) {
        // App is told it is starting up
        case EVT_APP_START:                        
	        // Call the camera start function
	        DBGPRINTF("Starting the Application");
      		c_SampleCameraApp_Start(pMe);
            return(TRUE);         

        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...
      	    return(TRUE);

        // App is being suspended 
        case EVT_APP_SUSPEND:
            // Add your code here...
      	    return(TRUE);

        // App is being resumed
        case EVT_APP_RESUME:
            // Add your code here...
      	    return(TRUE);

        // An SMS message has arrived for this app. 
        // The Message is in the dwParam above as (char *).
        // sender simply uses this format "//BREW:ClassId:Message", 
        // example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
	        // Add your code here...
      	    return(TRUE);

        // A key was pressed. Look at the wParam above to see which key was 
        // pressed. The key codes are in AEEVCodes.h. Example "AVK_1" means 
        // that the "1" key was pressed.
        case EVT_KEY:
	        switch (wParam) {
					case AVK_UP:  // Zoom controls
					case AVK_DOWN:
					{
						if (pMe->bSupportZoom)
						{
							AEEParmInfo ZoomInfo;
							int32       nCurrentZoom;

							// Retrieve the current zoom settings 
							nErr = ICAMERA_GetParm(pMe->pICamera, CAM_PARM_ZOOM, 
                                      &nCurrentZoom, (int32 *)&ZoomInfo);
							DBGPRINTF("ICAMERA_GetParm CAM_PARM_ZOOM %d ", nErr);
							if (!nErr)
							{
								if (wParam == AVK_UP)
									nErr = ICAMERA_SetParm(pMe->pICamera, CAM_PARM_ZOOM, 
                                            nCurrentZoom + ZoomInfo.nStep, NULL);
								else
									nErr = ICAMERA_SetParm(pMe->pICamera, CAM_PARM_ZOOM, 
                                            nCurrentZoom - ZoomInfo.nStep, NULL);
								DBGPRINTF("ICAMERA_SetParm CAM_PARM_ZOOM %d ", nErr);
								return TRUE;
							}
						}
						break;
					}
					case AVK_SOFT1:
					{
						//Depending on the current mode of the camera, the softkeys command different things
						ICAMERA_GetMode(pMe->pICamera, &nMode, NULL);
						if (nMode == CAM_MODE_READY)
						{
							// if the mode is Ready, toggle the camera preview type
							if (pMe->nPreviewType == CAM_PREVIEW_MOVIE)
								c_SampleCameraApp_SetMediaDataParameters(pMe, CAM_PREVIEW_SNAPSHOT);
							else
								c_SampleCameraApp_SetMediaDataParameters(pMe, CAM_PREVIEW_MOVIE);
								c_SampleCameraApp_PrintStatus(pMe);
						}
						else
						{
							// if the mode is not ready, allow the user to stop what is happening
							nErr = ICAMERA_Stop(pMe->pICamera);
							DBGPRINTF("ICAMERA_Stop %d", nErr);
						}

						break; 
					}
					case AVK_SOFT2:
					{
						//Depending on the current mode of the camera, the softkeys command different things
						ICAMERA_GetMode(pMe->pICamera, &nMode, NULL);
						switch (nMode)
						{
							case CAM_MODE_READY:
							{
								//if the mode is ready allow the user to change it to preview
								nErr = ICAMERA_Preview(pMe->pICamera);
								DBGPRINTF("ICAMERA_Preview = %d ", nErr);
								break;
							}
							case CAM_MODE_PREVIEW:
							{
								// Store the command to execute, and wait till the camera has 
								// transitioned into the ready state
								pMe->bCommandToExecute = TRUE;
								if (pMe->nPreviewType == CAM_PREVIEW_MOVIE)
									pMe->nCommandToExecute = RECORD_MOVIE;
								else
									pMe->nCommandToExecute = RECORD_SNAPSHOT;
								break;
							}
						}
						if (pMe->bCommandToExecute )
						{
							//Call _Stop to transition the camera to the ready state
							nErr = ICAMERA_Stop(pMe->pICamera);
							DBGPRINTF("ICAMERA_Stop %d", nErr);
						}
						break;
					}
			}  

      	    return(TRUE);

        // Clamshell has opened/closed
        // wParam = TRUE if open, FALSE if closed
        case EVT_FLIP:
            // Add your code here...
            return(TRUE);
      
	    // Clamtype device is closed and reexposed when opened, and LCD 
        // is blocked, or keys are locked by software. 
        // wParam = TRUE if keygaurd is on
        case EVT_KEYGUARD:
            // Add your code here...
            return (TRUE);

        // If nothing fits up to this point then we'll just break out
        default:
            break;
    }
    return (FALSE);
}

/*=============================================================================
FUNCTION c_SampleCameraApp_InitAppData

DESCRIPTION
  This function is called when your application is starting up.  It will check
  for device info and then initialize different values accordingly.

PROTOTYPE:
  boolean c_SampleCameraApp_InitAppData(SampleCameraApp* pMe)

PARAMETERS:
   pMe: Pointer to the applet data stucture

DEPENDENCIES
   none

RETURN VALUE
   TRUE: If everything is initialized correctly
   FALSE: If the function is unable to initialize the application data

SIDE EFFECTS
   none
=============================================================================*/
boolean c_samplecamera_InitAppData(c_samplecamera* pMe)
{
    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->Applet.m_pIShell,&pMe->DeviceInfo);
    pMe->pIDisplay = pMe->Applet.m_pIDisplay;
    pMe->pIShell   = pMe->Applet.m_pIShell;
    pMe->nWidth =  pMe->DeviceInfo.cxScreen; // Cache width of device screen
    pMe->nHeight = pMe->DeviceInfo.cyScreen; // Cache height of device screen
    
	// Insert your code here for initializing or allocating resources...
if (&pMe->DeviceInfo.cxScreen > &pMe->DeviceInfo.cyScreen)
   {
      DBGPRINTF("ERROR: Landscape screen - Not yet supported by Application");
      return FALSE;
   }
   else
   {
      //portrait screen
      if (pMe->DeviceInfo.cxScreen >= SQCIF_X && pMe->DeviceInfo.cyScreen >= SQCIF_Y)
      {
         pMe->ImageSize.cx = SQCIF_X;
         pMe->ImageSize.cy = SQCIF_Y; 
      }

      // Organize the screen into several text boxes.  These text boxes will 
      // display status as well as which keys control which functions
      pMe->Status1TextRect.x = 0;
      pMe->Status1TextRect.y = SQCIF_Y ;
      pMe->Status1TextRect.dx = pMe->DeviceInfo.cxScreen;
      pMe->Status1TextRect.dy = TEXT_HEIGHT;
      pMe->ZoomStatusTextRect.x = 0;
      pMe->ZoomStatusTextRect.y = SQCIF_Y + TEXT_HEIGHT;
      pMe->ZoomStatusTextRect.dx = pMe->DeviceInfo.cxScreen;
      pMe->ZoomStatusTextRect.dy = TEXT_HEIGHT;
      pMe->Status2TextRect.x = 0;
      pMe->Status2TextRect.y = SQCIF_Y + TEXT_HEIGHT * 2;
      pMe->Status2TextRect.dx = pMe->DeviceInfo.cxScreen;
      pMe->Status2TextRect.dy = TEXT_HEIGHT;

      pMe->SoftKey1TextRect.x = 0;
      pMe->SoftKey1TextRect.y = pMe->DeviceInfo.cyScreen - TEXT_HEIGHT;
      pMe->SoftKey1TextRect.dx = pMe->DeviceInfo.cxScreen*2/3;
      pMe->SoftKey1TextRect.dy = TEXT_HEIGHT;
      pMe->SoftKey2TextRect.x = pMe->DeviceInfo.cxScreen/3;
      pMe->SoftKey2TextRect.y = pMe->DeviceInfo.cyScreen - TEXT_HEIGHT;
      pMe->SoftKey2TextRect.dx = pMe->DeviceInfo.cxScreen*2/3;
      pMe->SoftKey2TextRect.dy = TEXT_HEIGHT;

      pMe->ZoomTextRect.x = pMe->DeviceInfo.cxScreen/4;
      pMe->ZoomTextRect.y = pMe->DeviceInfo.cyScreen - TEXT_HEIGHT * 2;
      pMe->ZoomTextRect.dx = pMe->DeviceInfo.cxScreen*3/4;
      pMe->ZoomTextRect.dy = TEXT_HEIGHT;
   }

   pMe->bCommandToExecute = FALSE;
   pMe->nCommandToExecute = NO_COMMAND;
   pMe->bAdjustedPreviewRotate = FALSE;

    // If there have been no failures up to this point then return success
    return (TRUE);
}

// This function is called when your application is exiting
void c_samplecamera_FreeAppData(c_samplecamera* pMe)
{
    // Insert your code here for freeing any resources you have allocated...

    if (pMe->pICamera != NULL)
   {
      ICAMERA_Release(pMe->pICamera);
      pMe->pICamera = NULL;
	}
}
/*=============================================================================
FUNCTION c_SampleCameraApp_SetMediaDataParameters

DESCRIPTION
  This function will set up ICamera parameters according to the preview type 
  that is passed in.

PROTOTYPE:
  static void c_SampleCameraApp_SetMediaDataParameters(SampleCameraApp* pMe, 
                                                   int32 nPreviewType)

PARAMETERS:
   pMe: Pointer to the applet data stucture
  
   nPreviewType:  this preview type is either CAM_PREVIEW_SNAPSHOT or 
      CAM_PREVIEW_MOVIE

DEPENDENCIES
   none

RETURN VALUE
   none

SIDE EFFECTS
   none
=============================================================================*/
static void c_SampleCameraApp_SetMediaDataParameters(c_samplecamera* pMe, 
                                                   int32 nPreviewType)
{
   AEEMediaData   md;
   int nErr = AEE_EFAILED;

   //Set the preview type
   nErr = ICAMERA_SetParm(pMe->pICamera, CAM_PARM_PREVIEW_TYPE, 
                          nPreviewType, 0);
   pMe->nPreviewType = nPreviewType;

   if (nPreviewType == CAM_PREVIEW_SNAPSHOT)
   {
      //set media data output to a file with the name SNAPSHOT_FILE_NAME
      md.clsData = MMD_FILE_NAME;
      md.pData = (void *)SNAPSHOT_FILE_NAME;
      md.dwSize = 0;
      nErr = ICAMERA_SetMediaData(pMe->pICamera, &md, "image/jpeg");
   }
   else //nPreviewType == CAM_PREVIEW_MOVIE
   {
      //Videos require video and audio encoding 
      nErr = ICAMERA_SetVideoEncode(pMe->pICamera, AEECLSID_MEDIAMPEG4, 0);
      if (nErr)
         DBGPRINTF("ICAMERA_SetVideoEncode %d",nErr);
      nErr = ICAMERA_SetAudioEncode(pMe->pICamera, AEECLSID_MEDIAQCP, 0);
      if (nErr)
         DBGPRINTF("ICAMERA_SetAudioEncode %d",nErr);

      // set the media data to output to a file with the name MOVIE_FILE_NAME
      md.clsData = MMD_FILE_NAME;
      md.pData = (void *)MOVIE_FILE_NAME;
      md.dwSize = 0;
      nErr = ICAMERA_SetMediaData(pMe->pICamera, &md, CAM_FILE_FORMAT_3G2);
   }
   if (nErr == AEE_EBADPARM)
      DBGPRINTF("App may not have permissions to this directory", nErr);
   else if (nErr)
      DBGPRINTF("Unable to set media data %d",nErr);
}

/*=============================================================================
FUNCTION c_SampleCameraApp_GetModeStr

DESCRIPTION
  This will return a pointer to a string that stores the current camera mode

PROTOTYPE:
  static char * c_SampleCameraApp_GetModeStr(SampleCameraApp* pMe)

PARAMETERS:
   pMe: Pointer to the applet data stucture
  
DEPENDENCIES
   none

RETURN VALUE
   The pointer to the string

SIDE EFFECTS
   none
=============================================================================*/
static char * c_SampleCameraApp_GetModeStr(c_samplecamera* pMe)
{
   char * pszMode = NULL;
   int32  nMode = 0;
   int nErr = AEE_EFAILED;

   //Get the current camera mode and return the corresponding string
   nErr = ICAMERA_GetMode(pMe->pICamera, &nMode, NULL);
   if (nErr)
      DBGPRINTF("Unable to fetch mode %d",nErr);
   switch (nMode)
   {
   case CAM_MODE_READY:    
      pszMode = "READY";       
      break;
   case CAM_MODE_PREVIEW:  
      //If the mode is preview, include extra information about the preview type
      if (pMe->nPreviewType == CAM_PREVIEW_SNAPSHOT )
         pszMode = "PREVIEW for a Snapshot";
      else
         pszMode = "PREVIEW for a Movie";
      break;
   case CAM_MODE_SNAPSHOT: 
      pszMode = "SNAPSHOT";    
      break;
   case CAM_MODE_MOVIE:    
      pszMode = "MOVIE";       
      break;
   default: 
      pszMode = "unknown"; 
      DBGPRINTF("mode is %d",nMode);  
      break;
   }
   return pszMode;
}

/*=============================================================================
FUNCTION c_SampleCameraApp_PrintStatus

DESCRIPTION
  This is a helper function that will print to the screen the status for the 
  application as well as the command that the soft keys control.   

PROTOTYPE:
  static void c_SampleCameraApp_PrintStatus(SampleCameraApp* pMe)

PARAMETERS:
   pMe: Pointer to the applet data stucture

DEPENDENCIES
   none

RETURN VALUE
   none

SIDE EFFECTS
   none
=============================================================================*/
static void c_SampleCameraApp_PrintStatus(c_samplecamera* pMe)
{
   int nErr = AEE_EFAILED;
   char *pszBuf = NULL;
   char pszBuf2[100];
   AECHAR wszBuf[100];
   int32 nMode = 0;

   //clear the screen 
   IDisplay_ClearScreen(pMe->Applet.m_pIDisplay);

   //If zoom is supported update the screen with Zoom information
   if (pMe->bSupportZoom)
   {
      AEEParmInfo ZoomInfo;
      int32       nCurrentZoom;

      // retrieve the current setting and the parameter information
      nErr = ICAMERA_GetParm(pMe->pICamera, CAM_PARM_ZOOM, 
                             &nCurrentZoom, (int32 *)&ZoomInfo);
      if (nErr)
         DBGPRINTF("Unable to get CAM_PARM_ZOOM %d", pMe->nPreviewType);

      // create a string with the current value and the range of possible values
      SNPRINTF(pszBuf2, sizeof(pszBuf2), "Zoom %d(%d to %d)", 
               nCurrentZoom, ZoomInfo.nMin, ZoomInfo.nMax);
      STRTOWSTR(pszBuf2, wszBuf, sizeof(wszBuf));
      IDisplay_DrawText(pMe->Applet.m_pIDisplay, AEE_FONT_NORMAL, 
                        wszBuf, -1, pMe->ZoomStatusTextRect.x, 
                        pMe->ZoomStatusTextRect.y, &pMe->ZoomStatusTextRect,
                        IDF_ALIGN_LEFT | IDF_ALIGN_TOP);

      //print our directions on how to control the zoom
      STRTOWSTR("Up/Dn to Zoom", wszBuf, sizeof(wszBuf));
      IDisplay_DrawText(pMe->Applet.m_pIDisplay, AEE_FONT_NORMAL, 
                        wszBuf, -1, pMe->ZoomTextRect.x, pMe->ZoomTextRect.y,
                        &pMe->ZoomTextRect, IDF_ALIGN_CENTER | IDF_ALIGN_TOP);
   }

   //Print out the current camera mode to the screen
   pszBuf = c_SampleCameraApp_GetModeStr(pMe);
   SNPRINTF(pszBuf2, sizeof(pszBuf2), "Mode: %s", pszBuf);
   STRTOWSTR(pszBuf2, wszBuf, sizeof(wszBuf));
   IDisplay_DrawText(pMe->Applet.m_pIDisplay, AEE_FONT_NORMAL, wszBuf, 
                     -1, pMe->Status1TextRect.x, pMe->Status1TextRect.y, 
                     &pMe->Status1TextRect, IDF_ALIGN_LEFT | IDF_ALIGN_TOP);
   // Depending on the preview type display the file name that will store the 
   // output of the camera's snapshot or video
   if (pMe->nPreviewType == CAM_PREVIEW_MOVIE)
      SNPRINTF(pszBuf2, sizeof(pszBuf2), "File Name: %s", MOVIE_FILE_NAME);
   else if (pMe->nPreviewType == CAM_PREVIEW_SNAPSHOT)
      SNPRINTF(pszBuf2, sizeof(pszBuf2), "File Name: %s", SNAPSHOT_FILE_NAME);
   STRTOWSTR(pszBuf2, wszBuf, sizeof(wszBuf));
   IDisplay_DrawText(pMe->Applet.m_pIDisplay, AEE_FONT_NORMAL, wszBuf, 
                     -1, pMe->Status2TextRect.x, pMe->Status2TextRect.y, 
                     &pMe->Status2TextRect, IDF_ALIGN_LEFT | IDF_ALIGN_TOP);

   //Get the current mode of the camera, and update the screen to specify what
   // commands the first soft key controls
   ICAMERA_GetMode(pMe->pICamera, &nMode, NULL);
   if (nMode == CAM_MODE_READY)
   {
      if (pMe->nPreviewType == CAM_PREVIEW_MOVIE)
         pszBuf = "Switch to Snapshot";
      else
         pszBuf = "Switch to Movie";
   }
   else
      pszBuf = "Stop";
   STRTOWSTR(pszBuf, wszBuf, sizeof(wszBuf));
   IDisplay_DrawText(pMe->Applet.m_pIDisplay, AEE_FONT_NORMAL, wszBuf, 
                     -1, pMe->SoftKey1TextRect.x, pMe->SoftKey1TextRect.y, 
                     &pMe->SoftKey1TextRect, IDF_ALIGN_LEFT | IDF_ALIGN_BOTTOM);

   //Update the screen to specify what commands the second soft key controls
   switch (nMode)
   {
   case CAM_MODE_READY:
      pszBuf = "Preview";
      break;
   case CAM_MODE_PREVIEW:
      {
         if (pMe->nPreviewType == CAM_PREVIEW_MOVIE)
            pszBuf = "RecordMovie";
         else
            pszBuf = "RecordSnapshot";
         break;
      }
   case CAM_MODE_MOVIE:
      pszBuf = "";
      break;
   case CAM_MODE_SNAPSHOT:
      pszBuf = "";
      break;
   }
   STRTOWSTR(pszBuf, wszBuf, sizeof(wszBuf));
   IDisplay_DrawText(pMe->Applet.m_pIDisplay, AEE_FONT_NORMAL, wszBuf, 
                     -1, pMe->SoftKey2TextRect.x, pMe->SoftKey2TextRect.y, 
                     &pMe->SoftKey2TextRect, IDF_ALIGN_RIGHT | IDF_ALIGN_BOTTOM);

   // Update the screen with all of the changes
   IDisplay_Update(pMe->Applet.m_pIDisplay);
}

/*=============================================================================
FUNCTION c_SampleCameraApp_CameraCBFunc

DESCRIPTION
  This callback notifier function handles all callbacks from ICamera.  
     Depending on how complex your application this function may need to handle
     several use cases.  The most common uses are error handling, state 
     transistion, displaying frames, and monitoring status.  

PROTOTYPE:
  static void c_SampleCameraApp_CameraCBFunc(SampleCameraApp* pMe, 
                                         AEECameraNotify * pcn)

PARAMETERS:
   pMe: Pointer to the applet data stucture
  
   pcn: This pointer to a AEECameraNotify structure contains information about
      the ICamera object.  

DEPENDENCIES
   none

RETURN VALUE
   none

SIDE EFFECTS
   none
=============================================================================*/
static void c_SampleCameraApp_CameraCBFunc(c_samplecamera* pMe, 
                                         AEECameraNotify * pcn)
{
   int nErr = AEE_EFAILED;

   // First validate the callback...
   if (!pMe || !pcn)
   {
      DBGPRINTF("NULL ptr in callback.");
      return;
   }

   c_SampleCameraApp_PrintStatus(pMe);
   
   switch (pcn->nStatus)
   {
   case CAM_STATUS_FRAME:
      {
         // A new frame is available, Get the raw frame, check orientation and blit it to the screen
         IBitmap * pFrame;
         AEEBitmapInfo BitmapInfo;

         ICAMERA_GetFrame( pMe->pICamera, &pFrame );
         if (!pFrame) break;

         
         //Check to see if the raw frame does not have the same orientation 
         // as what expected.  If they are not the same, rotate the preview so
         // that the final image on the screen is rotated correctly.  This 
         // will handle the different mounting possitions of the camera.
         IBitmap_GetInfo(pFrame, &BitmapInfo, sizeof(AEEBitmapInfo));
         
         if (BitmapInfo.cx == pMe->ImageSize.cy && BitmapInfo.cy == pMe->ImageSize.cx )
         {
            DBGPRINTF("Frame is %d by %d.  Expected %d by %d", BitmapInfo.cx, BitmapInfo.cy, pMe->ImageSize.cx, pMe->ImageSize.cy );
            if (!pMe->bAdjustedPreviewRotate)
            {
               nErr = ICAMERA_SetParm(pMe->pICamera, CAM_PARM_ROTATE_PREVIEW, 90, 0);
               DBGPRINTF("Rotating the image - nErr is %d", nErr);
               pMe->bAdjustedPreviewRotate = TRUE;
            }
         }
         IDISPLAY_BitBlt(pMe->Applet.m_pIDisplay, 0, 0,
                         pMe->ImageSize.cx, pMe->ImageSize.cy,  
                         pFrame, 0, 0, AEE_RO_COPY);

         //Display buffer is now updated, need to update the screen
         IDISPLAY_Update(pMe->Applet.m_pIDisplay);
         IBITMAP_Release(pFrame);
         break;
      }
   case CAM_STATUS_DONE:
      {
         //if there is a command to execute, call the appropriate function 
         if (pMe->bCommandToExecute)
         {
            pMe->bCommandToExecute = FALSE;
            switch (pMe->nCommandToExecute)
            {
            case RECORD_SNAPSHOT:
               {
                  nErr = ICAMERA_RecordSnapshot(pMe->pICamera);
                  DBGPRINTF("ICAMERA_RecordSnapshot %d", nErr);
                  break;
               }
            case RECORD_MOVIE:
               {
                  nErr = ICAMERA_RecordMovie(pMe->pICamera);
                  DBGPRINTF("ICAMERA_RecordMovie %d", nErr);
                  break;
               }
            }
            pMe->nCommandToExecute = NO_COMMAND;
         }
         break;
      }

   }

}


/*=============================================================================
FUNCTION c_SampleCameraApp_Start

DESCRIPTION
  This is function is creates an ICamera object, sets up some parameters and 
    initializes the application into a known state.

PROTOTYPE:
  static void c_SampleCameraApp_Start(c_samplecamera* pMe )

PARAMETERS:
   pMe: Pointer to the applet data stucture
  
   e: adf.

DEPENDENCIES
   none

RETURN VALUE
   TRUE: If 
   FALSE: If 

SIDE EFFECTS
   none
=============================================================================*/
static void c_SampleCameraApp_Start(c_samplecamera* pMe )
{
   int nErr = AEE_EFAILED;

   // Create each of the controls used by the application.
   nErr = ISHELL_CreateInstance(pMe->Applet.m_pIShell, AEECLSID_CAMERA,
                                (void**)( &pMe->pICamera));
   if (nErr != AEE_SUCCESS)
   {
      DBGPRINTF("Creating ICamera Failed %d", nErr);
      if (nErr == AEE_EPRIVLEVEL)
      {
         DBGPRINTF("Check if MIF has permissions to create AEECLSID_CAMERA");
      }
      
	  if (nErr == AEE_EFAILED)
      {
		  // If a camera is not attached, on the simulator, close the app,
		  // attach a camera to the computer, and then on the simuator, use the app manager to 
		  // run the app. You don't have to stop and restart the debugging session.
         DBGPRINTF("Attach a camera to the PC and restart the sample application.");
      }
      return;
   }

   nErr = ICAMERA_GetParm(pMe->pICamera, CAM_PARM_IS_SUPPORT, 
                          (int32 *)CAM_PARM_ZOOM, (int32 *)&pMe->bSupportZoom);
   if (pMe->bSupportZoom != TRUE)
   {
      DBGPRINTF("Zoom is not supported on this build");
   }

   //Register for Notify Callback
   ICAMERA_RegisterNotify(pMe->pICamera, 
                          (PFNCAMERANOTIFY) c_SampleCameraApp_CameraCBFunc, 
                          (void *)pMe);

   //Set the size of the image to be recorded by the camera.
   nErr = ICAMERA_SetSize(pMe->pICamera, &pMe->ImageSize);

   //Set the size of the image to be displayed to the screen.
   nErr = ICAMERA_SetDisplaySize(pMe->pICamera, &pMe->ImageSize);

   //Initialize the application into the preview snapshot mode
   c_SampleCameraApp_SetMediaDataParameters(pMe, CAM_PREVIEW_SNAPSHOT);

   //Call helper function to print messages to the screen.
   c_SampleCameraApp_PrintStatus(pMe);
} // end of static void c_SampleCameraApp_Start(SampleCameraApp* pMe )



