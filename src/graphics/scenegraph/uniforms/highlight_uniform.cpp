#include "graphics/scenegraph/uniforms/highlight_uniform.hpp"
#include "tools/opengl.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <glm/glm.hpp>
#include <cmath>

namespace BlueBear::Graphics::SceneGraph::Uniforms {

	HighlightUniform::HighlightUniform( const std::string& uniformId, int animDuration ) : uniformId( uniformId ), animDuration( ConfigManager::getInstance().getIntValue( "fps_overview" ) * animDuration ), animating( false ), currentFrame( 0 ) {}

	double HighlightUniform::getAnimDuration() const {
		return animDuration;
	}


	void HighlightUniform::setAnimDuration( int animDuration ) {
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
				float alpha = ( float ) animDuration / ( float ) currentFrame;
				currentColor = glm::mix( fromColor, toColor, alpha );
			} else {
				animating = false;
				currentFrame = 0;
			}
		}
	}

	void HighlightUniform::send() {
		Tools::OpenGL::setUniform( uniformId, currentColor );
	}

}