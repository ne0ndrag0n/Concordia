#ifndef LEVEL_UNIFORM
#define LEVEL_UNIFORM

#include "graphics/scenegraph/uniform.hpp"
#include "graphics/shader.hpp"
#include <unordered_map>

namespace BlueBear::Graphics::SceneGraph::Uniforms {

	class LevelUniform : public Uniform {
		float level = 0.0f;

		std::unordered_map< const void*, Shader::Uniform > uniforms;

	public:
		LevelUniform( float level );

		void setLevel( float level );
		float getLevel() const;

		void send( const Shader& shader ) override;
	};

}

#endif