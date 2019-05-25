#ifndef CUTAWAY_MANAGER
#define CUTAWAY_MANAGER

#include "models/room.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <optional>
#include <vector>
#include <utility>

namespace BlueBear::Graphics{ class Camera; }
namespace BlueBear::Graphics::SceneGraph{ class Model; }
namespace BlueBear::Gameplay {

	class CutawayManager {
		std::shared_ptr< Graphics::SceneGraph::Model > wallRig;
		const std::vector< std::vector< Models::Room > >& rooms;
		const Graphics::Camera& camera;

		std::vector< glm::vec3 > animationBaked;

		std::unordered_map< std::shared_ptr< Graphics::SceneGraph::Model >, int > activeAnimations;

		void bakeAnimation();
		std::vector< Graphics::SceneGraph::Model* > queryFor( const std::function< bool(const std::shared_ptr< Graphics::SceneGraph::Model >&) >& predicate, std::optional< int > level = {} );

	public:
		CutawayManager( std::shared_ptr< Graphics::SceneGraph::Model > wallRig, const std::vector< std::vector< Models::Room > >& rooms, const Graphics::Camera& camera );

		void update( int currentLevel );
	};

}

#endif