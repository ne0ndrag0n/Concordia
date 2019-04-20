#include "graphics/scenegraph/bounding_volume/axis_aligned_bounding_volume.hpp"
#include "geometry/methods.hpp"
#include <limits>

namespace BlueBear::Graphics::SceneGraph::BoundingVolume {

	void AxisAlignedBoundingVolume::generate( const std::vector< ModelTriangle >& triangles ) {
		boundingBox = {
			{ std::numeric_limits< float >::max(), std::numeric_limits< float >::max(), std::numeric_limits< float >::max() },
			{ std::numeric_limits< float >::lowest(), std::numeric_limits< float >::lowest(), std::numeric_limits< float >::lowest() }
		};

		for( const auto& modelTriangle : triangles ) {
			Geometry::Triangle triangle{
				modelTriangle.second * glm::vec4{ modelTriangle.first[ 0 ], 1.0f },
				modelTriangle.second * glm::vec4{ modelTriangle.first[ 1 ], 1.0f },
				modelTriangle.second * glm::vec4{ modelTriangle.first[ 2 ], 1.0f }
			};

			boundingBox.minima.x = std::min(
				boundingBox.minima.x,
				std::min( triangle[ 0 ].x,
					std::min( triangle[ 1 ].x, triangle[ 2 ].x )
				)
			);

			boundingBox.minima.y = std::min(
				boundingBox.minima.y,
				std::min( triangle[ 0 ].y,
					std::min( triangle[ 1 ].y, triangle[ 2 ].y )
				)
			);

			boundingBox.minima.z = std::min(
				boundingBox.minima.z,
				std::min( triangle[ 0 ].z,
					std::min( triangle[ 1 ].z, triangle[ 2 ].z )
				)
			);

			boundingBox.maxima.x = std::max(
				boundingBox.maxima.x,
				std::max( triangle[ 0 ].x,
					std::max( triangle[ 1 ].x, triangle[ 2 ].x )
				)
			);

			boundingBox.maxima.y = std::max(
				boundingBox.maxima.y,
				std::max( triangle[ 0 ].y,
					std::max( triangle[ 1 ].y, triangle[ 2 ].y )
				)
			);

			boundingBox.maxima.z = std::max(
				boundingBox.maxima.z,
				std::max( triangle[ 0 ].z,
					std::max( triangle[ 1 ].z, triangle[ 2 ].z )
				)
			);
		}
	}

	bool AxisAlignedBoundingVolume::intersects( const Geometry::Ray& ray ) const {
		return Geometry::getIntersectionPoint( ray, boundingBox ).operator bool();
	}

}