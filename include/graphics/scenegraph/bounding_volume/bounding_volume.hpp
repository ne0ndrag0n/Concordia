#ifndef SG_MODEL_BOUNDING_VOLUME
#define SG_MODEL_BOUNDING_VOLUME

#include "graphics/scenegraph/modeltriangle.hpp"
#include "geometry/ray.hpp"
#include <vector>

namespace BlueBear::Graphics::SceneGraph::BoundingVolume {

	class BoundingVolume {
	public:
		virtual void generate( const std::vector< ModelTriangle >& triangles ) = 0;
		virtual bool intersects( const Geometry::Ray& ray ) const = 0;
	};

}

#endif