#include "graphics/utilities/shader_manager.hpp"
#include "graphics/shader.hpp"

namespace BlueBear::Graphics::Utilities {

	std::shared_ptr< Shader > ShaderManager::getShader( const std::string& vertex, const std::string& fragment, bool deferGlOperations ) {
		auto it = shaders.find( { vertex, fragment } );
		if( it != shaders.end() ) {
			return it->second;
		}

		return shaders[ { vertex, fragment } ] = std::make_shared< Shader >( vertex, fragment, deferGlOperations );
	}

}