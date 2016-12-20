#include "scripting/luakit/serializer.hpp"
#include "scripting/serializableinstance.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <jsoncpp/json/json.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <regex>

namespace BlueBear {
  namespace Scripting {
    namespace LuaKit {

      const std::string Serializer::TYPE_TABLE = "table";
      const std::string Serializer::TYPE_FUNCTION = "function";
      const std::string Serializer::TYPE_SFUNCTION = "sfunction";
      const std::string Serializer::TYPE_REF = "ref";
      const std::string Serializer::TYPE_CLASSID = "class";
      const std::string Serializer::TYPE_ENVREF = "envref";

      const std::string Serializer::ENVREF_MODE_BBGLOBAL = "bluebear";
      const std::string Serializer::ENVREF_MODE_G = "_G";

      Serializer::Serializer( lua_State* L ) : L( L ) {}

      /**
       * Using the Engine-tracked index of system.entity.base objects as a starting point, save the current state of the Lua world.
       */
      Json::Value Serializer::saveWorld( std::vector< SerializableInstance >& objects ) {
        world = Json::Value( Json::objectValue );

        // STOP the garbage collector so pointer references remain intact as we operate
        // Lua currently doesn't move items around as part of garbage collection (I think) but relying
        // on it is still undefined behaviour
        lua_gc( L, LUA_GCSTOP, 0 );

        // Build all required substitutions (classes, the bluebear global)
        buildSubstitutions();

        for( SerializableInstance& instance : objects ) {
          // table
          lua_rawgeti( L, LUA_REGISTRYINDEX, instance.luaVMInstance );

          // EMPTY
          createTableOnMasterList();
        }

        // Use this regex when saving to a file, it fixes an annoying thing with JsonCpp where the "\u" is replaced by "\\u"
        Log::getInstance().debug( "LuaKit::Serializer::saveWorld", "\n" + std::regex_replace( world.toStyledString(), std::regex( R"(\\\\u)" ), "\\u" ) );

        // Restart the garbage collector, and give it a good cycle
        lua_gc( L, LUA_GCRESTART, 0 );
        lua_gc( L, LUA_GCCOLLECT, 0 );

        return world;
      }

      /**
       * Create a table on the master list
       *
       * STACK ARGS: table
       * RETURNS: none
       */
      void Serializer::createTableOnMasterList() {

        // As soon as a table is found, go ahead and throw it on the pile
        std::string this_table( Tools::Utility::pointerToString( lua_topointer( L, -1 ) ) );
        Json::Value& parentItem = world[ this_table ] = Json::Value( Json::objectValue );

        parentItem[ "type" ] = Serializer::TYPE_TABLE;
        Json::Value& item = parentItem[ "entries" ] = Json::Value( Json::arrayValue );

        lua_pushnil( L ); // nil table

        // Iterate over the contents of this table
        while( lua_next( L, -2 ) != 0 ) { // value key table

          // { "key": <objtype>, "value": <objtype> }
          Json::Value pair = Json::Value( Json::objectValue );

          inferType( pair, "value" ); // key table

          lua_pushvalue( L, -1 ); // key key table
          inferType( pair, "key" ); // key table

          item.append( pair );
        } // table


        lua_pop( L, 1 ); // EMPTY
      }

      /**
       * Create a function on the master list
       *
       * STACK ARGS: function
       * RETURNS: none
       */
       void Serializer::createFunctionOnMasterList() {

         std::string thisTable( Tools::Utility::pointerToString( lua_topointer( L, -1 ) ) );

         Json::Value func( Json::objectValue );
         func[ "type" ] = Serializer::TYPE_FUNCTION;

         // Serialize the text/function body of a closure
         lua_getglobal( L, "string" ); // string function
         lua_pushstring( L, "dump" ); // "dump" string function
         lua_gettable( L, -2 ); // <string.dump> string function
         lua_pushvalue( L, -3 ); // function <string.dump> string function

         if( lua_pcall( L, 1, 1, 0 ) == 0 ) { // "serialized" string function
           int serializedLength = lua_rawlen( L, -1 );
           const char* serialized = lua_tostring( L, -1 );

           std::stringstream stringBuilder;
           for( int i = 0; i != serializedLength; i++ ) {
             char c = serialized[ i ];
             unsigned char uc = ( unsigned char ) c;
             int ic = ( int )uc;

             if( ic < 32 || ic > 126 ) {
               // Spit out '\uxxxx'
               stringBuilder << "\\u" << std::setfill( '0' ) << std::setw( 4 ) << std::hex << ic;
             } else {
               // Spit out the literal, printable character
               stringBuilder << c;
             }
           }

           func[ "body" ] = stringBuilder.str();
           func[ "len" ] = serializedLength;

           lua_pop( L, 2 ); // function

           // Now serialize the associated upvalues
           addUpvalues( func );

           world[ thisTable ] = func;
         } else { // "error" string function
            Log::getInstance().error( "LuaKit::Serializer::createFunctionOnMasterList", "Could not serialize function: " + std::string( lua_tostring( L, -1 ) ) );
            lua_pop( L, 2 ); // function
         }

         lua_pop( L, 1 ); // EMPTY
       }

      /**
       * Infer the type, load references if necessary, and place it into pair[field]
       *
       * STACK ARGS: (any lua type to infer)
       * RETURNS: EMPTY
       */
      void Serializer::inferType( Json::Value& pair, const std::string& field ) {

        // Get type
        const char* type = lua_typename( L, lua_type( L, -1 ) );

        // Different action based on type
        // make sure our hash function is good here
        switch( Tools::Utility::hash( type ) ) {
          case Tools::Utility::hash( "string" ):
            lua_pushvalue( L, -1 ); // string string
            pair[ field ] = lua_tostring( L, -1 );
            lua_pop( L, 1 ); // string
            break;
          case Tools::Utility::hash( "number" ):
            pair[ field ] = lua_tonumber( L, -1 );
            break;
          case Tools::Utility::hash( "boolean" ):
            pair[ field ] = lua_toboolean( L, -1 ) ? true : false;
            break;
          case Tools::Utility::hash( "table" ):
            {

              // If this table was already found in world, then simply create the reference. Else, expose table to world, then create the reference.
              std::string worldPointer = Tools::Utility::pointerToString( lua_topointer( L, -1 ) );
              auto substitution = substitutions.find( worldPointer );

              if( substitution == substitutions.end() ) {
                // This is a plain old table that does not require any special action ("substitution")

                if( !world.isMember( worldPointer ) ) {
                  // Need to create the table
                  // Copy the stack value as createTableOnMasterList will remove it
                  lua_pushvalue( L, -1 ); // table table
                  createTableOnMasterList(); // table
                }

                pair[ field ] = createReference();
              } else {
                // This involves a substitution
                pair[ field ] = substitution->second();
              }

            }
            break;
          case Tools::Utility::hash( "function" ):
            {
              std::string worldPointer = Tools::Utility::pointerToString( lua_topointer( L, -1 ) );
              auto substitution = substitutions.find( worldPointer );

              if( substitution == substitutions.end() ) {
                // No substitution required for this function.

                // Now determine if we need to create a reference to this function, or we need to create an "sfunction", by peeping at the upvalues.
                if( canCreateSfunction() ) {
                  // This sfunction can be created
                  Json::Value sfunction( Json::objectValue );
                  sfunction[ "type" ] = Serializer::TYPE_SFUNCTION;

                  // Build everything we need to create an sfunction

                  getUpvalueByName( "__derived_class" ); // "__derived_class" function
                  sfunction[ "class" ] = lua_tostring( L, -1 );
                  lua_pop( L, 1 ); // function

                  getUpvalueByName( "__derived_func" ); // "__derived_func" function
                  sfunction[ "method" ] = lua_tostring( L, -1 );
                  lua_pop( L, 1 ); // function

                  getUpvalueByName( "context" ); // context function
                  inferType( sfunction, "context" ); // function

                  Json::Value& args = sfunction[ "args" ] = Json::Value( Json::arrayValue );
                  getUpvalueByName( "args" ); // args function
                  lua_pushnil( L ); // nil args function
                  while( lua_next( L, -2 ) != 0 ) { // arg index args function
                    Json::Value container = Json::Value( Json::objectValue );
                    inferType( container, "value" ); // index args function
                    args.append( container[ "value" ] );
                  } // args function

                  lua_pop( L, 1 ); // function

                  pair[ field ] = sfunction;
                } else {
                  // We need to serialize code directly to the file
                  if( !world.isMember( worldPointer ) ) {
                    // Need to create the function
                    lua_pushvalue( L, -1 ); // function function
                    createFunctionOnMasterList(); // function
                  }

                  pair[ field ] = createReference();

                }

              } else {
                // This involves a substitution
                pair[ field ] = substitution->second();
              }
            }
            break;
          default:
            Log::getInstance().warn( "LuaKit::Serializer::inferType", "Invalid type: " + std::string( type ) + ", substituting null." );
          case Tools::Utility::hash( "nil" ):
              pair[ field ] = Json::Value::null;
        }

        lua_pop( L, 1 ); // EMPTY
      }

      /**
       * Adds the function's upvalues to the function-type JSON object defined in funcType (on the "upvalues" field).
       *
       * STACK ARGS: function
       * (Stack is unmodified after call)
       */
      void Serializer::addUpvalues( Json::Value& funcType ) {
        Json::Value& upvalues = funcType[ "upvalues" ] = Json::Value( Json::arrayValue );

        for( int i = 1; const char* upvalueId = lua_getupvalue( L, -1, i ); i++ ) { // upvalue function
          Json::Value upvalue = Json::Value( Json::objectValue );

          upvalue[ "key" ] = upvalueId;
          inferType( upvalue, "value" ); // function

          upvalues.append( upvalue );
        }
      }

      /**
       * Determine if the closure is an sfunction (serialisable function). This condition is met if there are two upvalues present, __derived_class and __derived_func,
       * each with string values. If this is an sfunction, we can save it in a safer manner that doesn't require us to write code to the lot file.
       *
       * STACK ARGS: function
       * (Stack is unmodified after call)
       */
      bool Serializer::canCreateSfunction() {
        bool derivedClassIsString = false;
        bool derivedFuncIsString = false;

        for( int i = 1; const char* upvalueId = lua_getupvalue( L, -1, i ); i++ ) { // upvalue function
          std::string key( upvalueId );

          derivedClassIsString = derivedClassIsString || ( key == "__derived_class" && lua_isstring( L, -1 ) );
          derivedFuncIsString = derivedFuncIsString || ( key == "__derived_func" && lua_isstring( L, -1 ) );

          lua_pop( L, 1 ); // function
        }

        return derivedClassIsString && derivedFuncIsString;
      }

      /**
       * Gets an upvalue by name. This will push the upvalue if it exists, nil otherwise.
       * O(n) method, use with care.
       *
       * STACK ARGS: function
       * RETURNS: (upvalue || nil) function
       */
      void Serializer::getUpvalueByName( const std::string& name ) {

        for( int i = 1; const char* upvalueId = lua_getupvalue( L, -1, i ); i++ ) { // upvalue function
          std::string key( upvalueId );

          if( key == name ) {
            return;
          }

          lua_pop( L, 1 ); // function
        }

        lua_pushnil( L ); // nil function
      }

      /**
       * Return a JSON value representing a reference to a floating world object. This function is only defined for the types specified below.
       *
       * STACK ARGS: table OR function
       * (Stack is unmodified after call)
       */
      Json::Value Serializer::createReference() {
        Json::Value val( Json::objectValue );

        val[ "type" ] = Serializer::TYPE_REF;
        val[ "ptr" ] = Tools::Utility::pointerToString( lua_topointer( L, -1 ) );

        return val;
      }

      /**
       * Return a JSON file representing a reference to a class.
       *
       * STACK ARGS: table
       * (Stack is unmodified after call)
       */
      Json::Value Serializer::createClassReference() {
        Json::Value val( Json::objectValue );

        val[ "type" ] = Serializer::TYPE_CLASSID;
        lua_pushstring( L, "name" ); // "name" table
        lua_gettable( L, -2 ); // "class.name" table
        // copy so lua_next doesn't get fucked up
        lua_pushvalue( L, -1 ); // "class.name" "class.name" table
        val[ "id" ] = lua_tostring( L, -1 );

        lua_pop( L, 2 ); // table

        return val;
      }

      /**
       * Create a reference to the "bluebear" global
       *
       * STACK ARGS: table
       * (Stack is unmodified after call)
       */
      Json::Value Serializer::createConcordiaNSReference() {
        Json::Value val( Json::objectValue );

        val[ "type" ] = Serializer::TYPE_ENVREF;
        val[ "object" ] = Serializer::ENVREF_MODE_BBGLOBAL;

        return val;
      }

      /**
       * Create a reference to _G, the global table
       *
       * STACK ARGS: table
       * (Stack is unmodified after call)
       */
      Json::Value Serializer::createGReference() {
        Json::Value val( Json::objectValue );

        val[ "type" ] = Serializer::TYPE_ENVREF;
        val[ "object" ] = Serializer::ENVREF_MODE_G;

        return val;
      }

      /**
       * Builds substitutions. These are specific table pointers that require an alternate route to be taken when serializing the table. That route is the value
       * of the "substitutions" map.
       *
       * STACK ARGS: none
       * (Stack is unmodified after call)
       */
      void Serializer::buildSubstitutions() {
        // Substitution 1: all classes
        // Every currently registered class needs a substitution. When we encounter these pointers as we save instances, pivot to createClassReference.

        lua_getglobal( L, "bluebear" ); // bluebear
        lua_pushstring( L, "classes" ); // "classes" bluebear
        lua_gettable( L, -2 ); // bluebear.classes bluebear

        traverseTableForSubstitutions(); // bluebear

        // Substitution 2: The bluebear table itself
        // This object, if it's referred to anywhere, needs to be serialized as "the bluebear table"
        substitutions[ Tools::Utility::pointerToString( lua_topointer( L, -1 ) ) ] = std::bind( &Serializer::createConcordiaNSReference, this );

        // Substitution 3: The _G/_ENV variable
        lua_getglobal( L, "_G" ); // _G bluebear
        substitutions[ Tools::Utility::pointerToString( lua_topointer( L, -1 ) ) ] = std::bind( &Serializer::createGReference, this );

        lua_pop( L, 2 ); // EMPTY
      }

      /**
       * Traverses a class table for substitutions
       *
       * STACK ARGS: table
       * RETURNS: EMPTY
       */
      void Serializer::traverseTableForSubstitutions() {

        lua_pushnil( L ); // nil table
        while( lua_next( L, -2 ) != 0 ) { // subtable "name" table

          lua_pushstring( L, "__middleclass" ); // "__middleclass" subtable "name" table
          lua_gettable( L, -2 ); // boolean subtable "name" table
          if( lua_isboolean( L, -1 ) && lua_toboolean( L, -1 ) ) {
            // Subtable is an actual class

            lua_pop( L, 1 ); // subtable "name" table

            // Type is a proper middleclass object
            substitutions[ Tools::Utility::pointerToString( lua_topointer( L, -1 ) ) ] = std::bind( &Serializer::createClassReference, this );

            lua_pop( L, 1 ); // "name" table
          } else {
            // Subtable is just another part of the namespace and needs to be traversed

            lua_pop( L, 1 ); // subtable "name" table
            traverseTableForSubstitutions(); // "name" table
          }
        } // table

        lua_pop( L, 1 ); // EMPTY

      }
    }
  }
}
