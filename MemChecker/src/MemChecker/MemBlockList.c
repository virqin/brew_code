/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "MemBlockList.h"

MemBlockNode *MemBlockNode_New()
{
    MemBlockNode *pNode = malloc(sizeof(MemBlockNode));
    memset(pNode, 0, sizeof(*pNode));
    INIT_LIST_HEAD(&pNode->CallStackList);

    return pNode;
}

void MemBlockNode_Free(MemBlockNode *pMemBlockNode)
{
    list_node *posCSNode = NULL;
    list_node *posCSNodeTemp = NULL;

    list_for_each_safe(posCSNode, posCSNodeTemp, &pMemBlockNode->CallStackList)
    {
        CallStackNode *pCSNode = NULL;

        list_del(posCSNode);
        pCSNode = list_entry(posCSNode, CallStackNode, list);
        CallStackNode_Free(pCSNode);
    }

    free(pMemBlockNode);
}

//==============================================================================

boolean MemBlockList_RemoveNode(list_head *pMemBlockList, void *pMem)
{
    list_node *pos = NULL;

    list_find(pMemBlockList, pMem, cmpfn_memblock, 0, pos);
    if (NULL != pos)
    {
        list_del(pos);
        MemBlockNode_Free(list_entry(pos, MemBlockNode, list));
        return TRUE;
    }

    return FALSE;
}

void MemBlockList_Free(list_head *pMemBlockList)
{
    list_node *posMemBlockNode = NULL;
    list_node *posMemBlockNodeTemp = NULL;

    list_for_each_safe(posMemBlockNode, posMemBlockNodeTemp, pMemBlockList)
    {
        MemBlockNode *pMemBlockNode = NULL;

        list_del(posMemBlockNode);
        pMemBlockNode = list_entry(posMemBlockNode, MemBlockNode, list);
        MemBlockNode_Free(pMemBlockNode);
        pMemBlockNode = NULL;
    }
}

