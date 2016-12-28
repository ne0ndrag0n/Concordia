#include "eventmanager.hpp"

namespace BlueBear {

  void EventManager::listen( EventManager::Signal signal, KeyType key, EventManager::Callback callback ) {
    Bucket& bucket = signals[ signal ];

    bucket[ key ] = callback;
  }

  void EventManager::stopListening( EventManager::Signal signal, KeyType key ) {
    auto pair = signals.find( signal );

    if( pair != signals.end() ) {
      Bucket& bucket = pair->second;

      bucket.erase( key );

      if( bucket.empty() ) {
        signals.erase( signal );
      }
    }
  }

  void EventManager::trigger( EventManager::Signal signal ) {
    auto pair = signals.find( signal );

    if( pair != signals.end() ) {
      Bucket& bucket = pair->second;

      for( auto& nestedPair : bucket ) {
        nestedPair.second();
      }
    }
  }
}
