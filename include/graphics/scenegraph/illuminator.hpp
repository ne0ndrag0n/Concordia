#ifndef SG_ILLUMINATOR
#define SG_ILLUMINATOR

namespace BlueBear::Graphics::SceneGraph {

	class Illuminator {
	public:
		Illuminator();
		virtual ~Illuminator();

		virtual void send() = 0;
	};

}

#endif