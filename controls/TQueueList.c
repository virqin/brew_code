#include <AEEStdLib.h>

#include "TQueueList.h"


boolean TQueueList_Init(TQueueList **pTQueueList)
{
	TQueueList *pHead;

	pHead = *pTQueueList;
	if (pHead != NULL)
	{
		FREE(pHead);
		pHead = NULL;
	}

// 	pHead = MALLOC(sizeof(TQueueList));
// 	if (pHead == NULL)
// 	{
// 		DBGPRINTF("malloc failed!");
// 		return FALSE;
// 	}
// 
// 	MEMSET(pHead, 0, sizeof(TQueueList));
// 
// 	*pTQueueList = pHead;

    DBGPRINTF("Init Success!");

	return TRUE;
}

void TQueueList_Destroy(TQueueList **pTQueueList)
{
	TQueueList *pHead;
	TQueueList *pTmp;
    TQueueList *ppNext;            //����һ����pHead���ڽڵ�

	pHead = pTmp = *pTQueueList;

    if(pHead == NULL)
    {
        DBGPRINTF("clearList����ִ�У�����Ϊ��\n");
        return;
    }

    while(pHead->pNext != NULL)
    {
		ppNext = pHead->pNext;
		
		if (pHead->pData)
		{
			FREE(pHead->pData);
			pHead->pData = NULL;
		}

        FREE(pHead);
        pHead = ppNext;
    }

	if (pHead->pData)
	{
		FREE(pHead->pData);
		pHead->pData = NULL;
	}

	//FREE(pTmp);
	//pTmp = NULL;

	*pTQueueList = NULL;

    DBGPRINTF("TQueueList_Destroy Over!");
}

int TQueueList_GetCount(TQueueList *pHead)
{
	int size = 0;

    while(pHead != NULL)
    {
        size ++;         //��������size��С�������ʵ�ʳ���С1
        pHead = pHead->pNext;
    }

    DBGPRINTF("List Count:%d", size);

    return size;    //�����ʵ�ʳ���
}


int TQueueList_Insert(TQueueList **pTQueueList, elemType elem)
{
    TQueueList *pInsert;
    TQueueList *pHead;
    TQueueList *pTmp; 
	int		pos = 0;

    pHead = *pTQueueList;
    pTmp = pHead;

    pInsert = (TQueueList *)MALLOC(sizeof(TQueueList)); 
    MEMSET(pInsert,0,sizeof(TQueueList));
	pInsert->pData = elem;

	if (pHead == NULL)
	{
		pHead = pInsert;
		*pTQueueList = pHead;
		return 0;
	}

	pos ++;

    while(pHead->pNext != NULL)
    {
        pHead = pHead->pNext;
    }

    pHead->pNext = pInsert;
    *pTQueueList = pTmp;

    DBGPRINTF("insert with pos :%d", pos);

    return pos;
}

elemType TQueueList_GetElem(TQueueList *pHead, uint32 pos)
{
	uint32 i = 0;

    if(pHead == NULL)
    {
        DBGPRINTF("LIST IS EMPTY");
        return NULL;
    }

    while(pHead !=NULL)
    {
        if(i == pos)
            break;

		i ++;
        pHead = pHead->pNext;
    }

    if(i < pos)
    {
        DBGPRINTF("NOT FOUND");
        return NULL;
    } 

    return pHead->pData;
}

void TQueueList_Print(TQueueList *pHead)
{
    if(NULL == pHead)
    {
        DBGPRINTF("LIST IS NULL!");
    }
    else
    {
        while(NULL != pHead)
        {
            DBGPRINTF("%x ",pHead->pData);
            pHead = pHead->pNext;
        }
    }
}

// 
// /* 2.�������Ա��˺������븺����ֹ��ȡ����*/
// TQueueList *creatList(TQueueList *pHead)
// {
//     TQueueList *p1;
//     TQueueList *p2;
// 
//     p1=p2=(TQueueList *)malloc(sizeof(TQueueList)); //�����½ڵ�
// 
//     if(p1 == NULL || p2 ==NULL)
//     {
//         DBGPRINTF("�ڴ����ʧ��\n");
// 		return NULL;
//     }
// 
//     memset(p1,0,sizeof(TQueueList));
// 
//     scanf("%d",&p1->pData);    //�����½ڵ�
// 
//     p1->pNext = NULL;         //�½ڵ��ָ����Ϊ��
// 
//     while(p1->pData > 0)        //�����ֵ����0�������ֱ�������ֵΪ��
//     {
//         if(pHead == NULL)       //�ձ������ͷ
//         {
//             pHead = p1;
//         }
//         else               
//         {
//             p2->pNext = p1;       //�ǿձ������β
//         }
// 
//         p2 = p1;
//         p1=(TQueueList *)malloc(sizeof(TQueueList));    //��������һ���ڵ�
//         if(p1 == NULL || p2 ==NULL)
//         {
// 			DBGPRINTF("�ڴ����ʧ��\n");
// 			return NULL;
//         }
//         memset(p1,0,sizeof(TQueueList));
//         scanf("%d",&p1->pData);
//         p1->pNext = NULL;
// 
//     }
// 
//     DBGPRINTF("creatList����ִ�У��������ɹ�\n");
// 
//     return pHead;           //���������ͷָ��
// }
// 
//  
// 
// /* 3.��ӡ��������ı���*/
// void printList(TQueueList *pHead)
// {
//     if(NULL == pHead)   //����Ϊ��
//     {
//         DBGPRINTF("PrintList����ִ�У�����Ϊ��\n");
//     }
//     else
//     {
//         while(NULL != pHead)
//         {
//             DBGPRINTF("%d ",pHead->pData);
//             pHead = pHead->pNext;
//         }
//         DBGPRINTF("\n");
//     }
// 
// }
// 
//  
// 
// /* 4.������Ա�L�е�����Ԫ�أ����ͷŵ�����L�����еĽ�㣬ʹ֮��Ϊһ���ձ� */
// void clearList(TQueueList *pHead)
// {
//     TQueueList *ppNext;            //����һ����pHead���ڽڵ�
// 
//     if(pHead == NULL)
//     {
//         DBGPRINTF("clearList����ִ�У�����Ϊ��\n");
//         return;
//     }
// 
//     while(pHead->pNext != NULL)
//     {
//         ppNext = pHead->pNext;//������һ����ָ��
//         free(pHead);
//         pHead = ppNext;      //��ͷ����
//     }
// 
//     DBGPRINTF("clearList����ִ�У������Ѿ����\n");
// }
// 
//  
// 
// /* 5.���ص�����ĳ��� */
// int sizeList(TQueueList *pHead)
// {
// 	int size = 0;
// 
//     while(pHead != NULL)
//     {
//         size++;         //��������size��С�������ʵ�ʳ���С1
//         pHead = pHead->pNext;
//     }
// 
//     DBGPRINTF("sizeList����ִ�У������� %d \n",size);
// 
//     return size;    //�����ʵ�ʳ���
// }
// 
// /* 6.��鵥�����Ƿ�Ϊ�գ���Ϊ���򷵻أ������򷵻أ� */
// int isEmptyList(TQueueList *pHead)
// {
//     if(pHead == NULL)
//     {
//         DBGPRINTF("isEmptyList����ִ�У�����Ϊ��\n");
//         return 1;
//     }
// 
// 	DBGPRINTF("isEmptyList����ִ�У�����ǿ�\n");
// 
//     return 0;
// }
// 
// /* 7.���ص������е�pos������е�Ԫ�أ���pos������Χ����ֹͣ�������� */
// elemType getpData(TQueueList *pHead, int pos)
// {
//     int i=0;
// 
// 	if(pos < 1)
//     {
//         DBGPRINTF("getpData����ִ�У�posֵ�Ƿ�\n");
//         return 0;
//     }
// 
//     if(pHead == NULL)
//     {
//         DBGPRINTF("getpData����ִ�У�����Ϊ��\n");
//         return 0;
//         //exit(1);
//     }
// 
//     while(pHead !=NULL)
//     {
//         ++i;
//         if(i == pos)
//         {
//             break;
//         }
// 
//         pHead = pHead->pNext; //�Ƶ���һ���
//     }
// 
//     if(i < pos)                  //�����Ȳ������˳�
//     {
//         DBGPRINTF("getpData����ִ�У�posֵ����������\n");
//         return 0;
//     } 
// 
//     return pHead->pData;
// }
// 
//  
// 
// /* 8.�ӵ������в��Ҿ��и���ֵx�ĵ�һ��Ԫ�أ������ҳɹ��򷵻ظý��pData��Ĵ洢��ַ�����򷵻�NULL */
// elemType *getElemAddr(TQueueList *pHead, elemType x)
// {
//     if(NULL == pHead)
//     {
//         DBGPRINTF("getElemAddr����ִ�У�����Ϊ��\n");
//         return NULL;
//     }
// 
//     if(x < 0)
//     {
//         DBGPRINTF("getElemAddr����ִ�У�����ֵX���Ϸ�\n");
//         return NULL;
//     }
// 
//     while((pHead->pData != x) && (NULL != pHead->pNext)) //�ж��Ƿ�����ĩβ���Լ��Ƿ������Ҫ�ҵ�Ԫ��
//     {
//         pHead = pHead->pNext;
//     }
// 
//     if((pHead->pData != x) && (pHead != NULL))
//     {
//         DBGPRINTF("getElemAddr����ִ�У���������δ�ҵ�xֵ\n");
//         return NULL;
//     }
// 
//     if(pHead->pData == x)
//     {
//         DBGPRINTF("getElemAddr����ִ�У�Ԫ�� %d �ĵ�ַΪ 0x%x\n",x,&(pHead->pData));
//     }
// 
//     return &(pHead->pData);//����Ԫ�صĵ�ַ
// }
// 
//  
// 
// /* 9.�ѵ������е�pos������ֵ�޸�Ϊx��ֵ�����޸ĳɹ����أ������򷵻أ� */
// int modifyElem(TQueueList *pTQueueList,int pos,elemType x)
// {
//     TQueueList *pHead;
//     pHead = pTQueueList;
//     int i = 0; 
// 
//     if(NULL == pHead)
//     {
//         DBGPRINTF("modifyElem����ִ�У�����Ϊ��\n");
//     }
// 
//     if(pos < 1)
//     {
//         DBGPRINTF("modifyElem����ִ�У�posֵ�Ƿ�\n");
//         return 0;
//     }
// 
//     while(pHead !=NULL)
//     {
//         ++i;
//         if(i == pos)
//             break;
// 
//         pHead = pHead->pNext; //�Ƶ���һ���
// 
//     }
// 
//     if(i < pos)                  //�����Ȳ������˳�
//     {
//         DBGPRINTF("modifyElem����ִ�У�posֵ����������\n");
//         return 0;
//     }
// 
//     pTQueueList = pHead;
//     pTQueueList->pData = x;
//     DBGPRINTF("modifyElem����ִ��\n");    
// 
//     return 1;
// 
// }
// 
//  
// 
// /* 10.������ı�ͷ����һ��Ԫ�� */
// int insertHeadList(TQueueList **pTQueueList,elemType insertElem)
// {
//     TQueueList *pInsert;
// 
//     pInsert = (TQueueList *)malloc(sizeof(TQueueList));
//     memset(pInsert,0,sizeof(TQueueList));
//     pInsert->pData = insertElem;
//     pInsert->pNext = *pTQueueList;
//     *pTQueueList = pInsert;
//     DBGPRINTF("insertHeadList����ִ�У����ͷ����Ԫ�سɹ�\n");
// 	
// 	return 1;
// }


/* 12.�������е�pos�����λ�ò���Ԫ��Ϊx�Ľ�㣬������ɹ����أ������򷵻أ� */
/* 13.�����������в���Ԫ��x��㣬ʹ�ò������Ȼ���� */
/* 14.�ӵ�������ɾ����ͷ��㣬���Ѹý���ֵ���أ���ɾ��ʧ����ֹͣ�������� */
/* 15.�ӵ�������ɾ����β��㲢��������ֵ����ɾ��ʧ����ֹͣ�������� */
/* 16.�ӵ�������ɾ����pos����㲢��������ֵ����ɾ��ʧ����ֹͣ�������� */
/* 17.�ӵ�������ɾ��ֵΪx�ĵ�һ����㣬��ɾ���ɹ��򷵻�1,���򷵻�0 */
/* 18.����2��Ԫ�ص�λ�� */
/* 19.�����Ա���п������� */

/******************************************************************/
/*
int main()
{
    TQueueList *pList=NULL;
    int length = 0;
    elemType posElem;
    initList(&pList);       //�����ʼ��

    printList(pList);       //����������ӡ����
    pList=creatList(pList); //��������
    printList(pList);
    sizeList(pList);        //����ĳ���
    printList(pList);
    isEmptyList(pList);     //�ж������Ƿ�Ϊ������
    posElem = getpData(pList,3);  //��ȡ������Ԫ�أ����Ԫ�ز���3�����򷵻�0
    DBGPRINTF("getpData����ִ�У�λ�� 3 �е�Ԫ��Ϊ %d\n",posElem);   
    printList(pList);
    getElemAddr(pList,5);   //���Ԫ��5�ĵ�ַ

    modifyElem(pList,4,1);  //��������λ��4�ϵ�Ԫ���޸�Ϊ1
    printList(pList);

    insertHeadList(&pList,5);   //��ͷ����Ԫ��12
    printList(pList);

    insertLastList(&pList,10);  //��β����Ԫ��10
    printList(pList);
    clearList(pList);       //�������
	system("pause");
}*/