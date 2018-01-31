#include "scripting/event/waitingtable.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <string>
#include <sstream>

namespace BlueBear {
  namespace Scripting {
    namespace Event {

      void WaitingTable::loadFromJSON( Json::Value& loadingTable, std::map< std::string, LuaReference >& entities, sol::state& lua ) {

        // do timerMap
        Json::Value& timerMap = loadingTable[ "timerMap" ];

        for( Json::Value::iterator iterator = timerMap.begin(); iterator != timerMap.end(); ++iterator ) {
          Json::Value key = iterator.key();
          Json::Value array = *iterator;

          Tick deadline = 0; std::stringstream( key.asString() ) >> deadline;

          std::list< sol::function >& bucket = getBucket( deadline );
          for( Json::Value& pVal : array ) {
            std::string pointer = pVal.asString();
            bucket.push_back(
              sol::function( lua.lua_state(), sol::ref_index{ entities.at( pointer ) } )
            );
          }
        }

      }

      /**
       * Recommended that you disable garbage collection before saving the WaitingTable (pointer may save incorrectly)
       * The assumption here is that any pointer we scoop up here is in the world table by now!
       */
      Json::Value WaitingTable::saveToJSON( lua_State* L ) {
        Json::Value json;

        // TODO !!

        return json;
      }

      std::list< sol::function >& WaitingTable::getBucket( Tick key ) {
        auto pair = timerMap.find( key );

        if( pair == timerMap.end() ) {
          timerMap.emplace( key, std::list< sol::function >{} );
        }

        return timerMap[ key ];
      }

      std::string WaitingTable::waitForTick( Tick deadline, sol::function function ) {
        std::list< sol::function >& bucket = getBucket( deadline );

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
          std::list< sol::function >& bucket = pair->second;

          for( std::list< sol::function >::iterator iterator = bucket.begin(), end = bucket.end(); iterator != end; ++iterator ) {
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
          std::list< sol::function >& bucket = pair->second;

          // Push everything in this bucket
          for( sol::function reference : bucket ) {
            queuedCallbacks.push( reference );
          }

          // Destroy the bucket, no need for it any longer
          timerMap.erase( tick );
        }
      }

    }
  }
}
