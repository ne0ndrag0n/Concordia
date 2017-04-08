#ifndef PAGEPSEUDOELEMENT
#define PAGEPSEUDOELEMENT

#include "graphics/gui/luapseudoelement/luapseudoelement.hpp"
#include "graphics/display.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <SFGUI/Widgets.hpp>
#include <memory>
#include <tinyxml2.h>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {
      class NBBinPseudoElement;

      class PagePseudoElement : public LuaPseudoElement {
        std::shared_ptr< sfg::Notebook > subject;
        unsigned int pageNumber;
        Display::MainGameState& displayState;

        NBBinPseudoElement* stagedTabElement = nullptr;
        NBBinPseudoElement* stagedContentElement = nullptr;

        void processXMLPseudoElement( lua_State* L, tinyxml2::XMLElement* child );

        void setStagedTabElement( lua_State* L, NBBinPseudoElement* stagedTabElement );
        void setStagedContentElement( lua_State* L, NBBinPseudoElement* stagedContentElement );

        void setStagedChild( lua_State* L, const std::string& xml );
        bool findElement( lua_State* L, const std::string& tag );
        int findElementStaged( lua_State* L, const std::string& tag );

        int getElementsByClass( lua_State* L, const std::string& classID );
        int getElementById( lua_State* L, const std::string& id );

        template < typename T > NBBinPseudoElement* getPseudoElement( std::unique_ptr< NBBinPseudoElement >& holdingPtr, NBBinPseudoElement* staged ) {
          if( subject ) {
            holdingPtr = std::make_unique< T >( subject, pageNumber, displayState );
            return holdingPtr.get();
          }

          // Will just return nullptr if either stagedTabElement or stagedContentElement are nullptr
          return staged;
        }

      public:
        PagePseudoElement(
          std::shared_ptr< sfg::Notebook > subject,
          unsigned int pageNumber,
          Display::MainGameState& displayState
        );

        void setMetatable( lua_State* L );
        std::string getName();

        bool setSubject( lua_State* L, std::shared_ptr< sfg::Notebook > notebook );

        static int create( lua_State* L, Display::MainGameState& displayState, tinyxml2::XMLElement* element );

        static int lua_add( lua_State* L );
        static int lua_removeWidget( lua_State* L );
        static int lua_getName( lua_State* L );
        static int lua_findById( lua_State* L );
        static int lua_findByClass( lua_State* L );
        static int lua_findElement( lua_State* L );
        static int lua_getProperty( lua_State* L );
        static int lua_setProperty( lua_State* L );
        static int lua_getContent( lua_State* L );
        static int lua_setContent( lua_State* L );
        static int lua_gc( lua_State* L );
      };

    }
  }
}


#endif
