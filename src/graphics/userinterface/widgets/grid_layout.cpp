#include "graphics/userinterface/widgets/grid_layout.hpp"
#include "graphics/userinterface/utility.hpp"

namespace BlueBear::Graphics::UserInterface::Widgets {

	GridLayout::GridLayout( const std::string& id, const std::vector< std::string >& classes )
		: Element::Element( "GridLayout", id, classes ) {}

	std::shared_ptr< GridLayout > GridLayout::create( const std::string& id, const std::vector< std::string >& classes ) {
		std::shared_ptr< GridLayout > gridLayout( new GridLayout( id, classes ) );

		return gridLayout;
	}

	glm::ivec2 GridLayout::getGridDimensions() const {
		LayoutProportions columns = localStyle.get< LayoutProportions >( "grid-columns" );
		LayoutProportions rows    = localStyle.get< LayoutProportions >( "grid-rows" );

		int totalColumns = 0;
		int totalRows = 0;

		for( const auto& column : columns ) { totalColumns += column; }
		for( const auto& row : rows ) { totalRows += row; }

		return { totalColumns, totalRows };
	}

	// Heavy TODO
	void GridLayout::positionAndSizeChildren() {
		glm::ivec2 gridDimensions = getGridDimensions();
		LayoutProportions columns = localStyle.get< LayoutProportions >( "grid-columns" );
		LayoutProportions rows = localStyle.get< LayoutProportions >( "grid-rows" );

		std::vector< int > columnSizes;
		for( int columnSize : columns ) {
			float factor = ( float ) columnSize / ( float ) gridDimensions.x;
			columnSizes.emplace_back( std::round( ( float ) allocation[ 2 ] * factor ) );
		}

		std::vector< int > rowSizes;
		for( int rowSize : rows ) {
			float factor = ( float ) rowSize / ( float ) gridDimensions.y;
			rowSizes.emplace_back( std::round( ( float ) allocation[ 3 ] * factor ) );
		}

		auto it = children.begin();
		for( int y = 0; y != rowSizes.size(); y++ ) {
			for( int x = 0; x != columnSizes.size(); x++ ) {
				if( it == children.end() ) {
					Log::getInstance().warn( "GridLayout::positionAndSizeChildren", "Out of children to place in grid" );
					return;
				}

				// TODO: vertical-orientation/horizontal-orientation on grid elements
				glm::ivec2 childPosition{ 0, 0 };
				for( int ySub = 0; ySub != y; ySub++ ) {
					childPosition.y += rowSizes[ ySub ];
				}
				for( int xSub = 0; xSub != x; xSub++ ) {
					childPosition.x += columnSizes[ xSub ];
				}

				( *it )->setAllocation( { childPosition.x, childPosition.y, columnSizes[ x ], rowSizes[ y ] }, false );

				++it;
			}
		}
	}

	/**
	 * No drawable will be rendered.
	 */
	bool GridLayout::drawableDirty() {
		return false;
	}

	/**
	 * No drawable will be rendered.
	 */
	void GridLayout::generateDrawable() {}

	void GridLayout::calculate() {
		glm::ivec2 dimensions = getGridDimensions();
		glm::uvec2 maxRequired{ 0, 0 };

		for( auto& child : children ) {
			child->calculate();
			glm::uvec2 requisition = Utility::getFinalRequisition( child );

			maxRequired.x = std::max( maxRequired.x, requisition.x );
			maxRequired.y = std::max( maxRequired.y, requisition.y );
		}

		requisition = {
			dimensions.x * maxRequired.x,
			dimensions.y * maxRequired.y
		};
	}

}