#ifndef _FLASHDEF_H_
#define _FLASHDEF_H_

typedef struct 
{
	DWORD beginaddr;						/*  area begin address							*/
	DWORD endaddr;							/*	area end address							*/
	WORD  blocks;							/*	area blocks									*/
}area_define_type;

#define	FLASH_BASE				0x00000000L	/*	flash base address							*/
#define BOOT_BEGINADDR			FLASH_BASE	/*  Boot area begin address						*/
#define BOOT_ENDADDR			0x10000L	/*  Boot area end address						*/
#define BOOT_BLOCK_64MBITS		8			/*  max boot block number for 64M flash			*/
#define BOOT_BLOCK_32MBITS		1			/*  max boot block number for 32M flash			*/
#define BOOT_BLOCK_16MBITS		1			/*  max boot block number for 16M flash			*/
#define APP_BEGINADDR			BOOT_ENDADDR/*	Application area begin address				*/
#define APP_ENDADDR_64MBITS		0x700000	/*  Application area end address for 64M flash	*/
#define APP_ENDADDR_32MBITS		0x380000	/*  Application area end address for 32M flash	*/
#define APP_ENDADDR_16MBITS		0x180000	/*  Application area end address for 16M flash	*/
#define APP_BLOCK_64MBITS		111			/*  max application block number for 64M flash	*/
#define APP_BLOCK_32MBITS		55			/*	max application block number for 32M flash	*/
#define APP_BLOCK_16MBITS		23			/*	max application block number for 16M flash	*/
#define NVM_BEGINADDR_64MBITS	APP_ENDADDR_64MBITS
											/*	NV ram area begin address for 64M flash		*/
#define NVM_BEGINADDR_32MBITS	APP_ENDADDR_32MBITS
											/*	NV ram area begin address for 32M flash		*/
#define NVM_BEGINADDR_16MBITS	APP_ENDADDR_16MBITS
											/*	NV ram area begin address for 16M flash		*/
#define NVM_ENDADDR_64MBITS		0x800000	/*	NV ram area end address for 64M flash		*/
#define NVM_ENDADDR_32MBITS		0x400000	/*	NV ram area end address for 32M flash		*/
#define NVM_ENDADDR_16MBITS		0x200000	/*	NV ram area end address for 16M flash		*/
#define NVM_BLOCK_64MBITS		23			/*	max ram block number for 64M flash			*/
#define NVM_BLOCK_32MBITS		15			/*	max ram block number for 32M flash			*/
#define NVM_BLOCK_16MBITS		15			/*	max ram block number for 16M flash			*/
#define LARGE_BLOCK_SIZE		0x010000U		/*	large block size			*/
#define PARA_BLOCK_SIZE			0x2000U			/*	parameter block size		*/

#define TOTAL_BLOCK_64MBITS		142			/*  total block for 64M flash					*/
#define TOTAL_BLOCK_32MBITS		71			/*  total block for 32M flash					*/
#define TOTAL_BLOCK_16MBITS		39			/*  total block for 16M flash					*/
#define TOTAL_FLASH_TYPES		0x23		/*	now support flash types						*/
#define TOTAL_FLASH_SIZE_TYPES  0x07		/*	now support flash size types				*/

enum
{
	FLASH_SIZE_INVALID		= 0x00,
	FLASH_SIZE_4M			= 0x01,
	FLASH_SIZE_8M			= 0x02,
	FLASH_SIZE_16TO8M		= 0x03,	
	FLASH_SIZE_16M			= 0x04,
	FLASH_SIZE_32M			= 0x05,
	FLASH_SIZE_64M			= 0x06
};

enum
{
	UNKNOWN_FLASH			= 0x00,
	INTEL_FLASH_1602T		= 0x01,
	INTEL_FLASH_1602B		= 0x02,
	INTEL_FLASH_3204T		= 0x03,
	INTEL_FLASH_3204B		= 0x04,
	INTEL_FLASH_UNDEF1		= 0x05,
	ST_FLASH_M36W216T		= 0x06,
	ST_FLASH_UNDEF1			= 0x07,
	ST_FLASH_UNDEF2			= 0x08,
	EPSON_FLASH_324B2T		= 0x09,   
	EPSON_FLASH_162B1T		= 0x0A,
	EPSON_FLASH_UNDEF1		= 0x0B,
	AMD_FLASH_DL163DT		= 0x0C,
	AMD_FLASH_322DT			= 0x0D,
	AMD_FLASH_323DT			= 0x0E,
	AMD_FLASH_49DL640AG		= 0x0F,
	AMD_FLASH_50DL128BG		= 0x10,
	AMD_FLASH_UNDEF3		= 0x11,
	AMD_FLASH_UNDEF4		= 0x12,
	FUJITSU_FLASH_21082		= 0x13,
	FUJITSU_FLASH_21083		= 0x14,
	FUJITSU_FLASH_22182		= 0x15,
	FUJITSU_FLASH_22183		= 0x16,
	FUJITSU_FLASH_22280		= 0x17,
	FUJITSU_FLASH_UNDEF1	= 0x18,
	FUJITSU_FLASH_UNDEF2	= 0x19,
	FUJITSU_FLASH_UNDEF3	= 0x1A,
	FUJITSU_FLASH_UNDEF4	= 0x1B,
	TOSHIBA_FLASH_2482		= 0x1C,
	TOSHIBA_FLASH_2580		= 0x1D,
	TOSHIBA_FLASH_3582		= 0x1E,
	TOSHIBA_FLASH_UNDEF1	= 0x1F,
	TOSHIBA_FLASH_UNDEF2	= 0x20,
	TOSHIBA_FLASH_UNDEF3	= 0x21,
	TOSHBIA_FLASH_UNDEF4	= 0x22
};

typedef struct 
{
	DWORD		blockBase;  /* from the device base address */
	DWORD		blockSize;  /* in bytes */
}block_map_type;

typedef struct
{
	area_define_type	boot_area;
	area_define_type	app_area;
	area_define_type	nvram_area;
	WORD				totalblocks;
	block_map_type * pblockMap;
}flash_block_type;

typedef struct 
{
	char flashmfr[32];
	char flashnam[64];
}flash_mfr_name_type;

static flash_mfr_name_type flash_mfr_name[TOTAL_FLASH_TYPES] =
{
	{	"UNKNOWN",			"UNKNOWN"				},
	{	"INTEL",			"INTEL_FLASH_1602T"		},
	{	"INTEL",			"INTEL_FLASH_1602B"		},
	{	"INTEL",			"INTEL_FLASH_3204T"		},
	{	"INTEL",			"INTEL_FLASH_3204B"		},
	{	"INTEL",			"UNKNOWN"				},
	{	"ST",				"ST_FLASH_M36W216T"		},
	{	"ST",				"UNKNOWN"				},
	{	"ST",				"UNKNOWN"				},
	{	"EPSON",			"EPSON_FLASH_324B2T"	},
	{	"EPSON",			"EPSON_FLASH_162B1T"	},
	{	"EPSON",			"UNKNOWN"				},
	{	"AMD",				"AM41DL1632GT"			},
	{	"AMD",				"AM41DL3228GT"			},
	{	"AMD",				"AM41DL3238GT"			},
	{	"AMD",				"AM49DL640AG"			},
	{	"AMD",				"AM50DL128BG"			},
	{	"AMD",				"UNKNOWN"				},
	{	"AMD",				"UNKNOWN"				},
	{	"FUJUTSU",			"MB84VD21082"			},
	{	"FUJUTSU",			"MB84VD21083"			},
	{	"FUJUTSU",			"MB84VD22182"			},
	{	"FUJUTSU",			"MB84VD22183"			},
	{	"FUJUTSU",			"MB84VD22280"			},
	{	"FUJUTSU",			"UNKNOWN"				},
	{	"FUJUTSU",			"UNKNOWN"				},
	{	"FUJUTSU",			"UNKNOWN"				},
	{	"FUJUTSU",			"UNKNOWN"				},
	{	"TOSHIBA",			"TH50VSF2482AASB"		},
	{	"TOSHIBA",			"TH50VSF2580"			},
	{	"TOSHIBA",			"TH50VSF3582BDSB"		},
	{	"TOSHIBA",			"UNKNOWN"				},
	{	"TOSHIBA",			"UNKNOWN"				},
	{	"TOSHIBA",			"UNKNOWN"				},
	{	"TOSHIBA",			"UNKNOWN"				},
};

static block_map_type block_map_64M[TOTAL_BLOCK_64MBITS+1] =
   {
	/* Boot Blocks begin */
     	{ 0x000000, 0x02000 },		/* BLOCK 0 */
     	{ 0x002000, 0x02000 },		/* BLOCK 1 */
     	{ 0x004000, 0x02000 },		/* BLOCK 2 */
     	{ 0x006000, 0x02000 },		/* BLOCK 3 */
     	{ 0x008000, 0x02000 },		/* BLOCK 4 */
     	{ 0x00A000, 0x02000 },		/* BLOCK 5 */
     	{ 0x00C000, 0x02000 },		/* BLOCK 6 */
     	{ 0x00E000, 0x02000 },		/* BLOCK 7 */
	/* Boot Blocks end */

	/* Application Blocks begin */
     	{ 0x010000, 0x10000 },		/* BLOCK 8 */
     	{ 0x020000, 0x10000 },		/* BLOCK 9 */
		{ 0x030000, 0x10000 },		/* BLOCK 10 */
		{ 0x040000, 0x10000 },		/* BLOCK 11 */
		{ 0x050000, 0x10000 },		/* BLOCK 12 */
		{ 0x060000, 0x10000 },     /* BLOCK 13 */
		{ 0x070000, 0x10000 },     /* BLOCK 14 */
		{ 0x080000, 0x10000 },     /* BLOCK 15 */
		{ 0x090000, 0x10000 },     /* BLOCK 16 */
		{ 0x0A0000, 0x10000 },     /* BLOCK 17 */
		{ 0x0B0000, 0x10000 },     /* BLOCK 18 */
		{ 0x0C0000, 0x10000 },     /* BLOCK 19 */
		{ 0x0D0000, 0x10000 },     /* BLOCK 20 */
		{ 0x0E0000, 0x10000 },     /* BLOCK 21 */
		{ 0x0F0000, 0x10000 },     /* BLOCK 22 */
		{ 0x100000, 0x10000 },     /* BLOCK 23 */
		{ 0x110000, 0x10000 },     /* BLOCK 24 */
		{ 0x120000, 0x10000 },     /* BLOCK 25 */
		{ 0x130000, 0x10000 },     /* BLOCK 26 */
		{ 0x140000, 0x10000 },     /* BLOCK 27 */
		{ 0x150000, 0x10000 },     /* BLOCK 28 */
		{ 0x160000, 0x10000 },     /* BLOCK 29 */
		{ 0x170000, 0x10000 },     /* BLOCK 30 */
		{ 0x180000, 0x10000 },     /* BLOCK 31 */
		{ 0x190000, 0x10000 },     /* BLOCK 32 */
		{ 0x1A0000, 0x10000 },     /* BLOCK 33 */
		{ 0x1B0000, 0x10000 },     /* BLOCK 34 */
		{ 0x1C0000, 0x10000 },     /* BLOCK 35 */
		{ 0x1D0000, 0x10000 },     /* BLOCK 36 */
		{ 0x1E0000, 0x10000 },     /* BLOCK 37 */
		{ 0x1F0000, 0x10000 },     /* BLOCK 38 */
		{ 0x200000, 0x10000 },     /* BLOCK 39 */
		{ 0x210000, 0x10000 },     /* BLOCK 40 */
		{ 0x220000, 0x10000 },     /* BLOCK 41 */
		{ 0x230000, 0x10000 },     /* BLOCK 42 */
		{ 0x240000, 0x10000 },     /* BLOCK 43 */
		{ 0x250000, 0x10000 },     /* BLOCK 44 */
		{ 0x260000, 0x10000 },     /* BLOCK 45 */
		{ 0x270000, 0x10000 },     /* BLOCK 46 */
		{ 0x280000, 0x10000 },     /* BLOCK 47 */
		{ 0x290000, 0x10000 },     /* BLOCK 48 */
		{ 0x2A0000, 0x10000 },     /* BLOCK 49 */
		{ 0x2B0000, 0x10000 },     /* BLOCK 50 */
		{ 0x2C0000, 0x10000 },     /* BLOCK 51 */
		{ 0x2D0000, 0x10000 },     /* BLOCK 52 */
		{ 0x2E0000, 0x10000 },     /* BLOCK 53 */
		{ 0x2F0000, 0x10000 },     /* BLOCK 54 */
		{ 0x300000, 0x10000 },     /* BLOCK 55 */
		{ 0x310000, 0x10000 },     /* BLOCK 56 */
		{ 0x320000, 0x10000 },     /* BLOCK 57 */
		{ 0x330000, 0x10000 },     /* BLOCK 58 */
		{ 0x340000, 0x10000 },     /* BLOCK 59 */
		{ 0x350000, 0x10000 },     /* BLOCK 60 */
		{ 0x360000, 0x10000 },     /* BLOCK 61 */
		{ 0x370000, 0x10000 },     /* BLOCK 62 */
		{ 0x380000, 0x10000 },     /* BLOCK 63 */
		{ 0x390000, 0x10000 },     /* BLOCK 64 */
		{ 0x3A0000, 0x10000 },     /* BLOCK 65 */
		{ 0x3B0000, 0x10000 },     /* BLOCK 66 */
		{ 0x3C0000, 0x10000 },     /* BLOCK 67 */
		{ 0x3D0000, 0x10000 },     /* BLOCK 68 */
		{ 0x3E0000, 0x10000 },     /* BLOCK 69 */
		{ 0x3F0000, 0x10000 },	   /* BLOCK 70 */
		{ 0x400000, 0x10000 },     /* BLOCK 71 */
		{ 0x410000, 0x10000 },     /* BLOCK 72 */
		{ 0x420000, 0x10000 },     /* BLOCK 73 */
		{ 0x430000, 0x10000 },     /* BLOCK 74 */
		{ 0x440000, 0x10000 },     /* BLOCK 75 */
		{ 0x450000, 0x10000 },     /* BLOCK 76 */
	    { 0x460000, 0x10000 },     /* BLOCK 77 */
		{ 0x470000, 0x10000 },     /* BLOCK 78 */
		{ 0x480000, 0x10000 },     /* BLOCK 79 */
		{ 0x490000, 0x10000 },     /* BLOCK 80 */
		{ 0x4A0000, 0x10000 },     /* BLOCK 81 */
		{ 0x4B0000, 0x10000 },     /* BLOCK 82 */
		{ 0x4C0000, 0x10000 },     /* BLOCK 83 */
		{ 0x4D0000, 0x10000 },     /* BLOCK 84 */
		{ 0x4E0000, 0x10000 },     /* BLOCK 85 */
		{ 0x4F0000, 0x10000 },	   /* BLOCK 86 */
		{ 0x500000, 0x10000 },     /* BLOCK 87 */
		{ 0x510000, 0x10000 },     /* BLOCK 88 */
		{ 0x520000, 0x10000 },     /* BLOCK 89 */
		{ 0x530000, 0x10000 },     /* BLOCK 90 */
		{ 0x540000, 0x10000 },     /* BLOCK 91 */
		{ 0x550000, 0x10000 },     /* BLOCK 92 */
		{ 0x560000, 0x10000 },     /* BLOCK 93 */
		{ 0x570000, 0x10000 },     /* BLOCK 94 */
		{ 0x580000, 0x10000 },     /* BLOCK 95 */
		{ 0x590000, 0x10000 },     /* BLOCK 96 */
		{ 0x5A0000, 0x10000 },     /* BLOCK 97 */
		{ 0x5B0000, 0x10000 },     /* BLOCK 98 */
		{ 0x5C0000, 0x10000 },     /* BLOCK 99 */
		{ 0x5D0000, 0x10000 },     /* BLOCK 100 */
		{ 0x5E0000, 0x10000 },     /* BLOCK 101 */
		{ 0x5F0000, 0x10000 },	   /* BLOCK 102 */ 	
	   	{ 0x600000, 0x10000 },     /* BLOCK 103 */
	    { 0x610000, 0x10000 },     /* BLOCK 104 */
	    { 0x620000, 0x10000 },     /* BLOCK 105 */
	    { 0x630000, 0x10000 },     /* BLOCK 106 */
		{ 0x640000, 0x10000 },     /* BLOCK 107 */
		{ 0x650000, 0x10000 },     /* BLOCK 108 */
		{ 0x660000, 0x10000 },     /* BLOCK 109 */
		{ 0x670000, 0x10000 },     /* BLOCK 110 */
		{ 0x680000, 0x10000 },     /* BLOCK 111 */
		{ 0x690000, 0x10000 },     /* BLOCK 112 */
		{ 0x6A0000, 0x10000 },     /* BLOCK 113 */
		{ 0x6B0000, 0x10000 },     /* BLOCK 114 */
		{ 0x6C0000, 0x10000 },     /* BLOCK 115 */
		{ 0x6D0000, 0x10000 },     /* BLOCK 116 */
		{ 0x6E0000, 0x10000 },     /* BLOCK 117 */
		{ 0x6F0000, 0x10000 },	   /* BLOCK 118 */
	/* Application Blocks end */

	/* Nvram Blocks Begin */
     	{ 0x700000, 0x10000 },     /* BLOCK 119 */
		{ 0x710000, 0x10000 },     /* BLOCK 120 */
		{ 0x720000, 0x10000 },     /* BLOCK 121 */
		{ 0x730000, 0x10000 },     /* BLOCK 122 */
		{ 0x740000, 0x10000 },     /* BLOCK 123 */
		{ 0x750000, 0x10000 },     /* BLOCK 124 */
		{ 0x760000, 0x10000 },     /* BLOCK 125 */
		{ 0x770000, 0x10000 },     /* BLOCK 126 */
		{ 0x780000, 0x10000 },     /* BLOCK 127 */
		{ 0x790000, 0x10000 },     /* BLOCK 128 */
		{ 0x7A0000, 0x10000 },     /* BLOCK 129 */
		{ 0x7B0000, 0x10000 },     /* BLOCK 130 */
		{ 0x7C0000, 0x10000 },     /* BLOCK 131 */
		{ 0x7D0000, 0x10000 },     /* BLOCK 132 */
		{ 0x7E0000, 0x10000 },     /* BLOCK 133 */
		{ 0x7F0000, 0x02000 },	   /* BLOCK 134 */
		{ 0x7F2000, 0x02000 },	   /* BLOCK 135 */
		{ 0x7F4000, 0x02000 },	   /* BLOCK 136 */
		{ 0x7F6000, 0x02000 },	   /* BLOCK 137 */
		{ 0x7F8000, 0x02000 },	   /* BLOCK 138 */
		{ 0x7FA000, 0x02000 },	   /* BLOCK 139 */
		{ 0x7FC000, 0x02000 },	   /* BLOCK 140 */
		{ 0x7FE000, 0x02000 }, 	   /* BLOCK 141 */
	/* Nvram Blocks end */
		{ 0x800000, 0x00000 }     /* END of FLASH */
   };

static block_map_type block_map_32M[TOTAL_BLOCK_32MBITS+1] =
{
	/* Boot Blocks begin */
	{ 0x00000000, 0x10000 },  /* BLOCK  0 */
	/* Boot Blocks end */

	/* Application Blocks begin */
	{ 0x00010000, 0x10000 },  /* BLOCK  1 */
	{ 0x00020000, 0x10000 },  /* BLOCK  2 */
	{ 0x00030000, 0x10000 },  /* BLOCK  3 */
	{ 0x00040000, 0x10000 },  /* BLOCK  4 */
	{ 0x00050000, 0x10000 },  /* BLOCK  5 */
	{ 0x00060000, 0x10000 },  /* BLOCK  6 */
	{ 0x00070000, 0x10000 },  /* BLOCK  7 */
	{ 0x00080000, 0x10000 },  /* BLOCK  8 */
	{ 0x00090000, 0x10000 },  /* BLOCK  9 */
	{ 0x000a0000, 0x10000 },  /* BLOCK 10 */
	{ 0x000b0000, 0x10000 },  /* BLOCK 11 */
	{ 0x000c0000, 0x10000 },  /* BLOCK 12 */
	{ 0x000d0000, 0x10000 },  /* BLOCK 13 */
	{ 0x000e0000, 0x10000 },  /* BLOCK 14 */
	{ 0x000f0000, 0x10000 },  /* BLOCK 15 */
	{ 0x00100000, 0x10000 },  /* BLOCK 16 */
	{ 0x00110000, 0x10000 },  /* BLOCK 17 */
	{ 0x00120000, 0x10000 },  /* BLOCK 18 */
	{ 0x00130000, 0x10000 },  /* BLOCK 19 */
	{ 0x00140000, 0x10000 },  /* BLOCK 20 */
	{ 0x00150000, 0x10000 },  /* BLOCK 21 */
	{ 0x00160000, 0x10000 },  /* BLOCK 22 */
	{ 0x00170000, 0x10000 },  /* BLOCK 23 */
	{ 0x00180000, 0x10000 },  /* BLOCK 24 */
	{ 0x00190000, 0x10000 },  /* BLOCK 25 */
	{ 0x001a0000, 0x10000 },  /* BLOCK 26 */
	{ 0x001b0000, 0x10000 },  /* BLOCK 27 */
	{ 0x001c0000, 0x10000 },  /* BLOCK 28 */
	{ 0x001d0000, 0x10000 },  /* BLOCK 29 */
	{ 0x001e0000, 0x10000 },  /* BLOCK 30 */
	{ 0x001f0000, 0x10000 },  /* BLOCK 31 */
	{ 0x00200000, 0x10000 },  /* BLOCK 32 */
	{ 0x00210000, 0x10000 },  /* BLOCK 33 */
	{ 0x00220000, 0x10000 },  /* BLOCK 34 */
	{ 0x00230000, 0x10000 },  /* BLOCK 35 */
	{ 0x00240000, 0x10000 },  /* BLOCK 36 */
	{ 0x00250000, 0x10000 },  /* BLOCK 37 */
	{ 0x00260000, 0x10000 },  /* BLOCK 38 */
	{ 0x00270000, 0x10000 },  /* BLOCK 39 */
	{ 0x00280000, 0x10000 },  /* BLOCK 40 */
	{ 0x00290000, 0x10000 },  /* BLOCK 41 */
	{ 0x002a0000, 0x10000 },  /* BLOCK 42 */
	{ 0x002b0000, 0x10000 },  /* BLOCK 43 */
	{ 0x002c0000, 0x10000 },  /* BLOCK 44 */
	{ 0x002d0000, 0x10000 },  /* BLOCK 45 */
	{ 0x002e0000, 0x10000 },  /* BLOCK 46 */
	{ 0x002f0000, 0x10000 },  /* BLOCK 47 */
	{ 0x00300000, 0x10000 },  /* BLOCK 48 */
	{ 0x00310000, 0x10000 },  /* BLOCK 49 */
	{ 0x00320000, 0x10000 },  /* BLOCK 40 */
	{ 0x00330000, 0x10000 },  /* BLOCK 51 */
	{ 0x00340000, 0x10000 },  /* BLOCK 52 */
	{ 0x00350000, 0x10000 },  /* BLOCK 53 */
	{ 0x00360000, 0x10000 },  /* BLOCK 54 */
	{ 0x00370000, 0x10000 },  /* BLOCK 55 */
	/* Application Blocks end */

	/* Nvram Blocks Begin */
	{ 0x00380000, 0x10000 },  /* BLOCK 56 */
	{ 0x00390000, 0x10000 },  /* BLOCK 57 */
	{ 0x003a0000, 0x10000 },  /* BLOCK 58 */
	{ 0x003b0000, 0x10000 },  /* BLOCK 59 */
	{ 0x003c0000, 0x10000 },  /* BLOCK 60 */
	{ 0x003d0000, 0x10000 },  /* BLOCK 61 */
	{ 0x003e0000, 0x10000 },  /* BLOCK 62 */
	{ 0x003f0000, 0x02000 },  /* BLOCK 63 */
	{ 0x003f2000, 0x02000 },  /* BLOCK 64 */
	{ 0x003f4000, 0x02000 },  /* BLOCK 65 */
	{ 0x003f6000, 0x02000 },  /* BLOCK 66 */
	{ 0x003f8000, 0x02000 },  /* BLOCK 67 */
	{ 0x003fa000, 0x02000 },  /* BLOCK 68 */
	{ 0x003fc000, 0x02000 },  /* BLOCK 69 */
	{ 0x003fe000, 0x02000 },  /* BLOCK 70 */
	/* Nvram Blocks end */
	{ 0x00400000, 0x00000 }   /* END of FLASH */
};

static block_map_type block_map_16M[TOTAL_BLOCK_16MBITS+1] =
{
	/* Boot Blocks begin */
	{ 0x00000000, 0x10000 },  /* BLOCK  0 */
	/* Boot Blocks end */

	/* Application Blocks begin */
	{ 0x00010000, 0x10000 },  /* BLOCK  1 */
	{ 0x00020000, 0x10000 },  /* BLOCK  2 */
	{ 0x00030000, 0x10000 },  /* BLOCK  3 */
	{ 0x00040000, 0x10000 },  /* BLOCK  4 */
	{ 0x00050000, 0x10000 },  /* BLOCK  5 */
	{ 0x00060000, 0x10000 },  /* BLOCK  6 */
	{ 0x00070000, 0x10000 },  /* BLOCK  7 */
	{ 0x00080000, 0x10000 },  /* BLOCK  8 */
	{ 0x00090000, 0x10000 },  /* BLOCK  9 */
	{ 0x000a0000, 0x10000 },  /* BLOCK 10 */
	{ 0x000b0000, 0x10000 },  /* BLOCK 11 */
	{ 0x000c0000, 0x10000 },  /* BLOCK 12 */
	{ 0x000d0000, 0x10000 },  /* BLOCK 13 */
	{ 0x000e0000, 0x10000 },  /* BLOCK 14 */
	{ 0x000f0000, 0x10000 },  /* BLOCK 15 */
	{ 0x00100000, 0x10000 },  /* BLOCK 16 */
	{ 0x00110000, 0x10000 },  /* BLOCK 17 */
	{ 0x00120000, 0x10000 },  /* BLOCK 18 */
	{ 0x00130000, 0x10000 },  /* BLOCK 19 */
	{ 0x00140000, 0x10000 },  /* BLOCK 20 */
	{ 0x00150000, 0x10000 },  /* BLOCK 21 */
	{ 0x00160000, 0x10000 },  /* BLOCK 22 */
	{ 0x00170000, 0x10000 },  /* BLOCK 23 */
	/* Application Blocks end */

	/* Nvram Blocks Begin */
	{ 0x00180000, 0x10000 },  /* BLOCK 24 */
	{ 0x00190000, 0x10000 },  /* BLOCK 25 */
	{ 0x001a0000, 0x10000 },  /* BLOCK 26 */
	{ 0x001b0000, 0x10000 },  /* BLOCK 27 */
	{ 0x001c0000, 0x10000 },  /* BLOCK 28 */
	{ 0x001d0000, 0x10000 },  /* BLOCK 29 */
	{ 0x001e0000, 0x10000 },  /* BLOCK 30 */
	{ 0x001f0000, 0x02000 },  /* BLOCK 31 */
	{ 0x001f2000, 0x02000 },  /* BLOCK 32 */
	{ 0x001f4000, 0x02000 },  /* BLOCK 33 */
	{ 0x001f6000, 0x02000 },  /* BLOCK 34 */
	{ 0x001f8000, 0x02000 },  /* BLOCK 35 */
	{ 0x001fa000, 0x02000 },  /* BLOCK 36 */
	{ 0x001fc000, 0x02000 },  /* BLOCK 37 */
	{ 0x001fe000, 0x02000 },  /* BLOCK 38 */
	/* Nvram Blocks end */
	{ 0x00200000, 0x00000 }   /* END of FLASH */
};

#endif