#ifndef LEVEL_UNIFORM
#define LEVEL_UNIFORM

#include "graphics/scenegraph/uniform.hpp"
#include "graphics/shader.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

namespace BlueBear::Graphics::SceneGraph::Uniforms {

	class LevelUniform : public Uniform {
		float level = 0.0f;
		glm::vec2 position;

		std::unordered_map< const void*, Shader::Uniform > uniforms;

	public:
		LevelUniform( const glm::vec2& position, float level );

		const glm::vec2& getPosition() const;

		void setLevel( float level );
		float getLevel() const;

		void send( const Shader& shader ) override;
	};

}

#endif