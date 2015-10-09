#ifndef TOOLTIP_H
#define TOOLTIP_H /* #ifndef TOOLTIP_H */
/*
  ===========================================================================
  
  FILE: tooltip.h
   
  SERVICES:  
     Tooltip
   
  DESCRIPTION: 
    Tooltip handles scheduling, drawing, and hiding a set of tooltips.
    A Tooltip keeps track of whether a tooltip is due to be displayed, 
     and with what.

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

/* =======================================================================
   Tooltip stuff
   
   A Tooltip keeps track of whether a tooltip is due to be displayed, 
   and with what.
   ======================================================================= */
typedef struct Tooltip
{
   IShell      *piShell;
   IDisplay    *piDisplay;
   uint32       ulDelay;   /* time until I show up */
   uint32       ulExpires;  /* time until I hide again */
               
   PFNNOTIFY    pfnInvalidate;
   void        *pInvalidateData;
               
   AEECallback  cb;
   
   AECHAR      *pwsz;
   AECHAR      *pStringData;
   AEERect      rc;

   int16        nX;
   int16        nDx;
   int16        nXPercent;
   boolean      bShown;

} Tooltip;

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**
  || Function
  || --------
  || void Tooltip_CtorZ(Tooltip *me, IShell *piShell, IDisplay *piDisplay,
  ||                    int16 yBottom, int16 nX, int16 nDx,
  ||                    uint32 ulDelay, uint32 ulExpires,
  ||                    PFNNOTIFY pfnInvalidate, void *pInvalidateData);
  ||
  ||
  || Description
  || -----------
  || Initialize a zero-allocated Tooltip.  Tooltips draw on a horizontal
  ||   line, always showing between nX and nDx.
  || 
  ||
  || Parameters
  || ----------
  || Tooltip *me: Tooltip
  || IShell *piShell: BREW's IShell
  || IDisplay *piDisplay: the display
  || int16 yBottom: bottom drawing coordinate
  || int16 nX: leftmost coordinate
  || int16 nDx: rightmost
  || uint32 ulDelay: time we wait after push before showing a tooltip
  || uint32 ulExpires: time we wait after showing before hiding
  || PFNNOTIFY pfnInvalidate: where to call to request a redraw
  || void *pInvalidateData: what to call pfnInvalidate with
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
void Tooltip_CtorZ(Tooltip *me, IShell *piShell, IDisplay *piDisplay,
                   int16 yBottom, int16 nX, int16 nDx,
                   uint32 ulDelay, uint32 ulExpires,
                   PFNNOTIFY pfnInvalidate, void *pInvalidateData);

/**
  || Function
  || --------
  || void Tooltip_Dtor(Tooltip *me);
  ||
  || Description
  || -----------
  || cleans up after a Tooltip
  ||
  || Parameters
  || ----------
  || Tooltip *me: the Tooltip
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
void Tooltip_Dtor(Tooltip *me);

/**
  || Function
  || --------
  || void Tooltip_Redraw(Tooltip *me);
  ||
  || Description
  || -----------
  || Redraws the Tooltip (if showing)
  ||
  || Parameters
  || ----------
  || Tooltip *me: Tooltip
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  || if the Tooltip is not showing, this call has no effect
  ||
  */
void Tooltip_Redraw(Tooltip *me);


/**
  || Function
  || --------
  || void Tooltip_Cancel(Tooltip *me);
  ||
  || Description
  || -----------
  || Hides a Tooltip if showing.  If not showing, cancel a Push()
  ||
  || Parameters
  || ----------
  || ToolTip *me: Tooltip
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
void Tooltip_Cancel(Tooltip *me);

/**
  || Function
  || --------
  || void Tooltip_Push(Tooltip *me,const char *cpszResFile, uint16 wId, 
  ||                   int nXPercent);
  || 
  ||
  || Description
  || -----------
  || Schedule a show if not showing, else update contents/position being 
  ||   shown
  ||
  || Parameters
  || ----------
  || Tooltip *me: the Tooltip
  || const char *cpszResFile: where to find the resource string
  || uint16 wId: id of the string to show as a tip
  || int nXPercent: percent of nDx (passed to CtorZ) to have on left/right
  ||                 of tooltip
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  || Push() resets the showing timer if not showing, Push() resets the 
  ||   expiration timer if showing.
  ||
  */
void Tooltip_Push(Tooltip *me,const char *cpszResFile, uint16 wId, 
                  int nXPercent);


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef TOOLTIP_H */
