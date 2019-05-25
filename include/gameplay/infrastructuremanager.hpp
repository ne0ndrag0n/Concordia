#ifndef INFRASTRUCTUREMANAGER
#define INFRASTRUCTUREMANAGER

#include "state/substate.hpp"
#include "graphics/scenegraph/light/lightmap_manager.hpp"
#include "graphics/scenegraph/light/directionallight.hpp"
#include "graphics/scenegraph/model.hpp"
#include "gameplay/cutawaymanager.hpp"
#include "models/infrastructure.hpp"
#include "models/wallsegment.hpp"
#include "models/room.hpp"
#include "tools/intersection_map.hpp"
#include "tools/sector_discovery.hpp"
#include <jsoncpp/json/json.h>
#include <glm/glm.hpp>
#include <optional>
#include <memory>
#include <vector>

namespace BlueBear::State { class HouseholdGameplayState; }
namespace BlueBear::Graphics::Utilities{ class ShaderManager; }
namespace BlueBear::Gameplay {

	class InfrastructureManager : public State::Substate {
		Models::Infrastructure model;
		Graphics::SceneGraph::Light::LightmapManager lightmapManager;
		std::optional< Gameplay::CutawayManager > cutawayManager;
		std::shared_ptr< Graphics::SceneGraph::Model > floorModel;
		std::shared_ptr< Graphics::SceneGraph::Model > wallModel;
		std::vector< std::vector< Models::Room > > rooms;

		std::vector< glm::vec2 > generateRoomNodes( const Tools::Sector& sector, const glm::uvec2& dimensions );
		Tools::Intersection::IntersectionList getIntersections( const std::vector< Models::WallSegment >& wallSegments );

	public:
		InfrastructureManager( State::State& state );

		void loadInfrastructure( const Json::Value& infrastructure );

		void generateRooms();

		bool update() override;
	};

}

#endif