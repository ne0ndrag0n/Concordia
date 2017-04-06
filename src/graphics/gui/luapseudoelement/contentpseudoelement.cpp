#include "graphics/gui/luapseudoelement/contentpseudoelement.hpp"
#include "graphics/widgetbuilder.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      ContentPseudoElement::ContentPseudoElement( std::shared_ptr< sfg::Notebook > subject, unsigned int pageNumber, Display::MainGameState& displayState ) :
        NBBinPseudoElement::NBBinPseudoElement( subject, pageNumber, displayState ) {}

      /**
       * @static
       * Create and push a new unstaged userdata for a <tab> pseudoelement. This can be disconnected/connected to pages before they are staged.
       *
       * STACK ARGS: (none)
       * Returns: userdata, or none
       */
      int ContentPseudoElement::create( lua_State* L, Display::MainGameState& displayState, tinyxml2::XMLElement* element ) {
        NBBinPseudoElement** userData = ( NBBinPseudoElement** ) lua_newuserdata( L, sizeof( NBBinPseudoElement* ) ); // userdata
        *userData = new ContentPseudoElement( nullptr, 0, displayState );
        ( *userData )->setMetatable( L );
        ( *userData )->stagedWidget = NBBinPseudoElement::createStagedWidget( displayState, element );

        return 1;
      }

      /**
       * FUCKING linker won't recognize that this is already defined in NBBinPseudoElement
       */
      void ContentPseudoElement::setMetatable( lua_State* L ) {
        NBBinPseudoElement::setMetatable( L );
      }

      std::string ContentPseudoElement::getName() {
        return "content";
      }

      std::shared_ptr< sfg::Widget > ContentPseudoElement::getSubjectChildWidget() {
        return subject->GetNthPage( pageNumber );
      }

    }
  }
}
