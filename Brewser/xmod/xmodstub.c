#include "AEEStdLib.h"
#include "AEEShell.h"           /* Shell interface definitions */

int xModule_New(IShell *piShell, IModule **pp);

#ifdef AEE_SIMULATOR
AEEHelperFuncs *g_pvtAEEStdLibEntry;
__declspec(dllexport) /* AEEMod_Load has to be exported from a DLL */
#endif /* #ifdef AEE_SIMULATOR */

/*
   ||
   || This bit of code has to be FIRST in a module, and its module *must*
   ||  be compiled in ARM mode for handset targets.
   ||
*/
int AEEMod_Load(IShell *piShell, void *ph, IModule **ppMod)
{
   /*	
     ||  IMPORTANT NOTE: g_dwAEEStdLibEntry global variable is defined 
     ||     for SDK ONLY!
     || 
     ||  This variable should NOT BE (1) overwritten and/or (2) USED 
     ||    DIRECTLY by BREW SDK users. It is used as an entry point to 
     ||    AEEStdLib.
     || 
     ||  DO NOT REMOVE next three lines.
   */
#ifdef AEE_SIMULATOR
   g_pvtAEEStdLibEntry = (AEEHelperFuncs *)ph;
#endif

   /* xModule_New is not implemented by xmod, the user of xmod must
      supply it */
   return xModule_New(piShell,ppMod);
}

