/*
  ===========================================================================

  FILE:  util.c
  
  SERVICES:  
    Utility code for brewser
  
  GENERAL DESCRIPTION:
    Really simple stuff to share, 
  
  REVISION HISTORY: 
    Fri Aug 23 09:49:12 2002

  ===========================================================================
  ===========================================================================
    
               Copyright © 2002 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ===========================================================================
  ===========================================================================
*/
#include "AEEStdLib.h"     /* for STRLEN, etc */

#include "util.h" /* for my definitions */

#define ISDIGIT(c)        ((unsigned)((c) - '0') < 10)
#define ISOCTDIGIT(c)     (((c) >= '0') && ((c) <= '7'))
#define ISHEXDIGIT(c)     (ISDIGIT(c)||((((c)|32) >= 'a')&&(((c)|32) <= 'f')))
#define HEXVAL(c)         (ISDIGIT(c)?((c)-'0'):(((c)|32)-'a'+10))

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
  || nothing
  ||
  || Remarks
  || -------
  ||
  */
uint32 ReleaseIf(IBase **ppif)
{
   uint32 uRefs;
   if ((IBase *)0 != *ppif) {
      uRefs = IBASE_Release(*ppif);
      *ppif = (IBase *)0;
   } else {
      uRefs = -1;
   }
   return uRefs;
}

boolean parseuint32(const char *cpsz, int nRadix, uint32 *pnNum)
{
   char *pc;
   
   *pnNum = STRTOUL(cpsz,&pc,nRadix);

   return (pc != cpsz) && ('\0' == *pc);
}

boolean parseuint16(const char *cpsz, int nRadix, uint16 *pnNum)
{
   char *pc;
   
   *pnNum = (uint16)STRTOUL(cpsz,&pc,nRadix);

   return (pc != cpsz) && ('\0' == *pc);
}

/**
  || Function
  || --------
  || static char *htmlquote(const char *pcIn, int *pnInLen, char *pcOut, 
  ||                        int *pnOutLen)
  ||
  || Description
  || -----------
  || quote strings for safe inclusion in HTML
  ||
  || Parameters
  || ----------
  || const char *pcIn: input
  || int *pnInLen: [in/out] on input is length of pcIn (in bytes), 
  ||                        on output is number of bytes consumed
  || char *pcOut: destination buffer, may null for measuring
  || int *pnOutLen: [in/out] on input is length of pcOut
  ||                         on output is number of bytes written to pcOut
  ||
  || Returns
  || -------
  || pcOut
  ||
  || Remarks
  || -------
  ||
  */
static char *htmlquote(const char *pcIn, int *pnInLen, char *pcOut, 
                       int *pnOutLen)
{
   const char *pc;
   int         nInLen = *pnInLen;
   int         nOutLen;

#define ISHTMLQUOTE(c) ('<' == (c) || '&' == (c) || '\'' == (c) || '"' == (c))
   /* how much space do we need? */
   for (pc = (pcIn + nInLen - 1), nOutLen = 0; pc >= pcIn; nOutLen++, pc--) {
      if (ISHTMLQUOTE(*pc)) {
         nOutLen += 5; /* five more ("&#xxx;") */ 
      }
   }

   /* no output buffer, just return space needed */
   if ((char *)0 == pcOut) {
      goto done;
   }

   /* more space or more data? */
   nOutLen = MIN(nOutLen,*pnOutLen);
   
   {
      int i;
      const char *pEnd = pcIn + nInLen;
      /* how much can we encode, assuming nOutLen... */
      for (i = nOutLen, pc = pcIn; i > 0 && pc < pEnd; pc++) {
         if (ISHTMLQUOTE(*pc)) {
            if (i >= 6) {
               i -= 6;
            } else {
               break;
            }
         } else {
            i--;
         }
      }

      /* only encode this much...*/
      nInLen = MIN(nInLen, pc - pcIn);
   }
   /* return how much we encoded */
   *pnInLen = nInLen;

   /* to encode in place */
   MEMMOVE(pcOut + nOutLen - nInLen, pcIn, nInLen);

   /* finally do work */
   for (pc = pcOut + nOutLen - nInLen, nOutLen = 0; nInLen > 0;
        pc++, nInLen--) {
      unsigned char c = *pc;
      if (ISHTMLQUOTE(c)) {
         SPRINTF(pcOut+nOutLen,"&#%03d",c);
         nOutLen += 5;
         pcOut[nOutLen++] = ';'; /* overwrite null term with ';' */
      } else {
         pcOut[nOutLen++] = c;
      }
   }

 done:
   *pnOutLen = nOutLen;

   return pcOut;
}

/**
  || Function
  || --------
  || int vsnxprintf(char *pszBuf, int nBufLen, const char *cpszFormat,
  ||               const char pcDelims[2], 
  ||               const char *cpszzTokFormats, va_list args)
  || int snxprintf(char *pszBuf, int nBufLen, const char *cpszFormat,
  ||               const char pcDelims[2], const char *cpszzTokFormats, ...)
  ||
  || Description
  || -----------
  || replace tokens in pszBuf with sprintf'd strings
  ||
  || Parameters
  || ----------
  || char *pszBuf: buffer to do replacement on
  || int nBufLen: size, in bytes of pszBuf
  || const char *cpszTemplatet: string to use as xprintf format string
  ||                              (has tokens to be replaced)
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
               const char acDelims[2], const char *cpszzTokFormats, va_list args)
{
   int   nNeeded = (STRLEN(cpszTemplate)+1) - nBufLen;
   char *pszToken;
   int   nTokLen;
   
   STRLCPY(pszBuf,cpszTemplate,nBufLen);
   
   for (pszToken = (nBufLen != 0) ? pszBuf : (char *)cpszTemplate;
        (char *)0 != (pszToken = STRCHR(pszToken,acDelims[0]));
        pszToken += nTokLen) {
      const char *cpszFormat;
      int         nReplNum;
      int         nReplLen;
     
      {
         char *p;

         if ((char *)0 == (p = STRCHR(pszToken,acDelims[1]))) {
            break;
         }
         
         nTokLen = p - pszToken + 1;
      }

      /* nothing in between? */
      if (nTokLen == 2) {
         continue;
      }

      {
         const char *cp;
         int         ncpLen;

         cpszFormat = "";
         /* does this match something in our token list? */
         for (nReplNum = 0, cp = cpszzTokFormats; 
              '\0' != *cp;
              cp += ncpLen + STRLEN(cpszFormat) + 1, nReplNum++) {

            cpszFormat = STRCHR(cp,'%');
            
            if ((char *)0 == cpszFormat) {
               return -EBADPARM;
            }
            ncpLen = cpszFormat - cp;
            
            if ((ncpLen == (nTokLen - 2)) &&
                !MEMCMP(cp,pszToken+1,ncpLen)) {
               break;
            }
         }
         if ('\0' == *cp) {
            nTokLen = 1; /* just skip acDelims[0], not the whole token */
            continue;
         }
      }

      /* calculate space needed, SNPRINTF return value includes space
         for null termination */
      if (!STRCMP("%Qs",cpszFormat)) { 
         const char *pcIn = (const char *)va_argat(args,nReplNum);
         int         nInLen = STRLEN(pcIn);

         htmlquote(pcIn,&nInLen,0,&nReplLen);
      } else {
         nReplLen = SNPRINTF(0,0,cpszFormat,va_argat(args,nReplNum)) - 1;
      }

      /* simplistic, conservative computation of space needed */
      if ((nReplLen - nTokLen) > 0) {
         nNeeded += (nReplLen - nTokLen);
      }
      
      if ((0 != nBufLen) && (0 >= nNeeded)) {
         /* remember length of "rest" */
         int nRestLen = STRLEN(pszToken+nTokLen);

         /* move remainder of string out to the end of the buffer,
            but don't move the null-termination, because SPRINTF needs
            the space */
         MEMMOVE(pszToken+nReplLen+1,pszToken+nTokLen,nRestLen);

         if (!STRCMP("%Qs",cpszFormat)) { 
            const char *pcIn = (const char *)va_argat(args,nReplNum);
            int         nInLen = STRLEN(pcIn);
            
            htmlquote(pcIn,&nInLen,pszToken,&nReplLen);
         } else {
            SNPRINTF(pszToken,nReplLen+1,cpszFormat,va_argat(args,nReplNum));
         }

         /* move our pointer up to end of the replaced text */
         pszToken += nReplLen;

         /* move remainder of string back by one, and re-introduce
            the null character on the end */
         MEMMOVE(pszToken,pszToken+1,nRestLen);
         pszToken[nRestLen] = '\0';

         /* let loop run, we've already updated pszToken */
         nTokLen = 0;
      }
   }

   return MAX(0,nNeeded);
}

int snxprintf(char *pszBuf, int nBufLen, const char *cpszTemplate,
              const char acDelims[2],
              const char *cpszzFormats, ...)
{
   int nret;
   va_list args;

   va_start(args, cpszzFormats);
   nret = vsnxprintf(pszBuf,nBufLen,cpszTemplate,acDelims,cpszzFormats,args);
   va_end(args);
   return nret;
}

char *basename(const char *cpszFile)
{
   const char *cpsz;
   if (((char *)0 != (cpsz = STRRCHR(cpszFile,'/'))) ||
       ((char *)0 != (cpsz = STRRCHR(cpszFile,'\\')))) {
      cpszFile = cpsz+1;
   }
   return (char *)cpszFile;
}

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
int dequote(char *pc, int nLen)
{
   if (-1 == nLen) {
      nLen = STRLEN(pc);
   }

   /* might not be quoted */
   if ('"' == pc[0]) {
      int i;
      int bBackSlash = FALSE;

      nLen--;
      MEMMOVE(pc,pc+1,nLen);
      
      for (i = 0; i < nLen; i++) {
         char c = pc[i];

         if (bBackSlash) {
            int nWhackLen = 1;

            bBackSlash = FALSE;

            switch (c) {
            case '"':
               break;

            case '\\':
               break;

            case 'n':
               c = '\n';
               break;
               
            case 'r':
               c = '\r';
               break;

            case 't':
               c = '\t';
               break;
                  
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
               c = c - '0';
               if (ISOCTDIGIT(pc[i+1])) {
                  c = (c * 8) + (pc[i+1] - '0');
                  if (ISOCTDIGIT(pc[i+2])) {
                     c = (c * 8) + (pc[i+2] - '0');
                     nWhackLen++;
                     i++;
                  }
                  nWhackLen++;
                  i++;
               }
               break;

            case 'x':
               if (ISHEXDIGIT(pc[i+1])) {
                  c = HEXVAL(pc[i+1]);
                  if (ISHEXDIGIT(pc[i+2])) {
                     c = (c * 16) + HEXVAL(pc[i+2]);
                     nWhackLen++;
                     i++;
                  }
                  nWhackLen++;
                  i++;
                  break;
               }

            default: 
               /* go ahead and whack the '\', dunno how to warn from here... */
               break;
            }
         
            pc[i] = c; /* might've changed c */
            MEMMOVE(pc+i-nWhackLen,pc+i,nLen-i);
            nLen -= nWhackLen;
            i -= nWhackLen; /* compensates for loop i++ */

         } else {
            if ('"' == c) {
               nLen = i; /* done! */
            } else if ('\\' == c) {
               bBackSlash = TRUE;
            }
         }
      }
   }

   return nLen;
}

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
int trim(char *pc, int nLen)
{
   char *pcSave = pc;
   char  c;
      
   if (-1 == nLen) {
      nLen = STRLEN(pc);
   }

   while ((0 < nLen) && 
          ((' ' == (c = *pc)) || 
           ('\t' == c) ||
           ('\r' == c) ||
           ('\n' == c))) {
      pc++;
      nLen--;
   }
   
   MEMMOVE(pcSave,pc,nLen);
   pc = pcSave;
   
   while ((0 < nLen) && 
          ((' ' == (c = pc[nLen-1])) || 
           ('\t' == c) ||
           ('\r' == c) ||
           ('\n' == c))) {
      pc[--nLen] = '\0';
   }

   return nLen;
}

/**
  || Function
  || --------
  || char *qstrchrsend(const char *cpszHaystack, const char *cpszNeedles)
  ||
  || Description
  || -----------
  || find the first occurence of any of cpszNeedles chars in cpszHaystack,
  ||  ignoring double-quoted chars.  So:
  ||
  ||  qstrchrsend("hi\";\";",";") returns 5, not 3
  ||
  || Parameters
  || ----------
  || const char *cpszHaystack: string to search
  || const char *cpszNeedles: chars to search for
  ||
  || Returns
  || -------
  || pointer to first occurrence, or pointer to end of string
  ||
  || Remarks
  || -------
  ||
  */
static char *qstrchrsend(const char *cpszHaystack, const char *cpszNeedles)
{
   char cSrch;
   const char *cpcSrch;
   boolean bInQuotes = FALSE;
   
   for (cpcSrch = cpszHaystack; '\0' != (cSrch = *cpcSrch); cpcSrch++) {
      if (!bInQuotes) {
         if ('"' == cSrch) {
            bInQuotes = TRUE;
         } else {
            const char *cpc;
            char        c;

            for (cpc = cpszNeedles; 
                 ((c = *cpc) != cSrch) && (c != '\0'); 
                 cpc++);

            if (cSrch == c) {
               break;
            }
         }
      } else if (('"' == cSrch) &&  /* found end quote */
                 (('\\' != cpcSrch[-1]) ||  /* either not escaped */
                  (cpcSrch - cpszHaystack > 3) && /* or escape is escaped */
                  ('\\' == cpcSrch[-2]))) {
         bInQuotes = FALSE;
      }
   }

   return (char *)cpcSrch;

}

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
                    char *pcOut, int *pnOutLen)
{
   int nInLen = *pnInLen;
   int nOutLen;
   const unsigned short *pc;
   
   /* how much space do we need? */
   for (pc = (cpcIn+nInLen-1), nOutLen = 0; pc >= cpcIn; pc--) {
      if (*pc > 0x007f) {
         nOutLen++;
         if (*pc > 0x07ff) {
            nOutLen++;
         }
      }
      nOutLen++;
   }

   /* if output buffer, do work */
   if ((char *)0 != pcOut) {

      /* if more data than space */
      if (nOutLen > *pnOutLen) {
         const unsigned short *pcEnd = cpcIn + nInLen;

         nOutLen = *pnOutLen;

         /* how much can we convert? */
         for (pc = cpcIn; pc < pcEnd; pc++) {
            int nNeed = 1;
            if (*pc > 0x007f) {
               nNeed++;
               if (*pc > 0x07ff) {
                  nNeed++;
               }
            }
            /* only increment if we can do so for the whole char */
            if (nOutLen < nNeed) {
               break;
            }
            nOutLen -= nNeed;
         }

         /* only convert this much, possibly pare nInLen... */
         nInLen = MIN(nInLen, pc - cpcIn);
      }
      
      /* "return" how much we'll convert */
      *pnInLen = nInLen;
      
      /* finally, do work */
      for (pc = cpcIn, nOutLen = 0; nInLen > 0; pc++, nInLen--) {
         unsigned short c = *pc;

         if (c < 0x0080) {
            pcOut[nOutLen++] = (char)c;
         } else {
            if (c > 0x07ff) {
               pcOut[nOutLen++] = (char)(0xE0|(c>>12));
               pcOut[nOutLen++] = (char)(0x80|((c>>6)&0x3F));
            } else {
               pcOut[nOutLen++] = (char)(0xC0|(c>>6));
            }
            pcOut[nOutLen++] = (char)(0x80|(c&0x3F));
         }
      }
   } /* else no output buffer, just return space needed */
   
   *pnOutLen = nOutLen;

   return pcOut;
}

// Expand UTF-8 to Unicode
//
static unsigned short *utf8_to_utf16(char *cpcIn, int *pnInLen, 
                                     unsigned short *pwcOut, int *pnOutLen)
{
   int nInLen = *pnInLen;
   int nOutLen;

   /* how much space do we need? */
   {
      const char *pcEnd = cpcIn+nInLen;

      for (nOutLen = 0; 0 != nInLen; nInLen--, nOutLen++) {
         unsigned c = pcEnd[-nInLen]; /* have to consume forwards, 
                                         even though we count down */
         if (c >= 0xC0) {
            if ((c & 0x20) == 0) {
               if (nInLen <= 1) { /* eats 2 bytes */
                  break;
               }
               nInLen--; 
            } else if ((c & 0x10) == 0) {
               if (nInLen <= 2) { /* eats 3 bytes */
                  break;
               }
               nInLen-=2; 
            } else if ((c & 0x08) == 0) {
               if (nInLen <= 3) { /* eats 4 bytes */
                  break;
               }
               nInLen-=3; 
               nOutLen++; /* takes 2 words to encode this sucker */
            }
         }
      }
   }

   /* nOutLen is set to what it'll take */

   if ((unsigned short *)0 != pwcOut) {
      int   i;
      char *pc;
      
      /* consume minimum of what we need and what we're passed */
      nOutLen = MIN(nOutLen,*pnOutLen);

      for (pc = cpcIn, i = 0; i < nOutLen; pc++, i++) {
         unsigned c = *pc;
         
         if (c >= 0xC0) {
            if ((c & 0x20) == 0) {
               
               /* 2 bytes (11 bits) */
               c = (c & 0x1f) << 6;
               c |= *++pc & 0x3F;
               
            } else if ((c & 0x10) == 0) {

               /* 3 bytes (16 bits) */
               
               c = (c & 0x0f) << 12;
               c |= (*++pc & 0x3F) << 6;
               c |= (*++pc & 0x3F);

            } else if ((c & 0x08) == 0) {
               if (i >= nOutLen-1) { /* no room, takes 2 words 
                                          to encode this sucker */
                  break;
               }

               /* 4 bytes (21 bits) */
               c = (c & 0x07) << 18;
               c |= (*++pc & 0x3F) << 12;
               c |= (*++pc & 0x3F) << 6;
               c |= (*++pc & 0x3F);
            }
            
            if (c >= 0x10000) {
               // Output high-order surrgate character & compute low-order character
               //   High Surrogates: U+D800..U+DBFF
               //   Low Surrogates:  U+DC00..U+DFFF
               //
               // Note that 4-byte UTF-8 represents 21 bits, while 2-word UTF-16
               // supports just over a 20 bit range (00000 ... 10FFFF).
               
               pwcOut[i] = (AECHAR) (0xD7c0 + ((c >> 10) & 0x3FF));
               c = (c & 0x3FF) | 0xDC00;
            }
         }

         pwcOut[i] = (unsigned short)c;
      }
      nOutLen = i;
      *pnInLen = pc - cpcIn;
   }

   *pnOutLen = nOutLen;

   return pwcOut;
}



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
  ||   token         =   1*<any CHAR except CTLs, <sep char> or <delim char> >
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
                 uint32 uFlags, WebNVPair *awavp, int nMaxPairs)
{
   char    *psz;
   int      nNumPairs;
   boolean  bTerminate = !(uFlags & WEBUTIL_PAVPF_NOTERMINATE);
   boolean  bTrim      = !(uFlags & WEBUTIL_PAVPF_NOTRIM);
   
   for (psz = *ppszIn, nNumPairs = 0; 
        (0 != *psz) && (nNumPairs < nMaxPairs); ) {
      WebNVPair wavp;
      int       nNameLen, nValueLen;

      /* find the name, value, and end of the field */
      wavp.pcName  = psz;
      psz          = qstrchrsend(psz,cpszDelims);
      wavp.pcValue = MEMCHREND(wavp.pcName,cSep,psz - wavp.pcName);
      
      /* get namelen and valuelen before poking termination */
      nValueLen = psz - wavp.pcValue;
      nNameLen  = wavp.pcValue - wavp.pcName;

      /* poke in null termination */
      if ('\0' != *psz) {
         if (bTerminate) {
            *psz = 0;
         }
         psz++;
      }
      if (cSep == *wavp.pcValue) {
         if (bTerminate) {
            *wavp.pcValue = 0;
         }
         wavp.pcValue++;
         nValueLen--;
      }
      
      /* decode in place, null terminate again... */
      if (bTrim) {
         nNameLen = trim(wavp.pcName,nNameLen);
         nValueLen = trim(wavp.pcValue,nValueLen);
      }
      
      if (bTerminate) {
         wavp.pcName[nNameLen] = 0;
         wavp.pcValue[nValueLen] = 0;
      }

      /* check to see if we found a real field (not just a separator) */
      if ((0 != nValueLen) || 
          (0 != nNameLen) || 
          (wavp.pcValue != wavp.pcName)) {
         
         if ((WebNVPair *)0 != awavp) {
            MEMMOVE(awavp,&wavp,sizeof(wavp));
            awavp++;
         }

         nNumPairs++;
      }
   }
      
   *ppszIn = psz;
   
   return nNumPairs;
}


int IWebUtil_GuessUrl(IWebUtil *me,const char *cpszUrl,char *pBuf,int nLen)
{
   UrlParts  up;
   BufBound  bb;

   IWEBUTIL_ParseUrl(me,cpszUrl,&up);

   BufBound_Init(&bb, pBuf, nLen);

   if (!UP_HASSCHM(&up)) {
      if (!UP_HASAUTH(&up)) {
         BufBound_Puts(&bb,"http://");
         if (cpszUrl[0] == '/') {
            cpszUrl++;
            IWEBUTIL_ParseUrl(me,cpszUrl,&up);
         }
         /* damnit, now we have to basically re-parse the URL */

         up.cpcPath = STRCHREND(up.cpcUser, '/');
         
         if ('\0' == *up.cpcPath) {
            up.cpcPath = STRCHREND(up.cpcUser, '?');
            
            if ('\0' == *up.cpcPath) {
               up.cpcPath = STRCHREND(up.cpcUser, '#');
            }
         }

         up.cpcHost = MEMRCHRBEGIN(up.cpcUser, '@', up.cpcPath-up.cpcUser);

         if (up.cpcHost[0] == '@') {
            up.cpcHost++;
         }
         
         up.cpcPass = MEMCHREND(up.cpcUser,':',up.cpcHost-up.cpcUser);
         
         up.cpcPort = MEMCHREND(up.cpcHost,':',up.cpcPath-up.cpcHost);

      } else {
         BufBound_Puts(&bb,"http:");
      }
   }

   /* not a fully-qualified hostname */
   if (UP_HASHOST(&up) && 
       ((char *)0 == MEMCHR(up.cpcHost,'.',UP_HOSTLEN(&up)))) {
      
      BufBound_Write(&bb,up.cpcSchm,up.cpcHost-up.cpcSchm);
      BufBound_Write(&bb,"www.",CSTRLEN("www."));
      BufBound_Write(&bb,up.cpcHost,UP_HOSTLEN(&up));
      BufBound_Write(&bb,".com",CSTRLEN(".com"));

      cpszUrl = up.cpcPort;
   }

   BufBound_Write(&bb,cpszUrl,STRLEN(cpszUrl)+1);
   
   return BUFBOUND_WROTE(&bb,pBuf);
}

int IWebUtil_GetFormFieldsV(IWebUtil *me, char *pszFormFields,
                            const char *cpszzFieldNames, va_list args)
{
   WebFormField  wff;
   int nRet = 0;
   
   while (IWEBUTIL_ParseFormFields(me,&pszFormFields,&wff,1,0)) {
      const char *cpszFieldName;
      int         i;
      
      for (i = 0, cpszFieldName = cpszzFieldNames; '\0' != cpszFieldName[0];
           cpszFieldName += STRLEN(cpszFieldName)+1, i++) {
         if (!STRCMP(wff.pcName,cpszFieldName)) {
            *(char **)(va_argat(args,i)) = wff.pcValue;
            nRet++;
         }
      }
   }
   return nRet;
}

int IWebUtil_GetFormFields(IWebUtil *me, char *pszFormFields,
                           const char *cpszzFieldNames, ...)
{
   va_list args;
   int     nRet;

   va_start(args,cpszzFieldNames);
   nRet = IWebUtil_GetFormFieldsV(me,pszFormFields,cpszzFieldNames,args);
   va_end(args);

   return nRet;
}

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
char *strchopped_nth(const char *cpsz, int nNth)
{
   for (;;) {
      while ('\0' == *cpsz) {
         cpsz++; /* skip nulls (including leading) */
      }
      if (nNth-- == 0) {
         return (char *)cpsz;
      }
      while ('\0' != *cpsz++); /* skip word */
   }
   /* not reached */
   //   return psz;
}

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
int strchop(char *pszToChop, const char *cpszChopChars)
{
   int   nLen;
   int   nNumFields = 0;
   char *pc;
   int   bOnWord = FALSE;

   if ((char *)0 == pszToChop) {
      return 0;
   }

   if ((char *)0 == cpszChopChars) {
      return 1;
   }

   nLen = STRLEN(pszToChop);
   pc = pszToChop + nLen;

   while (--pc >= pszToChop) {
      if (STRCHR(cpszChopChars,*pc)) {

         *pc = '\0';
         bOnWord = FALSE;

      } else {

         if (!bOnWord) {
            ++nNumFields;
         }
         bOnWord = TRUE;

      }
   }

   return nNumFields;
}

/**
  || Function
  || --------
  || static boolean strmxnstr(const char *cpszHaystack, 
  ||                          const char *cpszNeedle,
  ||                          int m, int *pn)
  ||
  || Description
  || -----------
  || find cpszNeedle in cpszHaystack, where cpszHaystack is a list of 
  ||  strings of equal length m, set *pn to the zero-based index where 
  ||  needle begins in haystack divided by exactly m.  Return TRUE if 
  ||  match found exactly
  ||
  || Parameters
  || ----------
  || const char *cpszHaystack: null-terminated list of potential matches
  || const char *cpszNeedle: string to find
  || int m: length of each potential match, cpszNeedle needn't be exactly 
  ||         m, but cpszNeedle must match cpszHaystack at an offset that
  ||         is a multiple of m
  || int *pn: output, which multiple of m cpszNeedle was found
  ||
  || Returns
  || -------
  || TRUE if a proper match was found
  ||
  || Remarks
  || -------
  ||
  */
static boolean strmxnstr(const char *cpszHaystack, const char *cpszNeedle,
                         int m, uint16 *pn)
{
   int nOffset;

   cpszNeedle = (const char *)STRSTR(cpszHaystack,cpszNeedle);

   nOffset = cpszNeedle-cpszHaystack;
   
   if ((uint16 *)0 != pn) {
      *pn = nOffset/m;
   }

   return ((char *)0 != cpszNeedle) && (0 == nOffset%m);
}

boolean parsemonth(const char *cpsz, uint16 *pnMonth)
{
   const char *cpszMonths = 
      "Jan"
      "Feb"
      "Mar"
      "Apr"
      "May"
      "Jun"
      "Jul"
      "Aug"
      "Sep"
      "Oct"
      "Nov"
      "Dec";
   
   if (3 != STRLEN(cpsz)) {
      return FALSE;
   }

   return strmxnstr(cpszMonths, cpsz, 3, pnMonth);
}

/* uncomment these and the checks in WebDate_Parse()
   if you want date parsing to be more rigorous */
//boolean parsewkday(const char *cpsz, uint16 *pnWkday)
//{
//   const char *cpszWkdays = 
//      "Sun"
//      "Mon"
//      "Tue"
//      "Wed"
//      "Thu"
//      "Fri"
//      "Sat";
//
//   if (3 != STRLEN(cpsz)) {
//      return FALSE;
//   }
//   
//   return strimxnstr(cpszWkdays,cpsz,3,pnWkday);
//}
//
//boolean parseweekday(const char *cpsz, uint16 *pnWeekday)
//{
//   const char *cpszWeekdays = 
//      "Sunday   "
//      "Monday   "
//      "Tuesday  "
//      "Wednesday"
//      "Thursday "
//      "Friday   "
//      "Saturday ";
//   
//   if (6 > STRLEN(cpsz) || (char *)0 != STRCHR(cpsz,' ')) {
//      return FALSE;
//   }
//   
//   return strimxnstr(cpszWeekdays,cpsz,9,pnWeekday);
//}


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
uint32 WebDate_Parse(char *pszIn)
{
   JulianType j;
   char       *pszYear;
   char       *pszMonth;
   char       *pszDay;
   char       *pszHour;
   char       *pszMinute;
   char       *pszSecond;

   ZEROAT(&j);

   /* all the formats have exactly 4 space-separated fields */
   switch (strchop(pszIn," ")) {
   case 4: /* parse rfc850 date */
      {
         /* char *pszWeekday = strchopped_nth(pszIn,0); */
         char *pszDate = strchopped_nth(pszIn,1);
         char *pszTime = strchopped_nth(pszIn,2);
      
         if (STRCMP(strchopped_nth(pszIn,3),"GMT") ||
             /* 
                You can uncomment these 2 tests out if you wanna check
                for the day being of the right form.  If you do, realize
                that there's no comma enforcement, and no requirement
                that day of week be right.

                (1 != strchop(pszWeekday,",")) ||
                !parseweekday(pszWeekday,0) ||
             */
      
             (3 != strchop(pszDate,"-")) ||
             (3 != strchop(pszTime,":"))) {
            return -1;
         }
         
         pszDay    = strchopped_nth(pszDate,0);
         pszMonth  = strchopped_nth(pszDate,1);
         pszYear   = strchopped_nth(pszDate,2);
         pszHour   = strchopped_nth(pszTime,0);
         pszMinute = strchopped_nth(pszTime,1);
         pszSecond = strchopped_nth(pszTime,2);
      }
      break;

   case 5: /* parse asctime */
      {
         /* char *pszWkday = strchopped_nth(pszIn,0); */
         char *pszTime = strchopped_nth(pszIn,3);

         pszMonth = strchopped_nth(pszIn,1);
         pszDay = strchopped_nth(pszIn,2);
         pszYear = strchopped_nth(pszIn,4);

         if (/* 
                You can uncomment these 2 tests out if you wanna check
                for the day being of the right form.  If you do, realize
                that there's no comma enforcement, and no requirement
                that day of week be right.
             (1 != strchop(pszWkday,",")) ||
             !parsewkday(pszWkday,0) ||
             */

             (3 != strchop(pszTime,":"))) {
            return -1;
         }
         pszHour   = strchopped_nth(pszTime,0);
         pszMinute = strchopped_nth(pszTime,1);
         pszSecond = strchopped_nth(pszTime,2);
      }
      break;

   case 6: /* parse rfc1123 date */
      {
         /* char *pszWkday = strchopped_nth(pszIn,0); */
         char *pszTime = strchopped_nth(pszIn,4);

         pszDay = strchopped_nth(pszIn,1);
         pszMonth = strchopped_nth(pszIn,2);
         pszYear = strchopped_nth(pszIn,3);

         if (STRCMP(strchopped_nth(pszIn,5),"GMT") ||
             /* 
                You can uncomment these 2 tests out if you wanna check
                for the day being of the right form.  If you do, realize
                that there's no comma enforcement, and no requirement
                that day of week be right.
             (1 != strchop(pszWkday,",")) ||
             !parsewkday(pszWkday,0) ||
             */
             (3 != strchop(pszTime,":"))) {
            return -1;
         }

         pszHour   = strchopped_nth(pszTime,0);
         pszMinute = strchopped_nth(pszTime,1);
         pszSecond = strchopped_nth(pszTime,2);
      }
      break;
      
   default:
      return -1;
   }

   if (!parseuint16(pszDay,10,&j.wDay) ||
       !parsemonth(pszMonth,&j.wMonth) ||
       !parseuint16(pszYear,10,&j.wYear) ||
       !parseuint16(pszYear,10,&j.wYear) ||
       !parseuint16(pszYear,10,&j.wYear) ||
       !parseuint16(pszYear,10,&j.wYear) ||
       !parseuint16(pszHour,10,&j.wHour) ||
       !parseuint16(pszMinute,10,&j.wMinute) ||
       !parseuint16(pszSecond,10,&j.wSecond)) {
      return -1;
   }

   /* all the above parsing uses 0-based index to month */
   j.wMonth += 1;
   
   /* 2-digit year? */
   if (j.wYear < 100) {
      j.wYear += (j.wYear>39?1900:2000);
   }


   {
      uint32 ulDate = JULIANTOSECONDS(&j);

      if (0 == ulDate) {
         ulDate = -1;
      }

      if (-1 != ulDate) {
         ulDate += LOCALTIMEOFFSET(0);
      }

      return ulDate;
   }
   
}


int IShell_LoadResBlob(IShell *piShell, const char *cpszFile, uint16 nResID, 
                       AEEResBlob **pprb, int *pnLen)
{
   uint32      nLen;
   AEEResBlob *prb = *pprb;

   if ((void *)-1 != ISHELL_LoadResDataEx(piShell, cpszFile, nResID,
                                          RESTYPE_IMAGE, (void*)-1, &nLen)) {
      return ERESOURCENOTFOUND;
   }
   
   if ((AEEResBlob *)0 == prb) {
      /* to include null-termination */
      prb = (AEEResBlob*)MALLOC(nLen+1);

      if ((AEEResBlob *)0 == prb) {
         return ENOMEMORY;
      }
   } else if (*pnLen < (int)(nLen+1)) {
      *pnLen = nLen+1;
      return EBADPARM;
   }
   
   if (prb != ISHELL_LoadResDataEx(piShell, cpszFile, nResID,
                                   RESTYPE_IMAGE, prb, &nLen)) {
      if ((AEEResBlob *)0 == *pprb) { 
         FREEIF(prb); 
      }
      return ERESOURCENOTFOUND;
   }

   *pprb = prb;
   *pnLen = (int)(nLen+1);

   return SUCCESS;
}



int AEEResBlob_VStrRepl(AEEResBlob **pprb, int *pnLen, 
                        const char *cpszzTokFormats, 
                        va_list args)
{
   AEEResBlob *prb = *pprb;
   int         nLen = *pnLen;
   int         nMore = 0;

   do {
      AEEResBlob *pNew;
      
      pNew = (AEEResBlob *)REALLOC(prb,nLen+nMore);
      
      if ((AEEResBlob *)0 == pNew) {
         break;
      }
      prb = pNew;
      nLen += nMore;
      
      nMore = vsnxprintf(RESBLOB_DATA(prb),nLen-prb->bDataOffset,
                         RESBLOB_DATA(prb),"{}",cpszzTokFormats,args);

   } while (nMore > 0);

   *pnLen = nLen;
   *pprb = prb;
   return nMore;
}


int AEEResBlob_StrRepl(AEEResBlob **pprb, int *pnLen, 
                       const char *cpszzTokFormats, ...)
{
   int     nMore;
   va_list args;

   va_start(args,cpszzTokFormats);
   nMore = AEEResBlob_VStrRepl(pprb,pnLen,cpszzTokFormats,args);
   va_end(args);

   return nMore;
}

int IShell_LoadResWsz(IShell *piShell,const char *cpszResFile,
                      uint16 uResId, AECHAR **ppwsz)
{
   AECHAR *pwsz;
   uint32  ulLen;
   int     nErr;
   
   *ppwsz = 0;
   
   if ((void *)-1 != ISHELL_LoadResDataEx(piShell,cpszResFile,uResId,
                                          RESTYPE_STRING,(void *)-1,&ulLen)) {
      return ERESOURCENOTFOUND;
   }

   nErr = ERR_MALLOC(ulLen,&pwsz);
   
   if (SUCCESS != nErr) {
      return nErr;
   }

   if (((void *)0 != ISHELL_LoadResDataEx(piShell,cpszResFile,uResId,
                                          RESTYPE_STRING,pwsz,&ulLen))) {

      switch ((unsigned char)pwsz[0]) { /* says encoding in strings */
      case AEE_ENC_UNICODE: /* utf-16, done */
         MEMMOVE(pwsz,pwsz+1,ulLen-sizeof(AECHAR));
         ulLen /= sizeof(AECHAR);
         pwsz[ulLen-1] = 0;
         *ppwsz = pwsz;
         pwsz = 0; /* so it won't be freed */
         break;

      case AEE_ENC_UTF8:
         {
            char           *pcIn;
            int             nInLen;
            unsigned short *pcOut;
            int             nOutLen;

            /* skip enc marker */
            pcIn = ((char *)pwsz)+1;
            ulLen--;

            nInLen = ulLen;
            utf8_to_utf16(pcIn,&nInLen,0,&nOutLen);

            nErr = ERR_MALLOC((nOutLen+1)*sizeof(AECHAR),&pcOut);
            DBGHEAPMARK(pcOut);
            
            if (SUCCESS == nErr) {
               nInLen = ulLen; /* re-set nInLen */
               utf8_to_utf16(pcIn,&nInLen,pcOut,&nOutLen);
               *ppwsz = pcOut;
               /* still have to free pwsz, leave it set */
            }
         }
         break;

      case AEE_ENC_S_JIS:
      case AEE_ENC_KSC5601:
      case AEE_ENC_EUC_CN:
         /* hope this actually accomodates the string, assume string 
            is the same encoding as the handset... */
         nErr = ERR_REALLOC(ulLen * 2,&pwsz);
         
         if (SUCCESS != nErr) {
            break;
         }
         
         STREXPAND(((byte *)pwsz)+2, ulLen-2, pwsz, ulLen * 2);
         *ppwsz = pwsz;
         pwsz = 0; /* so it won't be freed */
         break;
         
      case AEE_ENC_ISOLATIN1:
         {
            unsigned char *pc;

            /* double size to accomodate the string... */
            nErr = ERR_REALLOC(ulLen * 2,&pwsz);
         
            if (SUCCESS != nErr) {
               break;
            }

            pc = (unsigned char *)pwsz;
            MEMMOVE(pc,pc+1,ulLen-1); /* move the string down */
            pc[ulLen-1] = 0; /* null terminate */
            
            while (ulLen-- != 0) { /* expand from end */
               pwsz[ulLen] = pc[ulLen];
            }
         }
         *ppwsz = pwsz;
         pwsz = 0; /* so it won't be freed */
         break;
         
      default:
         nErr = EINVALIDFORMAT;
         break;
      }
   } else {
      nErr = ERESOURCENOTFOUND;
   }

   FREEIF(pwsz);
   return nErr;
}

int IShell_LoadResSz(IShell *piShell,const char *cpszResFile, 
                     uint16 uResId, char **ppsz)
{
   char   *psz;
   uint32  ulLen;
   int     nErr;
   
   *ppsz = 0;
   
   if ((void *)-1 != ISHELL_LoadResDataEx(piShell,cpszResFile,uResId,
                                          RESTYPE_STRING,(void *)-1,&ulLen)) {
      return ERESOURCENOTFOUND;
   }
   
   nErr = ERR_MALLOC(ulLen,&psz);
   
   if (SUCCESS != nErr) {
      return nErr;
   }

   if (((void *)0 != ISHELL_LoadResDataEx(piShell,cpszResFile,uResId,
                                          RESTYPE_STRING,psz,&ulLen))) {

      switch ((unsigned char)psz[0]) { /* says encoding in strings */
      case AEE_ENC_UNICODE: /* utf-16 must convert to utf-8! */
         {
            unsigned short *pcIn;
            int             nInLen;
            char           *pcOut;
            int             nOutLen;

            /* skip enc marker */
            pcIn = ((unsigned short *)psz)+1;
            ulLen = (ulLen/sizeof(unsigned short)) - 1;

            nInLen = ulLen;
            utf16_to_utf8(pcIn,&nInLen,0,&nOutLen);

            nErr = ERR_MALLOC(nOutLen+1,&pcOut);
            
            if (SUCCESS == nErr) {
               nInLen = ulLen; /* re-set nInLen */
               utf16_to_utf8(pcIn,&nInLen,pcOut,&nOutLen);
               *ppsz = pcOut;
               /* still have to free psz, leave it set */
            }
         }
         break;

      case AEE_ENC_S_JIS:
      case AEE_ENC_KSC5601:
      case AEE_ENC_EUC_CN:
         /* single-byte strings, with 2 marker bytes */
         MEMMOVE(psz,psz+2,ulLen-2);
         psz[ulLen-2] = 0;
         *ppsz = psz;
         psz = 0; /* so it won't be freed */
         nErr = SUCCESS;
         break;

      case AEE_ENC_UTF8:
      case AEE_ENC_ISOLATIN1:
         /* single-byte strings, with 2 marker bytes */
         MEMMOVE(psz,psz+1,ulLen-1);
         psz[ulLen-1] = 0;
         *ppsz = psz;
         psz = 0; /* so it won't be freed */
         nErr = SUCCESS;
         break;
         
      default:
         nErr = EINVALIDFORMAT;
         break;
      }
   } else {
      nErr = ERESOURCENOTFOUND;
   }

   FREEIF(psz);
   return nErr;
}

/*====================================================================
  bufbound.c stuff 
  ===================================================================*/

void BufBound_Init(BufBound *me, char *pBuf, int nLen)
{
   me->pcWrite = pBuf; 
   me->pcEnd   = ((char *)0 != pBuf) ? pBuf + nLen : 0;
}

void BufBound_Write(BufBound *me, const char *pc, int nLen)
{
   char *pcStop = MIN(me->pcEnd, me->pcWrite + nLen);
   char *pcWrite = me->pcWrite;

   while (pcWrite < pcStop) {
      *pcWrite++ = *pc++;
   }

   me->pcWrite += nLen;
}

void BufBound_Putc(BufBound *me, char c)
{
   if (me->pcWrite < me->pcEnd) {
      *me->pcWrite = c;
   }
   me->pcWrite++;
}

void BufBound_Puts(BufBound *me, const char *psz)
{
   BufBound_Write(me, psz, STRLEN(psz));
}

/*====================================================================
  err memory stuff 
  ===================================================================*/
int err_strdup(const char *pszSrc, char **pp)
{
   *pp = 0;

   if (((char *)0 == pszSrc) || 
       ((char *)0 != (*pp = STRDUP(pszSrc)))) {
      return SUCCESS;
   }
   return ENOMEMORY;
}

int err_realloc(int nLen, void **pp)
{
   void *p;

   p = REALLOC(*pp,nLen);   

   if ((((void *)0 != p) && (0 != nLen)) ||
       (((void *)0 == p) && (0 == nLen))) {
      *pp = p;
      return SUCCESS;
   }

   return ENOMEMORY;
}


// This works like IFILEMGR_OpenFile() *should* work.
//
//   m = one of (_OFM_READ | _OFM_READWRITE | _OFM_APPEND) 
//       plus optionally (_OFM_CREATE)
//
int IShell_OpenFile(IShell *psh, const char *pszName, 
                    AEEOpenFileMode m, IFile **ppf)
{
   IFileMgr        *pfm;
   IFile           *pf = 0;
   int              nErr;
   AEEOpenFileMode  mExists = (m & ~_OFM_CREATE);

   nErr = ISHELL_CreateInstance(psh, AEECLSID_FILEMGR, (void **)&pfm);

   if (SUCCESS == nErr) {
      do {
         // Try opening without creating
         pf = IFILEMGR_OpenFile(pfm, pszName, mExists);
         if ((IFile *)0 != pf) {
            break;
         }

         if (m != mExists) {
            // _OFM_CREATE will fail if the file already exists.
            pf = IFILEMGR_OpenFile(pfm, pszName, _OFM_CREATE);
         }
      
         if ((IFile *)0 == pf) {
            nErr = IFILEMGR_GetLastError(pfm);
            break;
         }

         if ((_OFM_READWRITE != mExists) && (_OFM_APPEND != mExists)) {
            // Close and re-open with the right flags  
            // (user may want READWRITE, not just WRITE)
            IFILE_Release(pf);
            pf = 0;
            nErr = SUCCESS;
            m = mExists; // don't try creating again
         }
      } while ((IFile *)0 == pf);

      if ((IFile *)0 != pf) {
         IFILE_SetCacheSize(pf,SCS_MAX);
      }

      IFILEMGR_Release(pfm);
   }

   *ppf = pf;

   return nErr;
}

/*====================================================================
  FlatDB
  ===================================================================*/


// Deconstruct FlatDB
//
// Frees resources.  Does not write any data to a file.
//
void FlatDB_Dtor(FlatDB *me)
{
   RELEASEIF(me->pf);
   FREEIF(me->pcData);
   ZEROAT(me);
}


// Construct FlatFB
//
// Returns SUCCESS or error code, but in any event a valid object is
// constructed and Dtor() must be called.  Other member functions may be
// called regardless of the return value.
//
// When initialized, there is no current record, and the "next" record is the
// first record.
//
int FlatDB_Ctor(FlatDB *me, IShell *psh, const char *pszName)
{
   AEEFileInfo fi;
   int         nErr;

   ZEROAT(me);

   nErr = IShell_OpenFile(psh, pszName, (_OFM_READWRITE|_OFM_CREATE), 
                          &me->pf);
   
   if (SUCCESS != nErr) {
      return nErr;
   }

   nErr = IFILE_GetInfo(me->pf, &fi);
   
   if (SUCCESS == nErr) {

      nErr = ERR_MALLOC(fi.dwSize + 1,&me->pcData);

      if (SUCCESS == nErr) {
         me->cbData = IFILE_Read(me->pf, me->pcData, fi.dwSize);
         if (me->cbData < 0) {
            nErr = EFAILED;
         }
      }
   }

   return nErr;
}

// Advance to the next record.
//
// Return value:
//    NULL = no more records
//    other = pointer to start of record; 'posRec' and 'cbRec' are updated.
//
const char *FlatDB_NextRecord(FlatDB *me)
{
   const char *pc;
   
   me->posRec += me->cbRec;

   if (me->posRec >= me->cbData) {
      me->cbRec = 0;
      return 0;
   }

   pc = me->pcData + me->posRec;
   me->cbRec = STRLEN(pc) + 1;
   return pc;
}


// Retain up to nMaxBytes of the rest of the file; discard the rest.
// 
// since posRec and cbRec may point past the truncated db, 
//   FlatDB_Truncate() does a FlatDB_Rewind()
//
void FlatDB_Truncate(FlatDB *me, int nMaxBytes)
{
   int cb = me->cbData;

   while ((cb > nMaxBytes) && (cb > 0)) {
      // remove one entry
      do {
         --cb;
      } while ((0 != cb) && ('\0' != me->pcData[cb-1]));
   }
   me->cbData = cb;

   FlatDB_Rewind(me);
}

// Replace current record with new record.
//
// Current record (if any) is deleted, and specified record (if non-NULL) is
// inserted at me->posRec.
//
// NOTE: This writes out a modified version of the database and leaves the
// FlatDB in an out-of-sync state.  Destruct and re-construct to read again.
//
boolean FlatDB_ReplaceRecord(FlatDB *me, const char *cpszInsert)
{
   int posTail = me->posRec + me->cbRec;
   int cbTail = me->cbData - posTail;
   int cbInsert = (cpszInsert ? STRLEN(cpszInsert) + 1 : 0);

   return ((IFILE_Seek(me->pf, _SEEK_START, me->posRec) == SUCCESS) &&
           (IFILE_Write(me->pf, cpszInsert, cbInsert) == (uint32)cbInsert) &&
           (IFILE_Write(me->pf, me->pcData + posTail, cbTail) == (uint32)cbTail) &&
           
           (IFILE_Truncate(me->pf, me->cbData + cbInsert - me->cbRec) == SUCCESS));
}


void FlatDB_Rewind(FlatDB *me)
{
   me->posRec = 0;
   me->cbRec = 0;
}

