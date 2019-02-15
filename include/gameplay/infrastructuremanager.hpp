#ifndef INFRASTRUCTUREMANAGER
#define INFRASTRUCTUREMANAGER

#include "state/substate.hpp"
#include "models/infrastructure.hpp"
#include <jsoncpp/json/json.h>


namespace BlueBear::State { class HouseholdGameplayState; }
namespace BlueBear::Gameplay {

	class InfrastructureManager : public State::Substate {
		Models::Infrastructure model;

	public:
		InfrastructureManager( State::State& state );

		void loadInfrastructure( const Json::Value& infrastructure );

		bool update() override;
	};

}

#endif