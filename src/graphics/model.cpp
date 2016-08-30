#include "graphics/model.hpp"
#include "graphics/material.hpp"
#include "graphics/mesh.hpp"
#include "graphics/drawable.hpp"
#include "graphics/texture.hpp"
#include "log.hpp"
#include <string>
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

namespace BlueBear {
  namespace Graphics {

    Model::Model( std::string path ) {
      loadModel( path );
    }

    // Used internally to generate child nodes
    Model::Model( aiNode* node, const aiScene* scene, std::string& directory, aiMatrix4x4 parentTransform ) : directory( directory ) {
      processNode( node, scene, parentTransform );
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

      // If the root node has no meshes and only one child, just skip to that child
      if( scene->mRootNode->mNumChildren == 1 && scene->mRootNode->mNumMeshes == 0 ) {
        processNode( scene->mRootNode->mChildren[ 0 ], scene, aiMatrix4x4() );
      } else {
        processNode( scene->mRootNode, scene, aiMatrix4x4() );
      }
    }

    void Model::processNode( aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform ) {
      Log::getInstance().debug( "Model::processNode", "Processing " + std::string( node->mName.C_Str() ) );

      aiMatrix4x4 resultantTransform = parentTransform * node->mTransformation;

      for( int i = 0; i < node->mNumMeshes; i++ ) {
        aiMesh* mesh = scene->mMeshes[ node->mMeshes[ i ] ];

        std::stringstream stream;
        stream << '\t' << "Loading mesh " << mesh->mName.C_Str();
        Log::getInstance().debug( "Model::processNode", stream.str() );

        this->processMesh( mesh, scene, node->mName.C_Str(), aiToGLMmat4( resultantTransform ) );
      }

      for( int i = 0; i < node->mNumChildren; i++ ) {
        children.emplace( node->mChildren[ i ]->mName.C_Str(), std::make_unique< Model >( node->mChildren[ i ], scene, directory, resultantTransform ) );
      }

      Log::getInstance().debug( "Model::processNode", "Done with " + std::string( node->mName.C_Str() ) );
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

      drawables.emplace( nodeTitle, Drawable( std::make_shared< Mesh >( vertices, indices ), defaultMaterial ) );
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

  }
}
