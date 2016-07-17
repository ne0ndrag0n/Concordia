#include "graphics/drawable.hpp"
#include "graphics/mesh.hpp"
#include "graphics/material.hpp"
#include <memory>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {

    Drawable::Drawable( std::shared_ptr< Mesh > mesh, std::shared_ptr< Material > material ) :
      mesh( mesh ), material( material ) {}

    void Drawable::render( GLuint shaderID ) {
      material->sendToShader( shaderID );
      mesh->drawElements();
    }

  }
}
