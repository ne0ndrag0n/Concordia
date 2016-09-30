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

    RWallInstance::RWallInstance( const Model& model, GLuint shaderProgram, TextureCache& hostTextureCache ) : WallInstance::WallInstance( model, shaderProgram, hostTextureCache ) {}

    void RWallInstance::setRotationAttributes( unsigned int rotation, std::map< std::string, std::unique_ptr< ImageSource > >& settings ) {

    }

  }
}
