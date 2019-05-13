#ifndef SG_ILLUMINATOR
#define SG_ILLUMINATOR

#include <string>

namespace BlueBear::Graphics { class Shader; }
namespace BlueBear::Graphics::SceneGraph {

	class Illuminator {
	public:
		virtual std::string getPreamble() = 0;
		virtual void send( const Shader& shader ) = 0;
	};

}

#endif