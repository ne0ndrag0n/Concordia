#ifndef NEW_GUI_DRAWABLE
#define NEW_GUI_DRAWABLE

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

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

        struct Corner {
          glm::vec3 position;
          glm::vec2 texture;
        };

        static std::vector< GLuint > MESH_INDICES;

        void generateTexture();
        std::vector< Corner > generateMesh( unsigned int x, unsigned int y, unsigned int width, unsigned int height );

      public:
        Drawable( Element& element );
        ~Drawable();

        void draw();
      };

    }
  }
}

#endif
