#ifndef LUA_GUI_CONTEXT
#define LUA_GUI_CONTEXT

#include <SFGUI/Widgets.hpp>
#include <SFGUI/Desktop.hpp>
#include <memory>
#include <vector>
#include <set>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class LuaGUIContext {
        std::shared_ptr< sfg::Container > rootContainer;
        sfg::Desktop& desktop;
        /**
         * Bookkeeping for items we remove later on
         */
        std::set< std::shared_ptr< sfg::Widget > > addedToDesktop;

      public:
        LuaGUIContext( sfg::Desktop& desktop );

        void add( std::shared_ptr< sfg::Widget > widget, bool toDesktop = false );
        std::shared_ptr< sfg::Widget > findById( const std::string& id );
        std::vector< std::shared_ptr< sfg::Widget > > findByClass( const std::string& clss );
      };

    }
  }
}


#endif
