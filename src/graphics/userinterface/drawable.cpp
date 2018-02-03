#include "graphics/userinterface/drawable.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      std::vector< GLuint > Drawable::MESH_INDICES = {
        0, 1, 2,
        1, 2, 3
      };

      Drawable::Drawable( unsigned int x, unsigned int y, unsigned int width, unsigned int height ) {
        glGenVertexArrays( 1, &VAO );
        glGenBuffers( 1, &VBO );
        glGenBuffers( 1, &EBO );

        glBindVertexArray( VAO );
          glBindBuffer( GL_ARRAY_BUFFER, VBO );
            std::vector< Drawable::Corner > quad = generateMesh( x, y, width, height );
            glBufferData( GL_ARRAY_BUFFER, quad.size() * sizeof( Drawable::Corner ), &quad[ 0 ], GL_STATIC_DRAW );
              glEnableVertexAttribArray( 0 );
              glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Drawable::Corner ), ( GLvoid* ) 0 );

              glEnableVertexAttribArray( 1 );
              glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( Drawable::Corner ), ( GLvoid* ) offsetof( Drawable::Corner, texture ) );

            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, MESH_INDICES.size() * sizeof( GLuint ), &MESH_INDICES[ 0 ], GL_STATIC_DRAW );

          glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindVertexArray( 0 );
      }

      Drawable::~Drawable() {
        glDeleteVertexArrays( 1, &VAO );
        glDeleteBuffers( 1, &VBO );
        glDeleteBuffers( 1, &EBO );
      }

      std::vector< Drawable::Corner > Drawable::generateMesh( unsigned int x, unsigned int y, unsigned int width, unsigned int height ) {
        std::vector< Drawable::Corner > vertices = {
          {
            { x, y, 1.0f },
            { 0.0f, 1.0f }
          },
          {
            { x + width, y, 1.0f },
            { 1.0f, 1.0f }
          },
          {
            { x, y + height, 1.0f },
            { 0.0f, 0.0f }
          },
          {
            { x + width, y + height, 1.0f },
            { 1.0f, 0.0f }
          }
        };

        return vertices;
      }

    }
  }
}
