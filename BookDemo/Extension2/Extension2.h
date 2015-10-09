#ifndef EXTENSION_H
#define EXTENSION_H
typedef struct _IExtClass2 IExtClass2;

QINTERFACE(IExtClass2)
{
	DECLARE_IBASE(IExtClass2)
	void      (*DrawHelloWorld)(IExtClass2 *po);
};

#define IEXTCLASS2_AddRef(p)           GET_PVTBL(p,IExtClass2)->AddRef(p)
#define IEXTCLASS2_Release(p)          GET_PVTBL(p,IExtClass2)->Release(p)
#define IEXTCLASS2_DrawHelloWorld(p)   GET_PVTBL(p,IExtClass2)->DrawHelloWorld(p)

typedef struct extclass2_s
{
	// Declare VTable
	DECLARE_VTBL(IExtClass2)
	// Class member variables
	uint32       m_nRefs;
	IShell *     m_pIShell;
	IDisplay *   m_pIDisplay;
	IModule *    m_pIModule;
} extclass2_t;

#endif 