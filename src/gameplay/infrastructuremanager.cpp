#include "gameplay/infrastructuremanager.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/scenegraph/light/sector_illuminator.hpp"
#include "graphics/vector/renderer.hpp"
#include "state/householdgameplaystate.hpp"
#include "application.hpp"

namespace BlueBear::Gameplay {

	static glm::vec2 correctByOrigin( const glm::ivec2& value, const glm::vec2& origin ) {
		return { origin.x + value.x, origin.y - value.y };
	}

	InfrastructureManager::InfrastructureManager( State::State& state ) : State::Substate( state ) {
		sectorLights = std::make_shared< Graphics::SceneGraph::Light::SectorIlluminator >();
	}

	void InfrastructureManager::loadInfrastructure( const Json::Value& infrastructure ) {
		model.load( infrastructure, state.as< State::HouseholdGameplayState >().getWorldCache() );

		auto& worldRenderer = state.as< State::HouseholdGameplayState >().getWorldRenderer();

		Graphics::SceneGraph::ModelLoader::FloorModelLoader floorModelLoader( model.getLevels(), state.as< State::HouseholdGameplayState >().getShaderManager() );
		Graphics::Vector::Renderer vectorRenderer( state.as< State::HouseholdGameplayState >().getApplication().getDisplayDevice() );
		Graphics::SceneGraph::ModelLoader::WallModelLoader wallModelLoader( model.getLevels(), vectorRenderer, state.as< State::HouseholdGameplayState >().getShaderManager() );

		worldRenderer.loadDirect( "__floorrig", floorModelLoader.get() );
		worldRenderer.loadDirect( "__wallrig", wallModelLoader.get() );

		worldRenderer.placeObject( "__floorrig", {} );
		worldRenderer.placeObject( "__wallrig", {} );

		generateRooms();
	}

	std::vector< glm::vec2 > InfrastructureManager::generateRoomNodes( const Tools::Sector& sector, const glm::uvec2& dimensions ) {
		std::vector< glm::vec2 > result;

		for( const Tools::SectorDiscoveryNode* node : sector ) {
			// Add origin-transformed node to room
			result.emplace_back( correctByOrigin( node->position, glm::vec2{ -( dimensions.x * 0.5f ), dimensions.y * 0.5f } ) );
		}

		// Determine winding direction - ensure it is CW and not CCW
		// (next_x - current_x)(next_y + current_y)
		// Result is positive - Clockwise (else counterclockwise)
		int windingDirection = 0;
		for( int i = 0; i != result.size(); i++ ) {
			glm::vec2 currentItem = result[ i ];
			glm::vec2 nextItem;

			if( i == ( result.size() - 1 ) ) {
				nextItem = result[ 0 ];
			} else {
				nextItem = result[ i + 1 ];
			}

			windingDirection += ( nextItem.x - currentItem.x ) * ( nextItem.y + currentItem.y );
		}

		if( windingDirection < 0 ) {
			std::vector< glm::vec2 > newResult;
			for( auto it = result.rbegin(); it != result.rend(); ++it ) {
				newResult.emplace_back( *it );
			}

			return newResult;
		}

		return result;
	}

	Tools::Intersection::IntersectionList InfrastructureManager::getIntersections( const std::vector< Models::WallSegment >& wallSegments ) {
		Tools::Intersection::IntersectionList intersectionList;
		for( const auto& wallSegment : wallSegments ) {
			Log::getInstance().debug( "InfrastructureManager::getIntersections", glm::to_string( wallSegment.start ) + " - " + glm::to_string( wallSegment.end ) );
			intersectionList.emplace_back( Tools::Intersection::IntersectionLineSegment{ wallSegment.start, wallSegment.end } );
		}

		return Tools::Intersection::generateIntersectionalList( std::move( intersectionList ) );
	}

	/**
	 * Refactoring TODO
	 */
	void InfrastructureManager::generateRooms() {
		float currentLevel = 0.0f;

		for( const auto& level : model.getLevels() ) {
			std::vector< Models::Room > roomsForLevel;

			// Generate intersection map from existing intersections/crossovers
			Tools::Intersection::IntersectionList intersectionList = getIntersections( level.wallSegments );

			Log::getInstance().debug( "InfrastructureManager::generateRooms", "---" );

			Tools::SectorIdentifier sectorIdentifier;
			for( const auto& computedSegment : intersectionList ) {
				Log::getInstance().debug( "InfrastructureManager::generateRooms", glm::to_string( computedSegment.start ) + " - " + glm::to_string( computedSegment.end ) );
				sectorIdentifier.addEdge( computedSegment.start, computedSegment.end );
			}

			auto sectors = sectorIdentifier.getSectors();

			Log::getInstance().debug( "InfrastructureManager::generateRooms", "---" );

			for( const Tools::Sector& sector : sectors ) {
				// Room object for sector
				Models::Room room(
					{
						{ 0.5, 0.5, -0.1 },
						{ 0.1, 0.1, 0.1 },
						{ 0.3, 0.3, 0.3 },
						{ 0.1, 0.1, 0.1 }
					},
					generateRoomNodes( sector, level.dimensions )
				);

				// SectorIlluminator requires legacy format we didn't have time to refactor
				Log::getInstance().debug( "InfrastructureManager::generateRooms", "Identified sector:" );

				const Tools::SectorDiscoveryNode* from = nullptr;
				const Tools::SectorDiscoveryNode* to = nullptr;
				std::vector< std::pair< glm::vec3, glm::vec3 > > sides;
				for( const Tools::SectorDiscoveryNode* node : sector ) {
					Log::getInstance().debug( "InfrastructureManager::generateRooms", glm::to_string( node->position ) );

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

				sectorLights->insert( {
					room.getBackgroundLight().getDirection(),
					room.getBackgroundLight().getAmbient(),
					room.getBackgroundLight().getDiffuse(),
					room.getBackgroundLight().getSpecular(),
					sides
				} );

				roomsForLevel.emplace_back( std::move( room ) );
			}

			glm::ivec2 dimensions = level.dimensions;
			sectorLights->setLevelData( glm::vec3{ -( dimensions.x * 0.5f ), dimensions.y * 0.5f, currentLevel }, dimensions );
			currentLevel += 4.0f;
		}

		// Falls through if already added
		state.as< State::HouseholdGameplayState >().getWorldRenderer().addIlluminator( "__sector_illuminator", sectorLights );
	}

	bool InfrastructureManager::update() {

	}

}