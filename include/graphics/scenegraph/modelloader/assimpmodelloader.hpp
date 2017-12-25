#ifndef SG_MODEL_LOADER_ASSIMP
#define SG_MODEL_LOADER_ASSIMP

#include "exceptions/genexc.hpp"
#include "graphics/scenegraph/modelloader/modelloader.hpp"
#include "graphics/scenegraph/animation/animator.hpp"
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

      namespace Animation {
        class Bone;
      }

      namespace ModelLoader {

        class AssimpModelLoader : public FileModelLoader {
          EXCEPTION_TYPE( MalformedAnimationException, "Malformed animation" );

          Assimp::Importer importer;
          struct ImportContext {
            const aiScene* scene;
            std::string directory;
          } context;

          unsigned int getFlags();
          std::vector< GLuint > getIndices( aiMesh* mesh );
          std::vector< std::shared_ptr< Texture > > getTextureList( aiMaterial* material, aiTextureType type );
          std::vector< std::string > getBoneIds( aiBone** bones, unsigned int numBones );
          template < typename VertexType > VertexType getVertex( aiVector3D& vertex, aiVector3D& normal );
          template < typename VertexType > void assignBonesToVertex( VertexType& vertex, unsigned int vertexIndex, aiBone** bones, unsigned int numBones );
          std::shared_ptr< Mesh::Mesh > getMesh( aiNode* node );
          std::shared_ptr< Material > getMaterial( aiMaterial* material );
          std::shared_ptr< Model > getNode( aiNode* node );
          std::map< double, glm::mat4 > getKeyframes( aiNodeAnim* nodeAnim );
          std::shared_ptr< Animation::Bone::AnimationMap > getAnimationMapForBone( const std::string& boneId );
          Animation::Bone getBoneFromNode( aiNode* node );
          std::unique_ptr< Animation::Animator > getAnimator( aiNode* node );

        public:
          EXCEPTION_TYPE( BadModelException, "Model could not be loaded!" );
          EXCEPTION_TYPE( TooManyBonesException, "Too many bones for this vertex!" );

          bool hintNoIndices = false;
          bool useBones = true;

          std::shared_ptr< Model > get( const std::string& filename ) override;
        };

      }
    }
  }
}

#endif
