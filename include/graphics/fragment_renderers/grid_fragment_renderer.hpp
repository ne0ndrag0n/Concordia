#ifndef FRAGMENT_RENDERER_GRID
#define FRAGMENT_RENDERER_GRID

#include "graphics/uniform_cache.hpp"
#include "graphics/shader.hpp"
#include <glm/glm.hpp>

namespace BlueBear::Graphics::FragmentRenderers {

	class GridFragmentRenderer {
		glm::vec2 origin = glm::vec2{ -5.0f, 5.0f };
		glm::vec2 dimensions = glm::vec2{ 10.0f, 10.0f };
		glm::vec4 gridColor = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };
		float lineSize = 0.025f;
		bool activated = false;

		struct GridUniformBundle {
			Shader::Uniform origin;
			Shader::Uniform dimensions;
			Shader::Uniform gridColor;
			Shader::Uniform lineSize;
			Shader::Uniform activated;

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
	};

}

#endif