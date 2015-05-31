#ifndef BLUEBEAR
#define BLUEBEAR

#include <string>
#include <vector>
#include <cstdio>
#include <cstdint>
#include <fstream>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#define BLUEBEAR_OBJECTS_DIRECTORY "assets/objects/"
#define BLUEBEAR_LOT_MAGIC_ID 		0x42424C54

/**
 * BlueBear objects
 *
 *
 */
namespace BlueBear {

	enum TerrainType { TERRAIN_GRASS, TERRAIN_DIRT, TERRAIN_SNOW, TERAIN_SAND, TERRAIN_MOON, TERRAIN_UNDERGROUND };
	
	class Object {
		
		private:
			int luaVMInstance;
			long nextExecution;
		
		public:
			Object();
			~Object();
			void execute( long worldTicks );
		
	};
	
	class Lot {
		
		public:
			Lot( int floorX, int floorY, int stories, int undergroundStories, TerrainType terrainType );
			
		private:
			int floorX;
			int floorY;
			int stories;
			int undergroundStories;
			std::vector< Object > objects;
			TerrainType terrainType;

	};

	class Engine {
		
		private:
			lua_State* L;
			long startTicks;
			Lot* currentLot;

		public:
			Engine();
			~Engine();
			bool setupRootEnvironment();
			void objectLoop();
			bool loadLot( const char* lotPath );
					
	};

	namespace Utility {
		
		uint16_t swap_uint16( uint16_t val );
		int16_t swap_int16( int16_t val );
		uint32_t swap_uint32( uint32_t val );
		int32_t swap_int32( int32_t val );
		static void stackDump( lua_State* L );
		std::vector< std::string > getSubdirectoryList( const char* rootSubDirectory );
	};
	
}

#endif
