#ifndef MODEL3D
#define MODEL3D

#include "graphics/material.hpp"
#include "graphics/drawable.hpp"
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

    class Model {

      public:
        std::unique_ptr< Drawable > drawable;
        // This is generally not useful anywhere but loadAnimations, where relative vectors
        // need to be computed
        glm::mat4 transform;
        std::map< std::string, std::shared_ptr< Model > > children;

        Model( std::string path );
        Model( aiNode* node, const aiScene* scene, std::string& directory, aiMatrix4x4 parentTransform, unsigned int level );

      private:
        std::string directory;

        glm::mat4 aiToGLMmat4( aiMatrix4x4& matrix );

        glm::vec4 aiToGLMvec4( aiVector3D& vector );

        glm::dquat aiToGLMquat( aiQuaternion& quaternion );

        void loadModel( std::string path );

        void processNode( aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform, unsigned int level = 0 );

        void processMesh( aiMesh* mesh, const aiScene* scene, std::string nodeTitle, glm::mat4 transformation );

        TextureList loadMaterialTextures( aiMaterial* material, aiTextureType type );

        std::shared_ptr< Model > findChildById( const std::string& id );

        void loadAnimations( aiAnimation** animations, unsigned int count );
    };
  }
}
#endif
