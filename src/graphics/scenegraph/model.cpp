#include "graphics/scenegraph/model.hpp"
#include "graphics/scenegraph/animation/animator.hpp"
#include "graphics/scenegraph/mesh/mesh.hpp"
#include "graphics/scenegraph/material.hpp"
#include "graphics/transform.hpp"
#include "graphics/shader.hpp"
#include "log.hpp"
#include <glm/glm.hpp>
#include <algorithm>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      Model::Model( std::string id, std::shared_ptr< Mesh::Mesh > mesh, Style style ) :
        id( id ), mesh( mesh ), style( style ) {}

      std::shared_ptr< Model > Model::create( std::string id, std::shared_ptr< Mesh::Mesh > mesh, Style style ) {
        return std::shared_ptr< Model >( new Model( id, mesh, style ) );
      }

      std::shared_ptr< Model > Model::copy() {
        std::shared_ptr< Model > copy( new Model() );

        copy->id = id;
        if( parent.lock() ) {
          Log::getInstance().warn( "Model::copy", "Disconnecting new Model copy from old parent" );
        }
        copy->mesh = mesh;
        copy->style = style;
        copy->transform = transform;
        copy->animator = std::make_shared< Animation::Animator >( *animator );

        for( std::shared_ptr< Model > child : submodels ) {
          copy->addChild( child->copy() );
        }

        return copy;
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

      void Model::detach() {
        if( std::shared_ptr< Model > realParent = getParent() ) {
          realParent->submodels.erase(
            std::remove( realParent->submodels.begin(), realParent->submodels.end(), shared_from_this() ),
            realParent->submodels.end()
          );

          parent = std::weak_ptr< Model >();
        }
      }

      void Model::addChild( std::shared_ptr< Model > child ) {
        child->detach();
        submodels.emplace_back( child );
        child->parent = shared_from_this();
      }

      Style& Model::getStyle() {
        return style;
      }

      void Model::setStyle( Style style ) {
        this->style = style;
      }

      Transform Model::getComputedTransform() const {
        if( std::shared_ptr< Model > realParent = parent.lock() ) {
          // A -> B -> C -> D
          // ( D * ( C * ( B * A ) ) )
          return transform * realParent->getComputedTransform();
        }

        return transform;
      }

      Transform Model::getLocalTransform() const {
        return transform;
      }

      void Model::setLocalTransform( Transform transform ) {
        this->transform = transform;
      }

      std::shared_ptr< Animation::Animator >& Model::getAnimatorRef() {
        return animator;
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

      void Model::draw() {

        // Models can have empty nodes which do not draw any mesh
        if( mesh ) {
          style.shader->use();

          getComputedTransform().send();
          style.material->send();

          if( animator ) {
            animator->update();
          }

          mesh->drawElements();
        }

        for( std::shared_ptr< Model >& model : submodels ) {
          model->draw();
        }
      }

    }
  }
}
