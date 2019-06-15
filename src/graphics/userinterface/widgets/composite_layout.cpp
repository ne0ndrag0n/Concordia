#include "graphics/userinterface/widgets/composite_layout.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/utility.hpp"
#include "log.hpp"

namespace BlueBear::Graphics::UserInterface::Widgets {

	CompositeLayout::CompositeLayout( const std::string& id, const std::vector< std::string >& classes ) : Element::Element( "CompositeLayout", id, classes ) {}

	std::shared_ptr< CompositeLayout > CompositeLayout::create( const std::string& id, const std::vector< std::string >& classes ) {
		std::shared_ptr< CompositeLayout > compositeLayout( new CompositeLayout( id, classes ) );

		return compositeLayout;
	}

	void CompositeLayout::positionAndSizeChildren() {
		sortElements();

		for( auto& child : children ) {
			child->setAllocation( { 0, 0, allocation[ 2 ], allocation[ 3 ] }, false );
		}
	}

	bool CompositeLayout::drawableDirty() {
		return false;
	}

	void CompositeLayout::generateDrawable() {}

	void CompositeLayout::calculate() {
		requisition = { 0, 0 };
		Element::calculate();
	}

}