/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "XUtil.h"

boolean AEE_IntersectRect(AEERect *prcDest, const AEERect *prcSrc1, const AEERect *prcSrc2)
{
    int aleft, atop, aright, abottom; // coordinates of prcSrc1
    int bleft, btop, bright, bbottom; // coordinates of prcSrc2

    aleft = prcSrc1->x;
    atop = prcSrc1->y;
    aright = aleft + prcSrc1->dx - 1;
    abottom = atop + prcSrc1->dy - 1;

    bleft = prcSrc2->x;
    btop = prcSrc2->y;
    bright = bleft + prcSrc2->dx - 1;
    bbottom = btop + prcSrc2->dy - 1;

    // early rejection test
    if (aleft > bright || atop > bbottom || bleft > aright || btop > abottom) {
        if (prcDest) {
            SETAEERECT(prcDest, 0, 0, 0, 0);
        }
        return FALSE;   // empty intersection
    }

    if (prcDest) {
        prcDest->x  = (int16)MAX(aleft, bleft);
        prcDest->y  = (int16)MAX(atop, btop);
        prcDest->dx = (int16)MIN(aright, bright) - prcDest->x + 1;
        prcDest->dy = (int16)MIN(abottom, bbottom) - prcDest->y + 1;
    }

    return TRUE;
}

boolean AEE_SubtractRect(AEERect *prcResult, const AEERect *prcBase, const AEERect *prcSub)
{
    int nEdges = 0;
    boolean bLeftEdge = 0;
    boolean bRightEdge = 0;
    boolean bTopEdge = 0;
    boolean bBottomEdge = 0;

    int bleft, btop, bright, bbottom;   // prcBase coordinates
    int sleft, stop, sright, sbottom;   // prcSub coordinates

    bleft = prcBase->x;
    btop = prcBase->y;
    bright = bleft + prcBase->dx - 1;
    bbottom = btop + prcBase->dy - 1;

    sleft = prcSub->x;
    stop = prcSub->y;
    sright = sleft + prcSub->dx - 1;
    sbottom = stop + prcSub->dy - 1;

    // count which base rect edges fall between the sub rect edges
    if (BETWEEN(bleft, sleft, sright + 1)) {
        bLeftEdge = 1;
        nEdges++;
    }
    if (BETWEEN(bright, sleft, sright + 1)) {
        bRightEdge = 1;
        nEdges++;
    }
    if (BETWEEN(btop, stop, sbottom + 1)) {
        bTopEdge = 1;
        nEdges++;
    }
    if (BETWEEN(bbottom, stop, sbottom + 1)) {
        bBottomEdge = 1;
        nEdges++;
    }

    // empty rect result
    if (nEdges == 4) {
        // set result to empty rect
        bleft = bright = btop = bbottom = 0;

    } else if (nEdges == 3) {

        if (!bTopEdge) {
            bbottom = stop;
        } 
        if (!bBottomEdge) {
            btop = sbottom;
        } 
        if (!bLeftEdge) {
            bright = sleft;
        } 
        if (!bRightEdge) {
            bleft = sright;
        }
    }

    SETAEERECT(prcResult, bleft, btop, bright-bleft, bbottom-btop);
    return (boolean)(nEdges > 2);
}


/* returns FALSE if either source rectangle is empty */
void AEE_UnionRect(AEERect *prcResult, const AEERect *prcSrc1, const AEERect *prcSrc2)
{
    int aleft, atop, aright, abottom;   // coordinates of prcSrc1
    int bleft, btop, bright, bbottom;   // coordinates of prcSrc2

    aleft = prcSrc1->x;
    atop = prcSrc1->y;
    aright = aleft + prcSrc1->dx;
    abottom = atop + prcSrc1->dy;

    bleft = prcSrc2->x;
    btop = prcSrc2->y;
    bright = bleft + prcSrc2->dx;
    bbottom = btop + prcSrc2->dy;

    prcResult->x = (int16)MIN(aleft, bleft);
    prcResult->dx = (int16)MAX(aright, bright) - prcResult->x;

    prcResult->y = (int16)MIN(atop, btop);
    prcResult->dy = (int16)MAX(abottom, bbottom) - prcResult->y;
}

int IBitmap_TestOpacity(IBitmap* piBitmap, int x, int y, boolean* pb)
{
    int nErr = SUCCESS;
    IDIB* piDib = NULL;
    boolean bInRect = FALSE;
    NativeColor nc = 0;
    byte* pPixel = NULL;

    nErr = IBitmap_QueryInterface(piBitmap, AEECLSID_DIB, (void **)&piDib);
    if (SUCCESS != nErr) {
        return nErr;
    }

    bInRect = (x >= 0 && x < piDib->cx && y >= 0 && y < piDib->cy);

    if (bInRect)
    {
        pPixel = piDib->pBmp + y * piDib->nPitch + x * piDib->nDepth / 8;

        if (piDib->nDepth < 8) {
            int nBitOffset = 8 - piDib->nDepth - (x * piDib->nDepth) % 8;
            nc = (*pPixel & ((((byte)1 << piDib->nDepth) - 1) << nBitOffset)) >> nBitOffset;
        } else if (8 == piDib->nDepth) {
            nc = *pPixel;
        } else if (16 == piDib->nDepth) {
            nc = *(uint16*)pPixel;
        } else if (24 == piDib->nDepth) {
            nc = *(uint16*)pPixel + ((uint32)*(pPixel + sizeof(uint16)) << 16);
        } else if (32 == piDib->nDepth) {
            nc = *(uint32*)pPixel;
        } else {
            nErr = EINVALIDFORMAT;
        }
    }

    if (SUCCESS == nErr && NULL != pb) {
        *pb = bInRect && (nc != piDib->ncTransparent);
    }

    RELEASEIF(piDib);

    return nErr;
}

int ISHELL_LoadResStringEx(IShell *pIShell, const char *pszBaseFile, uint16 nResID, AECHAR **ppBuff)
{
    uint32 nLen = 0;
    AECHAR *pBuff = NULL;
    int nRet = 0;

    ISHELL_GetResSize(pIShell, pszBaseFile, nResID, RESTYPE_STRING, &nLen);
    if (0 == nLen) {
        return nRet;
    }

    nLen *= 2;

    pBuff = MALLOC(nLen);
    if (NULL == pBuff) {
        return nRet;
    }

    nRet = ISHELL_LoadResString(pIShell, pszBaseFile, nResID, pBuff, nLen);
    if (0 == nRet) {
        FREE(pBuff);
    }

    *ppBuff = pBuff;
    return nRet;
}
