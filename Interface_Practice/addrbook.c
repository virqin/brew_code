/*===========================================================================  
02.  
03.FILE: addrbook.c  
04.  
05.SERVICES: Sample applet illustrating use of BREW Address Book Interface  
06.  
07.DESCRIPTION  
08.  This file contains usage examples of IAddrBook and IAddrRec interfaces in BREW  
09.  
10.PUBLIC CLASSES:    
11.   N/A  
12.  
13.  
14.       Copyright ? 2000-2001 QUALCOMM Incorporated.  
15.                      All Rights Reserved.  
16.                   QUALCOMM Proprietary/GTDR  
17.===========================================================================*/   
18.   
19./*===============================================================================  
20.                     INCLUDES AND VARIABLE DEFINITIONS  
21.=============================================================================== */   
22.#include "AEEModGen.h"   
23.#include "AEEAppGen.h"   
24.#include "AEEMenu.h"   
25.#include "AEEStdLib.h"   
26.#include "AEEAddrBook.h"   
27.#include "addrbook.bid"   
28.   
29./*===========================================================================  
30.  
31.                      PUBLIC DATA DECLARATIONS  
32.  
33.===========================================================================*/   
34.   
35./*-------------------------------------------------------------------  
36.            Type Declarations  
37.-------------------------------------------------------------------*/   
38.// AddrBook app struct. This is the main struct for this applet. This will hold    
39.// all the data members that needs to be remembered throughout the life of   
40.// the applet.    
41.// THE FIRST DATA MEMBER OF THIS STRUCT MUST BE AN AEEApplet OBJECT.    
42.// This is critical requirement which is related to an applet being   
43.// dynamically loaded. The app developers should add their data members   
44.// following the AEEApplet data member.   
45.typedef struct _CAddrBkApp   
46.{   
47.    AEEApplet   a;        // Mandatory first AEEApplet data member   
48.   IMenuCtl *  m_pIMenu;   
49.   
50.   // Device parameters   
51.   int             m_nLineHeight;   
52.   AEEDeviceInfo   m_dInfo;   
53.   
54.}CAddrBkApp;   
55.   
56./*-------------------------------------------------------------------  
57.            Function Prototypes  
58.-------------------------------------------------------------------*/   
59.// App Handle Event function   
60.static boolean AddrBkApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);   
61.   
62.// App specific data alloc-init/free functions   
63.static boolean AddrBk_InitAppData(IApplet* pMe);   
64.static void AddrBk_FreeAppData(IApplet* pMe);   
65.   
66.static void BuildMainMenu(CAddrBkApp *pMe);   
67.static boolean AddrBkUsage (CAddrBkApp * pMe, uint16 wParam);   
68.static void DisplayEvent (CAddrBkApp *pMe, uint16 wParam);   
69.static void DisplayOutput(CAddrBkApp * pMe, int nline, char *pszStr);   
70.   
71.static int  AddrBkTest(CAddrBkApp *pme);   
72.   
73./*-------------------------------------------------------------------  
74.            Global Constant Definitions  
75.-------------------------------------------------------------------*/   
76.// App Resource File   
77.#define  APP_RES_FILE      "addrbkuse.bar"   
78.   
79.// App specific constants   
80.#define ADDRBK_CREATE_REC           101   
81.#define ADDRBK_ADD_FIELD            102   
82.#define ADDRBK_REMOVE_FIELD         103   
83.#define ADDRBK_UPDATE_FIELD         104   
84.#define ADDRBK_UPDATE_ALL_FIELDS    105   
85.#define ADDRBK_REMOVE_REC           106    
86.#define ADDRBK_GET_FIELD_COUNT      107   
87.#define ADDRBK_COUNT_RECS           108   
88.#define ADDRBK_ENUM_CATEGORIES      109   
89.#define ADDRBK_ENUM_FIELDS          110   
90.   
91.   
92.   
93.   
94.   
95./*===============================================================================  
96.                     FUNCTION DEFINITIONS  
97.=============================================================================== */   
98.   
99./*===========================================================================  
100.  
101.FUNCTION: AEEClsCreateInstance  
102.  
103.DESCRIPTION  
104.    This function is invoked while the app is being loaded. All Modules must provide this   
105.    function. Ensure to retain the same name and parameters for this function.  
106.    In here, the module must verify the ClassID and then invoke the AEEApplet_New() function  
107.    that has been provided in AEEAppGen.c.   
108.  
109.   After invoking AEEApplet_New(), this function can do app specific initialization. In this  
110.   example, a generic structure is provided so that app developers need not change app specific  
111.   initialization section every time except for a call to IDB_InitAppData().   
112.   This is done as follows: InitAppData() is called to initialize AppletData   
113.   instance. It is app developers responsibility to fill-in app data initialization   
114.   code of InitAppData(). App developer is also responsible to release memory   
115.   allocated for data contained in AppletData -- this can be done in   
116.   IDB_FreeAppData().  
117.  
118.PROTOTYPE:  
119.   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)  
120.  
121.PARAMETERS:  
122.    clsID: [in]: Specifies the ClassID of the applet which is being loaded  
123.  
124.    pIShell: [in]: Contains pointer to the IShell interface.   
125.  
126.    pIModule: pin]: Contains pointer to the IModule interface to the current module to which  
127.    this app belongs  
128.  
129.    ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation  
130.    of memory for this structure and initializing the base data members is done by AEEApplet_New().  
131.  
132.DEPENDENCIES  
133.  none  
134.  
135.RETURN VALUE  
136.  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was  
137.     successful  
138.  EFAILED: If the app does not need to be loaded or if errors occurred in   
139.     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.  
140.  
141.SIDE EFFECTS  
142.  none  
143.===========================================================================*/   
144.int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)   
145.{   
146.   *ppObj = NULL;   
147.           
148.   // We want to load this App. So, invoke AEEApplet_New().To it, pass the   
149.   // address of the app-specific handle event function.   
150.   if(ClsId == AEECLSID_ADDRBOOK_BID)   
151.   {   
152.      if(AEEApplet_New(sizeof(CAddrBkApp), ClsId, pIShell,po,(IApplet**)ppObj,   
153.         (AEEHANDLER)AddrBkApp_HandleEvent,(PFNFREEAPPDATA)AddrBk_FreeAppData)   
154.         == TRUE)   
155.      {   
156.         if (AddrBk_InitAppData((IApplet*)*ppObj) == TRUE)   
157.         {   
158.            return(AEE_SUCCESS);   
159.         }   
160.      }   
161.   }   
162.    return (EFAILED);   
163.}   
164.   
165./*===========================================================================  
166.  
167.FUNCTION AddrBkApp_HandleEvent  
168.  
169.DESCRIPTION  
170.    This is the EventHandler for this app. All events to this app are handled in this  
171.    function. All APPs must supply an Event Handler.  
172.  
173.PROTOTYPE:  
174.    boolean AddrBkApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)  
175.  
176.PARAMETERS:  
177.    pi: Pointer to the AEEApplet structure. This structure contains information specific  
178.    to this applet. It was initialized during the AEEClsCreateInstance() function.  
179.  
180.    ecode: Specifies the Event sent to this applet  
181.  
182.   wParam, dwParam: Event specific data.  
183.  
184.DEPENDENCIES  
185.  none  
186.  
187.RETURN VALUE  
188.  TRUE: If the app has processed the event  
189.  FALSE: If the app did not process the event  
190.  
191.SIDE EFFECTS  
192.  none  
193.===========================================================================*/   
194.static boolean AddrBkApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)   
195.{     
196.   CAddrBkApp * pMe = (CAddrBkApp*)pi;   
197.   
198.   switch (eCode)    
199.    {   
200.      case EVT_APP_START:   
201.         // Create the IMenu interface object. When the applet is started the    
202.         // main menu is set up. When the user selects a menu item from the main    
203.         // menu, an EVT_COMMAND event is received by this applet. The processing for    
204.         // EVT_COMMAND event is found below.   
205.            if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_MENUCTL, (void **)&pMe->m_pIMenu)   
206.            != SUCCESS)   
207.         {   
208.                return FALSE;   
209.         }   
210.   
211.         // Build the main menu   
212.            BuildMainMenu(pMe);   
213.            return(TRUE);   
214.      case EVT_APP_STOP:   
215.         return(TRUE);   
216.      case EVT_KEY:    
217.         // We come here when a key is pressed on the handset keypad. Call    
218.         // the IMenuCtl handle event function to handle this key press.   
219.         // If this key press corressponds to an Up/Down arrow    
220.         // key press, this event will be handled by the IMenuCtl object   
221.         // in moving to the next menu item (in the given direction).    
222.         // If the key is neither Up/Down, the IMenuCtl the key press    
223.         // is not handled, hence, return FALSE.   
224.            if(pMe->m_pIMenu != NULL && IMENUCTL_HandleEvent(pMe->m_pIMenu, EVT_KEY, wParam, 0))   
225.                return TRUE;   
226.         else   
227.            return FALSE;   
228.        case EVT_COMMAND:   
229.         // When a menu item is selected this applet receives the   
230.         // EVT_COMMAND event in the wParam field.   
231.          return AddrBkUsage (pMe, wParam);   
232.      default:   
233.         break;   
234.   }   
235.   return FALSE;   
236.}   
237.   
238.   
239.   
240./*===========================================================================  
241.  
242.FUNCTION AddrBk_InitAppData  
243.  
244.DESCRIPTION  
245.   This function initializes app specific data allocates memory for app data   
246.   (AppletData) and sets it to pAppData of AEEApplet. It also function .   
247.   App developer needs to fill-in the initialization code.  
248.  
249.PROTOTYPE:  
250.    boolean AddrBk_InitAppData(IApplet* pi);  
251.  
252.PARAMETERS:  
253.    pi [in]: Pointer to the IApplet structure. This structure contains   
254.   information specific to this applet. It was initialized during the   
255.   AEEClsCreateInstance().  
256.  
257.DEPENDENCIES  
258.   Assumes pi is not NULL  
259.  
260.RETURN VALUE  
261.  TRUE: If the app has app data is allocated and initialized successfully  
262.  FALSE: Either app data could not be allocated or initialzied  
263.  
264.SIDE EFFECTS  
265.  none  
266.===========================================================================*/   
267.static boolean AddrBk_InitAppData(IApplet* pi)   
268.{   
269.   int pnAscent;        // Stores the ascent in number of pixels   
270.   int pnDescent;       // Stores the descent in number of pixels   
271.   
272.   CAddrBkApp * pMe = (CAddrBkApp*)pi;   
273.   
274.   // Initialize the MenuCtl pointer to NULL   
275.   pMe->m_pIMenu = NULL;   
276.   
277.   // Get the font metrics info   
278.   pMe->m_nLineHeight = IDISPLAY_GetFontMetrics (pMe->a.m_pIDisplay, AEE_FONT_NORMAL,   
279.      &pnAscent, &pnDescent);   
280.   
281.   ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->m_dInfo);   
282.   
283.   return TRUE;   
284.}   
285.   
286./*===========================================================================  
287.  
288.FUNCTION AddrBk_FreeAppData  
289.  
290.DESCRIPTION  
291.   Frees data contained in app data and memory for app data itself.  
292.   App developer needs to free data contained in AppletData.  
293.  
294.PROTOTYPE:  
295.    void AddrBk_FreeAppData(IApplet* pi);  
296.  
297.PARAMETERS:  
298.    pi [in]: Pointer to the IApplet structure. This structure contains   
299.   information specific to this applet. It was initialized during the   
300.   AEEClsCreateInstance().  
301.  
302.DEPENDENCIES  
303.   Assumes pi is not NULL  
304.  
305.RETURN VALUE  
306.   None  
307.  
308.SIDE EFFECTS  
309.   None  
310.===========================================================================*/   
311.static void AddrBk_FreeAppData(IApplet* pi)   
312.{   
313.   CAddrBkApp * pMe = (CAddrBkApp*)pi;   
314.   
315.   // Release main menu   
316.   if (pMe->m_pIMenu != NULL)   
317.   {   
318.      IMENUCTL_Release (pMe->m_pIMenu);   
319.      pMe->m_pIMenu = NULL;   
320.   }      
321.}   
322.   
323./*===============================================================================  
324.                     APP HELPER FUNCTION DEFINITIONS  
325.=============================================================================== */   
326.   
327./*===========================================================================  
328.  
329.FUNCTION: AddrBkUsage  
330.  
331.DESCRIPTION  
332.    This function encompasses all the usage examples of all the functions  
333.   in code blocks switched using the BREW API function Id passed into this  
334.   function. The code blocks will include almost all the variable declarations  
335.   needed to demonstrate the usage of a given function. Within the code block  
336.   all the needed interfaces are instantiated and released subsequent to their  
337.   use.  
338.  
339.   There are some exceptions to the above general rule, such as call-back functions,  
340.   common helper function, or common display functions called from within the  
341.   code block which are defined elsewhere in this file.  
342.  
343.   There are some instances where an object instantiated within a code block is  
344.   not released at the end of the code block, to be released at a later point  
345.   (for example in a call-back function, in this case pointers to such objects  
346.   are stored in the AEEApplet object to be released later).  
347.     
348.PROTOTYPE:  
349.   void AddrBkUsage (CAddrBkApp * pMe, uint16 wParam)  
350.  
351.PARAMETERS:  
352.    pMe: [in]: Contains a pointer to the CAddrBkApp struct.  
353.   wParam: [in]: Contains the Id of the function type to be used (contains the  
354.      Id corresponding to the menu item selected on the emulator).  
355.  
356.DEPENDENCIES  
357.  none  
358.  
359.RETURN VALUE  
360.  TRUE: If the functions call was successful. FALSE otherwise  
361.  
362.SIDE EFFECTS  
363.  none  
364.===========================================================================*/   
365.static boolean AddrBkUsage (CAddrBkApp * pme, uint16 wParam)   
366.{   
367.   IShell *pIShell;   
368.   char *szWorkNum,*szHomeNum, *szName, *szEmail, *szURL, *szAddr,*szNotes;   
369.   AECHAR *aszWorkNum,*aszHomeNum, *aszName, *aszEmail, *aszURL, *aszAddr,*aszNotes;   
370.   AEEAddrField field[7];   
371.   boolean bRet = TRUE;   
372.   uint16   nSize;      
373.   
374.   
375.   szWorkNum = (char *)MALLOC(20);   
376.   szHomeNum = (char *)MALLOC(20);   
377.   szName = (char *)MALLOC(30);   
378.   szEmail = (char *)MALLOC(30);   
379.   szURL = (char *)MALLOC(30);   
380.   szAddr = (char *)MALLOC(30);   
381.   szNotes = (char *)MALLOC(30);   
382.   
383.   aszWorkNum = (AECHAR *)MALLOC(40);   
384.   aszHomeNum = (AECHAR *)MALLOC(40);   
385.   aszName = (AECHAR *)MALLOC(60);   
386.   aszEmail = (AECHAR *)MALLOC(60);   
387.   aszURL = (AECHAR *)MALLOC(60);   
388.   aszAddr = (AECHAR *)MALLOC(60);   
389.   aszNotes = (AECHAR *)MALLOC(60);   
390.   
391.   if(!szWorkNum || !szHomeNum ||  !szName ||  !szEmail ||  !szURL ||  !szAddr || !szNotes ||    
392.      !aszWorkNum || !aszHomeNum ||  !aszName ||  !aszEmail ||  !aszURL ||  !aszAddr ||    
393.      !aszNotes)   
394.   {   
395.      return(FALSE);   
396.   }   
397.   
398.   
399.   pIShell = pme->a.m_pIShell;   
400.   
401.   // Erase screen first for display output purposes.   
402.   IDISPLAY_ClearScreen (pme->a.m_pIDisplay);   
403.   
404.   switch (wParam)   
405.   {   
406.   
407.   
408.      case ADDRBK_CREATE_REC:   
409.      {   
410.   
411.           IAddrBook *pAddrBook;   
412.   
413.           //Work Num   
414.           field[0].fID = AEE_ADDRFIELD_PHONE_WORK;   
415.           field[0].fType = AEEDB_FT_STRING;   
416.           STRCPY(szWorkNum,"8589996666");   
417.           STRTOWSTR(szWorkNum,aszWorkNum,40);   
418.           field[0].pBuffer= aszWorkNum;   
419.           field[0].wDataLen = (WSTRLEN(aszWorkNum)+1)*sizeof(AECHAR);   
420.   
421.           //HOME Num   
422.           field[1].fID = AEE_ADDRFIELD_PHONE_HOME;   
423.           field[1].fType = AEEDB_FT_STRING;   
424.           STRCPY(szHomeNum,"8581112222");   
425.           STRTOWSTR(szHomeNum,aszHomeNum,40);   
426.           field[1].pBuffer= aszHomeNum;   
427.           field[1].wDataLen = (WSTRLEN(aszHomeNum)+1)*sizeof(AECHAR);   
428.   
429.           //Name   
430.           field[2].fID = AEE_ADDRFIELD_NAME;   
431.           field[2].fType = AEEDB_FT_STRING;   
432.           STRCPY(szName,"My BREW");   
433.           STRTOWSTR(szName,aszName,60);   
434.           field[2].pBuffer= aszName;   
435.           field[2].wDataLen = (WSTRLEN(aszName)+1)*sizeof(AECHAR);   
436.   
437.           //Email   
438.           field[3].fID = AEE_ADDRFIELD_EMAIL;   
439.           field[3].fType = AEEDB_FT_STRING;   
440.           STRCPY(szEmail,"me@brew.com");   
441.           STRTOWSTR(szEmail,aszEmail,60);   
442.           field[3].pBuffer= aszEmail;   
443.           field[3].wDataLen = (WSTRLEN(aszEmail)+1)*sizeof(AECHAR);   
444.   
445.   
446.           //URL   
447.           field[4].fID = AEE_ADDRFIELD_URL;   
448.           field[4].fType = AEEDB_FT_STRING;   
449.           STRCPY(szURL,"www.brew.com");   
450.           STRTOWSTR(szURL,aszURL,60);   
451.           field[4].pBuffer= aszURL;   
452.           field[4].wDataLen = (WSTRLEN(aszURL)+1)*sizeof(AECHAR);   
453.   
454.           //Address   
455.           field[5].fID = AEE_ADDRFIELD_ADDRESS;   
456.           field[5].fType = AEEDB_FT_STRING;   
457.           STRCPY(szAddr,"21, Park Avenue");   
458.           STRTOWSTR(szAddr,aszAddr,60);   
459.           field[5].pBuffer= aszAddr;   
460.           field[5].wDataLen = (WSTRLEN(aszAddr)+1)*sizeof(AECHAR);   
461.   
462.   
463.           //Notes   
464.           field[6].fID = AEE_ADDRFIELD_NOTES;   
465.           field[6].fType = AEEDB_FT_STRING;   
466.           STRCPY(szNotes,"Test Notes.Hoping.");   
467.           STRTOWSTR(szNotes,aszNotes,60);   
468.           field[6].pBuffer= aszNotes;   
469.           field[6].wDataLen = (WSTRLEN(aszNotes)+1)*sizeof(AECHAR);   
470.   
471.   
472.   
473.           if(!ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ADDRBOOK, (void **)&pAddrBook))   
474.           {   
475.               IAddrRec *pRec;   
476.   
477.               pRec = IADDRBOOK_CreateRec(pAddrBook,AEE_ADDR_CAT_PERSONAL, (AEEAddrField*)field, 7);   
478.   
479.               if(pRec)   
480.               {   
481.                  DisplayOutput (pme, -1, "MyBREW Record Create Successful");   
482.                  IADDRREC_Release(pRec);   
483.               }   
484.               else   
485.                  DisplayOutput (pme, -1, "MyBREW Record Create Failed");   
486.   
487.   
488.               IADDRBOOK_Release(pAddrBook);   
489.           }   
490.           else   
491.               DisplayOutput (pme, -1, "Could not create IAddrBook");   
492.   
493.         }   
494.         break;   
495.   
496.   
497.   
498.   
499.      case ADDRBK_ADD_FIELD:   
500.         {   
501.   
502.            //Add Field. Add field to the MyBREW record   
503.   
504.            IAddrRec *pR;   
505.            IAddrBook *pb;   
506.            int nRet;   
507.   
508.            STRCPY(szName,"My BREW");   
509.            STRTOWSTR(szName,aszName,60);   
510.   
511.   
512.            if(!ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ADDRBOOK, (void **)&pb))   
513.            {   
514.               
515.               nSize = (WSTRLEN(aszName)+1) *sizeof(AECHAR);   
516.   
517.               nRet = IADDRBOOK_EnumRecInit(pb,  AEE_ADDR_CAT_NONE, AEE_ADDRFIELD_NAME, (void *)aszName, nSize);   
518.               if(nRet == AEE_SUCCESS)   
519.               {   
520.                  pR = IADDRBOOK_EnumNextRec(pb);   
521.                  if(pR)   
522.                  {   
523.   
524.                    //Fax Num   
525.                    field[0].fID = AEE_ADDRFIELD_PHONE_FAX;   
526.                    field[0].fType = AEEDB_FT_STRING;   
527.                    STRCPY(szHomeNum,"9998886666");   
528.                    STRTOWSTR(szHomeNum,aszHomeNum,40);   
529.                    field[0].pBuffer= aszHomeNum;   
530.                    field[0].wDataLen = (WSTRLEN(aszHomeNum)+1)*sizeof(AECHAR);   
531.                     
532.                    if(IADDRREC_AddField(pR,(AEEAddrField*)(&field[0])) == AEE_SUCCESS)   
533.                        DisplayOutput (pme, -1, "Added FaxNum to MyBrew");   
534.                    else   
535.                        DisplayOutput (pme, -1, "Failed to Add FaxNum field to MyBrew");   
536.   
537.                    IADDRREC_Release(pR);   
538.                  }   
539.                  else   
540.                     DisplayOutput (pme, -1, "Failed to Locate MyBrew");   
541.               }   
542.               else   
543.                  DisplayOutput (pme, -1, "Failed to Initialize Search for MyBrew");   
544.   
545.               IADDRBOOK_Release(pb);   
546.            }   
547.            else   
548.               DisplayOutput (pme, -1, "Could not create IAddrBook");   
549.         }   
550.         break;   
551.   
552.      case ADDRBK_REMOVE_FIELD:   
553.      {   
554.   
555.            //Add Field. Remove field from the MyBREW record   
556.   
557.            IAddrRec *pR;   
558.            IAddrBook *pb;   
559.            int nRet,nCount;   
560.   
561.            STRCPY(szName,"My BREW");   
562.            STRTOWSTR(szName,aszName,60);   
563.   
564.            if(!ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ADDRBOOK, (void **)&pb))   
565.            {   
566.               
567.               nSize = (WSTRLEN(aszName)+1) *sizeof(AECHAR);   
568.               nRet = IADDRBOOK_EnumRecInit(pb,  AEE_ADDR_CAT_NONE, AEE_ADDRFIELD_NAME, (void *)aszName, nSize);   
569.               if(nRet == AEE_SUCCESS)   
570.               {   
571.                  pR = IADDRBOOK_EnumNextRec(pb);   
572.                  if(pR)   
573.                  {   
574.   
575.                     nCount = IADDRREC_GetFieldCount(pR);   
576.   
577.                     if(IADDRREC_RemoveField(pR,nCount-1) == AEE_SUCCESS)   
578.                        DisplayOutput (pme, -1, "Removed last field from MyBrew");   
579.                    else   
580.                        DisplayOutput (pme, -1, "Failed to Remove field from MyBrew");   
581.   
582.                    IADDRREC_Release(pR);   
583.                  }   
584.                  else   
585.                     DisplayOutput (pme, -1, "Failed to Locate MyBrew");   
586.               }   
587.               else   
588.                  DisplayOutput (pme, -1, "Failed to Initialize Search for MyBrew");   
589.   
590.               IADDRBOOK_Release(pb);   
591.            }   
592.            else   
593.               DisplayOutput (pme, -1, "Could not create IAddrBook");   
594.   
595.      }   
596.      break;   
597.   
598.      case ADDRBK_UPDATE_FIELD:   
599.      {   
600.            //Update field in the MyBREW record   
601.   
602.            IAddrRec *pR;   
603.            IAddrBook *pb;   
604.            int nRet;   
605.   
606.            STRCPY(szName,"My BREW");   
607.            STRTOWSTR(szName,aszName,60);   
608.   
609.            if(!ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ADDRBOOK, (void **)&pb))   
610.            {   
611.               
612.               nSize = (WSTRLEN(aszName)+1) *sizeof(AECHAR);   
613.               nRet = IADDRBOOK_EnumRecInit(pb,  AEE_ADDR_CAT_NONE, AEE_ADDRFIELD_NAME, (void *)aszName, nSize);   
614.               if(nRet == AEE_SUCCESS)   
615.               {   
616.                  pR = IADDRBOOK_EnumNextRec(pb);   
617.                  if(pR)   
618.                  {   
619.   
620.                     //Change Work Phone Number   
621.                     AEEAddrField newF, *ptr = NULL;   
622.                     int i,nFields = IADDRREC_GetFieldCount(pR);   
623.   
624.   
625.                     for(i = 0; i < nFields; i++)   
626.                     {   
627.                        ptr = IADDRREC_GetField(pR,i);   
628.                        if(ptr->fID == AEE_ADDRFIELD_PHONE_WORK)   
629.                           break;   
630.                     }   
631.                     if(ptr)   
632.                     {   
633.                        newF.fID = ptr->fID;   
634.                        newF.fType = ptr->fType;   
635.                        STRCPY(szWorkNum,"9142356789");   
636.                        STRTOWSTR(szWorkNum,aszWorkNum,40);   
637.                        newF.pBuffer = aszWorkNum;   
638.                        newF.wDataLen = (WSTRLEN(aszWorkNum)+1)*sizeof(AECHAR);   
639.                           
640.                        if(IADDRREC_UpdateField(pR,i,&newF) == AEE_SUCCESS)   
641.                           DisplayOutput (pme, -1, "Updated WorkNum in MyBrew");   
642.                        else   
643.                           DisplayOutput (pme, -1, "Failed to Update WorkNum in MyBrew");   
644.                     }   
645.                     else   
646.                        DisplayOutput (pme, -1, "Failed to Locate WorkNum in MyBrew");   
647.   
648.                    IADDRREC_Release(pR);   
649.                  }   
650.                  else   
651.                     DisplayOutput (pme, -1, "Failed to Locate MyBrew");   
652.               }   
653.               else   
654.                  DisplayOutput (pme, -1, "Failed to Initialize Search for MyBrew");   
655.   
656.               IADDRBOOK_Release(pb);   
657.            }   
658.            else   
659.               DisplayOutput (pme, -1, "Could not create IAddrBook");   
660.   
661.      }   
662.      break;   
663.   
664.      case ADDRBK_UPDATE_ALL_FIELDS:   
665.      {   
666.            //Update all fields in the MyBREW record   
667.   
668.            IAddrRec *pR;   
669.            IAddrBook *pb;   
670.            int nRet;   
671.   
672.            STRCPY(szName,"My BREW");   
673.            STRTOWSTR(szName,aszName,60);   
674.   
675.            if(!ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ADDRBOOK, (void **)&pb))   
676.            {   
677.               
678.               nSize = (WSTRLEN(aszName)+1) *sizeof(AECHAR);   
679.               nRet = IADDRBOOK_EnumRecInit(pb,  AEE_ADDR_CAT_NONE, AEE_ADDRFIELD_NAME, (void *)aszName, nSize);   
680.               if(nRet == AEE_SUCCESS)   
681.               {   
682.                  pR = IADDRBOOK_EnumNextRec(pb);   
683.                  if(pR)   
684.                  {   
685.   
686.                    //Work Num   
687.                    field[0].fID = AEE_ADDRFIELD_PHONE_WORK;   
688.                    field[0].fType = AEEDB_FT_STRING;   
689.                    STRCPY(szWorkNum,"1111111111");   
690.                    STRTOWSTR(szWorkNum,aszWorkNum,40);   
691.                    field[0].pBuffer= aszWorkNum;   
692.                    field[0].wDataLen = (WSTRLEN(aszWorkNum)+1)*sizeof(AECHAR);   
693.   
694.                    //HOME Num   
695.                    field[1].fID = AEE_ADDRFIELD_PHONE_HOME;   
696.                    field[1].fType = AEEDB_FT_STRING;   
697.                    STRCPY(szHomeNum,"2222222222");   
698.                    STRTOWSTR(szHomeNum,aszHomeNum,40);   
699.                    field[1].pBuffer= aszHomeNum;   
700.                    field[1].wDataLen = (WSTRLEN(aszHomeNum)+1)*sizeof(AECHAR);   
701.   
702.                    //Name   
703.                    field[2].fID = AEE_ADDRFIELD_NAME;   
704.                    field[2].fType = AEEDB_FT_STRING;   
705.                    STRCPY(szName,"My BREW");   
706.                    STRTOWSTR(szName,aszName,60);   
707.                    field[2].pBuffer= aszName;   
708.                    field[2].wDataLen = (WSTRLEN(aszName)+1)*sizeof(AECHAR);   
709.   
710.                    //Email   
711.                    field[3].fID = AEE_ADDRFIELD_EMAIL;   
712.                    field[3].fType = AEEDB_FT_STRING;   
713.                    STRCPY(szEmail,"new@new.com");   
714.                    STRTOWSTR(szEmail,aszEmail,60);   
715.                    field[3].pBuffer= aszEmail;   
716.                    field[3].wDataLen = (WSTRLEN(aszEmail)+1)*sizeof(AECHAR);   
717.                           
718.                    if(IADDRREC_UpdateAllFields(pR,field,4) == AEE_SUCCESS)   
719.                      DisplayOutput (pme, -1, "Updated all fields in MyBrew");   
720.                    else   
721.                      DisplayOutput (pme, -1, "Failed to Update all fields in MyBrew");   
722.   
723.                    IADDRREC_Release(pR);   
724.                  }   
725.                  else   
726.                   DisplayOutput (pme, -1, "Failed to Locate MyBrew");   
727.                    
728.                }   
729.                else   
730.                  DisplayOutput (pme, -1, "Failed to Initialize Search for MyBrew");   
731.   
732.               IADDRBOOK_Release(pb);   
733.            }   
734.            else   
735.               DisplayOutput (pme, -1, "Could not create IAddrBook");   
736.   
737.      }   
738.      break;   
739.   
740.      case ADDRBK_REMOVE_REC:   
741.      {   
742.            //Remove MyBREW record   
743.   
744.            IAddrRec *pR;   
745.            IAddrBook *pb;   
746.            int nRet;   
747.   
748.            STRCPY(szName,"My BREW");   
749.            STRTOWSTR(szName,aszName,60);   
750.   
751.            if(!ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ADDRBOOK, (void **)&pb))   
752.            {   
753.               
754.               nSize = (WSTRLEN(aszName)+1) *sizeof(AECHAR);   
755.               nRet = IADDRBOOK_EnumRecInit(pb,  AEE_ADDR_CAT_NONE, AEE_ADDRFIELD_NAME, (void *)aszName, nSize);   
756.               if(nRet == AEE_SUCCESS)   
757.               {   
758.                  pR = IADDRBOOK_EnumNextRec(pb);   
759.                  if(pR)   
760.                  {   
761.   
762.                     if(IADDRREC_RemoveRec(pR) == AEE_SUCCESS)   
763.                        DisplayOutput (pme, -1, "MyBrew Removed");   
764.                     else   
765.                        DisplayOutput (pme, -1, "Failed to remove MyBrew");   
766.                  }   
767.                  else   
768.                     DisplayOutput (pme, -1, "Failed to Locate MyBrew");   
769.               }   
770.               else   
771.                  DisplayOutput (pme, -1, "Failed to Initialize Search for MyBrew");   
772.   
773.               IADDRBOOK_Release(pb);   
774.            }   
775.            else   
776.               DisplayOutput (pme, -1, "Could not create IAddrBook");   
777.   
778.      }   
779.      break;   
780.   
781.      case ADDRBK_GET_FIELD_COUNT:   
782.      {   
783.            //Get Field Count   
784.   
785.            IAddrRec *pR;   
786.            IAddrBook *pb;   
787.            int nRet,nCount;   
788.   
789.            STRCPY(szName,"My BREW");   
790.            STRTOWSTR(szName,aszName,60);   
791.   
792.            if(!ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ADDRBOOK, (void **)&pb))   
793.            {   
794.               
795.               nSize = (WSTRLEN(aszName)+1) *sizeof(AECHAR);   
796.               nRet = IADDRBOOK_EnumRecInit(pb,  AEE_ADDR_CAT_NONE, AEE_ADDRFIELD_NAME, (void *)aszName, nSize);   
797.               if(nRet == AEE_SUCCESS)   
798.               {   
799.                  pR = IADDRBOOK_EnumNextRec(pb);   
800.                  if(pR)   
801.                  {   
802.   
803.                     nCount = IADDRREC_GetFieldCount(pR);   
804.                     SPRINTF(szName,"MyBREW has %d fields",nCount);   
805.   
806.                     DisplayOutput (pme, -1, szName);   
807.                     IADDRREC_Release(pR);   
808.                  }   
809.                  else   
810.                     DisplayOutput (pme, -1, "Failed to Locate MyBrew");   
811.               }   
812.               else   
813.                  DisplayOutput (pme, -1, "Failed to Initialize Search for MyBrew");   
814.   
815.               IADDRBOOK_Release(pb);   
816.            }   
817.            else   
818.               DisplayOutput (pme, -1, "Could not create IAddrBook");   
819.   
820.      }   
821.      break;   
822.   
823.      case ADDRBK_COUNT_RECS:   
824.      {   
825.            //Count Recs in phone book   
826.   
827.            IAddrBook *pb;   
828.            int nCount;   
829.   
830.   
831.            if(!ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ADDRBOOK, (void **)&pb))   
832.            {   
833.               
834.               nCount = IADDRBOOK_GetNumRecs(pb);   
835.   
836.               SPRINTF(szName,"Address Book has %d records",nCount);   
837.   
838.               DisplayOutput (pme, -1, szName);   
839.   
840.               IADDRBOOK_Release(pb);   
841.            }   
842.            else   
843.               DisplayOutput (pme, -1, "Could not create IAddrBook");   
844.   
845.      }   
846.      break;   
847.   
848.      case ADDRBK_ENUM_CATEGORIES:   
849.      {   
850.            //Enum Categories   
851.   
852.            IAddrBook *pb;   
853.            int nCount;   
854.   
855.   
856.            if(!ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ADDRBOOK, (void **)&pb))   
857.            {   
858.               
859.                  if(IADDRBOOK_EnumCategoryInit(pb) == AEE_SUCCESS)   
860.                  {   
861.                     AEEAddrCat cat;   
862.                     nCount = 0;   
863.                     while(IADDRBOOK_EnumNextCategory(pb,&cat))   
864.                        nCount++;   
865.         
866.                     SPRINTF(szName,"%d Address Categories supported",nCount);   
867.   
868.                     DisplayOutput (pme, -1, szName);   
869.                  }   
870.                  else   
871.                     DisplayOutput (pme, -1, "Error EnumCategoryInit");   
872.         
873.                  IADDRBOOK_Release(pb);   
874.            }   
875.            else   
876.               DisplayOutput (pme, -1, "Could not create IAddrBook");   
877.   
878.      }   
879.      break;   
880.   
881.      case ADDRBK_ENUM_FIELDS:   
882.      {   
883.            //Enum Fields   
884.   
885.            IAddrBook *pb;   
886.            int nCount;   
887.   
888.   
889.            if(!ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_ADDRBOOK, (void **)&pb))   
890.            {   
891.               
892.                  if(IADDRBOOK_EnumFieldsInfoInit(pb,AEE_ADDR_CAT_PERSONAL) == AEE_SUCCESS)   
893.                  {   
894.                     AEEAddrFieldInfo fInfo;   
895.                     nCount = 0;   
896.                     while(IADDRBOOK_EnumNextFieldsInfo(pb,&fInfo))   
897.                        nCount++;   
898.         
899.                     SPRINTF(szName,"%d Address Fields supported",nCount);   
900.   
901.                     DisplayOutput (pme, -1, szName);   
902.                  }   
903.                  else   
904.                     DisplayOutput (pme, -1, "Error EnumFieldsInfoInit");   
905.         
906.                  IADDRBOOK_Release(pb);   
907.            }   
908.            else   
909.               DisplayOutput (pme, -1, "Could not create IAddrBook");   
910.   
911.      }   
912.      break;   
913.   
914.      default:   
915.         bRet = FALSE;   
916.   }   
917.      
918.   // Display above event.    
919.   DisplayEvent (pme, wParam);   
920.   
921.   //Cleanup   
922.   FREE(szWorkNum);   
923.   FREE(szHomeNum);   
924.   FREE(szName);    
925.   FREE(szEmail);    
926.   FREE(szURL);    
927.   FREE(szAddr);   
928.   FREE(szNotes);   
929.   FREE(aszWorkNum);   
930.   FREE(aszHomeNum);    
931.   FREE(aszName);    
932.   FREE(aszEmail);    
933.   FREE(aszURL);    
934.   FREE(aszAddr);   
935.   FREE(aszNotes);   
936.   
937.   return (bRet);   
938.}   
939.   
940./*===========================================================================  
941.  
942.FUNCTION: DisplayEvent  
943.  
944.DESCRIPTION  
945.    This function is a display function which sets up the initial screen  
946.    for the display output for a given interface function. This displays  
947.    the function name at the top of the screen and displays the  
948.    strings "Press Up/Down" and "keys to continue.." at the bottom of   
949.    the screen. In the case of functions that have no output it also   
950.    displays the string "No Output..." in the middile of the screen.  
951.  
952.PROTOTYPE:  
953.   void DisplayEvent (CAddrBkApp *pMe, uint16 wParam)  
954.  
955.PARAMETERS:  
956.   pMe:    [in]: Pointer to CAddrBkApp interface object  
957.   wParam: [in]: API function type Id selected by user  
958.  
959.DEPENDENCIES  
960.  None  
961.  
962.RETURN VALUE  
963.  None  
964.  
965.SIDE EFFECTS  
966.  None  
967.  
968.===========================================================================*/   
969.static void DisplayEvent (CAddrBkApp *pMe, uint16 wParam)   
970.{   
971.   AECHAR szBuf[100];   
972.   AEERect rc;   
973.   
974.   if (pMe == NULL || pMe->a.m_pIShell == NULL || pMe->a.m_pIDisplay == NULL)   
975.      return;   
976.   
977.   // Set display rectangle   
978.   SETAEERECT (&rc, 0, 0, pMe->m_dInfo.cxScreen, pMe->m_dInfo.cyScreen);   
979.   
980.   if (pMe->a.m_pIDisplay)   
981.   {   
982.   
983.      STR_TO_WSTR("Press Up/Down", szBuf, sizeof(szBuf));   
984.        IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szBuf, -1,5, rc.dy -    
985.         2*pMe->m_nLineHeight, &rc, IDF_ALIGN_NONE|  IDF_TEXT_TRANSPARENT);   
986.      STR_TO_WSTR("keys to continue..", szBuf, sizeof(szBuf));   
987.        IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, szBuf, -1,5, rc.dy -    
988.         pMe->m_nLineHeight, &rc, IDF_ALIGN_NONE|  IDF_TEXT_TRANSPARENT);   
989.        IDISPLAY_UpdateEx(pMe->a.m_pIDisplay,TRUE);   
990.   }   
991.}   
992.   
993./*===========================================================================  
994.  
995.FUNCTION: BuildMainMenu  
996.  
997.DESCRIPTION  
998.    This function builds the main menu when this applet is started.  
999.    It displays a simple menu with menu items corresponding to   
1000.    each of the interface functions.  
1001.      
1002.PROTOTYPE:  
1003.   void BuildMainMenu(CAddrBkApp *pMe)  
1004.  
1005.PARAMETERS:  
1006.   pMe: [in]: Pointer to AEEApplet struct  
1007.  
1008.DEPENDENCIES  
1009.  None  
1010.  
1011.RETURN VALUE  
1012.  None  
1013.  
1014.SIDE EFFECTS  
1015.  None  
1016.  
1017.===========================================================================*/   
1018.static void BuildMainMenu(CAddrBkApp *pMe)   
1019.{   
1020.    AEERect rc;   
1021.   AECHAR szBuf[40];   
1022.   
1023.   if (pMe->m_pIMenu == NULL || pMe->a.m_pIShell == NULL)   
1024.      return;   
1025.   
1026.    // Set Title   
1027.   STR_TO_WSTR("AddrBook Examples", szBuf, sizeof(szBuf));   
1028.    IMENUCTL_SetTitle(pMe->m_pIMenu, NULL, 0, szBuf);   
1029.   
1030.    //Set the Rectangle for the Menu   
1031.   SETAEERECT (&rc, 0, 0, pMe->m_dInfo.cxScreen, pMe->m_dInfo.cyScreen);   
1032.   
1033.    IMENUCTL_SetRect(pMe->m_pIMenu, &rc);     
1034.   
1035.    //Add invidual entries to the Menu   
1036.   
1037.   
1038.   STR_TO_WSTR("Create Record", szBuf, sizeof(szBuf));   
1039.    IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, ADDRBK_CREATE_REC, szBuf, 0);   
1040.   
1041.   
1042.   STR_TO_WSTR("Add Field", szBuf, sizeof(szBuf));   
1043.    IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, ADDRBK_ADD_FIELD, szBuf, 0);   
1044.   
1045.      
1046.   STR_TO_WSTR("Remove Field", szBuf, sizeof(szBuf));   
1047.    IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, ADDRBK_REMOVE_FIELD, szBuf, 0);   
1048.               
1049.   STR_TO_WSTR("Update Field", szBuf, sizeof(szBuf));   
1050.    IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, ADDRBK_UPDATE_FIELD, szBuf, 0);   
1051.   
1052.      
1053.   STR_TO_WSTR("Update All Fields", szBuf, sizeof(szBuf));   
1054.    IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, ADDRBK_UPDATE_ALL_FIELDS, szBuf, 0);   
1055.   
1056.   STR_TO_WSTR("Get Field Count", szBuf, sizeof(szBuf));   
1057.    IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, ADDRBK_GET_FIELD_COUNT, szBuf, 0);   
1058.   
1059.   STR_TO_WSTR("Remove Record", szBuf, sizeof(szBuf));   
1060.    IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, ADDRBK_REMOVE_REC, szBuf, 0);   
1061.   
1062.   
1063.   STR_TO_WSTR("Count Records", szBuf, sizeof(szBuf));   
1064.    IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, ADDRBK_COUNT_RECS, szBuf, 0);     
1065.   
1066.   
1067.   STR_TO_WSTR("Enum Categories", szBuf, sizeof(szBuf));   
1068.    IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, ADDRBK_ENUM_CATEGORIES, szBuf, 0);    
1069.   
1070.      
1071.   STR_TO_WSTR("Enum Fields", szBuf, sizeof(szBuf));   
1072.    IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, ADDRBK_ENUM_FIELDS, szBuf, 0);    
1073.   
1074.   
1075.   // Activate menu   
1076.    IMENUCTL_SetActive(pMe->m_pIMenu,TRUE);   
1077.}   
1078.   
1079./*===========================================================================  
1080.  
1081.FUNCTION: DisplayOutput  
1082.  
1083.DESCRIPTION  
1084.    This function displays an output string at a given line number on the  
1085.    screen. If the nline parameter is a negative value (-1) the string  
1086.    is displayed in the middle of the screen. If the "nline" value is larger  
1087.    than or equal to zero the "nline" value is multiplied by 15 and the   
1088.    resulting value in pixels is set to the y-coordinate of the start of   
1089.    the string display on the screen. If the string does not fit on one line  
1090.    the string wraps around to the next line (spaced rougly 10-15 pixels apart).  
1091.    By default 5 is used as the starting the x-coordinate of a displayed   
1092.    string.  
1093.  
1094.    How many characters that fit on one line is calculated for each line   
1095.    that is wrapped around to the next line.  
1096.  
1097.    Note: depending on the phone screen size and the fonts used for characters   
1098.          the output might differ on different handsets (devices). Where some   
1099.          handsets will have a smaller screen and large default fonts which will   
1100.          cause partial overlapping of lines. This function does not try to address  
1101.          these issues (this is meant as a simple display function).  
1102.      
1103.PROTOTYPE:  
1104.   void DisplayOutput(CAddrBkApp * pMe, int nline, char *pszStr)  
1105.  
1106.PARAMETERS:  
1107.   pMe:     [in]: Contains a pointer to the CAddrBkApp class.  
1108.   nline:  [in]: Contains the line number to start displaying the text.   
1109.        The line numbers are by default spaced 15 pixels apart along the y-axis.  
1110.   pszStr: [in]: The character string to be displayed on the screen.  
1111.  
1112.DEPENDENCIES  
1113.  None  
1114.  
1115.RETURN VALUE  
1116.  None  
1117.  
1118.SIDE EFFECTS  
1119.  None  
1120.  
1121.===========================================================================*/   
1122.static void DisplayOutput(CAddrBkApp * pMe, int nline, char *pszStr)   
1123.{   
1124.   AECHAR * pszBuf = NULL;   
1125.   AECHAR * psz = NULL;   
1126.   int pixelWidth;   
1127.   AEEFont font = AEE_FONT_NORMAL;   
1128.   int pnFits = 0, dy;   
1129.   int totalCh = 0;   
1130.   
1131.   if (pMe == NULL)   
1132.      return;   
1133.   
1134.   // Allocate buffer to hold string.   
1135.   if ((pszBuf = (AECHAR *)MALLOC(200)) == NULL)   
1136.      return;   
1137.   
1138.   // Convert to wide string (unicode)   
1139.   STR_TO_WSTR ((char *)pszStr, pszBuf, 200);   
1140.   
1141.   // If nlines is zero then print this string starting around the middle of    
1142.   // the screen. Or else multiply nlines by 10 to decide the y coordinate of   
1143.   // the start of the string.   
1144.   if (nline < 0) {   
1145.      dy = pMe->m_dInfo.cyScreen*2/5;   
1146.   }   
1147.   else{   
1148.      dy = nline * pMe->m_nLineHeight;   
1149.   }   
1150.   
1151.   // psz keeps track of the point from which to write from the string buffer   
1152.   // in case the string does not fit one line and needs to wrap around in the   
1153.   // next line.   
1154.   psz = pszBuf;   
1155.         
1156.   // Need to calculate the lotal string length to decide if any wrapping   
1157.   // around is needed.   
1158.   totalCh = STRLEN ((char *)pszStr);   
1159.   
1160.   // Keep displaying text string on multiple lines if the string can't be displayed   
1161.   // on one single line. Lines are spaced 15 pixels apart.   
1162.   while ((totalCh > 0) && (*psz != NULL))   
1163.   {    
1164.      // Get information on how many characters will fit in a line.   
1165.      // Give the pointer to the buffer to be displayed, and the number of   
1166.      // pixels along the x axis you want to display the string in (max number)   
1167.      // pnFits will have the max number of chars that will fit in the maxWidth   
1168.      // number of pixels (given string can't fit in one line), or the number of    
1169.      // chars in the string (if it does fit in one line). pnWidth gives the   
1170.      // number of pixels that will be used to display pnFits number of chars.   
1171.      pixelWidth = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,   
1172.                      font,    
1173.                      (AECHAR *) psz,  // Start of the buffer to display,   
1174.                      -1,   
1175.                      pMe->m_dInfo.cxScreen - 5, // maxWidth   
1176.                      &pnFits);         // Number of chars that will fit a line   
1177.   
1178.      // If pnFits is zero there is something wrong in the input to above function.    
1179.      // Normally this scenario should not occur. But, have the check anyway.   
1180.      if (pnFits == 0)   
1181.      {   
1182.         FREE (pszBuf);   
1183.         return;   
1184.      }   
1185.   
1186.      IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_NORMAL, psz, pnFits, 5 /*start dx*/,    
1187.         dy, 0 /* use default rectangle coordinates */, 0);   
1188.   
1189.      psz += pnFits;      // move pointer to the next segment to be displayed   
1190.      totalCh -= pnFits;  // reduce the total number of characters to still display   
1191.      dy += pMe->m_nLineHeight; // Place next line charHeight pixels below the    
1192.                                // previous line.    
1193.      IDISPLAY_Update(pMe->a.m_pIDisplay);   
1194.      if (totalCh < pnFits)   
1195.         pnFits = totalCh;  // if total number is less than pnFits, adjust pnFits   
1196.   }   
1197.   
1198.   FREE (pszBuf);   
1199.   return;      
1200.} // End of DisplayOutput   
1201.   
