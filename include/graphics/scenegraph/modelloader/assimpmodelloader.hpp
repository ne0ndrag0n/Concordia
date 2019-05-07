#ifndef SG_MODEL_LOADER_ASSIMP
#define SG_MODEL_LOADER_ASSIMP

#include "exceptions/genexc.hpp"
#include "graphics/scenegraph/animation/animation.hpp"
#include "graphics/scenegraph/modelloader/filemodelloader.hpp"
#include "graphics/scenegraph/animation/bone.hpp"
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <GL/glew.h>
#include <exception>
#include <vector>
#include <memory>
#include <map>

namespace BlueBear {
  namespace Graphics {
    class Texture;
    class Shader;

    namespace Utilities { class ShaderManager; }

    namespace SceneGraph {
      class Model;
      class Material;
      class ResourceBank;

      namespace Mesh { class Mesh; }
      namespace Animation { class Animator; }

      namespace ModelLoader {

        class AssimpModelLoader : public FileModelLoader {
          Utilities::ShaderManager& shaderManager;
          Assimp::Importer importer;
          struct ImportContext {
            const aiScene* scene;
            std::string directory;
            unsigned int logIndentation = 0;
          } context;

          void log( const std::string& tag, const std::string& message );

          std::shared_ptr< Shader > getShader( const std::string& vertexPath, const std::string& fragmentPath );
          std::shared_ptr< Texture > getTexture( const std::string& path );
          template < typename... Signature > std::shared_ptr< Material > getMaterial( Signature... params );

          unsigned int getFlags();
          std::vector< GLuint > getIndices( aiMesh* mesh );
          std::vector< std::shared_ptr< Texture > > getTextureList( aiMaterial* material, aiTextureType type );
          std::vector< std::string > getBoneIds( aiBone** bones, unsigned int numBones );
          template < typename VertexType > VertexType getVertex( const aiVector3D& vertex, const aiVector3D& normal );
          template < typename VertexType > void assignBonesToVertex( VertexType& vertex, unsigned int vertexIndex, aiBone** bones, unsigned int numBones );
          std::shared_ptr< Mesh::Mesh > getMesh( aiMesh* mesh, aiMatrix4x4 transform );
          std::shared_ptr< Material > getMaterial( aiMaterial* material );
          std::shared_ptr< Model > getNode( aiNode* node, aiMatrix4x4 parentTransform = {} );
          std::map< double, glm::mat4 > getKeyframes( aiNodeAnim* nodeAnim );
          std::shared_ptr< Animation::Bone::AnimationMap > getAnimationMapForBone( const std::string& boneId );
          Animation::Bone getBoneFromNode( aiNode* node );
          std::map< std::string, Animation::Animation > getAnimationList();
          std::shared_ptr< Animation::Animator > getAnimator( aiNode* node );

        public:
          EXCEPTION_TYPE( MalformedAnimationException, "Malformed animation data" );
          EXCEPTION_TYPE( TooManyBonesException, "Too many bones for this vertex!" );

          AssimpModelLoader( Utilities::ShaderManager& shaderManager );

          // Only a hint!
          bool useIndices = true;
          bool useBones = true;
          bool deferGLOperations = false;
          SceneGraph::ResourceBank* cache = nullptr;

          std::shared_ptr< Model > get( const std::string& filename ) override;
        };

      }
    }
  }
}

#endif
