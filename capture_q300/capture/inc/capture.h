/*======================================================
FILE: capture.h

SERVICES: 
   IWidget implementation for frame widget

GENERAL DESCRIPTION:
  
  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright ?1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef __CAPTURE_H__
#define __CAPTURE_H__

#include "capture.bid"

typedef struct _CameraApp CameraApp;

#define NORMAL_STR_LEN		64
#define CAPTURE_EVENT_MSG	"GetPicUploadInfo"

#ifndef _echat_camer_upload_info_
#define _echat_camer_upload_info_
typedef struct echat_camer_upload_info {
	char			userid[NORMAL_STR_LEN];	//用户ID
	char			lon[NORMAL_STR_LEN];	//经纬度
	char			lat[NORMAL_STR_LEN];	
	int			coordinate;						//坐标类型
}echat_camer_upload_info;
#endif //_echat_camer_upload_info_

#endif   //__CAPTURE_H__

