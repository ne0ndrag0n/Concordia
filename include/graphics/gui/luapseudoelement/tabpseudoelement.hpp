#ifndef TABPSEUDOELEMENT
#define TABPSEUDOELEMENT

#include "graphics/gui/luapseudoelement/nbbinpseudoelement.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class TabPseudoElement : public NBBinPseudoElement {

      public:
        TabPseudoElement(
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
