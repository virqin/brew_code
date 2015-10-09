#ifndef STATUSBOX_H
#define STATUSBOX_H
/*
  ===========================================================================
  
  FILE: statusbox.c
   
  SERVICES:  
      A simple status box control
   
  DESCRIPTION: 
     Statusbox can show an animated or filmstrip-style image and some 
     text, all loaded via BREW resources.  If the image cannot be loaded,
     only the text is shown.
  
  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/
#include "AEEShell.h"
#include "AEEDisp.h"
#include "AEEImage.h"


/* =======================================================================
   StatusBox stuff
   ======================================================================= */
typedef struct StatusBox
{
   /* under control of owner (for now) */
   IShell       *piShell;
   IDisplay     *piDisplay;
   IImage       *piImage;

   /* under control of StatusBox */
   int           nFrame;
   int           nFrames;
   AEERect       rcCenter;
   AEERect       rcImage;
   AEERect       rcText;
   uint16        uFrameWidth;
   uint16        uFrameDuration; /* this has to be right after uFrameWidth */
   unsigned      bReady:1;
   unsigned      bStarted:1;
   unsigned      bTextMeasured:1;
   unsigned      bSelfAnimated:1;
   unsigned      bActive:1;
   
   AECHAR       *pwsz;
   AECHAR       *pStringData;
               
   PFNNOTIFY     pfnInvalidate;
   void         *pInvalidateData;

   AEECallback  cb;

} StatusBox;


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */


/**
  || Function
  || --------
  || void StatusBox_CtorZ(StatusBox *me,
  ||                      IShell *piShell,IDisplay *piDisplay,
  ||                      const char *cpszResFile, uint16 wResId,
  ||                      AEERect *prcCenter,
  ||                      PFNNOTIFY pfnInvalidate, 
  ||                      void *pInvalidateData);
  || 
  ||
  || Description
  || -----------
  || initialize a StatusBox
  ||
  || Parameters
  || ----------
  || StatusBox *me: zero-initialized memory big enough for a StatusBox
  || IShell *piShell: a BREW IShell
  || IDisplay *piDisplay: a BREW IDisplay
  || const char *cpszResFile: where to get the image to show
  || uint16 wResId: resource id of image
  || AEERect *prcCenter: rectangle to center StatusBox drawing in
  || PFNNOTIFY pfnInvalidate: where to call to request a redraw
  || void *pInvalidateData: with what
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  || StatusBox_CtorZ loads the IImage, if possible.  If the image is 
  ||  non-self-animated, StatusBox assumes it's a film strip whose frame 
  ||  width and duration (in ms) is stored in the resource file under the 
  ||  same id, as RESTYPE_BINARY (0x5000), as 2 16-bit values.  If the 
  ||  width is not found or is zero, StatusBox tries to use the height 
  ||  of the image as the frame width, unless the width of the image is 
  ||  not a multiple of the image height, in which case theres no 
  ||  animation.  If duration is 0 or unset, 150ms is used.
  ||
  */
void StatusBox_CtorZ(StatusBox *me,
                     IShell *piShell,IDisplay *piDisplay,
                     const char *cpszResFile, uint16 wResId,
                     AEERect *prcCenter,
                     PFNNOTIFY pfnInvalidate, 
                     void *pInvalidateData);

/**
  || Function
  || --------
  || void StatusBox_Dtor(StatusBox *me);
  ||
  || Description
  || -----------
  || cleans up after a StatusBox
  ||
  || Parameters
  || ----------
  || StatusBox *me: the StatusBox
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  || does not free me
  ||
  */
void StatusBox_Dtor(StatusBox *me);

/**
  || Function
  || --------
  || void StatusBox_Redraw(StatusBox *me);
  ||
  || Description
  || -----------
  || Redraws the StatusBox, if showing
  ||
  || Parameters
  || ----------
  || StatusBox *me: the StatusBox
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
void StatusBox_Redraw(StatusBox *me);
   
/**
  || Function
  || --------
  || void StatusBox_Update(StatusBox *me,const char *cpszResFile,uint16 wId);
  ||
  || Description
  || -----------
  || re-sets the text of the StatusBox to the string referenced by 
  ||  cpszResFile, wId
  ||
  || Parameters
  || ----------
  || StatusBox *me: the StatusBox
  || const char *cpszResFile: file to pass to ISHELL_LoadResString()
  || uint16 wId: id to pass to ISHELL_LoadResString()
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
void StatusBox_Update(StatusBox *me,const char *cpszResFile, uint16 wId);
   
/**
  || Function
  || --------
  || void StatusBox_Start(StatusBox *me);
  || void StatusBox_Stop(StatusBox *me);
  ||
  || Description
  || -----------
  || shows/starts or hides/stops a status box
  ||
  || Parameters
  || ----------
  || StatusBox *me: the StatusBox
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  || these function does not affect the current status string
  ||
  */
void StatusBox_Start(StatusBox *me);
void StatusBox_Stop(StatusBox *me);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */
#endif /* #ifndef STATUSBOX_H */
