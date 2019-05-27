#include "gameplay/cutawaymanager.hpp"
#include "graphics/scenegraph/uniforms/level_uniform.hpp"
#include "graphics/scenegraph/model.hpp"
#include "graphics/camera.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"
#include <bezier.hpp>

#include "log.hpp"

namespace BlueBear::Gameplay {

	CutawayManager::CutawayManager( std::shared_ptr< Graphics::SceneGraph::Model > wallRig, std::vector< std::vector< Models::Room > >& rooms, const Graphics::Camera& camera )
	: rooms( rooms ), camera( camera ), numFrames( ConfigManager::getInstance().getIntValue( "fps_overview" ) * ( ConfigManager::getInstance().getIntValue( "wall_cutaway_animation_speed" ) / 100.0f ) ) {
		bakeAnimation();
		sortWallPanels( wallRig );
	}

	void CutawayManager::sortWallPanels( const std::shared_ptr< Graphics::SceneGraph::Model >& wallRig ) {
		sortedWalls.clear();

		for( const auto& level : wallRig->getChildren() ) {
			std::unordered_map< glm::vec2, Graphics::SceneGraph::Model* > sortedLevel;

			for( const auto& wallSegment : level->getChildren() ) {
				Graphics::SceneGraph::Uniforms::LevelUniform* levelUniform = ( Graphics::SceneGraph::Uniforms::LevelUniform* ) wallSegment->getUniform( "level" );
				sortedLevel.emplace( levelUniform->getPosition(), wallSegment.get() );
			}

			sortedWalls.emplace_back( std::move( sortedLevel ) );
		}
	}

	void CutawayManager::bakeAnimation() {
		animationBaked.clear();

		Bezier::Bezier< 3 > cubicBezier( { { 0.0f, 0.0f }, { 0.42f, 0.0f }, { 0.58f, 1.0f }, { 1.0f, 1.0f } } );
		for( int i = 0; i != numFrames; i++ ) {
			// bezierX is the position along the Bezier curve for time
			// The y-value of the curve is what will be used to position the wall
			float step = ( float ) i / ( float ) numFrames;
			Bezier::Point p = cubicBezier.valueAt( step );

			animationBaked.emplace_back( 0.0f, 0.0f, - ( 4.0f * p.y ) );
		}
	}

	void CutawayManager::updateAnimations() {
		for( auto it = activeAnimations.begin(); it != activeAnimations.end(); ) {
			Animation& animation = it->second;
			if( animation.step > numFrames || animation.step < 0 ) {
				it = activeAnimations.erase( it );
			} else {
				it->first->getLocalTransform().setPosition( animationBaked[ animation.step ] );
				animation.step += animation.direction;
				++it;
			}
		}
	}

	void CutawayManager::update( int currentLevel ) {
		// Walls from basement to currentLevel go fully up
		for( int i = 0; i != currentLevel; i++ ) {
			const auto& level = sortedWalls[ i ];
			for( const auto& wallSegmentEntry : level ) {
				if( !Tools::Utility::equalEpsilon( wallSegmentEntry.second->getLocalTransform().getPosition().z, 0.0f ) ) {
					Animation& animation = activeAnimations[ wallSegmentEntry.second ];
					animation.direction = -1;

					if( animation.step == 0 ) {
						animation.step = numFrames - 1;
					}
				}
			}
		}

		// Wall segments on the current level variously go up or down based on angle
		// Angle 135-225 from the camera angle means the walls go down
		auto& roomLevel = rooms[ currentLevel ];
		auto& wallLevel = sortedWalls[ currentLevel ];
		for( auto& room : roomLevel ) {
			for( int i = 0; i < room.getLineSegments().size(); i++ ) {
				const auto& wallLineSegment = room.getLineSegments()[ i ];
				// Get angle of wall segment relative to camera
				const glm::vec2& angleCartesian = room.getWallNormals()[ i ];
				float wallAngle = glm::atan( angleCartesian.y, angleCartesian.x );
				if( wallAngle < 0 ) {
					wallAngle = 360.0f - wallAngle;
				}
				float differential = std::abs( camera.getRotationAngle() - wallAngle );

				if( differential >= 135.0f && differential <= 225.0f ) {
					// All wall panels forming this segment get lowered
					// Walk the line segment one piece at a time, calling against sortedWalls for the level
					glm::vec2 direction = glm::normalize( wallLineSegment.second - wallLineSegment.first );
					glm::vec2 cursor = wallLineSegment.first;
					int steps = glm::distance( wallLineSegment.second, wallLineSegment.first );

					for( int i = 0; i != steps; i++ ) {
						Graphics::SceneGraph::Model* model = wallLevel[ cursor ];
						if( !model ) {
							Log::getInstance().error( "CutawayManager::update", "No model" );
							continue;
						}

						if( activeAnimations.find( model ) == activeAnimations.end() ) {
							activeAnimations[ model ] = Animation{ 0, 1 };
						}
					}
				}
			}
		}

		// Wall segments above currentLevel do not show
		for( int i = currentLevel + 1; i < sortedWalls.size(); i++ ) {
			const auto& level = sortedWalls[ i ];
			for( const auto& pair : level ) {
				pair.second->getLocalTransform().setPosition( { 0.0f, 0.0f, -4.0f } );
			}
		}
	}

}