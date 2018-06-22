#ifndef SG_MODEL
#define SG_MODEL

#include "graphics/scenegraph/transform.hpp"
#include "graphics/scenegraph/drawable.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <optional>

namespace BlueBear {
  namespace Graphics {
    class Shader;

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

        Model() = default;
        Model( const std::string& id, const std::vector< Drawable >& drawables );
        Model( const Model& other );

        void sendBones( const Mesh::Mesh& mesh, const std::map< std::string, glm::mat4 >& bones );

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

        Transform getComputedTransform() const;
        Transform& getLocalTransform();
        void setLocalTransform( Transform transform );

        std::shared_ptr< Animation::Animator > getAnimator() const;
        void setAnimator( std::shared_ptr< Animation::Animator > animator );

        std::shared_ptr< Model > findChildById( const std::string& id ) const;

        void sendDeferredObjects();

        void draw( Animation::Animator* parentAnimator = nullptr );
      };

    }
  }
}


#endif
