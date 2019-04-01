#include "graphics/scenegraph/mesh/mesh.hpp"

namespace BlueBear::Graphics::SceneGraph::Mesh {

	const std::vector< Geometry::Triangle >& Mesh::getTriangles() const {
		return genericTriangles;
	}

}