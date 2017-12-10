#include "graphics/scenegraph/model.hpp"
#include "graphics/transform.hpp"
#include <algorithm>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      Model::Model( std::weak_ptr< Model > parent, std::string id, std::shared_ptr< Mesh::Mesh > mesh, Style style ) :
        parent( parent ), id( id ), mesh( mesh ), style( style ) {}

      const std::string& Model::getId() const {
        return id;
      }

      void Model::setId( const std::string& id ) {
        this->id = id;
      }

      std::shared_ptr< Model > Model::getParent() const {
        return parent.lock();
      }

      void Model::setParentTo( std::shared_ptr< Model > newParent ) {
        // FIXME: fix undefined behaviour here
        std::vector< std::shared_ptr< Model > >::iterator self;
        std::shared_ptr< Model > oldParent = parent.lock();

        if( oldParent ) {
          self = std::find_if(
            oldParent->submodels.begin(),
            oldParent->submodels.end(),
            [ & ]( std::shared_ptr< Model > submodel ) {
              return submodel.get() == this;
            }
          );
        }

        this->parent = newParent;
        if( newParent && oldParent ) {
          newParent->submodels.emplace_back( *self );
          oldParent->submodels.erase( self );
        }
      }

    }
  }
}
