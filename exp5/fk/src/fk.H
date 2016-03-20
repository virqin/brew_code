#ifndef _EG_
#define _EG_

#include "AEEBitmap.h"
#include "AEEModGen.h"        // Module interface definitions
#include "AEEAppGen.h"        // Applet interface definitions
#include "AEEShell.h"         // Shell interface definitions


#include "AEEFile.h"   // File interface definitions
#include "AEEStdlib.h"
#include "AEEBitmap.h"
#include "AEEMenu.h"


#include "Commondef.h"


#include "fk_res.h"
#include "fk.bid"

#include "GAME.h"
#include "STATE.h"
#include "helpfunc.h"
#include "PicBrowser.h"
#include "MainMenu.h"
#include "read.h"
#include "ani.h"
#include "CommonDefine.h"
#include "ani_res.h"


static boolean EG_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void           EG_FreeAppData(GAME * pGame);
void           EG_InitAppData(GAME * pGame);
void           FreeIF(IBase ** ppif);

#endif