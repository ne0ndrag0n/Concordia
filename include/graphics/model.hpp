#ifndef MODEL3D
#define MODEL3D

#include "graphics/material.hpp"
#include "graphics/drawable.hpp"
#include "graphics/animation.hpp"
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

      public:
        std::unique_ptr< Drawable > drawable;
        // not acceptable if we eventually develop the ability to "break off" pieces of a model
        Model* parent;
        std::map< std::string, std::shared_ptr< Model > > children;
        std::shared_ptr< std::map< std::string, Animation > > animations;

        Model( std::string path );
        Model( Model* parent, aiNode* node, const aiScene* scene, std::string& directory, aiMatrix4x4 parentTransform, unsigned int level );

      private:
        struct KeyframeBuilder {
          aiVectorKey* positionKey;
          aiQuatKey* rotationKey;
          aiVectorKey* scalingKey;
        };
        std::string directory;
        glm::mat4 transform;

        glm::mat4 aiToGLMmat4( aiMatrix4x4& matrix );

        glm::vec4 aiToGLMvec4( aiVector3D& vector );

        glm::dquat aiToGLMquat( aiQuaternion& quaternion );

        Model* getRootOfThis();

        void loadModel( std::string path );

        void processNode( aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform, unsigned int level = 0 );

        void processMesh( aiMesh* mesh, const aiScene* scene, std::string nodeTitle, glm::mat4 transformation );

        TextureList loadMaterialTextures( aiMaterial* material, aiTextureType type );

        std::shared_ptr< Model > findChildById( const std::string& id );

        void loadAnimations( aiAnimation** anims, unsigned int count );
    };
  }
}
#endif
