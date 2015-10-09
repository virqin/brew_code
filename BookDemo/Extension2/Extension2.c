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
#include "Extension2.bid"
#include "Extension2.h"


int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
{
     *ppObj = NULL;

     if( ClsId == AEECLSID_EXTCLASS2_CLS )
     {
          if( ExtClass_New(sizeof(extclass2_t), pIShell, po,
               (IModule **)ppObj) == SUCCESS )
                    return AEE_SUCCESS;
     }

     return EFAILED;
}
static uint32 ExtClass2_AddRef(IExtClass2 * po)
{
     return (++((extclass2_t *)po)->m_nRefs);
}
static uint32 ExtClass2_Release(IExtClass2 * po)
{
     extclass2_t *     pMe = (extclass2_t *)po;

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
     FREE(pMe);

     return 0;
}

void ExtClass2_HelloWorld(IExtClass2 * po)
{
     //simply draw "hello world" string as we do in the early examples

     extclass2_t *     pMe = (extclass2_t *) po;
     AECHAR     szBuf[] = {'H','e','l','l','o',' ', 'L', 'i', 'P', 
                           'e', 'n','g', '\0'};

     if(!pMe || !pMe->m_pIDisplay)
          return;

     IDISPLAY_DrawText(pMe->m_pIDisplay,
           AEE_FONT_BOLD, szBuf, -1, 0, 0, 0, IDF_ALIGN_CENTER | 
           IDF_ALIGN_MIDDLE);
}
int ExtClass_New(int16 nSize, IShell *pIShell, IModule* pIModule, 
IModule ** ppMod)
{

     extclass2_t *        pMe = NULL;
     VTBL(IExtClass2) *   modFuncs;

     if( !ppMod || !pIShell || !pIModule )
          return EFAILED;

     *ppMod = NULL;

     // Allocate memory for the ExtensionCls object
     if( nSize < sizeof(extclass2_t) )
          nSize += sizeof(extclass2_t);

     if( (pMe = (extclass2_t *)MALLOC(nSize + sizeof(IExtClass2Vtbl))) == NULL )
          return ENOMEMORY;

     // Allocate the vtbl and assign each function pointer
     // to the correct function address
     modFuncs = (IExtClass2Vtbl *)((byte *)pMe + nSize);

     modFuncs->AddRef           = ExtClass2_AddRef;
     modFuncs->Release          = ExtClass2_Release;
     modFuncs->DrawHelloWorld   = ExtClass2_HelloWorld;

     //initialize the vtable
     INIT_VTBL(pMe, IModule, *modFuncs);

     //We've got one reference since this class is allocated
     pMe->m_nRefs = 1;

     // initialize our internal member variables
     pMe->m_pIShell    = pIShell;
     pMe->m_pIModule   = pIModule;

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
