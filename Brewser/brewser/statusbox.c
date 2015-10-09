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
#include "AEEStdLib.h"
#include "statusbox.h"
#include "util.h"

static const AECHAR dotdotdot[4] = { '.','.','.',0 }; /* sheesh */


void StatusBox_Redraw(StatusBox *me)
{
   /* don't draw anything if we're not around */
   if (!me->bActive || !me->bStarted) {
      return;
   }
   
   if ((IImage *)0 != me->piImage) {
      IIMAGE_DrawFrame(me->piImage,me->nFrame,
                       me->rcImage.x+1,me->rcImage.y+1);

      /* frame image */
      IDISPLAY_DrawRect(me->piDisplay,&me->rcImage,RGB_BLACK,0xbeef,
                        IDF_RECT_FRAME);
   }
   
   if (!me->bTextMeasured) {
      me->rcText.dx = IDISPLAY_MeasureText(me->piDisplay,
                                           AEE_FONT_NORMAL,me->pwsz)+4;
      me->rcText.dx = MIN(me->rcCenter.dx,me->rcText.dx);

      if (me->rcText.y == (me->rcImage.y + me->rcImage.dy - 1)) {
         me->rcText.dx = MAX(me->rcText.dx,me->rcImage.dx);
      }
      
      me->bTextMeasured = TRUE;
      
      me->rcText.x = me->rcCenter.x + ((me->rcCenter.dx - me->rcText.dx) / 2);
   }

   IDISPLAY_DrawText(me->piDisplay,AEE_FONT_NORMAL,
                     me->pwsz,-1,0,0,&me->rcText,
                     IDF_RECT_FILL|IDF_RECT_FRAME|
                     IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);

}

void StatusBox_Update(StatusBox *me,const char *cpszResFile, uint16 wId)
{
   me->bTextMeasured = FALSE;

   FREEIF(me->pStringData);
   /* if we're showing the text part */
   if (SUCCESS == 
       IShell_LoadResWsz(me->piShell,cpszResFile,wId, &me->pStringData)) {
      me->pwsz = me->pStringData;
   } else {
      me->pwsz = (AECHAR *)dotdotdot;
   }

   if (me->bStarted) {
      me->bActive = TRUE;
      me->pfnInvalidate(me->pInvalidateData);
   }
}

void StatusBox_Stop(StatusBox *me)
{
   me->bActive = FALSE;
   me->bStarted = FALSE;

   if ((IImage *)0 != me->piImage) {
      IIMAGE_Stop(me->piImage);
   }

   CALLBACK_Cancel(&me->cb);
   me->pfnInvalidate(me->pInvalidateData);
}

/**
  || Function
  || --------
  || static void StatusBox_Init(StatusBox *me)
  ||
  || Description
  || -----------
  || load the IImage, calculate rectangles
  ||
  || Parameters
  || ----------
  || StatusBox *me: the status box
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static void StatusBox_Init(StatusBox *me)
{
   AEEImageInfo ii;
   
   ZEROAT(&ii);

   /* re-init this stuff every time */
   if ((IImage *)0 != me->piImage) {
      
      IIMAGE_GetInfo(me->piImage, &ii);
      
      if ((ii.cx == 0) || (ii.cy == 0)) {
         /* IIMAGE barfed */
         RELEASEIF(me->piImage);
         ZEROAT(&ii);
      } else if (ii.bAnimated) {
         me->bSelfAnimated = TRUE;
         me->nFrame = -1;
         IIMAGE_SetParm(me->piImage, IPARM_REDRAW, 
                        (int)me->pfnInvalidate, (int)me->pInvalidateData);
      } else {

         uint16 uFrameWidth;

         if (me->uFrameWidth != 0) {
            uFrameWidth = me->uFrameWidth;
         } else { /* use the height, for square frames */
            uFrameWidth = ii.cy;
         }

         me->nFrames = MAX(1,ii.cx/uFrameWidth);
         
         if ((me->nFrames * uFrameWidth) != ii.cx) { 
            /* not exact frames, don't animate */
            me->nFrames = 1;
         } else {
            ii.cx = uFrameWidth; /* fake out rect calc stuff below */
         }

         if (0 == me->uFrameDuration) {
            me->uFrameDuration = 150;
         }
         
         IIMAGE_SetFrameCount(me->piImage, me->nFrames);
         me->nFrame = 0;
         /* 
            Don't do this, run animation myself for invalidation code to work

            animate over 1 second...
            IIMAGE_SetAnimationRate(me->piImage, 1000/nFrames); 
         */
      }
   }

   /* initialize status rectangles */
   
   /* if we found the image above and it's non-empty...*/
   if (0 != ii.cy) {
      me->rcImage.y = me->rcCenter.y + ((me->rcCenter.dy - ii.cy) / 2) - 1;
      me->rcImage.x = me->rcCenter.x + ((me->rcCenter.dx - ii.cx) / 2) - 1;
      /* insist on square */
      me->rcImage.dy = ii.cy + 2;
      me->rcImage.dx = ii.cx + 2;

      /* image too big for screen, fuggedaboudit */
      if ((me->rcImage.dy > me->rcCenter.dy) ||
          (me->rcImage.dx > me->rcCenter.dx)) {
         RELEASEIF(me->piImage);
         ZEROAT(&me->rcImage);
      }
   } else {/* no image, just text box... */
      ZEROAT(&me->rcImage);
   }

   {
      int nAscent;
      int nDescent;
   
      IDISPLAY_GetFontMetrics(me->piDisplay,AEE_FONT_NORMAL,
                              &nAscent,&nDescent);

      me->rcText.dy = nAscent + nDescent + 4;
      
   }

   if ((0 == me->rcImage.dy) || 
       (me->rcImage.dy + me->rcText.dy - 1 > me->rcCenter.dy)) {
      me->rcText.y = 
         me->rcCenter.y + ((me->rcCenter.dy - me->rcText.dy) / 2);
   } else {
      me->rcImage.y -= me->rcText.dy/2;
      me->rcText.y = me->rcImage.y + me->rcImage.dy - 1;      
   }

}

/**
  || Function
  || --------
  || static void StatusBox_AnimateTimer(void *p)
  ||
  || Description
  || -----------
  || timer tick for non-self-animated IImages
  ||
  || Parameters
  || ----------
  || void *p: the StatusBox
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static void StatusBox_AnimateTimer(void *p)
{
   StatusBox *me = (StatusBox *)p;

   if (++me->nFrame >= me->nFrames) {
      me->nFrame = 0;
   }

   me->pfnInvalidate(me->pInvalidateData);

   ISHELL_SetTimerEx(me->piShell,me->uFrameDuration,&me->cb);
}

void StatusBox_Start(StatusBox *me)
{
   if ((IImage *)0 != me->piImage) {
      if (me->bSelfAnimated) {
         IIMAGE_Start(me->piImage,me->rcImage.x+1,me->rcImage.y+1);
      } else {
         StatusBox_AnimateTimer(me);
      }
   }
   me->pwsz = (AECHAR *)dotdotdot;

   me->bStarted = TRUE;
}

void StatusBox_CtorZ(StatusBox *me,
                     IShell *piShell,IDisplay *piDisplay,
                     const char *cpszResFile, uint16 wResId,
                     AEERect *prcCenter,
                     PFNNOTIFY pfnInvalidate, 
                     void *pInvalidateData)
{
   me->piShell         = piShell;
   me->piDisplay       = piDisplay;
   me->pfnInvalidate   = pfnInvalidate;
   me->pInvalidateData = pInvalidateData;

   me->piImage = ISHELL_LoadResImage(piShell,cpszResFile,wResId);

   {
      uint32 nLen = sizeof(me->uFrameWidth)+sizeof(me->uFrameDuration);

      ISHELL_LoadResDataEx(piShell,cpszResFile,wResId,
                           RESTYPE_BINARY,
                           &me->uFrameWidth, /* uFrameDuration's right next */
                           &nLen);
   }
   
   CALLBACK_Init(&me->cb,StatusBox_AnimateTimer,me);

   me->rcCenter = *prcCenter;

   StatusBox_Init(me);
  
}

void StatusBox_Dtor(StatusBox *me)
{
   FREEIF(me->pStringData);
   CALLBACK_Cancel(&me->cb);
   RELEASEIF(me->piImage);
   ZEROAT(me);
}

