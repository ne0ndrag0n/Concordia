#include "models/wallsegment.hpp"
#include "log.hpp"

namespace BlueBear::Models {

  Sides::Sides( const Json::Value& sides, Utilities::WorldCache& worldCache ) {
    std::string frontId = sides[ "front" ].asString();
    std::string backId = sides[ "back" ].asString();

    auto frontOptional = worldCache.getWallpaper( frontId );
    auto backOptional = worldCache.getWallpaper( backId );

    if( !frontOptional ) {
      Log::getInstance().error( "Sides::Sides", "Invalid wallpaper: " + frontId );
      throw InvalidWallpaperException();
    }

    if( !backOptional ) {
      Log::getInstance().error( "Sides::Sides", "Invalid wallpaper: " + backId );
      throw InvalidWallpaperException();
    }

    front = { frontId, *frontOptional };
    back = { backId, *backOptional };
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
