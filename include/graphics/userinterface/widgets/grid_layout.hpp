#ifndef CONCORDIA_GUI_GRID_LAYOUT
#define CONCORDIA_GUI_GRID_LAYOUT

#include "graphics/userinterface/element.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace BlueBear::Graphics::UserInterface::Widgets {

	// 1. Addition to types.hpp
	// 2. LuaRegistrant.cpp downcast and registerWidgets
	// 3. XMLLoader updates
	class GridLayout : public Element {
	protected:
		GridLayout( const std::string& id, const std::vector< std::string >& classes );
		glm::ivec2 getGridDimensions() const;

	public:
		void positionAndSizeChildren() override;
		bool drawableDirty() override;
		void generateDrawable() override;
		void calculate() override;

		static std::shared_ptr< GridLayout > create( const std::string& id, const std::vector< std::string >& classes );
	};
}

#endif