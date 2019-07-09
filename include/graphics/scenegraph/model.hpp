#ifndef SG_MODEL
#define SG_MODEL

#include "graphics/scenegraph/transform.hpp"
#include "graphics/scenegraph/bounding_volume/bounding_volume.hpp"
#include "graphics/scenegraph/drawable.hpp"
#include "graphics/scenegraph/uniform.hpp"
#include "graphics/scenegraph/modeltriangle.hpp"
#include "geometry/triangle.hpp"
#include "geometry/ray.hpp"
#include "graphics/shader.hpp"
#include <sol.hpp>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <optional>
#include <unordered_map>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      class Material;

      namespace Mesh {
        class Mesh;
      }
      namespace Animation {
        class Animator;
      }

      class Model : public std::enable_shared_from_this< Model > {
        std::string id;
        Transform transform;
        std::weak_ptr< Model > parent;
        std::vector< Drawable > drawables;
        std::shared_ptr< Animation::Animator > animator;
        std::vector< std::shared_ptr< Model > > submodels;
        std::map< std::string, std::unique_ptr< Uniform > > uniforms;
        std::unique_ptr< BoundingVolume::BoundingVolume > boundingVolume;

        std::unordered_map< const void*, Shader::Uniform > transformUniform;

        Model() = default;
        Model( const std::string& id, const std::vector< Drawable >& drawables );
        Model( const Model& other );

        void generateBoundingVolume();
        glm::mat4 getHierarchicalTransform();
        Shader::Uniform getTransformUniform( const Shader* shader );

      public:
        virtual ~Model() = default;

        static void submitLuaContributions( sol::state& lua );

        static std::shared_ptr< Model > create( const std::string& id, const std::vector< Drawable >& drawables );

        std::shared_ptr< Model > copy();

        const std::string& getId() const;
        void setId( const std::string& id );

        std::shared_ptr< Model > getParent() const;
        void addChild( std::shared_ptr< Model > child, std::optional< int > index = {} );
        void detach();
        const std::vector< std::shared_ptr< Model > >& getChildren() const;
        const std::map< std::string, std::unique_ptr< Uniform > >& getUniforms() const;

        Drawable& getDrawable( unsigned int index );
        const std::vector< Drawable >& getDrawableList() const;

        Uniform* getUniform( const std::string& id );
        void setUniform( const std::string& id, std::unique_ptr< Uniform > uniform );
        void removeUniform( const std::string& id );

        Transform getComputedTransform() const;
        Transform& getLocalTransform();
        const Transform& getLocalTransform() const;

        void setLocalTransform( Transform transform );

        std::shared_ptr< Animation::Animator > getAnimator() const;
        void setAnimator( std::shared_ptr< Animation::Animator > animator );

        std::shared_ptr< Model > findChildById( const std::string& id ) const;

        void sendDeferredObjects();

        std::vector< ModelTriangle > getModelTriangles( Animation::Animator* parentAnimator = nullptr ) const;
        bool intersectsBoundingVolume( const Geometry::Ray& ray );
        void invalidateBoundingVolume();
      };

    }
  }
}


#endif
