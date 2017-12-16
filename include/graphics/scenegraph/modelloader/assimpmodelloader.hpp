#ifndef SG_MODEL_LOADER_ASSIMP
#define SG_MODEL_LOADER_ASSIMP

#include "graphics/scenegraph/modelloader/modelloader.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <exception>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      class Model;

      namespace ModelLoader {

        class AssimpModelLoader : public FileModelLoader {
          Assimp::Importer importer;
          struct ImportPackage {
            const aiScene* scene;
            std::string directory;
          } importPackage;

          unsigned int getFlags();

        public:
          struct BadModelException : public std::exception {
            const char* what() const throw() {
              return "Model could not be loaded!";
            }
          };

          bool useIndices = false;

          std::shared_ptr< Model > get( const std::string& filename ) override;
        };

      }
    }
  }
}

#endif
