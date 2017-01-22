#ifndef MODEL3D
#define MODEL3D

#include "graphics/material.hpp"
#include "graphics/drawable.hpp"
#include "graphics/keyframebundle.hpp"
#include "graphics/bone.hpp"
#include <exception>
#include <string>
#include <map>
#include <memory>
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

    class Model {
      using BoneList = std::vector< Bone< Model > >;

      public:
        std::unique_ptr< Drawable > drawable;
        std::map< std::string, std::shared_ptr< Model > > children;
        std::shared_ptr< std::map< std::string, std::shared_ptr< KeyframeBundle > > > animations;

        Model( std::string path );
        Model( aiNode* node, const aiScene* scene, Model& root, std::string& directory, aiMatrix4x4 parentTransform, unsigned int level );

      private:
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

        glm::mat4 aiToGLMmat4( aiMatrix4x4& matrix );

        glm::vec4 aiToGLMvec4( aiVector3D& vector );

        glm::vec3 aiToGLMvec3( aiVector3D& vector );

        glm::dquat aiToGLMquat( aiQuaternion& quaternion );

        unsigned int getIndexOfNode( std::shared_ptr< Model > bone, aiMatrix4x4& ibpMatrix );

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
