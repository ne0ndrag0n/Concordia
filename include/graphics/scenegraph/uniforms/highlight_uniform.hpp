#ifndef HIGHLIGHT_UNIFORM
#define HIGHLIGHT_UNIFORM

#include "graphics/scenegraph/uniform.hpp"
#include "graphics/shader.hpp"
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

		std::unordered_map< const void*, Shader::Uniform > uniforms;
		Shader::Uniform getUniform( const Shader* shader );

	public:
		HighlightUniform( const std::string& uniformId, float animDuration );
		std::unique_ptr< Uniform > copy() override;

		double getAnimDuration() const;
		void setAnimDuration( float animDuration );

		void fadeTo( const glm::vec4& color );

		void update() override;
		void send( const Shader& shader ) override;
	};

}

#endif