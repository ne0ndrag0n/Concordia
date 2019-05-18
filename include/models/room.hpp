#ifndef CONCORDIA_MODELS_ROOM
#define CONCORDIA_MODELS_ROOM

#include "graphics/scenegraph/light/directionallight.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace BlueBear::Models {

	class Room {
		Graphics::SceneGraph::Light::DirectionalLight backgroundLight;
		std::vector< glm::vec2 > points;

	public:
		Room( const Graphics::SceneGraph::Light::DirectionalLight backgroundLight, const std::vector< glm::vec2 >& points );

		Graphics::SceneGraph::Light::DirectionalLight& getBackgroundLight();
		const Graphics::SceneGraph::Light::DirectionalLight& getBackgroundLight() const;
		const std::vector< glm::vec2 >& getPoints() const;
	};

}

#endif