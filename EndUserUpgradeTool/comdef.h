#ifndef COMDEF_H
#define COMDEF_H

typedef  unsigned char      byte;         /* Unsigned 8  bit value type. */
typedef  unsigned short int word;         /* Unsinged 16 bit value type. */
typedef  unsigned long  int dword;        /* Unsigned 32 bit value type. */
typedef struct 
{
  dword lo;
  dword hi;
} qword;
typedef  unsigned char     boolean;		 /* Boolean value type. */
typedef  unsigned char     uint1;        /* Unsigned 8  bit value type. */
typedef  unsigned short    uint2;        /* Unsigned 16 bit value type. */
typedef  unsigned long     uint4;        /* Unsigned 32 bit value type. */

typedef  signed char       int1;         /* Signed 8  bit value type. */
typedef  signed short      int2;         /* Signed 16 bit value type. */
typedef  long int          int4;         /* Signed 32 bit value type. */

typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
typedef  unsigned short     uint16;      /* Unsigned 16 bit value */
typedef  unsigned char      uint8;       /* Unsigned 8  bit value */
typedef struct
{
  uint32 lo;
  uint32 hi;
}uint64;

typedef  signed long int    int32;       /* Signed 32 bit value */
typedef  signed short       int16;       /* Signed 16 bit value */
typedef  signed char        int8;        /* Signed 8  bit value */

#endif  /* COMDEF_H */

