/******************************************************************************
FILE:  GMenuCtl.c

SERVICES:  
   Implements a custom control that can be used for displaying monetary
   values adjusted for the two places of the decimal.  

GENERAL DESCRIPTION:
   This is NOT a true BREW control.  It simply uses an interface that makes 
   it behave like one.  The primary difference between this control 
   and true BREW controls is that this control is not derived from IControl.
   Therefore, special handling is occasionally required when using this control.

   This decimal precision of the control can be increased or reduced by 
   changing the value of CGMENUCTL_DECIMAL_PRECISION defined in CGMeuCtl.h.

   The maximum number of digits the control can display can be increased or
   decreased by changing the value of CGMENUCTL_MAX_DIGITS defined in 
   CGMeuCtl.h.

PUBLIC CLASSES AND STATIC FUNCTIONS:
	List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

       Copyright © 2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
******************************************************************************/
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AEE.h"
#include "AEEDisp.h"
#include "AEEStdLib.h"
#include "AEEShell.h"
#include "nmdef.h"

#include "GMenuCtl.h"

/*===========================================================================

FUNCTION: CGMENUCTL_CreateInstance

DESCRIPTION:
	This function creates an instance of the CGMeuCtl and returns its pointer.

PARAMETERS:
	pIShell [in] - A pointer to the application's IShell instance.

DEPENDENCIES:
   None

RETURN VALUE:
   Returns a pointer to the created control or NULL if the control could
   not be created successfully.

SIDE EFFECTS:
  None
===========================================================================*/

extern CGMeuCtl* CGMENUCTL_CreateInstance( IShell* pIShell )
{
  CGMeuCtl* pAmtCtl = MALLOC( sizeof( CGMeuCtl ) );

  if( !pAmtCtl )
    return NULL;
  else
  {
    
  }
  return pAmtCtl;
}

/*===========================================================================

FUNCTION: CGMENUCTL_HandleEvent

DESCRIPTION:
	The event handler for a CGMeuCtl.

PARAMETERS:
	pCtl [in] - A pointer to a CGMeuCtl instance

  eCode [in] - Event code

  wParam [in] - 16-bit event data

  dwParam [in] - 32-bit event data

DEPENDENCIES:
   None

RETURN VALUE:
   TRUE - If the event was handled by the control
   FALSE - if the event was not handled

SIDE EFFECTS:
  None
===========================================================================*/
extern boolean CGMENUCTL_HandleEvent( CGMeuCtl* pCtl, AEEEvent eCode, uint16 wParam, uint32 dwParam )
{
  AEEApplet* pApp = (AEEApplet*) GETAPPINSTANCE();

  // Only handle the event of the control is currently active
  if( pCtl && pCtl->m_bIsActive )
  {
  /*
    switch( eCode )
    {
    case EVT_KEY:
      switch( wParam )
      {
      case AVK_CLR:  // Delete a digit if the Clr button is pressed.
                     // If all the digits have been cleared return FALSE
        if( pCtl->m_nAmount > 0 )
        {
          pCtl->m_nAmount /= 10;
          CGMENUCTL_Redraw( pCtl );
          return TRUE;
        }
        else
          return FALSE;

      // Send an EVT_CTL_TAB event if the left or right buttons are pressed.
      case AVK_RIGHT:
          ISHELL_PostEvent( pApp->m_pIShell, pApp->clsID, EVT_CTL_TAB, CTL_TAB_RIGHT, NULL );
          return TRUE;

      case AVK_LEFT:
          ISHELL_PostEvent( pApp->m_pIShell, pApp->clsID, EVT_CTL_TAB, CTL_TAB_LEFT, NULL );
          return TRUE;
      }
      break;

    // Update the control's display is a digit is pressed.
    case EVT_KEY_PRESS:
      switch( wParam )
      {
      case AVK_0:
      case AVK_1:
      case AVK_2:
      case AVK_3:
      case AVK_4:
      case AVK_5:
      case AVK_6:
      case AVK_7:
      case AVK_8:
      case AVK_9:
        CGMENUCTL_SetValue( pCtl, ( pCtl->m_nAmount * 10 ) + ( wParam - AVK_0 ) );
        CGMENUCTL_Redraw( pCtl );
        return TRUE;
      }
      break;
    }
	*/
  }
  return FALSE;
}

/*===========================================================================

FUNCTION: CGMENUCTL_SetActive

DESCRIPTION:
	Sets the active state of the control.

PARAMETERS:
	pCtl [in] - A pointer to a CGMeuCtl instance

  bActive [in] - A boolean value indicating if the control state is active (TRUE)
    or inactive (FALSE)

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
  None
===========================================================================*/
extern void CGMENUCTL_SetActive( CGMeuCtl* pCtl, boolean bActive )
{
  if( pCtl )
  {
    pCtl->m_bIsActive = bActive;
    CGMENUCTL_Redraw( pCtl );
  }
}

/*===========================================================================

FUNCTION: CGMENUCTL_SetActive

DESCRIPTION:
	Returns the current active state of the control.

PARAMETERS:
	pCtl [in] - A pointer to a CGMeuCtl instance

DEPENDENCIES:
   None

RETURN VALUE:
   TRUE - If the control is currently active
   FALSE - If the control is currently inactive

SIDE EFFECTS:
  None
===========================================================================*/
extern boolean CGMENUCTL_IsActive( CGMeuCtl* pCtl )
{
  if( pCtl )
    return pCtl->m_bIsActive;
  else
    return FALSE;
}

/*===========================================================================

FUNCTION: CGMENUCTL_Release

DESCRIPTION:
	Deletes the control and all associated memory.

PARAMETERS:
	pCtl [in] - A pointer to a CGMeuCtl instance

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void CGMENUCTL_Release( CGMeuCtl* pCtl )
{
  if( pCtl )
  {
    FREEIF( pCtl );
  }
}

/*===========================================================================

FUNCTION: CGMENUCTL_Reset

DESCRIPTION:
	Resets the value of the control to zero.  This function redraws the control.

PARAMETERS:
	pCtl [in] - A pointer to a CGMeuCtl instance

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void CGMENUCTL_Reset( CGMeuCtl* pCtl )
{
  if( pCtl )
  {

  }
}


/*===========================================================================

FUNCTION: CGMENUCTL_Redraw

DESCRIPTION:
	Redraws the control on the device screen.  If the control currently has
  focus, the control is drawn with a border.

PARAMETERS:
	pCtl [in] - A pointer to a CGMeuCtl instance

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  Updates the device screen.
===========================================================================*/
extern void CGMENUCTL_Redraw( CGMeuCtl* pCtl )
{
  AEEApplet* pApp = (AEEApplet*) GETAPPINSTANCE();
  AEERect rRect;

  if( pCtl )
  {
    
  }
}

/*===========================================================================

FUNCTION: CGMENUCTL_SetRect

DESCRIPTION:
	Sets the rectangular coordinates of the control.

PARAMETERS:
	pCtl [in] - A pointer to a CGMeuCtl instance

  pRect [in] - A pointer to the new rectangle coordinates.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void CGMENUCTL_SetRect( CGMeuCtl* pCtl, AEERect* pRect )
{
  if( pCtl && pRect )
  { 
	  pCtl->m_Rect = *pRect;
  }
}

/*===========================================================================

FUNCTION: CGMENUCTL_GetRect

DESCRIPTION:
	Get the rectangular coordinates of the control.

PARAMETERS:
	pCtl [in] - A pointer to a CGMeuCtl instance

	pRect [out] - A pointer to the new rectangle coordinates.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None
===========================================================================*/
extern void CGMENUCTL_GetRect( CGMeuCtl* pCtl, AEERect* pRect )
{
  if( pCtl && pRect )
  { 
	  *pRect = pCtl->m_Rect;
  }
}

