/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "MemHook.h"

typedef void (*PFNLoad)(void *pv);

class Loader
{
public:
    Loader(PFNLoad pfnLoad, void *pvLoad, PFNLoad pfnUnload, void *pvUnload);
    ~Loader();

private:
    Loader();
    PFNLoad m_pfnUnload;
    void *m_pvUnload;
};

Loader::Loader(PFNLoad pfnLoad, void *pvLoad, PFNLoad pfnUnload, void *pvUnload)
{
    if (pfnLoad)
    {
        pfnLoad(pvLoad);
    }

    m_pfnUnload = pfnUnload;
    m_pvUnload = pvUnload;
}

Loader::~Loader()
{
    if (m_pfnUnload)
    {
        m_pfnUnload(m_pvUnload);
    }
}

// Loader loader((PFNLoad)MemHook_Install, 0, (PFNLoad)MemHook_Uninstall, 0);

