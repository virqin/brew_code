/*===========================================================================

FILE: Extension.c
===========================================================================*/
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEStdlib.h"
#include "Extension.bid"
#include "Extension.h"
#include "Test.h"

extern "C"
{
	int ExtClass_New(int16 nSize, IShell *pIShell, IModule* pIModule, 
IModule ** ppMod);
}
extern "C" 
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
{
     *ppObj = NULL;

     if( ClsId == AEECLSID_EXTCLASS_CLS )
     {
          if( ExtClass_New(sizeof(extclass_t), pIShell, po,
               (IModule **)ppObj) == SUCCESS )
                    return AEE_SUCCESS;
     }

     return EFAILED;
}
extern "C" static uint32 ExtClass_AddRef(IExtClass * po)
{
     return (++((extclass_t *)po)->m_nRefs);
}
extern "C" static uint32 ExtClass_Release(IExtClass * po)
{
     extclass_t *     pMe = (extclass_t *)po;

     //Decrease the number of references.  If we still
     //have some references to this object, return
     //and do not free resources
     if(-pMe->m_nRefs != 0)
          return pMe->m_nRefs;

     //Release display
     if(pMe->m_pIDisplay)
              IDISPLAY_Release(pMe->m_pIDisplay);

     // Release interfaces
     ISHELL_Release(pMe->m_pIShell);
     IMODULE_Release(pMe->m_pIModule);
     //Free the object
     FREE_VTBL(pMe, IModule);
		 delete pMe->m_pApp;
     FREE(pMe);

     return 0;
}

extern "C" void ExtClass_DrawHelloWorld(IExtClass * po)
{
     //simply draw "hello world" string as we do in the early examples

     extclass_t *     pMe = (extclass_t *) po;
     AECHAR     szBuf[] = {'H','e','l','l','o',' ', 'W', 'o', 'r', 
                           'l', 'd', '\0'};

     if(!pMe || !pMe->m_pIDisplay)
         return;

     IDISPLAY_DrawText(pMe->m_pIDisplay,
           AEE_FONT_BOLD, szBuf, -1, 0, 0, 0, IDF_ALIGN_CENTER | 
           IDF_ALIGN_MIDDLE);
}
extern "C" int ExtClass_New(int16 nSize, IShell *pIShell, IModule* pIModule, 
IModule ** ppMod)
{

     extclass_t *        pMe = NULL;
     VTBL(IExtClass) *   modFuncs;

     if( !ppMod || !pIShell || !pIModule )
          return EFAILED;

     *ppMod = NULL;

     // Allocate memory for the ExtensionCls object
     if( nSize < sizeof(extclass_t) )
          nSize += sizeof(extclass_t);

     if( (pMe = (extclass_t *)MALLOC(nSize + sizeof(IExtClassVtbl))) == NULL )
          return ENOMEMORY;

     // Allocate the vtbl and assign each function pointer
     // to the correct function address
     modFuncs = (IExtClassVtbl *)((byte *)pMe + nSize);

     modFuncs->AddRef           = ExtClass_AddRef;
     modFuncs->Release          = ExtClass_Release;
     modFuncs->DrawHelloWorld   = ExtClass_DrawHelloWorld;
     //initialize the vtable
     INIT_VTBL(pMe, IModule, *modFuncs);

     //We've got one reference since this class is allocated
     pMe->m_nRefs = 1;

     // initialize our internal member variables
     pMe->m_pIShell    = pIShell;
     pMe->m_pIModule   = pIModule;

		 pMe->m_pApp = new Test();
     //Add References to the interfaces we're using
     ISHELL_AddRef(pIShell);
     IMODULE_AddRef(pIModule);

     //we need to get access to the screen
     if( ISHELL_CreateInstance(pIShell, AEECLSID_DISPLAY,
          (void **)&pMe->m_pIDisplay) != SUCCESS )
     return EFAILED;

     *ppMod = (IModule*)pMe;

     return AEE_SUCCESS;
}
