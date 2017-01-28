#include "graphics/model.hpp"
#include "graphics/material.hpp"
#include "graphics/mesh.hpp"
#include "graphics/drawable.hpp"
#include "graphics/texture.hpp"
#include "graphics/transform.hpp"
#include "graphics/armature/armature.hpp"
#include "tools/utility.hpp"
#include "exceptions/itemnotfound.hpp"
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
    Model::Model(
      aiNode* node,
      const aiScene* scene,
      Model& root,
      std::string& directory,
      unsigned int level,
      Model* parent
    ) : parent( parent ), directory( directory ) {
      processNode( node, scene, root, level );
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

    glm::vec3 Model::aiToGLMvec3( aiVector3D& vector ) {
      glm::vec3 result;

      result.x = vector.x;
      result.y = vector.y;
      result.z = vector.z;

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
        processNode( scene->mRootNode->mChildren[ 0 ], scene, *this );
      } else {
        processNode( scene->mRootNode, scene, *this );
      }

      // After everything is done, walk the tree for animations and apply them to the correct nodes
      if( scene->HasAnimations() ) {
        Log::getInstance().debug( "Model::loadModel", "Loading animations for " + path );
        loadAnimations( scene->mAnimations, scene->mNumAnimations );
      }
    }

    void Model::processNode( aiNode* node, const aiScene* scene, Model& root, unsigned int level ) {
      std::string indentation;
      for( int i = 0; i != level; i++ ) {
        indentation = indentation + "\t";
      }

      Log::getInstance().debug( "Model::processNode", indentation + "Processing " + std::string( node->mName.C_Str() ) + " { " );

      assimpData.localTransform = node->mTransformation;
      transform = aiToGLMmat4( assimpData.localTransform );

      if( node->mNumMeshes ) {
        // Generally we're only going to worry about the first mesh here. Where is there ever more meshes? Blender doesn't seem to permit >1 mesh. I'll probably regret undoing this.
        aiMesh* mesh = scene->mMeshes[ node->mMeshes[ 0 ] ];

        Log::getInstance().debug( "Model::processNode", indentation + "\tLoading mesh " + mesh->mName.C_Str() );

        this->processMesh( mesh, scene, root, node->mName.C_Str() );
      }

      for( int i = 0; i < node->mNumChildren; i++ ) {
        // If mName fits a special case, call that and don't add the node! Otherwise, add it.
        aiNode* nextNode = node->mChildren[ i ];

        if( !alternateAction( nextNode, root ) ) {
          children.emplace( nextNode->mName.C_Str(), std::make_unique< Model >( nextNode, scene, root, directory, level + 1, this ) );
        } else {
          Log::getInstance().debug( "Model::processNode", indentation + "\tLoaded " + std::string( nextNode->mName.C_Str() ) );
        }
      }

      Log::getInstance().debug( "Model::processNode", indentation + "}" );
     }

    bool Model::alternateAction( aiNode* node, Model& root ) {
      const char* id = node->mName.C_Str();

      switch( Tools::Utility::hash( id ) ) {
        case Tools::Utility::hash( "Armature" ):
          root.bind = std::make_shared< Armature >( node );
          return true;
        default:
          return false;
      }
    }

    void Model::processMesh( aiMesh* mesh, const aiScene* scene, Model& root, std::string nodeTitle ) {
      std::vector< Vertex > vertices;
      std::vector< std::vector< BoneData > > boneBuilders;
      std::vector< Index > indices;

      std::shared_ptr< Material > defaultMaterial;

      for( int i = 0; i < mesh->mNumVertices; i++ ) {
        Vertex vertex;
        vertex.position = glm::vec3( aiToGLMvec4( mesh->mVertices[ i ] ) );
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

      std::vector< std::string > boneIndices;

      if( mesh->HasBones() ) {
        boneBuilders.resize( vertices.size() );

        for( int i = 0; i != mesh->mNumBones; i++ ) {
          aiBone* boneData = mesh->mBones[ i ];
          unsigned int boneID = getBoneId( boneIndices, boneData->mName.C_Str() );

          for( int i = 0; i != boneData->mNumWeights; i++ ) {
            aiVertexWeight& vertexAndWeight = boneData->mWeights[ i ];
            std::vector< BoneData >& vertexBones = boneBuilders.at( vertexAndWeight.mVertexId );

            if( vertexBones.size() == 4 ) {
              throw TooManyBonesException();
            }

            vertexBones.push_back( { boneID, vertexAndWeight.mWeight } );
          }
        }
      }

      for( int vertexIndex = 0; vertexIndex != boneBuilders.size(); vertexIndex++ ) {
        std::vector< BoneData >& vertexBoneData = boneBuilders.at( vertexIndex );
        Vertex& vertex = vertices.at( vertexIndex );

        for( int boneDataIndex = 0; boneDataIndex != vertexBoneData.size(); boneDataIndex++ ) {
          BoneData& boneData = vertexBoneData.at( boneDataIndex );

          vertex.boneIDs[ boneDataIndex ] = boneData.boneID;
          vertex.boneWeights[ boneDataIndex ] = boneData.boneWeight;
        }
      }

      drawable = std::make_unique< Drawable >(
        std::make_shared< Mesh >( vertices, indices, boneIndices, root.bind ),
        defaultMaterial
      );
    }

    unsigned int Model::getBoneId( std::vector< std::string >& list, const std::string& nodeID ) {
      auto it = std::find( list.begin(), list.end(), nodeID );

      if( it != list.end() ) {
        return std::distance( list.begin(), it ) + 1;
      } else {
        list.push_back( nodeID );

        // Implicitly add "1" to every index because index 0 is an identity matrix for non-boned meshes
        return list.size();
      }
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
        std::shared_ptr< Model > temp_ptr = pair.second->findChildById( id );
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
          std::string nodeID = nodeAnimation->mNodeName.C_Str();
          Log::getInstance().debug( "Model::loadAnimations", "Series of keyframes for node: " + nodeID );
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

          std::map< std::string, std::shared_ptr< KeyframeBundle > > nodeAnimList;

          std::shared_ptr< KeyframeBundle > animation = nodeAnimList[ anim->mName.C_Str() ] = std::make_shared< KeyframeBundle >( anim->mTicksPerSecond, anim->mDuration );

          // Add an identity transform at 0.0 to ensure correct interpolation
          animation->addKeyframe( 0.0, Transform() );

          // Use the keyframes in builder to assemble a premade list of transformation matrices
          // If there is a nullptr in any of the keys, use the one previous to the one in the list
          auto vectorKey = std::make_unique< aiVectorKey >();
          auto rotationKey = std::make_unique< aiQuatKey >();
          auto scalingKey = std::make_unique< aiVectorKey >();

          auto firstElementIterator = builder.begin();
          auto firstElementKey = firstElementIterator->first;
          KeyframeBuilder& firstElementBuilder = firstElementIterator->second;

          vectorKey->mTime = firstElementKey;
          vectorKey->mValue = aiVector3D();

          rotationKey->mTime = firstElementKey;
          rotationKey->mValue = aiQuaternion( 1.0f, 0.0f, 0.0f, 0.0f );

          scalingKey->mTime = firstElementKey;
          scalingKey->mValue = aiVector3D( 1.0f, 1.0f, 1.0f );

          aiVectorKey* usablePositionKey = vectorKey.get();
          aiQuatKey* usableRotationKey = rotationKey.get();
          aiVectorKey* usableScalingKey = scalingKey.get();

          for( auto& kvPair : builder ) {
            KeyframeBuilder& kb = kvPair.second;

            // Set the next group of usable keys - nulls will just use the last one set
            if( kb.positionKey != nullptr ) { usablePositionKey = kb.positionKey; }
            if( kb.rotationKey != nullptr ) { usableRotationKey = kb.rotationKey; }
            if( kb.scalingKey != nullptr ) { usableScalingKey = kb.scalingKey; }

            glm::mat4 absoluteKeyframe;
            absoluteKeyframe = glm::translate( absoluteKeyframe, glm::vec3( usablePositionKey->mValue.x, usablePositionKey->mValue.y, usablePositionKey->mValue.z ) );
            absoluteKeyframe = absoluteKeyframe * glm::toMat4( glm::quat( usableRotationKey->mValue.w, usableRotationKey->mValue.x, usableRotationKey->mValue.y, usableRotationKey->mValue.z ) );
            absoluteKeyframe = glm::scale( absoluteKeyframe, glm::vec3( usableScalingKey->mValue.x, usableScalingKey->mValue.y, usableScalingKey->mValue.z ) );

            animation->addKeyframe( kvPair.first, Transform( absoluteKeyframe ) );
          }

        }
      }
    }
  }
}
