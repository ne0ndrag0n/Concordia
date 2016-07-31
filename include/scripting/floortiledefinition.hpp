#ifndef FLOORTILE
#define FLOORTILE

#include <string>
#include <vector>
#include <exception>
#include <jsoncpp/json/json.h>

namespace BlueBear {
  namespace Scripting {

    class FloorTileDefinition {
      static constexpr const char* BASE_JSON_PATH = "assets/flooring/classes.json";
      static bool baseLoaded;
      static Json::Value baseClasses;

      private:
        void loadBase();

      public:
        // This will change as we lay out sf::Sound
        std::string soundPath;
        unsigned char saturation;
        std::vector< std::string > sequenceImagePaths;

        FloorTileDefinition( const std::string& jsonPath );
        struct CannotLoadFileException : public std::exception { const char* what () const throw () { return "Could not load a required file!"; } };
        struct JsonClassNotFoundException : public std::exception { const char* what () const throw () { return "Invalid JSON extend specified!"; } };
    };

  }
}


#endif
