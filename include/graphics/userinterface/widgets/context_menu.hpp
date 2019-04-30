#ifndef CONCORDIA_GUI_CONTEXT_MENU
#define CONCORDIA_GUI_CONTEXT_MENU

#include "graphics/userinterface/element.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <variant>
#include <vector>
#include <string>
#include <memory>

namespace BlueBear::Graphics::UserInterface::Widgets {

	class ContextMenu : public Element {
	protected:
		ContextMenu( const std::string& id, const std::vector< std::string >& classes );

	public:
		void positionAndSizeChildren() override;
		void calculate() override;
		void render( Vector::Renderer& renderer );

		static std::shared_ptr< ContextMenu > create( const std::string& id, const std::vector< std::string >& classes );
	};

}

#endif