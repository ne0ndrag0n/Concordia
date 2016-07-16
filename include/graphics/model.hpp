#ifndef MODEL3D
#define MODEL3D

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace BlueBear {
  namespace Graphics {
    class Drawable;
    class TextureList;

    class Model {

      public:
        std::map< std::string, Drawable > drawables;
        std::map< std::string, std::unique_ptr< Model > > children;

        Model( std::string path );
        Model( aiNode* node, const aiScene* scene, std::string& directory, aiMatrix4x4 parentTransform );

      private:
        std::string directory;

        glm::mat4 aiToGLMmat4( aiMatrix4x4& matrix );

        glm::vec4 aiToGLMvec4( aiVector3D& vector );

        void loadModel( std::string path );

        void processNode( aiNode* node, const aiScene* scene, aiMatrix4x4 parentTransform );

        void processMesh( aiMesh* mesh, const aiScene* scene, std::string nodeTitle, glm::mat4 transformation );

        TextureList loadMaterialTextures( aiMaterial* material, aiTextureType type );
    };
  }
}
#endif
