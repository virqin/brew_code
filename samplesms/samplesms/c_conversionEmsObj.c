/*===========================================================================
FILE: c_conversionEmsObj.c

AUTHOR:
   QUALCOMM Incorporated

SERVICES: 
   C_SAMPLESMS applet showing usage of BREW ISMS interfaces.

DESCRIPTION:
   This sample application (C_SAMPLESMS) demonstrates:
   - Using ISMS to send messages encapsulated by ISMSMsg interface
   - Using ISMS to receive incoming message(s) by registering for notification 
   issued by ISMSNotifier (AEECLSID_SMSNOTIFIER).
   - Sending and receiving EMS utilizing User Data Header (UDH). 
   - SMSStorage for messages, including performing operation on messages 
   (store, delete, update, read). The application will also provide 
   message classification as inbox, outbox, or draft messages.

   This file represents the program for converting .bmp data to EMS picture format,
   and vice versa, including streaming the image data buffer to display image
   on the screen.


               Copyright © 2008 QUALCOMM Incorporated.
                      All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/

#include "c_conversionEmsObj.h"

/*===============================================================================
                        FUNCTION PROTOTYPES
=============================================================================== */
void c_samplesms_ImageReady(CSampleSMS *pMe, IImage *pIImage, AEEImageInfo *pi, int nErr);


/*===========================================================================
FUNCTION c_samplesms_ConvertResPicToEMS

DESCRIPTION
  Converts BMP format to EMS Format.
  BMP data is retrieved from resource file.

===========================================================================*/

boolean c_samplesms_ConvertResPicToEMS(CSampleSMS *pMe, 
                            int16 resID, ems_udh_type *pUdh) {
   int i,j;
   BITMAPFILEHEADER *hdr;
   BITMAPINFO *bmpInfo;
   byte *ptrResImage;
   byte *ptrImage;
   
   int index;
   int width;
   char str[3];


   ptrResImage = ISHELL_LoadResData(pMe->m_pIShell, C_SAMPLESMS_RES_FILE, 
               resID, RESTYPE_IMAGE);
   //Adjust the offset to point to actual data
   ptrImage = ptrResImage + RES_FILE_IMAGEDATA_OFFSET;
   
   hdr = MALLOC(sizeof(BITMAPFILEHEADER));
   if(hdr == NULL) {
      DBGPRINTF("Buffer Could Not be Allocated");
      return FALSE;
   }

   /*Get the Header Info*/
   (void)MEMCPY(hdr,ptrImage,sizeof(BITMAPFILEHEADER));

   str[0] = ptrImage[0];
   str[1] = ptrImage[1];
   str[2] = '\0';

   /*Is it a BMP?*/
   if(STRCMP(str,"BM") == 0) {
      bmpInfo = MALLOC(sizeof(BITMAPINFO));

      if(bmpInfo == NULL)   {
         DBGPRINTF("Buffer Could Not be Allocated",0,0,0);
         FREEIF(hdr);
         ISHELL_FreeResData(pMe->m_pIShell, (void *) ptrResImage);   
         return FALSE;
      }

      (void)MEMCPY(bmpInfo,ptrImage+sizeof(BITMAPFILEHEADER),sizeof(BITMAPINFO));

      /*Number of bytes that fit on one line*/
      width = (int)(bmpInfo->bmiHeader.biWidth/8);

      /*index of the output buffer*/
      index = width-1;
      /*BMP to EMS Conversion, bytes in a bmp are organised the following way
      30 31 28 29 26 27......2 3 0 1 */

      switch(pUdh->header_id)
      {   
         case AEESMS_UDH_SMALL_PICTURE:
            
            for(i=(int)bmpInfo->bmiHeader.biSizeImage-1;i>=0;)   {
               i -= 4-width; // Word Boundary
               for(j=0;j<width;j++) {
                  pUdh->u.small_picture.data[index--] = 
                     ~ptrImage[hdr->bfOffBits + i--];
               }
               index += 2*width;
            }
            break;

         case AEESMS_UDH_LARGE_PICTURE:
            for(i=(int)bmpInfo->bmiHeader.biSizeImage-1;i>=0;)  {
               i -= 4-width;
               for(j=0;j<width;j++) {
                  pUdh->u.large_picture.data[index--] = 
                     ~ptrImage[hdr->bfOffBits + i--];
               }
               index += 2*width;
            }
            break;

         default:
            FREEIF(bmpInfo);
            FREEIF(hdr);
            ISHELL_FreeResData(pMe->m_pIShell, (void *) ptrResImage);   
            return FALSE;
      }
      FREEIF(bmpInfo);
   }

   FREEIF(hdr);
   ISHELL_FreeResData(pMe->m_pIShell, (void *) ptrResImage);   
   return TRUE;
}



/*===========================================================================
FUNCTION c_samplesms_ConvertEmsPicToBmp

DESCRIPTION
  Converts EMS picture to bitmap form to be displayed.

===========================================================================*/
void c_samplesms_ConvertEmsPicToBmp (byte *imageBuf, BITMAPFILEHEADER **hdr, 
                  BITMAPINFO *bmpInfo, uint8 *data, int totalSize) {
  int width;
  int factor;
  int j,k,l,height;

  *hdr = (BITMAPFILEHEADER*)MALLOC(sizeof(BITMAPFILEHEADER));

  if(*hdr == NULL)
  {
    DBGPRINTF("Buffer Could Not be Allocated",0,0,0);
    return;
  }

  (*hdr)->bfType = (uint16)('M' << 8 | 'B');

  (*hdr)->bfOffBits = (uint16)(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO));


  bmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmpInfo->bmiHeader.biPlanes = 1;
  bmpInfo->bmiHeader.biBitCount = 1;//Number of bits per pixel
  bmpInfo->bmiHeader.biCompression = 0;

  bmpInfo->bmiHeader.biXPelsPerMeter = 0;//3780;
  bmpInfo->bmiHeader.biYPelsPerMeter = 0;//3780;
  bmpInfo->bmiHeader.biClrUsed = 0;
  bmpInfo->bmiHeader.biClrImportant = 0;

  bmpInfo->bmiColors[0].rgbBlue = 0xFF;
  bmpInfo->bmiColors[0].rgbGreen= 0xFF;
  bmpInfo->bmiColors[0].rgbRed  = 0xFF;

  bmpInfo->bmiColors[1].rgbBlue = 0x00;
  bmpInfo->bmiColors[1].rgbGreen= 0x00;
  bmpInfo->bmiColors[1].rgbRed  = 0x00;

  width = (int)bmpInfo->bmiHeader.biWidth /8;
  if(bmpInfo->bmiHeader.biWidth %8 > 0)
  {
    width++;
  }

  factor = width%4;

  if(factor!=0)
  {
    factor = 4-factor;
  }

  //Since the width is not a double word, ,multiple width by factor
  bmpInfo->bmiHeader.biSizeImage = (dword)(factor+width) * bmpInfo->bmiHeader.biHeight;
  (*hdr)->bfSize = (uint16)((*hdr)->bfOffBits + bmpInfo->bmiHeader.biSizeImage);


  (void)MEMSET(imageBuf,0x00,IMAGE_SIZE);

  (void)MEMCPY(imageBuf,
         *hdr,
         sizeof(BITMAPFILEHEADER));

  (void)MEMCPY(imageBuf+sizeof(BITMAPFILEHEADER),
         bmpInfo,
         sizeof(BITMAPINFO));

  j = (*hdr)->bfOffBits;
  k=0;
  l=((int)(bmpInfo->bmiHeader.biWidth/8) * (int)(bmpInfo->bmiHeader.biHeight))- width;

  //The bmp starts at the lower left hand corner
  for(height=(int)(bmpInfo->bmiHeader.biHeight-1);height>=0;height--)
  {
    for(k=0;k<width;k++)
    {
      imageBuf[j++]= data[l++];
    }
    l=l-(2*width);
    j +=factor;
  }
}
                   
/*===========================================================================
FUNCTION c_samplesms_DisplayEMSObject

DESCRIPTION
  Display EMS Object from data buffer 

===========================================================================*/
void c_samplesms_DisplayEMSObject(CSampleSMS *pMe, byte* imageBuff, int buffSize) {
   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_MEMASTREAM, 
      (void **)&pMe->m_pIMemAStream) != SUCCESS) {
      return;
   }

   if (ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_NATIVEBMP, 
      (void **)&pMe->m_pIImage) != SUCCESS) {
      return;
   }

   IMEMASTREAM_Set(pMe->m_pIMemAStream, imageBuff, IMAGE_SIZE, 0, FALSE); 

   IIMAGE_Notify(pMe->m_pIImage,(PFNIMAGEINFO)c_samplesms_ImageReady,pMe);
   IIMAGE_SetStream(pMe->m_pIImage,(IAStream*)pMe->m_pIMemAStream);
}


/*===========================================================================
FUNCTION c_samplesms_ImageReady

DESCRIPTION
  Image data buffer has been streamed and is ready to be displayed 

===========================================================================*/
void c_samplesms_ImageReady(CSampleSMS *pMe, IImage *pIImage, AEEImageInfo *pi, int nErr) {
   int ascent, descent;
   int strTotalPixel;
   AECHAR *pwText;
   int x,y, textHeight;
   SMSMsgOpt smo;
   
   IDisplay_GetFontMetrics(pMe->m_pIDisplay, AEE_FONT_NORMAL, &ascent, &descent);
   textHeight = ascent + descent;
   x = 0;
   y = 2*textHeight;
   if (pMe->m_pReadMsgBuffer) {
      if (NULL == (pwText = (AECHAR *) MALLOC((pMe->m_objPos+1) *sizeof(AECHAR)) )) {            
         DBGPRINTF("Couldn't allocate memory");
      } else {
         STRTOWSTR(pMe->m_pReadMsgBuffer, pwText, ((pMe->m_objPos+1) * sizeof(AECHAR)));
      }
      
      if (pwText) {
         strTotalPixel = IDisplay_MeasureText(pMe->m_pIDisplay, AEE_FONT_NORMAL, pwText);
         x = strTotalPixel % (pMe->DeviceInfo.cxScreen);
         y += (strTotalPixel / (pMe->DeviceInfo.cxScreen)) * textHeight;
         FREE(pwText);
      }
   }
   
   if (SUCCESS == ISMSMSG_GetOpt(pMe->m_pISMSMsg, MSGOPT_UDH_CONCAT, &smo) ) {
      y += textHeight;   
   }
   
   IIMAGE_Draw(pMe->m_pIImage,x,y);
   IDISPLAY_Update(pMe->m_pIDisplay);
   IMEMASTREAM_Release(pMe->m_pIMemAStream);
   pMe->m_pIMemAStream=NULL;
}


