#include "models/room.hpp"
#include "tools/utility.hpp"

#include "log.hpp"

namespace BlueBear::Models {

	Room::Room( const Graphics::SceneGraph::Light::DirectionalLight backgroundLight, const std::vector< glm::vec2 >& points )
		: backgroundLight( backgroundLight ), points( points ) { computeDirections(); }

	Graphics::SceneGraph::Light::DirectionalLight& Room::getBackgroundLight() {
		return backgroundLight;
	}

	const Graphics::SceneGraph::Light::DirectionalLight& Room::getBackgroundLight() const {
		return backgroundLight;
	}

	const std::vector< glm::vec2 >& Room::getPoints() const {
		return points;
	}

	const std::vector< glm::vec2 >& Room::getWallNormals() const {
		return computedDirections;
	}

	void Room::computeDirections() {
		for( int i = 0; i != points.size(); i++ ) {
			glm::vec2 direction = glm::normalize(
				Tools::Utility::getCircularIndex( points, i + 1 ) -
				Tools::Utility::getCircularIndex( points, i )
			);

			computedDirections.emplace_back( -direction.y, direction.x );
		}
	}

}