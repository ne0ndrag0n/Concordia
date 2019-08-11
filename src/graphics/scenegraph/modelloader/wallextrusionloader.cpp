#include "graphics/scenegraph/modelloader/wallextrusionloader.hpp"

namespace BlueBear::Graphics::SceneGraph::ModelLoader {

	WallExtrusionLoader::WallExtrusionLoader(
		const std::vector< Models::Infrastructure::FloorLevel >& floorLevels,
		Vector::Renderer& renderer,
		Utilities::ShaderManager& shaderManager
	) : floorLevels( floorLevels ) {

	}

	std::shared_ptr< Model > WallExtrusionLoader::get() {
		return nullptr;
	}

}