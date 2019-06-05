#include "graphics/fragment_renderers/grid_fragment_renderer.hpp"

namespace BlueBear::Graphics::FragmentRenderers {

	GridFragmentRenderer::GridFragmentRenderer() {
		Shader::SHADER_CHANGE.listen( this, std::bind( &GridFragmentRenderer::send, this, std::placeholders::_1 ) );
	}

	GridFragmentRenderer::~GridFragmentRenderer() {
		Shader::SHADER_CHANGE.stopListening( this );
	}

	void GridFragmentRenderer::setParams( const glm::vec2& origin, const glm::vec2& dimensions ) {
		this->origin = origin;
		this->dimensions = dimensions;
	}

	std::pair< glm::vec2, glm::vec2 > GridFragmentRenderer::getParams() const {
		return { origin, dimensions };
	}

	void GridFragmentRenderer::setColor( const glm::vec4& gridColor ) {
		this->gridColor = gridColor;
	}

	glm::vec4 GridFragmentRenderer::getColor() const {
		return gridColor;
	}

	void GridFragmentRenderer::setActivated( bool activated ) {
		this->activated = activated;
	}


	bool GridFragmentRenderer::getActivated() const {
		return activated;
	}


	void GridFragmentRenderer::send( const Shader& shader ) {
		const GridUniformBundle& bundle = uniforms.getUniforms( shader );

		shader.sendData( bundle.origin, origin );
		shader.sendData( bundle.dimensions, dimensions );
		shader.sendData( bundle.gridColor, gridColor );
		shader.sendData( bundle.lineSize, lineSize );
		shader.sendData( bundle.activated, activated ? 1.0f : 0.0f );
	}

	GridFragmentRenderer::GridUniformBundle::GridUniformBundle( const Shader& shader ) {
		origin = shader.getUniform( "grid.origin" );
		dimensions = shader.getUniform( "grid.dimensions" );
		gridColor = shader.getUniform( "grid.gridColor" );
		lineSize = shader.getUniform( "grid.lineSize" );
		activated = shader.getUniform( "grid.activated" );
	}

}