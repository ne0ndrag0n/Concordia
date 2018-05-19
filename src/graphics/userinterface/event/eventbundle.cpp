#include "graphics/userinterface/event/eventbundle.hpp"
#include "graphics/userinterface/element.hpp"
#include "scripting/coreengine.hpp"
#include "device/input/input.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Event {

        EventBundle::EventBundle( Element* parent ) : parent( parent ) {
          // Dump all events when Lua closes
          Scripting::CoreEngine::LUA_STATE_CLOSE.listen( this, std::bind( &EventBundle::dumpEvents, this ) );
        }

        EventBundle::~EventBundle() {
          Scripting::CoreEngine::LUA_STATE_CLOSE.stopListening( this );
        }

        void EventBundle::dumpEvents() {
          inputEvents.clear();
        }

        unsigned int EventBundle::registerInputEvent( const std::string& key, std::function< void( Device::Input::Metadata ) > callback ) {
          return insertElement( inputEvents, key, callback );
        }

        void EventBundle::unregisterInputEvent( const std::string& key, unsigned int id ) {
          removeElement( inputEvents, key, id );
        }

        void EventBundle::trigger( const std::string& key, Device::Input::Metadata metadata, bool bubble ) {
          auto it = inputEvents.find( key );

          if( it != inputEvents.end() ) {
            for( auto callback : it->second ) {
              if( callback ) {
                callback( metadata );
              }
            }
          }

          // Bubble up the event
          if( bubble ) {
            std::shared_ptr< Element > element = parent->getParent();
            if( element ) {
              element->getEventBundle().trigger( key, metadata );
            }
          }
        }

      }
    }
  }
}
