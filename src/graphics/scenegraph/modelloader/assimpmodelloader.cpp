#include "graphics/scenegraph/modelloader/assimpmodelloader.hpp"
#include "graphics/scenegraph/mesh/meshdefinition.hpp"
#include "graphics/scenegraph/mesh/basicvertex.hpp"
#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "graphics/scenegraph/mesh/riggedvertex.hpp"
#include "graphics/scenegraph/mesh/texturedriggedvertex.hpp"
#include "graphics/scenegraph/mesh/boneuniform.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/scenegraph/material.hpp"
#include "graphics/scenegraph/transform.hpp"
#include "graphics/texture.hpp"
#include "graphics/shader.hpp"
#include "tools/assimptools.hpp"
#include "log.hpp"
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/material.h>
#include <map>

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

        std::vector< std::string > AssimpModelLoader::getBoneIds( aiBone** bones, unsigned int numBones ) {
          std::vector< std::string > boneIDs;

          for( int boneIndex = 0; boneIndex < numBones; boneIndex++ ) {
            boneIDs.push_back( bones[ boneIndex ]->mName.C_Str() );
          }

          return boneIDs;
        }

        template < typename VertexType >
        VertexType AssimpModelLoader::getVertex( aiVector3D& vertex, aiVector3D& normal ) {
          VertexType result;
          result.position = Tools::AssimpTools::aiToGLMvec3( vertex );
          result.normal = Tools::AssimpTools::aiToGLMvec3( normal );
          return result;
        }
        template Mesh::TexturedRiggedVertex AssimpModelLoader::getVertex( aiVector3D&, aiVector3D& );
        template Mesh::RiggedVertex AssimpModelLoader::getVertex( aiVector3D&, aiVector3D& );
        template Mesh::TexturedVertex AssimpModelLoader::getVertex( aiVector3D&, aiVector3D& );
        template Mesh::BasicVertex AssimpModelLoader::getVertex( aiVector3D&, aiVector3D& );

        template < typename VertexType >
        void AssimpModelLoader::assignBonesToVertex( VertexType& vertex, unsigned int vertexIndex, aiBone** bones, unsigned int numBones ) {
          // Max 4 bones per vertex
          unsigned int vertexBoneNumber = 0;

          for( int boneIndex = 0; boneIndex < numBones; boneIndex++ ) {
            aiBone* bone = bones[ boneIndex ];

            for( int weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++ ) {
              aiVertexWeight& vertexWeight = bone->mWeights[ weightIndex ];

              if( vertexWeight.mVertexId == vertexIndex ) {
                if( vertexBoneNumber >= 4 ) {
                  throw TooManyBonesException();
                }

                // boneIndex + 1, because bone 0 is always an identity bone
                vertex.boneIDs[ vertexBoneNumber ] = boneIndex + 1;
                vertex.boneWeights[ vertexBoneNumber ] = vertexWeight.mWeight;
                vertexBoneNumber++;
                // This same bone is not going to influence the same vertex twice
                break;
              }
            }
          }
        }
        template void AssimpModelLoader::assignBonesToVertex( Mesh::TexturedRiggedVertex&, unsigned int, aiBone**, unsigned int );
        template void AssimpModelLoader::assignBonesToVertex( Mesh::RiggedVertex&, unsigned int, aiBone**, unsigned int );

        std::shared_ptr< Mesh::Mesh > AssimpModelLoader::getMesh( aiNode* node ) {
          if( node->mNumMeshes ) {
            aiMesh* mesh = context.scene->mMeshes[ node->mMeshes[ 0 ] ];
            bool usesBones = useBones && mesh->HasBones();
            bool usesTextures = (
              mesh->mMaterialIndex >= 0 && (
                context.scene->mMaterials[ mesh->mMaterialIndex ]->GetTextureCount( aiTextureType_DIFFUSE ) > 0 ||
                context.scene->mMaterials[ mesh->mMaterialIndex ]->GetTextureCount( aiTextureType_SPECULAR ) > 0
              )
            );
            bool usesIndices = !hintNoIndices || mesh->mNumFaces;

            std::shared_ptr< Mesh::Mesh > result;

            if( usesBones ) {
              if( usesTextures ) {
                // usesBones && usesTextures - TexturedRiggedVertex (textured material, bones)
                std::vector< Mesh::TexturedRiggedVertex > vertices;
                for( int i = 0; i < mesh->mNumVertices; i++ ) {
                  Mesh::TexturedRiggedVertex vertex = getVertex< Mesh::TexturedRiggedVertex >( mesh->mVertices[ i ], mesh->mNormals[ i ] );
                  vertex.textureCoordinates = glm::vec2( mesh->mTextureCoords[ 0 ][ i ].x, mesh->mTextureCoords[ 0 ][ i ].y );
                  assignBonesToVertex( vertex, i, mesh->mBones, mesh->mNumBones );
                  vertices.push_back( vertex );
                }

                auto md = usesIndices ?
                  std::make_shared< Mesh::MeshDefinition< Mesh::TexturedRiggedVertex > >( vertices, getIndices( mesh ) ) :
                  std::make_shared< Mesh::MeshDefinition< Mesh::TexturedRiggedVertex > >( vertices );

                md->meshUniforms.emplace( "bone", std::make_unique< Mesh::BoneUniform >(
                  getBoneIds( mesh->mBones, mesh->mNumBones )
                ) );
                result = md;

              } else {
                // usesBones && !usesTextures - RiggedVertex (solid material, bones)
                std::vector< Mesh::RiggedVertex > vertices;
                for( int i = 0; i < mesh->mNumVertices; i++ ) {
                  Mesh::RiggedVertex vertex = getVertex< Mesh::RiggedVertex >( mesh->mVertices[ i ], mesh->mNormals[ i ] );
                  assignBonesToVertex( vertex, i, mesh->mBones, mesh->mNumBones );
                  vertices.push_back( vertex );
                }

                auto md = usesIndices ?
                  std::make_shared< Mesh::MeshDefinition< Mesh::RiggedVertex > >( vertices, getIndices( mesh ) ) :
                  std::make_shared< Mesh::MeshDefinition< Mesh::RiggedVertex > >( vertices );

                md->meshUniforms.emplace( "bone", std::make_unique< Mesh::BoneUniform >(
                  getBoneIds( mesh->mBones, mesh->mNumBones )
                ) );
                result = md;

              }
            } else {
              if( usesTextures ) {
                // !usesBones && usesTextures - TexturedVertex (textured material, no bones)
                std::vector< Mesh::TexturedVertex > vertices;
                for( int i = 0; i < mesh->mNumVertices; i++ ) {
                  Mesh::TexturedVertex vertex = getVertex< Mesh::TexturedVertex >( mesh->mVertices[ i ], mesh->mNormals[ i ] );
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
                  vertices.push_back( getVertex< Mesh::BasicVertex >( mesh->mVertices[ i ], mesh->mNormals[ i ] ) );
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

        std::vector< std::shared_ptr< Texture > > AssimpModelLoader::getTextureList( aiMaterial* material, aiTextureType type ) {
          std::vector< std::shared_ptr< Texture > > textures;

          unsigned int texCount = material->GetTextureCount( type );
          for( int i = 0; i < texCount; i++ ) {
            aiString filename;
            material->GetTexture( type, i, &filename );
            textures.push_back( std::make_shared< Texture >( context.directory + "/" + filename.C_Str() ) );
          }

          return textures;
        }

        std::shared_ptr< Material > AssimpModelLoader::getMaterial( aiMaterial* material ) {
          std::shared_ptr< Material > result;

          // Determine the magic combo of solids and textures
          unsigned int diffuseTextures = material->GetTextureCount( aiTextureType_DIFFUSE );
          unsigned int specularTextures = material->GetTextureCount( aiTextureType_SPECULAR );

          // Defaults
          aiVector3D ambient( 0.f, 0.f, 0.f );
          material->Get( AI_MATKEY_COLOR_AMBIENT, ambient );
          aiVector3D diffuse( 0.f, 0.f, 0.f );
          material->Get( AI_MATKEY_COLOR_DIFFUSE, diffuse );
          aiVector3D specular( 0.f, 0.f, 0.f );
          material->Get( AI_MATKEY_COLOR_SPECULAR, specular );
          float shininess = 0.0f;
          material->Get( AI_MATKEY_SHININESS, shininess );

          if( diffuseTextures && specularTextures ) {
            result = std::make_shared< Material >(
              Tools::AssimpTools::aiToGLMvec3( ambient ),
              getTextureList( material, aiTextureType_DIFFUSE ),
              getTextureList( material, aiTextureType_SPECULAR ),
              shininess
            );
          } else if ( diffuseTextures ) {
            result = std::make_shared< Material >(
              Tools::AssimpTools::aiToGLMvec3( ambient ),
              getTextureList( material, aiTextureType_DIFFUSE ),
              Tools::AssimpTools::aiToGLMvec3( specular ),
              shininess
            );
          } else if ( specularTextures ) {
            result = std::make_shared< Material >(
              Tools::AssimpTools::aiToGLMvec3( ambient ),
              Tools::AssimpTools::aiToGLMvec3( diffuse ),
              getTextureList( material, aiTextureType_SPECULAR ),
              shininess
            );
          } else {
            // Solid colours only
            result = std::make_shared< Material >(
              Tools::AssimpTools::aiToGLMvec3( ambient ),
              Tools::AssimpTools::aiToGLMvec3( diffuse ),
              Tools::AssimpTools::aiToGLMvec3( specular ),
              shininess
            );
          }

          return result;
        }

        std::unique_ptr< Animation::Animator > AssimpModelLoader::getAnimator( aiNode* node ) {
          // TODO
          return std::unique_ptr< Animation::Animator >();
        }

        std::shared_ptr< Model > AssimpModelLoader::getNode( aiNode* node ) {
          std::shared_ptr< Mesh::Mesh > mesh = getMesh( node );
          std::shared_ptr< Shader > shader = mesh ? mesh->getDefaultShader() : nullptr;
          std::shared_ptr< Material > material = nullptr;

          // Try to load the material
          if( node->mNumMeshes ) {
            unsigned int materialIndex = context.scene->mMeshes[ node->mMeshes[ 0 ] ]->mMaterialIndex;
            if( materialIndex >= 0 ) {
              material = getMaterial( context.scene->mMaterials[ materialIndex ] );
            }
          }

          std::shared_ptr< Model > model = Model::create( node->mName.C_Str(), mesh, { shader, material } );
          model->setTransform( Transform( Tools::AssimpTools::aiToGLMmat4( node->mTransformation ) ) );

          // Add children - check this
          for( int i = 0; i < node->mNumChildren; i++ ) {
            aiNode* assimpChild = node->mChildren[ i ];
            if( assimpChild->mName.C_Str() == "_armature" ) {
              model->getAnimatorRef() = getAnimator( assimpChild );
            } else {
              std::shared_ptr< Model > child = getNode( node->mChildren[ i ] );
              child->setParent( model );
            }
          }

          return model;
        }

        std::shared_ptr< Model > AssimpModelLoader::get( const std::string& filename ) {
          context = ImportContext();

          context.scene = importer.ReadFile( filename, getFlags() );
          if( !context.scene || context.scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !context.scene->mRootNode ) {
            Log::getInstance().error( "AssimpModelLoader::get", std::string( "Warning: could not import file " ) + filename );
            Log::getInstance().error( "AssimpModelLoader::get", importer.GetErrorString() );
            throw BadModelException();
          }

          context.directory = filename.substr( 0, filename.find_last_of( '/' ) );
          // Fix Assimp's incoorect root transformation for COLLADA imports
          context.scene->mRootNode->mTransformation = aiMatrix4x4();

          std::shared_ptr< Model > result = getNode( context.scene->mRootNode );

          importer.FreeScene();
          return result;
        }

      }
    }
  }
}
