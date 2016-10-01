#ifndef DWALLINSTANCE
#define DWALLINSTANCE

#include "graphics/instance/wallinstance.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/texturecache.hpp"
#include <memory>
#include <map>
#include <string>

namespace BlueBear {
  namespace Graphics {
    class Model;

    class DWallInstance : public WallInstance {
    public:
      static std::shared_ptr< Model > Piece;

      DWallInstance( GLuint shaderProgram, TextureCache& hostTextureCache );
    protected:
      void setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings );
    };

  }
}

#endif
