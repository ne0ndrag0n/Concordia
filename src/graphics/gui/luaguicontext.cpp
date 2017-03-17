#include "graphics/gui/luaguicontext.hpp"
#include "graphics/gui/sfgroot.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      LuaGUIContext::LuaGUIContext( sfg::Desktop& desktop ) : rootContainer( RootContainer::Create() ), desktop( desktop ) {}

      void LuaGUIContext::add( std::shared_ptr< sfg::Widget > widget, bool toDesktop ) {
        rootContainer->Add( widget );

        if( toDesktop ) {
          desktop.Add( widget );

          addedToDesktop.insert( widget );
        }
      }

      std::shared_ptr< sfg::Widget > LuaGUIContext::findById( const std::string& id ) {
        return rootContainer->GetWidgetById( id );
      }

      std::vector< std::shared_ptr< sfg::Widget > > LuaGUIContext::findByClass( const std::string& clss ) {
        return rootContainer->GetWidgetsByClass( clss );
      }
    }
  }
}
