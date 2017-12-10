#ifndef SG_MODEL
#define SG_MODEL

#include "graphics/scenegraph/style.hpp"
#include "graphics/scenegraph/transform.hpp"
#include <string>
#include <vector>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {
        class Mesh;
      }

      class Model {
        std::string id;
        std::weak_ptr< Model > parent;
        std::shared_ptr< Mesh::Mesh > mesh;
        Style style;
        std::unique_ptr< Transform > transform;
        std::vector< std::shared_ptr< Model > > submodels;

      public:
        Model( std::weak_ptr< Model > parent, std::string id, std::shared_ptr< Mesh::Mesh > mesh, Style style );

        const std::string& getId() const;
        void setId( const std::string& id );

        std::shared_ptr< Model > getParent() const;
        void setParentTo( std::shared_ptr< Model > newParent );

        std::shared_ptr< Model > findChildById( const std::string& id ) const;
      };

    }
  }
}


#endif
