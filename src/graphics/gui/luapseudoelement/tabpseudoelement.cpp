#include "graphics/gui/luapseudoelement/tabpseudoelement.hpp"
#include "graphics/widgetbuilder.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      TabPseudoElement::TabPseudoElement( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber, Display::MainGameState& displayState ) :
        NBBinPseudoElement::NBBinPseudoElement( subject, pageNumber, displayState ) {}

      /**
       * @static
       * Create and push a new unstaged userdata for a <tab> pseudoelement. This can be disconnected/connected to pages before they are staged.
       *
       * STACK ARGS: (none)
       * Returns: userdata, or none
       */
      int TabPseudoElement::create( lua_State* L, Display::MainGameState& displayState, tinyxml2::XMLElement* element ) {
        std::shared_ptr< sfg::Widget > widget = nullptr;
        tinyxml2::XMLElement* child = element->FirstChildElement();

        if( child != NULL ) {
          try {
            WidgetBuilder widgetBuilder( displayState.instance.eventManager, displayState.getImageCache() );
            widget = widgetBuilder.getWidgetFromElementDirect( child );
          } catch( std::exception& e ) {
            Log::getInstance().error( "NBBinPseudoElement::lua_create", "Failed to add widget XML: " + std::string( e.what() ) );
            return 0;
          }
        }

        NBBinPseudoElement** userData = ( NBBinPseudoElement** ) lua_newuserdata( L, sizeof( NBBinPseudoElement* ) ); // userdata
        *userData = new TabPseudoElement( nullptr, 0, displayState );
        ( *userData )->setMetatable( L );
        ( *userData )->stagedWidget = widget;

        return 1;
      }

      /**
       * FUCKING linker won't recognize that this is already defined in NBBinPseudoElement
       */
      void TabPseudoElement::setMetatable( lua_State* L ) {
        NBBinPseudoElement::setMetatable( L );
      }

      std::string TabPseudoElement::getName() {
        return "tab";
      }

      std::shared_ptr< sfg::Widget > TabPseudoElement::getSubjectChildWidget() {
        return subject->GetNthTabLabel( pageNumber );
      }

    }
  }
}
