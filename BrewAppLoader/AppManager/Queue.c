#include "Queue.h"


void   queue_add(Queue* queue,void* data);
void   queue_remove(Queue* queue,void* data);
void   queue_remove_at(Queue* queue,int index);
void*  queue_at(Queue* queue,int index);
void   queue_release(Queue* queue);
void   queue_release_all(Queue* queue);
int    queue_index_of(Queue* queue,void* data);


Queue* Queue_CreateInstance(void)
{
	Queue* queue = (Queue*)MALLOC(sizeof(Queue));
	queue->Add      = queue_add;
	queue->Remove   = queue_remove;
	queue->RemoveAt = queue_remove_at;
 	queue->At       = queue_at;
	queue->IndexOf  = queue_index_of;
	queue->Release  = queue_release;
	queue->ReleaseAll  = queue_release_all;

	queue->Size = 0;
	queue->Start = NULL;
	queue->End = NULL;
	return queue;
}
void   queue_add(Queue* queue,void* data)
{
	_element* elem = NULL;
	if (queue == NULL || data == NULL)
		return;
	elem = (_element*)MALLOC(sizeof(_element));
	elem->Data = NULL;
	elem->After= NULL;
	elem->Before = NULL;

	elem->Data = data;
	if (queue->Size == 0)
	{
		queue->Start = elem;
		queue->End = elem;
	}
	else
	{
		elem->Before = queue->End;
		queue->End->After = elem;
		queue->End = elem;
	}
	queue->Size++;
}
void   queue_remove(Queue* queue,void* data)
{
	_element* elem = NULL;
	if (queue == NULL || data == NULL)
		return;
	elem = queue->Start;
	while(elem != NULL)
	{
		if (elem->Data == data)
		{
			_element* before = elem->Before;
			_element* after  = elem->After;
			if (before != NULL)
			{
				before->After =  after;
			}
			else
			{
				queue->Start = after;
			}
			if (after != NULL)
			{
				after->Before = before;
			}
			else
			{
				queue->End = before;
			}
			FREEIF(elem);
			queue->Size --;
			return;
		}
		else
		{
			elem = elem->After;
		}
	}
}

int    queue_index_of(Queue* queue,void* data)
{
	_element* elem = NULL;
	int index = 0;
	if (queue == NULL || data == NULL)
		return -1;
	elem = queue->Start;
	while(elem != NULL)
	{
		if (elem->Data == data)
		{
			return index;
		}
		else
		{
			elem = elem->After;
			index++;
		}
	}
	return -1;
}

void*   queue_at(Queue* queue,int index)
{
	_element* elem  = NULL;
	int i;
	if (queue == NULL || index < 0 || index >= queue->Size)
		return NULL;
	elem = queue->Start;
	for(i=0;i<index;i++)
	{
		elem = elem->After;
	}
	return elem->Data;
}


void   queue_remove_at(Queue* queue,int index)
{
	_element* elem = NULL;
	int i;
	_element* before = NULL;
	_element* after = NULL;
	if (queue == NULL || index < 0 || index >= queue->Size)
		return;
	elem = queue->Start;
	for(i=0;i<index;i++)
	{
		elem = elem->After;
	}

	before = elem->Before;
	after  = elem->After;

	if (before != NULL)
	{
		before->After =  after;
	}
	else
	{
		queue->Start = after;
	}
	if (after != NULL)
	{
		after->Before = before;
	}
	else
	{
		queue->End = before;
	}
	FREEIF(elem);
	queue->Size --;
	return;
}

void   queue_release(Queue* queue)
{
	_element* elem = NULL;
	if (queue == NULL)
		return;
	elem = queue->Start;
	while(elem != NULL)
	{
		_element* temp = elem;
		elem = elem->After;
		FREEIF(temp);
	}
	FREEIF(queue);
}

void   queue_release_all(Queue* queue)
{
	_element* elem = NULL;
	_element* before = NULL;
	if (queue == NULL)
		return;
	elem = queue->Start;
	while(elem != NULL)
	{
		elem = elem->After;
		before = elem->Before;
		FREEIF(before->Data);
		FREEIF(before);
		elem->Before = NULL;
	}
	FREEIF(queue);
}
