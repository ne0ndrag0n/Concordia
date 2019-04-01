#ifndef SG_MESH
#define SG_MESH

#include "graphics/scenegraph/uniform.hpp"
#include "geometry/triangle.hpp"
#include "log.hpp"
#include <GL/glew.h>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <map>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      namespace Mesh {

        class Mesh {
        protected:
          std::vector< Geometry::Triangle > genericTriangles;

          template< typename VertexType >
          void storeGenericTriangles( const std::vector< VertexType >& vertexList, const std::vector< GLuint >& indexList ) {
            if( indexList.size() ) {
              bool check = indexList.size() % 3 == 0;
              if( check ) {
                for( int i = 0; i < indexList.size(); i += 3 ) {
                  genericTriangles.emplace_back( Geometry::Triangle{
                    vertexList[ indexList[ i ] ].position,
                    vertexList[ indexList[ i + 1 ] ].position,
                    vertexList[ indexList[ i + 2 ] ].position
                  } );
                }
              } else {
                Log::getInstance().warn( "Mesh::storeGenericTriangles", "Mesh indices are not a multiple of 3; skipping generic triangle generation." );
              }
            } else {
              bool check = vertexList.size() % 3 == 0;
              if( check ) {
                for( int i = 0; i < vertexList.size(); i += 3 ) {
                  genericTriangles.emplace_back( Geometry::Triangle{
                    vertexList[ i ].position,
                    vertexList[ i + 1 ].position,
                    vertexList[ i + 2 ].position
                  } );
                }
              } else {
                Log::getInstance().warn( "Mesh::storeGenericTriangles", "Mesh vertices are not a multiple of 3; skipping generic triangle generation." );
              }
            }
          };

        public:
          std::map< std::string, std::shared_ptr< Uniform > > meshUniforms;
          std::function< std::pair< std::string, std::string >() > getDefaultShader;

          virtual ~Mesh() = default;

          virtual void sendDeferred() = 0;
          virtual void drawElements() = 0;

          const std::vector< Geometry::Triangle >& getTriangles() const;
        };

      }
    }
  }
}

#endif
