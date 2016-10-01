#include "graphics/instance/dwallinstance.hpp"
#include "graphics/instance/wallinstance.hpp"
#include "graphics/model.hpp"
#include "graphics/imagebuilder/imagesource.hpp"
#include "graphics/imagebuilder/directimagesource.hpp"
#include <memory>
#include <map>
#include <string>

namespace BlueBear {
  namespace Graphics {

    std::shared_ptr< Model > DWallInstance::Piece( nullptr );

    DWallInstance::DWallInstance( GLuint shaderProgram, TextureCache& hostTextureCache ) : WallInstance::WallInstance( *DWallInstance::Piece, shaderProgram, hostTextureCache ) {}

    void DWallInstance::setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings ) {

    }

  }
}
