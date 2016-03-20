#include "sourceUtils.h"

//************************************
// Method:    initStringByID
// FullName:  initStringByID
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: AECHAR * * aString
// Parameter: int16 aStringID
//Example:
/************************************************************************/
/*       AECHAR *queryMsg = NULL;
         initStringByID(&queryMsg,IDS_APP_NAME);        
*/
/************************************************************************/
void initStringByID(AECHAR** aString,int16 aStringID)
{
	int len;
	AECHAR    szText[64];
	IShell*   pIShell = ((AEEApplet*) GETAPPINSTANCE() )->m_pIShell;
	len = ISHELL_LoadResString(pIShell,SOURCE_RES_FILE,aStringID,szText,sizeof(szText));
	*aString = MALLOC((len+1)*sizeof(AECHAR));
	WSTRLCPY (*aString,(const AECHAR*)szText,64);
}

//************************************
// Method:    initImageByID
// FullName:  initImageByID
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: IImage * * pImage
// Parameter: int16 aImageID
//************************************
void initImageByID(IImage** pImage,int16 aImageID)
{
	IShell* pIShell = ((AEEApplet*) GETAPPINSTANCE() )->m_pIShell;
	/*if(DEVICE_WIDTH < 200 && aImageID >= 6000)
		*pImage = ISHELL_LoadResImage(pIShell,SOURCE_RES_FILE,aImageID+1000);
	else*/
		*pImage = ISHELL_LoadResImage(pIShell,SOURCE_RES_FILE,aImageID);
}
