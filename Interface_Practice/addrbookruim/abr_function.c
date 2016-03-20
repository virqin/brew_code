/*===========================================================================

FILE: abr_function.c
ABR: Address Book on Ruim
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEStdLib.h"
#include "AEEAddrbook.h"
#include "AEEKeysConfig.h"

#include "addrbookruim.h"
#include "addrbookruim.brh"
#include "abr_function.h"

/*===========================================================================

FUNCTION: ABR_AddContactRec

DESCRIPTION:
  This function add a record to address book

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 
  pTextName[in]- name field of record
  pTextNum[in]- telephone number field of record

DEPENDENCIES:
   None

RETURN VALUE:
  AEE_SUCCESS -  IADDRBOOK_CreateRec () invocation was  successful
  other - fail


SIDE EFFECTS:
   None
===========================================================================*/
int32 ABR_AddContactRec( CAddrBookRuim *pMe, AEECLSID ContactCLS, AECHAR *pTextName, AECHAR *pTextNum)
{
    AEEAddrField field[2];   //  fields of a record
    int32 nRet = 0;  // return code
    IAddrBook *pAddrBook;

    // check point
    if(pMe==NULL || pMe->pIShell==NULL || pTextName==NULL || pTextNum==NULL)
    {
      return -1;
    }

    // create instance of ADDRESS BOOK for RUIM
    if(!ISHELL_CreateInstance(pMe->pIShell, ContactCLS, (void **)&pAddrBook))
    {
        IAddrRec *pRec;

        //Num
        field[0].fID = (ContactCLS==AEECLSID_ADDRBOOK_RUIM)?
                            AEE_ADDRFIELD_PHONE_GENERIC:AEE_ADDRFIELD_PHONE_WORK;
        field[0].fType = AEEDB_FT_PHONE;
        field[0].pBuffer= pTextNum;
        field[0].wDataLen = (WSTRLEN(pTextNum)+1)*sizeof(AECHAR);

        //Name
        field[1].fID = AEE_ADDRFIELD_NAME;
        field[1].fType = AEEDB_FT_STRING;
        field[1].pBuffer= pTextName;
        field[1].wDataLen = (WSTRLEN(pTextName)+1)*sizeof(AECHAR);

        // create a record with 2 fields
        pRec = IADDRBOOK_CreateRec(pAddrBook, AEE_ADDR_CAT_NONE, (AEEAddrField*)field, 2);

        if(pRec)
        {
            IADDRREC_Release(pRec);
            nRet=0; // success
        }
        else
        {
            nRet=2; // IADDRBOOK_CreateRec fail;
        }

        IADDRBOOK_Release(pAddrBook);
    }
    else
    {
        nRet=1; // error : ISHELL_CreateInstance fail
    }
    DBGPRINTF("Add contact nRet = %d", nRet);
    return nRet;
}

/*===========================================================================

FUNCTION: ABR_GetContactRec

DESCRIPTION:
  This function get all record from address book and show every record as a manu item

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 
  pMenu[out] - menu show all address book menu  
  pnTotal[out] - all records count of address book

DEPENDENCIES:
   None

RETURN VALUE:
  AEE_SUCCESS -  successfully enumate all record in address book
  other - fail


SIDE EFFECTS:
   None
===========================================================================*/
int32 ABR_GetContactRec( CAddrBookRuim * pMe, AEECLSID ContactCLS, IMenuCtl* pMenu, uint32* pnTotal)
{
    IAddrBook *pb;
    IAddrRec *pR;
    int nRet = 0;
    uint32 nTempTotal;
    boolean bFlgNameGet;
    boolean bFlgNumGet;

    AECHAR *aszNum, *aszName;


    if(pMe==NULL || pMe->pIShell==NULL || pMenu==NULL || pnTotal==NULL)
    {
        return (-1);
    }

    aszNum = (AECHAR *)MALLOC(MAX_CONTACT_NUM_SIZE*sizeof(AECHAR));
    aszName = (AECHAR *)MALLOC(MAX_CONTACT_NAME_SIZE*sizeof(AECHAR));

    
    if(aszNum==NULL ||  aszName==NULL)
    {
        return (-2);
    }

    // Clear any items from the IMenuCtl
    IMENUCTL_Reset( pMenu );
    IMENUCTL_SetRect( pMenu, &pMe->m_rScreenRect );
    IMENUCTL_SetTitle( pMenu, ADDRBOOKRUIM_RES_FILE, IDS_VIEW_MENU, NULL);

    nTempTotal = 0;

    if(!ISHELL_CreateInstance(pMe->pIShell, ContactCLS, (void **)&pb))
    {
        nRet = IADDRBOOK_EnumRecInit(pb,  AEE_ADDR_CAT_NONE, AEE_ADDRFIELD_NONE, NULL, 0);
        if(nRet == AEE_SUCCESS)
        {
            while((pR = IADDRBOOK_EnumNextRec(pb)) != NULL)
            {
                AEEAddrField *ptr = NULL;
                int i;
                int nFields;

                bFlgNameGet = FALSE;
                bFlgNumGet = FALSE;
                MEMSET( aszName, 0, MAX_CONTACT_NAME_SIZE*sizeof(AECHAR));
                MEMSET( aszNum,  0, MAX_CONTACT_NUM_SIZE*sizeof(AECHAR));
                nFields= IADDRREC_GetFieldCount(pR);
                for(i = 0; i < nFields; i++)
                {
                    ptr = IADDRREC_GetField(pR,i);
                    DBGPRINTF("lintao fID   =%d", ptr->fID);
                    // find name field and save it;
                    if (   ptr->fID == AEE_ADDRFIELD_NAME  
                       ||ptr->fID == AEE_ADDRFIELD_FIRSTNAME
                       ||ptr->fID == AEE_ADDRFIELD_LASTNAME
                       ||ptr->fID == AEE_ADDRFIELD_NICKNAME)
                    { 
                        if(bFlgNameGet==FALSE)
                        {
                            bFlgNameGet = TRUE;
                            WSTRCPY(aszName, ptr->pBuffer);
                        }
                    }

                    // find number field and save it;
                    if (    ptr->fID >= AEE_ADDRFIELD_PHONE_WORK
                        && ptr->fID <= AEE_ADDRFIELD_PHONE_OTHER)
                    { 
                        if(bFlgNumGet==FALSE)
                        {
                            bFlgNumGet = TRUE;
                            WSTRCPY(aszNum, ptr->pBuffer);
                        }
                    }

                }

                if(bFlgNameGet && bFlgNumGet) 
                {
                    nTempTotal++;
                    IMENUCTL_AddItem(pMenu, 
                                     NULL, 
                                     0,  
                                     ABR_REC_LIST_ID + IADDRREC_GetRecID(pR), 
                                     aszName, 
                                     NULL);
                }
            }                  
        }

        IADDRBOOK_Release(pb);
    }
    else
    {
        nRet=1; // error : ISHELL_CreateInstance fail
    }

    *pnTotal = nTempTotal;
    IMENUCTL_SetActive( pMenu, TRUE );

    FREEIF(aszNum);
    FREEIF(aszName);
    return nRet;
}


/*===========================================================================

FUNCTION: ABR_UpdateContactRec

DESCRIPTION:
  This function update record whose id is wID

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 
  wID[in] - record id which is want to be update
  pTextName[in] - name field which record will change to
  pTextNum[in] - number field which record will change to

DEPENDENCIES:
   None

RETURN VALUE:
  AEE_SUCCESS -  IADDRREC_UpdateAllFields operate successfully
  other - fail


SIDE EFFECTS:
   None
===========================================================================*/
int32 ABR_UpdateContactRec( CAddrBookRuim *pMe, AEECLSID ContactCLS, uint16 wID, AECHAR *pTextName, AECHAR *pTextNum)
{
    int nRet = 0;
    IAddrBook *pb;
    IAddrRec *pR;
    AEEAddrField field;
    boolean bFlgNameGet;
    boolean bFlgNumGet;
    
    if(pMe==NULL || pMe->pIShell==NULL || pTextName==NULL || pTextNum==NULL)
    {
        return (-1);
    }

    if(!ISHELL_CreateInstance(pMe->pIShell, ContactCLS, (void **)&pb))
    {
        pR =IADDRBOOK_GetRecByID(pb, wID);
        if(pR)
        {
                int nFields;
                int i;
                AEEAddrField *ptr = NULL;
                bFlgNameGet = FALSE;
                bFlgNumGet = FALSE;
                nFields= IADDRREC_GetFieldCount(pR);
                for(i = 0; i < nFields; i++)
                {
                    ptr = IADDRREC_GetField(pR,i);
                    DBGPRINTF("lintao fID   =%d", ptr->fID);
                    DBGPRINTF("lintao fType=%d", ptr->fType);

                    // find name field and save it;
                    if (   ptr->fID == AEE_ADDRFIELD_NAME  
                       ||ptr->fID == AEE_ADDRFIELD_FIRSTNAME
                       ||ptr->fID == AEE_ADDRFIELD_LASTNAME
                       ||ptr->fID == AEE_ADDRFIELD_NICKNAME)
                    { 
                        if(bFlgNameGet==FALSE)
                        {
                            bFlgNameGet = TRUE;
                            field.fID = ptr->fID;
                            field.fType = ptr->fType;
                            field.pBuffer = pTextName;
                            field.wDataLen = (WSTRLEN(pTextName)+1)*sizeof(AECHAR);
                            IADDRREC_UpdateField(pR, i, &field);
                        }
                    }

                    // find number field and save it;
                    if (    ptr->fID >= AEE_ADDRFIELD_PHONE_WORK  
                        && ptr->fID <= AEE_ADDRFIELD_PHONE_OTHER)
                    { 
                        if(bFlgNumGet==FALSE)
                        {
                            bFlgNumGet = TRUE;
                            field.fID = ptr->fID;
                            field.fType = ptr->fType;
                            field.pBuffer = pTextNum;
                            field.wDataLen = (WSTRLEN(pTextNum)+1)*sizeof(AECHAR);
                            IADDRREC_UpdateField(pR, i, &field);
                        }
                    }
                }

                // 姓名和号码均有
                if(bFlgNameGet && bFlgNumGet) 
                {
                    nRet=0;
                }
                else
                {
                    nRet=4;
                }
        }
        else
        {
            nRet=2;  // IADDRBOOK_GetRecByID fail
        }

        IADDRBOOK_Release(pb);
    }
    else
    {
        nRet = 3;
    }
    DBGPRINTF("nRet=%d", nRet);
    return nRet;
}

/*===========================================================================

FUNCTION: ABR_DeleteContactRec

DESCRIPTION:
  This function delete record whose id is wID

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 
  wID[in] - record id which is want to be deleted

DEPENDENCIES:
   None

RETURN VALUE:
  AEE_SUCCESS -  IADDRREC_RemoveRec operate successfully
  other - fail


SIDE EFFECTS:
   None
===========================================================================*/
int32 ABR_DeleteContactRec( CAddrBookRuim *pMe, AEECLSID ContactCLS, uint16 wID)
{
    int nRet = 0;
    IAddrBook *pb;
    IAddrRec *pR;
    
    if ( pMe==NULL || pMe->pIShell==NULL )
    {
        return (-1);
    }

    if(!ISHELL_CreateInstance(pMe->pIShell, ContactCLS, (void **)&pb))
    {
        pR = IADDRBOOK_GetRecByID(pb, wID);
        if(pR)
        {
            nRet = IADDRREC_RemoveRec(pR);
        }
        else
        {
            nRet = 2;  // IADDRBOOK_GetRecByID fail
        }
        IADDRBOOK_Release(pb);
    }
    else
    {
        nRet = 1;
    }

    return nRet;
}

/*===========================================================================

FUNCTION: ARB_GetTotalRecNum

DESCRIPTION:
  This function get all record count in address book

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 

DEPENDENCIES:
   None

RETURN VALUE:
  records count in address book

SIDE EFFECTS:
   None
===========================================================================*/
uint32 ARB_GetTotalRecNum( CAddrBookRuim * pMe, AEECLSID ContactCLS )
{
    IAddrBook *pb;
    int nCount=0;
    
    if(pMe==NULL||pMe->pIShell==NULL)
    {
        return 0;
    }
    DBGPRINTF("contact cls id=0x%x",ContactCLS);
    if(!ISHELL_CreateInstance(pMe->pIShell, ContactCLS, (void **)&pb))
    {
        nCount = IADDRBOOK_GetNumRecs(pb);    
        IADDRBOOK_Release(pb);
    }

    return nCount;
}

/*===========================================================================

FUNCTION: ABR_GetContactFieldByItemID

DESCRIPTION:
  This function get all fields of record whose id is wID

PARAMETERS:
  pMe [in] - Pointer to the CAddrBookRuim structure. This structure contains 
    information specific to this applet. 
  wID[in] - record id to be query
  pTextName[out] - name field 
  pTextNum[out] - number field

DEPENDENCIES:
   None

RETURN VALUE:
  AEE_SUCCESS -  IADDRREC_UpdateAllFields operate successfully
  other - fail

SIDE EFFECTS:
   None
===========================================================================*/
int32  ABR_GetContactFieldByItemID(CAddrBookRuim *pMe, AEECLSID ContactCLS, uint16 wID, AECHAR *pTextName, AECHAR *pTextNum)
{
    int nRet = 0;
    IAddrBook *pb;
    IAddrRec *pR;
    boolean bFlgNameGet;
    boolean bFlgNumGet;
    
    if(pMe==NULL || pMe->pIShell==NULL || pTextName==NULL || pTextNum==NULL)
    {
        return (-1);
    }

    if(!ISHELL_CreateInstance(pMe->pIShell, ContactCLS, (void **)&pb))
    {
        pR =IADDRBOOK_GetRecByID(pb, wID);
        if(pR)
        {
            AEEAddrField *ptr = NULL;
            int i;
            int nFields;

            bFlgNameGet = FALSE;
            bFlgNumGet = FALSE;
            nFields= IADDRREC_GetFieldCount(pR);
            for(i = 0; i < nFields; i++)
            {
               ptr = IADDRREC_GetField(pR,i);
               // find name field and save it;
               if (   ptr->fID == AEE_ADDRFIELD_NAME  
                   ||ptr->fID == AEE_ADDRFIELD_FIRSTNAME
                   ||ptr->fID == AEE_ADDRFIELD_LASTNAME
                   ||ptr->fID == AEE_ADDRFIELD_NICKNAME)
               { 
                  if(bFlgNameGet==FALSE)
                   {
                      bFlgNameGet = TRUE;
                      WSTRCPY(pTextName, ptr->pBuffer);
                   }
               }

               // find number field and save it;
               if (    ptr->fID >= AEE_ADDRFIELD_PHONE_WORK  
                  && ptr->fID <= AEE_ADDRFIELD_PHONE_OTHER)
               { 
                  if(bFlgNumGet==FALSE)
                   {
                      bFlgNumGet = TRUE;
                      WSTRCPY(pTextNum, ptr->pBuffer);
                   }
               }

            }

            if(bFlgNameGet && bFlgNumGet) 
            {
                nRet=0;
            }
            else
            {
                nRet=3;
            }
        }
        else
        {
            nRet=2;  // IADDRBOOK_GetRecByID fail
        }

        IADDRBOOK_Release(pb);
    }
    else
    {
        nRet = 1; // fail ISHELL_CreateInstance
    }

    return nRet;
}

