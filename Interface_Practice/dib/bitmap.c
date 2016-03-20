/*===========================================================================
FILE: bitmap.c
Author: brooks
Date: August. 2008
Description:
This file contain all common bitmap functions of pages
===========================================================================*/

#define __BITMAP_C__
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "bitmap.h"

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

//--------------------------------------------------------------------
//  BlendAlpha
//--------------------------------------------------------------------
#define BITSPREAD(u,msk)     (((u) | ((u) << 16)) & (msk) )
#define SPREAD565     0x07e0f81f
#define SPREAD555     0x03e07c1f
#define BIAS565       0x02008010
#define BIAS555       0x02004010
#define DWADD(pw,i)   ((uint32*)( ((byte*)(pw)) + (i) ))
#define DWB(pw,i)     (*DWADD(pw,i))
#define WADD(pw,i)    ((uint16*)( ((byte*)(pw)) + (i) ))
#define WB(pw,i)      (*WADD(pw,i))

#define MASK666RB 0x0003F03F
#define MASK666G  0x00000FC0
#define BIAS666RB 0x00010010
#define BIAS666G  0x00000400
#define MASK888RB 0x00FF00FF
#define MASK888G  0x0000FF00
#define BIAS888RB 0x00100010
#define BIAS888G  0x00001000

// Reduce 0...255 alpha value to a 0...32 alpha
//
#define REDUCEALPHA(a)    (((a)*32 + 144) >> 8)

// handles IDIB_COLORSCHEME_555 and IDIB_COLORSCHEME_565
// 
// See documentation for DIB_AlphaBlit16 for more details.  Works
// the way, except here we're blending in a single color rather
// than a source image.
//
int DIB_AlphaFill16(IDIB *me, int x, int cx, int y, int cy, NativeColor nc, int nAlpha)
{
   uint16 *pwRow  = (uint16*) (me->pBmp + y*me->nPitch + x*2);
   int cbRow      = cx * 2;
   int nAlphaLo   = REDUCEALPHA(nAlpha);

   if ((nc == 0 || nc == 0xFFFF) && (nAlphaLo == 16 || nAlphaLo == 24)) {

      // 50% or 75% black/white: simplified case much faster

      uint16 *pw = pwRow;
      uint32 uMask = 0x84108410;         // erase top bit of each field
      
      if (me->nColorScheme == IDIB_COLORSCHEME_555)
         uMask = 0x42104210;

      if (nc == 0) {

         // BLACK
         uMask = ~uMask;     // erase top bit

         do {
         
            for (pw = pwRow, y = cy; --y >= 0; pw = WADD(pw, me->nPitch)) {
               int i = cbRow;

               // right-most non-aligned word
               if ((((uint32)pw) + i) & 2) {
                  i -= 2;
                  WB(pw,i) = (WB(pw,i) >> 1 ) & (uint16) uMask;
               }
               // 32 bits at a time:  3 instructions/pixel
               while ( (i -= 4) >= 0) {
                  DWB(pw,i) = (DWB(pw,i) >> 1) & uMask;
               }
               // now:  i == -4 (done) or -2 (one more pixel)
               if ( (i += 2) == 0) {
                  WB(pw,i) = (WB(pw,i) >> 1 ) & (uint16) uMask;

               }
            }
            // repeat once to handle 75% black case
         } while ( (nAlphaLo -= 16) > 0);

      } else {

         // WHITE = same as black, but ORing instead of ANDing the top bit

         do {
            for (pw = pwRow, y = cy; --y >= 0; pw = WADD(pw, me->nPitch)) {
               int i = cbRow;
               if ((((uint32)pw) + i) & 2) {
                  i -= 2;
                  WB(pw,i) = (WB(pw,i) >> 1 ) | (uint16) uMask;
               }
               while ( (i -= 4) >= 0) {
                  DWB(pw,i) = (DWB(pw,i) >> 1) | uMask;
               }
               if ( (i += 2) == 0) {
                  WB(pw,i) = (WB(pw,i) >> 1 ) | (uint16) uMask;
               }
            }
         } while ( (nAlphaLo -= 16) > 0);
      }
      
   } else if (nAlphaLo != 0) {
   
      uint16 *pw = pwRow;
      register int nBeta32 = 32 - nAlphaLo;
      register uint32 uSrcProduct;
      register uint32 uMask = SPREAD565;
      uint32 uBias = BIAS565;

      if (me->nColorScheme == IDIB_COLORSCHEME_555) {
         uMask = SPREAD555;
         uBias = BIAS555;
      }
      
      uSrcProduct = BITSPREAD(nc,uMask) * nAlphaLo;
      uSrcProduct += uBias;

      for (y = cy; --y >= 0; pw = WADD(pw,me->nPitch) ) {
         int i = cbRow;

         while ( (i -= 2) >= 0) {
            uint32 u = WB(pw,i);
            u = BITSPREAD(u,uMask);        // construct three 10- or 11-bit fields
            u *= nBeta32;
            u += uSrcProduct;
            u = (u >> 5) & uMask;
            u = u | (u >> 16);
            WB(pw,i) = (uint16) u;
         }
      }
   }
   return 0;
}

#ifndef IDIB_COLORSCHEME_666
#define IDIB_COLORSCHEME_666  18   // 6 red, 6 green, 6 blue
#endif


// handles IDIB_COLORSCHEME_666 and IDIB_COLORSCHEME_888
//
// See documentation for DIB_AlphaBlit32 for more details. Works
// the way, except here we're blending in a single color rather
// than a source image.
//
int DIB_AlphaFill32(IDIB *me, int x, int cx, int y, int cy, NativeColor nc, int nAlpha)
{
   uint32 *pwRow  = (uint32*) (me->pBmp + y*me->nPitch + x*4);
   int cbRow      = cx * 4;
   int nAlphaLo   = REDUCEALPHA(nAlpha);

   // TODO: add 50% and 75% alpha optimizations

   if (nAlphaLo != 0) {
   
      uint32 *pw = pwRow;
      register int nBeta32 = 32 - nAlphaLo;
      register uint32 uSrcProductrb;
      register uint32 uSrcProductg;
      uint32 maskrb = MASK666RB;
      uint32 maskg = MASK666G;
      uint32 biasrb = BIAS666RB;
      uint32 biasg = BIAS666G;

      if (me->nColorScheme == IDIB_COLORSCHEME_888) {
         maskrb = MASK888RB;
         maskg = MASK888G;
         biasrb = BIAS888RB;
         biasg = BIAS888G;
      }

      // red and blue channels
      uSrcProductrb = (nc & maskrb) * nAlphaLo;
      uSrcProductrb += biasrb;

      // green channel
      uSrcProductg = (nc & maskg) * nAlphaLo;
      uSrcProductg += biasg;

      for (y = cy; --y >= 0; pw = DWADD(pw,me->nPitch) ) {
         int i = cbRow;

         while ( (i -= 4) >= 0) {
            // get pixel
            uint32 u = DWB(pw,i);
            uint32 rb, g;

            // red and blue
            rb = u & maskrb;  
            rb *= nBeta32;
            rb += uSrcProductrb;
            rb = (rb >> 5) & maskrb;

            // green
            g = u & maskg;
            g *= nBeta32;
            g += uSrcProductg;
            g = (g >> 5) & maskg;

            // recombine rb and g
            u = rb | g;

            // set pixel
            DWB(pw,i) = u;
         }
      }
   }
   return 0;
}

// Alpha blend a rect and a single color.  Performs clipping, then
// hands off to the right DIB_AlphaFillxxx() routine to do the
// actual blending.
//
int DIB_AlphaFill(IDIB *me, const AEERect *prc, NativeColor nc, int nAlpha)
{
   int x = prc->x;
   int y = prc->y;
   int xMax = x + prc->dx;
   int yMax = y + prc->dy;
   int cx, cy;
   
   // clipping
   if (x < 0)         x = 0;
   if (xMax > me->cx) xMax = me->cx;
   if (y < 0)         y = 0;
   if (yMax > me->cy) yMax = me->cy;

   cx = xMax - x;
   cy = yMax - y;
   
   // blending
   if (cx > 0) {
      // 16 bit
      if (me->nColorScheme == IDIB_COLORSCHEME_555 || me->nColorScheme == IDIB_COLORSCHEME_565) 
         return DIB_AlphaFill16(me, x, cx, y, cy, nc, nAlpha);
      // 32 bit
      if (me->nColorScheme == IDIB_COLORSCHEME_666 || me->nColorScheme == IDIB_COLORSCHEME_888)
         return DIB_AlphaFill32(me, x, cx, y, cy, nc, nAlpha);
   }
   return EFAILED;
}

// Obtain a DIB and invalidate a rectangle in it.  
//
int GetDIB(IBitmap *pib, const AEERect *prc, IDIB **ppdib)
{
   int nErr;

   nErr = IBITMAP_QueryInterface(pib, AEECLSID_DIB, (void**)ppdib);

#ifdef AEECLSID_DIB_20
   // We're compiling with 2.1+, where AEECLSID_DIB is non-invalidating
   // Try to invalidate if it succeeded, and try the old DIB if it failed
   if (nErr == SUCCESS) {
      // invalidate bitmap
      if (prc) {
         IBITMAP_Invalidate(pib, prc);
      }
   } else {
      nErr = IBITMAP_QueryInterface(pib, AEECLSID_DIB_20, (void**)ppdib);
   }
#endif

   return nErr;
}

// Blend a color with the contents of a rectangle.  Works on DIBs of colorscheme
// IDIB_COLORSCHEME_555, IDIB_COLORSCHEME_565, IDIB_COLORSCHEME_666 and
// IDIB_COLORSCHEME_888.
//
int BlendRect(IDisplay *pid, const AEERect *prc, RGBVAL rgb, int nAlpha)
{
   IBitmap *pb;
   IDIB *pdib;
   int nErr;
//   AEERect rcClip, rcFill;

   if (nAlpha == 255) {
      IDISPLAY_FillRect(pid, prc, rgb);
      return SUCCESS;
   }

   // Since we purport to draw to IDisplay, we must honor its clipping.
//   IDISPLAY_GetClipRect(pid, &rcClip);
//   if (!IntersectRect(&rcFill, &rcClip, prc)) {
//      return SUCCESS;
//   }

   pb = IDISPLAY_GetDestination(pid);
   if (!pb) {
      return EUNSUPPORTED;
   }

//   nErr = GetDIB(pb, &rcFill, &pdib);
	nErr = GetDIB(pb, prc, &pdib);
   if (SUCCESS == nErr) {
      if (pdib->nColorScheme == IDIB_COLORSCHEME_555 ||
          pdib->nColorScheme == IDIB_COLORSCHEME_565 ||
          pdib->nColorScheme == IDIB_COLORSCHEME_666 ||
          pdib->nColorScheme == IDIB_COLORSCHEME_888) {
         NativeColor nc = IBITMAP_RGBToNative(pb, rgb);
//         DIB_AlphaFill(pdib, &rcFill, nc, nAlpha);
		DIB_AlphaFill(pdib, prc, nc, nAlpha);
      } else {
         // If the device can't do blending, then just treat as opaque
         IDISPLAY_FillRect(pid, prc, rgb);
         nErr = EUNSUPPORTED;
      }
      IDIB_Release(pdib);
   } else {
      // If the device can't do blending, then just treat as opaque
      IDISPLAY_FillRect(pid, prc, rgb);
   }
   IBITMAP_Release(pb);

   return nErr;
}

// Blending 5-5-5 and 5-6-5 data:
//
// R, G, B fields are spread out across a 32-bit value in 11- and 10-bit
// fields, so one multiplication can generate products for each component.
//
// Alpha and "beta" values are calcuated for the source and destination,
// respectively.  The 10- or 11-bit result for each color component is given
// by src*alpha + dest*beta.  This is then shifted right 5 bits to get the
// final result.
//
//   A) When alpha and beta add to 33, the maximum result for each component
//   will be 33 * 31 = 1023.  Since 1023 = 32*32 - 1, dividing by 32 gives a
//   value nicely distributed across the values 0...31.  However, 33*63 yields
//   a result larger than 32*64, so this cumulative factor of 33 is unsuitable
//   for 5-6-5 bitmaps.
//
//   B) When alpha and beta add to 32, 5-bit components fill the range 0...992
//   and six-bits fill the range 0...2016.  These results fill their
//   respective 10- and 11-bit fields with the exception of the higher 31
//   values, so a bias correction of 15 or 16 should be added to the values to
//   prevent a bias toward dark images.
//
// Strategy B leaves the minimum and maximum values less likely to be chosen
// by a factor of 50%, which is what a simple nearest-match algorithm would
// produce, so this is most likely a benefit, not an aberration.
//
// Alpha values from 0..32 *inclusive* are calculated from original 0..255
// values.  As with color values, this is done using a nearest-match
// algorithm, since that yields results very close to optimal (i.e. if 8-bit
// alpha values were used).
//
int DIB_AlphaBlit16(IDIB *me, int x, int cx, int y, int cy, IDIB *pdibSrc, int xSrc, int ySrc, int nAlphaHi)
{
   uint32 uAlphaLo;
   
   nAlphaHi &= 255;
   uAlphaLo = REDUCEALPHA(nAlphaHi);

   // if (uAlphaLo == 32) DDBBLIT();

   if (cx > 0 && uAlphaLo != 0) {

      int cbRow = cx * 2;
      uint16 *pw    = (uint16*) (me->pBmp + y*me->nPitch + x*2);
      uint16 *pwSrc = (uint16*) (pdibSrc->pBmp + ySrc*pdibSrc->nPitch + xSrc*2);

      register uint32 uBetaLo = 32 - uAlphaLo;
      register uint32 uMask = SPREAD565;
      uint32 uBias = BIAS565;

      if (me->nColorScheme == IDIB_COLORSCHEME_555) {
         uMask = SPREAD555;
         uBias = BIAS555;
      }
         
      for (y = cy; --y >= 0;  ) {
         int i = cbRow;

         while ( (i -= 2) >= 0) {
            uint32 u, v;
               
            u = WB(pw,i);
            u = BITSPREAD(u,uMask);        // three 10- or 11-bit fields
            u *= uBetaLo;

            v = WB(pwSrc,i);
            v = BITSPREAD(v,uMask);
            v *= uAlphaLo;
                  
            u += v + uBias;
            u = (u >> 5) & uMask;
            u = u | (u >> 16);
            WB(pw,i) = (uint16) u;
         }
         pw = WADD(pw,me->nPitch);
         pwSrc = WADD(pwSrc, pdibSrc->nPitch);
      }
   }

   return SUCCESS;
}

// Blending 6-6-6 and 8-8-8 data:
//
// This works like DIB_AlphaBlit16(), only on 666 and 888 color schemes.
// The 555 and 565 cases handled by DIB_AlphaBlit16() involve spreading
// the bits of the color values across a 32bit integer so there's
// enough room (5 bits) between each of them to do the alpha blend
// calculations in a single pass.
//
// 32 bit color values can't be spread apart adequately to do the
// alpha blending using the same algorithm.  A 666 color value requires 
// 3*(5+6)=33 bits to spread far enough apart.  Similarly, an 888 color 
// value requires (3*(5+8))=39 bits.  Both color schemes require more room 
// than a uint32 provides.  
//
// So we'll isolate and blend the individual channels in separate steps.
// We isolate the red and blue channels with a mask and blend them
// simultaneously.  There's 6 or 8 bits between these two channels already
// and plenty of room in the msb of the uint32 color value, so there's no 
// need to shift bits around at all.  Once the red and blue channels are
// calculated, the green channel is isolated and blended in the same way.
// Combining the alpha blended results is a simple OR operation, since
// nothing's been shifted. 
//
// The alpha blend step involves calculating (d * alpha) + (s * (1-alpha))
// where alpha is a value from 0...1.  We opt to convert alpha values 
// to a range from 0...32 and then divide the result by 32 when we're 
// done.  This allows us to do the calculation with reasonable 
// precision using integers.  And it requires only 5 extra bits per
// channel.  Our calculation is actually:
//
// ((d * (alpha * 32)) + (s * (32 - alpha*32)))/32
//
// Since divsion rounds down, we factor in a small bias in each
// channel to spread the results better.
//
// The actual algorithm is as follows:
//
// Reduce alpha (0x00...0xFF) to a 5 bit value (0...32)
// for each pixel
//    get source pixel
//    get destination pixel
//    isolate red and blue channels
//    alpha blend them
//    isolate green channel
//    alpha blend it
//    combine results, store in destination
//
int DIB_AlphaBlit32(IDIB *me, int x, int cx, int y, int cy, IDIB *pdibSrc, int xSrc, int ySrc, int nAlphaHi)
{
   uint32 uAlphaLo;
   
   nAlphaHi &= 255;
   uAlphaLo = REDUCEALPHA(nAlphaHi);

   // if (uAlphaLo == 32) DDBBLIT();

   if (cx > 0 && uAlphaLo != 0) {

      int cbRow = cx * 4;
      uint32 *pw    = (uint32*) (me->pBmp + y*me->nPitch + x*4);
      uint32 *pwSrc = (uint32*) (pdibSrc->pBmp + ySrc*pdibSrc->nPitch + xSrc*4);

      register uint32 uBetaLo = 32 - uAlphaLo;
      register uint32 maskrb = MASK666RB;
      register uint32 maskg = MASK666G;
      uint32 biasrb = BIAS666RB;
      uint32 biasg = BIAS666G;

      if (me->nColorScheme == IDIB_COLORSCHEME_888) {
         maskrb = MASK888RB;
         maskg = MASK888G;
         biasrb = BIAS888RB;
         biasg = BIAS888G;
      }

      for (y = cy; --y >= 0;  ) {
         int i = cbRow;

         while ( (i -= 4) >= 0) {
            // get pixels
            uint32 u = DWB(pw,i);
            uint32 v = DWB(pwSrc,i);
            uint32 urb, ug;
            uint32 vrb, vg;
               
            // red and blue
            urb = u & maskrb;
            urb *= uBetaLo;

            vrb = v & maskrb;
            vrb *= uAlphaLo;
                  
            urb += vrb + biasrb;
            urb = (urb >> 5) & maskrb;

            // green
            ug = u & maskg;
            ug *= uBetaLo;

            vg = v & maskg;
            vg *= uAlphaLo;
                  
            ug += vg + biasg;
            ug = (ug >> 5) & maskg;
   
            // recombine rb and g
            u = urb | ug;

            // set pixel
            DWB(pw,i) = u;
         }
         pw = DWADD(pw,me->nPitch);
         pwSrc = DWADD(pwSrc, pdibSrc->nPitch);
      }
   }

   return SUCCESS;
}

// Alpha blend two bitmpas.  Performs clipping, then
// hands off to the right DIB_AlphaBlitxxx() routine to do the
// actual blending.
//
int DIB_AlphaBlit(IDIB *me, const AEERect *prc, IDIB *pdibSrc, int xSrc, int ySrc, int nAlphaHi)
{
   int x = prc->x;
   int y = prc->y;
   int xMax = x + prc->dx;
   int yMax = y + prc->dy;
   int cx, cy;
   
   // check dest
   if (x < 0)         { xSrc -= x; x = 0; }
   if (y < 0)         { ySrc -= y; y = 0; }
   if (xMax > me->cx) { xMax = me->cx; }
   if (yMax > me->cy) { yMax = me->cy; }

   // check src
   if (xSrc < 0)      { x -= xSrc; xSrc = 0; }
   if (ySrc < 0)      { y -= ySrc; ySrc = 0; }
   if (xMax > x + pdibSrc->cx - xSrc) { xMax = x + pdibSrc->cx - xSrc; }
   if (yMax > y + pdibSrc->cy - ySrc) { yMax = y + pdibSrc->cy - ySrc; }

   cx = xMax - x;
   cy = yMax - y;

   if (pdibSrc->nDepth != me->nDepth ||
       pdibSrc->nColorScheme != me->nColorScheme) {
      // Unsupported bit depth or colorscheme combination
      return EUNSUPPORTED;
   }

   // do the actual blend
   if (me->nColorScheme == IDIB_COLORSCHEME_555 || me->nColorScheme == IDIB_COLORSCHEME_565) 
      return DIB_AlphaBlit16(me, x, cx, y, cy, pdibSrc, xSrc, ySrc, nAlphaHi);
      
   if (me->nColorScheme == IDIB_COLORSCHEME_666 || me->nColorScheme == IDIB_COLORSCHEME_888)
      return DIB_AlphaBlit32(me, x, cx, y, cy, pdibSrc, xSrc, ySrc, nAlphaHi);
        

   return EFAILED;
}



// Alpha Blend two bitmaps.  Works on DIBs of colorscheme
// IDIB_COLORSCHEME_555, IDIB_COLORSCHEME_565, IDIB_COLORSCHEME_666 and
// IDIB_COLORSCHEME_888.
//
int BlendBlit(IBitmap *pibDest, const AEERect *prcDest, IBitmap *pibSrc, int xSrc, int ySrc, int nAlpha)
{
   IDIB *pdibDest, *pdibSrc;
   int nErr = EUNSUPPORTED;

   if (nAlpha != 255) 
   {
      if (SUCCESS == GetDIB(pibDest, prcDest, &pdibDest)) 
	  {
         if (SUCCESS == GetDIB(pibSrc, NULL, &pdibSrc)) 
		 {
            nErr = DIB_AlphaBlit(pdibDest, prcDest, pdibSrc, xSrc, ySrc, nAlpha);
            IDIB_Release(pdibSrc);
         }
         IDIB_Release(pdibDest);
      }
   }

   if (nErr != SUCCESS)
   {
	  
      nErr = IBITMAP_BltIn(pibDest, prcDest->x, prcDest->y, prcDest->dx, prcDest->dy,
		                   pibSrc, xSrc, ySrc, AEE_RO_TRANSPARENT);
 //                          pibSrc, xSrc, ySrc, AEE_RO_COPY);

   }

   return nErr;
}


#undef __BITMAP_C__