#ifndef INFRASTRUCTUREMANAGER
#define INFRASTRUCTUREMANAGER

#include "state/substate.hpp"
#include "graphics/scenegraph/light/directionallight.hpp"
#include "models/infrastructure.hpp"
#include "models/wallsegment.hpp"
#include "tools/intersection_map.hpp"
#include "tools/sector_discovery.hpp"
#include <jsoncpp/json/json.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace BlueBear::State { class HouseholdGameplayState; }
namespace BlueBear::Graphics::Utilities{ class ShaderManager; }
namespace BlueBear::Graphics::SceneGraph::Light { class SectorIlluminator; }
namespace BlueBear::Gameplay {

	class InfrastructureManager : public State::Substate {
		struct Room {
			Graphics::SceneGraph::Light::DirectionalLight backgroundLight;
			std::vector< glm::vec2 > points;
		};

		Models::Infrastructure model;
		std::shared_ptr< Graphics::SceneGraph::Light::SectorIlluminator > sectorLights;
		std::vector< std::vector< Room > > rooms;

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