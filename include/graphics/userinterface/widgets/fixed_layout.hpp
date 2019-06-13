#ifndef GUI_FIXED_LAYOUT
#define GUI_FIXED_LAYOUT

#include "graphics/userinterface/element.hpp"
#include <vector>
#include <string>
#include <memory>

namespace BlueBear::Graphics::UserInterface::Widgets {

	class FixedLayout : public Element {
	protected:
		FixedLayout( const std::string& id, const std::vector< std::string >& classes );

	public:
		void positionAndSizeChildren() override;
		bool drawableDirty() override;
		void generateDrawable() override;
		void calculate() override;

		static std::shared_ptr< FixedLayout > create( const std::string& id, const std::vector< std::string >& classes );
	};

}


#endif