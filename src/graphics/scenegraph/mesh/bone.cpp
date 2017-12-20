#include "graphics/scenegraph/mesh/bone.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        const Bone* Bone::getChildById( const std::string& id ) const {
          if( this->id == id ) {
            return this;
          }

          for( const Bone& child : children ) {
            if( const Bone* result = child.getChildById( id ) ) {
              return result;
            }
          }

          return nullptr;
        }

      }
    }
  }
}
