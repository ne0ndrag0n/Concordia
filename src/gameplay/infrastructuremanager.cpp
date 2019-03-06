#include "gameplay/infrastructuremanager.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/scenegraph/light/sector_illuminator.hpp"
#include "graphics/vector/renderer.hpp"
#include "state/householdgameplaystate.hpp"
#include "tools/intersection_map.hpp"
#include "application.hpp"

namespace BlueBear::Gameplay {

	InfrastructureManager::InfrastructureManager( State::State& state ) : State::Substate( state ) {
		sectorLights = std::make_shared< Graphics::SceneGraph::Light::SectorIlluminator >();
	}

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
		float currentLevel = 0.0f;

		for( const auto& level : model.getLevels() ) {
			// Generate intersection map from existing intersections/crossovers
			Tools::Intersection::IntersectionList intersectionList;
			for( const auto& wallSegment : level.wallSegments ) {
				Log::getInstance().debug( "test 1", glm::to_string( wallSegment.start ) + " - " + glm::to_string( wallSegment.end ) );
				intersectionList.emplace_back( Tools::Intersection::IntersectionLineSegment{ wallSegment.start, wallSegment.end } );
			}
			intersectionList = Tools::Intersection::generateIntersectionalList( std::move( intersectionList ) );

			Log::getInstance().debug( "---", "---" );

			Tools::SectorIdentifier sectorIdentifier;
			for( const auto& computedSegment : intersectionList ) {
				Log::getInstance().debug( "test 2", glm::to_string( computedSegment.start ) + " - " + glm::to_string( computedSegment.end ) );
				sectorIdentifier.addEdge( computedSegment.start, computedSegment.end );
			}

			auto sectors = sectorIdentifier.getSectors();

			Log::getInstance().debug( "---", "---" );

			for( const Tools::Sector& sector : sectors ) {
				Log::getInstance().debug( "test 3", "Identified sector:" );

				const Tools::SectorDiscoveryNode* from = nullptr;
				const Tools::SectorDiscoveryNode* to = nullptr;
				std::vector< std::pair< glm::vec3, glm::vec3 > > sides;
				for( const Tools::SectorDiscoveryNode* node : sector ) {
					Log::getInstance().debug( "test 3", glm::to_string( node->position ) );

					if( from == nullptr ) {
						from = node;
					} else {
						to = node;
					}

					if( to ) {
						sides.emplace_back( glm::vec3{ from->position.x, from->position.y, currentLevel }, glm::vec3{ to->position.x, to->position.y, currentLevel } );
						from = to;
					}
				}
				sides.emplace_back( glm::vec3{ sector.back()->position.x, sector.back()->position.y, currentLevel }, glm::vec3{ sector.front()->position.x, sector.front()->position.y, currentLevel } );

				// TODO: actual lighting in sectors
				sectorLights->insert( {
					{ 0.5, 0.5, -0.1 },
					{ 0.1, 0.1, 0.1 },
					{ 0.3, 0.3, 0.3 },
					{ 0.1, 0.1, 0.1 },
					sides
				} );
			}

			glm::ivec2 dimensions = level.dimensions;
			sectorLights->setOrigin( currentLevel, glm::vec3{ -( dimensions.x * 0.5f ), dimensions.y * 0.5f, 0.0f } );
			currentLevel += 4.0f;
		}

		// Falls through if already added
		state.as< State::HouseholdGameplayState >().getWorldRenderer().addIlluminator( "__sector_illuminator", sectorLights );
		sectorLights->send();
	}

	bool InfrastructureManager::update() {

	}

}