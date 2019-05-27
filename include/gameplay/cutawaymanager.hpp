#ifndef CUTAWAY_MANAGER
#define CUTAWAY_MANAGER

#include "tools/vector_hash.hpp"
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
		std::vector< std::unordered_map< glm::vec2, Graphics::SceneGraph::Model* > > sortedWalls;
		std::vector< std::vector< Models::Room > >& rooms;
		const Graphics::Camera& camera;
		const int numFrames;

		std::vector< glm::vec3 > animationBaked;

		struct Animation { int step = 0; int direction = 1; };
		std::unordered_map< Graphics::SceneGraph::Model*, Animation > activeAnimations;

		void bakeAnimation();
		void updateAnimations();

	public:
		CutawayManager( std::shared_ptr< Graphics::SceneGraph::Model > wallRig, std::vector< std::vector< Models::Room > >& rooms, const Graphics::Camera& camera );

		void sortWallPanels( const std::shared_ptr< Graphics::SceneGraph::Model >& wallRig );

		void update( int currentLevel );
	};

}

#endif