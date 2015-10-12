#ifndef C_CONVERSIONEMSOBJ_H
#define C_CONVERSIONEMSOBJ_H
/*=====================================================================
FILE:  c_conversionEmsObj.h

AUTHOR:
   QUALCOMM Incorporated

SERVICES: 
   C_SAMPLESMS applet showing usage of BREW ISMS interfaces.

DESCRIPTION:
   This file provides definitions, typedefs and function prototypes
   for c_samplesms/c_conversionEmsObj.c


        Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================================*/

#include "c_samplesms.h"

typedef struct
{
  uint16    bfType;      /*Specifies the file type, must be BM. */
  uint16    bfSize;      /*Specifies the size, in bytes, of the bitmap file. */
  uint16    bfSize2;     /*Reserved; must be zero. */
  uint16    bfReserved1; /*Reserved; must be zero. */
  uint16    bfReserved2;
  uint16    bfOffBits;   /*Specifies the offset, in bytes, from the beginning of the */
  uint16    bfOffbits2;  /*  BITMAPFILEHEADER structure to the bitmap bits. */

} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{

  dword  biSize;          /*Number of bytes required by the structure. */
  dword  biWidth;         /*Width of the bitmap, in pixels. */
  int32  biHeight;        /*Height of the bitmap, in pixels. If biHeight is positive, the bitmap is a bottom-up DIB and its origin is the lower-left corner. If biHeight is negative, the bitmap is a top-down DIB and its origin is the upper-left corner.
                            If biHeight is negative, indicating a top-down DIB, */
  word   biPlanes;        /*number of planes for the target device. This value must be set to 1. */
  word   biBitCount;      /*bits-per-pixel. */
  dword  biCompression;   /*Type of compression for a compressed bottom-up bitmap (top-down DIBs cannot be compressed)*/
  dword  biSizeImage;     /*size, in bytes, of the image*/
  dword  biXPelsPerMeter; /*horizontal resolution*/
  dword  biYPelsPerMeter; /*Vertical Resolution*/
  dword  biClrUsed;       /*Number of color indexes in the color table that are actually used by the bitmap*/
  dword  biClrImportant;  /*Number of color indexes that are required for displaying the bitmap*/
} BITMAPINFOHEADER;


typedef struct tagRGBQUAD
{
  byte    rgbBlue;
  byte    rgbGreen;
  byte    rgbRed;
  byte    rgbReserved;
} RGBQUAD;


typedef struct tagBITMAPINFO
{
  BITMAPINFOHEADER    bmiHeader;
  RGBQUAD             bmiColors[2];
} BITMAPINFO;


#define IMAGE_SIZE               1000
#define SMALL_PIC_SIZE             16
#define LARGE_PIC_SIZE             32
#define RES_FILE_IMAGEDATA_OFFSET  12

/*===============================================================================
                        FUNCTION PROTOTYPES
=============================================================================== */
boolean c_samplesms_ConvertResPicToEMS(CSampleSMS *pMe, int16 resID, ems_udh_type *pUdh);
void c_samplesms_ConvertEmsPicToBmp (byte *imageBuf, BITMAPFILEHEADER **hdr, 
                           BITMAPINFO *bmpInfo, uint8 *data, int totalSize);
void c_samplesms_DisplayEMSObject(CSampleSMS *pMe, byte* imageBuff, int buffSize);

#endif
