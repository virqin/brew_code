/*===========================================================================

FILE: ExRSA.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions


#include "ExRSA.bid"
#include "AEERSA.h"
#include "AEESecurity.h"
#include "AEEStdLib.h"
#include "ExRsa_res.h"
#include "AEEGraphics.h" 

/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
typedef struct _ExRSA {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
	AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
	IDisplay      *pIDisplay;  // give a standard way to access the Display interface
	IShell        *pIShell;    // give a standard way to access the Shell interface

	// add your own variables here...
	IRSA *pIRSA;
	ICipher* pICipher;



} ExRSA;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean ExRSA_HandleEvent(ExRSA* pMe, 
																	AEEEvent eCode, uint16 wParam, 
																	uint32 dwParam);
boolean ExRSA_InitAppData(ExRSA* pMe);
void    ExRSA_FreeAppData(ExRSA* pMe);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================
FUNCTION: AEEClsCreateInstance
===========================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;

	if( ClsId == AEECLSID_EXRSA )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(ExRSA),
			ClsId,
			pIShell,
			po,
			(IApplet**)ppObj,
			(AEEHANDLER)ExRSA_HandleEvent,
			(PFNFREEAPPDATA)ExRSA_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function

		{
			//Initialize applet data, this is called before sending EVT_APP_START
			// to the HandleEvent function
			if(ExRSA_InitAppData((ExRSA*)*ppObj))
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
===========================================================================*/
static boolean ExRSA_HandleEvent(ExRSA* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  

	switch (eCode) 
	{
		// App is told it is starting up
	case EVT_APP_START:       
		{
			/*
			AECHAR *pBuffer;
			uint32 resSize;
			int fontHeight;
			int pageLine;
			int i;
			int fitNumber;
			int count;
			ISHELL_GetResSize(pMe->pIShell,EXRSA_RES_FILE,IDS_DEMO,RESTYPE_STRING,&resSize);
			pBuffer = (AECHAR*)MALLOC(resSize);
			ISHELL_LoadResString(pMe->pIShell,EXRSA_RES_FILE,IDS_DEMO,pBuffer,resSize);
			IDISPLAY_SetColor(pMe->pIDisplay,CLR_USER_TEXT,MAKE_RGB(0,0,0));
			IDISPLAY_SetColor(pMe->pIDisplay,CLR_USER_BACKGROUND,MAKE_RGB(255,255,255));
			fontHeight = IDISPLAY_GetFontMetrics(pMe->pIDisplay,AEE_FONT_NORMAL,NULL,NULL);
			pageLine = pMe->DeviceInfo.cyScreen/fontHeight;
			count = 0;
			for(i = 0;i<pageLine;i++)
			{
				IDISPLAY_MeasureTextEx(pMe->pIDisplay,AEE_FONT_NORMAL,pBuffer+count,-1,pMe->DeviceInfo.cxScreen,&fitNumber);
				if(fitNumber!=-1)
				{
					IDISPLAY_DrawText(pMe->pIDisplay,AEE_FONT_NORMAL,
						pBuffer+count,fitNumber,0,i*fontHeight,
						NULL,IDF_TEXT_TRANSPARENT);
					count += fitNumber;
				}
				else
				{
					IDISPLAY_DrawText(pMe->pIDisplay,AEE_FONT_NORMAL,pBuffer+count,-1,0,i*fontHeight,NULL,IDF_TEXT_TRANSPARENT);
				}
			}
			IDISPLAY_Update(pMe->pIDisplay);
			FREE(pBuffer);*/
			IGraphics *pIGra;
			AEEPoint point;
			AEECircle circle;
			AEEEllipse ellipise;
			point.x = 10;
			point.y = 10;
			circle.cx = 100;
			circle.cy = 100;
			circle.r = 10;
			ellipise.cx = 50;
			ellipise.cy = 50;
			ellipise.wx = 10;
			ellipise.wy = 20;


			ISHELL_CreateInstance(pMe->pIShell,AEECLSID_GRAPHICS,(void**)&pIGra);
			//IGRAPHICS_SetBackground(pIGra,0,0,0);
			IGRAPHICS_ClearViewport(pIGra);
			IGRAPHICS_DrawPoint(pIGra,&point);
			IGRAPHICS_SetFillColor(pIGra,255,0,0,0);
			IGRAPHICS_SetColor(pIGra,0,255,0,0);
			IGRAPHICS_SetFillMode(pIGra,TRUE);
			IGRAPHICS_DrawCircle(pIGra,&circle);
			//STROKE_DOTTED
				IGRAPHICS_SetStrokeStyle(pIGra,STROKE_DOTTED);
			//IGRAPHICS_SetStokeStyle(pIGra,STROKE_SOLID);
							IGRAPHICS_SetFillColor(pIGra,255,255,0,0);
			IGRAPHICS_SetColor(pIGra,0,255,255,0);
				IGRAPHICS_DrawEllipse(pIGra,&ellipise);
			IGRAPHICS_Update(pIGra);
			IGRAPHICS_Release(pIGra);



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
boolean ExRSA_InitAppData(ExRSA* pMe)
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

	//ISHELL_CreateInstance(pMe->pIShell,AEECLSID_RSA,(void**)&pMe->pIRSA);
	//ISHELL_CreateInstance(pMe->pIShell,AEECLSID_ARC4,(void **)&pMe->pICipher);

	// if there have been no failures up to this point then return success
	return TRUE;
}

// this function is called when your application is exiting
void ExRSA_FreeAppData(ExRSA* pMe)
{
	//IRSA_Release(pMe->pIRSA);
}


			/*
			//对称加密算法
			ICipher* pICipher = NULL;
			IRSA* pIRSA = NULL;

			byte szInput[]	= "Test Data";				// String to encrypt
			byte szOutput[16]={0};
			byte key [16]	= {
				0x73,0xA5,0x7C,0xAC,
					0xCC,0xBB,0x78,0xA9,
					0xD6,0xD1,0x08,0x01,
					0x87,0x33,0xDA,0xE1
			};

			byte N[]={
				0x23,0x2E,0x0B,0x8F,0xD5,0x27,0x12,0x24,0x47,0x62,0xA7,0x4C,0xCD,0x56,0x0D,0x93,
					0xEC,0xEB,0xB6,0x14,0xF7,0xB3,0xDF,0x8A,0x67,0x65,0xFB,0x3C,0xB2,0x72,0x65,0x61,
					0x0A,0xA3,0xBA,0x1E,0x4C,0x63,0x59,0x2D,0xF7,0xF7,0x2E,0x7A,0x13,0x0F,0x1C,0xB4,
					0x4A,0x02,0x3A,0x6C,0xF0,0xB8,0x14,0x13,0x03,0xF1,0x6C,0x57,0x02,0x1F,0x46,0x32,
					0x4F,0xD6,0x2A,0x1B,0x1D,0xC8,0x77,0xBD,0x87,0xE0,0xAA,0x71,0x47,0xD1,0x01,0x7F,
					0x9C,0x51,0xD9,0xF0,0x8E,0xCC,0x9C,0x9D,0x6D,0x48,0xDC,0x19,0x12,0x83,0x53,0x0B,
					0x39,0xAB,0xEC,0xAE,0x8A,0x90,0x6A,0xFD,0x39,0xD1,0xCE,0xAD,0x10,0x31,0x80,0x69,
					0x81,0xE3,0xA5,0x65,0x81,0x17,0x82,0x9D,0x6D,0x2C,0x5E,0x2D,0xDF,0x1C,0x47,0xD5
			};
			byte E[] = {
				0x1D,0x5C,0x25,0x4C,0x11,0xE1,0x7C,0xB3,0x88,0x5C,0x1E,0x31,0xEB,0xC0,0xA0,0xAE,
					0x4E,0xF1,0x5D,0x51,0xDC,0x74,0x62,0xE0,0x98,0x70,0xB5,0x1F,0xAF,0xF1,0xE7,0x07,
					0xF9,0x20,0x31,0x89,0x6D,0xC9,0x10,0x39,0x5C,0x14,0x0F,0xD6,0xD3,0xA7,0x2E,0x16,
					0x8D,0x19,0x94,0xF7,0x2C,0x63,0x04,0x65,0xE6,0x49,0x8B,0x58,0x86,0x2D,0xE1,0xA0,
					0x11,0x7C,0xB4,0xB1,0x27,0x94,0xEB,0x6E,0xC9,0x47,0xB9,0x56,0xA5,0x88,0x51,0x91,
					0x95,0xE5,0x1C,0x37,0x6D,0x1A,0x77,0x42,0x22,0x2B,0x56,0x52,0xB6,0x4E,0x1F,0x20,
					0xF9,0x05,0x79,0x8F,0xF6,0x66,0x13,0x88,0x49,0xE2,0x39,0xC6,0x01,0x92,0xEE,0xB4,
					0x6F,0x01,0x2A,0x5D,0x90,0x53,0x43,0x86,0x94,0x97,0x06,0x92,0x14,0xD9,0x2A,0xB5};
				byte D[] = {0x01,0x00,0x01};


				int len = sizeof(szOutput);

				byte *pOutput;
				int size;
				ISHELL_CreateInstance(pMe->pIShell,AEECLSID_ARC4,(void **)&pICipher);
				if(ICIPHER_Init(pICipher, key, 16) != SUCCESS)
				{
				}

				if(ICIPHER_Cipher(pICipher, szInput, sizeof(szInput), szOutput, &len) == EFAILED)
				{
				}


				ICIPHER_Release(pICipher);

				if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_RSA,(void **)&pIRSA) != SUCCESS || pIRSA == NULL)
				{
				}

				if(IRSA_Init(pIRSA, N, sizeof(N), E, sizeof(E)) != SUCCESS)
				{
				}
				//把对称加密算法的密钥加密
				if(IRSA_RSA(pIRSA, key, sizeof(key), &pOutput, &size, 0) != SUCCESS)
				{
				}


				//解密
				if(IRSA_Init(pIRSA,  N, sizeof(N), D, sizeof(D)) != SUCCESS)
				{
				}

				if(IRSA_RSA(pIRSA, pOutput, size, &pOutput, &size, 0) != SUCCESS)
				{
				}

				ISHELL_CreateInstance(pMe->pIShell,AEECLSID_ARC4,(void **)&pICipher);
				if(ICIPHER_Init(pICipher, pOutput, size) != SUCCESS)
				{
				}
				if(ICIPHER_Cipher(pICipher, szOutput, len, szOutput, &len) == EFAILED)
				{
				}
				if(MEMCMP(szInput,szOutput,len)==0)
				{
					//成功解密
				}
				ICIPHER_Release(pICipher);

				IRSA_Release(pIRSA);*/



