#include "models/infrastructure.hpp"
#include "tools/utility.hpp"
#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include <vector>
#include "log.hpp"

namespace BlueBear::Models {

  Infrastructure::Infrastructure( Device::Display::Adapter::Component::WorldRenderer& worldRenderer, Utilities::WorldCache& worldCache ) : worldRenderer( worldRenderer ), worldCache( worldCache ) {}

  Json::Value Infrastructure::save() {

  }

  void Infrastructure::load( const Json::Value& data ) {
    if( data != Json::Value::null ) {
      const Json::Value& tiles = data[ "tiles" ];
      for( auto it = tiles.begin(); it != tiles.end(); ++it ) {
        const Json::Value& level = *it;
        FloorLevel current;
        current.dimensions = { level[ "dimensions" ][ 0 ].asUInt(), level[ "dimensions" ][ 1 ].asUInt() };

        const Json::Value& innerTiles = level[ "tiles" ];
        for( auto it = innerTiles.begin(); it != innerTiles.end(); ++it ) {
          const Json::Value& set = *it;
          std::vector< std::optional< FloorTile > > tileSet;
          for( auto it = set.begin(); it != set.end(); ++it ) {
            std::string desiredTile = it->asString();
            if( desiredTile != "" ) {
              auto originalTile = worldCache.getFloorTile( desiredTile );
              if( !originalTile ) {
                Log::getInstance().error( "Infrastructure::load", "Tile found in lot but not registered: " + desiredTile );
              }
              tileSet.emplace_back( std::move( originalTile ) );
            } else {
              tileSet.emplace_back();
            }
          }

          current.tiles.emplace_back( std::move( tileSet ) );
        }

        const Json::Value& vertices = level[ "vertices" ];
        for( auto it = vertices.begin(); it != vertices.end(); ++it ) {
          const Json::Value& jsonVertices = *it;
          std::vector< float > vertexSet;
          for( auto it = jsonVertices.begin(); it != jsonVertices.end(); ++it ) {
            vertexSet.push_back( it->asFloat() );
          }
          current.vertices.emplace_back( std::move( vertexSet ) );
        }

        const Json::Value& corners = level[ "corners" ];
        for( auto it = corners.begin(); it != corners.end(); ++it ) {
          const Json::Value& corner = *it;
          current.corners.emplace_back(
            glm::uvec2{ corner[ "start" ][ 0 ].asUInt(), corner[ "start" ][ 1 ].asUInt() },
            glm::uvec2{ corner[ "end"   ][ 0 ].asUInt(), corner[ "end"   ][ 1 ].asUInt() }
          );
        }

        const Json::Value& wallpaper = level[ "wallpaper" ];
        for( auto it = wallpaper.begin(); it != wallpaper.end(); ++it ) {
          const Json::Value& wallpaperArray = *it;
          std::vector< WallpaperRegion > regionSet;

          for( auto it = wallpaperArray.begin(); it != wallpaperArray.end(); ++it ) {
            const Json::Value& wallpaperRegion = *it;

            if( wallpaperRegion == Json::Value::null ) {
              regionSet.emplace_back();
            } else {
              WallpaperRegion region;
              if( wallpaperRegion[ "north" ] != Json::Value::null ) { region.north = worldCache.getWallpaper( wallpaperRegion[ "north" ].asString() ); }
              if( wallpaperRegion[ "east" ] != Json::Value::null ) { region.east = worldCache.getWallpaper( wallpaperRegion[ "east" ].asString() ); }
              if( wallpaperRegion[ "west" ] != Json::Value::null ) { region.west = worldCache.getWallpaper( wallpaperRegion[ "west" ].asString() ); }
              if( wallpaperRegion[ "south" ] != Json::Value::null ) { region.south = worldCache.getWallpaper( wallpaperRegion[ "south" ].asString() ); }
              regionSet.emplace_back( std::move( region ) );
            }
          }
        }

        levels.emplace_back( std::move( current ) );
      }

      Graphics::SceneGraph::ModelLoader::FloorModelLoader floorModelLoader( levels );
      worldRenderer.insertDirect( floorModelLoader.get() );
    }
  }
}
