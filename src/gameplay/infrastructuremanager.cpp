#include "gameplay/infrastructuremanager.hpp"
#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/vector/renderer.hpp"
#include "state/householdgameplaystate.hpp"
#include "tools/intersection_map.hpp"
#include "application.hpp"

namespace BlueBear::Gameplay {

	InfrastructureManager::InfrastructureManager( State::State& state ) : State::Substate( state ) {}

	void InfrastructureManager::loadInfrastructure( const Json::Value& infrastructure ) {
		model.load( infrastructure, state.as< State::HouseholdGameplayState >().getWorldCache() );

		auto& worldRenderer = state.as< State::HouseholdGameplayState >().getWorldRenderer();

		Graphics::SceneGraph::ModelLoader::FloorModelLoader floorModelLoader( model.getLevels() );
		Graphics::Vector::Renderer vectorRenderer( state.as< State::HouseholdGameplayState >().getApplication().getDisplayDevice() );
		Graphics::SceneGraph::ModelLoader::WallModelLoader wallModelLoader( model.getLevels(), vectorRenderer );

		worldRenderer.insertDirect( floorModelLoader.get() );
		worldRenderer.insertDirect( wallModelLoader.get() );

		generateRooms();
	}

	/**
	 * Heavy TODO
	 */
	void InfrastructureManager::generateRooms() {
		for( const auto& level : model.getLevels() ) {
			// Generate intersection map from existing intersections/crossovers
			Tools::Intersection::IntersectionList intersectionList;
			for( const auto& wallSegment : level.wallSegments ) {
				Log::getInstance().debug( "test 1", glm::to_string( wallSegment.start ) + " - " + glm::to_string( wallSegment.end ) );
				intersectionList.emplace_back( Tools::Intersection::IntersectionLineSegment{ wallSegment.start, wallSegment.end } );
			}
			intersectionList = Tools::Intersection::generateIntersectionalList( std::move( intersectionList ) );

			Log::getInstance().debug( "---", "---" );

			// remove this debugging stuff
			for( const auto& intersection : intersectionList ) {
				Log::getInstance().debug( "test 2", glm::to_string( intersection.start ) + " - " + glm::to_string( intersection.end ) );
			}
		}
	}

	bool InfrastructureManager::update() {

	}

}