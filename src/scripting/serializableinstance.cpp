#include "scripting/serializableinstance.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "tools/utility.hpp"
#include "log.hpp"
#include <json/json.h>
#include <cstring>
#include <string>

namespace BlueBear {
	namespace Scripting {

		SerializableInstance::SerializableInstance( lua_State* L, const Tick& currentTickReference, int luaVMInstance ) : L( L ), luaVMInstance( luaVMInstance ), currentTickReference( currentTickReference ) {
			// table
			lua_rawgeti( L, LUA_REGISTRYINDEX, luaVMInstance );

			if( !lua_istable( L, -1 ) ) {
				Log::getInstance().error( "SerializableInstance::SerializableInstance", "Constructor called on an item that isn't a table!" );
				// EMPTY
				lua_pop( L, 1 );
				throw InvalidLuaVMInstanceException();
			}

			// "cid" table
			Tools::Utility::getTableValue( L, "_cid" );

			if( !lua_isstring( L, -1 ) ) {
				Log::getInstance().error( "SerializableInstance::SerializableInstance", "Constructor called on a table that is neither serializable or has a valid _cid!" );
				// EMPTY
				lua_pop( L, 2 );
				throw LuaValueNotTableException();
			}

			cid = std::string( lua_tostring( L, -1 ) );

			// EMPTY
			lua_pop( L, 2 );
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
