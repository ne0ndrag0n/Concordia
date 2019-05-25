#include "gameplay/cutawaymanager.hpp"
#include "configmanager.hpp"
#include "graphics/scenegraph/model.hpp"
#include <bezier.hpp>
#include "log.hpp"

namespace BlueBear::Gameplay {

	CutawayManager::CutawayManager( std::shared_ptr< Graphics::SceneGraph::Model > wallRig, const std::vector< std::vector< Models::Room > >& rooms, const Graphics::Camera& camera )
	: wallRig( wallRig ), rooms( rooms ), camera( camera ) {
		bakeAnimation();
	}

	std::vector< Graphics::SceneGraph::Model* > CutawayManager::queryFor( const std::function< bool(const std::shared_ptr< Graphics::SceneGraph::Model >&) >& predicate, std::optional< int > level ) {
		std::vector< Graphics::SceneGraph::Model* > result;

		if( level ) {
			const auto& wallLevel = wallRig->getChildren().at( *level );

			for( const auto& item : wallLevel->getChildren() ) {
				if( predicate( item ) ) {
					result.emplace_back( item.get() );
				}
			}
		} else {
			for( const auto& level : wallRig->getChildren() ) {
				for( const auto& item : level->getChildren() ) {
					if( predicate( item ) ) {
						result.emplace_back( item.get() );
					}
				}
			}
		}

		return result;
	}

	void CutawayManager::bakeAnimation() {
		animationBaked.clear();

		static int fps = ConfigManager::getInstance().getIntValue( "fps_overview" );
		static float secs = ConfigManager::getInstance().getIntValue( "wall_cutaway_animation_speed" ) / 100.0f;

		int numFrames = fps * secs;
		Bezier::Bezier< 3 > cubicBezier( { { 0.0f, 0.0f }, { 0.42f, 0.0f }, { 0.58f, 1.0f }, { 1.0f, 1.0f } } );

		for( int i = 0; i != numFrames; i++ ) {
			// bezierX is the position along the Bezier curve for time
			// The y-value of the curve is what will be used to position the wall
			float step = ( float ) i / ( float ) numFrames;
			Bezier::Point p = cubicBezier.valueAt( step );

			animationBaked.emplace_back( 0.0f, 0.0f, 4.0f * p.y );
		}
	}

	void CutawayManager::update( int currentLevel ) {
		// Current level gets the cutaway
		// Levels above do not display, levels below are walls up

	}

}