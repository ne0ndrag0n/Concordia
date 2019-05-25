#include "graphics/scenegraph/uniforms/level_uniform.hpp"

namespace BlueBear::Graphics::SceneGraph::Uniforms {

	LevelUniform::LevelUniform( const glm::vec2& position, float level ) : position( position ), level( level ) {}

	const glm::vec2& LevelUniform::getPosition() const {
		return position;
	}

	void LevelUniform::setLevel( float level ) {
		this->level = level;
	}

	float LevelUniform::getLevel() const {
		return level;
	}

	void LevelUniform::send( const Shader& shader ) {
		Shader::Uniform uniform;

		auto it = uniforms.find( &shader );
		if( it != uniforms.end() ) {
			uniform = it->second;
		} else {
			uniform = uniforms[ &shader ] = shader.getUniform( "wallBaseline" );
		}

		shader.sendData( uniform, level );
	}

}