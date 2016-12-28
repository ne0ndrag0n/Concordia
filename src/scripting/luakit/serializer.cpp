#include "scripting/luakit/serializer.hpp"
#include "scripting/engine.hpp"
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
#include <unordered_set>

namespace BlueBear {
  namespace Scripting {
    namespace LuaKit {

      const std::string Serializer::TYPE_TABLE = "table";
      // itables are tables we need to re-create an instance from first, then overlaying the data onto it from file
      const std::string Serializer::TYPE_ITABLE = "itable";
      const std::string Serializer::TYPE_FUNCTION = "function";
      // sfunctions are serialized function bindings that can be recreated at runtime without saving actual code to file
      const std::string Serializer::TYPE_SFUNCTION = "sfunction";
      // refs are references to lua referenceable items (functions and tables only)
      const std::string Serializer::TYPE_REF = "ref";
      // class is a reference to a class type. This allows us to serialise a lot without saving the class/class code in the lot JSON file
      const std::string Serializer::TYPE_CLASSID = "class";
      // An envref refers to a system-level global present in all instances of a Concordia lot (e.g. the "bluebear" global)
      const std::string Serializer::TYPE_ENVREF = "envref";

      const std::string Serializer::ENVREF_MODE_BBGLOBAL = "bluebear";
      const std::string Serializer::ENVREF_MODE_G = "_G";

      Serializer::Serializer( lua_State* L ) : L( L ) {}

      /**
       * Using the Engine-tracked index of system.entity.base objects as a starting point, save the current state of the Lua world.
       */
      Json::Value Serializer::saveWorld( std::vector< LuaReference >& objects, Engine& engine ) {
        world = Json::Value( Json::objectValue );

        // STOP the garbage collector so pointer references remain intact as we operate
        // Lua currently doesn't move items around as part of garbage collection (I think) but relying
        // on it is still undefined behaviour
        lua_gc( L, LUA_GCSTOP, 0 );

        // Build all required substitutions (classes, the bluebear global)
        buildSubstitutions();

        // First scoop up our system.entity.base objects that are tracked in the objects std::vector
        for( LuaReference instance : objects ) {
          // table
          lua_rawgeti( L, LUA_REGISTRYINDEX, instance );

          // EMPTY
          createTableOnMasterList();
        }

        // Next, scoop up any items that are known only to the engine, but don't have any reference anywhere else in the game world.
        lua_pushvalue( L, LUA_REGISTRYINDEX ); // registry
        lua_pushnil( L ); // nil registry
        while( lua_next( L, -2 ) ) { // item 1 registry
          std::string pointer = Tools::Utility::pointerToString( lua_topointer( L, -1 ) );
          if( !world.isMember( pointer ) ) {
            // Needs to be scooped up and saved, it's a part of the Luasphere that is known to the engine but not the game world itself
            // ORDINARILY, these should only be table or function refs.
            if( lua_istable( L, -1 ) ) {
              createTableOnMasterList(); // 1 registry
            } else if ( lua_isfunction( L, -1 ) ) {
              createFunctionOnMasterList(); // 1 registry
            } else {
              Log::getInstance().error( "LuaKit::Serializer::saveWorld", "Engine tracked a non-table, non-function Lua reference, and I can't serialize it. Bitch at ne0ndrag0n because this is a fully preventable bug." );
              lua_pop( L, 1 ); // 1 registry
            }
          } else {
            lua_pop( L, 1 ); // 1 registry
          }
        } // registry

        lua_pop( L, 1 ); // EMPTY

        Json::Value result = Json::Value( Json::objectValue );
        result[ "world" ] = world;
        result[ "waitingTable" ] = engine.waitingTable.saveToJSON( L );

        // Use this regex when saving to a file, it fixes an annoying thing with JsonCpp where the "\u" is replaced by "\\u"
        // TODO: Remove when we actually save to file
        Log::getInstance().debug( "LuaKit::Serializer::saveWorld", "\n" + std::regex_replace( result.toStyledString(), std::regex( R"(\\\\u)" ), "\\u" ) );

        // Restart the garbage collector, and give it a good cycle
        lua_gc( L, LUA_GCRESTART, 0 );
        lua_gc( L, LUA_GCCOLLECT, 0 );

        return result;
      }

      /**
       * Load (deserialise) the game world.
       */
      std::vector< LuaReference > Serializer::loadWorld( Json::Value& engineDefinition, Engine& engine ) {
        world = engineDefinition[ "world" ];

        globalEntities.clear();
        globalInstanceEntities.clear();

        // STOP the garbage collector so pointer references remain intact as we operate
        lua_gc( L, LUA_GCSTOP, 0 );

        for( Json::Value::iterator jsonIterator = world.begin(); jsonIterator != world.end(); ++jsonIterator ) {
          std::string addressKey = jsonIterator.key().asString();

          // This address key may have been already been scooped up by a prior getReference() call
          if( !globalItemExists( addressKey ) ) {
            createGlobalItem( addressKey );
          }
        }

        // Load waiting functions into waitingTableExclusions, returning a list of items that should NOT be unref'd
        // TODO: This is shit. Would it kill us to repurpose globalInstanceEntities into a general list where references are not released?
        std::unordered_set< LuaReference > waitingTableExclusions = engine.waitingTable.loadFromJSON( engineDefinition[ "waitingTable" ], globalEntities );

        // Release references to items we no longer require. This allows the engine to start discarding items it no longer requires.
        for( auto& entityPair : globalEntities ) {
          if( waitingTableExclusions.find( entityPair.second ) == waitingTableExclusions.end() ) {
            luaL_unref( L, LUA_REGISTRYINDEX, entityPair.second );
          }
        }

        // After we're done, restart the garbage collector and give it a good cycle
        // If, for some reason, there's any disconnected item in the original file...it will be discarded here
        lua_gc( L, LUA_GCRESTART, 0 );
        lua_gc( L, LUA_GCCOLLECT, 0 );

        // Return the globalInstanceEntities map as a vector
        std::vector< LuaReference > serializableInstances;
        for( auto& instancePair : globalInstanceEntities ) {
          serializableInstances.push_back( instancePair.second );
        }

        return serializableInstances;
      }

      /**
       * Create a "global" Lua item. These are at the top level of world and can be table, itable, function, or sfunction.
       */
      LuaReference Serializer::createGlobalItem( const std::string& addressKey ) {
        Json::Value& item = world[ addressKey ];

        switch( Tools::Utility::hash( item[ "type" ].asCString() ) ) {
          case Tools::Utility::hash( "table" ):
            return createTable( addressKey, item );
          case Tools::Utility::hash( "itable" ):
            return createITable( addressKey, item );
          case Tools::Utility::hash( "function" ):
            return createFunction( addressKey, item );
          case Tools::Utility::hash( "sfunction" ):
            return createSFunction( addressKey, item );
        }
      }

      /**
       * Create a table in RAM and associate it with the given address. Register it with globalEntities, return the reference we just created.
       */
      LuaReference Serializer::createTable( const std::string& addressKey, Json::Value& tableDefinition ) {
        lua_newtable( L ); // table

        for( Json::Value& pair : tableDefinition[ "entries" ] ) {
          inferTypeFromJSON( pair[ "key" ] ); // key_object table
          inferTypeFromJSON( pair[ "value" ] ); // value_object key_object table

          lua_settable( L, -3 ); // table
        }

        // Now set the metatable if it exists
        if( tableDefinition.isMember( "metatable" ) ) {
          Json::Value& metatable = tableDefinition[ "metatable" ];
          determineInnerItem( metatable ); // metatable table
          lua_setmetatable( L, -2 ); // table
        }

        return globalEntities[ addressKey ] = luaL_ref( L, LUA_REGISTRYINDEX ); // EMPTY
      }

      /**
       * Create an itable, which requires first getting the instance of the specified classID, then overlaying the specified properties.
       */
      LuaReference Serializer::createITable( const std::string& addressKey, Json::Value& tableDefinition ) {
        lua_getglobal( L, "bluebear" ); // bluebear
        lua_pushstring( L, "get_class" ); // "get_class" bluebear
        lua_gettable( L, -2 ); // <bluebear.get_class> bluebear

        std::string classID = tableDefinition[ "classID" ].asString();
        lua_pushstring( L, classID.c_str() ); // "classID" <bluebear.get_class> bluebear

        if( lua_pcall( L, 1, 1, 0 ) != 0 ) { // "error" bluebear
          Log::getInstance().error( "LuaKit::Serializer::createITable", "Class " + classID + " doesn't appear to be loaded!" );
          lua_pop( L, 2 ); // EMPTY
          return -1;
        } // Class bluebear

        // "new" for any object shouldn't take any arguments
        lua_pushstring( L, "new" ); // "new" Class bluebear
        lua_gettable( L, -2 ); // <Class.new> Class bluebear
        lua_pushvalue( L, -2 ); // Class <Class.new> Class bluebear

        if( lua_pcall( L, 1, 1, 0 ) != 0 ) { // "error" Class bluebear
          Log::getInstance().error( "LuaKit::Serializer::createITable", "Could not initialize class: " + std::string( lua_tostring( L, -1 ) ) );
          lua_pop( L, 3 ); // EMPTY
          return -1;
        } // instance Class bluebear

        // Using instance, overlay all properties specified in tableDefinition
        for( Json::Value& pair : tableDefinition[ "entries" ] ) {
          inferTypeFromJSON( pair[ "key" ] ); // key_object instance Class bluebear
          inferTypeFromJSON( pair[ "value" ] ); // value_object key_object instance Class bluebear

          lua_settable( L, -3 ); // instance Class bluebear
        }

        // No metatable? itable-type objects should never set a metatable because middleclass already does that. Consequently, you should never fuck with the metatable in your game entities.

        // Set reference
        int ref = globalInstanceEntities[ addressKey ] = luaL_ref( L, LUA_REGISTRYINDEX ); // Class bluebear

        lua_pop( L, 2 ); // EMPTY

        return ref;
      }

      /**
       * Create a function from its serialized function text and set its upvalues. You generally shouldn't use this functionality; instead, write code to create sfunctions instead.
       */
      LuaReference Serializer::createFunction( const std::string& addressKey, Json::Value& tableDefinition ) {

        // push the function definition
        std::string functionBody = Tools::Utility::decodeUTF8( tableDefinition[ "body" ].asString() );
        if( luaL_loadbuffer( L, functionBody.c_str(), functionBody.length(), "__serialized_lua_chunk" ) ) { // error
          Log::getInstance().error( "LuaKit::Serializer::createFunction", "Could not load a serialized function chunk: " + std::string( lua_tostring( L, -1 ) ) );
          lua_pop( L, 1 ); // EMPTY
          return -1;
        } // <function>

        unpackUpvalues( tableDefinition[ "upvalues" ] );

        return globalEntities[ addressKey ] = luaL_ref( L, LUA_REGISTRYINDEX ); // EMPTY
      }

      /**
       * Create an sfunction, a serialized form of function where the function body is accessible via a reference to a modpack-registered class.
       */
      LuaReference Serializer::createSFunction( const std::string& addressKey, Json::Value& tableDefinition ) {
        // Set up a standard sfunction-type bluebear.util.bind function without any arguments

        lua_getglobal( L, "bluebear" ); // bluebear
        lua_pushstring( L, "util" ); // "util" bluebear
        lua_gettable( L, -2 ); // bluebear.util bluebear

        lua_pushstring( L, "bind" ); // "bind" bluebear.util bluebear
        lua_gettable( L, -2 ); // <bind> bluebear.util bluebear

        std::string key = tableDefinition[ "class" ].asString() + ":" + tableDefinition[ "method" ].asString();
        lua_pushstring( L, key.c_str() ); // "namespace.class:method" <bind> bluebear.util bluebear

        if( lua_pcall( L, 1, 1, 0 ) != 0 ) { // "error" bluebear.util bluebear
          Log::getInstance().error( "LuaKit::Serializer::createSFunction", "Error creating sfunction: " + std::string( lua_tostring( L, -1 ) ) );
          lua_pop( L, 3 ); // EMPTY
          return -1;
        } // <bound> bluebear.util bluebear

        // Now take <bound> and modify its "args" upvalue to point to the table it originally pointed to
        int upvalueIndex = getUpvalueByName( "args" ); // args <bound> bluebear.util bluebear
        // We only wanted the index
        lua_pop( L, 1 ); // <bound> bluebear.util bluebear

        determineInnerItem( tableDefinition[ "args" ] ); // args <bound> bluebear.util bluebear
        setUpvalueByIndex( upvalueIndex ); // <bound> bluebear.util bluebear

        int ref = globalEntities[ addressKey ] = luaL_ref( L, LUA_REGISTRYINDEX ); // bluebear.util bluebear

        lua_pop( L, 2 ); // EMPTY

        return ref;
      }

      /**
       * Add upvalues to the function on top of the stack.
       *
       * STACK ARGS: <function>
       * (Stack is unmodified after call)
       */
      void Serializer::unpackUpvalues( Json::Value& upvalues ) {
        unsigned int upvalueIndex = 1;
        for( Json::Value& upvalue : upvalues ) {
          inferTypeFromJSON( upvalue ); // upvalue <function>

          setUpvalueByIndex( upvalueIndex ); // <function>

          upvalueIndex++;
        }
      }

      /**
       * Given a function and a value, set function's upvalueIndex-th upvalue to value using the lua_upvaluejoin hack.
       * We have to proxy into bluebear.util.set_upvalue_by_index as it appears there's no way to create a function with an upvalue referring to local scope in C.
       *
       * STACK ARGS: value <function>
       * RETURNS: <function>
       */
      void Serializer::setUpvalueByIndex( int upvalueIndex ) {

        lua_getglobal( L, "bluebear" ); // bluebear value <function>

        lua_pushstring( L, "util" ); // "util" bluebear value <function>
        lua_gettable( L, -2 ); // bluebear.util bluebear value <function>

        lua_pushstring( L, "set_upvalue_by_index" ); // "set_upvalue_by_index" bluebear.util bluebear value <function>
        lua_gettable( L, -2 ); // <set_upvalue_by_index> bluebear.util bluebear value <function>

        lua_pushvalue( L, -5 ); // <function> <set_upvalue_by_index> bluebear.util bluebear value <function>

        lua_pushnumber( L, upvalueIndex ); // index <function> <set_upvalue_by_index> bluebear.util bluebear value <function>

        lua_pushvalue( L, -6 ); // value index <function> <set_upvalue_by_index> bluebear.util bluebear value <function>

        if( lua_pcall( L, 3, 0, 0 ) ) { // "error" bluebear.util bluebear value <function>
          Log::getInstance().error( "LuaKit::Serializer::setUpvalueByIndex", std::string( lua_tostring( L, -1 ) ) );
          lua_pop( L, 4 ); // <function>
          return;
        } // bluebear.util bluebear value <function>

        lua_pop( L, 3 ); // <function>

      }

      /**
       * Using the given objectToken, determine what value to push onto the lua stack from the given token.
       *
       * STACK ARGS: none
       * RETURNS: (One of any lua type)
       */
      void Serializer::inferTypeFromJSON( Json::Value& objectToken ) {
        switch( objectToken.type() ) {
          case Json::ValueType::intValue:
          case Json::ValueType::uintValue:
          case Json::ValueType::realValue:
            lua_pushnumber( L, objectToken.asDouble() ); // number
            return;
          case Json::ValueType::stringValue:
            lua_pushstring( L, objectToken.asCString() ); // string
            return;
          case Json::ValueType::objectValue:
            determineInnerItem( objectToken ); // (table, function, or nil)
            return;
          case Json::ValueType::nullValue:
          default:
            lua_pushnil( L ); // nil
            return;
        }
      }

      /**
       * Push the referred object onto the stack (and create it if necessary)
       *
       * STACK ARGS: none
       * RETURNS: (table or function)
       */
      void Serializer::getReference( const std::string& addressKey ) {
        // Does the item need to be created or does it already exist?
        auto geEntry = globalEntities.find( addressKey );
        if( geEntry != globalEntities.end() ) {
          lua_rawgeti( L, LUA_REGISTRYINDEX, geEntry->second ); // item
          return;
        }

        auto gieEntry = globalInstanceEntities.find( addressKey );
        if( gieEntry != globalInstanceEntities.end() ) {
          lua_rawgeti( L, LUA_REGISTRYINDEX, gieEntry->second ); // item
          return;
        }

        // If we got here, then it looks like we need to create the item before pushing it onto the stack
        lua_rawgeti( L, LUA_REGISTRYINDEX, createGlobalItem( addressKey ) ); // item
      }

      /**
       * Push the correct envref object from the given envKey.
       *
       * STACK ARGS: none
       * RETURNS: (table or function)
       */
      void Serializer::determineInnerItem( Json::Value& objectToken ) {
        const char* type = objectToken[ "type" ].asCString();

        switch( Tools::Utility::hash( type ) ) {
          case Tools::Utility::hash( "ref" ):
            getReference( objectToken[ "ptr" ].asString() ); // (table or function)
            return;
          case Tools::Utility::hash( "envref" ):
            getEnvReference( objectToken[ "object" ].asString() ); // (table or function)
            return;
          default:
            lua_pushnil( L ); // nil
        }
      }

      /**
       * Determine what kind of non-unique "envref" to give back is.
       *
       * STACK ARGS: none
       * RETURNS: (table or function)
       */
      void Serializer::getEnvReference( const std::string& envRefKey ) {
        const char* key = envRefKey.c_str();

        switch( Tools::Utility::hash( key ) ) {
          case Tools::Utility::hash( "bluebear" ):
            lua_getglobal( L, "bluebear" ); // bluebear
            return;
          case Tools::Utility::hash( "_G" ):
            lua_getglobal( L, "_G" ); // _G
            return;
        }
      }

      /**
       * Returns true if the addressKey is defined in either globalEntities or globalInstanceEntities
       */
      bool Serializer::globalItemExists( const std::string& addressKey ) {
        return ( globalEntities.find( addressKey ) != globalEntities.end() ) || ( globalInstanceEntities.find( addressKey ) != globalInstanceEntities.end() );
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
        Json::Value& item = parentItem[ "entries" ] = Json::Value( Json::arrayValue );

        // Is this an INSTANCE-TYPE table? Instance-type tables have a "class" field pointing to a table with the __middleclass system property set to "true"
        std::string instanceClassID;
        bool isInstanceTable = false;
        lua_pushstring( L, "class" ); // "class" table
        lua_gettable( L, -2 ); // Class table
        if( lua_istable( L, -1 ) ) {

          lua_pushstring( L, "__middleclass" ); // "__middleclass" Class table
          lua_gettable( L, -2 ); // Class.__middleclass Class table

          if( lua_isboolean( L, -1 ) && lua_toboolean( L, -1 ) ) {
            lua_pop( L, 1 ); // Class table

            lua_pushstring( L, "name" ); // "name" Class table
            lua_gettable( L, -2 ); // Class.name Class table
            parentItem[ "classID" ] = instanceClassID = std::string( lua_tostring( L, -1 ) );
          }

          lua_pop( L, 2 ); // table

        } else {
          lua_pop( L, 1 ); // table
        }
        isInstanceTable = !( instanceClassID.empty() );

        parentItem[ "type" ] = isInstanceTable ? Serializer::TYPE_ITABLE : Serializer::TYPE_TABLE;

        lua_pushnil( L ); // nil table

        // Iterate over the contents of this table
        while( lua_next( L, -2 ) != 0 ) { // value key table

          lua_pushvalue( L, -2 ); // key value key table
          if( isInstanceTable && lua_isstring( L, -1 ) && std::string( lua_tostring( L, -1 ) ) == "class" ) {
            // Don't do anything for this key-value pair
            lua_pop( L, 2 ); // key table
          } else {
            lua_pop( L, 1 ); // value key table

            // { "key": <objtype>, "value": <objtype> }
            Json::Value pair = Json::Value( Json::objectValue );

            inferType( pair, "value" ); // key table

            lua_pushvalue( L, -1 ); // key key table
            inferType( pair, "key" ); // key table

            item.append( pair );
          }
        } // table

        if( !isInstanceTable && lua_getmetatable( L, -1 ) ) { // metatable table
          std::string worldPointer( Tools::Utility::pointerToString( lua_topointer( L, -1 ) ) );

          if( !world.isMember( worldPointer ) ) {
            lua_pushvalue( L, -1 ); // metatable metatable table
            createTableOnMasterList(); // metatable table
          }

          parentItem[ "metatable" ] = createReference();

          lua_pop( L, 1 ); // table
        }

        lua_pop( L, 1 ); // EMPTY
      }

      /**
       * Create a function/sfunction on the master list
       *
       * STACK ARGS: function
       * RETURNS: none
       */
       void Serializer::createFunctionOnMasterList() {

         std::string thisTable( Tools::Utility::pointerToString( lua_topointer( L, -1 ) ) );

         Json::Value func( Json::objectValue );

         if( canCreateSfunction() ) {
           // This sfunction can be created
           func[ "type" ] = Serializer::TYPE_SFUNCTION;

           // Build everything we need to create an sfunction

           getUpvalueByName( "__derived_class" ); // "__derived_class" function
           func[ "class" ] = lua_tostring( L, -1 );
           lua_pop( L, 1 ); // function

           getUpvalueByName( "__derived_func" ); // "__derived_func" function
           func[ "method" ] = lua_tostring( L, -1 );
           lua_pop( L, 1 ); // function

           getUpvalueByName( "args" ); // args function
           std::string upvaluePointer( Tools::Utility::pointerToString( lua_topointer( L, -1 ) ) );
           if( !world.isMember( upvaluePointer ) ) {
             lua_pushvalue( L, -1 ); // args args function
             createTableOnMasterList(); // args function
           }

           func[ "args" ] = createReference();

           lua_pop( L, 1 ); // function

           world[ thisTable ] = func;
         } else {
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

               // Spit out '\uxxxx'
               stringBuilder << "\\u" << std::setfill( '0' ) << std::setw( 4 ) << std::hex << ic;
             }

             func[ "body" ] = stringBuilder.str();

             lua_pop( L, 2 ); // function

             // Now serialize the associated upvalues
             addUpvalues( func );

             world[ thisTable ] = func;
           } else { // "error" string function
              Log::getInstance().error( "LuaKit::Serializer::createFunctionOnMasterList", "Could not serialize function: " + std::string( lua_tostring( L, -1 ) ) );
              lua_pop( L, 2 ); // function
           }

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
                // We need to serialize code directly to the file
                if( !world.isMember( worldPointer ) ) {
                  // Need to create the function
                  lua_pushvalue( L, -1 ); // function function
                  createFunctionOnMasterList(); // function
                }

                pair[ field ] = createReference();
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

          inferType( upvalue, "value" ); // function

          upvalues.append( upvalue[ "value" ] );
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
       * O(n) method, use with care. Returns upvalue index in C++.
       *
       * STACK ARGS: function
       * RETURNS: (upvalue || nil) function
       */
      int Serializer::getUpvalueByName( const std::string& name ) {

        for( int i = 1; const char* upvalueId = lua_getupvalue( L, -1, i ); i++ ) { // upvalue function
          std::string key( upvalueId );

          if( key == name ) {
            return i;
          }

          lua_pop( L, 1 ); // function
        }

        lua_pushnil( L ); // nil function
        return -1;
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
