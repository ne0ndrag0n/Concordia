#ifndef WALL_SEGMENT
#define WALL_SEGMENT

#include "models/utilities/worldcache.hpp"
#include "exceptions/genexc.hpp"
#include "models/wallpaper.hpp"
#include <jsoncpp/json/json.h>
#include <glm/glm.hpp>
#include <vector>
#include <utility>

namespace BlueBear::Models {

  struct Sides {
    std::pair< std::string, Wallpaper > front;
    std::pair< std::string, Wallpaper > back;

    EXCEPTION_TYPE( InvalidWallpaperException, "Wallpaper not found" );

    Sides( const Json::Value& sides, Utilities::WorldCache& worldCache );
  };

  struct WallSegment {
    glm::ivec2 start;
    glm::ivec2 end;
    std::vector< Sides > faces;

    EXCEPTION_TYPE( InvalidFormatException, "Invalid format" );

    WallSegment( const Json::Value& segment, Utilities::WorldCache& worldCache );
  };

}

#endif
