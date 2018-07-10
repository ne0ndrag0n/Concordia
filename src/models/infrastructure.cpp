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

        const Json::Value& joints = level[ "walljoints" ];
        for( auto it = joints.begin(); it != joints.end(); ++it ) {
          const Json::Value& jointList = *it;
          std::vector< std::optional< WallJoint > > jointSet;

          for( auto it = jointList.begin(); it != jointList.end(); ++it ) {
            if( *it != Json::Value::null ) {
              jointSet.emplace_back( WallJoint{
                ( *it )[ "n" ].isBool() && ( *it )[ "n" ].asBool(),
                ( *it )[ "e" ].isBool() && ( *it )[ "e" ].asBool(),
                ( *it )[ "s" ].isBool() && ( *it )[ "s" ].asBool(),
                ( *it )[ "w" ].isBool() && ( *it )[ "w" ].asBool()
              } );
            } else {
              jointSet.emplace_back();
            }
          }

          current.wallJoints.emplace_back( std::move( jointSet ) );
        }

        levels.emplace_back( std::move( current ) );
      }

      Graphics::SceneGraph::ModelLoader::FloorModelLoader floorModelLoader( levels );
      worldRenderer.insertDirect( floorModelLoader.get() );
    }
  }
}
