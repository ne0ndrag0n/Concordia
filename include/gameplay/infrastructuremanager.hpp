#ifndef INFRASTRUCTUREMANAGER
#define INFRASTRUCTUREMANAGER

#include "state/substate.hpp"
#include "models/infrastructure.hpp"
#include <jsoncpp/json/json.h>
#include <glm/glm.hpp>
#include <vector>

namespace BlueBear::State { class HouseholdGameplayState; }
namespace BlueBear::Gameplay {

	class InfrastructureManager : public State::Substate {
		Models::Infrastructure model;
		std::vector< std::vector< glm::vec3 > > rooms;

		struct RoomBuilderCell {
			bool upperWall;
			bool leftWall;
			bool diagonalWall;
			bool reverseDiagonalWall;
			int upperColor = -1;
			int rightColor = -1;
			int lowerColor = -1;
			int leftCorner = -1;
		};

		using RoomBuilderMap = std::vector< std::vector< RoomBuilderCell > >;

		RoomBuilderMap prepareRoomBuilderMap( const glm::ivec2& dimensions );
		void addLineSegment( glm::ivec2 start, glm::ivec2 end, RoomBuilderMap& map );

	public:
		InfrastructureManager( State::State& state );

		void loadInfrastructure( const Json::Value& infrastructure );

		void generateRooms();

		bool update() override;
	};

}

#endif