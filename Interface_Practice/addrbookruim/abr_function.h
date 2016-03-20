/******************************************************************************
FILE:  abr_function.h

SERVICES:  Header file

GENERAL DESCRIPTION:
  addrbookruim is address book on ruim card

PUBLIC CLASSES AND STATIC FUNCTIONS:
  Classes:
    IMenuCtl
    ITextCtl

INITIALIZATION & SEQUENCING REQUIREMENTS:

  See Exported Routines

       Copyright © 2008 unicom-brew
               All Rights Reserved.
******************************************************************************/
#ifndef ABR_FUNCTION_H
#define ABR_FUNCTION_H

extern int32 ABR_AddContactRec( CAddrBookRuim *pMe, AEECLSID ContactCLS, AECHAR *pTextName, AECHAR *pTextNum);
extern int32 ABR_GetContactRec( CAddrBookRuim * pMe, AEECLSID ContactCLS, IMenuCtl* pMenu, uint32* pnTotal);
extern uint32 ARB_GetTotalRecNum( CAddrBookRuim * pMe,AEECLSID ContactCLS);
extern int32 ABR_UpdateContactRec( CAddrBookRuim *pMe, AEECLSID ContactCLS, uint16 wID, AECHAR *pTextName, AECHAR *pTextNum);
extern int32 ABR_DeleteContactRec( CAddrBookRuim *pMe, AEECLSID ContactCLS, uint16 wID);
extern int32  ABR_GetContactFieldByItemID(CAddrBookRuim *pMe, AEECLSID ContactCLS, uint16 wID, AECHAR *pTextName, AECHAR *pTextNum);

#endif // ABR_FUNCTION_H
