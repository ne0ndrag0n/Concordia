#include "graphics/scenegraph/modelloader/assimpmodelloader.hpp"
#include "graphics/scenegraph/mesh/meshdefinition.hpp"
#include "graphics/scenegraph/mesh/basicvertex.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "graphics/scenegraph/mesh/riggedvertex.hpp"
#include "graphics/scenegraph/mesh/texturedriggedvertex.hpp"
#include "graphics/scenegraph/model.hpp"
#include "tools/assimptools.hpp"
#include "log.hpp"
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>


namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace ModelLoader {

        unsigned int AssimpModelLoader::getFlags() {
          unsigned int result = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals;

          if( hintNoIndices ) {
            result |= aiProcess_JoinIdenticalVertices;
          }

          return result;
        }

        std::vector< GLuint > AssimpModelLoader::getIndices( aiMesh* mesh ) {
          std::vector< GLuint > indices;

          for( int i = 0; i < mesh->mNumFaces; i++ ) {
            aiFace face = mesh->mFaces[ i ];
            for( int j = 0; j < face.mNumIndices; j++ ) {
              indices.push_back( face.mIndices[ j ] );
            }
          }

          return indices;
        }

        std::shared_ptr< Mesh::Mesh > AssimpModelLoader::getMesh( aiNode* node ) {
          if( node->mNumMeshes ) {
            aiMesh* mesh = importPackage.scene->mMeshes[ node->mMeshes[ 0 ] ];
            bool usesBones = useBones && mesh->HasBones();
            bool usesTextures = (
              mesh->mMaterialIndex >= 0 && (
                importPackage.scene->mMaterials[ mesh->mMaterialIndex ]->GetTextureCount( aiTextureType_DIFFUSE ) > 0 ||
                importPackage.scene->mMaterials[ mesh->mMaterialIndex ]->GetTextureCount( aiTextureType_SPECULAR ) > 0
              )
            );
            bool usesIndices = !hintNoIndices || mesh->mNumFaces;

            std::shared_ptr< Mesh::Mesh > result;

            if( usesBones ) {
              // Bones are stored in this backward format where we need to put the IDs together first
              // TODO

              if( usesTextures ) {
                // usesBones && usesTextures - TexturedRiggedVertex (textured material, bones)
                std::vector< Mesh::TexturedRiggedVertex > vertices;
                vertices.reserve( mesh->mNumVertices );

              } else {
                // usesBones && !usesTextures - RiggedVertex (solid material, bones)
                std::vector< Mesh::RiggedVertex > vertices;
                vertices.reserve( mesh->mNumVertices );
              }
            } else {
              if( usesTextures ) {
                // !usesBones && usesTextures - TexturedVertex (textured material, no bones)
                std::vector< Mesh::TexturedVertex > vertices;

                for( int i = 0; i < mesh->mNumVertices; i++ ) {
                  Mesh::TexturedVertex vertex;
                  vertex.position = glm::vec3( Tools::AssimpTools::aiToGLMvec4( mesh->mVertices[ i ] ) );
                  vertex.normal = glm::vec3( mesh->mNormals[ i ].x, mesh->mNormals[ i ].y, mesh->mNormals[ i ].z );
                  vertex.textureCoordinates = glm::vec2( mesh->mTextureCoords[ 0 ][ i ].x, mesh->mTextureCoords[ 0 ][ i ].y );
                  vertices.push_back( vertex );
                }

                result = usesIndices ?
                  std::make_shared< Mesh::MeshDefinition< Mesh::TexturedVertex > >( vertices, getIndices( mesh ) ) :
                  std::make_shared< Mesh::MeshDefinition< Mesh::TexturedVertex > >( vertices );

              } else {
                // !usesBones && !usesTextures - BasicVertex (solid material, no bones)
                std::vector< Mesh::BasicVertex > vertices;

                for( int i = 0; i < mesh->mNumVertices; i++ ) {
                  Mesh::BasicVertex vertex;
                  vertex.position = glm::vec3( Tools::AssimpTools::aiToGLMvec4( mesh->mVertices[ i ] ) );
                  vertex.normal = glm::vec3( mesh->mNormals[ i ].x, mesh->mNormals[ i ].y, mesh->mNormals[ i ].z );
                  vertices.push_back( vertex );
                }

                result = usesIndices ?
                  std::make_shared< Mesh::MeshDefinition< Mesh::BasicVertex > >( vertices, getIndices( mesh ) ) :
                  std::make_shared< Mesh::MeshDefinition< Mesh::BasicVertex > >( vertices );
              }
            }

            return result;
          } else {
            return std::shared_ptr< Mesh::Mesh >( nullptr );
          }
        }

        std::shared_ptr< Model > AssimpModelLoader::getNode( aiNode* node ) {

        }

        std::shared_ptr< Model > AssimpModelLoader::get( const std::string& filename ) {
          importPackage = ImportPackage();

          importPackage.scene = importer.ReadFile( filename, getFlags() );
          if( !importPackage.scene || importPackage.scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !importPackage.scene->mRootNode ) {
            Log::getInstance().error( "AssimpModelLoader::get", std::string( "Warning: could not import file " ) + filename );
            Log::getInstance().error( "AssimpModelLoader::get", importer.GetErrorString() );
            throw BadModelException();
          }

          importPackage.directory = filename.substr( 0, filename.find_last_of( '/' ) );
          // Fix Assimp's incoorect root transformation for COLLADA imports
          importPackage.scene->mRootNode->mTransformation = aiMatrix4x4();

          std::shared_ptr< Model > result;

          importer.FreeScene();
          return result;
        }

      }
    }
  }
}
