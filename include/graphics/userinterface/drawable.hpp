#ifndef NEW_GUI_DRAWABLE
#define NEW_GUI_DRAWABLE

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      class Drawable {
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;

        struct Corner {
          glm::vec3 position;
          glm::vec2 texture;
        };

        static std::vector< GLuint > MESH_INDICES;

        std::vector< Corner > generateMesh( unsigned int x, unsigned int y, unsigned int width, unsigned int height );

      public:
        Drawable( unsigned int x, unsigned int y, unsigned int width, unsigned int height );
        ~Drawable();

        void draw();
      };

    }
  }
}

#endif
