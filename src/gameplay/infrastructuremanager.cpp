#include "gameplay/infrastructuremanager.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/vector/renderer.hpp"
#include "state/householdgameplaystate.hpp"
#include "tools/utility.hpp"
#include "application.hpp"

namespace BlueBear::Gameplay {

	static glm::vec2 correctByOrigin( const glm::ivec2& value, const glm::vec2& origin ) {
		return { origin.x + value.x, origin.y - value.y };
	}

	InfrastructureManager::InfrastructureManager( State::State& state ) : State::Substate( state ) {}

	void InfrastructureManager::loadInfrastructure( const Json::Value& infrastructure ) {
		model.load( infrastructure, state.as< State::HouseholdGameplayState >().getWorldCache() );

		generateWallRig();
		generateFloorRig();

		auto& worldRenderer = state.as< State::HouseholdGameplayState >().getWorldRenderer();

		worldRenderer.loadDirect( "__floorrig", floorModel );
		worldRenderer.loadDirect( "__wallrig", wallModel );

		worldRenderer.placeObject( "__floorrig", {} );
		worldRenderer.placeObject( "__wallrig", {} );

		generateRooms();
	}

	void InfrastructureManager::generateWallRig() {
		Graphics::Vector::Renderer vectorRenderer( state.as< State::HouseholdGameplayState >().getApplication().getDisplayDevice() );

		Graphics::SceneGraph::ModelLoader::WallModelLoader wallModelLoader(
			model.getLevels(),
			vectorRenderer,
			state.as< State::HouseholdGameplayState >().getShaderManager()
		);
		wallModel = wallModelLoader.get();
	}

	void InfrastructureManager::generateFloorRig() {
		Graphics::SceneGraph::ModelLoader::FloorModelLoader floorModelLoader( model.getLevels(), state.as< State::HouseholdGameplayState >().getShaderManager() );
		floorModel = floorModelLoader.get();
	}

	std::vector< glm::vec2 > InfrastructureManager::generateRoomNodes( const Tools::Sector& sector, const glm::uvec2& dimensions ) {
		std::vector< glm::vec2 > result;

		for( const Tools::SectorDiscoveryNode* node : sector ) {
			// Add origin-transformed node to room
			result.emplace_back( correctByOrigin( node->position, glm::vec2{ -( dimensions.x * 0.5f ), dimensions.y * 0.5f } ) );
		}

		// Determine winding direction
		int size = result.size();
		int target = -1;
		glm::vec2 last{ std::numeric_limits< float >::max(), std::numeric_limits< float >::lowest() };
		for( int i = 0; i != size; i++ ) {
			if( result[ i ].y < last.y ) {
				last = result[ i ];
				target = i;
			} else if( result[ i ].y == last.y && result[ i ].x > last.x ) {
				// Highest X wins
				last = result[ i ];
				target = i;
			}
		}

		if( target != -1 ) {
			const glm::vec2& b = result[ ( ( target - 1 ) + size ) % size ];
			const glm::vec2& a = result[ ( target + size ) % size ];
			const glm::vec2& c = result[ ( ( target + 1 ) + size ) % size ];

			// Clockwise if area is negative
			float sign = Tools::Utility::cross( a * b, a * c );
			if( sign > 0.0f ) {
				// Reverse
				std::vector< glm::vec2 > newResult;
				for( auto it = result.rbegin(); it != result.rend(); ++it ) {
					newResult.emplace_back( *it );
				}
				return newResult;
			}
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
		std::vector< std::vector< Models::Room > > roomLevels;

		for( const auto& level : model.getLevels() ) {
			std::vector< Models::Room > roomsForLevel;

			// Generate intersection map from existing intersections/crossovers
			Tools::Intersection::IntersectionList intersectionList = getIntersections( level.wallSegments );
			Tools::SectorIdentifier sectorIdentifier;
			for( const auto& computedSegment : intersectionList ) {
				sectorIdentifier.addEdge( computedSegment.start, computedSegment.end );
			}

			auto sectors = sectorIdentifier.getSectors();
			for( const Tools::Sector& sector : sectors ) {
				Models::Room room(
					{
						{ 0.5, 0.5, -0.1 },
						{ 0.1, 0.1, 0.1 },
						{ 0.3, 0.3, 0.3 },
						{ 0.1, 0.1, 0.1 }
					},
					generateRoomNodes( sector, level.dimensions )
				);


				roomsForLevel.emplace_back( std::move( room ) );
			}

			roomLevels.emplace_back( std::move( roomsForLevel ) );
		}

		lightmapManager.setRooms( roomLevels );
		lightmapManager.calculateLightmaps();
	}

	int InfrastructureManager::getCurrentLevel() const {
		return currentLevel;
	}


	void InfrastructureManager::setCurrentLevel( int currentLevel ) {
		this->currentLevel = currentLevel;
	}

	bool InfrastructureManager::update() {

	}

}