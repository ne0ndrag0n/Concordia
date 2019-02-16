#include "gameplay/infrastructuremanager.hpp"
#include "graphics/scenegraph/modelloader/floormodelloader.hpp"
#include "graphics/scenegraph/modelloader/wallmodelloader.hpp"
#include "graphics/vector/renderer.hpp"
#include "state/householdgameplaystate.hpp"
#include "application.hpp"
#include <geos/operation/polygonize/Polygonizer.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/LineString.h>
#include <geos/operation/polygonize/Polygonizer.h>
#include <geos/geom/Polygon.h>

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
	}

	void InfrastructureManager::generateRooms() {
		auto factory = geos::geom::GeometryFactory::create();
		auto csFactory = factory->getCoordinateSequenceFactory();

		// Add line segments to polygon generator
		float upper = 4.0f;
		auto levels = model.getLevels();
		for( auto& level : levels ) {
			// For each level, polygonise the line segments and create volume cubes
			auto polygonizer = geos::operation::polygonize::Polygonizer();

			for( auto& wallSegment : level.wallSegments ) {
				auto temp = csFactory->create( ( size_t ) 0, 0 );
				temp->add( geos::geom::Coordinate( wallSegment.start.x, wallSegment.start.y ) );
				temp->add( geos::geom::Coordinate( wallSegment.end.x, wallSegment.end.y ) );

				geos::geom::Geometry* lineSegment = factory->createLineString( temp );
				polygonizer.add( lineSegment );
			}

			auto polygons = polygonizer.getPolygons();
			for( geos::geom::Polygon* polygon : *polygons ) {
				auto coordinates = polygon->getCoordinates()->toVector();
				std::vector< glm::vec3 > room;

				for( const geos::geom::Coordinate& coordinate : *coordinates ) { room.emplace_back( coordinate.x, coordinate.y, upper - 4.0f ); }
				for( const geos::geom::Coordinate& coordinate : *coordinates ) { room.emplace_back( coordinate.x, coordinate.y, upper ); }

				rooms.emplace_back( std::move( room ) );
			}

			upper += 4.0f;
		}
	}

	bool InfrastructureManager::update() {

	}

}