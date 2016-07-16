#ifndef DRAWABLE
#define DRAWABLE

#include <memory>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    class Mesh;
    class Material;

    class Drawable {

      public:
        std::shared_ptr< Mesh > mesh;
        std::shared_ptr< Material > material;

        Drawable( std::shared_ptr< Mesh > mesh, std::shared_ptr< Material > material );

        void render( GLuint shaderID );
    };
  }
}

#endif
