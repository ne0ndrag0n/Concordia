#include "graphics/userinterface/widgets/fixed_layout.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/utility.hpp"
#include "log.hpp"

namespace BlueBear::Graphics::UserInterface::Widgets {

	FixedLayout::FixedLayout( const std::string& id, const std::vector< std::string >& classes ) : Element::Element( "FixedLayout", id, classes ) {}

	std::shared_ptr< FixedLayout > FixedLayout::create( const std::string& id, const std::vector< std::string >& classes ) {
		std::shared_ptr< FixedLayout > fixedLayout( new FixedLayout( id, classes ) );

		return fixedLayout;
	}

	void FixedLayout::positionAndSizeChildren() {
		if( getParent() == nullptr ) {
			calculate();
		}

		for( auto& child : children ) {
			glm::uvec2 finalRequisition = Utility::getFinalRequisition( child );

			child->setAllocation( {
				child->getPropertyList().get< int >( "left" ),
				child->getPropertyList().get< int >( "top" ),
				finalRequisition.x,
				finalRequisition.y
			}, false );
		}
	}

	bool FixedLayout::drawableDirty() {
		return false;
	}

	void FixedLayout::generateDrawable() {}

	void FixedLayout::calculate() {
		requisition = { 0, 0 };
		Element::calculate();
	}

}