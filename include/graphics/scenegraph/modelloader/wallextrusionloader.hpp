#ifndef WALL_EXTRUSION_LOADER
#define WALL_EXTRUSION_LOADER

#include "graphics/scenegraph/modelloader/proceduralmodelloader.hpp"
#include "graphics/utilities/textureatlas.hpp"
#include "models/infrastructure.hpp"
#include "models/wallsegment.hpp"
#include <vector>
#include <memory>

namespace BlueBear::Graphics { class Shader; }
namespace BlueBear::Graphics::SceneGraph { class Model; }
namespace BlueBear::Graphics::Vector { class Renderer; }
namespace BlueBear::Graphics::Utilities { class ShaderManager; }
namespace BlueBear::Graphics::SceneGraph::ModelLoader {

	class WallExtrusionLoader : public ProceduralModelLoader {
		const std::vector< Models::Infrastructure::FloorLevel >& floorLevels;
		std::shared_ptr< Shader > shader;
		Utilities::TextureAtlas atlas;

	public:
		WallExtrusionLoader(
			const std::vector< Models::Infrastructure::FloorLevel >& floorLevels,
			Vector::Renderer& renderer,
			Utilities::ShaderManager& shaderManager
		);
		std::shared_ptr< Model > get() override;
	};

}

#endif