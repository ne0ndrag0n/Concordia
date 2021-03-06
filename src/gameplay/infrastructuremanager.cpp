#include "gameplay/infrastructuremanager.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "graphics/scenegraph/uniforms/level_uniform.hpp"
#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/vector/renderer.hpp"
#include "geometry/methods.hpp"
#include "state/householdgameplaystate.hpp"
#include "tools/utility.hpp"
#include "application.hpp"
#include "configmanager.hpp"
#include <bezier.hpp>
#include <unordered_set>
#include <glm/gtx/string_cast.hpp>

namespace BlueBear::Gameplay {

	static glm::vec2 correctByOrigin( const glm::ivec2& value, const glm::vec2& origin ) {
		return { origin.x + value.x, origin.y - value.y };
	}

	static std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > findByCell(
		const std::vector< std::shared_ptr< Graphics::SceneGraph::Model > >& list,
		const glm::vec2& direction,
		const glm::vec2& cell,
		int currentLevel
	) {
		std::vector< std::shared_ptr< Graphics::SceneGraph::Model > > result;
		std::string match;

		if( direction == glm::vec2{ 1.0f, 0.0f } ) {
			match = "__horizontal";
		} else if ( direction == glm::vec2{ 0.0f, -1.0f } ) {
			match = "__vertical";
		} else if ( direction == glm::vec2{ 1.0f, -1.0f } ) {
			match = "__diagonal";
		} else if ( direction == glm::vec2{ 1.0f, 1.0f } ) {
			match = "__reverseDiagonal";
		}

		std::copy_if( list.begin(), list.end(), std::back_inserter( result ), [ & ]( const std::shared_ptr< Graphics::SceneGraph::Model >& model ) {
			Graphics::SceneGraph::Uniforms::LevelUniform* uniform = ( Graphics::SceneGraph::Uniforms::LevelUniform* ) model->getUniform( "level" );

			return model->getId() == match && uniform->getLevel() == currentLevel && uniform->getPosition() == cell;
		} );

		return result;
	}

	InfrastructureManager::InfrastructureManager( State::State& state ) : State::Substate( state ) {}

	InfrastructureManager::~InfrastructureManager() {
		state.as< State::HouseholdGameplayState >().getWorldRenderer().getCamera().CAMERA_ROTATED.stopListening( this );
	}

	Json::Value InfrastructureManager::save() {
		return Json::Value::null;
	}

	void InfrastructureManager::load( const Json::Value& infrastructure ) {
		model.load( infrastructure, state.as< State::HouseholdGameplayState >().getWorldCache() );

		switch( Tools::Utility::hash( infrastructure[ "wallMode" ].asCString() ) ) {
			default:
				Log::getInstance().warn( "InfrastructureManager::load", "Invalid value given for infrastructure.wallMode given: " + infrastructure[ "wallMode" ].asString() + ", defaulting to \"cutaway\"." );
				[[fallthrough]];
			case Tools::Utility::hash( "cutaway" ): {
				wallMode = WallMode::WALLS_CUT;
				break;
			}
			case Tools::Utility::hash( "down" ): {
				wallMode = WallMode::WALLS_DOWN;
				break;
			}
			case Tools::Utility::hash( "up" ): {
				wallMode = WallMode::WALLS_UP;
				break;
			}
		}

		generateWallRig();
		generateFloorRig();

		auto& worldRenderer = state.as< State::HouseholdGameplayState >().getWorldRenderer();

		worldRenderer.loadDirect( "__floorrig", floorModel );
		worldRenderer.loadDirect( "__wallrig", wallModel );

		worldRenderer.placeObject( "__floorrig", {} );
		worldRenderer.placeObject( "__wallrig", {} );

		generateRooms();

		hideUpperLevels();
		updateWallMode();

		const glm::ivec2& dimensions = model.getLevels()[ currentLevel ].dimensions;
		grid.setParams( { -( dimensions.x * 0.5f ), -( dimensions.y * 0.5f ) }, { dimensions.x, dimensions.y } );

		state.as< State::HouseholdGameplayState >().getWorldRenderer().getCamera().CAMERA_ROTATED.listen( this, std::bind( &InfrastructureManager::updateWallMode, this ) );
	}

	void InfrastructureManager::generateWallRig() {
		Graphics::Vector::Renderer vectorRenderer( state.as< State::HouseholdGameplayState >().getApplication().getDisplayDevice() );

		Graphics::SceneGraph::ModelLoader::WallModelLoader wallModelLoader(
			model.getLevels(),
			vectorRenderer,
			state.as< State::HouseholdGameplayState >().getShaderManager()
		);
		wallModel = wallModelLoader.get();

		activeWallAnims.clear();
	}

	void InfrastructureManager::generateFloorRig() {
		Graphics::SceneGraph::ModelLoader::FloorModelLoader floorModelLoader( model.getLevels(), state.as< State::HouseholdGameplayState >().getShaderManager() );
		floorModel = floorModelLoader.get();
	}

	/**
	 * Cycle through and play animations in activeWallAnims
	 */
	void InfrastructureManager::updateAnimations() {
		static Bezier::Bezier< 3 > cubicBezier( { { 0.0f, 0.0f }, { 0.23f, 1.00f }, { 0.32f, 1.00f }, { 1.0f, 1.0f } } );

		for( auto it = activeWallAnims.begin(); it != activeWallAnims.end(); ) {
			auto& pair = *it;

			if( pair.second.currentFrame <= pair.second.maxFrames ) {
				float step = ( float ) pair.second.currentFrame / ( float ) pair.second.maxFrames;
				float span = pair.second.destination - pair.second.source;
				pair.first->getLocalTransform().setPosition( { 0.0f, 0.0f, pair.second.source + ( span * cubicBezier.valueAt( step ).y ) } );
				pair.second.currentFrame++;

				++it;
			} else {
				it = activeWallAnims.erase( it );
			}
		}
	}

	void InfrastructureManager::enqueueAnimation( Graphics::SceneGraph::Model* key, const InfrastructureManager::Animation&& animation ) {
		float source = key->getLocalTransform().getPosition().z;

		auto it = activeWallAnims.find( key );
		if( it == activeWallAnims.end() ) {
			activeWallAnims[ key ] = animation;
			activeWallAnims[ key ].source = source;
		} else {
			it->second.destination = animation.destination;
			it->second.source = source;
		}
	}

	void InfrastructureManager::updateWallMode() {
		// TODO!
		switch( wallMode ) {
			case WallMode::WALLS_DOWN:
				setWallsDown();
				break;
			default:
				[[fallthrough]];
			case WallMode::WALLS_CUT:
				setWallCutaways();
				break;
			case WallMode::WALLS_UP:
				setWallsUp();
				break;
		}
	}

	/**
	 * Hide upper levels by sinking them all the way into their floor - the shader will cut them off using the discard functionality
	 *
	 * Event to be run:
	 * - When level is modified using setCurrentLevel
	 */
	void InfrastructureManager::hideUpperLevels() {
		static int numFrames = ConfigManager::getInstance().getIntValue( "fps_overview" ) * ( ( float ) ConfigManager::getInstance().getIntValue( "wall_cutaway_animation_speed" ) / 1000.0f );
		std::shared_ptr< Graphics::SceneGraph::Model > wallRigInstance = state.as< State::HouseholdGameplayState >().getWorldRenderer().findObjectsByType( "__wallrig" )[ 0 ];
		const auto& levels = wallRigInstance->getChildren();

		for( size_t i = currentLevel + 1; i < levels.size(); i++ ) {
			const auto& level = levels[ i ]->getChildren();
			for( const auto& wall : level ) {
				enqueueAnimation( wall.get(), { 0, numFrames, -4.0f } );
			}
		}
	}

	/**
	 * Sink wall segments 90% of the way into the floor if their face vector falls within 135 and 225 degrees of the camera vector
	 *
	 * Event to be run:
	 * - When level is modified using setCurrentLevel
	 * - When camera angle changes
	 */
	void InfrastructureManager::setWallCutaways() {
		static int numFrames = ConfigManager::getInstance().getIntValue( "fps_overview" ) * ( ( float ) ConfigManager::getInstance().getIntValue( "wall_cutaway_animation_speed" ) / 1000.0f );
		std::shared_ptr< Graphics::SceneGraph::Model > wallRigInstance = state.as< State::HouseholdGameplayState >().getWorldRenderer().findObjectsByType( "__wallrig" )[ 0 ];
		const auto& camera = state.as< State::HouseholdGameplayState >().getWorldRenderer().getCamera();
		auto& roomLevel = rooms[ currentLevel ];
		auto& segments = wallRigInstance->getChildren()[ currentLevel ]->getChildren();

		std::unordered_set< Graphics::SceneGraph::Model* > selectedSegments;

		// For each room on the current level, walk its walls in the winding direction and set the walls to animate down to -z3.75
		// if the angle of the wall is between 135 and 225 relative to the camera.
		for( auto& room : roomLevel ) {
			const auto& normals = room.getWallNormals();
			for( const auto& wall : normals ) {
				float angle = Tools::Utility::positiveAngle(
					Tools::Utility::positiveAngle( glm::degrees( glm::atan( wall.perpendicular.y, wall.perpendicular.x ) ) ) +
					camera.getRotationAngle()
				);

				Log::getInstance().debug(
					"InfrastructureManager::setWallCutaways",
					"segment from " + glm::to_string( wall.segment.first ) + " to " + glm::to_string( wall.segment.second ) +
					" direction: " + glm::to_string( wall.direction ) + " perpendicular: " + glm::to_string( wall.perpendicular ) + " // angle: " + std::to_string( angle )
				);

				if( angle >= 225.0f && angle <= 315.0f ) {
					glm::vec2 start;
					glm::vec2 finish;
					glm::vec2 direction;
					if( wall.direction == glm::vec2{ -1.0f, 0.0f } || wall.direction == glm::vec2{ 0.0f, 1.0f } || wall.direction == glm::vec2{ -1.0f, -1.0f } || wall.direction == glm::vec2{ -1.0f, 1.0f } ) {
						start = wall.segment.second;
						finish = wall.segment.first;
						direction = glm::vec2{ -wall.direction.x, -wall.direction.y };
					} else {
						start = wall.segment.first;
						finish = wall.segment.second;
						direction = wall.direction;
					}

					int totalSteps = std::abs( glm::distance( finish, start ) );
					glm::vec2 cursor = start;
					for( int i = 0; i < totalSteps; i++ ) {
						auto matchingSides = findByCell( segments, direction, cursor, currentLevel );
						for( const auto& match : matchingSides ) {
							selectedSegments.insert( match.get() );
							enqueueAnimation( match.get(), { 0, numFrames, -3.75f } );
						}

						cursor += direction;
					}
				}
			}
		}

		// For all wall panels that were not selected in the process above, and are lower than their default configuration
		// Set an animation to return them back to z+0
		for( const auto& segment : segments ) {
			if( selectedSegments.find( segment.get() ) == selectedSegments.end() ) {
				auto& segmentTransform = segment->getLocalTransform();
				if( segmentTransform.getPosition() != glm::vec3{ 0.0f, 0.0f, 0.0f } ) {
					enqueueAnimation( segment.get(), { 0, numFrames, 0.0f } );
				} else {
					// Element has been queued for animation previously, but it has not yet moved,
					// and has not been selected for this run. Remove the animation in its entirety.
					activeWallAnims.erase( segment.get() );
				}
			}
		}
	}

	void InfrastructureManager::setWallsDown() {
		static int numFrames = ConfigManager::getInstance().getIntValue( "fps_overview" ) * ( ( float ) ConfigManager::getInstance().getIntValue( "wall_cutaway_animation_speed" ) / 1000.0f );
		std::shared_ptr< Graphics::SceneGraph::Model > wallRigInstance = state.as< State::HouseholdGameplayState >().getWorldRenderer().findObjectsByType( "__wallrig" )[ 0 ];
		auto& segments = wallRigInstance->getChildren()[ currentLevel ]->getChildren();

		for( const auto& segment : segments ) {
			enqueueAnimation( segment.get(), { 0, numFrames, -3.75f } );
		}
	}

	void InfrastructureManager::setWallsUp() {
		static int numFrames = ConfigManager::getInstance().getIntValue( "fps_overview" ) * ( ( float ) ConfigManager::getInstance().getIntValue( "wall_cutaway_animation_speed" ) / 1000.0f );
		std::shared_ptr< Graphics::SceneGraph::Model > wallRigInstance = state.as< State::HouseholdGameplayState >().getWorldRenderer().findObjectsByType( "__wallrig" )[ 0 ];
		auto& segments = wallRigInstance->getChildren()[ currentLevel ]->getChildren();

		for( const auto& segment : segments ) {
			enqueueAnimation( segment.get(), { 0, numFrames, 0.0f } );
		}
	}

	std::vector< glm::vec2 > InfrastructureManager::generateRoomNodes( const Tools::Sector& sector, const glm::uvec2& dimensions ) {
		std::vector< glm::vec2 > result;

		for( const Tools::SectorDiscoveryNode* node : sector ) {
			// Add origin-transformed node to room
			result.emplace_back( correctByOrigin( node->position, glm::vec2{ -( dimensions.x * 0.5f ), dimensions.y * 0.5f } ) );
		}

		// TODO: Geometry methods to determine winding direction of polygon
		if( Geometry::polygonClockwise( result ) ) {
			Log::getInstance().debug( "InfrastructureManager::generateRoomNodes", "Clockwise" );
		} else {
			Log::getInstance().debug( "InfrastructureManager::generateRoomNodes", "Counterclockwise - reversing" );
			return Geometry::polygonReverse( result );
		}

		return result;
	}

	Tools::Intersection::IntersectionList InfrastructureManager::getIntersections( const std::vector< Models::WallSegment >& wallSegments, const glm::ivec2& dimensions ) {
		Tools::Intersection::IntersectionList intersectionList;
		for( const auto& wallSegment : wallSegments ) {
			intersectionList.emplace_back( Tools::Intersection::IntersectionLineSegment{ wallSegment.start, wallSegment.end } );
		}

		return Tools::Intersection::generateIntersectionalList( std::move( intersectionList ), dimensions );
	}

	/**
	 * Refactoring TODO
	 */
	void InfrastructureManager::generateRooms() {
		rooms.clear();

		for( const auto& level : model.getLevels() ) {
			std::vector< Models::Room > roomsForLevel;

			// Generate intersection map from existing intersections/crossovers
			Tools::Intersection::IntersectionList intersectionList = getIntersections( level.wallSegments, level.dimensions + glm::uvec2{ 1, 1 } );
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

			rooms.emplace_back( std::move( roomsForLevel ) );
		}

		lightmapManager.setRooms( rooms );
		lightmapManager.calculateLightmaps();
	}

	int InfrastructureManager::getCurrentLevel() const {
		return currentLevel;
	}


	void InfrastructureManager::setCurrentLevel( int currentLevel ) {
		this->currentLevel = currentLevel;
	}

	void InfrastructureManager::setWallMode( WallMode mode ) {
		wallMode = mode;
		updateWallMode();
	}

	bool InfrastructureManager::update() {
		updateAnimations();
		return true;
	}

}