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
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
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
        void configureBones( const Mesh::Mesh& mesh, const std::map< std::string, glm::mat4 >& bones );
        void sendUniforms() const;
        void removeUniformEvents() const;
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
        void addChild( std::shared_ptr< Model > child );
        void detach();

        Drawable& getDrawable( unsigned int index );
        std::vector< Drawable >& getDrawableList();

        Uniform* getUniform( const std::string& id );
        void setUniform( const std::string& id, std::unique_ptr< Uniform > uniform );
        void removeUniform( const std::string& id );

        Transform getComputedTransform() const;
        Transform& getLocalTransform();
        void setLocalTransform( Transform transform );

        std::shared_ptr< Animation::Animator > getAnimator() const;
        void setAnimator( std::shared_ptr< Animation::Animator > animator );

        std::shared_ptr< Model > findChildById( const std::string& id ) const;

        void sendDeferredObjects();

        std::vector< ModelTriangle > getModelTriangles( Animation::Animator* parentAnimator = nullptr ) const;
        bool intersectsBoundingVolume( const Geometry::Ray& ray );

        void draw( Animation::Animator* parentAnimator = nullptr );
      };

    }
  }
}


#endif
