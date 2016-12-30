#include "scripting/event/waitingtable.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <string>
#include <sstream>

namespace BlueBear {
  namespace Scripting {
    namespace Event {

      std::unordered_set< LuaReference > WaitingTable::loadFromJSON( Json::Value& loadingTable, std::map< std::string, LuaReference >& entities ) {
        std::unordered_set< LuaReference > visitedItems;

        // do timerMap
        Json::Value& timerMap = loadingTable[ "timerMap" ];

        for( Json::Value::iterator iterator = timerMap.begin(); iterator != timerMap.end(); ++iterator ) {
          Json::Value key = iterator.key();
          Json::Value array = *iterator;

          Tick deadline = 0; std::stringstream( key.asString() ) >> deadline;

          std::list< LuaReference >& bucket = getBucket( deadline );
          for( Json::Value& pVal : array ) {
            std::string pointer = pVal.asString();

            LuaReference function = entities.at( pointer );
            bucket.push_back( function );
            visitedItems.insert( function );
          }
        }

        return visitedItems;
      }

      /**
       * Recommended that you disable garbage collection before saving the WaitingTable (pointer may save incorrectly)
       * The assumption here is that any pointer we scoop up here is in the world table by now!
       */
      Json::Value WaitingTable::saveToJSON( lua_State* L ) {
        Json::Value json;

        Json::Value& timerMapJSON = json[ "timerMap" ] = Json::Value( Json::objectValue );

        for( auto& pair : timerMap ) {
          Json::Value& entry = timerMapJSON[ std::to_string( pair.first ) ] = Json::Value( Json::arrayValue );

          for( LuaReference reference : pair.second ) {
            lua_rawgeti( L, LUA_REGISTRYINDEX, reference ); // reference

            entry.append( Tools::Utility::pointerToString( lua_topointer( L, -1 ) ) );

            lua_pop( L, 1 ); // EMPTY
          }
        }

        return json;
      }

      std::list< LuaReference >& WaitingTable::getBucket( Tick key ) {
        auto pair = timerMap.find( key );

        if( pair == timerMap.end() ) {
          timerMap.emplace( key, std::list< LuaReference >{} );
        }

        return timerMap[ key ];
      }

      std::string WaitingTable::waitForTick( Tick deadline, LuaReference function ) {
        auto pair = timerMap.find( deadline );

        if( pair == timerMap.end() ) {
          // Need to create the bucket
          timerMap.emplace( deadline, std::list< LuaReference >{} );
        }

        std::list< LuaReference >& bucket = timerMap[ deadline ];
        bucket.push_back( function );

        return std::to_string( deadline ) + ":" + Tools::Utility::pointerToString( &( bucket.back() ) );
      }

      void WaitingTable::cancelTick( const std::string& handle ) {
        // Decompose the handle into a Tick and bucket pointer
        std::vector< std::string > tokens = Tools::Utility::split( handle, ':' );
        Tick deadline = 0; std::stringstream( tokens[ 0 ] ) >> deadline;
        void* specificPointer = Tools::Utility::stringToPointer( tokens[ 1 ] );

        auto pair = timerMap.find( deadline );
        if( pair != timerMap.end() ) {
          std::list< LuaReference >& bucket = pair->second;

          for( std::list< LuaReference >::iterator iterator = bucket.begin(), end = bucket.end(); iterator != end; ++iterator ) {
            // FUCKING BARF
            void* pointer = &( *iterator );

            if( pointer == specificPointer ) {
              bucket.erase( iterator );
              return;
            }
          }

          Log::getInstance().warn( "Event::WaitingTable::cancelTick", "Bucket found but no callback found for handle " + handle );
        } else {
          Log::getInstance().warn( "Event::WaitingTable::cancelTick", "Deadline bucket didn't exist for handle " + handle );
        }
      }

      void WaitingTable::triggerTick( Tick tick ) {
        auto pair = timerMap.find( tick );

        if( pair != timerMap.end() ) {
          std::list< LuaReference >& bucket = pair->second;

          // Push everything in this bucket
          for( LuaReference reference : bucket ) {
            queuedCallbacks.push( reference );
          }

          // Destroy the bucket, no need for it any longer
          timerMap.erase( tick );
        }
      }

    }
  }
}
