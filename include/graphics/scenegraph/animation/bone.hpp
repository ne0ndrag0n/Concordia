#ifndef SG_BONE
#define SG_BONE

#include "exceptions/genexc.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>
#include <map>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Animation {

        class Bone {
        public:
          using AnimationMap = std::map< std::string, std::map< double, glm::mat4 > >;

        private:
          Bone* parent = nullptr;
          std::string id;
          glm::mat4 matrix;
          std::vector< Bone > children;
          std::shared_ptr< AnimationMap > animations;

          void copyReparent( const Bone& other );
          void setToAnimation( const std::string& animationId, double animationTick );

        public:
          EXCEPTION_TYPE( AnimationNotFoundException, "Animation not found!" );
          EXCEPTION_TYPE( BoneNotFoundException, "Bone not found!" );

          Bone() = default;
          Bone( const std::string& id, const glm::mat4& matrix, std::shared_ptr< AnimationMap > animationMap = nullptr );
          Bone( const Bone& other );
          Bone& operator=( const Bone& other );

          std::vector< std::string > getAllIds() const;
          Bone getAnimationCopy( const std::string& animationId, double animationTick ) const;
          void addChild( const Bone& bone );
          const Bone* getChildById( const std::string& id ) const;
          glm::mat4 getMatrixById( const std::string& id ) const;
          glm::mat4 getLocalMatrix() const;
          void printToLog();
        };

      }
    }
  }
}

#endif
