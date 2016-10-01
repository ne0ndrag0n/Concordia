#include "graphics/instance/rwallinstance.hpp"
#include "graphics/instance/wallinstance.hpp"
#include "graphics/model.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/directimagesource.hpp"
#include <memory>
#include <map>
#include <string>

namespace BlueBear {
  namespace Graphics {

    std::shared_ptr< Model > RWallInstance::Piece( nullptr );

    RWallInstance::RWallInstance( GLuint shaderProgram, TextureCache& hostTextureCache ) : WallInstance::WallInstance( *RWallInstance::Piece, shaderProgram, hostTextureCache ) {}

    void RWallInstance::setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings ) {

    }

  }
}
