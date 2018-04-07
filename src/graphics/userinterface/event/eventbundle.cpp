#include "graphics/userinterface/event/eventbundle.hpp"
#include "graphics/userinterface/element.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Event {

        EventBundle::EventBundle( Element* parent ) : parent( parent ) {}

        unsigned int EventBundle::registerInputEvent( const std::string& key, std::function< void( Device::Input::Input::Metadata ) > callback ) {
          return insertElement( inputEvents, key, callback );
        }

        void EventBundle::unregisterInputEvent( const std::string& key, unsigned int id ) {
          removeElement( inputEvents, key, id );
        }

        void EventBundle::trigger( const std::string& key, Device::Input::Input::Metadata metadata ) {
          auto it = inputEvents.find( key );

          if( it != inputEvents.end() ) {
            for( auto callback : it->second ) {
              if( callback ) {
                callback( metadata );
              }
            }
          }

          // Bubble up the event
          std::shared_ptr< Element > element = parent->getParent();
          if( element ) {
            element->getEventBundle().trigger( key, metadata );
          }
        }

      }
    }
  }
}
