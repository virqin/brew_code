/*
  ===========================================================================
  
  FILE: tooltip.c
   
  SERVICES:  
     Tooltip
  
   
  DESCRIPTION: 
    Tooltip handles scheduling, drawing, and hiding a set of tooltips
  
  
  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/
#include "AEEStdlib.h"
#include "tooltip.h"
#include "util.h"


/**
  || Function
  || --------
  || static void Tooltip_Hide(void *p)
  ||
  || Description
  || -----------
  || sets the Tooltip to not showing (internal func)
  ||
  || Parameters
  || ----------
  || void *p: the Tooltip object
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static void Tooltip_Hide(void *p)
{
   Tooltip *me = (Tooltip *)p;
   
   me->bShown = FALSE;

   me->pfnInvalidate(me->pInvalidateData);
}

/**
  || Function
  || --------
  || static void Tooltip_Show(void *p)
  ||
  || Description
  || -----------
  || sets the Tooltip to showing (internal func)
  ||
  || Parameters
  || ----------
  || void *p: the Tooltip
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
static void Tooltip_Show(void *p)
{
   Tooltip *me = (Tooltip *)p;
   
   me->bShown = TRUE;
   
   me->pfnInvalidate(me->pInvalidateData);

   CALLBACK_Init(&me->cb,Tooltip_Hide,me);
   ISHELL_SetTimerEx(me->piShell,me->ulExpires,&me->cb);
}


void Tooltip_Redraw(Tooltip *me)
{
   if (!me->bShown || (0 == WSTRLEN(me->pwsz))) {
      return;
   }

   me->rc.dx = IDISPLAY_MeasureText(me->piDisplay,AEE_FONT_NORMAL,me->pwsz)+4;

   me->rc.x = me->nX + (((me->nDx-me->rc.dx)*me->nXPercent)/100);
   me->rc.x = CONSTRAIN(me->rc.x, me->nX, (me->nX + me->nDx) - me->rc.dx);
   
   IDISPLAY_DrawText(me->piDisplay,AEE_FONT_NORMAL,me->pwsz,-1,
                     me->rc.x+2,me->rc.y+2,
                     &me->rc,IDF_RECT_FILL|IDF_RECT_FRAME);
}

void Tooltip_Cancel(Tooltip *me)
{
   if (me->bShown) {
      me->pfnInvalidate(me->pInvalidateData);
   }
   me->bShown = FALSE;
   CALLBACK_Cancel(&me->cb);
}

static const AECHAR dotdotdot[4] = { '.','.','.',0 }; /* sheesh */

void Tooltip_Push(Tooltip *me,const char *cpszResFile, uint16 wId, 
                  int nXPercent)
{
   me->nXPercent = nXPercent;

   FREEIF(me->pStringData);
   
   if (SUCCESS ==
       IShell_LoadResWsz(me->piShell,cpszResFile,wId,&me->pStringData)) {
      me->pwsz = me->pStringData;
   } else {
      me->pwsz = (AECHAR *)dotdotdot;
   }

   if (!me->bShown) {
      CALLBACK_Init(&me->cb,Tooltip_Show,me);
      ISHELL_SetTimerEx(me->piShell,me->ulDelay,&me->cb);
   } else {
      Tooltip_Show(me);
   }
}

void Tooltip_CtorZ(Tooltip *me, IShell *piShell, IDisplay *piDisplay,
                   int16 yBottom, int16 nX, int16 nDx,
                   uint32 ulDelay, uint32 ulExpires,
                   PFNNOTIFY pfnInvalidate, void *pInvalidateData)
{
   me->piShell         = piShell;
   ISHELL_AddRef(piShell);
   me->piDisplay       = piDisplay;
   IDISPLAY_AddRef(piDisplay);

   me->nX              = nX;
   me->nDx             = nDx;
   me->ulDelay         = ulDelay;
   me->ulExpires       = ulExpires;
   me->pfnInvalidate   = pfnInvalidate;
   me->pInvalidateData = pInvalidateData;

   {
      int nAscent, nDescent;
      IDISPLAY_GetFontMetrics(piDisplay,AEE_FONT_NORMAL,&nAscent,&nDescent);
      
      me->rc.x = 0;
      me->rc.dy = nAscent + nDescent + 4;
      me->rc.y = MAX(0, yBottom - me->rc.dy - 2);
   }
}

void Tooltip_Dtor(Tooltip *me)
{
   FREEIF(me->pStringData);
   RELEASEIF(me->piDisplay);
   RELEASEIF(me->piShell);
   CALLBACK_Cancel(&me->cb);
   ZEROAT(me);
}

