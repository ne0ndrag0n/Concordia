#ifndef INFRASTRUCTUREMANAGER
#define INFRASTRUCTUREMANAGER

#include "state/substate.hpp"
#include "graphics/scenegraph/light/lightmap_manager.hpp"
#include "graphics/scenegraph/light/directionallight.hpp"
#include "graphics/scenegraph/model.hpp"
#include "models/infrastructure.hpp"
#include "models/wallsegment.hpp"
#include "models/room.hpp"
#include "tools/intersection_map.hpp"
#include "tools/sector_discovery.hpp"
#include <jsoncpp/json/json.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <optional>
#include <memory>
#include <vector>

namespace BlueBear::State { class HouseholdGameplayState; }
namespace BlueBear::Graphics::Utilities{ class ShaderManager; }
namespace BlueBear::Gameplay {

	class InfrastructureManager : public State::Substate {
		struct Animation {
			int currentFrame = 0;
			int maxFrames = 0;
			float destination = 0.0f;
		};

		Models::Infrastructure model;

		int currentLevel = 0;

		Graphics::SceneGraph::Light::LightmapManager lightmapManager;

		std::shared_ptr< Graphics::SceneGraph::Model > floorModel;
		std::shared_ptr< Graphics::SceneGraph::Model > wallModel;

		std::vector< std::vector< Models::Room > > rooms;

		std::unordered_map< Graphics::SceneGraph::Model*, Animation > activeWallAnims;

		void generateWallRig();
		void generateFloorRig();

		void updateAnimations();

		// To be run any time currentLevel changes
		void hideUpperLevels();
		// To be run any time currentLevel changes, or on mouse move
		void setWallCutaways();

		std::vector< glm::vec2 > generateRoomNodes( const Tools::Sector& sector, const glm::uvec2& dimensions );
		Tools::Intersection::IntersectionList getIntersections( const std::vector< Models::WallSegment >& wallSegments );

	public:
		InfrastructureManager( State::State& state );

		void loadInfrastructure( const Json::Value& infrastructure );
		void generateRooms();

		int getCurrentLevel() const;
		void setCurrentLevel( int currentLevel );

		bool update() override;
	};

}

#endif