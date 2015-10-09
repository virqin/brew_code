/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "Base.h"
#include "XUtil.h"

uint32 CBase_AddRef(IBase *po)
{
    DECLARE_ME(CBase);

    return ++pMe->nRefs;
}

uint32 CBase_Release(IBase *po)
{
    DECLARE_ME(CBase);
    uint32 nRefs = --pMe->nRefs;

    if (0 == nRefs)
    {
#ifdef AEE_SIMULATOR
        MEMSET(po, -1, sizeof(CBase)); // for debugging
#endif
        FREE(pMe);
    }

    return nRefs;
}

