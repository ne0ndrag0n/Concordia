#ifndef SG_MESH
#define SG_MESH

#include "graphics/scenegraph/uniform.hpp"
#include "geometry/triangle.hpp"
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

        public:
          std::map< std::string, std::shared_ptr< Uniform > > meshUniforms;
          std::function< std::pair< std::string, std::string >() > getDefaultShader;

          virtual ~Mesh() = default;

          virtual void sendDeferred() = 0;
          virtual void drawElements() = 0;

          virtual std::vector< Geometry::Triangle > getTriangles() = 0;
        };

      }
    }
  }
}

#endif
