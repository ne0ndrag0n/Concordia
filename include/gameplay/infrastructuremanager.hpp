#ifndef INFRASTRUCTUREMANAGER
#define INFRASTRUCTUREMANAGER

#include "state/substate.hpp"
#include "models/infrastructure.hpp"
#include <jsoncpp/json/json.h>
#include <glm/glm.hpp>

namespace BlueBear::State { class HouseholdGameplayState; }
namespace BlueBear::Gameplay {

	class InfrastructureManager : public State::Substate {
		Models::Infrastructure model;
		std::vector< std::vector< glm::vec3 > > rooms;

	public:
		InfrastructureManager( State::State& state );

		void loadInfrastructure( const Json::Value& infrastructure );

		void generateRooms();

		bool update() override;
	};

}

#endif