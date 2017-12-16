#include "graphics/scenegraph/modelloader/assimpmodelloader.hpp"
#include "graphics/scenegraph/model.hpp"
#include "log.hpp"
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace ModelLoader {

        unsigned int AssimpModelLoader::getFlags() {
          unsigned int result = aiProcess_Triangulate | aiProcess_FlipUVs;

          if( useIndices ) {
            result |= aiProcess_JoinIdenticalVertices;
          }

          return result;
        }

        std::shared_ptr< Model > AssimpModelLoader::get( const std::string& filename ) {
          importPackage = ImportPackage();

          importPackage.scene = importer.ReadFile( filename, getFlags() );
          if( !importPackage.scene || importPackage.scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !importPackage.scene->mRootNode ) {
            Log::getInstance().error( "AssimpModelLoader::get", std::string( "Warning: could not import file " ) + filename );
            Log::getInstance().error( "AssimpModelLoader::get", importer.GetErrorString() );
            throw BadModelException();
          }

          importPackage.directory = filename.substr( 0, filename.find_last_of( '/' ) );
          // Fix Assimp's incoorect root transformation for COLLADA imports
          importPackage.scene->mRootNode->mTransformation = aiMatrix4x4();

          std::shared_ptr< Model > result;


          importer.FreeScene();
          return result;
        }

      }
    }
  }
}
