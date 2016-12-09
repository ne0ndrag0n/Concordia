#ifndef LOT
#define LOT

#include "containers/collection3d.hpp"
#include "containers/conccollection3d.hpp"
#include "scripting/infrastructurefactory.hpp"
#include "threading/lockable.hpp"
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
		class SerializableInstance;
		class Tile;
		struct WallCell;

		class Lot {

			private:
				lua_State* L;
				const Tick& currentTickReference;
				InfrastructureFactory& infrastructureFactory;
				void buildFloorMap( Json::Value& floor );
				void buildWallMap( Json::Value& walls );
				Threading::Lockable< WallCell > getWallCell( Json::Value& object, std::vector< Threading::Lockable< Wallpaper > >& lookup );
				inline Threading::Lockable< Tile > getTile( int index, std::vector< Threading::Lockable< Tile > >& lookup );

			public:
				std::map< std::string, std::unique_ptr< SerializableInstance > > objects;
				std::unique_ptr< Containers::ConcCollection3D< Threading::Lockable< Tile > > > floorMap;
				std::unique_ptr< Containers::ConcCollection3D< Threading::Lockable< WallCell > > > wallMap;
				int floorX;
				int floorY;
				int stories;
				int undergroundStories;
				unsigned int currentRotation;
				BlueBear::TerrainType terrainType;

				Lot( lua_State* L, const Tick& currentTickReference, InfrastructureFactory& infrastructureFactory, Json::Value& rootObject );

		};
	}
}

#endif
