/**
 * Copyright (c) 2011. All rights reserved.
 *
 * @author kindlywolf@gmail.com
 *
 */

#include "INIFileParser.h"
#include "XUtil.h"
#include "AEEFile.h"
#include "AEESource.h"

#define SPACE_STR           " \t"
#define EQUAL_MARK_STR      "="
#define QUOTATION_MARK_STR  "\""
#define SEMICOLON_STR       ";"

#define IS_EQUAL_MARK(c)        ('=' == c)
#define IS_QUOTATION_MARK(c)    ('"' == c)
#define IS_SEMICOLON(c)         (';' == c)

static boolean ParseLine(const char *pszLine, int nLen, IniParseLineCB pfnHandler, void *pUser)
{
    int nPosEqualMark = -1;
    int nKeyS = -1;
    int nKeyE = -1;
    int nValueS = -1;
    int nValueE = -1;
    int nCommentS = -1;
    
    // look for start of key
    Array_Find(pszLine, nLen, SPACE_STR, !cmpfn_strchr, 0, nKeyS);
    if (-1 == nKeyS || IS_EQUAL_MARK(pszLine[nKeyS]) || IS_SEMICOLON(pszLine[nKeyS])) {
        return FALSE;
    }
    
    // look for equal mark
    Array_Find(&pszLine[nKeyS + 1], nLen - (nKeyS + 1), EQUAL_MARK_STR, cmpfn_memory, 1, nPosEqualMark);
    if (-1 == nPosEqualMark) {
        return FALSE;
    }
    nPosEqualMark += nKeyS + 1;
    
    // look for end of key
    Array_Find_Rev(&pszLine[nKeyS], nPosEqualMark- nKeyS, SPACE_STR, !cmpfn_strchr, 0, nKeyE);
    if (-1 == nKeyE) {
        return FALSE;
    }
    nKeyE += nKeyS;
    
    // look for start of value
    Array_Find(&pszLine[nPosEqualMark + 1], nLen - (nPosEqualMark + 1), SPACE_STR, !cmpfn_strchr, 0, nValueS);
    if (-1 == nValueS) {
        return FALSE;
    }
    nValueS += nPosEqualMark + 1;
    
    // look for end of value
    if (IS_QUOTATION_MARK(pszLine[nValueS]))
    {
        nValueS++;
        Array_Find(&pszLine[nValueS], nLen - nValueS, QUOTATION_MARK_STR, cmpfn_memory, 1, nValueE);
        if (-1 != nValueE) {
            nValueE += nValueS;
            nValueE--;
        } else {
            nValueE = nLen - 1;
        }
    }
    else
    {
        // look for comment
        Array_Find(&pszLine[nValueS], nLen - nValueS, SEMICOLON_STR, cmpfn_memory, 1, nCommentS);
        if (-1 != nCommentS) {
            nCommentS += nValueS;
            Array_Find_Rev(&pszLine[nValueS], nCommentS - nValueS, SPACE_STR, !cmpfn_strchr, 0, nValueE);
        } else {
            Array_Find_Rev(&pszLine[nValueS], nLen - nValueS, SPACE_STR, !cmpfn_strchr, 0, nValueE);
        }
        
        if (-1 == nValueE) {
            return FALSE;
        }
        nValueE += nValueS;
    }
    
    if (pfnHandler) {
        pfnHandler(pUser, pszLine + nKeyS, nKeyE - nKeyS + 1, pszLine + nValueS, nValueE - nValueS + 1);
    }
    
    return TRUE;
}

int IniFileParser_LoadFile(IShell *pIShell, IniParseLineCB pfnHandler, void *pUser)
{
    int nErr = SUCCESS;
    IFileMgr *pIFileMgr = NULL;
    IFile *pIFile = NULL;
    ISourceUtil *pISourceUtil = NULL;
    ISource *pISource = NULL;
    IGetLine *pIGetLine = NULL;
    GetLine sGetLine = {0};
    int32 nCode = 0;
    
    nErr = ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr);
    if (SUCCESS != nErr) {
        return EFAILED;
    }
    
    pIFile = IFILEMGR_OpenFile(pIFileMgr, "config.ini", _OFM_READ);
    if (NULL == pIFile) {
        nErr = EFAILED;
        goto clear;
    }

    nErr = ISHELL_CreateInstance(pIShell, AEECLSID_SOURCEUTIL, (void **)&pISourceUtil);
    if (SUCCESS != nErr) {
        goto clear;
    }
    nErr = ISOURCEUTIL_SourceFromAStream(pISourceUtil, (IAStream *)pIFile, &pISource);
    if (SUCCESS != nErr) {
        goto clear;
    }
    nErr = ISOURCEUTIL_GetLineFromSource(pISourceUtil, pISource, 4096, &pIGetLine);
    if (SUCCESS != nErr) {
        goto clear;
    }
    RELEASEIF(pISourceUtil);

    do 
    {
        nCode = IGETLINE_GetLine(pIGetLine, &sGetLine, IGETLINE_LF);
        ParseLine(sGetLine.psz, sGetLine.nLen, pfnHandler, pUser);
    } while(!IGETLINE_Exhausted(nCode));

clear:
    RELEASEIF(pIGetLine);
    RELEASEIF(pISource);
    RELEASEIF(pIFile);
    RELEASEIF(pIFileMgr);

    return nErr;
}
