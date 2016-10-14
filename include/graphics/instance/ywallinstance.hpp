#ifndef YWALLINSTANCE
#define YWALLINSTANCE

#include "graphics/instance/wallinstance.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagecache.hpp"
#include "graphics/texturecache.hpp"
#include <memory>
#include <map>
#include <string>

namespace BlueBear {
  namespace Graphics {
    class Model;

    class YWallInstance : public WallInstance {
    public:
      static std::shared_ptr< Model > Piece;
      static std::shared_ptr< Model > EdgePiece;

      YWallInstance( GLuint shaderProgram, TextureCache& hostTextureCache, ImageCache& hostImageCache, bool edgePiece = false );
    protected:
      void setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings );
    };

  }
}

#endif
