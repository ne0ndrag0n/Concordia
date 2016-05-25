#ifndef LOT
#define LOT

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "bbtypes.hpp"
#include "lotentity.hpp"
#include "json/json.h"
#include "eventmanager.hpp"
#include <vector>
#include <map>
#include <string>
#include <memory>

namespace BlueBear {

	class Lot {

		private:
			lua_State* L;
			EventManager eventManager;

		public:
			std::map< std::string, std::unique_ptr< BlueBear::LotEntity > > objects;
			int floorX;
			int floorY;
			int stories;
			int undergroundStories;
			BlueBear::TerrainType terrainType;

			Lot( lua_State* L, int floorX, int floorY, int stories, int undergroundStories, BlueBear::TerrainType terrainType );
			int getLotObjectByCid( const std::string& cid );
			int createLotEntity( const std::string& classID );
			int createLotEntityFromJSON( const Json::Value& serialEntity );
			void buildLuaInterface();

			static int lua_registerEvent( lua_State* L );
			static int lua_unregisterEvent( lua_State* L );
			static int lua_broadcastEvent( lua_State* L );
			static int lua_getLotObjects( lua_State* L );
			static int lua_getLotObjectsByType( lua_State* L );
			static int lua_getLotObjectByCid( lua_State* L );
			static int lua_createLotEntity( lua_State* L );
	};
}

#endif
