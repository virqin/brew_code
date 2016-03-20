/*===========================================================================

FILE: Trans.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions

#include "AEEImage.h"
#include "Trans.bid"
#include "res.brh"

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _Trans {
    AEEApplet      a ;           // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo;   // always have access to the hardware device information

    // add your own variables here...



} Trans;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean Trans_HandleEvent(Trans* pMe, AEEEvent eCode, 
                                             uint16 wParam, uint32 dwParam);
boolean Trans_InitAppData(Trans* pMe);
void    Trans_FreeAppData(Trans* pMe);
boolean DrawTransImage(IDisplay * pIDisplay, IImage* pImage, int x, int y, uint8 nTrans );
boolean DrawTransRect(IDisplay * pIDisplay, AEERect *pRect, uint8 nTrans );


/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION
    This function is invoked while the app is being loaded. All Modules must provide this 
    function. Ensure to retain the same name and parameters for this function.
    In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
    that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IDisplay_InitAppData(). 
   This is done as follows: InitAppData() is called to initialize AppletData 
   instance. It is app developers responsibility to fill-in app data initialization 
   code of InitAppData(). App developer is also responsible to release memory 
   allocated for data contained in AppletData -- this can be done in 
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
    clsID: [in]: Specifies the ClassID of the applet which is being loaded

    pIShell: [in]: Contains pointer to the IShell object. 

    pIModule: pin]: Contains pointer to the IModule object to the current module to which
    this app belongs

    ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
    of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
    *ppObj = NULL;

    if( ClsId == AEECLSID_TRANS )
    {
        // Create the applet and make room for the applet structure
        if( AEEApplet_New(sizeof(Trans),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)Trans_HandleEvent,
                          (PFNFREEAPPDATA)Trans_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
        {
            //Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
            if(Trans_InitAppData((Trans*)*ppObj))
            {
                //Data initialized successfully
                return(AEE_SUCCESS);
            }
            else
            {
                //Release the applet. This will free the memory allocated for the applet when
                // AEEApplet_New was called.
                IAPPLET_Release((IApplet*)*ppObj);
                return EFAILED;
            }

        } // end AEEApplet_New

    }

    return(EFAILED);
}


/*===========================================================================
FUNCTION SampleAppWizard_HandleEvent

DESCRIPTION
    This is the EventHandler for this app. All events to this app are handled in this
    function. All APPs must supply an Event Handler.

PROTOTYPE:
    boolean SampleAppWizard_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
    pi: Pointer to the AEEApplet structure. This structure contains information specific
    to this applet. It was initialized during the AEEClsCreateInstance() function.

    ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean Trans_HandleEvent(Trans* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  

    switch (eCode) 
    {
        // App is told it is starting up
        case EVT_APP_START:                        
            {
                IImage *pBGImage = NULL;
                IImage *pTranImage = NULL;
                AEERect Rect = {pMe->DeviceInfo.cxScreen / 4, 0 , pMe->DeviceInfo.cxScreen / 4 * 3,pMe->DeviceInfo.cyScreen / 2};
                AEERect RectScreen = {0,0,pMe->DeviceInfo.cxScreen,pMe->DeviceInfo.cyScreen};

                IDISPLAY_FillRect(pMe->a.m_pIDisplay,&RectScreen,MAKE_RGB(0,0,0));
                pBGImage = ISHELL_LoadResImage(pMe->a.m_pIShell,RES_RES_FILE,IDI_OBJECT_5002);
                pTranImage = ISHELL_LoadResImage(pMe->a.m_pIShell,RES_RES_FILE,IDI_OBJECT_5003);

                if(NULL != pBGImage)
                {
                    AEEImageInfo rInfo;
                    IIMAGE_GetInfo(pBGImage,&rInfo);
                    IIMAGE_Draw(pBGImage,0,(pMe->DeviceInfo.cyScreen - rInfo.cy) / 2);
                    IIMAGE_Release(pBGImage);
                    pBGImage = NULL;
                }

                
                if(NULL != pTranImage)
                {
                    DrawTransImage(pMe->a.m_pIDisplay, pTranImage,pMe->DeviceInfo.cxScreen / 4,pMe->DeviceInfo.cyScreen / 2,50);
                    IIMAGE_Release(pTranImage);
                    pTranImage = NULL;
                }

                DrawTransRect(pMe->a.m_pIDisplay,&Rect,40);
            }

            return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...

              return(TRUE);


        // App is being suspended 
        case EVT_APP_SUSPEND:
            // Add your code here...

              return(TRUE);


        // App is being resumed
        case EVT_APP_RESUME:
            // Add your code here...

              return(TRUE);


        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
            // Add your code here...

              return(TRUE);

        // A key was pressed. Look at the wParam above to see which key was pressed. The key
        // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
        case EVT_KEY:
            // Add your code here...

              return(TRUE);


        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}


// this function is called when your application is starting up
boolean Trans_InitAppData(Trans* pMe)
{
    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

    // Insert your code here for initializing or allocating resources...



    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void Trans_FreeAppData(Trans* pMe)
{
    // insert your code here for freeing any resources you have allocated...

    // example to use for releasing each interface:
    // if ( pMe->pIMenuCtl != NULL )         // check for NULL first
    // {
    //    IMENUCTL_Release(pMe->pIMenuCtl)   // release the interface
    //    pMe->pIMenuCtl = NULL;             // set to NULL so no problems trying to free later
    // }
    //

}

boolean DrawTransImage(IDisplay * pIDisplay, IImage* pImage, int x, int y, uint8 nTrans )
{
    //IDisplay* pIDisplay = ((AEEApplet*) GETAPPINSTANCE() )->m_pIDisplay;
    //IShell* pIShell = ((AEEApplet*) GETAPPINSTANCE() )->m_pIShell;

    IBitmap* pBmpSrn = NULL;
    IBitmap* pBmpBg = NULL;

    NativeColor NCSrn, NCBg;
    AEEImageInfo rInfo;    

    if( pImage )
    {
        // 如果透明度为0,直接画图,返回
        if( nTrans == 0 )
        {
            IIMAGE_Draw( pImage, x, y );
            return TRUE;
        }

        // 取得目标图像信息
        IIMAGE_GetInfo( pImage, &rInfo );    
    } 
    else
    {
        return FALSE;
    }

    // 取得与设备兼容的位图
    if( ( pBmpSrn = IDISPLAY_GetDestination( pIDisplay ) ) == NULL )
        return FALSE;

    // 创建图像背景位图
    if( ENOMEMORY == IBITMAP_CreateCompatibleBitmap( pBmpSrn, &pBmpBg, rInfo.cx, rInfo.cy ) )
    {        
        IBITMAP_Release( pBmpSrn );
        return FALSE;
    }

    // 将当前屏幕位图复制到图像背景位图中
    IBITMAP_BltIn( pBmpBg, 0, 0, rInfo.cx, rInfo.cy, pBmpSrn, x, y, AEE_RO_COPY );

    // 将目标图像画在当前屏幕位图上
    IIMAGE_Draw( pImage, x, y );

    // 像素替换
    if( nTrans < 100 )
    {    
        int i, j;
        uint32 pSrn[3], pBg[3];

        for ( i = 0; i < rInfo.cx; i++)
        {
            for ( j = 0; j < rInfo.cy; j++)
            {
                IBITMAP_GetPixel( pBmpBg, i, j, &NCBg );
                IBITMAP_GetPixel( pBmpSrn, x + i, y + j, &NCSrn );

                pBg[0] = NCBg & 0x1F;
                pBg[1] = (NCBg >>5) & 0x3F;
                pBg[2] = (NCBg >>11) & 0x1F;
                
                pSrn[0] = NCSrn & 0x1F;
                pSrn[1] = (NCSrn >>5) & 0x3F;
                pSrn[2] = (NCSrn >>11) & 0x1F;        

                pBg[0] = (pBg[0] * nTrans) / 100 + (pSrn[0] * (100 - nTrans)) / 100;
                pBg[1] = (pBg[1] * nTrans) / 100 + (pSrn[1] * (100 - nTrans)) / 100;
                pBg[2] = (pBg[2] * nTrans) / 100 + (pSrn[2] * (100 - nTrans)) / 100;

                NCBg = (pBg[2] << 11) + (pBg[1] << 5) + pBg[0];

                // 像素输出
                IBITMAP_DrawPixel( pBmpSrn, x + i, y + j, NCBg, AEE_RO_COPY );
            }
        }
    } 
    else
    {
        // 透明度大于100时,重新将背景位图复制到屏幕,不显示目标图像
        IDISPLAY_BitBlt( pIDisplay, x, y, rInfo.cx, rInfo.cy, pBmpBg, 0, 0, AEE_RO_COPY );
    }

    // 释放资源
    IBITMAP_Release( pBmpBg );
    IBITMAP_Release( pBmpSrn );

    // 屏幕刷新
    IDISPLAY_Update( pIDisplay );    

    return TRUE;
}

boolean DrawTransRect(IDisplay * pIDisplay, AEERect *pRect, uint8 nTrans )
{
    IBitmap* pBmpSrn = NULL;
    IBitmap* pBmpBg = NULL;
    int16 x = 0; 
    int16 y = 0;
    uint16 cx = 0;
    uint16 cy = 0;

    NativeColor NCSrn, NCBg;
    
    if( pRect )
    {
        if( nTrans == 0 )
        {
            IDISPLAY_FillRect( pIDisplay,(const AEERect *)pRect,MAKE_RGB(0xFF,0xFF,0xFF));
            IDISPLAY_Update( pIDisplay );
            return TRUE;
        }
        
        x = pRect->x;
        y = pRect->y;
        cx = pRect->dx;
        cy = pRect->dy;
    } 
    else
    {
        return FALSE;
    }
    
    if( ( pBmpSrn = IDISPLAY_GetDestination( pIDisplay ) ) == NULL )
        return FALSE;
    
    if( ENOMEMORY == IBITMAP_CreateCompatibleBitmap( pBmpSrn, &pBmpBg, cx, cy ) )
    {        
        IBITMAP_Release( pBmpSrn );
        return FALSE;
    }
    
    IBITMAP_BltIn( pBmpBg, 0, 0, cx, cy, pBmpSrn, x, y, AEE_RO_COPY );
    IDISPLAY_FillRect( pIDisplay,(const AEERect *)pRect,MAKE_RGB(0xFF,0xFF,0xFF));
    
    if( nTrans < 100 )
    {    
        int i, j;
        uint32 pSrn[3], pBg[3];
        
        for ( i = 0; i < cx; i++)
        {
            for ( j = 0; j < cy; j++)
            {
                IBITMAP_GetPixel( pBmpBg, i, j, &NCBg );
                IBITMAP_GetPixel( pBmpSrn, x + i, y + j, &NCSrn );
                
                pBg[0] = NCBg & 0x1F;
                pBg[1] = (NCBg >>5) & 0x3F;
                pBg[2] = (NCBg >>11) & 0x1F;
                
                pSrn[0] = NCSrn & 0x1F;
                pSrn[1] = (NCSrn >>5) & 0x3F;
                pSrn[2] = (NCSrn >>11) & 0x1F;        
                
                pBg[0] = (pBg[0] * nTrans) / 100 + (pSrn[0] * (100 - nTrans)) / 100;
                pBg[1] = (pBg[1] * nTrans) / 100 + (pSrn[1] * (100 - nTrans)) / 100;
                pBg[2] = (pBg[2] * nTrans) / 100 + (pSrn[2] * (100 - nTrans)) / 100;
                
                NCBg = (pBg[2] << 11) + (pBg[1] << 5) + pBg[0];
                
                IBITMAP_DrawPixel( pBmpSrn, x + i, y + j, NCBg, AEE_RO_COPY );
            }
        }
    } 
    else
    {
        IDISPLAY_BitBlt( pIDisplay, x, y, cx, cy, pBmpBg, 0, 0, AEE_RO_COPY );
    }
    
    IBITMAP_Release( pBmpBg );
    IBITMAP_Release( pBmpSrn );
    IDISPLAY_Update( pIDisplay );    
    
    return TRUE;
}
