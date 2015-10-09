/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "MemHook.h"
#include "CallStackUtil.h"
#include "MemBlockList.h"
#include "MemCheckerUtil.h"
#include "FuncHookUtil.h"

//#define FEATURE_BREWMP_SUPPORT

#ifdef FEATURE_BREWMP_SUPPORT
#define AEECLSID_Realloc 0x01030522
#include "AEEIEnv.h"
#include "AEEEnv.bid"
#endif

#define DBGMSG_HEADER   "## MemChecker ---- "
#define DBGMSG_APPINFO  "[%S](0x%p) " // App name, App clsid

typedef enum
{
    MEM_OP_REALLOC,
    MEM_OP_MALLOC,
    MEM_OP_FREE
} EMemOpType;

typedef int (*PFN_CreateInstance)(void *pif, AEECLSID cls, void ** ppo);

#ifdef FEATURE_BREWMP_SUPPORT
typedef int (*PFN_ErrRealloc)(IRealloc* piRealloc, int nSize, void** ppOut);
typedef int (*PFN_ErrReallocName)(IRealloc* piRealloc, int nSize, void** ppOut, const char* cpszName);
typedef int (*PFN_ErrReallocNoZI)(IRealloc* piRealloc, int nSize, void** ppOut);
typedef int (*PFN_ErrReallocNameNoZI)(IRealloc* piRealloc, int nSize, void** ppOut, const char* cpszName);
#endif

typedef struct MemHookContext
{
    AEECLSID     clsApp;
    EOutputMode  eOutputMode;

    IShell      *pIShell;
    IAppletCtl  *pIAppletCtl;
    StackHelper *pStackHelper;

    boolean      bInstalled;
    boolean      bInHookHandler;

    EMemOpType   eMemOpType;

    FuncHookCtx *pCtxMalloc;
    FuncHookCtx *pCtxFree;
    FuncHookCtx *pCtxRealloc;
    PFN_CreateInstance pfnOldCI;

#ifdef FEATURE_BREWMP_SUPPORT
    FuncHookCtx *pCtxEnvRealloc;
    FuncHookCtx *pCtxEnvReallocName;
    FuncHookCtx *pCtxEnvReallocNoZI;
    FuncHookCtx *pCtxEnvReallocNameNoZI;
    FuncHookCtx *pCtxEnvCI;
#endif

    list_head    mem_block_list;

} MemHookContext;

static MemHookContext g_MemHookCtx;

#define ENTER_HOOK_HANDLER  do {if (!g_MemHookCtx.bInHookHandler) { g_MemHookCtx.bInHookHandler = TRUE;
#define LEAVE_HOOK_HANDLER  g_MemHookCtx.bInHookHandler = FALSE; }} while (0)

static void OnCallStackEntryCB(void *pUser, int nIndex, CallStackEntry *pCSEntry)
{
    list_head *pCallStackList = pUser;
    CallStackNode *pCSNode = NULL;

    if (0 == nIndex) {
        return;
    }

    pCSNode = CallStackNode_New();
    pCSNode->csEntry = *pCSEntry;

    list_add_tail(pCallStackList, &pCSNode->list);
}

static MemBlockNode *AddNewMemBlockNode(void *pMem, uint32 dwSize)
{
    MemBlockNode *pNode = NULL;

    if (NULL == pMem) {
        return NULL;
    }

    pNode = MemBlockNode_New();
    pNode->pMem = pMem;
    pNode->nSize = dwSize;
    list_add_tail(&g_MemHookCtx.mem_block_list, &pNode->list);

    return pNode;
}

static void DumpCallStack(MemBlockNode *pMemBlockNode, CONTEXT *pContext, void *pAddr, const char *pszName)
{
    CallStackNode *pCSNode = NULL;

    if (NULL == pMemBlockNode) {
        return;
    }

    pCSNode = CallStackNode_New();
    pCSNode->csEntry.offset = (uint32)pAddr;
    strcpy(pCSNode->csEntry.name, pszName);

    list_add_tail(&pMemBlockNode->CallStackList, &pCSNode->list);

    StackHelper_StackWalk(g_MemHookCtx.pStackHelper, NULL, pContext, OnCallStackEntryCB, &pMemBlockNode->CallStackList);
}

static void *malloc_hook(uint32 dwSize)
{
    void *pMem = NULL;

    g_MemHookCtx.eMemOpType = MEM_OP_MALLOC;
    HOOK_PAUSE(g_MemHookCtx.pCtxMalloc);
    pMem = GET_HELPER()->malloc(dwSize);
    HOOK_RESUME(g_MemHookCtx.pCtxMalloc);
    g_MemHookCtx.eMemOpType = MEM_OP_REALLOC;

    ENTER_HOOK_HANDLER
    {
        AEECLSID clsApp = IAPPLETCTL_RunningApplet(g_MemHookCtx.pIAppletCtl);
        AECHAR *pszAppName = GetAppName(g_MemHookCtx.pIShell, clsApp);

        DbgPrintfLn(DBGMSG_HEADER DBGMSG_APPINFO"MALLOC: %d bytes, 0x%p", 
            pszAppName, clsApp, dwSize, pMem);
        FREEIF(pszAppName);

        if (g_MemHookCtx.clsApp == clsApp)
        {
            CONTEXT context = {0};
            GET_CURRENT_CONTEXT(context, CONTEXT_FULL);
            DumpCallStack(AddNewMemBlockNode(pMem, dwSize), &context, GET_HELPER()->malloc, __TOSTR__(MALLOC));
        }
    }
    LEAVE_HOOK_HANDLER;

    return pMem;
}

static void free_hook(void *pObj)
{
    ENTER_HOOK_HANDLER
    {
        AEECLSID clsApp = IAPPLETCTL_RunningApplet(g_MemHookCtx.pIAppletCtl);
        AECHAR *pszAppName = GetAppName(g_MemHookCtx.pIShell, clsApp);

        DbgPrintfLn(DBGMSG_HEADER DBGMSG_APPINFO"FREE: 0x%p", 
            pszAppName, clsApp, pObj);

        if (NULL != pObj) {
            if (!MemBlockList_RemoveNode(&g_MemHookCtx.mem_block_list, pObj) 
                && g_MemHookCtx.clsApp == clsApp) {
                DbgPrintfLn(DBGMSG_HEADER DBGMSG_APPINFO"Can not find the memory block which to be freed -- %p", 
                    pszAppName, clsApp, pObj);
            }
        }

        FREEIF(pszAppName);
    }
    LEAVE_HOOK_HANDLER;

    g_MemHookCtx.eMemOpType = MEM_OP_FREE;
    HOOK_PAUSE(g_MemHookCtx.pCtxFree);
    GET_HELPER()->free(pObj);
    HOOK_RESUME(g_MemHookCtx.pCtxFree);
    g_MemHookCtx.eMemOpType = MEM_OP_REALLOC;
}

static void *realloc_hook(void *pSrc, uint32 dwSize)
{
    void *pMem = NULL;

    HOOK_PAUSE(g_MemHookCtx.pCtxRealloc);
    pMem = GET_HELPER()->realloc(pSrc, dwSize);
    HOOK_RESUME(g_MemHookCtx.pCtxRealloc);

    ENTER_HOOK_HANDLER
    {
        AEECLSID clsApp = IAPPLETCTL_RunningApplet(g_MemHookCtx.pIAppletCtl);

        if (MEM_OP_REALLOC == g_MemHookCtx.eMemOpType)
        {
            AECHAR *pszAppName = GetAppName(g_MemHookCtx.pIShell, clsApp);

            DbgPrintfLn(DBGMSG_HEADER DBGMSG_APPINFO"REALLOC: 0x%p -->%d bytes, 0x%p", 
                pszAppName, clsApp, pSrc, dwSize, pMem);

            if (g_MemHookCtx.clsApp == clsApp)
            {
                if (NULL != pSrc && 
                    (NULL != pMem || 0 == dwSize)) {
                    // free old memory
                    if (!MemBlockList_RemoveNode(&g_MemHookCtx.mem_block_list, pSrc)) {
                        DbgPrintfLn(DBGMSG_HEADER DBGMSG_APPINFO"Can not find the memory block which to be freed -- %p", 
                            pszAppName, clsApp, pSrc);
                    }
                }

                {
                    CONTEXT context = {0};
                    GET_CURRENT_CONTEXT(context, CONTEXT_FULL);
                    DumpCallStack(AddNewMemBlockNode(pMem, dwSize), &context, GET_HELPER()->realloc, __TOSTR__(REALLOC));
                }
            }

            FREEIF(pszAppName);
        }
    }
    LEAVE_HOOK_HANDLER;

    g_MemHookCtx.eMemOpType = MEM_OP_REALLOC;
    return pMem;
}

static int CreateInstance_Hook(IShell *pIShell, AEECLSID cls, void **ppo)
{
    int nErr = SUCCESS;

    nErr = g_MemHookCtx.pfnOldCI(pIShell, cls, ppo);

    ENTER_HOOK_HANDLER
    {
        AEECLSID clsApp = IAPPLETCTL_RunningApplet(g_MemHookCtx.pIAppletCtl);
        AECHAR *pszAppName = GetAppName(g_MemHookCtx.pIShell, clsApp);

        DbgPrintfLn(DBGMSG_HEADER DBGMSG_APPINFO"IShell_CreateInstance: cls 0x%p, 0x%p",
            pszAppName, clsApp, cls, *ppo);
        FREEIF(pszAppName);

        if (g_MemHookCtx.clsApp == clsApp)
        {
            MemBlockNode *pNode = NULL;

            list_node *pos = NULL;

            list_find(&g_MemHookCtx.mem_block_list, *ppo, cmpfn_memblock, 0, pos);
            if (NULL != pos)
            {
                pNode = list_entry(pos, MemBlockNode, list);
                {
                    CONTEXT context = {0};
                    GET_CURRENT_CONTEXT(context, CONTEXT_FULL);
                    DumpCallStack(pNode, &context, g_MemHookCtx.pfnOldCI, __TOSTR__(ISHELL_CreateInstance));
                }
            }
        }
    }
    LEAVE_HOOK_HANDLER;

    return nErr;
}

#ifdef FEATURE_BREWMP_SUPPORT

static int ErrRealloc_Hook(IEnv* piEnv, int nSize, void** ppOut)
{
    int nErr = SUCCESS;
    void *pSrc = *ppOut;
    void *pMem = NULL;

    HOOK_PAUSE(g_MemHookCtx.pCtxRealloc);
    nErr = AEEGETPVTBL(piEnv, IEnv)->ErrRealloc(piEnv, nSize, ppOut);
    HOOK_RESUME(g_MemHookCtx.pCtxRealloc);

    pMem = *ppOut;

    ENTER_HOOK_HANDLER
    {
        AEECLSID clsApp = IAPPLETCTL_RunningApplet(g_MemHookCtx.pIAppletCtl);

        if (MEM_OP_REALLOC == g_MemHookCtx.eMemOpType)
        {
            AECHAR *pszAppName = GetAppName(g_MemHookCtx.pIShell, clsApp);

            DbgPrintfLn(DBGMSG_HEADER DBGMSG_APPINFO"REALLOC: 0x%p -->%d bytes, 0x%p",
                pszAppName, clsApp, pSrc, nSize, pMem);

            if (g_MemHookCtx.clsApp == clsApp)
            {
                if (NULL != pSrc &&
                    (NULL != pMem || 0 == nSize)) {
                    // free old memory
                    if (!MemBlockList_RemoveNode(&g_MemHookCtx.mem_block_list, pSrc)) {
                        DbgPrintfLn(DBGMSG_HEADER DBGMSG_APPINFO"Can not find the memory block which to be freed -- %p",
                            pszAppName, clsApp, pSrc);
                    }
                }

                {
                    CONTEXT context = {0};
                    GET_CURRENT_CONTEXT(context, CONTEXT_FULL);
                    DumpCallStack(AddNewMemBlockNode(pMem, nSize), &context, GET_HELPER()->realloc, __TOSTR__(REALLOC));
                }
            }

            FREEIF(pszAppName);
        }
    }
    LEAVE_HOOK_HANDLER;

    g_MemHookCtx.eMemOpType = MEM_OP_REALLOC;

    return nErr;
}

static int EnvCreateInstance_Hook(IEnv* piEnv, AEECLSID cls, void** ppo)
{
    int nErr = SUCCESS;

    HOOK_PAUSE(g_MemHookCtx.pCtxRealloc);
    nErr = IEnv_CreateInstance(piEnv, cls, ppo);
    HOOK_RESUME(g_MemHookCtx.pCtxRealloc);

    ENTER_HOOK_HANDLER
    {
        AEECLSID clsApp = IAPPLETCTL_RunningApplet(g_MemHookCtx.pIAppletCtl);
        AECHAR *pszAppName = GetAppName(g_MemHookCtx.pIShell, clsApp);

        DbgPrintfLn(DBGMSG_HEADER DBGMSG_APPINFO"IEnv_CreateInstance: cls 0x%p, 0x%p",
            pszAppName, clsApp, cls, *ppo);
        FREEIF(pszAppName);

        if (g_MemHookCtx.clsApp == clsApp)
        {
            MemBlockNode *pNode = NULL;

            list_node *pos = NULL;

            list_find(&g_MemHookCtx.mem_block_list, *ppo, cmpfn_memblock, 0, pos);
            if (NULL != pos)
            {
                pNode = list_entry(pos, MemBlockNode, list);
                {
                    CONTEXT context = {0};
                    GET_CURRENT_CONTEXT(context, CONTEXT_FULL);
                    DumpCallStack(pNode, &context, g_MemHookCtx.pfnOldCI, __TOSTR__(ISHELL_CreateInstance));
                }
            }
        }
    }
    LEAVE_HOOK_HANDLER;

    return nErr;
}

#endif

//==============================================================================

static void DumpMemLeak(list_head *pMemBlockList)
{
    list_node *posMemBlockNode = NULL;
    int nCount = 0;

    list_for_each(posMemBlockNode, pMemBlockList)
    {
        MemBlockNode *pMemBlockNode = NULL;
        list_node *posCSNode = NULL;
        int nIndex = 0;

        pMemBlockNode = list_entry(posMemBlockNode, MemBlockNode, list);

        DbgPrintfLn("");
        DbgPrintfLn(DBGMSG_HEADER"###########################################");
        DbgPrintfLn(DBGMSG_HEADER"memory leak: 0x%p, %d bytes", pMemBlockNode->pMem, pMemBlockNode->nSize);
        DbgPrintfLn(DBGMSG_HEADER"###########################################");

        list_for_each(posCSNode, &pMemBlockNode->CallStackList)
        {
            CallStackNode *pCSNode = NULL;

            pCSNode = list_entry(posCSNode, CallStackNode, list);

            if ('\0' != pCSNode->csEntry.lineFileName[0]) {
                DbgPrintf("%s(%d) : ", pCSNode->csEntry.lineFileName, pCSNode->csEntry.lineNumber);
            } else {
                DbgPrintf("----(--) : ");
            }

            DbgPrintf("(0x%p)", pCSNode->csEntry.offset);
            if ('\0' != pCSNode->csEntry.name[0]) {
                DbgPrintf("%s", pCSNode->csEntry.name);
                if (pCSNode->csEntry.offsetFromSmybol) {
                    DbgPrintf(" + 0x%x", pCSNode->csEntry.offsetFromSmybol);
                }
            } else {
                DbgPrintf("--Unknown Name--");
            }

            if ('\0' != pCSNode->csEntry.moduleName[0]) {
                DbgPrintf(" -- Module: %s", pCSNode->csEntry.moduleName);
                if (NULL != pCSNode->csEntry.symTypeString) {
                    DbgPrintf("(%s)", pCSNode->csEntry.symTypeString);
                }
            }

            DbgPrintfLn("");

            nIndex++;
        }

        DbgPrintfLn("");

        nCount++;
    }

    DbgPrintfLn("");
    DbgPrintfLn(DBGMSG_HEADER"===============================================");
    DbgPrintfLn(DBGMSG_HEADER"Total %d memory leaks.", nCount);
    DbgPrintfLn(DBGMSG_HEADER"===============================================");
    DbgPrintfLn("");
}

boolean MemHook_Install(MemHookInitParam *pParam)
{
    DWORD dwOldProtect = 0;

    if (g_MemHookCtx.bInstalled) {
        return FALSE;
    }

    INIT_LIST_HEAD(&g_MemHookCtx.mem_block_list);

    g_MemHookCtx.pIShell = pParam->pIShell;
    g_MemHookCtx.clsApp = pParam->clsApp;
    g_MemHookCtx.eOutputMode = CONSTRAIN(pParam->eOutputMode, OUTPUT_TO_DEBUGGER, OUTPUT_TO_FILE);
    ISHELL_CreateInstance(g_MemHookCtx.pIShell, AEECLSID_APPLETCTL, (void **)&g_MemHookCtx.pIAppletCtl);

    g_MemHookCtx.pStackHelper = StackHelper_New();
    if (g_MemHookCtx.pStackHelper) {
        StackHelper_Init(g_MemHookCtx.pStackHelper, 0);
    }

    // Hook aee_malloc(), aee_realloc(), aee_free()
    HOOK_FUNC(GET_HELPER()->malloc, malloc_hook, &g_MemHookCtx.pCtxMalloc);
    HOOK_FUNC(GET_HELPER()->free, free_hook, &g_MemHookCtx.pCtxFree);
    HOOK_FUNC(GET_HELPER()->realloc, realloc_hook, &g_MemHookCtx.pCtxRealloc);

    // Hook ISHELL_CreateInstance()
    g_MemHookCtx.pfnOldCI = g_MemHookCtx.pIShell->pvt->CreateInstance;
    MemoryProtect(g_MemHookCtx.pIShell->pvt, sizeof(*g_MemHookCtx.pIShell->pvt), PAGE_READWRITE, &dwOldProtect);
    g_MemHookCtx.pIShell->pvt->CreateInstance = CreateInstance_Hook;
    MemoryProtect(g_MemHookCtx.pIShell->pvt, sizeof(*g_MemHookCtx.pIShell->pvt), dwOldProtect, &dwOldProtect);

#ifdef FEATURE_BREWMP_SUPPORT
    {
        IRealloc *piRealloc = NULL;
        IEnv *piEnv = NULL;

        ISHELL_CreateInstance(g_MemHookCtx.pIShell, AEECLSID_Realloc, (void **)&piRealloc);
        HOOK_FUNC(AEEGETPVTBL(piRealloc, IRealloc)->ErrRealloc, ErrRealloc_Hook, &g_MemHookCtx.pCtxEnvRealloc);
        HOOK_FUNC(AEEGETPVTBL(piRealloc, IRealloc)->ErrReallocName, ErrReallocName_Hook, &g_MemHookCtx.pCtxEnvReallocName);
        HOOK_FUNC(AEEGETPVTBL(piRealloc, IRealloc)->ErrReallocNoZI, ErrReallocNoZI_Hook, &g_MemHookCtx.pCtxEnvReallocNoZI);
        HOOK_FUNC(AEEGETPVTBL(piRealloc, IRealloc)->ErrReallocNameNoZI, ErrReallocNameNoZI_Hook, &g_MemHookCtx.pCtxEnvReallocNameNoZI);
        RELEASEIF(piRealloc);

        ISHELL_CreateInstance(g_MemHookCtx.pIShell, AEECLSID_Env, (void **)&piEnv);
        HOOK_FUNC(AEEGETPVTBL(piEnv, IEnv)->CreateInstance, EnvCreateInstance_Hook, &g_MemHookCtx.pCtxEnvCI);
        RELEASEIF(piEnv);
    }
#endif

    g_MemHookCtx.bInstalled = TRUE;
    return TRUE;
}

void MemHook_Uninstall()
{
    DWORD dwOldProtect = 0;

    if (!g_MemHookCtx.bInstalled) {
        return;
    }

    // Restore old pointer
    MemoryProtect(g_MemHookCtx.pIShell->pvt, sizeof(*g_MemHookCtx.pIShell->pvt), PAGE_READWRITE, &dwOldProtect);
    g_MemHookCtx.pIShell->pvt->CreateInstance = g_MemHookCtx.pfnOldCI;
    MemoryProtect(g_MemHookCtx.pIShell->pvt, sizeof(*g_MemHookCtx.pIShell->pvt), dwOldProtect, &dwOldProtect);

    UNHOOK_FUNC(g_MemHookCtx.pCtxMalloc);
    UNHOOK_FUNC(g_MemHookCtx.pCtxFree);
    UNHOOK_FUNC(g_MemHookCtx.pCtxRealloc);

    RELEASEIF(g_MemHookCtx.pIAppletCtl);

    if (g_MemHookCtx.pStackHelper)
    {
        StackHelper_Release(g_MemHookCtx.pStackHelper);
        g_MemHookCtx.pStackHelper = NULL;
    }

    DumpMemLeak(&g_MemHookCtx.mem_block_list);
    MemBlockList_Free(&g_MemHookCtx.mem_block_list);

    g_MemHookCtx.bInstalled = FALSE;
}
