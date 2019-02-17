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

	InfrastructureManager::RoomBuilderMap InfrastructureManager::prepareRoomBuilderMap( const glm::ivec2& dimensions ) {
		RoomBuilderMap map;

		map.resize( dimensions.y );
		for( auto& row : map ) {
			row.resize( dimensions.x );
		}

		return map;
	}

	void InfrastructureManager::addLineSegment( glm::ivec2 start, glm::ivec2 end, RoomBuilderMap& map ) {
		glm::ivec2 direction = glm::normalize( glm::vec2( end ) - glm::vec2( start ) );

		switch( direction.x ) {
			case -1: {
				switch( direction.y ) {
					case -1: {
						direction = { 1, 1 };
						glm::ivec2 oldStart = start;
						start = end;
						end = oldStart;
						break;
					}
					case 0: {
						direction = { 1, 0 };
						glm::ivec2 oldStart = start;
						start = end;
						end = oldStart;
						break;
					}
					case 1: {
						direction = { 1, -1 };
						glm::ivec2 oldStart = start;
						start = end;
						end = oldStart;
						break;
					}
				}
				break;
			}
			case 0: {
				switch( direction.y ) {
					case -1: {
						direction = { 0, 1 };
						glm::ivec2 oldStart = start;
						start = end;
						end = oldStart;
						break;
					}
				}
				break;
			}
		}

		int distance = std::abs( glm::distance( glm::vec2( start ), glm::vec2( end ) ) );
		glm::ivec2 cursor = ( direction == glm::ivec2{ 1, -1 } ) ? start : start - glm::ivec2{ 0, 1 };

		// doing this shit again
		for( int i = 0; i != distance; i++ ) {
			RoomBuilderCell& cell = map.at( cursor.y ).at( cursor.x );

			switch( direction.x ) {
				case 1: {
					switch( direction.y ) {
						case -1: {
							cell.reverseDiagonalWall = true;
							break;
						}
						case 0: {
							cell.upperWall = true;
							break;
						}
						case 1: {
							cell.diagonalWall = true;
							break;
						}
					}
					break;
				}
				case 0: {
					switch( direction.y ) {
						case 1: {
							cell.leftWall = true;
							break;
						}
					}
					break;
				}
			}

			cursor += direction;
		}
	}

	void InfrastructureManager::generateRooms() {

		float upper = 4.0f;
		for( const auto& level : model.getLevels() ) {
			RoomBuilderMap levelMap = prepareRoomBuilderMap( level.dimensions );

			// Draw lines onto level map
			for( const auto& wallSegment : level.wallSegments ) {
				try {
					addLineSegment( wallSegment.start, wallSegment.end, levelMap );
				} catch( std::exception& e ) {
					Log::getInstance().error( "InfrastructureManager::generateRooms", "Malformed wall definition in lot; cannot build room cells" );
					return;
				}
			}

			// Fill in -1 items
			glm::ivec2 cursor{ 0, 0 };
			for( ; cursor.y != level.dimensions.y; cursor.y++ ) {
				for( ; cursor.x != level.dimensions.x; cursor.x++ ) {
					// TODO
				}
			}
		}

	}

	bool InfrastructureManager::update() {

	}

}