#include "graphics/userinterface/widgets/flex_layout.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/utility.hpp"
#include "log.hpp"

namespace BlueBear::Graphics::UserInterface::Widgets {

	FlexLayout::FlexLayout( const std::string& id, const std::vector< std::string >& classes ) : Element::Element( "FlexLayout", id, classes ) {}

	std::shared_ptr< FlexLayout > FlexLayout::create( const std::string& id, const std::vector< std::string >& classes ) {
		std::shared_ptr< FlexLayout > flexLayout( new FlexLayout( id, classes ) );

		return flexLayout;
	}

	bool FlexLayout::isHorizontal() const {
		const Gravity& gravity = localStyle.get< Gravity >( "gravity" );

		switch( gravity ) {
			case Gravity::LEFT:
			case Gravity::RIGHT:
				return true;
			default:
				return false;
		}
	}

	bool FlexLayout::isForward() const {
		const Gravity& gravity = localStyle.get< Gravity >( "gravity" );

		switch( gravity ) {
			case Gravity::LEFT:
			case Gravity::TOP:
				return true;
			default:
				return false;
		}
	}

	glm::ivec2 FlexLayout::getOrigin() const {
		if( isHorizontal() ) {
			if( isForward() ) {
				return { 0, 0 };
			} else {
				return { allocation[ 2 ], 0 };
			}
		} else {
			// vertical
			if( isForward() ) {
				return { 0, 0 };
			} else {
				return { 0, allocation[ 3 ] };
			}
		}
	}

	glm::ivec2 FlexLayout::getDirection() const {
		if( isHorizontal() ) {
			if( isForward() ) {
				return { 1, 0 };
			} else {
				return { -1, 0 };
			}
		} else {
			// vertical
			if( isForward() ) {
				return { 0, 1 };
			} else {
				return { 0, -1 };
			}
		}
	}

	void FlexLayout::positionAndSizeChildren() {
		glm::ivec2 cursor = getOrigin();
		glm::ivec2 direction = getDirection();

		std::string perpendicularProperty = isHorizontal() ? "vertical-orientation" : "horizontal-orientation";
		int perpendicularIndex = isHorizontal() ? 1 : 0;

		std::vector< std::shared_ptr< Element > >::iterator start;
		std::vector< std::shared_ptr< Element > >::iterator finish;
		int step;

		if( isForward() ) {
			start = children.begin();
			finish = children.end();
			step = 1;
		} else {
			start = children.end() - 1;
			finish = children.begin();
			step = -1;
		}

		for( auto& it = start; it != finish; it += step ) {
			auto& child = *it;

			glm::ivec4 childRequisition;

			// Advance cursor by multiplying width, height by the direction

			child->setAllocation( childRequisition, false );
		}
	}

	bool FlexLayout::drawableDirty() {
		return false;
	}

	void FlexLayout::generateDrawable() {}

	void FlexLayout::calculate() {
		Element::calculate();
		requisition = { 0, 0 };

		if( isHorizontal() ) {
			for( const auto& child : children ) {
				auto finalRequisition = Utility::getFinalRequisition( child );
				requisition.x += finalRequisition.x;
				requisition.y = std::max( finalRequisition.y, requisition.y );
			}
		} else {
			for( const auto& child : children ) {
				auto finalRequisition = Utility::getFinalRequisition( child );
				requisition.x = std::max( finalRequisition.x, requisition.x );
				requisition.y += finalRequisition.y;
			}
		}
	}

}