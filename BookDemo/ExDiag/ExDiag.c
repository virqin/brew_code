/*===========================================================================

FILE: ExDiag.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "ExDiag.bid"
#include "ExDiag_res.h"
#include "AEEMenu.h"
#include "AEETransform.h"
#include "AEEText.h"
#include "AEEMenu.h"
#include "AEEStdLib.h"
#include "ExDiag.bid"

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
typedef struct _ExDiag {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface

    // add your own variables here...
		IDialog *pIDiag;
		IMenuCtl *pIMenuCtl;
		ITextCtl *pITextCtl;
		IStatic *pIStatic;
} ExDiag;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean ExDiag_HandleEvent(ExDiag* pMe, 
                                                   AEEEvent eCode, uint16 wParam, 
                                                   uint32 dwParam);
boolean ExDiag_InitAppData(ExDiag* pMe);
void    ExDiag_FreeAppData(ExDiag* pMe);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;

	if( ClsId == AEECLSID_EXDIAG )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(ExDiag),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)ExDiag_HandleEvent,
                          (PFNFREEAPPDATA)ExDiag_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function
			if(ExDiag_InitAppData((ExDiag*)*ppObj))
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

void ConvertPic(IBitmap *pbm)
{
	IDIB *pDib;
	int  i = 0, j = 0;
	int  src1,des1,src2,des2,src3,des3;
	byte byTemp1,byTemp2,byTemp3;
	int picWidth;
	int picHeight;
	AEEBitmapInfo info;
	IBITMAP_GetInfo(pbm,&info,sizeof(info));
	picWidth = info.cx;
	picHeight = info.cy;

	IBITMAP_QueryInterface(pbm, AEECLSID_DIB, (void**)&pDib);

	if (pDib->nDepth == 8)
	{
		for (i = 0; i < picHeight; i++)
		{
			for (j = 0; j < picWidth / 2; j++)
			{
				src1 = pDib->nPitch * i + j;
				des1 = pDib->nPitch * i + (picWidth - 1 - j);

				byTemp1 = pDib->pBmp[src1];
				pDib->pBmp[src1] = pDib->pBmp[des1];
				pDib->pBmp[des1] = byTemp1;
			}
		}
	}
	else if(pDib->nDepth == 4)
	{
		for(i = 0; i < picHeight; i ++)
		{
			for(j = 0; j < picWidth / 4; j ++)
			{
				src1 = pDib->nPitch * i + j;
				des1 = pDib->nPitch * i + (picWidth / 2 - 1 - j);

				byTemp1 = pDib->pBmp[src1];
				pDib->pBmp[src1] = pDib->pBmp[des1];
				pDib->pBmp[des1] = byTemp1;

				byTemp1 = pDib->pBmp[src1];
				pDib->pBmp[src1] = ((byTemp1 & 0xF) << 4) | ((byTemp1 & 0xF0) >> 4);
				byTemp1 = pDib->pBmp[des1];
				pDib->pBmp[des1] = ((byTemp1 & 0xF) << 4) | ((byTemp1 & 0xF0) >> 4);
			}
		}
	}
	else if (pDib->nDepth == 16)
	{
		for (i = 0; i < picHeight; i++)
		{
			for (j = 0; j < picWidth / 2; j++)
			{
				src1 = pDib->nPitch * i + j * 2;
				src2 = pDib->nPitch * i + j * 2 + 1;
				des1 = pDib->nPitch * i + (picWidth - 1 - j) * 2;
				des2 = pDib->nPitch * i + (picWidth - 1 - j) * 2 + 1;

				byTemp1 = pDib->pBmp[src1];
				byTemp2 = pDib->pBmp[src2];
				pDib->pBmp[src1] = pDib->pBmp[des1];
				pDib->pBmp[src2] = pDib->pBmp[des2];
				pDib->pBmp[des1] = byTemp1;
				pDib->pBmp[des2] = byTemp2;
			}
		}
	}
	else if (pDib->nDepth == 24)
	{
		for (i = 0; i < picHeight; i++)
		{
			for (j = 0; j < picWidth / 2; j++)
			{
				src1 = pDib->nPitch * i + j * 3;
				src2 = pDib->nPitch * i + j * 3 + 1;
				src3 = pDib->nPitch * i + j * 3 + 2;
				des1 = pDib->nPitch * i + (picWidth - 1 - j) * 3;
				des2 = pDib->nPitch * i + (picWidth - 1 - j) * 3 + 1;
				des3 = pDib->nPitch * i + (picWidth - 1 - j) * 3 + 2;

				byTemp1 = pDib->pBmp[src1];
				byTemp2 = pDib->pBmp[src2];
				byTemp3 = pDib->pBmp[src3];
				pDib->pBmp[src1] = pDib->pBmp[des1];
				pDib->pBmp[src2] = pDib->pBmp[des2];
				pDib->pBmp[src3] = pDib->pBmp[des3];
				pDib->pBmp[des1] = byTemp1;
				pDib->pBmp[des2] = byTemp2;
				pDib->pBmp[des3] = byTemp3;
			}
		}
	}
	
	//pbm = IDIB_TO_IBITMAP(pDib);
	IDIB_Release(pDib);
}
/*===========================================================================
FUNCTION SampleAppWizard_HandleEvent
===========================================================================*/
static boolean ExDiag_HandleEvent(ExDiag* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
				//IMENUCTL_HandleEvent(pMe->pIMenuCtl,eCode,wParam,dwParam);
	if(pMe->pITextCtl)
	{
		ITEXTCTL_HandleEvent(pMe->pITextCtl,eCode,wParam,dwParam);
	}
	if(pMe->pIStatic)
	{
		ISTATIC_HandleEvent(pMe->pIStatic,eCode,wParam,dwParam);
	}


    switch (eCode) 
	{
        // App is told it is starting up
        case EVT_APP_START:                        
		    // Add your code here...
					{
						
						int res;
						IMenuCtl *pList;
						res = ISHELL_CreateDialog(pMe->pIShell,EXDIAG_RES_FILE,IDD_DEMO,NULL);
						pMe->pIDiag = ISHELL_GetActiveDialog(pMe->pIShell);
						IDIALOG_SetFocus(pMe->pIDiag,IDD_LIST);
						pList = (IMenuCtl*)IDIALOG_GetControl(pMe->pIDiag,IDD_LIST);
						{
							AEERect rect;
							rect.x = 0;
							rect.y = 20;
							rect.dx = pMe->DeviceInfo.cxScreen;
							rect.dy = 20;
							IMENUCTL_SetRect(pList,&rect);
						}
						IDIALOG_Redraw(pMe->pIDiag);
						/*
						//b
						/*IImage *pImage;
						AEEImageInfo imageInfo;
						pImage = ISHELL_LoadImage(pMe->pIShell,"demo.bmp");
						IIMAGE_GetInfo(pImage,&imageInfo);
						//IIMAGE_SetParm(pImage,IPARM_ROP,AEE_RO_TRANSPARENT,0);
						//IIMAGE_
						//png,图片的透明不好用，
						
						IIMAGE_Draw(pImage,0,0);
						//IIMAGE_SetFrameSize(pImage,20);
						//IIMAGE_Start(pImage,0,0);
						IDISPLAY_Update(pMe->pIDisplay);
						IIMAGE_Release(pImage);
						//IBitmap *pIBmp = ISHELL_LoadBitmap(pMe->pIShell,"demo.bmp");


						//IDISPLAY_Update(pMe->pIDisplay);
						IBitmap *pScreen;
						IBitmap *pBitmap;
						IBitmap *pBmp2;
						IBitmap *pImage;
						AEEBitmapInfo Info;
						NativeColor color;
						ITransform *pITransform;
						AEETransformMatrix matrix;

						pScreen = IDISPLAY_GetDestination(pMe->pIDisplay);
						pImage = ISHELL_LoadBitmap(pMe->pIShell,"demo.bmp");
						if(pImage==NULL)
						{
							IBITMAP_Release(pScreen);
						}
						IBITMAP_GetInfo(pImage, &Info,sizeof(Info));
						IBITMAP_CreateCompatibleBitmap(pScreen, &pBitmap, (uint16)Info.cx,(uint16)Info.cy);
						IBITMAP_Release(pScreen);
						if(pBitmap==NULL)
						{
							IBITMAP_Release(pImage);
						}
						IBITMAP_BltIn(pBitmap,0,0,Info.cx,Info.cy,pImage,0,0,AEE_RO_COPY);
						IBITMAP_Release(pImage);
						IBITMAP_GetPixel(pBitmap,0,0,&color);
						IBITMAP_SetTransparencyColor(pBitmap,color);

						IBITMAP_CreateCompatibleBitmap(pBitmap, &pBmp2, (uint16)Info.cx,(uint16)Info.cy);
						IBITMAP_QueryInterface(pBmp2,AEECLSID_TRANSFORM,(void**)&pITransform);
						matrix.A = -256;
						matrix.B = 0;
						matrix.C = 0;
						matrix.D = 256;
						ITRANSFORM_TransformBltSimple(pITransform,0,0,pBitmap,0,0,Info.cx,Info.cy,TRANSFORM_ROTATE_90,COMPOSITE_OPAQUE);
						//ITRANSFORM_TransformBltComplex(pITransform,0,0,pBitmap,0,0,20,20,&matrix,COMPOSITE_OPAQUE);

						ITRANSFORM_Release(pITransform);

						IDISPLAY_BitBlt(pMe->pIDisplay,0,0,Info.cx,Info.cy,pBmp2,0,0,AEE_RO_COPY);
						IDISPLAY_Update(pMe->pIDisplay);
						IBITMAP_Release(pBitmap);*/

						//IMENUCTL_SetActive(pMe->pIMenuCtl,TRUE);
					/*	{
							AEERect rect;
								AECHAR buffer[1]={0};
	
							ISHELL_CreateInstance(pMe->pIShell,AEECLSID_TEXTCTL,(void**)&pMe->pITextCtl);
							ISHELL_CreateInstance(pMe->pIShell,AEECLSID_SOFTKEYCTL,(void**)&pMe->pIMenuCtl);
							IMENUCTL_AddItem(pMe->pIMenuCtl,
							rect.x = 0;
							rect.y = 0;
							rect.dx = pMe->DeviceInfo.cxScreen;
							rect.dy = 50;
							 
							ITEXTCTL_SetTitle(pMe->pITextCtl,EXDIAG_RES_FILE,IDS_TEXT_TITLE,NULL);
							ITEXTCTL_SetRect(pMe->pITextCtl,&rect);
							ITEXTCTL_SetMaxSize(pMe->pITextCtl,15);
							ITEXTCTL_SetProperties(pMe->pITextCtl,TP_FRAME);
							ITEXTCTL_SetText(pMe->pITextCtl,buffer,1);
							//ITEXTCTL_SetInputMode(pMe->pITextCtl,AEE_TM_LETTERS);
							//ITEXTCTL_SetActive(pMe->pITextCtl,TRUE);
						}*/
						//ITEXTCTL_SetInputMode(pMe->pITextCtl,AEE_TM_LETTERS);
						//ITEXTCTL_SetActive(pMe->pITextCtl,TRUE);
						//IMENUCTL_SetActive(pMe->pIMenuCtl,TRUE);

					//	ITEXTCTL_SetActive(pMe->pITextCtl,TRUE);
					//ISTATIC_SetActive(pMe->pIStatic,TRUE);

					}
            return(TRUE);
				case EVT_DIALOG_INIT:
					
					return(TRUE);
				case EVT_DIALOG_START:
					
					return(TRUE);
				case EVT_DIALOG_END:
					return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...

      		return(TRUE);


        // App is being suspended 
        case EVT_APP_SUSPEND:
					if(pMe->pIStatic)
					{
						ISTATIC_Release(pMe->pIStatic);
						pMe->pIStatic = NULL;
					}
      		return(TRUE);
        case EVT_APP_RESUME:
					{
						AEERect rect;
						AECHAR *pBuffer;
						int size;
						rect.x = 0;
						rect.y = 0;
						rect.dx = pMe->DeviceInfo.cxScreen;
						rect.dy = pMe->DeviceInfo.cyScreen;
						ISHELL_CreateInstance(pMe->pIShell,AEECLSID_STATIC,(void**)&pMe->pIStatic);
						ISTATIC_SetRect(pMe->pIStatic,&rect);
						ISTATIC_SetProperties(pMe->pIStatic,ST_NOSCROLL);
						ISHELL_GetResSize(pMe->pIShell,EXDIAG_RES_FILE,IDS_STATIC,RESTYPE_STRING,&size);
						pBuffer = (AECHAR*)MALLOC(size);
						ISHELL_LoadResString(pMe->pIShell,EXDIAG_RES_FILE,IDS_STATIC,pBuffer,size);
						ISTATIC_SetText(pMe->pIStatic,NULL,pBuffer,AEE_FONT_NORMAL,AEE_FONT_NORMAL);
						FREE(pBuffer);
					}
      		return(TRUE);
				case EVT_COMMAND:
					switch(wParam)
					{
					case IDS_SOFT_1:
						{
							AECHAR userName[20];
							ITEXTCTL_GetText(pMe->pITextCtl,userName,19);
						}
						break;

					}
					break;


        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
		    // Add your code here...

      		return(TRUE);

        // A key was pressed. Look at the wParam above to see which key was pressed. The key
        // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
        case EVT_KEY:
					//if(pMe->pIMenuCtl)
					{
						//if(IMENUCTL_HandleEvent(pMe->pIMenuCtl,eCode,wParam,dwParam))
						{
							//return TRUE;
						}
					}
					//if(pMe->pITextCtl)
					{
						//IMENUCTL_HandleEvent(pMe->pIMenuCtl,eCode,wParam,dwParam);
						//if(ITEXTCTL_HandleEvent(pMe->pITextCtl,eCode,wParam,dwParam))
						{
							//return TRUE;
						}
					//	else
						{
							//if(wParam==AVK_SELECT)
							{
							//	return TRUE;

							}
						}
					}

      		
					break;


        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }

   return FALSE;
}


// this function is called when your application is starting up
boolean ExDiag_InitAppData(ExDiag* pMe)
{
    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
		pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

    // The display and shell interfaces are always created by
    // default, so we'll asign them so that you can access
    // them via the standard "pMe->" without the "a."
    pMe->pIDisplay = pMe->a.m_pIDisplay;
    pMe->pIShell   = pMe->a.m_pIShell;

    // Insert your code here for initializing or allocating resources...

		{
			AEERect rect;
			CtlAddItem ai;



		/*	ai.dwData = 2;
			ai.pszResImage = EXDIAG_RES_FILE;
			ai.pszResText = EXDIAG_RES_FILE;
			ai.wItemID = IDS_MENU_3;
			ai.wText = IDS_MENU_3;
			ai.wImage = IDB_DIR;
			ai.pText = NULL;
			ai.pImage = NULL;
			IMENUCTL_AddItemEx(pMe->pIMenuCtl, &ai);

			ai.wItemID = IDS_MENU_1;
			ai.wText = IDS_MENU_1;
			ai.wImage = IDB_ICON1;
			IMENUCTL_AddItemEx(pMe->pIMenuCtl, &ai);
			ai.wText = IDS_MENU_2;
			ai.wItemID = IDS_MENU_2;
			ai.wImage = IDB_ICON2;*/
			//IMENUCTL_EnableCommand(pMe->pIMenuCtl,FALSE);
			//IMENUCTL_AddItemEx(pMe->pIMenuCtl, &ai);
			//IMENUCTL_SetRect(pMe->pIMenuCtl,&rect);
			//IMENUCTL_SetActive(pMe->pIMenuCtl,TRUE);		
		}
		/*{
			AEERect rect;
			ISHELL_CreateInstance(pMe->pIShell,AEECLSID_SOFTKEYCTL,(void**)&pMe->pIMenuCtl);
			IMENUCTL_AddItem(pMe->pIMenuCtl,EXDIAG_RES_FILE,IDS_SOFT_1,IDS_SOFT_1,NULL,1);
			ISHELL_CreateInstance(pMe->pIShell,AEECLSID_TEXTCTL,(void**)&pMe->pITextCtl);
			rect.x = 0;
			rect.y = 0;
			rect.dx = pMe->DeviceInfo.cxScreen;
			rect.dy = 50;

			ITEXTCTL_SetProperties(pMe->pITextCtl,TP_FRAME);
			ITEXTCTL_SetTitle(pMe->pITextCtl,EXDIAG_RES_FILE,IDS_TEXT_TITLE,NULL);
			ITEXTCTL_SetRect(pMe->pITextCtl,&rect);
			ITEXTCTL_SetMaxSize(pMe->pITextCtl,15);
			ITEXTCTL_SetSoftKeyMenu(pMe->pITextCtl, pMe->pIMenuCtl); 
			ITEXTCTL_SetInputMode(pMe->pITextCtl,AEE_TM_NUMBERS);
			//ITEXTCTL_EnableCommand(pMe->pITextCtl,TRUE,IDS_MENU_1);
			ITEXTCTL_SetActive(pMe->pITextCtl,TRUE);
		}*/
		{
			AEERect rect;
			AECHAR *pBuffer;
			int size;
			rect.x = 0;
			rect.y = 0;
			rect.dx = pMe->DeviceInfo.cxScreen;
			rect.dy = pMe->DeviceInfo.cyScreen;
			ISHELL_CreateInstance(pMe->pIShell,AEECLSID_STATIC,(void**)&pMe->pIStatic);
			ISTATIC_SetRect(pMe->pIStatic,&rect);
			ISTATIC_SetProperties(pMe->pIStatic,ST_NOSCROLL);
			ISHELL_GetResSize(pMe->pIShell,EXDIAG_RES_FILE,IDS_STATIC,RESTYPE_STRING,&size);
			pBuffer = (AECHAR*)MALLOC(size);
			ISHELL_LoadResString(pMe->pIShell,EXDIAG_RES_FILE,IDS_STATIC,pBuffer,size);
			ISTATIC_SetText(pMe->pIStatic,NULL,pBuffer,AEE_FONT_NORMAL,AEE_FONT_NORMAL);
			FREE(pBuffer);
		}



    // if there have been no failures up to this point then return success
    return TRUE;
}

// this function is called when your application is exiting
void ExDiag_FreeAppData(ExDiag* pMe)
{
	// insert your code here for freeing any resources you have allocated...
	if ( pMe->pIMenuCtl != NULL )         // check for NULL first
	{
		IMENUCTL_Release(pMe->pIMenuCtl);   // release the interface
		pMe->pIMenuCtl = NULL;             // set to NULL so no problems trying to free later
	}
	if(pMe->pITextCtl!=NULL)
	{
	}
}
