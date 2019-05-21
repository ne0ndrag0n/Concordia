#ifndef SHADER_UNIFORM_CACHE
#define SHADER_UNIFORM_CACHE

#include "graphics/shader.hpp"
#include <unordered_map>

namespace BlueBear::Graphics {

	template< typename Bundle >
	class UniformCache {
		std::unordered_map< const Shader*, Bundle > bundles;

	public:
		const Bundle& getUniforms( const Shader& shader ) {
			auto it = bundles.find( &shader );
			if( it != bundles.end() ) {
				return it->second;
			}

			return bundles[ &shader ] = Bundle( shader );
		};
	};

}

#endif