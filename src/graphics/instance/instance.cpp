#include "graphics/instance/instance.hpp"
#include "graphics/model.hpp"
#include "graphics/drawable.hpp"
#include "graphics/animplayer.hpp"
#include "tools/utility.hpp"
#include "tools/opengl.hpp"
#include "log.hpp"
#include <GL/glew.h>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace BlueBear {
  namespace Graphics {

    Instance::Instance( const Model& model ) : transform( std::make_shared< Transform >() ) {
      boneList = std::make_shared< BoneList >();
      for( int i = 0; i < model.boneList->size(); i++ ) {
        boneList->push_back( Bone< Instance >{ std::shared_ptr< Instance >(), glm::mat4() } );
      }
      animationList = std::make_shared< AnimationList >();

      prepareInstanceRecursive( model, model.boneList );
    }

    Instance::Instance( const Model& model, std::shared_ptr< BoneList > boneList, std::shared_ptr< ModelBoneList > modelBones, std::shared_ptr< AnimationList > animationList )
      : animationList( animationList ), boneList( boneList ), transform( std::make_shared< Transform >() ) {
      prepareInstanceRecursive( model, modelBones );
    }

    void Instance::prepareInstanceRecursive( const Model& model, std::shared_ptr< ModelBoneList > modelBones ) {
      // Copy the drawable
      if( model.drawable ) {
        drawable = std::make_shared< Drawable >( *model.drawable );
      }

      for( auto& pair : model.children ) {
        auto& child = *( pair.second );

        // Hand down the same transform as the parent to this model
        std::shared_ptr< Instance > instance = std::make_shared< Instance >( child, boneList, modelBones, animationList );
        // Collect its animations into the master animation list
        if( child.animations ) {
          for( auto& pair : *child.animations ) {
            ( *animationList )[ pair.first ].push_back( { instance, pair.second } );
          }
        }

        findMatchingSubmodel( modelBones, pair.second, instance );
        instance->transform->setParent( transform );

        children[ pair.first ] = instance;
      }
    }

    /**
     * Given a particular model node, see if it can be used to populate boneList
     */
    void Instance::findMatchingSubmodel( std::shared_ptr< ModelBoneList > modelBones, std::shared_ptr< Model > model, std::shared_ptr< Instance > inst ) {
      for( int i = 1; i != modelBones->size(); i++ ) {
        Bone< Model >& modelBone = modelBones->at( i );

        if( modelBone.node == model ) {
          Bone< Instance >& boneData = boneList->at( i );
          boneData.node = inst;
          boneData.inverseBindPose = modelBone.inverseBindPose;
          return;
        }
      }
    }

    /**
     * FIXME: This only goes one level of recursion deep, you derp
     */
    std::shared_ptr< Instance > Instance::findChildByName( std::string name ) {
      return children[ name ];
    }


    /**
     * setAnimation will act the same whether or not it's called on a child node or the root node. It will activate all animations as if called from the top of the scene graph.
     */
    void Instance::setAnimation( const std::string& animKey ) {
      auto pair = animationList->find( animKey );

      if( pair != animationList->end() ) {
        for( auto& animationBundle : pair->second ) {
          animationBundle.instance->animPlayer = std::make_shared< AnimPlayer >( *animationBundle.keyframes );
        }
      } else {
        Log::getInstance().warn( "Instance::setAnimation", "Could not find animation " + animKey );
      }
    }

    /**
     * Public-facing overload
     */
    void Instance::drawEntity() {
      drawEntity( false, false, *this );
    }

    void Instance::drawEntity( bool dirty, bool sentBones, Instance& rootInstance ) {

      // If we find one transform at this level that's dirty, every subsequent transform needs to get updated
      dirty = ( animPlayer && animPlayer->generateNextFrame() ) || transform->dirty || dirty;

      // Update if dirty
      if( dirty ) {
        transform->update( animPlayer ? animPlayer->nextMatrix : glm::mat4() );
      }

      if( drawable ) {
        sendBonesToShader( sentBones, rootInstance );
        transform->sendToShader();
        drawable->render();
      }

      for( auto& pair : children ) {
        // If "dirty" was true here, it'll get passed down to subsequent instances. But if "dirty" was false, and this call ends up being "dirty",
        // it should only propagate to its own children since dirty is passed by value here.
        pair.second->drawEntity( dirty, sentBones, rootInstance );
      }
    }

    void Instance::sendBonesToShader( bool& sentBones, Instance& rootInstance ) {
      // This method will get called on encountering the first drawable. We're assuming bones have been transformed and conquered before any meshes.
      // Do this expensive operation ONCE. Also makes the assumption that bones will be transformed before meshes; check your file formats.
      if( !sentBones ) {
        glm::mat4 inverseRoot = glm::inverse( rootInstance.transform->matrix );

        std::vector< glm::mat4 > matrices;
        // The first one is always identity
        matrices.push_back( glm::mat4() );

        for( int i = 1; i < boneList->size(); i++ ) {
          Bone< Instance >& bone = boneList->at( i );

          matrices.push_back( inverseRoot * bone.node->transform->matrix );
        }

        // TODO: Uncomment when actually ready to use. OpenGL optimizes out "bones"
        glUniformMatrix4fv( Tools::OpenGL::getUniformLocation( "bones" ), matrices.size(), GL_FALSE, glm::value_ptr( matrices[ 0 ] ) );
        sentBones = true;
      }
    }

    glm::vec3 Instance::getPosition() {
      return transform->getPosition();
    }

    void Instance::setPosition( const glm::vec3& position ) {
      transform->setPosition( position );
    }

    glm::vec3 Instance::getScale() {
      return transform->getScale();
    }

    void Instance::setScale( const glm::vec3& scale ) {
      transform->setScale( scale );
    }

    GLfloat Instance::getRotationAngle() {
      return transform->getRotationAngle();
    }

    glm::vec3 Instance::getRotationAxes() {
      return transform->getRotationAxes();
    }

    void Instance::setRotationAngle( GLfloat rotationAngle, const glm::vec3& rotationAxes ) {
      // Generate new quat
      transform->setRotationAngle( rotationAngle, rotationAxes );
    }

  }
}
