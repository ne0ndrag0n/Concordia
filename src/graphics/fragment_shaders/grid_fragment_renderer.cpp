#include "graphics/fragment_renderers/grid_fragment_renderer.hpp"
#include "configmanager.hpp"

namespace BlueBear::Graphics::FragmentRenderers {

	GridFragmentRenderer::GridFragmentRenderer() : lineSize( ConfigManager::getInstance().getIntValue( "shader_grid_line_size" ) / 1000.0f ) {
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

	std::vector< GridFragmentRenderer::SelectedRegion > GridFragmentRenderer::getSelectedRegions() const {
		std::vector< GridFragmentRenderer::SelectedRegion > result;

		for( const auto& pair : selectedRegions ) {
			result.emplace_back( SelectedRegion{ pair.first, pair.second } );
		}

		return result;
	}

	void GridFragmentRenderer::setSelected( const GridFragmentRenderer::SelectedRegion& region ) {
		selectedRegions[ region.region ] = region.color;
	}

	void GridFragmentRenderer::send( const Shader& shader ) {
		const GridUniformBundle& bundle = uniforms.getUniforms( shader );

		shader.sendData( bundle.origin, origin );
		shader.sendData( bundle.dimensions, dimensions );
		shader.sendData( bundle.gridColor, gridColor );
		shader.sendData( bundle.lineSize, lineSize );
		shader.sendData( bundle.activated, activated ? 1.0f : 0.0f );

		int i = 0;
		for( const auto& selectedRegion : selectedRegions ) {
			shader.sendData( bundle.selectedRegionsRegion[ i ], selectedRegion.first );
			shader.sendData( bundle.selectedRegionsColor[ i ], selectedRegion.second );
			i++;
		}

		shader.sendData( bundle.numSelectedRegions, i );
	}

	GridFragmentRenderer::GridUniformBundle::GridUniformBundle( const Shader& shader ) {
		origin = shader.getUniform( "grid.origin" );
		dimensions = shader.getUniform( "grid.dimensions" );
		gridColor = shader.getUniform( "grid.gridColor" );
		lineSize = shader.getUniform( "grid.lineSize" );
		activated = shader.getUniform( "grid.activated" );

		static int selectableTiles = ConfigManager::getInstance().getIntValue( "shader_grid_selectable_tiles" );
		for( int i = 0; i != selectableTiles; i++ ) {
			selectedRegionsRegion.emplace_back( shader.getUniform( "selectedRegions[" + std::to_string( i ) + "].region" ) );
			selectedRegionsColor.emplace_back( shader.getUniform( "selectedRegions[" + std::to_string( i ) + "].color" ) );
		}

		numSelectedRegions = shader.getUniform( "numSelectedRegions" );
	}

}