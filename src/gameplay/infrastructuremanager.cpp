#include "gameplay/infrastructuremanager.hpp"
#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/vector/renderer.hpp"
#include "state/householdgameplaystate.hpp"
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

	void InfrastructureManager::generateRooms() {
		for( const auto& level : model.getLevels() ) {
			Tools::SectorDiscovery::SectorDiscoveryGraph graph;

			for( const auto& wallSegment : level.wallSegments ) {
				Tools::SectorDiscovery::addEdge( graph, wallSegment.start, wallSegment.end );
			}

			sectors.emplace_back();
			Tools::SectorDiscovery::findSectors( &graph.begin()->second, nullptr, sectors[ sectors.size() - 1 ] );
		}

		// debug shit
		for( const auto& sectorLevel : sectors ) {
			for( const auto& sectorList : sectorLevel ) {
				for( const auto& node : sectorList ) {
					Log::getInstance().debug( "test", Tools::Utility::glmToString( node ) );
				}
			}
		}
	}

	bool InfrastructureManager::update() {

	}

}