#ifndef BLUEBEAR_TYPES
#define BLUEBEAR_TYPES

#include <chrono>
#include <cstdint>

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;

typedef char				s8;
typedef short				s16;
typedef int					s32;

typedef std::chrono::duration< uint32_t, std::milli > millisecond_t;

namespace BlueBear {
	
	enum TerrainType { TERRAIN_GRASS, TERRAIN_DIRT, TERRAIN_SNOW, TERAIN_SAND, TERRAIN_MOON, TERRAIN_UNDERGROUND };
	
	enum PopCategory { POP_OBJECT, POP_CHARACTER };
	
	typedef struct {
		
		uint32_t magicID;
		uint8_t formatRevision;
		
		uint8_t lotX;
		uint8_t lotY;
		
		uint8_t numStories;
		uint8_t undergroundStories;
		
		uint8_t terrainType;
		
	} BBLTLotHeader;
	
}

#endif
