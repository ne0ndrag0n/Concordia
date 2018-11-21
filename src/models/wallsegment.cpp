#include "models/wallsegment.hpp"
#include "log.hpp"

namespace BlueBear::Models {

  Sides::Sides( const Json::Value& sides, Utilities::WorldCache& worldCache ) {
    auto frontOptional = worldCache.getWallpaper( sides[ "front" ].asString() );
    auto backOptional = worldCache.getWallpaper( sides[ "back" ].asString() );

    if( !frontOptional ) {
      Log::getInstance().error( "Sides::Sides", "Invalid wallpaper: " + sides[ "front" ].asString() );
      throw InvalidWallpaperException();
    }

    if( !backOptional ) {
      Log::getInstance().error( "Sides::Sides", "Invalid wallpaper: " + sides[ "back" ].asString() );
      throw InvalidWallpaperException();
    }

    front = *frontOptional;
    back = *backOptional;
  }

  WallSegment::WallSegment( const Json::Value& segment, Utilities::WorldCache& worldCache ) {
    if( !segment.isObject() ) {
      throw InvalidFormatException();
    }

    start = glm::ivec2{ segment[ "start" ][ 0 ].asInt(), segment[ "start" ][ 1 ].asInt() };
    end = glm::ivec2{ segment[ "end" ][ 0 ].asInt(), segment[ "end" ][ 1 ].asInt() };

    for( const Json::Value& object : segment[ "faces" ] ) {
      faces.emplace_back( object, worldCache );
    }
  }

}
