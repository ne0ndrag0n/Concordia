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
		for( auto& child : children ) {
			int width = child->getPropertyList().get< int >( "width" );
			int height = child->getPropertyList().get< int >( "height" );

			if( !Utility::valueIsLiteral( width ) ) {
				Log::getInstance().warn( "FixedLayout::positionAndSizeChildren", "Non-literal value given for width; defaulting to 10" );
				width = 10;
			}

			if( !Utility::valueIsLiteral( height ) ) {
				Log::getInstance().warn( "FixedLayout::positionAndSizeChildren", "Non-literal value given for height; defaulting to 10" );
				height = 10;
			}

			child->setAllocation( {
				child->getPropertyList().get< int >( "left" ),
				child->getPropertyList().get< int >( "top" ),
				width,
				height
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