#ifndef YWALLINSTANCE
#define YWALLINSTANCE

#include "graphics/instance/wallinstance.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/texturecache.hpp"
#include <memory>
#include <map>
#include <string>

namespace BlueBear {
  namespace Graphics {
    class Model;

    class YWallInstance : public WallInstance {
    public:
      YWallInstance( const Model& model, GLuint shaderProgram, TextureCache& hostTextureCache );
    protected:
      void setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings );
    };

  }
}

#endif
