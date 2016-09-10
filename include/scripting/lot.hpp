#ifndef LOT
#define LOT

#include "containers/collection3d.hpp"
#include "scripting/infrastructurefactory.hpp"
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
		class Tile;
		struct WallCell;

		class Lot {

			private:
				lua_State* L;
				const Tick& currentTickReference;
				InfrastructureFactory& infrastructureFactory;
				void buildFloorMap( Json::Value& floor );
				void buildWallMap( Json::Value& walls );
				std::shared_ptr< WallCell > getWallCell( Json::Value& object, std::vector< std::shared_ptr< Wallpaper > >& lookup );

			public:
				std::map< std::string, std::unique_ptr< LotEntity > > objects;
				std::unique_ptr< Containers::Collection3D< std::shared_ptr< Tile > > > floorMap;
				std::unique_ptr< Containers::Collection3D< std::shared_ptr< WallCell > > > wallMap;
				int floorX;
				int floorY;
				int stories;
				int undergroundStories;
				BlueBear::TerrainType terrainType;

				Lot( lua_State* L, const Tick& currentTickReference, InfrastructureFactory& infrastructureFactory, Json::Value& rootObject );
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
