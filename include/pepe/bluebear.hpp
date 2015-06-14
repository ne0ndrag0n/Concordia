#ifndef BLUEBEAR
#define BLUEBEAR

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
	
	class Object {
		
		private:
			bool ok;
			lua_State* L;
		public:
			Object( lua_State* L, const char* idKey );
			Object( lua_State* L, const char* idKey, char* popPackage, int popSize );
			int luaVMInstance;
			int lotTableRef;
			void execute( unsigned int worldTicks );
			char* save();
			void load( char* pickledObject );
			bool good();
		
	};
	
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

	};

	class Engine {
		
		private:
			lua_State* L;
			std::vector< BlueBear::Object > objects;
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

	namespace Utility {
		uint16_t swap_uint16( uint16_t val );

		int16_t swap_int16( int16_t val );

		uint32_t swap_uint32( uint32_t val );

		int32_t swap_int32( int32_t val );

		static void stackDump( lua_State* L );

		std::vector< std::string > getSubdirectoryList( const char* rootSubDirectory );

		void clearLuaStack( lua_State* L );

		void getTableValue( lua_State* L, const char* key );
		
		void setTableIntValue( lua_State* L, const char* key, int value );
		
		void setTableStringValue( lua_State* L, const char* key, const char* value );
		
		void setTableFunctionValue( lua_State* L, const char* key, lua_CFunction value );
		
		inline uint8_t getuint8_t( std::ifstream* istream );
		
		inline uint16_t getuint16_t( std::ifstream* istream );
		
		inline uint32_t getuint32_t( std::ifstream* istream );
	};
	
}

#endif
