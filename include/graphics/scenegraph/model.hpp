#ifndef SG_MODEL
#define SG_MODEL

#include <string>
#include <vector>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      class Mesh;
      class Style;
      class Transform;

      class Model {
        std::string id;
        std::weak_ptr< Model > parent;
        std::shared_ptr< Mesh > mesh;
        std::unique_ptr< Style > style;
        std::unique_ptr< Transform > transform;
        std::vector< std::shared_ptr< Model > > submodels;

      public:
        Model( std::weak_ptr< Model > parent, std::string id, std::shared_ptr< Mesh > mesh, std::unique_ptr< Style > style );
        Model::~Model() = default;

        const std::string& getId() const;
        void setId( const std::string& id );

        std::shared_ptr< Model > getParent() const;
        void setParent( std::shared_ptr< Model > parent );

        std::shared_ptr< Model > findChildById( const std::string& id ) const;
      };

    }
  }
}


#endif
