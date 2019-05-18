#ifndef BOUNDED_OBJECT
#define BOUNDED_OBJECT

namespace BlueBear::Containers {

	template< typename T >
	struct BoundedObject {
		int width;
		int height;
		T object;
	};

}

#endif