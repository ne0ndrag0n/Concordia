#include "graphics/userinterface/drawable.hpp"
#include "graphics/userinterface/element.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "configmanager.hpp"
#include "tools/opengl.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      std::vector< GLuint > Drawable::MESH_INDICES = {
        0, 1, 2,
        1, 2, 3
      };

      Drawable::Drawable( std::shared_ptr< Vector::Renderer::Texture > texture, unsigned int width, unsigned int height ) :
        dimensions( width, height ), texture( texture ) {
        glGenVertexArrays( 1, &VAO );
        glGenBuffers( 1, &VBO );
        glGenBuffers( 1, &EBO );

        glBindVertexArray( VAO );
          glBindBuffer( GL_ARRAY_BUFFER, VBO );
            std::vector< Drawable::Corner > quad = generateMesh( width, height );
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

      std::vector< Drawable::Corner > Drawable::generateMesh( unsigned int width, unsigned int height ) {
        std::vector< Drawable::Corner > vertices = {
          {
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f }
          },
          {
            { width, 0.0f, 0.0f },
            { 1.0f, 1.0f }
          },
          {
            { 0.0f, height, 0.0f },
            { 0.0f, 0.0f }
          },
          {
            { width, height, 0.0f },
            { 1.0f, 0.0f }
          }
        };

        return vertices;
      }

      const glm::ivec2& Drawable::getDimensions() {
        return dimensions;
      }

      std::shared_ptr< Vector::Renderer::Texture > Drawable::getTexture() {
        return texture;
      }

      void Drawable::draw( const Shader& guiShader, const glm::ivec2& position ) {
        static float viewportX = ConfigManager::getInstance().getIntValue( "viewport_x" );
        static float viewportY = ConfigManager::getInstance().getIntValue( "viewport_y" );
        static glm::mat4 orthoProjection = glm::ortho( 0.0f, viewportX, viewportY, 0.0f, -1.0f, 1.0f );

        if( !uniforms ) {
          uniforms = Uniforms {
            guiShader.getUniform( "orthoProjection" ),
            guiShader.getUniform( "translation" ),
            guiShader.getUniform( "surface" )
          };
        }

        guiShader.sendData( uniforms->orthoProjection, orthoProjection );

        glm::mat4 translation = glm::translate( glm::mat4( 1.0f ), glm::vec3{ position.x, position.y, 0.0f } );
        guiShader.sendData( uniforms->translation, translation );

        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, texture->getTextureId() );
        guiShader.sendData( uniforms->surface, 0 );

        glBindVertexArray( VAO );
          glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0 );
        glBindVertexArray( 0 );
      }

    }
  }
}
