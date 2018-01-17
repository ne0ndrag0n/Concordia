#include "models/houseenvironment.hpp"
#include "tools/jsontools.hpp"
#include "scripting/infrastructurefactory.hpp"
#include <algorithm>

namespace BlueBear {
  namespace Models {

    HouseEnvironment::HouseEnvironment( Scripting::InfrastructureFactory& infrastructureFactory, const Json::Value& lot ) :
      infrastructureFactory( infrastructureFactory ), dimensions( { lot[ "floorx" ].asDouble(), lot[ "floory" ].asDouble() } ) {
      Json::Value world = Tools::JsonTools::getUncompressedRLEArray( lot[ "world" ] );

      for( const Json::Value& elevationSegment : world ) {
        double elevation = elevationSegment[ "elevation" ].asDouble();
        prepElevationPairs( elevation );
        prepVertices( elevationSegment, elevation );
        prepTiles( elevationSegment, lot[ "dict" ], elevation );
      }
    }

    void HouseEnvironment::prepVertices( const Json::Value& elevationSegment, double elevation ) {
      Json::Value elevations = Tools::JsonTools::getUncompressedRLEArray( elevationSegment[ "elevations" ] );

      glm::uvec2 segmentDimensions( elevationSegment[ "dimensions" ][ 0 ].asUInt() + 1, elevationSegment[ "dimensions" ][ 1 ].asUInt() + 1 );
      glm::uvec2 origin( elevationSegment[ "originVertex" ][ 0 ].asUInt(), elevationSegment[ "originVertex" ][ 1 ].asUInt() );
      glm::uvec2 boundary = origin + segmentDimensions;

      unsigned int i = 0;
      for( unsigned int y = origin.y; y != boundary.y; y++ ) {
        for( unsigned int x = origin.x; x != boundary.x; x++ ) {
          vertices[ elevation ][ ( y * ( dimensions.y + 1 ) ) + x ] = elevations[ i++ ].asDouble();
        }
      }
    }

    void HouseEnvironment::prepTiles( const Json::Value& elevationSegment, const Json::Value& dict, double elevation ) {
      Json::Value tilesJson = Tools::JsonTools::getUncompressedRLEArray( elevationSegment[ "tiles" ] );

      glm::uvec2 segmentDimensions( elevationSegment[ "dimensions" ][ 0 ].asUInt(), elevationSegment[ "dimensions" ][ 1 ].asUInt() );
      glm::uvec2 origin( elevationSegment[ "originVertex" ][ 0 ].asUInt(), elevationSegment[ "originVertex" ][ 1 ].asUInt() );
      glm::uvec2 boundary = origin + segmentDimensions;

      unsigned int i = 0;
      for( unsigned int y = origin.y; y != boundary.y; y++ ) {
        for( unsigned int x = origin.x; x != boundary.x; x++ ) {
          int dictIndex = tilesJson[ i++ ].asInt();
          if( dictIndex != -1 ) {
            tiles[ elevation ][ ( y * dimensions.y ) + x ] = infrastructureFactory.getFloorTile( dict[ dictIndex ].asString() );
          }
        }
      }
    }

    void HouseEnvironment::prepElevationPairs( double elevation ) {
      if( vertices.find( elevation ) == vertices.end() ) {
        vertices[ elevation ].resize( ( dimensions.x + 1 ) * ( dimensions.y + 1 ) );
        tiles[ elevation ].resize( dimensions.x * dimensions.y );
      }
    }

    std::shared_ptr< Graphics::SceneGraph::Model > HouseEnvironment::generateEnvironment() {
      return nullptr;
    }

  }
}
