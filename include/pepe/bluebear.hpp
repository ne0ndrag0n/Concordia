#ifndef BLUEBEAR
#define BLUEBEAR

#include "object.hpp"
#include <string>
#include <vector>
#include <cstdio>
#include <cstdint>
#include <fstream>
#include <cstdint>
#include <chrono>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#define BLUEBEAR_OBJECTS_DIRECTORY "assets/objects/"
#define BLUEBEAR_LOT_MAGIC_ID 		0x42424C54
#define WORLD_TICKS_MAX				604800000

/**
 * BlueBear objects
 *
 *
 */
namespace BlueBear {

	enum TerrainType { TERRAIN_GRASS, TERRAIN_DIRT, TERRAIN_SNOW, TERAIN_SAND, TERRAIN_MOON, TERRAIN_UNDERGROUND };
	
	class Lot {
		
		public:
			Lot();
			Lot( int floorX, int floorY, int stories, int undergroundStories, TerrainType terrainType );
			int floorX;
			int floorY;
			int stories;
			int undergroundStories;
			std::vector< BlueBear::Object > objects;
			TerrainType terrainType;
			static int lua_getLotObjects( lua_State* L );
			static int lua_getLotObjectsByType( lua_State* L );

	};

	class Engine {
		
		private:
			lua_State* L;
			unsigned int worldTicks;
			Lot* currentLot;
			bool verifyODT( std::vector< std::string > odt );
			int createLotTable( Lot* lot );

		public:
			Engine();
			~Engine();
			bool setupRootEnvironment();
			void objectLoop();
			bool loadLot( const char* lotPath );
					
	};
	
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
