#ifndef CONCORDIA_ENTITY_MANAGER
#define CONCORDIA_ENTITY_MANAGER

#include "scripting/entitykit/registry.hpp"
#include "scripting/entitykit/entity.hpp"
#include "serializable.hpp"
#include <memory>
#include <vector>

namespace BlueBear::Gameplay {

	class EntityManager : public Serializable {
		Scripting::EntityKit::Registry registry;
		std::vector< std::shared_ptr< Scripting::EntityKit::Entity > > activeEntities;

	public:
		Json::Value save() override;
    	void load( const Json::Value& data ) override;
	};

}

#endif