#ifndef LOT
#define LOT

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "bbtypes.hpp"
#include <jsoncpp/json/json.h>
#include <vector>
#include <map>
#include <string>
#include <memory>

namespace BlueBear {
	namespace Scripting {
		class LotEntity;

		class Lot {

			private:
				lua_State* L;
				const Tick& currentTickReference;

			public:
				std::map< std::string, std::unique_ptr< LotEntity > > objects;
				int floorX;
				int floorY;
				int stories;
				int undergroundStories;
				BlueBear::TerrainType terrainType;

				Lot( lua_State* L, const Tick& currentTickReference, int floorX, int floorY, int stories, int undergroundStories, BlueBear::TerrainType terrainType );
				int getLotObjectByCid( const std::string& cid );
				int createLotEntity( const std::string& classID );
				int createLotEntityFromJSON( const Json::Value& serialEntity );

				static int lua_getLotObjects( lua_State* L );
				static int lua_getLotObjectsByType( lua_State* L );
				static int lua_getLotObjectByCid( lua_State* L );
				static int lua_createLotEntity( lua_State* L );
		};
	}
}

#endif
