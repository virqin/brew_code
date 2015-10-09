#ifndef EXTENSION_H
#define EXTENSION_H
#ifdef __cplusplus
extern "C"
{
#endif
typedef struct _IExtClass IExtClass;

QINTERFACE(IExtClass)
{
	DECLARE_IBASE(IExtClass)
	void      (*DrawHelloWorld)(IExtClass * po);
};

#define IEXTCLASS_AddRef(p)           GET_PVTBL(p,IExtClass)->AddRef(p)
#define IEXTCLASS_Release(p)          GET_PVTBL(p,IExtClass)->Release(p)
#define IEXTCLASS_DrawHelloWorld(p)   GET_PVTBL(p,IExtClass)->DrawHelloWorld(p)

typedef struct extclass_s
{
	// Declare VTable
	DECLARE_VTBL(IExtClass)
	// Class member variables
	uint32       m_nRefs;
	IShell *     m_pIShell;
	IDisplay *   m_pIDisplay;
	IModule *    m_pIModule;
	void *m_pApp;
} extclass_t;

#ifdef __cplusplus
}
#endif



#endif 