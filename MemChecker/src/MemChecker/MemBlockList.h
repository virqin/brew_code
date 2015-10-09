/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#ifndef __MEMBLOCKLIST_H__
#define __MEMBLOCKLIST_H__

#include "list.h"
#include "StackHelper.h"

typedef struct CallStackNode
{
    list_node list;
    CallStackEntry csEntry;
} CallStackNode;

static __inline CallStackNode *CallStackNode_New()
{
    CallStackNode *pNode = NULL;

    pNode = (CallStackNode *)malloc(sizeof(CallStackNode));
    memset(pNode, 0, sizeof(*pNode));
    return pNode;
}

static __inline void CallStackNode_Free(CallStackNode *pCSNode)
{
    free(pCSNode);
}

//==============================================================================

typedef struct MemBlockNode
{
    list_node   list;

    void       *pMem;
    int         nSize;

    list_head   CallStackList;

} MemBlockNode;

#define cmpfn_memblock(pos, mem, cmparg) (list_entry(pos, MemBlockNode, list)->pMem == mem)

MemBlockNode   *MemBlockNode_New();
void            MemBlockNode_Free(MemBlockNode *pMemBlockNode);

//==============================================================================

boolean         MemBlockList_RemoveNode(list_head *pMemBlockList, void *pMem);
void            MemBlockList_Free(list_head *pMemBlockList);

#endif // __MEMBLOCKLIST_H__
