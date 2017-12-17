#ifndef SG_MODEL_LOADER_ASSIMP
#define SG_MODEL_LOADER_ASSIMP

#include "graphics/scenegraph/modelloader/modelloader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <GL/glew.h>
#include <exception>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    class Texture;

    namespace SceneGraph {
      class Model;
      class Material;

      namespace Mesh {
        class Mesh;
      }

      namespace ModelLoader {

        class AssimpModelLoader : public FileModelLoader {
          Assimp::Importer importer;
          struct ImportPackage {
            const aiScene* scene;
            std::string directory;
          } importPackage;

          unsigned int getFlags();
          std::vector< GLuint > getIndices( aiMesh* mesh );
          std::vector< std::shared_ptr< Texture > > getTextureList( aiMaterial* material, aiTextureType type );
          template < typename VertexType > VertexType getVertex( aiVector3D& vertex, aiVector3D& normal );
          template < typename VertexType > void assignBonesToVertex( VertexType& vertex, unsigned int vertexIndex, aiBone** bones, unsigned int numBones );
          std::shared_ptr< Mesh::Mesh > getMesh( aiNode* node );
          std::shared_ptr< Material > getMaterial( aiMaterial* material );
          std::shared_ptr< Model > getNode( aiNode* node );

        public:
          struct BadModelException : public std::exception {
            const char* what() const throw() {
              return "Model could not be loaded!";
            }
          };
          struct TooManyBonesException : public std::exception {
            const char* what() const throw() {
              return "Too many bones for this vertex!";
            }
          };

          bool hintNoIndices = false;
          bool useBones = true;

          std::shared_ptr< Model > get( const std::string& filename ) override;
        };

      }
    }
  }
}

#endif
