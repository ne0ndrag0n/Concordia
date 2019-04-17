#ifndef AXIS_ALIGNED_BOUNDING_VOLUME
#define AXIS_ALIGNED_BOUNDING_VOLUME

#include "graphics/scenegraph/bounding_volume/bounding_volume.hpp"
#include "geometry/aabb.hpp"

namespace BlueBear::Graphics::SceneGraph::BoundingVolume {

	class AxisAlignedBoundingVolume : public BoundingVolume {
		Geometry::AABB boundingBox;

	public:
		void generate( const std::vector< ModelTriangle >& triangles ) override;
		bool intersects( const Geometry::Ray& ray ) const override;
	};

}

#endif