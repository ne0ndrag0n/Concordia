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

      class Model : public std::enable_shared_from_this< Model > {
        std::string id;
        std::weak_ptr< Model > parent;
        std::shared_ptr< Mesh::Mesh > mesh;
        Style style;
        Transform transform;
        std::vector< std::shared_ptr< Model > > submodels;

        Model() = delete;
        Model(
          std::string id,
          std::shared_ptr< Mesh::Mesh > mesh,
          Style style
        );
        Model( const Model& other );

      public:
        static std::shared_ptr< Model > create(
          std::string id,
          std::shared_ptr< Mesh::Mesh > mesh,
          Style style
        );

        const std::string& getId() const;
        void setId( const std::string& id );

        std::shared_ptr< Model > getParent() const;
        void setParent( std::shared_ptr< Model > newParent );

        Style& getStyle();
        void setStyle( Style style );

        Transform& getTransform();
        void setTransform( Transform transform );

        std::shared_ptr< Model > findChildById( const std::string& id ) const;

        void draw();
      };

    }
  }
}


#endif
