#ifndef HIGHLIGHT_UNIFORM
#define HIGHLIGHT_UNIFORM

#include "graphics/scenegraph/uniform.hpp"
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <optional>

namespace BlueBear::Graphics::SceneGraph::Uniforms {

	class HighlightUniform : public Uniform {
		std::string uniformId;
		int animDuration;

		glm::vec4 currentColor;

		bool animating;
		glm::vec4 fromColor;
		glm::vec4 toColor;
		int currentFrame;

	public:
		HighlightUniform( const std::string& uniformId, int animDuration );

		double getAnimDuration() const;
		void setAnimDuration( int animDuration );

		void fadeTo( const glm::vec4& color );

		void update() override;
		void send() override;
	};

}

#endif