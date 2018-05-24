#include "scripting/luakit/dynamicusertype.hpp"
#include "scripting/luakit/utility.hpp"
#include "tools/utility.hpp"

namespace BlueBear::Scripting::LuaKit {

  DynamicUsertype::DynamicUsertype( const std::map< std::string, sol::object >& typeTable ) {
    for( const std::pair< std::string, sol::object >& pair : typeTable ) {
      // Deep copy all tables
      if( pair.second.is< sol::table >() ) {
        types[ pair.first ] = Utility::copy( pair.second.as< sol::table >(), true );
      } else {
        types[ pair.first ] = pair.second;
      }
    }
  }

  Json::Value DynamicUsertype::save() {
    sol::object object = get( "save" );

    if( object.is< sol::function >() ) {
      std::function< std::string( DynamicUsertype& ) > saveFunction = Utility::cast< sol::function >( object );
      return Tools::Utility::stringToJson( saveFunction( *this ) );
    } else {
      // Nothing to serialise or return
      return {};
    }
  }

  void DynamicUsertype::load( const Json::Value& data ) {
    if( data != Json::Value::null ) {
      sol::object object = get( "load" );

      if( object.is< sol::function >() ) {
        Utility::cast< sol::function >( object )( *this, Tools::Utility::jsonToString( data ) );
      }
    }
  }

  sol::object DynamicUsertype::operator[]( const std::string& key ) {
    return get( key );
  }

  void DynamicUsertype::set( const std::string& key, sol::object object ) {
    types[ key ] = object;
  }

  sol::object DynamicUsertype::get( const std::string& key ) {
    auto it = types.find( key );

    if( it == types.end() ) {
      return sol::nil;
    }

    return it->second;
  }

  unsigned int DynamicUsertype::size() {
    return types.size();
  }

  void DynamicUsertype::submitLuaContributions( sol::table& types ) {
    types.new_usertype< DynamicUsertype >( "DynamicUsertype",
      sol::meta_function::index, &DynamicUsertype::get,
      sol::meta_function::new_index, &DynamicUsertype::set,
      sol::meta_function::length, &DynamicUsertype::size
    );
  }

}
