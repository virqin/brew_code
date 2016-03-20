#ifndef QUEUE_UTIL_H
#define QUEUE_UTIL_H

#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "aeestdlib.h"

struct _element_;
typedef struct _element_ _element;

struct _Queue;
typedef struct _Queue Queue;

typedef struct _element_
{
	_element* Before;
	void*     Data;
	_element* After;
}/*_element*/;

typedef struct _Queue
{
	void   (*Add)(Queue* queue,void* data);
	void   (*Remove)(Queue* queue,void* data);
	void   (*RemoveAt)(Queue* queue,int index);
	void*  (*At)(Queue* queue,int index);
	int    (*IndexOf)(Queue* queue,void* data);
	void   (*Release)(Queue* queue);
	void   (*ReleaseAll)(Queue* queue);/*连Data一起释放 慎用*/
	int     Size;
	_element* End;
	_element* Start;

}/*Queue*/;



Queue* Queue_CreateInstance(void);



#endif