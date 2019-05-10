#include "graphics/scenegraph/uniforms/highlight_uniform.hpp"
#include "tools/opengl.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <cmath>

namespace BlueBear::Graphics::SceneGraph::Uniforms {

	HighlightUniform::HighlightUniform( const std::string& uniformId, float animDuration ) : uniformId( uniformId ), animDuration( ConfigManager::getInstance().getIntValue( "fps_overview" ) * animDuration ), animating( false ), currentFrame( 0 ) {}

	Shader::Uniform HighlightUniform::getUniform( const Shader* shader ) {
		auto it = uniforms.find( shader );
		if( it != uniforms.end() ) {
			return it->second;
		}

		return uniforms[ shader ] = shader->getUniform( uniformId );
	}

	double HighlightUniform::getAnimDuration() const {
		return animDuration;
	}


	void HighlightUniform::setAnimDuration( float animDuration ) {
		this->animDuration = ConfigManager::getInstance().getIntValue( "fps_overview" ) * animDuration;
	}

	void HighlightUniform::fadeTo( const glm::vec4& color ) {
		animating = true;

		fromColor = currentColor;
		toColor = color;

		if( currentFrame != 0 ) {
			currentFrame = animDuration - currentFrame;
		}
	}

	void HighlightUniform::update() {
		if( animating ) {
			int nextFrame = currentFrame + 1;

			if( nextFrame <= animDuration ) {
				currentFrame = nextFrame;
				float alpha = ( float ) currentFrame / ( float ) animDuration;
				currentColor = glm::mix( fromColor, toColor, alpha );
			} else {
				animating = false;
				currentFrame = 0;
			}
		}
	}

	void HighlightUniform::send( const Shader& shader ) {
		shader.sendData( getUniform( &shader ), currentColor );
	}

}