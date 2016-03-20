/**
 *	file: framework.h
 *	author: yuleiming
 */
#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include "AEEModGen.h"
#include "AEEShell.h"
#include "AEEWEB.H"
#include "AEEFile.h"
#include "AEEStdlib.h"
#include "AEEThread.h"

#include "bff/bff.h"
#include "bff/bff-accessors.h"
#include "bff/bff-applet.h"
#include "bff/bff-applet-driver.h"
#include "bff/bff-color.h"
#include "bff/bff-handlers.h"
#include "bff/bff-logging.h"
#include "bff/bff-mem.h"
#include "bff/bff-scene.h"
#include "bff/bff-scene-driver.h"
#include "bff/bff-task.h"
#include "bff/bff-task-driver.h"
#include "bff/bff-version-brew.h"

#if BFF_BREW_VERSION_LT(3,1,5,0)
typedef void CallbackNotifyFunc(void* pData);
#endif

#endif /*FRAMEWORK_H*/