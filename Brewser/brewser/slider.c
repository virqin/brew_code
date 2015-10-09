/*
  ===========================================================================
  
  FILE: slider.c
   
  SERVICES:  
    Slider!
   
  DESCRIPTION: 
   A "Slider" is an object that changes a signed short from one value
    to another (half the distance between them each step) over some
    amount of time, while calling an update function for each change.

  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/

#include "AEEStdlib.h"
#include "slider.h"


/**
  || Function
  || --------
  || static void Slider_Inc(void *p)
  ||
  || Description
  || -----------
  || internal Slider increment function, calls client's inc function
  ||  after updating *pnInc
  ||
  || Parameters
  || ----------
  || void *p: the Slider
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static void Slider_Inc(void *p)
{
   Slider *me = (Slider *)p;
   int16 nDiff = me->nTarget - *me->pnInc;
   
   if (0 != nDiff) {
      if (nDiff != 1 && nDiff != -1) {
         
         if (nDiff & 1) { /* odd, round up */
            if (nDiff < 0) {
               nDiff--;
            } else {
               nDiff++;
            }
         }
         *me->pnInc += nDiff/2;
      } else {
         *me->pnInc += nDiff;
         nDiff = 0;
      }
   }
   
   /* set timer before calling the inc function (may take a while) */
   if (0 != nDiff) {
      ISHELL_SetTimerEx(me->piShell,me->ulDelay,&me->cb);
   }

   /* call the inc function for every step, even last */
   if ((PFNNOTIFY)0 != me->pfnInc) {
      me->pfnInc(me->pNotifyData);
   }

   /* call done if no diff */
   if ((0 == nDiff) && ((PFNNOTIFY)0 != me->pfnDone)) {
      me->pfnDone(me->pNotifyData);
   }
}

void Slider_Stop(Slider *me)
{
   CALLBACK_Cancel(&me->cb);
}

void Slider_Start(Slider *me, int16 nTarget, PFNNOTIFY pfnDone, 
                  uint32 ulDelay)
{
   int16 nDiff = me->nTarget - *me->pnInc;

   me->nTarget = nTarget;
   me->pfnDone = pfnDone;
   me->ulDelay = ulDelay;
   
   if (0 == me->ulDelay) {

      *me->pnInc = me->nTarget;

   } else if (0 < (int32)me->ulDelay) { /* if delay is positive, means
                                    total delay, calculate number of 
                                    steps to take total delay */
      int nlog;
      
      for (nlog = 1; 0 != (nDiff/=2); nlog++); /* calculate log(nDiff) */
      
      me->ulDelay = me->ulDelay / nlog;
   } else {
      me->ulDelay = 0-me->ulDelay; /* if delay is negative, 
                                              is explicit time 
                                              between steps */
   }
   
   ISHELL_SetTimerEx(me->piShell,0,&me->cb);
}

boolean Slider_Sliding(Slider *me)
{
   return CALLBACK_IsQueued(&me->cb);
}

void Slider_CtorZ(Slider *me, IShell *piShell, int16 *pnInc, 
                  PFNNOTIFY pfnInc, void *pNotifyData)
{
   /* init my slider, the below don't change */
   me->piShell     = piShell;
   me->pfnInc      = pfnInc;
   me->pNotifyData = pNotifyData;
   me->pnInc       = pnInc;

   CALLBACK_Init(&me->cb,Slider_Inc,me);
}

void Slider_Dtor(Slider *me)
{
   Slider_Stop(me);
   ZEROAT(me);
}
