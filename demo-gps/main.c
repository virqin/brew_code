
#include "AEEAppGen.h"
#include "AEEStdLib.h"
#include "AEEError.h"
#include "demo.bid"

extern int demo_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);
extern int sound_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);
extern int sound1_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);
extern int exsound_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);
extern int sms_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);
extern int media_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);
extern int media2_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);

extern int text_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);
extern int net_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);
extern int gps_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);
extern int gps2_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);

extern int dlg2_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);
extern int tel_AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj);

int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,void ** ppObj)
{
	if (ClsId != AEECLSID_DEMO)
		return EFAILED;

	return tel_AEEClsCreateInstance(ClsId, pIShell, pMod, ppObj);
}

void screen_show(const char * msg, AEEApplet *pMe)
{
	AEERect rect = {0, 0, 150, 20};
	AECHAR wmsg[100];

	STR_TO_WSTR(msg, wmsg, 100);
	IDISPLAY_ClearScreen(pMe->m_pIDisplay);
	IDISPLAY_DrawText(pMe->m_pIDisplay, AEE_FONT_BOLD, wmsg, -1, 0, 0, &rect, IDF_ALIGN_TOP | IDF_ALIGN_LEFT);
	IDISPLAY_Update (pMe->m_pIDisplay);
	rect.y += 20;
}
