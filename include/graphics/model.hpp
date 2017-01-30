#ifndef MODEL3D
#define MODEL3D

#include "graphics/material.hpp"
#include "graphics/drawable.hpp"
#include "graphics/keyframebundle.hpp"
#include <exception>
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace BlueBear {
  namespace Graphics {
    class Drawable;
    class Transform;
    class Armature;

    class Model {

      public:
        std::unique_ptr< Drawable > drawable;
        std::map< std::string, std::shared_ptr< Model > > children;

        // Root-level objects
        std::shared_ptr< Armature > bind;
        std::shared_ptr< std::map< std::string, Animation > > animations;

        Model( std::string path );
        Model(
          aiNode* node,
          const aiScene* scene,
          Model& root,
          std::string& directory,
          aiMatrix4x4 parentTransform,
          unsigned int level,
          Model* parent
        );

        static glm::mat4 aiToGLMmat4( aiMatrix4x4& matrix );

        static glm::vec4 aiToGLMvec4( aiVector3D& vector );

        static glm::vec3 aiToGLMvec3( aiVector3D& vector );

        static glm::dquat aiToGLMquat( aiQuaternion& quaternion );

      private:
        Model* parent = nullptr;
        struct KeyframeBuilder {
          aiVectorKey* positionKey;
          aiQuatKey* rotationKey;
          aiVectorKey* scalingKey;
        };
        struct BoneData {
          unsigned int boneID;
          float boneWeight;
        };
        struct TooManyBonesException : public std::exception {

          const char* what() const throw() {
            return "Too many bones for this vertex!";
          }

        };
        std::string directory;
        glm::mat4 transform;
        /* This is used to track data that may be called back by an assimp method */
        struct {
          aiMatrix4x4 localTransform;
        } assimpData;

        bool alternateAction( aiNode* node, Model& root );

        unsigned int getBoneId( std::vector< std::string >& list, const std::string& nodeID );

        void loadModel( std::string path );

        void processNode( aiNode* node, const aiScene* scene, Model& root, aiMatrix4x4 parentTransform, unsigned int level = 0 );

        void processMesh( aiMesh* mesh, const aiScene* scene, Model& root, std::string nodeTitle, glm::mat4 transformation );

        TextureList loadMaterialTextures( aiMaterial* material, aiTextureType type );

        std::shared_ptr< Model > findChildById( const std::string& id );

        void loadAnimations( aiAnimation** anims, unsigned int count );
    };
  }
}
#endif
