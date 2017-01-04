#include "graphics/model.hpp"
#include "graphics/material.hpp"
#include "graphics/mesh.hpp"
#include "graphics/drawable.hpp"
#include "graphics/texture.hpp"
#include "graphics/transform.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <string>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>
#include <algorithm>

namespace BlueBear {
  namespace Graphics {

    Model::Model( std::string path ) {
      loadModel( path );
    }

    // Used internally to generate child nodes
    Model::Model( aiNode* node, const aiScene* scene, std::string& directory, aiMatrix4x4 parentTransform, unsigned int level ) : directory( directory ) {
      processNode( node, scene, parentTransform, level );
    }

    glm::mat4 Model::aiToGLMmat4( aiMatrix4x4& matrix ) {
      glm::mat4 result;

      result[ 0 ] = glm::vec4( matrix[ 0 ][ 0 ], matrix[ 1 ][ 0 ], matrix[ 2 ][ 0 ], matrix[ 3 ][ 0 ] );
      result[ 1 ] = glm::vec4( matrix[ 0 ][ 1 ], matrix[ 1 ][ 1 ], matrix[ 2 ][ 1 ], matrix[ 3 ][ 1 ] );
      result[ 2 ] = glm::vec4( matrix[ 0 ][ 2 ], matrix[ 1 ][ 2 ], matrix[ 2 ][ 2 ], matrix[ 3 ][ 2 ] );
      result[ 3 ] = glm::vec4( matrix[ 0 ][ 3 ], matrix[ 1 ][ 3 ], matrix[ 2 ][ 3 ], matrix[ 3 ][ 3 ] );

      return result;
    }

    glm::vec4 Model::aiToGLMvec4( aiVector3D& vector ) {
      glm::vec4 result;

      result.x = vector.x;
      result.y = vector.y;
      result.z = vector.z;
      result.w = 1.0f;

      return result;
    }

    glm::dquat Model::aiToGLMquat( aiQuaternion& quaternion ) {
      glm::dquat result;

      result.x = quaternion.x;
      result.y = quaternion.y;
      result.z = quaternion.z;
      result.w = quaternion.w;

      return result;
    }

    void Model::loadModel( std::string path ) {
      Assimp::Importer importer;
      const aiScene* scene = importer.ReadFile( path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals );

      if( !scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode ) {
        // god this is an abomination fucking piece of shit c++ not having a fucking formatted print for god damn std::string
        std::stringstream stream( "Warning: Could not import file " );
        stream << path;
        Log::getInstance().error( "Model::loadModel", stream.str() );
        return;
      }

      directory = path.substr( 0, path.find_last_of( '/' ) );

      // Assimp's mRootNode mTransformation is NOT CORRECT for COLLADA imports!!
      scene->mRootNode->mTransformation = aiMatrix4x4();

      // If the root node has no meshes and only one child, just skip to that child
      if( scene->mRootNode->mNumChildren == 1 && scene->mRootNode->mNumMeshes == 0 ) {
        processNode( scene->mRootNode->mChildren[ 0 ], scene, aiMatrix4x4() );
      } else {
        processNode( scene->mRootNode, scene, aiMatrix4x4() );
      }

      // After everything is done, walk the tree for animations and apply them to the correct nodes
      if( scene->HasAnimations() ) {
        Log::getInstance().debug( "Model::loadModel", "Loading animations for " + path );
        loadAnimations( scene->mAnimations, scene->mNumAnimations );
      }
    }

    void Model::processNode( aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform, unsigned int level ) {
      std::string indentation;
      for( int i = 0; i != level; i++ ) {
        indentation = indentation + "\t";
      }

      Log::getInstance().debug( "Model::processNode", indentation + "Processing " + std::string( node->mName.C_Str() ) + " { " );

      aiMatrix4x4 resultantTransform = parentTransform * node->mTransformation;
      transform = aiToGLMmat4( resultantTransform );

      if( node->mNumMeshes ) {
        // Generally we're only going to worry about the first mesh here. Where is there ever more meshes? Blender doesn't seem to permit >1 mesh. I'll probably regret undoing this.
        aiMesh* mesh = scene->mMeshes[ node->mMeshes[ 0 ] ];

        Log::getInstance().debug( "Model::processNode", indentation + "\tLoading mesh " + mesh->mName.C_Str() );

        this->processMesh( mesh, scene, node->mName.C_Str(), transform );
      }

      for( int i = 0; i < node->mNumChildren; i++ ) {
        children.emplace( node->mChildren[ i ]->mName.C_Str(), std::make_unique< Model >( node->mChildren[ i ], scene, directory, resultantTransform, level + 1 ) );
      }

      Log::getInstance().debug( "Model::processNode", indentation + "}" );
     }

    void Model::processMesh( aiMesh* mesh, const aiScene* scene, std::string nodeTitle, glm::mat4 transformation ) {
      std::vector< Vertex > vertices;
      std::vector< Index > indices;

      std::shared_ptr< Material > defaultMaterial;

      for( int i = 0; i < mesh->mNumVertices; i++ ) {
        Vertex vertex;
        vertex.position = glm::vec3( transformation * aiToGLMvec4( mesh->mVertices[ i ] ) );
        vertex.normal = glm::vec3( mesh->mNormals[ i ].x, mesh->mNormals[ i ].y, mesh->mNormals[ i ].z );
        if( mesh->mTextureCoords[ 0 ] ) {
          vertex.textureCoordinates = glm::vec2( mesh->mTextureCoords[ 0 ][ i ].x, mesh->mTextureCoords[ 0 ][ i ].y );
        } else {
          vertex.textureCoordinates = glm::vec2( 0.0f, 0.0f );
        }
        vertices.push_back( vertex );
      }

      for( int i = 0; i < mesh->mNumFaces; i++ ) {
        aiFace face = mesh->mFaces[ i ];
        for( int j = 0; j < face.mNumIndices; j++ ) {
          indices.push_back( face.mIndices[ j ] );
        }
      }

      if( mesh->mMaterialIndex >= 0 ) {
        aiMaterial* material = scene->mMaterials[ mesh->mMaterialIndex ];

        defaultMaterial = std::make_shared< Material >( loadMaterialTextures( material, aiTextureType_DIFFUSE ) );
      }

      drawable = std::make_unique< Drawable >( std::make_shared< Mesh >( vertices, indices ), defaultMaterial );
    }

    TextureList Model::loadMaterialTextures( aiMaterial* material, aiTextureType type ) {
      TextureList textures;

      auto texCount = material->GetTextureCount( type );
      for( int i = 0; i < texCount; i++ ) {
        aiString str;
        material->GetTexture( type, i, &str );
        textures.push_back( std::make_shared< Texture >( directory + "/" + str.C_Str() ) );
      }

      return textures;
    }

    /**
     * Recursively discover a node name
     */
    std::shared_ptr< Model > Model::findChildById( const std::string& id ) {

      auto pair = children.find( id );
      if( pair != children.end() ) {
        return pair->second;
      }

      for( auto& pair : children ) {
        std::shared_ptr< Model > temp_ptr = findChildById( id );
        if( temp_ptr ) {
          return temp_ptr;
        }
      }

      return std::shared_ptr< Model >( nullptr );
    }

    /**
     * Load animations at the top-level of the scene graph ONLY
     */
    void Model::loadAnimations( aiAnimation** anims, unsigned int count ) {
      for( int i = 0; i != count; i++ ) {
        aiAnimation* anim = anims[ i ];

        Log::getInstance().debug( "Model::loadAnimations",
          std::string( "ANIMATION INFO: " ) + "\n\t\t\t\t\t\t " +
          "ID: " + std::to_string( i ) + "\n\t\t\t\t\t\t " +
          "Name: " + anim->mName.C_Str() + "\n\t\t\t\t\t\t " +
          "FPS: " + std::to_string( anim->mTicksPerSecond ) + "\n\t\t\t\t\t\t " +
          "Duration: " + std::to_string( anim->mDuration )  + "\n\t\t\t\t\t\t " +
          "Channels: " + std::to_string( anim->mNumChannels ) + "\n\t\t\t\t\t\t " +
          "Mesh Channels: " + std::to_string( anim->mNumMeshChannels )
        );

        // Each channel controls the behaviour of one particular node
        for( int i = 0; i != anim->mNumChannels; i++ ) {
          aiNodeAnim* nodeAnimation = anim->mChannels[ i ];
          std::shared_ptr< Model > node = findChildById( nodeAnimation->mNodeName.C_Str() );
          glm::mat4 nodeTransformInverse = glm::inverse( node->transform );

          std::map< double, KeyframeBuilder > builder;

          for( int i = 0; i != nodeAnimation->mNumPositionKeys; i++ ) {
            aiVectorKey& positionKey = nodeAnimation->mPositionKeys[ i ];

            builder[ positionKey.mTime ].positionKey = &positionKey;
          }

          for( int i = 0; i != nodeAnimation->mNumRotationKeys; i++ ) {
            aiQuatKey& rotationKey = nodeAnimation->mRotationKeys[ i ];

            builder[ rotationKey.mTime ].rotationKey = &rotationKey;
          }

          for( int i = 0; i != nodeAnimation->mNumScalingKeys; i++ ) {
            aiVectorKey& scalingKey = nodeAnimation->mScalingKeys[ i ];

            builder[ scalingKey.mTime ].scalingKey = &scalingKey;
          }

          Animation& animation = node->animations[ anim->mName.C_Str() ] = Animation();
          animation.rate = anim->mTicksPerSecond;
          animation.duration = anim->mDuration;

          // Use the keyframes in builder to assemble a premade list of transformation matrices
          // If there is a nullptr in any of the keys, use the one previous to the one in the list
          auto vectorKey = std::make_unique< aiVectorKey >();
          auto rotationKey = std::make_unique< aiQuatKey >();
          auto scalingKey = std::make_unique< aiVectorKey >();
          double key = builder.begin()->first;
          KeyframeBuilder& first = builder.begin()->second;
          if( first.positionKey == nullptr ) {
            vectorKey->mTime = key;
            vectorKey->mValue = aiVector3D();

            first.positionKey = vectorKey.get();
          }
          if( first.rotationKey == nullptr ) {
            rotationKey->mTime = key;
            rotationKey->mValue = aiQuaternion( 1.0f, 0.0f, 0.0f, 0.0f );

            first.rotationKey = rotationKey.get();
          }
          if( first.scalingKey == nullptr ) {
            scalingKey->mTime = key;
            scalingKey->mValue = aiVector3D( 1.0f, 1.0f, 1.0f );

            first.scalingKey = scalingKey.get();
          }
          // Do the first keyframe manually
          Transform firstTransform;
          firstTransform.setPosition(
            glm::vec3(
              first.positionKey->mValue.x,
              first.positionKey->mValue.y,
              first.positionKey->mValue.z
            )
          );
          firstTransform.setRotation(
            glm::quat(
              first.rotationKey->mValue.w,
              first.rotationKey->mValue.x,
              first.rotationKey->mValue.y,
              first.rotationKey->mValue.z
            )
          );
          firstTransform.setScale(
            glm::vec3(
              first.scalingKey->mValue.x,
              first.scalingKey->mValue.y,
              first.scalingKey->mValue.z
            )
          );
          animation.keyframes.emplace_back( key, firstTransform.getUpdatedMatrix() );

          for( auto i = std::next( builder.begin(), 1 ); i != builder.end(); ++i ) {
            Transform relativeTransform;
            KeyframeBuilder& current = i->second;
            KeyframeBuilder& previous = std::prev( i )->second;

            relativeTransform.setPosition(
              current.positionKey == nullptr ?
                glm::vec3( previous.positionKey->mValue.x, previous.positionKey->mValue.y, previous.positionKey->mValue.z ) :
                glm::vec3( current.positionKey->mValue.x, current.positionKey->mValue.y, current.positionKey->mValue.z )
            );

            relativeTransform.setRotation(
              current.rotationKey == nullptr ?
                glm::quat( previous.rotationKey->mValue.w, previous.rotationKey->mValue.x, previous.rotationKey->mValue.y, previous.rotationKey->mValue.z ) :
                glm::quat( current.rotationKey->mValue.w, current.rotationKey->mValue.x, current.rotationKey->mValue.y, current.rotationKey->mValue.z )
            );

            relativeTransform.setScale(
              current.scalingKey == nullptr ?
                glm::vec3( previous.scalingKey->mValue.x, previous.scalingKey->mValue.y, previous.scalingKey->mValue.z ) :
                glm::vec3( current.scalingKey->mValue.x, current.scalingKey->mValue.y, current.scalingKey->mValue.z )
            );

            // TODO: Check this matrix!!
            animation.keyframes.emplace_back( i->first, nodeTransformInverse * relativeTransform.getUpdatedMatrix() );
          }

        }
      }
    }

  }
}
