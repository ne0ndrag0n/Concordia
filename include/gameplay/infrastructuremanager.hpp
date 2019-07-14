#ifndef INFRASTRUCTUREMANAGER
#define INFRASTRUCTUREMANAGER

#include "state/substate.hpp"
#include "serializable.hpp"
#include "graphics/fragment_renderers/grid_fragment_renderer.hpp"
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

	class InfrastructureManager : public State::Substate, public Serializable {
	public:
		enum class WallMode { WALLS_DOWN, WALLS_CUT, WALLS_UP, WALLS_ROOF };

	private:
		struct Animation {
			int currentFrame = 0;
			int maxFrames = 0;
			float destination = 0.0f;
			float source = 0.0f;
		};

		Models::Infrastructure model;

		int currentLevel = 0;
		WallMode wallMode = WallMode::WALLS_CUT;

		Graphics::SceneGraph::Light::LightmapManager lightmapManager;
		Graphics::FragmentRenderers::GridFragmentRenderer grid;

		std::shared_ptr< Graphics::SceneGraph::Model > floorModel;
		std::shared_ptr< Graphics::SceneGraph::Model > wallModel;

		std::vector< std::vector< Models::Room > > rooms;

		std::unordered_map< Graphics::SceneGraph::Model*, Animation > activeWallAnims;

		void generateWallRig();
		void generateFloorRig();

		void enqueueAnimation( Graphics::SceneGraph::Model* key, const Animation&& animation );
		void updateAnimations();

		void updateWallMode();
		// To be run any time currentLevel changes
		void hideUpperLevels();
		// To be run any time currentLevel changes, or on camera move
		void setWallCutaways();
		void setWallsDown();
		void setWallsUp();

		std::vector< glm::vec2 > generateRoomNodes( const Tools::Sector& sector, const glm::uvec2& dimensions );
		Tools::Intersection::IntersectionList getIntersections( const std::vector< Models::WallSegment >& wallSegments, const glm::ivec2& dimensions );

	public:
		InfrastructureManager( State::State& state );
		~InfrastructureManager();

		Json::Value save() override;
		void load( const Json::Value& data ) override;

		void generateRooms();

		int getCurrentLevel() const;
		void setCurrentLevel( int currentLevel );

		void setWallMode( WallMode mode );

		bool update() override;
	};

}

#endif