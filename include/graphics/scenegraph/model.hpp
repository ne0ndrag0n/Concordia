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
      namespace Animation {
        class Animator;
      }

      class Model : public std::enable_shared_from_this< Model > {
        std::string id;
        std::weak_ptr< Model > parent;
        std::shared_ptr< Mesh::Mesh > mesh;
        Style style;
        Transform transform;
        std::shared_ptr< Animation::Animator > animator;
        std::vector< std::shared_ptr< Model > > submodels;

        Model() = default;
        Model(
          std::string id,
          std::shared_ptr< Mesh::Mesh > mesh,
          Style style
        );
        Model( const Model& other );

        void computeAnimation();

      public:
        static std::shared_ptr< Model > create(
          std::string id,
          std::shared_ptr< Mesh::Mesh > mesh,
          Style style
        );

        std::shared_ptr< Model > copy();

        const std::string& getId() const;
        void setId( const std::string& id );

        std::shared_ptr< Model > getParent() const;
        void addChild( std::shared_ptr< Model > child );
        void detach();

        Style& getStyle();
        void setStyle( Style style );

        Transform getComputedTransform() const;
        Transform getLocalTransform() const;
        void setLocalTransform( Transform transform );

        std::shared_ptr< Animation::Animator > findNearestAnimator() const;
        void setAnimator( std::shared_ptr< Animation::Animator > animator );

        std::shared_ptr< Model > findChildById( const std::string& id ) const;

        void draw();
      };

    }
  }
}


#endif
