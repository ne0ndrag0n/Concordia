#ifndef FRAGMENT_RENDERER_GRID
#define FRAGMENT_RENDERER_GRID

#include "graphics/uniform_cache.hpp"
#include "tools/vector_hash.hpp"
#include "graphics/shader.hpp"
#include <unordered_map>
#include <glm/glm.hpp>

namespace BlueBear::Graphics::FragmentRenderers {

	class GridFragmentRenderer {
	public:
		struct SelectedRegion {
			glm::vec2 region;
			glm::vec4 color;
		};

	private:
		glm::vec2 origin = glm::vec2{ -5.0f, 5.0f };
		glm::vec2 dimensions = glm::vec2{ 10.0f, 10.0f };
		glm::vec4 gridColor = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
		float lineSize = 0.025f;
		bool activated = false;

		std::unordered_map< glm::vec2, glm::vec4 > selectedRegions;

		struct GridUniformBundle {
			Shader::Uniform origin;
			Shader::Uniform dimensions;
			Shader::Uniform gridColor;
			Shader::Uniform lineSize;
			Shader::Uniform activated;

			std::vector< Shader::Uniform > selectedRegionsRegion;
			std::vector< Shader::Uniform > selectedRegionsColor;

			Shader::Uniform numSelectedRegions;

			GridUniformBundle() = default;
			GridUniformBundle( const Shader& shader );
		};

		UniformCache< GridUniformBundle > uniforms;

		void send( const Shader& shader );

	public:
		GridFragmentRenderer();
		~GridFragmentRenderer();

		void setParams( const glm::vec2& origin, const glm::vec2& dimensions );
		std::pair< glm::vec2, glm::vec2 > getParams() const;

		void setActivated( bool activated );
		bool getActivated() const;

		void setColor( const glm::vec4& gridColor );
		glm::vec4 getColor() const;

		std::vector< SelectedRegion > getSelectedRegions() const;
		void setSelected( const SelectedRegion& region );
	};

}

#endif