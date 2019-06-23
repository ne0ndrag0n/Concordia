#include "models/world.hpp"
#include "eventmanager.hpp"
#include "scripting/luakit/utility.hpp"
#include "scripting/entitykit/entity.hpp"
#include "graphics/scenegraph/model.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include <functional>

namespace BlueBear::Models {

  World::World( Device::Display::Adapter::Component::WorldRenderer& worldRenderer ) : worldRenderer( worldRenderer ) {
    eventManager.LUA_STATE_READY.listen( this, std::bind( &World::submitLuaContributions, this, std::placeholders::_1 ) );
    Scripting::EntityKit::Entity::ENTITY_CLOSING.listen( this, [ & ]( std::shared_ptr< Scripting::EntityKit::Entity > entity ) {
      removeObject( entity );
    } );
  }

  World::~World() {
    eventManager.LUA_STATE_READY.stopListening( this );
    Scripting::EntityKit::Entity::ENTITY_CLOSING.stopListening( this );
  }

  void World::submitLuaContributions( sol::state& lua ) {
    sol::table types = lua[ "bluebear" ][ "util" ][ "types" ];
    sol::table world;
    if( lua[ "bluebear" ][ "world" ] == sol::nil ) {
      world = lua[ "bluebear" ][ "world" ] = lua.create_table();
    } else {
      world = lua[ "bluebear" ][ "world" ];
    }

    world.set_function( "place_object", [ & ]( Scripting::EntityKit::Entity& self, const std::string& objectId, sol::table classes ) {
      std::set< std::string > classesSet;
      for( std::pair< sol::object, sol::object >& pair : classes ) {
        classesSet.insert( Scripting::LuaKit::Utility::cast< std::string >( pair.second ) );
      }

      std::shared_ptr< Graphics::SceneGraph::Model > model = worldRenderer.placeObject( objectId, classesSet );

      objects.push_back( {
        self.shared_from_this(),
        model
      } );

      return model;
    } );

    world.set_function( "remove_object", [ & ]( Graphics::SceneGraph::Model& self ) {
      removeObject( self.shared_from_this() );
    } );
  }

  void World::removeObject( std::shared_ptr< Scripting::EntityKit::Entity > comparison ) {
    for( auto it = objects.begin(); it != objects.end(); ) {
      if( it->associatedEntity == comparison ) {
        worldRenderer.removeObject( it->model );
        it = objects.erase( it );
      } else {
        ++it;
      }
    }
  }

  void World::removeObject( std::shared_ptr< Graphics::SceneGraph::Model > comparison ) {
    for( auto it = objects.begin(); it != objects.end(); ) {
      if( it->model == comparison ) {
        worldRenderer.removeObject( it->model );
        it = objects.erase( it );
      } else {
        ++it;
      }
    }
  }

  Json::Value World::save() {
    return Json::Value::null;
  }

  void World::load( const Json::Value& data ) {

  }

}
