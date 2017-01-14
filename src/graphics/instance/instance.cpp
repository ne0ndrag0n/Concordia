#include "graphics/instance/instance.hpp"
#include "graphics/model.hpp"
#include "graphics/drawable.hpp"
#include "graphics/animplayer.hpp"
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

      prepareInstanceRecursive( model, model.boneList );
    }

    Instance::Instance( const Model& model, std::shared_ptr< BoneList > boneList, std::shared_ptr< ModelBoneList > modelBones ) : boneList( boneList ), transform( std::make_shared< Transform >() ) {
      prepareInstanceRecursive( model, modelBones );
    }

    void Instance::prepareInstanceRecursive( const Model& model, std::shared_ptr< ModelBoneList > modelBones ) {
      // Copy the drawable
      if( model.drawable ) {
        drawable = std::make_shared< Drawable >( *model.drawable );
      }

      // Copy the anim pointer
      animations = model.animations;

      for( auto& pair : model.children ) {
        auto& child = *( pair.second );

        // Hand down the same transform as the parent to this model
        auto instance = std::make_shared< Instance >( child, boneList, modelBones );
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

    void Instance::setAnimation( const std::string& animKey ) {
      if( !animations ) {
        Log::getInstance().warn( "Instance::setAnimation", "Tried to play animation " + animKey + " but node doesn't have animations." );
        return;
      }

      auto pair = animations->find( animKey );
      if( pair == animations->end() ) {
        Log::getInstance().warn( "Instance::setAnimation", "Tried to play animation " + animKey + " but it doesn't exist on this node." );
        return;
      }

      animPlayer = std::make_shared< AnimPlayer >( pair->second );
    }

    /**
     * Public-facing overload
     */
    void Instance::drawEntity() {
      drawEntity( false, false, *this );
    }

    void Instance::drawEntity( bool dirty, bool sentBones, Instance& rootInstance ) {

      // TODO: Bone loading, sending

      // If we find one transform at this level that's dirty, every subsequent transform needs to get updated
      dirty = dirty || transform->dirty || ( animPlayer && animPlayer->generateNextFrame() );

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
