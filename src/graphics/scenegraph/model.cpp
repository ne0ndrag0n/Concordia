#include "graphics/scenegraph/model.hpp"
#include "graphics/transform.hpp"
#include <algorithm>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      Model::Model( std::weak_ptr< Model > parent, std::string id, std::shared_ptr< Mesh::Mesh > mesh, Style style ) :
        parent( parent ), id( id ), mesh( mesh ), style( style ) {}

      std::shared_ptr< Model > Model::create( std::weak_ptr< Model > parent, std::string id, std::shared_ptr< Mesh::Mesh > mesh, Style style ) {
        return std::shared_ptr< Model >(
          new Model( parent, id, mesh, style )
        );
      }

      const std::string& Model::getId() const {
        return id;
      }

      void Model::setId( const std::string& id ) {
        this->id = id;
      }

      std::shared_ptr< Model > Model::getParent() const {
        return parent.lock();
      }

      void Model::setParent( std::shared_ptr< Model > newParent ) {
        std::shared_ptr< Model > sharedFromThis = shared_from_this();

        std::shared_ptr< Model > oldParent = parent.lock();
        parent = newParent;

        if( oldParent ) {
          std::vector< std::shared_ptr< Model > >::iterator self = std::find_if(
            oldParent->submodels.begin(),
            oldParent->submodels.end(),
            [ & ]( std::shared_ptr< Model > submodel ) {
              return submodel == sharedFromThis;
            }
          );

          oldParent->submodels.erase( self );
        }

        if( newParent ) {
          newParent->submodels.emplace_back( sharedFromThis );
        }
      }

      Style& Model::getStyle() {
        return style;
      }

      void Model::setStyle( Style style ) {
        this->style = style;
      }

      Transform& Model::getTransform() {
        return transform;
      }

      void Model::setTransform( Transform transform ) {
        this->transform = transform;
      }

      std::shared_ptr< Model > Model::findChildById( const std::string& id ) const {
        for( std::shared_ptr< Model > submodel : submodels ) {
          if( submodel->getId() == id ) {
            return submodel;
          }

          if( std::shared_ptr< Model > result = submodel->findChildById( id ) ) {
            return result;
          }
        }

        return std::shared_ptr< Model >();
      }

    }
  }
}
