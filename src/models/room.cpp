#include "models/room.hpp"

namespace BlueBear::Models {

	Room::Room( const Graphics::SceneGraph::Light::DirectionalLight backgroundLight, const std::vector< glm::vec2 >& points )
		: backgroundLight( backgroundLight ), points( points ) {}


	Graphics::SceneGraph::Light::DirectionalLight& Room::getBackgroundLight() {
		return backgroundLight;
	}

	const std::vector< glm::vec2 >& Room::getPoints() const {
		return points;
	}
}