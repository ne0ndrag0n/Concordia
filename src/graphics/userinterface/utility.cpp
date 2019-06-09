#include "graphics/userinterface/utility.hpp"
#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/style/style.hpp"

namespace BlueBear::Graphics::UserInterface {

	bool Utility::valueIsLiteral( int r ) {
		return ( Requisition ) r != Requisition::AUTO &&
			( Requisition ) r != Requisition::NONE &&
			( Requisition ) r != Requisition::FILL_PARENT;
	}

	glm::uvec2 Utility::getFinalRequisition( const std::shared_ptr< Element >& prospect ) {
		int width = prospect->getPropertyList().get< int >( "width" );
		int height = prospect->getPropertyList().get< int >( "height" );

		return glm::uvec2{
			valueIsLiteral( width ) ? width : prospect->getRequisition().x,
			valueIsLiteral( height ) ? height : prospect->getRequisition().y
		};
	}

}