/*===========================================================================
FILE: bitmap.h
Author: brooks
Date: Aug 2008
This file contain all Macro and configuration definition of common bitmap functions
===========================================================================*/
#ifndef __BITMAP_H__
#define __BITMAP_H__


#endif //__BITMAP_H__

//-----------------------------bitmap.c--------------------------------
#ifndef __BITMAP_C__
	#define EXTERN extern
#else
   #define EXTERN
#endif
EXTERN int BlendRect(IDisplay *pid, const AEERect *prc, RGBVAL rgb, int nAlpha);
EXTERN int BlendBlit(IBitmap *pibDest, const AEERect *prcDest, IBitmap *pibSrc, int xSrc, int ySrc, int nAlpha);
EXTERN int DIB_AlphaFill(IDIB *me, const AEERect *prc, NativeColor nc, int nAlpha);
EXTERN int GetDIB(IBitmap *pib, const AEERect *prc, IDIB **ppdib);
#undef EXTERN