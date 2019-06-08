#ifndef CONCORDIA_GUI_FLOATING_PANE
#define CONCORDIA_GUI_FLOATING_PANE

#include "graphics/userinterface/element.hpp"
#include <vector>
#include <string>
#include <memory>

namespace BlueBear::Graphics::UserInterface::Widgets {

	class FloatingPane : public Element {
	protected:
		FloatingPane( const std::string& id, const std::vector< std::string >& classes );

	public:
		void reflow( bool selectorsInvalidated ) override;
		void positionAndSizeChildren() override;
		void calculate() override;
		void render( Vector::Renderer& renderer );

		static std::shared_ptr< FloatingPane > create( const std::string& id, const std::vector< std::string >& classes );
	};

}

#endif