#include "scripting/luakit/dynamicusertype.hpp"

namespace BlueBear::Scripting::LuaKit {

  DynamicUsertype::DynamicUsertype( const std::map< std::string, sol::object >& types ) : types( types ) {}

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
