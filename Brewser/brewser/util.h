/*
  ===========================================================================

  FILE: util.h
   
  SERVICES:  
     Shared simple stuff
   
  DESCRIPTION: 
     Utility code

  ===========================================================================
  ===========================================================================
    
               Copyright ?2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/
#ifndef UTIL_H
#define UTIL_H /* #ifndef UTIL_H */

#include "AEE.h"
#include "AEEFile.h"
#include "AEEStdLib.h"
#include "AEEShell.h"

/* handy macros */
#define CSTRLEN(x) (sizeof((x))-1)

#if defined(_WIN32)
#define BREAKPOINT() _asm { int 3 }
#else  /* #if defined(AEE_SIMULATOR) */
#define BREAKPOINT() 0
#endif /* #if defined(AEE_SIMULATOR) */


#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#define FARF_ALWAYS  1
#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 

#define ASSERT_FARF(x) if (!(x)) { FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); }

/* for AEEComdef.h */
#if !defined(MAX_UINT16)
#define MAX_UINT16 0x0ffff
#endif

#if !defined(MAX_INT16)
#define MAX_INT16 0x07fff
#endif

#if !defined(MAX_UINT32)
#define MAX_UINT32 0xffffffff
#endif

#if !defined(MAX_INT32)
#define MAX_INT32 0x7fffffff
#endif


#define RELEASEIF(p) ReleaseIf((IBase**)&(p))

#if defined (__arm)
#define va_argat(args,i)  ((*(int **)args)[i])
#elif defined (_WIN32) || defined (__linux) /* #if defined(__arm) */
#define va_argat(args,i)  (((int *)args)[i])
#else /* #if defined(__arm) #elif defined (_WIN32) || defined(_linux) */
#error unknown va_list format, please help me
#endif /* #if defined(__arm) #elif defined (_WIN32) */


#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

/**
  || Function
  || --------
  || uint32 ReleaseIf(IBase **ppif)
  ||
  || Description
  || -----------
  || releases the interface pointer, sets it to null
  ||
  || Parameters
  || ----------
  || IBase **ppif: interfacee pointer
  ||
  || Returns
  || -------
  || return value of Release(), unless object is NULL, then -1
  ||
  || Remarks
  || -------
  ||
  */
uint32 ReleaseIf(IBase **ppif);

/**
  || Function
  || --------
  || boolean parseuint32(const char *cpsz, int nRadix, uint32 *pnNum);
  || boolean parseuint16(const char *cpsz, int nRadix, uint16 *pnNum);
  ||
  || Description
  || -----------
  || parse an unsigned long into pnNum
  ||
  || Parameters
  || ----------
  || const char *cpsz: null-terminated string to parse
  || int nRadix: number base
  || uint32 *pnNum: number to fill
  ||
  || Returns
  || -------
  || whether parsing succeeded
  ||
  || Remarks
  || -------
  || cpsz must contain only number contents, 
  || these will fail, even though they might work in strtoul():
  ||
  ||   " 1"
  ||   "1 "
  ||   "1g"
  ||
  */
boolean parseuint32(const char *cpsz, int nRadix, uint32 *pnNum);
boolean parseuint16(const char *cpsz, int nRadix, uint16 *pnNum);

/**
  || Function
  || --------
  || boolean parsemonth(const char *cpsz, uint16 *pnMonth);
  || boolean parsewkday(const char *cpsz, uint16 *pnWkday);
  || boolean parseweekday(const char *cpsz, uint16 *pnWeekday);
  ||
  || Description
  || -----------
  || parse a 3-letter month, 3-letter day, fully spelled out day
  ||
  || Parameters
  || ----------
  || const char *cpsz: string
  || uint16 *pnMonth, *pnWkday, *pnWeekday: output month, day number
  ||
  || Returns
  || -------
  || parsemonth: TRUE if cpsz is:
  ||   cpsz    pnMonth
  ||   ----    -------
  ||   "Jan"      0
  ||   "Feb"      1
  ||   "Mar"      2
  ||   "Apr"      3
  ||   "May"      4
  ||   "Jun"      5
  ||   "Jul"      6
  ||   "Aug"      7
  ||   "Sep"      8
  ||   "Oct"      9
  ||   "Nov"      10
  ||   "Dec"      11
  ||
  || parsewkday: TRUE if cpsz is:
  ||   cpsz    pnWkday
  ||   ----    -------
  ||   "Sun"      0
  ||   "Mon"      1
  ||   "Tue"      2
  ||   "Wed"      3
  ||   "Thu"      4
  ||   "Fri"      5
  ||   "Sat"      6
  ||
  || parseweekday: TRUE if cpsz is:
  ||   cpsz           pnWeekday
  ||   ----           -------
  ||   "Sunday"          0
  ||   "Monday"          1
  ||   "Tuesday"         2
  ||   "Wednesday"       3
  ||   "Thursday"        4
  ||   "Friday"          5
  ||   "Saturday"        6
  ||
  || FALSE for anything else
  ||
  || Remarks
  || -------
  || no other formats are currently supported, this 
  ||  function is case-sensitive
  ||
  */
boolean parsemonth(const char *cpsz, uint16 *pnMonth);
boolean parseweekday(const char *cpsz, uint16 *pnWeekday);
boolean parsewkday(const char *cpsz, uint16 *pnWkday);


/**
  || Function
  || --------
  || int vsnxprintf(char *pszBuf, int nBufLen, const char *cpszFormat,
  ||               char pcDelims[2], const char *cpszzTokFormats, va_list args)
  || int snxprintf(char *pszBuf, int nBufLen, const char *cpszFormat,
  ||               char pcDelims[2], const char *cpszzTokFormats, ...)
  ||
  || Description
  || -----------
  || replace tokens in pszBuf with sprintf'd strings
  ||
  || Parameters
  || ----------
  || char *pszBuf: buffer to do replacement on
  || int nBufLen: size, in bytes of pszBuf
  || const char *cpszFormat: string to use as xprintf format string
  ||                         (has tokens)
  || char pcDelims[2]: 2 characters that mark the beginning and end
  ||                     of each token
  || const char **cpszzTokFormats: a doubly-null-terminated list of tokens 
  ||                        and printf format specifiers.  
  || ... or va_list args: the arguments to sprintf according to cpszzTokens
  ||
  || Returns
  || -------
  || 0 on success (string fit completely)
  || > 0 on partial write, the return value is the number of *additional* 
  ||    bytes required to successfully vxsnprintf, including null-termination
  || -EBADPARM if cpszzTokFormats cannot be parsed into 
  ||             token/printf spec pairs
  ||         
  || Remarks
  || -------
  || cpszzTokFormats has the form "string%s\0num%d\0" where "string" 
  ||  and "num" are arbitrary words that may not contain '%', and 
  ||  should not contain either of pcDelims.
  ||
  || You can discover how much space you'll need up front by passing 0 for
  ||  nBufLen.
  ||
  || Example use:
  ||   Say I wanna generate an html document using a template like:
  ||
  ||   "<html><h1>{title}</h1>{body}</html>"
  || 
  ||   I can use vsnxprinf() to help me, like so:
  ||
  ||     #define TEMPLATE "<html><h1>{title}</h1>{body}</html>"
  ||     int   nDocLen;
  ||     char *pszDoc;
  ||     
  ||     nDocLen = snxprintf(0, 0, // pass 0 for buffer and length
  ||                         TEMPLATE, // xprintf format templat
  ||                         "{}",     // use '{' for beginning, '}' for end
  ||                         "title%s\0body%s\0", // doubly-null-terminated!!
  ||                         "Hello World!",       // the title
  ||                         "Nice to be here.");  // the body
  ||     
  ||     pszDoc = (char *)MALLOC(nDocLen);
  ||     
  ||     if (pszDoc) {
  ||        // re-do the above, with the buffer
  ||        snxprintf(pszDoc,  
  ||                  nDocLen, 
  ||                  TEMPLATE,
  ||                  "{}",     
  ||                  "title%s\0body%s\0",
  ||                  "Hello World!",  // arguments must appear in the same
  ||                  "Nice to be here."); // order as in the format string
  ||     }
  ||
  ||
  */
int vsnxprintf(char *pszBuf, int nBufLen, const char *cpszTemplate,
               const char pcDelims[2], const char *cpszzTokFormats, 
               va_list args);
int snxprintf(char *pszBuf, int nBufLen, const char *cpszTemplate,
               const char pcDelims[2], const char *cpszzTokFormats, ...);

/**
  || Function
  || --------
  || char *basename(const char *cpszFile)
  ||
  || Description
  || -----------
  || return a pointer to the basename of the passed in file
  ||
  || Parameters
  || ----------
  || const char *cpszFile: the full file name
  ||
  || Returns
  || -------
  || pointer just past the last '/' or '\\'
  ||
  || Remarks
  || -------
  || don't pass null to this function
  ||
  */
char *basename(const char *cpszFile);

/**
  || Function
  || --------
  || int dequote(char *pc, int nLe)n
  ||
  || Description
  || -----------
  || tear quotes and escaped quotes off a string
  ||
  || Parameters
  || ----------
  || char *pc: string to clean up
  || int nLen: length of the passed in string (not necessarily 
  ||             null-terminated), if -1 STRLEN(pc) is used.
  ||
  || Returns
  || -------
  || length of resulting string
  ||
  || Remarks
  || -------
  || the resulting cleaned up string is not null terminated, this
  ||   function modifies only pc[0]<->pc[nLen-1]
  || 
  || if the passed in string does not begin *and* end with '"', the string's
  ||   not modified
  ||
  */
int dequote(char *pc, int nLen);

/**
  || Function
  || --------
  || int trim(char *pc, int nLen)
  ||
  || Description
  || -----------
  || trim leading and trailing whitespace (' ', '\t', '\r', and '\n' ) 
  ||    from a string 
  ||
  || Parameters
  || ----------
  || char *pc: string to trim
  || int nLen: length of pc
  ||
  || Returns
  || -------
  || length of trimmed string (not including null-term)
  ||
  || Remarks
  || -------
  || the passed-in string needn't be null-terminated.
  ||
  || if no spaces are trimmed from the end of the string, but 
  ||  some are trimmed from the beginning, the *result* will not 
  ||  be null-terminated.
  ||
  */
int trim(char *pc, int nLen);

/**
  || Function
  || --------
  || int strchop(char *pszToChop, const char *cpszChopChars)
  ||
  || Description
  || -----------
  || chop a string into its component words, using cpszChopChars 
  ||   as delimters 
  ||
  || Parameters
  || ----------
  || char *pszToChop: string to chop
  || const char *cpszChopChars: delimiting characters
  ||
  || Returns
  || -------
  || number of words the string was chopped into
  ||
  || Remarks
  || -------
  || chopping is effected by replacing all occurences 
  ||  of characters in cpszChopChars with '\0'
  ||
  */
int strchop(char *pszToChop, const char *cpszChopChars);

int IShell_LoadResBlob(IShell *piShell, const char *cpszFile, uint16 nResID, 
                       AEEResBlob **pprb, int *pnLen);

int IShell_LoadResSz(IShell *piShell,const char *cpszResFile,
                     uint16 uResId, char **ppsz);

int IShell_LoadResWsz(IShell *piShell,const char *cpszResFile,
                      uint16 uResId, AECHAR **ppwsz);

int IShell_OpenFile(IShell *psh, const char *pszName, 
                    AEEOpenFileMode m, IFile **ppf);


int AEEResBlob_VStrRepl(AEEResBlob **pprb, int *pnLen, 
                        const char *cpszzTokFormats, va_list args);
int AEEResBlob_StrRepl(AEEResBlob **pprb, int *pnLen, 
                       const char *cpszzTokFormats, ...);
   
/**
  || Function
  || --------
  || char *strchopped_nth(const char *cpsz, int nNth)
  ||
  || Description
  || -----------
  || returns a pointer to the nNth string in a chopped string (see strchop)
  ||
  || Parameters
  || ----------
  || const char *cpsz: chopped string 
  || int nNth: 0-based index into cpsz
  ||
  || Returns
  || -------
  || pointer to the nNth string
  ||
  || Remarks
  || -------
  || you must know the contents of cpsz, and must not pass an index 
  ||  higher than the number of null terminated strings in cpsz.
  ||
  || if cpsz has leading nulls, they're skipped, so for example:
  || 
  ||   strchopped_nth("\0\0hi\0there\0",1) returns "there"
  ||
  */
char *strchopped_nth(const char *cpsz, int nNth);


/**
  || Function
  || --------
  || char *utf16_to_utf8(unsigned short *cpcIn, int *pnInLen, 
  ||                     char *pcOut, int *pnOutLen);
  ||
  || Description
  || -----------
  || convert a utf16 string to a utf8 string
  ||
  || Parameters
  || ----------
  || unsigned short *cpcIn: utf16 string
  || int *pnInLen: [in/out] input: holds number of characters in cpcIn 
  ||                               to convert 
  ||                        output: holds number of characters *actually* 
  ||                                converted
  || char *pcOut: output buffer, may be null
  || int *pnOutLen: [in/out] input: holds length of pcOut, unless pcOut
  ||                                is null, then ignored
  ||                         output: holds number of bytes written to 
  ||                                 pcOut, unless pcOut is null, then 
  ||                                 holds size of pcOut necessary to 
  ||                                 convert all of cpcIn
  ||
  || Returns
  || -------
  || pcOut
  ||
  || Remarks
  || -------
  || pcOut should not be the same as cpcIn, the behaviour of this 
  ||   function is undefined in this case
  || 
  || pnInLen is set whether counting or actually converting.  Here's an
  ||   example call:
  ||
  ||  {
  ||      char *pcOut;
  ||      unsigned short cpcIn[] = { 0x0041, 0x2262, 0x0391, 0x002E };
  || 
  ||      nInLen = 4; // 4 wide chars, call utf16_to_utf8 to count...
  ||      utf16_to_utf8(pcIn,&nInLen,0,&nOutLen);
  ||    
  ||      // alloc space..
  ||      pcOut = (char *)malloc(nOutLen);
  ||
  ||      nInLen = 4; // previous call to utf16_to_utf8 destroys nInLen
  ||
  ||      // call again to actually convert
  ||      utf16_to_utf8(pcIn,&nInLen,pcOut,&nOutLen);
  || 
  ||      // pcOut should now hold "0x41 0xE2 0x89 0xA2 0xCE 0x91 0x2E"
  ||  }
  ||
  ||
  ||  Conversion rules, where x's are the bits of each input char, 
  ||        and the MSB ends up in the first byte of output:
  ||  
  ||    UTF-16 range (hex)    UTF-8 octet sequence (binary)
  ||    0000 to 007F          0xxxxxxx
  ||    0080 to 07FF          110xxxxx 10xxxxxx
  ||    0800 to FFFF          1110xxxx 10xxxxxx 10xxxxxx
  || 
  ||
  */
char *utf16_to_utf8(unsigned short *cpcIn, int *pnInLen, 
                    char *pcOut, int *pnOutLen);


#include "AEEWeb.h" /* definition of WebFormField */

#define WEBUTIL_PAVPF_NOTERMINATE 0x04
#define WEBUTIL_PAVPF_NOTRIM      0x08

/*
  || A WebNVPair is a structure representing an attribute-value pair, 
  ||  usually parsed out of a string of the form "attr=value".
*/
typedef WebFormField WebNVPair;

/**
  || Function
  || --------
  || int ParseNVPairs(char **ppszIn, const char *pszDelims, char cSep,
  ||                  uint32 uFlags, WebNVPair *awavp, int nMaxPairs)
  ||
  || Description
  || -----------
  || Parses a string of attr-value pairs with the pseudo-BNF:
  || 
  ||   av-pairs      =   av-pair *(<delim char> av-pair)
  ||   av-pair       =   attr [<sep char> value]
  ||   attr          =   token
  ||   value         =   token | quoted-string
  ||   token         =   1*<any CHAR except CTLs, <sep char> or <delim char>>
  ||   quoted-string =   ( <"> *(qdtext | quoted-pair ) <"> )
  ||   qdtext        =   <any TEXT except <">>
  ||   quoted-pair   =   "\" CHAR
  ||
  || Parameters
  || ----------
  || char **ppszIn: [in/out] pointer to pointer to string to parse,
  ||                         on return, is set to the next attr val pair
  || const char *cpszDelims: null-terminated list of characters to use as
  ||                          av-pair delimiters (list of "delim chars")
  ||                          above
  || char cSep: character to use to separate name from value
  || uint32 uFlags: special behaviours, logical-OR of the following:
  ||     WEBUTIL_PAVPF_NOTERMINATE: don't terminate the strings, just 
  ||                                initialize awavp members to point
  ||                                to the start of each part
  ||     WEBUTIL_PAVPF_NOTRIM: don't trim whitespace from names or
  ||                             values
  || WebNVPair *awavp: [out] array of WebNVPairs to fill
  || int nNumFields: number of elements in the array awavp
  ||
  || Returns
  || -------
  || the number of successfully parsed av-pairs fields, always less than or
  ||  equal to nMaxPairs
  ||
  || Remarks
  || -------
  || the sep char must not be a member of the list of delim chars, 
  ||   the behaviour of this function is undefined if this constraint
  ||   is violated
  ||
  */
int ParseNVPairs(char **ppszIn, const char *cpszDelims, char cSep,
                 uint32 uFlags, WebNVPair *awavp, int nMaxPairs);


/**
  || Function
  || --------
  || int IWebUtil_GuessUrl(IWebUtil *me, const char *cpszUrl, char *pBuf, int nLen);
  ||
  || Description
  || -----------
  ||
  || Guess scheme from format of given URL and prepend. 
  || Write result to buffer.
  ||
  */
int IWebUtil_GuessUrl(IWebUtil *me, const char *cpszUrl, char *pBuf, int nLen);


/**
  || Function
  || --------
  || int IWebUtil_GetFormFieldsV(IWebUtil *me, char *pszFormFields,
  ||                             const char **cpszzFieldNames, va_list args);
  || 
  || int IWebUtil_GetFormFields(IWebUtil *me, char *pszFormFields,
  ||                            const char **cpszzFieldNames, ...);
  ||
  || Description
  || -----------
  || parse out and fill char * pointers with form field values
  ||
  || Parameters
  || ----------
  || IWebUtil *me: the webutil
  || char *pszFormFields: [input] the form fields to be parsed.  This string
  ||                      is *destroyed* by parsing (i.e. chopped into 
  ||                      name-value pairs with nulls in between
  || const char *cpszFieldNames: a doubly-null-terminated list of field 
  ||                             names,e.g. "user\0pass\0"
  || ... or args: a variable length list of char * pointers to be filled 
  ||              with pointers into pszFormFields
  ||
  || Returns
  || -------
  || the number of fields successfully parsed out and matched
  ||
  || Remarks
  || -------
  || Here's an example use, where pszFormFields should look like:
  ||  " ... &user=x&pass=y& ... "
  ||
  || {
  ||    int nParsed;
  ||
  ||    nParsed = IWebUtil_GetFormFields(piWebUtil,pszFormFields,
  ||                                     "user\0pass\0",
  ||                                     &pszUser,&pszPass);
  ||    if (nParsed != 2) {
  ||       return EFAILED;
  ||    }
  ||    // now:
  ||    //    pszUser == "x"
  ||    //    pszPass == "y"
  || }
  || 
  || 
  ||
  */
int IWebUtil_GetFormFieldsV(IWebUtil *me, char *pszFormFields,
                            const char *cpszzFieldNames, va_list args);
int IWebUtil_GetFormFields(IWebUtil *me, char *pszFormFields,
                           const char *cpszzFieldNames, ...);

/*
  ||
  || BufBound
  ||
  ||  for easy writing..
  ||
*/
typedef struct BufBound
{
   char *pcWrite;
   char *pcEnd;
} BufBound;

/**
  || Function
  || --------
  || void BufBound_Init(BufBound *me, char *pBuf, int nLen);
  ||
  || Description
  || -----------
  || initializes a BufBound with pBuf and nLen
  ||
  || Parameters
  || ----------
  || BufBound *me: the bufbound
  || char *pBuf: the buffer we'll be writing to, which may be NULL
  || int nLen: the size of pBuf, if pBuf is NULL, this parameter is ignored
  || 
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
void BufBound_Init(BufBound *me, char *pBuf, int nLen);

void BufBound_Write(BufBound *me, const char *pc, int nLen);
void BufBound_Putc(BufBound *me, char c);

/**
  || Function
  || --------
  || void BufBound_Puts(BufBound *me, const char *psz);
  ||
  || Description
  || -----------
  || append a string to a BufBound
  ||
  || Parameters
  || ----------
  || BufBound *me: keeps track of a bounded buf
  || const char *psz: null-terminated string to append
  ||
  || Returns
  || -------
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
void BufBound_Puts(BufBound *me, const char *psz);

#define BUFBOUND_ISCOUNTER(pbb)           ((char *)0 == (pbb)->pcEnd)
#define BUFBOUND_LEFT(pbb)                ((pbb)->pcEnd - (pbb)->pcWrite)
#define BUFBOUND_REALLYWROTE(pbb, pStart) (MIN((pbb)->pcEnd,(pbb)->pcWrite)-(pStart))
#define BUFBOUND_WROTE(pbb, pStart)       ((pbb)->pcWrite-(pStart))
#define BUFBOUND_FULL(pbb)                 (BUFBOUND_LEFT(pbb) < 1)


/**
  || Function
  || --------
  || int err_strdup(char *pszSrc, char **pp);
  || int err_realloc(int nLen, void **pp);
  ||
  || Description
  || -----------
  || error-returning memory routines
  ||
  || Parameters
  || ----------
  || int nLen or char *pszSrc: how to allocate memory, string to copy or 
  ||                             new length of pointer
  || void **pp: place to store result
  ||
  || Returns
  || -------
  || SUCCESS on success, other AEE error value otherwise (usually ENOMEMORY)
  ||
  || Remarks
  || -------
  || pp is unmolested on realloc() failure
  || pp is set to 0 on strdup() failure or on strdup(0)
  || strup(0) returns SUCCESS
  ||
  */
int err_strdup(const char *cpszSrc, char**ppszDup);
int err_realloc(int nLen, void **pp);

#define ERR_MALLOCREC(t,pp)       ERR_MALLOC(sizeof(t),(pp))
#define ERR_MALLOCREC_EX(t,n,pp)  ERR_MALLOC(sizeof(t)+n,(pp))
#define ERR_REALLOC(nlen,pp)      err_realloc(nlen,(void **)(pp))
#define ERR_MALLOC(nlen,pp)       (*(pp)=0,err_realloc(nlen,(void **)(pp)))
#define ERR_STRDUP(ps,pp)         err_strdup((ps),(pp))

typedef struct FlatDB 
{
   IFile *pf;
   char  *pcData;
   int    cbData;

   // current entry:
   int    posRec;
   int    cbRec;
} FlatDB;



void         FlatDB_Dtor(FlatDB *me);
int          FlatDB_Ctor(FlatDB *me, IShell *psh, const char *pszName);
const char * FlatDB_NextRecord(FlatDB *me);
void         FlatDB_Truncate(FlatDB *me, int nMaxBytes);
boolean      FlatDB_ReplaceRecord(FlatDB *me, const char *cpszReplace);
void         FlatDB_Rewind(FlatDB *me);

/**
  || Function
  || --------
  || uint32 WebDate_Parse(char *psz);
  ||
  || Description
  || -----------
  || All HTTP date/time stamps MUST be represented in Greenwich Mean Time 
  || (GMT), without exception. For the purposes of HTTP, GMT is exactly 
  || equal to UTC (Coordinated Universal Time). This is indicated in the 
  || first two formats by the inclusion of "GMT" as the three-letter 
  || abbreviation for time zone, and MUST be assumed when reading the 
  || asctime format. HTTP-date is case sensitive and MUST NOT include 
  || additional LWS beyond that specifically included as SP in the 
  || grammar. 
  || 
  ||        HTTP-date    = rfc1123-date | rfc850-date | asctime-date
  ||        rfc1123-date = wkday "," SP date1 SP time SP "GMT"
  ||        rfc850-date  = weekday "," SP date2 SP time SP "GMT"
  ||        asctime-date = wkday SP date3 SP time SP 4DIGIT
  ||        date1        = 2DIGIT SP month SP 4DIGIT
  ||                       ; day month year (e.g., 02 Jun 1982)
  ||        date2        = 2DIGIT "-" month "-" 2DIGIT
  ||                       ; day-month-year (e.g., 02-Jun-82)
  ||        date3        = month SP ( 2DIGIT | ( SP 1DIGIT ))
  ||                       ; month day (e.g., Jun  2)
  ||        time         = 2DIGIT ":" 2DIGIT ":" 2DIGIT
  ||                       ; 00:00:00 - 23:59:59
  ||        wkday        = "Mon" | "Tue" | "Wed"
  ||                     | "Thu" | "Fri" | "Sat" | "Sun"
  ||        weekday      = "Monday" | "Tuesday" | "Wednesday"
  ||                     | "Thursday" | "Friday" | "Saturday" | "Sunday"
  ||        month        = "Jan" | "Feb" | "Mar" | "Apr"
  ||                     | "May" | "Jun" | "Jul" | "Aug"
  ||                     | "Sep" | "Oct" | "Nov" | "Dec"
  || 
  || Note: HTTP requirements for the date/time stamp format apply only
  || to their usage within the protocol stream. Clients and servers are
  || not required to use these formats for user presentation, request
  || logging, etc.
  || 
  || This function parses a string of the above format into CDMA time, 
  || in seconds.
  ||
  || Parameters
  || ----------
  || char *psz: the string
  ||
  || Returns
  || -------
  || a CMDA seconds time, or -1 on error
  ||
  || Remarks
  || -------
  || this function destroys the string
  ||
  */
uint32 WebDate_Parse(char *psz);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif /* #ifndef UTIL_H */

