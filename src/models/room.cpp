#include "models/room.hpp"
#include "tools/utility.hpp"

#include "log.hpp"

namespace BlueBear::Models {

	Room::Room( const Graphics::SceneGraph::Light::DirectionalLight backgroundLight, const std::vector< glm::vec2 >& points )
		: backgroundLight( backgroundLight ), points( points ) {}

	Graphics::SceneGraph::Light::DirectionalLight& Room::getBackgroundLight() {
		return backgroundLight;
	}

	const Graphics::SceneGraph::Light::DirectionalLight& Room::getBackgroundLight() const {
		return backgroundLight;
	}

	const std::vector< glm::vec2 >& Room::getPoints() const {
		return points;
	}

	const std::vector< Room::Normal >& Room::getWallNormals() {
		if( computedDirections.empty() ) {
			computeDirections();
		}

		return computedDirections;
	}

	void Room::computeDirections() {
		for( int i = 0; i != points.size(); i++ ) {
			const glm::vec2& first = Tools::Utility::getCircularIndex( points, i );
			const glm::vec2& second = Tools::Utility::getCircularIndex( points, i + 1 );

			glm::vec2 direction = glm::normalize( second - first );
			computedDirections.emplace_back( Normal{ { first, second }, { -direction.y, direction.x } } );
		}
	}

}