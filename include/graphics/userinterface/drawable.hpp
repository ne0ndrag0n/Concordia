#ifndef NEW_GUI_DRAWABLE
#define NEW_GUI_DRAWABLE

#include "graphics/vector/renderer.hpp"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include <memory>

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {
          class GuiComponent;
        }
      }
    }
  }

  namespace Graphics {
    namespace UserInterface {
      class Element;

      class Drawable {
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        glm::ivec2 dimensions;
        std::shared_ptr< Vector::Renderer::Texture > texture;

        struct Corner {
          glm::vec3 position;
          glm::vec2 texture;
        };

        static std::vector< GLuint > MESH_INDICES;

        std::vector< Corner > generateMesh( unsigned int width, unsigned int height );

      public:
        Drawable( std::shared_ptr< Vector::Renderer::Texture > texture, unsigned int width, unsigned int height );
        ~Drawable();

        const glm::ivec2& getDimensions();
        std::shared_ptr< Vector::Renderer::Texture > getTexture();
        void draw( const glm::ivec2& position );
      };

    }
  }
}

#endif
