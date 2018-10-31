#include "models/infrastructure.hpp"
#include "tools/utility.hpp"
#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "graphics/vector/renderer.hpp"
#include <vector>
#include "log.hpp"

namespace BlueBear::Models {

  std::shared_ptr< sf::Image > getDefaults( Graphics::Vector::Renderer& renderer ) {
    std::shared_ptr< sf::Image > result = std::make_shared< sf::Image >();
    std::shared_ptr< unsigned char[] > bitmap = renderer.generateBitmap( { 1, 10 }, [ & ]( Graphics::Vector::Renderer& renderer ) {
      renderer.drawRect( { 0, 0, 1, 10 }, { 128, 64, 0, 255 } );
    } );

    result->create( 1, 10, bitmap.get() );

    return result;
  }

  Json::Value Infrastructure::save() {

  }

  void Infrastructure::load( const Json::Value& data, Device::Display::Adapter::Component::WorldRenderer& worldRenderer, Graphics::Vector::Renderer& renderer, Utilities::WorldCache& worldCache ) {
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

        levels.emplace_back( std::move( current ) );
      }

      Graphics::SceneGraph::ModelLoader::FloorModelLoader floorModelLoader( levels );
      worldRenderer.insertDirect( floorModelLoader.get() );
    }
  }

  void Infrastructure::load( const Json::Value& data ) {
    Log::getInstance().error( "Infrastructure::load", "Called the wrong load method. This is probably a bug." );
  }
}
