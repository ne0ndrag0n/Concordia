#ifndef LOT
#define LOT

#include "bbtypes.hpp"
#include "lotentity.hpp"
#include <vector>
#include <map>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

namespace BlueBear {

	class Lot {

		public:
			Lot();
			Lot( int floorX, int floorY, int stories, int undergroundStories, BlueBear::TerrainType terrainType );
			int floorX;
			int floorY;
			int stories;
			int undergroundStories;
			std::map< std::string, BlueBear::LotEntity > objects;
			BlueBear::TerrainType terrainType;
			static int lua_getLotObjects( lua_State* L );
			static int lua_getLotObjectsByType( lua_State* L );
			static int lua_getLotObjectByCid( lua_State* L );

	};

}

#endif
