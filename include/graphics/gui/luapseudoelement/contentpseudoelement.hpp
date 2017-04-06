#ifndef CONTENTPSEUDOELEMENT
#define CONTENTPSEUDOELEMENT

#include "graphics/gui/luapseudoelement/nbbinpseudoelement.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class ContentPseudoElement : public NBBinPseudoElement {

      public:
        ContentPseudoElement(
          std::shared_ptr< sfg::Notebook > subject,
          unsigned int pageNumber,
          Display::MainGameState& displayState
        );

        std::shared_ptr< sfg::Widget > getSubjectChildWidget();
        void setMetatable( lua_State* L );
        std::string getName();

        static int create( lua_State* L, Display::MainGameState& displayState, tinyxml2::XMLElement* element );

      };

    }
  }
}


#endif
