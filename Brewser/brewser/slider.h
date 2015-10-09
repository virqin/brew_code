#ifndef SLIDER_H
#define SLIDER_H
/*
  ===========================================================================

  FILE:  slider.h
  
  SERVICES:  
    Slider
  
  GENERAL DESCRIPTION:
   A "Slider" is an object that changes a signed short from one value
    to another (half the distance between them each step) over some
    amount of time, while calling an update function for each change.
  
  REVISION HISTORY: 
    Thu Dec 12 11:21:50 2002: Created

  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/

#include "AEEShell.h"

typedef struct Slider
{
   int16       nTarget;     /* where I'm going */
   int16      *pnInc;       /* current value, and what to change */

   PFNNOTIFY   pfnInc;      /* where to call with each update */
   PFNNOTIFY   pfnDone;     /* where to call when done */
   void       *pNotifyData; /* with what */

   IShell     *piShell;     /* shell for timers */

   uint32      ulDelay;     /* *** modified by Slider ***
                               input: if positive, holds total delay, step 
                               delay is calculated 
                               if negative then holds delay per step 
                            */
   AEECallback cb;          /* my timer callback */
} Slider;

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**
  || Function
  || --------
  || void Slider_CtorZ(Slider *me, IShell *piShell, int16 *pnInc, 
  ||                   PFNNOTIFY pfnInc, void *pNotifyData)
  ||
  || Description
  || -----------
  || initialize a new Slider
  ||
  || Parameters
  || ----------
  || Slider *me: Slider memory, zero-initialized
  || IShell *piShell: a BREW IShell implementation
  || int16 *pnInc: sentinel of value to change during each slide
  || PFNNOTIFY pfnInc: where to call each time *pnInc is updated
  || void *pNotifyData: what to call pfnInc and 
  ||                      pfnDone (from Slider_Start()) with
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
void Slider_CtorZ(Slider *me, IShell *piShell, int16 *pnInc, 
                  PFNNOTIFY pfnInc, void *pNotifyData);

/**
  || Function
  || --------
  || void Slider_Start(Slider *me, int16 nTarget, PFNNOTIFY pfnDone, 
  ||                   uint32 ulDelay)
  ||
  || Description
  || -----------
  || starts a slide
  ||
  || Parameters
  || ----------
  || Slider *me: the Slider
  || int16 nTarget: where we're going
  || PFNNOTIFY pfnDone: done function, called with pNotifyData supplied to
  ||                       Slider_CtorZ
  || uint32 ulDelay: How long, in milliseconds, the slide should take
  ||                  Positive values are interpreted as total delay,
  ||                  negative values are interpreted as a delay between 
  ||                  each step.
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
void Slider_Start(Slider *me, int16 nTarget, PFNNOTIFY pfnDone, 
                  uint32 ulDelay);

/**
  || Function
  || --------
  || void Slider_Stop(Slider *me);
  ||
  || Description
  || -----------
  || stops a slide
  ||
  || Parameters
  || ----------
  || Slider *me: the Slider
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  || Sliders stop automatically, you need only call this in 
  ||  circumstances where you don't want to get to your target
  ||
  || It's unnecessary to call Slider_Stop() prior to Slider_Dtor()
  ||
  */
void Slider_Stop(Slider *me);

/**
  || Function
  || --------
  || boolean Slider_Sliding(Slider *me)
  ||
  || Description
  || -----------
  || whether the Slider's running
  ||
  || Parameters
  || ----------
  || Slider *me: the Slider
  ||
  || Returns
  || -------
  || TRUE if Start() has been called and the "done" function hasn't, and 
  ||    we haven't been cancelled, FALSE otherwise.
  ||
  || Remarks
  || -------
  ||
  */
boolean Slider_Sliding(Slider *me);

/**
  || Function
  || --------
  || void Slider_Dtor(Slider *me)
  ||
  || Description
  || -----------
  || Slider cleanup
  ||
  || Parameters
  || ----------
  || Slider *me: the slider
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  || doesn't free *me, just cancels and zero's memory
  ||
  */
void Slider_Dtor(Slider *me);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef SLIDER_H */
