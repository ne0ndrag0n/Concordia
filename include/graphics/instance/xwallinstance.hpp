#ifndef XWALLINSTANCE
#define XWALLINSTANCE

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

    class XWallInstance : public WallInstance {
    public:
      static std::shared_ptr< Model > Piece;
      static std::shared_ptr< Model > EdgePiece;

      XWallInstance( GLuint shaderProgram, TextureCache& hostTextureCache, ImageCache& hostImageCache );
    protected:
      void setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings );
    };

  }
}

#endif
