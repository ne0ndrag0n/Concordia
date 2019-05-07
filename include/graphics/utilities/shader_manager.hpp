#ifndef CONCORDIA_SHADER_MANAGER
#define CONCORDIA_SHADER_MANAGER

#include <memory>
#include <string>
#include <map>

namespace BlueBear::Graphics{ class Shader; }
namespace BlueBear::Graphics::Utilities {

	class ShaderManager {
		std::map< std::pair< std::string, std::string >, std::shared_ptr< Shader > > shaders;

		ShaderManager( const ShaderManager& shaderManager ) = delete;
		ShaderManager& operator=( const ShaderManager& shaderManager ) = delete;

	public:
		std::shared_ptr< Shader > getShader( const std::string& vertex, const std::string& fragment, bool deferGlOperations = false );

		ShaderManager() = default;
	};

}


#endif