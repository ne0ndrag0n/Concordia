#include "scripting/serializableinstance.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "tools/utility.hpp"
#include "log.hpp"
#include <jsoncpp/json/json.h>
#include <cstring>
#include <string>

namespace BlueBear {
	namespace Scripting {
		Json::FastWriter SerializableInstance::writer;

		SerializableInstance::SerializableInstance( lua_State* L, const Tick& currentTickReference, const std::string& classID )
			: L( L ), currentTickReference( currentTickReference ), classID( classID ) {
				createEntityTable();

				// When creating a plain SerializableInstance (one that is not loaded from JSON)
				// call its on_create() method
				onCreate();
		}

		SerializableInstance::SerializableInstance( lua_State* L, const Tick& currentTickReference, const Json::Value& serialEntity )
			: L( L ), currentTickReference( currentTickReference ), classID( serialEntity[ "classID" ].asString() ) {
			createEntityTable( serialEntity );
		}

		/**
		 * Call the on_create method of this Lua instance
		 */
		void SerializableInstance::onCreate() {
			// instance
			lua_rawgeti( L, LUA_REGISTRYINDEX, luaVMInstance );

			// <on_create> instance
			Tools::Utility::getTableValue( L, "on_create" );

			// instance <on_create> instance
			lua_pushvalue( L, -2 );

			// instance
			if( lua_pcall( L, 1, 0, 0 ) != 0 ) {
				// error instance
				Log::getInstance().error( "SerializableInstance::onCreate", std::string( lua_tostring( L, -1 ) ) );

				// instance
				lua_pop( L, 1 );
			}

			// EMPTY
			lua_pop( L, 1 );
		}

		void SerializableInstance::createEntityTable() {
			// Get bluebear.classes
			// bluebear
			lua_getglobal( L, "bluebear" );

			// bluebear.classes bluebear
			Tools::Utility::getTableValue( L, "classes" );

			// Get the actual class referred to by this classID
			// Class bluebear
			Tools::Utility::getTableTreeValue( L, classID );

			// If "nil bluebear": Cannot continue: the class was not found
			if( lua_istable( L, -1 ) ) {
				// Class is at the top of the stack
				// Let's start by creating a new instance

				// <new> Class bluebear
				Tools::Utility::getTableValue( L, "new" );

				// Class <new> Class bluebear
				lua_pushvalue( L, -2 );

				// instance Class bluebear
				if( lua_pcall( L, 1, 1, 0 ) == 0 ) {
					// Instance is created!
					ok = true;

					// this->luaVMInstance holds a Lua registry index to the table returned by this function
					// Class bluebear
					luaVMInstance = luaL_ref( L, LUA_REGISTRYINDEX );

					// EMPTY
					lua_pop( L, 2 );
				} else {
					// error instance Class bluebear
					Log::getInstance().error( "SerializableInstance::createEntityTable", std::string( lua_tostring( L, -1 ) ) );
					lua_pop( L, 4 );
				}
			} else {
				Log::getInstance().error( "SerializableInstance::createEntityTable", "Could not find class " + classID );

				lua_pop( L, 2 );
			}
		}

		void SerializableInstance::createEntityTable( const Json::Value& serialEntity ) {
			// Get bluebear.classes
			// bluebear
			lua_getglobal( L, "bluebear" );

			// bluebear.classes bluebear
			Tools::Utility::getTableValue( L, "classes" );

			// Get the actual class referred to by this classID
			// Class bluebear
			Tools::Utility::getTableTreeValue( L, classID );

			// If "nil bluebear": Cannot continue: the class was not found
			if( lua_istable( L, -1 ) ) {
				// Class is at the top of the stack
				// Let's start by creating a new instance

				// <new> Class bluebear
				Tools::Utility::getTableValue( L, "new" );

				// Class <new> Class bluebear
				lua_pushvalue( L, -2 );

				// Use JSON.decode( JSON, "serialisedInstance" ) to transform the serialised instance to a table
				// JSON Class <new> Class bluebear
				lua_getglobal( L, "JSON" );

				// <decode> JSON Class <new> Class bluebear
				Tools::Utility::getTableValue( L, "decode" );

				// JSON <decode> JSON Class <new> Class bluebear
				lua_pushvalue( L, -2 );

				// "serialisedInstance" JSON <decode> JSON Class <new> Class bluebear
				lua_pushstring( L, writer.write( serialEntity[ "instance" ] ).c_str() );

				// Deserialize the JSON object into a Lua table
				// instanceTable JSON Class <new> Class bluebear
				if( lua_pcall( L, 2, 1, 0 ) == 0 ) {
					// Prepare for the pcall that makes the instance
					// instanceTable Class <new> Class bluebear
					lua_remove( L, -2 );
				} else {
					// error JSON Class <new> Class bluebear
					Log::getInstance().error( "SerializableInstance::createEntityTable( const Json::Value& serialEntity )", "Problem deserialising using Lua JSON lib on " + classID );

					lua_pop( L, 6 );
					return;
				}

				// instance Class bluebear
				if( lua_pcall( L, 2, 1, 0 ) == 0 ) {
					// Instance is created!

					// Get the cid. There is absolutely now a cid.
					// cid instance Class bluebear
					Tools::Utility::getTableValue( L, "_cid" );
					if( lua_isstring( L, -1 ) ) {
						cid = lua_tostring( L, -1 );
						ok = true;
					} else {
						Log::getInstance().error( "SerializableInstance::createEntityTable( const Json::Value& serialEntity )", "Instance failed to deserialize or create a string-type cid!" );
						// EMPTY
						lua_pop( L, 4 );
						return;
					}

					// instance Class bluebear
					lua_pop( L, 1 );

					// this->luaVMInstance holds a Lua registry index to the table returned by this function
					// Class bluebear
					luaVMInstance = luaL_ref( L, LUA_REGISTRYINDEX );

					// EMPTY
					lua_pop( L, 2 );
				} else {
					// error Class bluebear
					Log::getInstance().error( "SerializableInstance::createEntityTable( const Json::Value& serialEntity )", std::string( lua_tostring( L, -1 ) ) );
					lua_pop( L, 3 );
				}
			} else {
				Log::getInstance().error( "SerializableInstance::createEntityTable( const Json::Value& serialEntity )", "Could not find class " + classID );

				lua_pop( L, 2 );
			}
		}

		/**
		 * Registers a callback to occur on the given tick. Call this function with a table full of
		 * arguments pushed onto the stack to pass along arguments to your callback as well. If you
		 * don't wish to call any arguments, push nil. Either push nil, or a table, because this
		 * function pops whatever is on the top of the stack.
		 *
		 * TODO: This function merely proxies into system.entity.base:register_callback. When
		 * we're ready to make system.entity.base more of a SerializableInstance, this function should
		 * serve as the only way to register a callback (and not be overridden on system.entity.base).
		 */
		void SerializableInstance::registerCallback( const std::string& callback, Tick tick ) {
			// Arguments on system.entity.base:register_callback are (self, tick, method, wrapped_arguments)

			// instance table/nil
			lua_rawgeti( L, LUA_REGISTRYINDEX, luaVMInstance );

			// register_callback() instance table/nil
			Tools::Utility::getTableValue( L, "register_callback" );

			// instance register_callback() instance table/nil
			lua_pushvalue( L, -2 );

			// tick instance register_callback() instance table/nil
			lua_pushnumber( L, ( double ) tick );

			// method tick instance register_callback() instance table/nil
			lua_pushstring( L, callback.c_str() );

			// table/nil method tick instance register_callback() instance table/nil
			lua_pushvalue( L, -6 );

			// instance table/nil
			if( lua_pcall( L, 4, 0, 0 ) ) {
				// error instance table/nil
				Log::getInstance().error( "SerializableInstance::registerCallback", std::string( lua_tostring( L, -1 ) ) );
				lua_pop( L, 1 );
			}

			// EMPTY
			lua_pop( L, 2 );
		}

		/**
		 * Defers to the next tick. Simply call registerCallback with the next tick. All the same stuff
		 * with pushing a nil or table applies.
		 */
		void SerializableInstance::deferCallback( const std::string& callback ) {
			Tick nextTick = currentTickReference + 1;
			registerCallback( callback, nextTick );
		}

		void SerializableInstance::execute() {
			// instance
			lua_rawgeti( L, LUA_REGISTRYINDEX, luaVMInstance );

			// _sys instance
			Tools::Utility::getTableValue( L, "_sys" );
			// _sys._sched _sys instance
			Tools::Utility::getTableValue( L, "_sched" );

			// Create the key we will need from currentTick
			std::string tickKey = std::to_string( currentTickReference ) + ".0";

			// Check if tickKey has an associated value in _sched
			Tools::Utility::getTableValue( L, tickKey.c_str() );

			if( lua_istable( L, -1 ) ) {
				// tick_array_table _sys._sched _sys instance

				// There's functions that need to be executed
				// nil tick_array_table _sys._sched _sys instance
				lua_pushnil( L );
				while( lua_next( L, -2 ) != 0 ) {
					// SFT 1 tick_array_table _sys._sched _sys instance
					// The Serialised Function Table (SFT) is the value, available on -1
					// The key is the index position, available on -2

					// STEP 1: Get the named function and push a closure with "self"
					// "method" SFT 1 tick_array_table _sys._sched _sys instance
					Tools::Utility::getTableValue( L, "method" );

					std::string functionName = lua_tostring( L, -1 );
					// SFT 1 tick_array_table _sys._sched _sys instance
					lua_pop( L, 1 );

					// object SFT 1 tick_array_table _sys._sched _sys instance
					lua_pushvalue( L, -6 );

					// <function> object SFT 1 tick_array_table _sys._sched _sys instance
					Tools::Utility::getTableValue( L, functionName.c_str() );

					// object <function> object SFT 1 tick_array_table _sys._sched _sys instance
					lua_pushvalue( L, -2 );

					// STEP 2: Push all the function's arguments and call!
					// SFT object <function> object SFT 1 tick_array_table _sys._sched _sys instance
					lua_pushvalue( L, -4 );
					// [argument]/nil SFT object <function> object SFT 1 tick_array_table _sys._sched _sys instance
					Tools::Utility::getTableValue( L, "arguments" );
					// [argument]/nil object <function> object SFT 1 tick_array_table _sys._sched _sys instance
					lua_remove( L, -2 );

					int totalArguments;

					if( lua_istable( L, -1 ) ) {
						// [argument] object <function> object SFT 1 tick_array_table _sys._sched _sys instance

						// The arguments array is now at the top of the stack. How many are in it?
						// This will always be at least 1 (because of self)
						totalArguments = lua_rawlen( L, -1 ) + 1;

						// Without accounting for self, count the number of arguments that need to be unrolled
						if( totalArguments - 1 > 0 ) {
							// e. Use this lovely loop to spit everything in the array out onto the stack
							for( int i = 1; i != totalArguments; i++ ) {
								// argument [argument] object <function> object SFT 1 tick_array_table _sys._sched _sys instance
								lua_rawgeti( L, -i, i );
							};
						}

						// Remove the actual array, which should be at -totalArguments
						// In the case of empty array - totalArguments should be 1 (0+1, the "self" arg)
						// In other cases, it should just be the negative of the length of the array

						// argument object <function> object SFT 1 tick_array_table _sys._sched _sys instance
						lua_remove( L, -totalArguments );
					} else {
						// There are no arguments.
						// nil object <function> object SFT 1 tick_array_table _sys._sched _sys instance

						totalArguments = 1;
						// object <function> object SFT 1 tick_array_table _sys._sched _sys instance
						lua_pop( L, 1 );
					}

					// Call that sumbitch!
					if( lua_pcall( L, totalArguments, 0, 0 ) != 0 ) {
						// error object SFT 1 tick_array_table _sys._sched _sys instance
						// We only get here if the function bombs out
						Log::getInstance().error( "SerializableInstance::execute", "Error in lot entity: " + std::string( lua_tostring( L, -1 ) ) );

						lua_pop( L, 1 );

						ok = false;
					}

					// object SFT 1 tick_array_table _sys._sched _sys instance

					// There's two junk items remaining on the stack; clean 'em up!
					// These two items should be the object table, followed by the SFT
					// leaving the key at -1 and the table at -2 (see how the loop restarts?)
					// 1 tick_array_table _sys._sched _sys instance
					lua_pop( L, 2 );
				}

				// After we're done, the below four items will be left on the stack
				// tick_array_table/nil _sys._sched _sys instance
				// Clear out the tick_array_table so it can be garbage collected

				// "tickKey" tick_array_table/nil _sys._sched _sys instance
				lua_pushstring( L, tickKey.c_str() );
				// nil "tickKey" tick_array_table/nil _sys._sched _sys instance
				lua_pushnil( L );
				// tick_array_table/nil _sys._sched _sys instance
				lua_settable( L, -4 );
			} else {
				// nil _sys._sched _sys instance
				// Nothing to do.
			}

			// Start popping all this crap off the stack
			// EMPTY
			lua_pop( L, 4 );
		}
	}
}
