#ifndef EVENT_MANAGER_ELEMENTS
#define EVENT_MANAGER_ELEMENTS

#include <unordered_map>
#include <functional>
#include <vector>
#include <string>

namespace BlueBear {
  namespace Device {
    namespace Input {
      class Metadata;
    }
  }

  namespace Graphics {
    namespace UserInterface {
      class Element;

      namespace Event {

        class EventBundle {
          Element* parent = nullptr;
          std::unordered_map< std::string, std::vector< std::function< void( Device::Input::Metadata ) > > > inputEvents;

          template < typename T >
          unsigned int insertElement(
            std::unordered_map< std::string, std::vector< std::function< void( T ) > > >& map,
            const std::string& key,
            std::function< void( T ) > value
          ) {
            auto& bucket = map[ key ];

            for( unsigned int i = 0; i != bucket.size(); i++ ) {
              if( !bucket[ i ] ) {
                bucket[ i ] = value;
                return i;
              }
            }

            bucket.push_back( value );
            return bucket.size() - 1;
          };

          template < typename T >
          void removeElement(
            std::unordered_map< std::string, std::vector< std::function< void( T ) > > >& map,
            const std::string& key,
            unsigned int id
          ) {
            auto it = map.find( key );

            if( it != map.end() && id < it->second.size() ) {
              it->second[ id ] = std::function< void( T ) >();
            }
          };

          void dumpEvents();

        public:
          EventBundle( Element* parent );
          ~EventBundle();

          unsigned int registerInputEvent( const std::string& key, std::function< void( Device::Input::Metadata ) > callback );
          void unregisterInputEvent( const std::string& key, unsigned int id );

          void trigger( const std::string& key, Device::Input::Metadata metadata, bool bubble = true );
        };

      }
    }
  }
}

#endif
