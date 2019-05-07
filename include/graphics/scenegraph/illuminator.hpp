#ifndef SG_ILLUMINATOR
#define SG_ILLUMINATOR

namespace BlueBear::Graphics { class Shader; }
namespace BlueBear::Graphics::SceneGraph {

	class Illuminator {
	public:
		Illuminator();
		virtual ~Illuminator();

		virtual void send( const Shader& shader ) = 0;
	};

}

#endif