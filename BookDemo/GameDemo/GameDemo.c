#include "GameDemo.h"
#include "KeyMask.h"

#define SPRITE_MAX_OFFSET_INDEX 5


static const uint16 game_map_1[]=
{
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0004,0x0008,0x0004,0x0008,0x0004,0x0008,0x0004,0x0008,
	0x0001,0x0005,0x0001,0x0005,0x0001,0x0005,0x0001,0x0005,
	0x0002,0x0006,0x0002,0x0006,0x0002,0x0006,0x0002,0x0006,
	0x0003,0x0007,0x0003,0x0007,0x0003,0x0007,0x0003,0x0007,
	0x0004,0x0008,0x0004,0x0008,0x0004,0x0008,0x0004,0x0008,
};
static const uint16 game_map_2[] =
{

		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x070a, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x070a, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x070a, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x070a, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x070a, 0x0709, 0x00ff, 0x00ff,
		0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x070a, 0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x070a, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x0215, 0x0216, 0x0217, 0x0216, 0x0217, 0x0218, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x070a, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x001c, 0x001a, 0x001a, 0x001a, 0x001b, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x0215, 0x0216, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x070a, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x070a, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x001d, 0x001a, 0x001a, 0x001a, 0x001e, 0x070a, 0x0709, 0x070a,
		0x0709, 0x070a, 0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x0709, 0x070a, 0x0709,
		0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x0709, 0x070a, 0x0709,
		0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x0019, 0x001a, 0x001a, 0x001a, 0x001b, 0x0709, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x0217, 0x0218, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x070a, 0x0215, 0x0216, 0x0a11, 0x0a12, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x070a, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x001c, 0x001a, 0x001e, 0x070a, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x001d,
		0x001a, 0x001b, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x0813, 0x0814, 0x00ff, 0x0215, 0x0216, 0x0217,
		0x0217, 0x0216, 0x0216, 0x0217, 0x0216, 0x0217, 0x0709, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x0019, 0x001a, 0x001e, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x001c,
		0x001a, 0x001a, 0x0215, 0x0216, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x0813, 0x0814, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x001d, 0x001a, 0x001e, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x0019,
		0x001a, 0x001a, 0x001a, 0x001b, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x0813, 0x0814, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x001d, 0x001a, 0x001b, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x0709,
		0x0709, 0x070a, 0x070a, 0x070a, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x0019, 0x001a, 0x001b, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x001c, 0x001a, 0x001a, 0x001a, 0x070a, 0x0709, 0x070a, 0x070a, 0x070a,
		0x0709, 0x070a, 0x0709, 0x0709, 0x070a, 0x0a11, 0x0a12, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 
		0x00ff, 0x00ff, 0x0007, 0x0008, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x0005, 0x0006, 0x00ff, 0x00ff, 0x0007, 0x0008, 0x00ff, 0x00ff,
		0x0007, 0x0008, 0x00ff, 0x00ff, 0x0019, 0x001a, 0x0709, 0x070a, 0x0709,
		0x070a, 0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x070a, 0x070a,
		0x070a, 0x0709, 0x0709, 0x0709, 0x0709, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x001c, 0x001a, 0x001e, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x0005, 0x0006, 0x00ff, 0x00ff, 0x0709, 0x070a,
		0x070a, 0x0709, 0x070a, 0x0709, 0x070a, 0x0709, 0x00ff, 0x00ff, 0x00ff,
		0x0019, 0x001a, 0x001a, 0x070a, 0x0709, 0x070a, 0x0709, 0x070a, 0x070a,
		0x0709, 0x070a, 0x070a, 0x070a, 0x0709, 0x0813, 0x0814, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x0005, 0x0006, 0x00ff, 0x00ff, 0x00ff, 0x0007,
		0x0008, 0x00ff, 
		0x001f, 0x0020, 0x001f, 0x0020, 0x001f, 0x0020, 0x00ff, 0x00ff, 0x00ff,
		0x001f, 0x0020, 0x001f, 0x0020, 0x001f, 0x0020, 0x001f, 0x00ff, 0x00ff,
		0x001f, 0x0020, 0x0709, 0x070a, 0x0709, 0x070a, 0x0709, 0x070a, 0x0709,
		0x0709, 0x070a, 0x0709, 0x0709, 0x070a, 0x0709, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x0709, 0x070a, 0x0709,
		0x0709, 0x0709, 0x070a, 0x0709, 0x0709, 0x070a, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x0019, 0x001a, 0x001b, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x001f, 0x0020, 0x001f, 0x0020, 0x0709, 0x070a, 0x0709,
		0x070a, 0x070a, 0x0709, 0x0709, 0x070a, 0x0709, 0x070a, 0x00ff, 0x00ff,
		0x0019, 0x001a, 0x0709, 0x0709, 0x070a, 0x070a, 0x0709, 0x070a, 0x0709,
		0x0709, 0x070a, 0x0709, 0x070a, 0x0709, 0x0813, 0x0814, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff, 0x00ff,
		0x00ff, 0x00ff, 0x001f, 0x0020, 0x001f, 0x0020, 0x001f, 0x0020, 0x001f,
		0x0020, 0x001f, 
		0x070f, 0x0710, 0x070f, 0x0710, 0x070f, 0x0710, 0x070f, 0x0710, 0x070f,
		0x0710, 0x070f, 0x0710, 0x070f, 0x0710, 0x070f, 0x0710, 0x070f, 0x0710,
		0x070f, 0x0710, 0x070d, 0x070e, 0x070d, 0x070e, 0x070d, 0x070e, 0x070d,
		0x070e, 0x070d, 0x070e, 0x070d, 0x070e, 0x070e, 0x0001, 0x0001, 0x070f,
		0x0710, 0x070f, 0x0710, 0x0710, 0x0710, 0x070f, 0x070d, 0x070e, 0x070d,
		0x070e, 0x070d, 0x070d, 0x070d, 0x070e, 0x070d, 0x070f, 0x0710, 0x070f,
		0x0710, 0x070f, 0x0710, 0x070f, 0x0710, 0x070f, 0x070e, 0x0001, 0x0001,
		0x0001, 0x0001, 0x070f, 0x0710, 0x070f, 0x0710, 0x070d, 0x070e, 0x070d,
		0x070e, 0x070d, 0x070e, 0x070d, 0x070e, 0x070d, 0x070e, 0x070f, 0x0710,
		0x070f, 0x0710, 0x070d, 0x070e, 0x070d, 0x070e, 0x070d, 0x070e, 0x070d,
		0x070e, 0x070d, 0x070e, 0x070d, 0x070e, 0x0710, 0x070f, 0x0710, 0x070f,
		0x0710, 0x070f, 0x0710, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
		0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001, 0x0001,
		0x0001, 0x0001, 0x070f, 0x0710, 0x070f, 0x0710, 0x070f, 0x0710, 0x070f,
		0x0710, 0x070f, 
		0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b,
		0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c,
		0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b,
		0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x1702, 0x1702, 0x070b,
		0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c,
		0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b,
		0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x1702, 0x1702,
		0x1702, 0x1702, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b,
		0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c,
		0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b,
		0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070b, 0x070c, 0x070b,
		0x070c, 0x070b, 0x070c, 0x1702, 0x1702, 0x1702, 0x1702, 0x1702, 0x1702,
		0x1702, 0x1702, 0x1702, 0x1702, 0x1702, 0x1702, 0x1702, 0x1702, 0x1702,
		0x1702, 0x1702, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b,
		0x070c, 0x070b, 
		0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c,
		0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b,
		0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c,
		0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x1703, 0x1703, 0x070c,
		0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b,
		0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c,
		0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x1703, 0x1703,
		0x1703, 0x1703, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c,
		0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b,
		0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c,
		0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070c, 0x070b, 0x070c,
		0x070b, 0x070c, 0x070b, 0x1703, 0x1703, 0x1703, 0x1703, 0x1703, 0x1703,
		0x1703, 0x1703, 0x1703, 0x1703, 0x1703, 0x1703, 0x1703, 0x1703, 0x1703,
		0x1703, 0x1703, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c, 0x070b, 0x070c,
		0x070b, 0x070c,
};

//主程序函数
static boolean GameDemo_HandleEvent(GameDemo* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
boolean GameDemo_InitAppData(GameDemo* pMe);
void GameDemo_FreeAppData(GameDemo* pMe);
static void MainLoop(GameDemo *pMe);
void MainAction(GameDemo *pMe);
void MainUpdate(GameDemo *pMe);
//辅助函数
void DrawTextByBuffer(GameDemo *pMe,const AECHAR *pText,int x,int y,RGBVAL rgb);
void DrawTextById(GameDemo *pMe,int16 nResID,int x,int y,RGBVAL rgb);
void DrawTextCenterById(GameDemo *pMe,int16 nResID,int y,RGBVAL rgb);
void DrawTextCenterByBuffer(GameDemo *pMe,const AECHAR *pText,int y,RGBVAL rgb);
//分程序函数

boolean MainMenuHandleEvent(GameDemo* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
boolean MainGameHandleEvent(GameDemo* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
void MainMenuAction(GameDemo *pMe);
void MainGameAction(GameDemo *pMe);
void MainMenuUpdate(GameDemo *pMe);
void MainGameUpdate(GameDemo *pMe);
void MainMenuInit(GameDemo *pMe);
void MainGameInit(GameDemo *pMe);
void MainGameFree(GameDemo *pMe);
void MainGameLoadResource(GameDemo *pMe);


static void MainAppDrawMidMap(GameDemo *pMe,Map *pMidMap,boolean isTransparent);
static void MediaNotify(void *pUser,AEEMediaCmdNotify *pCmdNotify);


//碰撞检查
void CheckTileCollisionVertical(GameDemo *pMe);
boolean CheckTileCollisionHorizon(GameDemo *pMe);
boolean IntersectRect(AEERect *pRect1,AEERect *pRect2);
void CheckBulletEnemyCollision(GameDemo *pMe);
void CheckMainSpriteEnemyCollision(GameDemo *pMe);


void SpriteInit8(AEESpriteCmd *pSprites,int index,int16 x,int16 y,int spriteIndex,uint8 unLayer);
void SpriteInit16(AEESpriteCmd *pSprites,int index,int16 x,int16 y,int spriteIndex,uint8 unLayer);
void SpriteInit32(AEESpriteCmd *pSprites,int index,int16 x,int16 y,int spriteIndex,uint8 unLayer);
void SaveCurrentData(GameDemo *pMe);
void LoadSaveData(GameDemo *pMe);



void EnemyAction(GameDemo *pMe,AEESpriteCmd *pSprites,int index,int num);

void PlayBgMedia(GameDemo *pMe);
void PlayGameMedia(GameDemo *pMe);

int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	*ppObj = NULL;
	if(ClsId == AEECLSID_GAMEDEMO)
	{
		if( AEEApplet_New(sizeof(GameDemo),
			ClsId,
			pIShell,
			po,
			(IApplet**)ppObj,
			(AEEHANDLER)GameDemo_HandleEvent,
			(PFNFREEAPPDATA)GameDemo_FreeAppData))
		{
			if(GameDemo_InitAppData((GameDemo*)*ppObj))
			{
				return(AEE_SUCCESS);
			}
			else
			{
				IAPPLET_Release((IApplet*)*ppObj);
				return EFAILED;
			}
		}
	}
	return(EFAILED);
}
static boolean GameDemo_HandleEvent(GameDemo* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{ 
	switch (eCode) 
	{
	case EVT_APP_START:    
		ISHELL_SetTimerEx(pMe->pIShell,0,&pMe->m_cbMainTimer);
		MainMenuInit(pMe);
		return(TRUE);
	case EVT_APP_STOP:
		return(TRUE);
	case EVT_APP_SUSPEND:
		return(TRUE);
	case EVT_APP_RESUME:
		return(TRUE);
	case EVT_APP_MESSAGE:
		return(TRUE);
	default:
		{
			switch(pMe->m_State)
			{
			case enMainMenu:
				return MainMenuHandleEvent(pMe,eCode,wParam,dwParam);
			case enMainGame:
				return MainGameHandleEvent(pMe,eCode,wParam,dwParam);
				break;
			}
		}
		break;
	}
	return FALSE;
}
static void MainLoop(GameDemo *pMe)
{
	ISHELL_SetTimerEx(pMe->pIShell,80,&pMe->m_cbMainTimer);
	MainAction(pMe);
	MainUpdate(pMe);
}

void MainAction(GameDemo *pMe)
{
	switch(pMe->m_State)
	{
	case enMainMenu:
		MainMenuAction(pMe);
		break;
	case enMainGame:
		MainGameAction(pMe);
		break;
	}
}
void MainUpdate(GameDemo *pMe)
{
	switch(pMe->m_State)
	{
	case enMainMenu:
		MainMenuUpdate(pMe);
		break;
	case enMainGame:
		MainGameUpdate(pMe);
		break;
	}
	IDISPLAY_Update(pMe->pIDisplay);
}

boolean GameDemo_InitAppData(GameDemo* pMe)
{
	int nAscent;       
	int nDescent; 
	pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
	ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);
	pMe->pIDisplay = pMe->a.m_pIDisplay;
	pMe->pIShell   = pMe->a.m_pIShell;
	pMe->m_nFontHeight = IDISPLAY_GetFontMetrics(pMe->pIDisplay,AEE_FONT_NORMAL,&nAscent,&nDescent);
	CALLBACK_Init(&pMe->m_cbMainTimer,MainLoop,pMe);
	pMe->m_bVolOn = TRUE;
	PlayBgMedia(pMe);
	return TRUE;
}
void GameDemo_FreeAppData(GameDemo* pMe)
{
	if(pMe->m_pIMedia)
	{
		IMEDIA_Stop(pMe->m_pIMedia);
		IMEDIA_RegisterNotify(pMe->m_pIMedia,NULL,NULL);
		IMEDIA_Release(pMe->m_pIMedia);
	}
	MainGameFree(pMe);
}

void DrawTextById(GameDemo *pMe,int16 nResID,int x,int y,RGBVAL rgb)
{
	uint32 bufferSize; 
	AECHAR *pText;
	ISHELL_GetResSize(pMe->pIShell,RES_FILE,nResID,RESTYPE_STRING,&bufferSize);
	pText =(AECHAR*)MALLOC(bufferSize);
	ISHELL_LoadResString(pMe->pIShell,RES_FILE,nResID,pText,bufferSize);
	DrawTextByBuffer(pMe,pText,x,y,rgb);
	FREE(pText);
}

void DrawTextByBuffer(GameDemo *pMe,const AECHAR *pText,int x,int y,RGBVAL rgb)
{
	IDISPLAY_SetColor(pMe->pIDisplay,CLR_USER_TEXT,rgb);
	IDISPLAY_DrawText(pMe->pIDisplay,
		AEE_FONT_NORMAL,
		pText,
		-1,
		x,
		y,
		NULL,
		IDF_TEXT_TRANSPARENT);
}
void DrawTextCenterById(GameDemo *pMe,int16 nResID,int y,RGBVAL rgb)
{
	uint32 bufferSize; 
	AECHAR *pText;
	ISHELL_GetResSize(pMe->pIShell,RES_FILE,nResID,RESTYPE_STRING,&bufferSize);
	pText =(AECHAR*)MALLOC(bufferSize);
	ISHELL_LoadResString(pMe->pIShell,RES_FILE,nResID,pText,bufferSize);
	DrawTextCenterByBuffer(pMe,pText,y,rgb);
	FREE(pText);
}
void DrawTextCenterByBuffer(GameDemo *pMe,const AECHAR *pText,int y,RGBVAL rgb)
{
	int width = IDISPLAY_MeasureText(pMe->pIDisplay,AEE_FONT_NORMAL,pText);
	DrawTextByBuffer(pMe,pText,(pMe->DeviceInfo.cxScreen-width)/2,y,rgb);
}

boolean MainMenuHandleEvent(GameDemo* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	switch(eCode)
	{
	case EVT_KEY:
		if(wParam==AVK_CLR)
		{
			return FALSE;
		}
		break;
	case EVT_KEY_PRESS:
		switch(wParam)
		{
		case AVK_UP:
		case AVK_2:
			if(!(pMe->m_Menu.m_iKeyState&KEY_UP)&&!(pMe->m_Menu.m_iKeyState&KEY_UP_MASK))
			{
				pMe->m_Menu.m_iKeyState |= (KEY_UP|KEY_UP_MASK);
			}
			return TRUE;
		case AVK_DOWN:
		case AVK_8:
			if(!(pMe->m_Menu.m_iKeyState&KEY_DOWN)&&!(pMe->m_Menu.m_iKeyState&KEY_DOWN_MASK))
			{
				pMe->m_Menu.m_iKeyState |= (KEY_DOWN|KEY_DOWN_MASK);
			}
			return TRUE;
		case AVK_SELECT:
		case AVK_5:
			if(!(pMe->m_Menu.m_iKeyState&KEY_OK)&&!(pMe->m_Menu.m_iKeyState&KEY_OK_MASK))
			{
				pMe->m_Menu.m_iKeyState |= (KEY_OK|KEY_OK_MASK);
			}
			return TRUE;
		}
		break;
	case EVT_KEY_RELEASE:
		switch(wParam)
		{
		case AVK_UP:
		case AVK_2:
			pMe->m_Menu.m_iKeyState &=~KEY_UP;
			return TRUE;
		case AVK_DOWN:
		case AVK_8:
			pMe->m_Menu.m_iKeyState &=~KEY_DOWN;
			return TRUE;
		case AVK_SELECT:
		case AVK_5:
			pMe->m_Menu.m_iKeyState &=~KEY_OK;
			return TRUE;
		}
		break;
	}
	return FALSE;
}
boolean MainGameHandleEvent(GameDemo* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	switch(eCode)
	{
	case EVT_KEY:
		switch(wParam)
		{
		case AVK_CLR:
			pMe->m_State = enMainMenu;
			MainGameFree(pMe);
			MainMenuInit(pMe);
			return TRUE;
		case AVK_POUND:
			pMe->m_bIsSave = TRUE;
			break;
		}
		break;
	case EVT_KEY_PRESS:
		switch(wParam)
		{
		case AVK_UP:
		case AVK_2:
			if(!(pMe->m_Game.m_MainSprite.m_iKeyState&KEY_UP)&&!(pMe->m_Game.m_MainSprite.m_iKeyState&KEY_UP_MASK))
			{
				pMe->m_Game.m_MainSprite.m_iKeyState |= (KEY_UP|KEY_UP_MASK);
				pMe->m_Game.m_MainSprite.dy -= 15;
			}
			return TRUE;
		case AVK_DOWN:
		case AVK_8:
			if(!(pMe->m_Game.m_MainSprite.m_iKeyState&KEY_DOWN)&&!(pMe->m_Game.m_MainSprite.m_iKeyState&KEY_DOWN_MASK))
			{
				pMe->m_Game.m_MainSprite.m_iKeyState |= (KEY_DOWN|KEY_DOWN_MASK);
			}
			return TRUE;
		case AVK_SELECT:
		case AVK_5:
			if(!(pMe->m_Game.m_MainSprite.m_iKeyState&KEY_OK)&&!(pMe->m_Game.m_MainSprite.m_iKeyState&KEY_OK_MASK))
			{
				pMe->m_Game.m_MainSprite.m_iKeyState |= (KEY_OK|KEY_OK_MASK);
			}
			return TRUE;
		case AVK_LEFT:
		case AVK_4:
			pMe->m_Game.m_MainSprite.m_iKeyState |= KEY_LEFT;
			return TRUE;
		case AVK_RIGHT:
		case AVK_6:
			pMe->m_Game.m_MainSprite.m_iKeyState |= KEY_RIGHT;
			return TRUE;
		}
		break;
	case EVT_KEY_RELEASE:
		switch(wParam)
		{
		case AVK_UP:
		case AVK_2:
			pMe->m_Game.m_MainSprite.m_iKeyState &=~KEY_UP;
			return TRUE;
		case AVK_DOWN:
		case AVK_8:
			pMe->m_Game.m_MainSprite.m_iKeyState &=~KEY_DOWN;
			return TRUE;
		case AVK_SELECT:
		case AVK_5:
			pMe->m_Game.m_MainSprite.m_iKeyState &=~KEY_OK;
			return TRUE;
		case AVK_LEFT:
		case AVK_4:
			pMe->m_Game.m_MainSprite.m_iKeyState &= ~KEY_LEFT;
			return TRUE;
		case AVK_RIGHT:
		case AVK_6:
			pMe->m_Game.m_MainSprite.m_iKeyState &= ~KEY_RIGHT;
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void MainMenuAction(GameDemo *pMe)
{
	switch(pMe->m_Menu.m_State)
	{
	case enMenuMain:
		{
			if(pMe->m_Menu.m_iKeyState&KEY_UP_MASK)
			{
				pMe->m_Menu.m_iKeyState &=~KEY_UP_MASK;
				pMe->m_Menu.m_bUpdate = TRUE;
				if(pMe->m_Menu.m_nWhichActive>0)
				{
					--pMe->m_Menu.m_nWhichActive;
				}
				else
				{
					pMe->m_Menu.m_nWhichActive = 4;
				}
			}
			else if(pMe->m_Menu.m_iKeyState&KEY_DOWN_MASK)
			{
				pMe->m_Menu.m_iKeyState &=~KEY_DOWN_MASK;
				pMe->m_Menu.m_bUpdate = TRUE;
				if(pMe->m_Menu.m_nWhichActive<4)
				{
					++pMe->m_Menu.m_nWhichActive;
				}
				else
				{
					pMe->m_Menu.m_nWhichActive = 0;
				}
			}
			else if(pMe->m_Menu.m_iKeyState&KEY_OK_MASK)
			{
				pMe->m_Menu.m_iKeyState &=~KEY_OK_MASK;
				pMe->m_Menu.m_bUpdate = TRUE;
				switch(pMe->m_Menu.m_nWhichActive)
				{
				case 0:
					pMe->m_State = enMainGame;
					MainGameInit(pMe);
					if(pMe->m_pIMedia)
					{
						IMEDIA_Stop(pMe->m_pIMedia);
						IMEDIA_RegisterNotify(pMe->m_pIMedia,NULL,NULL);
						IMEDIA_Release(pMe->m_pIMedia);
						pMe->m_pIMedia = NULL;
					}
					break;
				case 1:
					pMe->m_State = enMainGame;
					MainGameInit(pMe);
					LoadSaveData(pMe);
					if(pMe->m_pIMedia)
					{
						IMEDIA_Stop(pMe->m_pIMedia);
						IMEDIA_RegisterNotify(pMe->m_pIMedia,NULL,NULL);
						IMEDIA_Release(pMe->m_pIMedia);
						pMe->m_pIMedia = NULL;
					}
					break;
				case 2:
					break;
				case 3:
					pMe->m_bVolOn = !pMe->m_bVolOn;
					if(pMe->m_bVolOn)
					{
						PlayBgMedia(pMe);
					}
					else
					{
						pMe->m_bIsContinue = FALSE;
						if(pMe->m_pIMedia)
						{
							IMEDIA_Stop(pMe->m_pIMedia);
							IMEDIA_RegisterNotify(pMe->m_pIMedia,NULL,NULL);
							IMEDIA_Release(pMe->m_pIMedia);
							pMe->m_pIMedia = NULL;
						}
					}
					break;
				case 4:
					break;
				}
			}
		}
		break;
	case enMenuHelp:
		break;
	}
}

void MainGameAction(GameDemo *pMe)
{
	int i;
	EnemyAction(pMe,pMe->m_Game.m_MoveSprites,6,2);
	CheckBulletEnemyCollision(pMe);
	CheckMainSpriteEnemyCollision(pMe);
	pMe->m_Game.m_MainSprite.dy += 2;
	CheckTileCollisionVertical(pMe);
	pMe->m_Game.m_MainSprite.m_sprite[0].y += pMe->m_Game.m_MainSprite.dy;

	if(pMe->m_Game.m_MainSprite.m_sprite[0].y + pMe->m_Game.m_MidMap.m_nY < 0)
	{
		pMe->m_Game.m_MainSprite.m_sprite[0].y = 0;
		pMe->m_Game.m_MainSprite.dy = 0;
	}
	if (pMe->m_Game.m_MainSprite.m_sprite[0].y <= pMe->DeviceInfo.cyScreen / 4
		||((pMe->m_Game.m_MidMap.m_nY !=pMe->m_Game.m_MidMap.m_nTileSize * pMe->m_Game.m_MidMap.m_nHeight-pMe->DeviceInfo.cyScreen)
		&& pMe->m_Game.m_MainSprite.m_sprite[0].y >= pMe->DeviceInfo.cyScreen / 4)
		)
	{
		pMe->m_Game.bgdy = (pMe->DeviceInfo.cyScreen / 4 - pMe->m_Game.m_MainSprite.m_sprite[0].y)/4;
		if (pMe->m_Game.bgdy == 0 && pMe->DeviceInfo.cyScreen / 4 - pMe->m_Game.m_MainSprite.m_sprite[0].y > 0)
		{
			pMe->m_Game.bgdy = pMe->DeviceInfo.cyScreen / 4 - pMe->m_Game.m_MainSprite.m_sprite[0].y > 0 ? 1 : -1;
		}
		if (pMe->m_Game.bgdy -pMe->m_Game.m_MidMap.m_nY <
			pMe->DeviceInfo.cyScreen -
			pMe->m_Game.m_MidMap.m_nTileSize * pMe->m_Game.m_MidMap.m_nHeight)
		{
			pMe->m_Game.bgdy = pMe->DeviceInfo.cyScreen -
				pMe->m_Game.m_MidMap.m_nTileSize * pMe->m_Game.m_MidMap.m_nHeight + pMe->m_Game.m_MidMap.m_nY;
		}
		if((pMe->m_Game.m_MidMap.m_nY <= 0) && pMe->m_Game.m_MainSprite.m_sprite[0].y < pMe->DeviceInfo.cyScreen / 4)
		{
			pMe->m_Game.m_MidMap.m_nY = 0;
			pMe->m_Game.bgdy = 0;
		}
	}
	else
	{
		pMe->m_Game.bgdy = 0;
	}

	if(pMe->m_Game.m_MainSprite.m_iKeyState&KEY_LEFT)
	{
		pMe->m_Game.m_MainSprite.m_bIsLeft = TRUE;
		if(!CheckTileCollisionHorizon(pMe))
		{
			pMe->m_Game.m_MainSprite.m_sprite[0].x -= MOVESPEED;	
		}
	}
	else if(pMe->m_Game.m_MainSprite.m_iKeyState&KEY_RIGHT)
	{
		pMe->m_Game.m_MainSprite.m_bIsLeft = FALSE;
		if(!CheckTileCollisionHorizon(pMe))
		{
			pMe->m_Game.m_MainSprite.m_sprite[0].x += MOVESPEED;
		}
	}
	if(pMe->m_Game.m_MainSprite.m_iKeyState&KEY_OK_MASK)
	{
		for(i=0;i<3;i++)
		{
			if(pMe->m_Game.m_StaticSprites[i].unLayer==SPRITE_LAYER_HIDDEN)
			{
				SpriteInit8(pMe->m_Game.m_StaticSprites,i,pMe->m_Game.m_MainSprite.m_sprite[0].x,pMe->m_Game.m_MainSprite.m_sprite[0].y+5,0,SPRITE_LAYER_1);
				break;
			}
		}
	}
	if (pMe->m_Game.m_MainSprite.m_sprite[0].x != pMe->DeviceInfo.cxScreen / 4)
	{
		pMe->m_Game.bgdx =  pMe->DeviceInfo.cxScreen / 4 - pMe->m_Game.m_MainSprite.m_sprite[0].x;
	}
	else
	{
		pMe->m_Game.bgdx = 0;
	}
	if(pMe->m_Game.m_MidMap.m_nX-pMe->m_Game.bgdx<0
		||pMe->m_Game.m_MidMap.m_nX-pMe->m_Game.bgdx>
		pMe->m_Game.m_MidMap.m_nTileSize*pMe->m_Game.m_MidMap.m_nWidth-pMe->DeviceInfo.cxScreen)
	{
		pMe->m_Game.bgdx=0;
	}

	//金币动作
	for(i=0;i<3;i++)
	{
		pMe->m_Game.m_MoveSpriteProp[0].cnt=++pMe->m_Game.m_MoveSpriteProp[0].cnt%4;
		pMe->m_Game.m_MoveSprites[i].unSpriteIndex = pMe->m_Game.m_MoveSpriteProp[0].cnt;
	}
	for(i=0;i<3;i++)
	{
		if(pMe->m_Game.m_StaticSprites[i].unLayer!=SPRITE_LAYER_HIDDEN)
		{
			//运动轨迹
			pMe->m_Game.m_StaticSprites[i].x+=BULLETSPEED;
			if(pMe->m_Game.m_StaticSprites[i].x>pMe->DeviceInfo.cxScreen)
			{
				pMe->m_Game.m_StaticSprites[i].unLayer = SPRITE_LAYER_HIDDEN;
			}
		}
	}

	pMe->m_Game.m_MainSprite.m_sprite[0].x += pMe->m_Game.bgdx;
	pMe->m_Game.m_MainSprite.m_sprite[0].y += pMe->m_Game.bgdy;
	pMe->m_Game.m_MidMap.m_nX -=pMe->m_Game.bgdx;
	pMe->m_Game.m_MidMap.m_nY -=pMe->m_Game.bgdy;
	pMe->m_Game.m_Maps[0].y += pMe->m_Game.bgdy;

	//其它的偏移
	for(i=0;i<sizeof(pMe->m_Game.m_MoveSprites)/sizeof(AEESpriteCmd);i++)
	{
		pMe->m_Game.m_MoveSprites[i].x += pMe->m_Game.bgdx;
		pMe->m_Game.m_MoveSprites[i].y += pMe->m_Game.bgdy;
	}
	//修正坐标
	if(pMe->m_Game.m_MidMap.m_nY<0)
	{
		pMe->m_Game.m_MainSprite.m_sprite[0].y += -pMe->m_Game.m_MidMap.m_nY;
		pMe->m_Game.bgdy = pMe->m_Game.bgdy+pMe->m_Game.m_MidMap.m_nY;
		pMe->m_Game.m_MidMap.m_nY = 0;
		pMe->m_Game.m_Maps[0].y = 0;				
	}
	if(pMe->m_Game.m_MainSprite.m_bIsLeft)
	{
		//pMe->m_Game.m_MainSprite.offset = 5;
		pMe->m_Game.m_MainSprite.offset = 0;
		pMe->m_Game.m_MainSprite.m_sprite[0].unTransform = SPRITE_FLIP_Y;
	}
	else
	{
		pMe->m_Game.m_MainSprite.offset = 0;
		pMe->m_Game.m_MainSprite.m_sprite[0].unTransform = 0;
	}

	if (pMe->m_Game.m_MainSprite.m_sprite[0].x < 0)
	{
		pMe->m_Game.m_MainSprite.m_sprite[0].x = 0;
	}
	if (pMe->m_Game.m_MainSprite.m_sprite[0].x >= pMe->DeviceInfo.cxScreen - 32)
	{
		pMe->m_Game.m_MainSprite.m_sprite[0].x = pMe->DeviceInfo.cxScreen - 32;
	}
	
	if(pMe->m_Game.m_MainSprite.m_iKeyState&KEY_OK_MASK)
	{
		pMe->m_Game.m_MainSprite.m_iKeyState&=~KEY_OK_MASK;
		pMe->m_Game.m_MainSprite.m_sprite[0].unSpriteIndex = pMe->m_Game.m_MainSprite.offset+1;
	}
	else if(pMe->m_Game.m_MainSprite.dy!=0)
	{
		pMe->m_Game.m_MainSprite.m_sprite[0].unSpriteIndex = 4+pMe->m_Game.m_MainSprite.offset;
	}
	else if(pMe->m_Game.m_MainSprite.m_iKeyState&KEY_LEFT
		||pMe->m_Game.m_MainSprite.m_iKeyState&KEY_RIGHT)
	{
		pMe->m_Game.m_MainSprite.m_sprite[0].unSpriteIndex++;
		if (pMe->m_Game.m_MainSprite.m_sprite[0].unSpriteIndex > 3 + pMe->m_Game.m_MainSprite.offset
			|| pMe->m_Game.m_MainSprite.m_sprite[0].unSpriteIndex < 1 + pMe->m_Game.m_MainSprite.offset)
		{
			pMe->m_Game.m_MainSprite.m_sprite[0].unSpriteIndex = 1 +  pMe->m_Game.m_MainSprite.offset;
		}
	}
	else
	{
		pMe->m_Game.m_MainSprite.m_sprite[0].unSpriteIndex = pMe->m_Game.m_MainSprite.offset;
	}
	if(pMe->m_bIsSave)
	{
		pMe->m_bIsSave = FALSE;
		SaveCurrentData(pMe);
	}
}

void MainMenuInit(GameDemo *pMe)
{
	pMe->m_Menu.m_iKeyState = 0;
	pMe->m_Menu.m_bUpdate = TRUE;
	pMe->m_Menu.m_State = enMenuMain;
}
void MainMenuUpdate(GameDemo *pMe)
{
	if(pMe->m_Menu.m_bUpdate)
	{
		int offset = (pMe->DeviceInfo.cyScreen-4*pMe->m_nFontHeight)/2;
		int dx = (pMe->DeviceInfo.cxScreen-6*pMe->m_nFontHeight)/2;
		AEERect rect;
		rect.x = 0;
		rect.y = 0;
		rect.dx = pMe->DeviceInfo.cxScreen;
		rect.dy = pMe->DeviceInfo.cyScreen;
		IDISPLAY_FillRect(pMe->pIDisplay,&rect,MAKE_RGB(255,255,255));
		pMe->m_Menu.m_bUpdate = FALSE;
		DrawTextCenterById(pMe,IDS_MENU_BEGIN,offset,MAKE_RGB(0,0,0));
		DrawTextCenterById(pMe,IDS_MENU_LOAD,offset+1*pMe->m_nFontHeight,MAKE_RGB(0,0,0));
		DrawTextCenterById(pMe,IDS_MENU_HELP,offset+2*pMe->m_nFontHeight,MAKE_RGB(0,0,0));
		if(pMe->m_bVolOn)
		{
			DrawTextCenterById(pMe,IDS_MENU_VOL_ON,offset+3*pMe->m_nFontHeight,MAKE_RGB(0,0,0));
		}
		else
		{
			DrawTextCenterById(pMe,IDS_MENU_VOL_OFF,offset+3*pMe->m_nFontHeight,MAKE_RGB(0,0,0));
		}
		DrawTextCenterById(pMe,IDS_MENU_EXIT,offset+4*pMe->m_nFontHeight,MAKE_RGB(0,0,0));
		DrawTextById(pMe,IDS_MENU_SELECT,dx,offset+pMe->m_Menu.m_nWhichActive*pMe->m_nFontHeight,MAKE_RGB(0,0,0));
	}
}
void MainGameLoadResource(GameDemo *pMe)
{
	IBitmap *pbmScreen = NULL;
	IBitmap *pbmDdb;
	IBitmap *pbmDib = NULL;
	NativeColor color;
	AEEBitmapInfo bi;
	pbmScreen = IDISPLAY_GetDestination(pMe->pIDisplay);
	ISHELL_CreateInstance(pMe->pIShell,AEECLSID_SPRITE,(void**)&pMe->m_Game.m_pISprite);

	pbmDib = ISHELL_LoadBitmap(pMe->pIShell,"map1.bmp");
	IBITMAP_GetInfo(pbmDib, &bi, sizeof(bi));
	IBITMAP_CreateCompatibleBitmap(pbmScreen, &pbmDdb, (uint16)bi.cx, (uint16)bi.cy);
	IBITMAP_BltIn(pbmDdb, 0, 0, (uint16)bi.cx, (uint16)bi.cy, pbmDib, 0, 0, AEE_RO_COPY);
	IBITMAP_Release(pbmDib);
	IBITMAP_GetPixel(pbmDdb, 0, 0, &color);
	IBITMAP_SetTransparencyColor(pbmDdb, color);
	ISPRITE_SetTileBuffer(pMe->m_Game.m_pISprite,TILE_SIZE_32X32,pbmDdb);
	IBITMAP_Release(pbmDdb);

	pbmDib = ISHELL_LoadBitmap(pMe->pIShell,"sprite8.bmp");
	IBITMAP_GetInfo(pbmDib, &bi, sizeof(bi));
	IBITMAP_CreateCompatibleBitmap(pbmScreen, &pbmDdb, (uint16)bi.cx, (uint16)bi.cy);
	IBITMAP_BltIn(pbmDdb, 0, 0, (uint16)bi.cx, (uint16)bi.cy, pbmDib, 0, 0, AEE_RO_COPY);
	IBITMAP_Release(pbmDib);
	IBITMAP_GetPixel(pbmDdb, 0, 0, &color);
	IBITMAP_SetTransparencyColor(pbmDdb, color);
	ISPRITE_SetSpriteBuffer(pMe->m_Game.m_pISprite,SPRITE_SIZE_8X8,pbmDdb);
	IBITMAP_Release(pbmDdb);

	pbmDib = ISHELL_LoadBitmap(pMe->pIShell,"sprite32.bmp");
	IBITMAP_GetInfo(pbmDib, &bi, sizeof(bi));
	IBITMAP_CreateCompatibleBitmap(pbmScreen, &pbmDdb, (uint16)bi.cx, (uint16)bi.cy);
	IBITMAP_BltIn(pbmDdb, 0, 0, (uint16)bi.cx, (uint16)bi.cy, pbmDib, 0, 0, AEE_RO_COPY);
	IBITMAP_Release(pbmDib);
	IBITMAP_GetPixel(pbmDdb, 0, 0, &color);
	IBITMAP_SetTransparencyColor(pbmDdb, color);
	ISPRITE_SetSpriteBuffer(pMe->m_Game.m_pISprite,SPRITE_SIZE_32X32,pbmDdb);
	IBITMAP_Release(pbmDdb);

	ISPRITE_SetDestination(pMe->m_Game.m_pISprite,pbmScreen);


	ISHELL_CreateInstance(pMe->pIShell,AEECLSID_SPRITE,(void**)&(pMe->m_Game.m_MainSprite.m_pISprite));
	pbmDib = ISHELL_LoadBitmap(pMe->pIShell,"main32.bmp");
	IBITMAP_GetInfo(pbmDib, &bi, sizeof(bi));
	IBITMAP_CreateCompatibleBitmap(pbmScreen, &pbmDdb, (uint16)bi.cx, (uint16)bi.cy);
	IBITMAP_BltIn(pbmDdb, 0, 0, (uint16)bi.cx, (uint16)bi.cy, pbmDib, 0, 0, AEE_RO_COPY);
	IBITMAP_Release(pbmDib);
	pbmDib = NULL;
	IBITMAP_GetPixel(pbmDdb, 0, 0, &color);
	IBITMAP_SetTransparencyColor(pbmDdb, color);
	ISPRITE_SetSpriteBuffer(pMe->m_Game.m_MainSprite.m_pISprite,SPRITE_SIZE_32X32,pbmDdb);
	IBITMAP_Release(pbmDdb);
	ISPRITE_SetDestination(pMe->m_Game.m_MainSprite.m_pISprite,pbmScreen);

	pbmDib = ISHELL_LoadBitmap(pMe->pIShell,"map2.bmp");
	IBITMAP_GetInfo(pbmDib, &bi, sizeof(bi));
	IBITMAP_CreateCompatibleBitmap(pbmScreen, &pMe->m_Game.m_MidMap.m_pIBmp, (uint16)bi.cx, (uint16)bi.cy);
	IBITMAP_BltIn(pMe->m_Game.m_MidMap.m_pIBmp, 0, 0, (uint16)bi.cx, (uint16)bi.cy, pbmDib, 0, 0, AEE_RO_COPY);
	IBITMAP_Release(pbmDib);
	IBITMAP_GetPixel(pMe->m_Game.m_MidMap.m_pIBmp, 0, 0, &color);
	IBITMAP_SetTransparencyColor(pMe->m_Game.m_MidMap.m_pIBmp, color);


	IBITMAP_Release(pbmScreen);
}

void SpriteInit8(AEESpriteCmd *pSprites,int index,int16 x,int16 y,int spriteIndex,uint8 unLayer)
{
	pSprites[index].unSpriteIndex = spriteIndex;
	pSprites[index].unSpriteSize = SPRITE_SIZE_8X8;
	pSprites[index].unComposite = COMPOSITE_KEYCOLOR;
	pSprites[index].unLayer = unLayer;
	pSprites[index].x = x;
	pSprites[index].y = y;
}
void SpriteInit16(AEESpriteCmd *pSprites,int index,int16 x,int16 y,int spriteIndex,uint8 unLayer)
{
	pSprites[index].unSpriteIndex = spriteIndex;
	pSprites[index].unSpriteSize = SPRITE_SIZE_16X16;
	pSprites[index].unComposite = COMPOSITE_KEYCOLOR;
	pSprites[index].unLayer = unLayer;
	pSprites[index].x = x;
	pSprites[index].y = y;
}
void SpriteInit32(AEESpriteCmd *pSprites,int index,int16 x,int16 y,int spriteIndex,uint8 unLayer)
{
	pSprites[index].unSpriteIndex = spriteIndex;
	pSprites[index].unSpriteSize = SPRITE_SIZE_32X32;
	pSprites[index].unComposite = COMPOSITE_KEYCOLOR;
	pSprites[index].unLayer = unLayer;
	pSprites[index].x = x;
	pSprites[index].y = y;
}

void InitGameOffset(GameDemo *pMe)
{
	int i;
	for(i=0;i<sizeof(pMe->m_Game.m_MoveSprites)/sizeof(AEESpriteCmd);i++)
	{
		pMe->m_Game.m_MoveSprites[i].x -= pMe->m_Game.m_MidMap.m_nX;
		pMe->m_Game.m_MoveSprites[i].y -= pMe->m_Game.m_MidMap.m_nY;
	}
}
void MainGameDataInit(GameDemo *pMe)
{
	pMe->m_Game.m_Maps[0].pMapArray = (uint16*)game_map_1;
	pMe->m_Game.m_Maps[0].unTileSize = TILE_SIZE_32X32;
	pMe->m_Game.m_Maps[0].unFlags= MAP_FLAG_WRAP;
	pMe->m_Game.m_Maps[0].x = 0;
	pMe->m_Game.m_Maps[0].y= pMe->DeviceInfo.cyScreen-256;
	pMe->m_Game.m_Maps[0].w = MAP_SIZE_8;
	pMe->m_Game.m_Maps[0].h = MAP_SIZE_8;
	pMe->m_Game.m_Maps[1].pMapArray = NULL;

	pMe->m_Game.m_MidMap.m_pMapArray = (uint16*)game_map_2;
	pMe->m_Game.m_MidMap.m_nHeight = 17;
	pMe->m_Game.m_MidMap.m_nWidth = 128;
	pMe->m_Game.m_MidMap.m_nTileSize = 16;
	pMe->m_Game.m_MidMap.m_nX = 0;
	pMe->m_Game.m_MidMap.m_nY = 17*16-pMe->DeviceInfo.cyScreen;

	pMe->m_Game.m_MainSprite.m_sprite[0].unComposite = COMPOSITE_KEYCOLOR;
	pMe->m_Game.m_MainSprite.m_sprite[0].unLayer = SPRITE_LAYER_0;
	pMe->m_Game.m_MainSprite.m_sprite[0].unSpriteSize = SPRITE_SIZE_32X32;
	pMe->m_Game.m_MainSprite.m_sprite[0].unSpriteIndex = 0;
	pMe->m_Game.m_MainSprite.m_sprite[0].x = 0;
	pMe->m_Game.m_MainSprite.m_sprite[0].y = 0;

	pMe->m_Game.m_MainSprite.m_sprite[1].unSpriteSize = SPRITE_SIZE_END;

	//金币
	SpriteInit8(pMe->m_Game.m_MoveSprites,0,16*5,16*11,0,SPRITE_LAYER_0);
	SpriteInit8(pMe->m_Game.m_MoveSprites,1,16*20,16*9,0,SPRITE_LAYER_0);
	SpriteInit8(pMe->m_Game.m_MoveSprites,2,16*15,16*10,0,SPRITE_LAYER_0);

	//血
	SpriteInit8(pMe->m_Game.m_MoveSprites,3,16*10,16*11,5,SPRITE_LAYER_0);
	SpriteInit8(pMe->m_Game.m_MoveSprites,4,16*25,16*9,5,SPRITE_LAYER_0);

	//敌人
	SpriteInit32(pMe->m_Game.m_MoveSprites,6,16*3,16*12,0,SPRITE_LAYER_0);
	pMe->m_Game.m_MoveSpriteProp[6].originX = 16*3;
	pMe->m_Game.m_MoveSpriteProp[6].interval = 40;
	pMe->m_Game.m_MoveSpriteProp[6].isLeft = TRUE;
	pMe->m_Game.m_MoveSpriteProp[6].offset = 0;

	SpriteInit32(pMe->m_Game.m_MoveSprites,7,16*23,16*12,0,SPRITE_LAYER_0);
	pMe->m_Game.m_MoveSpriteProp[7].originX = 16*23;
	pMe->m_Game.m_MoveSpriteProp[7].interval = 80;
	pMe->m_Game.m_MoveSpriteProp[7].isLeft = TRUE;
	pMe->m_Game.m_MoveSpriteProp[7].offset = 0;

	pMe->m_Game.m_MoveSprites[8].unSpriteSize = SPRITE_SIZE_END;

	//子弹
	SpriteInit8(pMe->m_Game.m_StaticSprites,0,16*5,16*11,4,SPRITE_LAYER_HIDDEN);
	SpriteInit8(pMe->m_Game.m_StaticSprites,1,16*20,16*9,4,SPRITE_LAYER_HIDDEN);
	SpriteInit8(pMe->m_Game.m_StaticSprites,2,16*15,16*10,4,SPRITE_LAYER_HIDDEN);

	//血图标
	SpriteInit8(pMe->m_Game.m_StaticSprites,3,0,0,5,SPRITE_LAYER_1);
	SpriteInit8(pMe->m_Game.m_StaticSprites,4,8,0,5,SPRITE_LAYER_1);
	SpriteInit8(pMe->m_Game.m_StaticSprites,5,8*2,0,5,SPRITE_LAYER_1);
	pMe->m_Game.m_StaticSprites[6].unSpriteSize = SPRITE_SIZE_END;

	


	InitGameOffset(pMe);
}
void MainGameInit(GameDemo *pMe)
{
	MainGameLoadResource(pMe);
	MainGameDataInit(pMe);
}

void MainGameFree(GameDemo *pMe)
{
	if(pMe->m_Game.m_pISprite)
	{
		ISPRITE_Release(pMe->m_Game.m_pISprite);
		pMe->m_Game.m_pISprite = NULL;
	}
	if(pMe->m_Game.m_MainSprite.m_pISprite)
	{
		ISPRITE_Release(pMe->m_Game.m_MainSprite.m_pISprite);
		pMe->m_Game.m_MainSprite.m_pISprite = NULL;
	}
	if(pMe->m_Game.m_MidMap.m_pIBmp)
	{
		IBITMAP_Release(pMe->m_Game.m_MidMap.m_pIBmp);
		pMe->m_Game.m_MidMap.m_pIBmp = NULL;
	}
}

void MainGameUpdate(GameDemo *pMe)
{
	ISPRITE_DrawTiles(pMe->m_Game.m_pISprite,pMe->m_Game.m_Maps);
	MainAppDrawMidMap(pMe,&pMe->m_Game.m_MidMap,TRUE);
	ISPRITE_DrawSprites(pMe->m_Game.m_pISprite,pMe->m_Game.m_MoveSprites);
	ISPRITE_DrawSprites(pMe->m_Game.m_pISprite,pMe->m_Game.m_StaticSprites);
	ISPRITE_DrawSprites(pMe->m_Game.m_MainSprite.m_pISprite,pMe->m_Game.m_MainSprite.m_sprite);
}

static void MainAppDrawMidMap(GameDemo *pMe,Map *pMidMap,boolean isTransparent)
{
	int beginX = 0;
	int beginY = 0;
	int nX;
	int nY;
	int tempY;
	int tempX;
	int arrayNum;

	nX=pMidMap->m_nX%pMidMap->m_nTileSize;
	nY=pMidMap->m_nY%pMidMap->m_nTileSize;

	while(TRUE)
	{
		while(TRUE)
		{
			tempY=((beginY+pMidMap->m_nY)/pMidMap->m_nTileSize)%pMidMap->m_nHeight;
			tempX=((beginX+pMidMap->m_nX)/pMidMap->m_nTileSize)%pMidMap->m_nWidth;
			arrayNum=	pMidMap->m_pMapArray[tempX+tempY*pMidMap->m_nWidth]&(0x00FF);
			if(arrayNum!=0xff)
			{
				if(isTransparent)
				{
					IDISPLAY_BitBlt(pMe->pIDisplay,
						beginX-nX,beginY-nY,
						pMidMap->m_nTileSize,pMidMap->m_nTileSize,
						pMidMap->m_pIBmp,
						0,pMidMap->m_nTileSize*arrayNum,
						AEE_RO_TRANSPARENT);
				}
				else
				{
					IDISPLAY_BitBlt(pMe->pIDisplay,
						beginX-nX,beginY-nY,
						pMidMap->m_nTileSize,pMidMap->m_nTileSize,
						pMidMap->m_pIBmp,
						0,pMidMap->m_nTileSize*arrayNum,
						AEE_RO_COPY);
				}

			}
			if(beginX<pMe->DeviceInfo.cxScreen)
			{
				beginX+=pMidMap->m_nTileSize;
			}
			else
			{
				break;
			}
		}
		if(beginY<pMe->DeviceInfo.cyScreen)
		{
			beginY+=pMidMap->m_nTileSize;
			beginX=0;
		}
		else
		{
			break;
		}
	}
}

void CheckTileCollisionVertical(GameDemo *pMe)
{
	int x = pMe->m_Game.m_MainSprite.m_sprite[0].x;
	int y = pMe->m_Game.m_MainSprite.m_sprite[0].y;
	int dy = pMe->m_Game.m_MainSprite.dy;
	int i;
	int xTile1;
	int xTile2;
	Map *pMap = &pMe->m_Game.m_MidMap;
	if (dy > 0)
	{
		for ( i = (y + pMap->m_nY+15) /
			pMap->m_nTileSize + 2;
			i <= (y + pMap->m_nY + dy) / pMap->m_nTileSize + 2; i++)
		{
			xTile1 = (x + pMap->m_nX + 12) / pMap->m_nTileSize;
			xTile2 = (x + pMap->m_nX + 20) /pMap->m_nTileSize;
			if (i < 0 || i > pMap->m_nHeight - 1)
			{
				continue;
			}
			if (xTile2 > pMap->m_nWidth - 1 || xTile1 < 0)
			{
				return ;
			}
			if(((pMap->m_pMapArray[xTile1+i*pMap->m_nWidth]>>8)&0x02)==0x02)
			{
				pMe->m_Game.m_MainSprite.dy=(i-2)*pMap->m_nTileSize-(y+pMap->m_nY);	
				pMe->m_Game.m_MainSprite.m_iKeyState &=~KEY_UP_MASK;
				break;
			}
			if(((pMap->m_pMapArray[xTile2+i*pMap->m_nWidth]>>8)&0x02)==0x02)
			{
				pMe->m_Game.m_MainSprite.dy=(i-2)*pMap->m_nTileSize-(y+pMap->m_nY);	
				pMe->m_Game.m_MainSprite.m_iKeyState &=~KEY_UP_MASK;
				break;
			}
		}
	}
	else if (pMe->m_Game.m_MainSprite.dy < 0)
	{
		for ( i = (y + pMap->m_nY+dy) /pMap->m_nTileSize;
			i <= (y + pMap->m_nY) / pMap->m_nTileSize; i++)
		{
			xTile1 = (x + pMap->m_nX + 12) / pMap->m_nTileSize;
			xTile2 = (x + pMap->m_nX + 20) /pMap->m_nTileSize;
			if (i < 0 || i > pMap->m_nHeight - 1)
			{
				continue;
			}
			if (xTile2 > pMap->m_nWidth - 1 || xTile1 < 0)
			{
				return ;
			}
			if(((pMap->m_pMapArray[xTile1+i*pMap->m_nWidth]>>8)&0x01)==0x01)
			{
				pMe->m_Game.m_MainSprite.dy = 0;
				pMe->m_Game.m_MainSprite.m_iKeyState &=~KEY_UP_MASK;
				break;
			}
			if(((pMap->m_pMapArray[xTile2+i*pMap->m_nWidth]>>8)&0x01)==0x01)
			{
				pMe->m_Game.m_MainSprite.dy = 0;
				pMe->m_Game.m_MainSprite.m_iKeyState &=~KEY_UP_MASK;
				break;
			}
		}
	}
}
boolean CheckTileCollisionHorizon(GameDemo *pMe)
{
	int i;
	int yTile1;
	int yTile2;
	int yTile3;
	AEERect mainRect;
	AEERect tileRect;
	
	mainRect.x = pMe->m_Game.m_MainSprite.m_sprite[0].x+pMe->m_Game.m_MidMap.m_nX+7;
	mainRect.y = pMe->m_Game.m_MainSprite.m_sprite[0].y+pMe->m_Game.m_MidMap.m_nY+1;
	mainRect.dx = 16;
	mainRect.dy = 31;

	tileRect.dx = 16;
	tileRect.dy = 16;

	if(pMe->m_Game.m_MainSprite.m_bIsLeft)
	{
		mainRect.x = pMe->m_Game.m_MainSprite.m_sprite[0].x+pMe->m_Game.m_MidMap.m_nX+7-MOVESPEED;
		i=(pMe->m_Game.m_MainSprite.m_sprite[0].x+pMe->m_Game.m_MidMap.m_nX+7-MOVESPEED)/pMe->m_Game.m_MidMap.m_nTileSize;
		{
			if(i<0||i>pMe->m_Game.m_MidMap.m_nWidth-1)
			{
				return FALSE;
			}
			yTile1=(pMe->m_Game.m_MainSprite.m_sprite[0].y+pMe->m_Game.m_MidMap.m_nY)/pMe->m_Game.m_MidMap.m_nTileSize;
			yTile2=(pMe->m_Game.m_MainSprite.m_sprite[0].y+pMe->m_Game.m_MidMap.m_nY)/pMe->m_Game.m_MidMap.m_nTileSize+1;
			yTile3=(pMe->m_Game.m_MainSprite.m_sprite[0].y+pMe->m_Game.m_MidMap.m_nY)/pMe->m_Game.m_MidMap.m_nTileSize+2;
			if(yTile1<0||yTile3>pMe->m_Game.m_MidMap.m_nHeight-1)
			{
				return FALSE;
			}
			if(((pMe->m_Game.m_MidMap.m_pMapArray[i+yTile1*pMe->m_Game.m_MidMap.m_nWidth]>>8)&0x04)==0x04)
			{
				tileRect.x=i*16;
				tileRect.y=yTile1*16;	
				if(IntersectRect(&mainRect,&tileRect))
				{
					return TRUE;
				}
			}
			if(((pMe->m_Game.m_MidMap.m_pMapArray[i+yTile2*pMe->m_Game.m_MidMap.m_nWidth]>>8)&0x04)==0x04)
			{
				tileRect.x=i*16;
				tileRect.y=yTile2*16;
				if(IntersectRect(&mainRect,&tileRect))
				{
					return TRUE;
				}
			}
			if(((pMe->m_Game.m_MidMap.m_pMapArray[i+yTile3*pMe->m_Game.m_MidMap.m_nWidth]>>8)&0x04)==0x04)
			{
				tileRect.x=i*16;
				tileRect.y=yTile3*16;
				if(IntersectRect(&mainRect,&tileRect))
				{
					return TRUE;
				}
			}
		}
	}
	else
	{ 
		mainRect.x = pMe->m_Game.m_MainSprite.m_sprite[0].x +pMe->m_Game.m_MidMap.m_nX+7+MOVESPEED;
		i = (pMe->m_Game.m_MainSprite.m_sprite[0].x+pMe->m_Game.m_MidMap.m_nX-7+MOVESPEED)/pMe->m_Game.m_MidMap.m_nTileSize+2;
		{
			if(i<0||i>pMe->m_Game.m_MidMap.m_nWidth-1)
			{
				return FALSE;
			}
			yTile1=(pMe->m_Game.m_MainSprite.m_sprite[0].y+pMe->m_Game.m_MidMap.m_nY)/pMe->m_Game.m_MidMap.m_nTileSize;
			yTile2=(pMe->m_Game.m_MainSprite.m_sprite[0].y+pMe->m_Game.m_MidMap.m_nY)/pMe->m_Game.m_MidMap.m_nTileSize+1;		
			yTile3=(pMe->m_Game.m_MainSprite.m_sprite[0].y+pMe->m_Game.m_MidMap.m_nY)/pMe->m_Game.m_MidMap.m_nTileSize+2;		
			if(yTile1<0||yTile3>pMe->m_Game.m_MidMap.m_nHeight-1)
			{
				return FALSE;
			}
			if(((pMe->m_Game.m_MidMap.m_pMapArray[i+yTile1*pMe->m_Game.m_MidMap.m_nWidth]>>8)&0x04)==0x04)
			{
				//判断矩形是否有交点
				tileRect.x=i*16;
				tileRect.y=yTile1*16;	
				if(IntersectRect(&mainRect,&tileRect))
				{
					return TRUE;
				}
			}
			if(((pMe->m_Game.m_MidMap.m_pMapArray[i+yTile2*pMe->m_Game.m_MidMap.m_nWidth]>>8)&0x04)==0x04)
			{
				//判断矩形是否有交点
				tileRect.x=i*16;
				tileRect.y=yTile2*16;
				if(IntersectRect(&mainRect,&tileRect))
				{
					return TRUE;
				}
			}
			if(((pMe->m_Game.m_MidMap.m_pMapArray[i+yTile3*pMe->m_Game.m_MidMap.m_nWidth]>>8)&0x04)==0x04)
			{
				//判断矩形是否有交点
				tileRect.x=i*16;
				tileRect.y=yTile3*16;
				if(IntersectRect(&mainRect,&tileRect))
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

boolean IntersectRect(AEERect *pRect1,AEERect *pRect2)
{
	if((pRect1->x + pRect1->dx > pRect2->x)&&(pRect1->y + pRect1->dy > pRect2->y)
		&&(pRect2->x + pRect2->dx > pRect1->x)&&(pRect2->y + pRect2->dy > pRect1->y))
	{
		return TRUE;
	}
	return FALSE;
}

void EnemyAction(GameDemo *pMe,AEESpriteCmd *pSprites,int index,int num)
{
	int i;
	for(i=index;i<index+num;i++)
	{
		if(pSprites[i].unLayer!=SPRITE_LAYER_HIDDEN)
		{        
			if(pMe->m_Game.m_MoveSpriteProp[i].isLeft)
			{
				pSprites[i].x-=4;
				if(pMe->m_Game.m_MoveSpriteProp[i].originX-(pMe->m_Game.m_MidMap.m_nX+pSprites[i].x)>pMe->m_Game.m_MoveSpriteProp[i].interval)
				{
					pMe->m_Game.m_MoveSpriteProp[i].isLeft=FALSE;
					pMe->m_Game.m_MoveSpriteProp[i].offset=3;
				}
			}
			else
			{
				pSprites[i].x+=4;
				if((pSprites[i].x+pMe->m_Game.m_MidMap.m_nX)-pMe->m_Game.m_MoveSpriteProp[i].originX>pMe->m_Game.m_MoveSpriteProp[i].interval)
				{
					pMe->m_Game.m_MoveSpriteProp[i].isLeft=TRUE;
					pMe->m_Game.m_MoveSpriteProp[i].offset=0;
				}
			}
			pSprites[i].unSpriteIndex++;
			if(pSprites[i].unSpriteIndex>2+pMe->m_Game.m_MoveSpriteProp[i].offset)
			{
				pSprites[i].unSpriteIndex=pMe->m_Game.m_MoveSpriteProp[i].offset;
			}
		}
	}
}

void CheckBulletEnemyCollision(GameDemo *pMe)
{
	//子弹与敌人的碰撞
	int i,j;
	AEERect rect1;
	AEERect rect2;
	rect1.dx = 8;
	rect1.dy = 8;
	rect2.dx = 20;
	rect2.dy = 20;

	for(i=0;i<3;i++)
	{
		if(pMe->m_Game.m_StaticSprites[i].unLayer!=SPRITE_LAYER_HIDDEN)
		{
			rect1.x = pMe->m_Game.m_StaticSprites[i].x;
			rect1.y = pMe->m_Game.m_StaticSprites[i].y;
			for(j=6;j<8;j++)
			{
				if(pMe->m_Game.m_MoveSprites[j].unLayer!=SPRITE_LAYER_HIDDEN)
				{
					rect2.x = pMe->m_Game.m_MoveSprites[j].x;
					rect2.y = pMe->m_Game.m_MoveSprites[j].y;
					if(IntersectRect(&rect1,&rect2))
					{
						pMe->m_Game.m_StaticSprites[i].unLayer = SPRITE_LAYER_HIDDEN;
						pMe->m_Game.m_MoveSprites[j].unLayer = SPRITE_LAYER_HIDDEN;
						PlayGameMedia(pMe);
						break;

					}
				}
			}
		}
	}
}
void CheckMainSpriteEnemyCollision(GameDemo *pMe)
{
	//敌人与主角的碰撞
	int i;
	AEERect rect1;
	AEERect rect2;
	rect1.dx = 20;
	rect1.dy = 32;
	rect2.dx = 20;
	rect2.dy = 32;
	rect1.x = pMe->m_Game.m_MainSprite.m_sprite[0].x;
	rect1.y = pMe->m_Game.m_MainSprite.m_sprite[0].y;
	for(i=6;i<8;i++)
	{
		if(pMe->m_Game.m_MoveSprites[i].unLayer!=SPRITE_LAYER_HIDDEN)
		{
			rect2.x = pMe->m_Game.m_MoveSprites[i].x;
			rect2.y = pMe->m_Game.m_MoveSprites[i].y;
			if(IntersectRect(&rect1,&rect2))
			{
				PlayGameMedia(pMe);
				MainGameDataInit(pMe);
				break;
			}
		}
	}
}

void SaveCurrentData(GameDemo *pMe)
{
	IFileMgr *pIFileMgr;
	IFile *pIFile;
	ISHELL_CreateInstance(pMe->pIShell,AEECLSID_FILEMGR,(void**)&pIFileMgr);
	if(IFILEMGR_Test(pIFileMgr,"save.dat")==SUCCESS)
	{
		pIFile = IFILEMGR_OpenFile(pIFileMgr,"save.dat",_OFM_READWRITE);
	}
	else
	{
		pIFile = IFILEMGR_OpenFile(pIFileMgr,"save.dat",_OFM_CREATE);
	}
	if(IFILE_Write(pIFile,pMe->m_Game.m_MoveSprites,sizeof(pMe->m_Game.m_MoveSprites))!=sizeof(pMe->m_Game.m_MoveSprites))
	{
		IFILE_Release(pIFile);
		IFILEMGR_Remove(pIFileMgr,"save.dat");
		IFILEMGR_Release(pIFileMgr);
		return;
	}
	if(IFILE_Write(pIFile,pMe->m_Game.m_StaticSprites,sizeof(pMe->m_Game.m_StaticSprites))!=sizeof(pMe->m_Game.m_StaticSprites))
	{
		IFILE_Release(pIFile);
		IFILEMGR_Remove(pIFileMgr,"save.dat");
		IFILEMGR_Release(pIFileMgr);
		return;
	}
	if(IFILE_Write(pIFile,pMe->m_Game.m_MoveSpriteProp,sizeof(pMe->m_Game.m_MoveSpriteProp))!=sizeof(pMe->m_Game.m_MoveSpriteProp))
	{
		IFILE_Release(pIFile);
		IFILEMGR_Remove(pIFileMgr,"save.dat");
		IFILEMGR_Release(pIFileMgr);
		return;
	}
	if(IFILE_Write(pIFile,pMe->m_Game.m_StaticSpriteProp,sizeof(pMe->m_Game.m_StaticSpriteProp))!=sizeof(pMe->m_Game.m_StaticSpriteProp))
	{
		IFILE_Release(pIFile);
		IFILEMGR_Remove(pIFileMgr,"save.dat");
		IFILEMGR_Release(pIFileMgr);
		return;
	}
	if(IFILE_Write(pIFile,&pMe->m_Game.m_MidMap.m_nX,sizeof(pMe->m_Game.m_MidMap.m_nX))!=sizeof(pMe->m_Game.m_MidMap.m_nX))
	{
		IFILE_Release(pIFile);
		IFILEMGR_Remove(pIFileMgr,"save.dat");
		IFILEMGR_Release(pIFileMgr);
		return;
	}
	if(IFILE_Write(pIFile,&pMe->m_Game.m_MidMap.m_nY,sizeof(pMe->m_Game.m_MidMap.m_nY))!=sizeof(pMe->m_Game.m_MidMap.m_nY))
	{
		IFILE_Release(pIFile);
		IFILEMGR_Remove(pIFileMgr,"save.dat");
		IFILEMGR_Release(pIFileMgr);
		return;
	}
	if(IFILE_Write(pIFile,&pMe->m_Game.m_MainSprite.dy,sizeof(pMe->m_Game.m_MainSprite.dy))!=sizeof(pMe->m_Game.m_MainSprite.dy))
	{
		IFILE_Release(pIFile);
		IFILEMGR_Remove(pIFileMgr,"save.dat");
		IFILEMGR_Release(pIFileMgr);
		return;
	}
	if(IFILE_Write(pIFile,&pMe->m_Game.m_MainSprite.m_bIsLeft,sizeof(pMe->m_Game.m_MainSprite.m_bIsLeft))!=sizeof(pMe->m_Game.m_MainSprite.m_bIsLeft))
	{
		IFILE_Release(pIFile);
		IFILEMGR_Remove(pIFileMgr,"save.dat");
		IFILEMGR_Release(pIFileMgr);
		return;
	}

	if(IFILE_Write(pIFile,pMe->m_Game.m_MainSprite.m_sprite,sizeof(pMe->m_Game.m_MainSprite.m_sprite))!=sizeof(pMe->m_Game.m_MainSprite.m_sprite))
	{
		IFILE_Release(pIFile);
		IFILEMGR_Remove(pIFileMgr,"save.dat");
		IFILEMGR_Release(pIFileMgr);
		return;
	}
	if(IFILE_Write(pIFile,&pMe->m_Game.m_MainSprite.offset,sizeof(pMe->m_Game.m_MainSprite.offset))!=sizeof(pMe->m_Game.m_MainSprite.offset))
	{
		IFILE_Release(pIFile);
		IFILEMGR_Remove(pIFileMgr,"save.dat");
		IFILEMGR_Release(pIFileMgr);
		return;
	}
	IFILE_Release(pIFile);
	IFILEMGR_Release(pIFileMgr);
}
void LoadSaveData(GameDemo *pMe)
{
	IFileMgr *pIFileMgr;
	IFile *pIFile;
	ISHELL_CreateInstance(pMe->pIShell,AEECLSID_FILEMGR,(void**)&pIFileMgr);
	if(IFILEMGR_Test(pIFileMgr,"save.dat")==SUCCESS)
	{
		pIFile = IFILEMGR_OpenFile(pIFileMgr,"save.dat",_OFM_READWRITE);
	}
	else
	{
		IFILEMGR_Release(pIFileMgr);
		return ;
	}
	IFILE_Read(pIFile,pMe->m_Game.m_MoveSprites,sizeof(pMe->m_Game.m_MoveSprites));
	IFILE_Read(pIFile,pMe->m_Game.m_StaticSprites,sizeof(pMe->m_Game.m_StaticSprites));
	IFILE_Read(pIFile,pMe->m_Game.m_MoveSpriteProp,sizeof(pMe->m_Game.m_MoveSpriteProp));
	IFILE_Read(pIFile,pMe->m_Game.m_StaticSpriteProp,sizeof(pMe->m_Game.m_StaticSpriteProp));
	IFILE_Read(pIFile,&pMe->m_Game.m_MidMap.m_nX,sizeof(pMe->m_Game.m_MidMap.m_nX));
	IFILE_Read(pIFile,&pMe->m_Game.m_MidMap.m_nY,sizeof(pMe->m_Game.m_MidMap.m_nY));

	IFILE_Read(pIFile,&pMe->m_Game.m_MainSprite.dy,sizeof(pMe->m_Game.m_MainSprite.dy));
	IFILE_Read(pIFile,&pMe->m_Game.m_MainSprite.m_bIsLeft,sizeof(pMe->m_Game.m_MainSprite.m_bIsLeft));
	IFILE_Read(pIFile,pMe->m_Game.m_MainSprite.m_sprite,sizeof(pMe->m_Game.m_MainSprite.m_sprite));
	IFILE_Read(pIFile,&pMe->m_Game.m_MainSprite.offset,sizeof(pMe->m_Game.m_MainSprite.offset));

	IFILE_Release(pIFile);
	IFILEMGR_Release(pIFileMgr);
}

static void MediaNotify(void *pUser,AEEMediaCmdNotify *pCmdNotify)
{
	GameDemo *pMe = (GameDemo*)pUser;
	if(pMe->m_bIsContinue)
	{
		if( pCmdNotify->nCmd == MM_CMD_PLAY)
		{
			if(pCmdNotify->nStatus == MM_STATUS_DONE)
			{
				IMEDIA_Play(pMe->m_pIMedia);
			}
		}
	}
}

void PlayBgMedia(GameDemo *pMe)
{
	if(pMe->m_bVolOn)
	{
		AEEMediaData pmd;
		if(pMe->m_pIMedia)
		{
			IMEDIA_Stop(pMe->m_pIMedia);
			IMEDIA_RegisterNotify(pMe->m_pIMedia,NULL,NULL);
			IMEDIA_Release(pMe->m_pIMedia);
		}
		ISHELL_CreateInstance(pMe->pIShell,AEECLSID_MEDIAMIDI,(void**)&pMe->m_pIMedia);
		pmd.clsData = MMD_FILE_NAME;
		pmd.pData = (void*)"begin.mid";
		pmd.dwSize=0;
		IMEDIA_SetMediaData(pMe->m_pIMedia, &pmd);
		IMEDIA_RegisterNotify(pMe->m_pIMedia,MediaNotify,pMe);
		pMe->m_bIsContinue = TRUE;
		IMEDIA_Play(pMe->m_pIMedia);
	}
}

void PlayGameMedia(GameDemo *pMe)
{
	if(pMe->m_bVolOn)
	{
		AEEMediaData pmd;
		if(pMe->m_pIMedia)
		{
			IMEDIA_Stop(pMe->m_pIMedia);
			IMEDIA_RegisterNotify(pMe->m_pIMedia,NULL,NULL);
			IMEDIA_Release(pMe->m_pIMedia);
		}
		ISHELL_CreateInstance(pMe->pIShell,AEECLSID_MEDIAMMF,(void**)&pMe->m_pIMedia);
		pmd.clsData = MMD_FILE_NAME;
		pmd.pData = (void*)"beat.mmf";
		pmd.dwSize=0;
		IMEDIA_SetMediaData(pMe->m_pIMedia, &pmd);
		IMEDIA_RegisterNotify(pMe->m_pIMedia,MediaNotify,pMe);
		pMe->m_bIsContinue = FALSE;
		IMEDIA_Play(pMe->m_pIMedia);
	}
}