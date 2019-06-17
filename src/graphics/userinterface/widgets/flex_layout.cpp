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

	glm::ivec2 FlexLayout::getDirection() const {
		const Gravity& gravity = localStyle.get< Gravity >( "gravity" );

		switch( gravity ) {
			case Gravity::LEFT:
				return { 1, 0 };
			case Gravity::RIGHT:
				return { -1, 0 };
			case Gravity::TOP:
				return { 0, 1 };
			case Gravity::BOTTOM:
				return { 0, -1 };
		}
	}

	glm::ivec2 FlexLayout::getLiteralSize( const std::shared_ptr< Element >& child ) const {
		glm::ivec2 params{
			std::min( allocation[ 2 ], child->getPropertyList().get< int >( "width" ) ),
			std::min( allocation[ 3 ], child->getPropertyList().get< int >( "height" ) )
		};

		if( !Utility::valueIsLiteral( params.x ) ) {
			Requisition symbol = ( Requisition ) params.x;
			switch( symbol ) {
				case Requisition::NONE:
					Log::getInstance().warn( "FlexLayout::getLiteralValue", "Requisition::NONE is not a valid requisition constant for children of FlexLayout; defaulting to AUTO." );
				default:
				case Requisition::AUTO: {
					params.x = child->getRequisition().x;
					break;
				}
				case Requisition::FILL_PARENT: {
					if( isHorizontal() ) {
						Log::getInstance().warn( "FlexLayout::getLiteralValue", "Requisition::FILL_PARENT is not valid for width when flow is horizontal; defaulting to AUTO." );
						params.x = child->getRequisition().x;
					} else {
						params.x = allocation[ 2 ];
					}
				}
			}
		}

		if( !Utility::valueIsLiteral( params.y ) ) {
			Requisition symbol = ( Requisition ) params.y;
			switch( symbol ) {
				case Requisition::NONE:
					Log::getInstance().warn( "FlexLayout::getLiteralValue", "Requisition::NONE is not a valid requisition constant for children of FlexLayout; defaulting to AUTO." );
				default:
				case Requisition::AUTO: {
					params.y = child->getRequisition().y;
					break;
				}
				case Requisition::FILL_PARENT: {
					if( isHorizontal() ) {
						params.y = allocation[ 3 ];
					} else {
						Log::getInstance().warn( "FlexLayout::getLiteralValue", "Requisition::FILL_PARENT is not valid for height when flow is vertical; defaulting to AUTO." );
						params.y = child->getRequisition().y;
					}
				}
			}
		}
	}

	glm::ivec2 FlexLayout::getElementsSize() const {
		glm::ivec2 totalSize;

		for( const auto& child : children ) {
			glm::ivec2 size = getLiteralSize( child );
			if( isHorizontal() ) {
				totalSize.x += size.x;
				totalSize.y = std::max( totalSize.y, size.y );
			} else {
				totalSize.x = std::max( totalSize.x, size.x );
				totalSize.y += size.y;
			}
		}

		return totalSize;
	}

	glm::ivec2 FlexLayout::getOrigin() const {
		const Orientation vertical = localStyle.get< Orientation >( "vertical-orientation" );
		const Orientation horizontal = localStyle.get< Orientation >( "horizontal-orientation" );

		glm::ivec2 origin;

		switch( vertical ) {
			default:
				Log::getInstance().warn( "FlexLayout::getOrigin", "Invalid value specified for vertical-orientation; defaulting to Orientation::TOP" );
			case Orientation::TOP: {
				origin.y = 0;
				break;
			}
			case Orientation::MIDDLE: {
				origin.y = allocation[ 3 ] / 2;
				break;
			}
			case Orientation::BOTTOM: {
				origin.x = allocation[ 3 ];
				break;
			}
		}

		switch( horizontal ) {
			default:
				Log::getInstance().warn( "FlexLayout::getOrigin", "Invalid value specified for horizontal-orientation; defaulting to Orientation::LEFT" );
			case Orientation::LEFT: {
				origin.x = 0;
				break;
			}
			case Orientation::MIDDLE: {
				origin.x = allocation[ 2 ] / 2;
				break;
			}
			case Orientation::RIGHT: {
				origin.x = allocation[ 2 ];
				break;
			}
		}

		return origin;
	}

	void FlexLayout::positionAndSizeChildren() {
		glm::ivec2 direction = getDirection();
		glm::ivec2 cursor = getOrigin();
		glm::vec2 adjustment{
			( float ) cursor.x / ( float ) allocation[ 2 ],
			( float ) cursor.y / ( float ) allocation[ 3 ]
		};

		int start, end, step;
		if( isForward() ) {
			step = 1;
			start = 0;
			end = children.size();
		} else {
			step = -1;
			start = children.size() - 1;
			end = -1;
		}

		for( int i = start; i != end; i += step ) {
			auto& child = children[ i ];
			auto finalRequisition = getLiteralSize( child );
			glm::ivec4 childRequisition;

			// TODO
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