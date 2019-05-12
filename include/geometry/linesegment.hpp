#ifndef CONCORDIA_LINE_SEGMENT
#define CONCORDIA_LINE_SEGMENT

namespace BlueBear::Geometry {

	template< typename VectorType >
	struct LineSegment {
		VectorType from;
		VectorType to;
	};

}

#endif