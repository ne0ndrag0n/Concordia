#include "graphics/userinterface/widgets/layout.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include <algorithm>
#include <functional>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Widgets {

        Layout::Layout( const std::string& id, const std::vector< std::string >& classes ) : Element::Element( "Layout", id, classes ) {}

        std::shared_ptr< Layout > Layout::create( const std::string& id, const std::vector< std::string >& classes ) {
          std::shared_ptr< Layout > layout( new Layout( id, classes ) );

          return layout;
        }

        void Layout::calculate() {
          glm::ivec2 total{ localStyle.get< int >( "width" ), localStyle.get< int >( "height" ) };
          int padding = localStyle.get< int >( "padding" );
          Gravity gravity = localStyle.get< Gravity >( "gravity" );
          bool horizontal = ( gravity == Gravity::LEFT || gravity == Gravity::RIGHT );

          // Do all children first
          std::vector< glm::uvec2 > requisitions;
          for( std::shared_ptr< Element > child : children ) {
            child->calculate();
            requisitions.push_back( child->getRequisition() );
          }

          if( !valueIsLiteral( total.x ) ) {
            if( horizontal ) {
              total.x = padding;
              for( glm::uvec2& requisition : requisitions ) {
                total.x += requisition.x + padding;
              }
            } else {
              unsigned int maxWidth = 0;
              for( glm::uvec2& requisition : requisitions ) {
                maxWidth = std::max( maxWidth, requisition.x );
              }
              total.x = maxWidth + ( padding * 2 );
            }
          }

          if( !valueIsLiteral( total.y ) ) {
            if( horizontal ) {
              unsigned maxHeight = 0;
              for( glm::uvec2& requisition : requisitions ) {
                maxHeight = std::max( maxHeight, requisition.y );
              }
              total.y = maxHeight + ( padding * 2 );
            } else {
              total.y = padding;
              for( glm::uvec2& requisition : requisitions ) {
                total.y += requisition.y + padding;
              }
            }
          }

          requisition = bindCalculations( total );
        }

        void Layout::render( Device::Display::Adapter::Component::GuiComponent& manager ) {
          // crazy nanovg shit goes here
        }

        Layout::Relations Layout::getRelations( Gravity gravity, int padding ) {
          bool xAxis = ( gravity == Gravity::LEFT || gravity == Gravity::RIGHT );

          int totalSpace = allocation[ xAxis ? 2 : 3 ] - ( 2 * padding );
          int totalWeight = 0;
          for( std::shared_ptr< Element > child : children ) {
            if( child->getPropertyList().get< Placement >( "placement" ) == Placement::FLOW ) {
              int layoutWeight = child->getPropertyList().get< int >( "layout-weight" );
              if( layoutWeight >= 1 ) {
                // This child will be sized by its layout proportion
                totalWeight += layoutWeight;
              } else {
                // This child will be sized using its requisition
                totalSpace -= xAxis ? child->getRequisition().x : child->getRequisition().y;
              }
            }
          }

          return Relations {
            xAxis ? "vertical-orientation" : "horizontal-orientation",
            xAxis ? "width" : "height",
            xAxis ? "height" : "width",
            padding,

            //int rFlowSize;
            xAxis ? 0 : 1,
            //int rPerpSize;
            xAxis ? 1 : 0,

            //int aFlowPos;
            xAxis ? 0 : 1,
            //int aPerpPos;
            xAxis ? 1 : 0,
            //int aFlowSize;
            xAxis ? 2 : 3,
            //int aPerpSize;
            xAxis ? 3 : 2,

            totalSpace,
            totalWeight,
            ( xAxis ? ( int ) allocation[ 3 ] : ( int ) allocation [ 2 ] ) - ( padding * 2 )
          };
        }

        /**
         * Boudnaries already defined by parent element
         */
        void Layout::positionAndSizeChildren() {
          int padding = localStyle.get< int >( "padding" );
          Gravity gravity = localStyle.get< Gravity >( "gravity" );

          Layout::Relations relations = getRelations( gravity, padding );
          for( std::shared_ptr< Element > child : children ) {
            glm::uvec4 childAllocation;

            if( child->getPropertyList().get< Placement >( "placement" ) == Placement::FLOW ) {
              // flow size - either a proportion derived from layout-weight or the requisition size
              int layoutWeight = child->getPropertyList().get< int >( "layout-weight" );
              childAllocation[ relations.aFlowSize ] = ( layoutWeight >= 1 ) ?
                ( ( layoutWeight / relations.flowTotalWeight ) * relations.flowTotalSpace ) :
                child->getRequisition()[ relations.rFlowSize ];

              // perp size - fill parent or use requisition
              childAllocation[ relations.aPerpSize ] = ( ( Requisition ) child->getPropertyList().get< int >( relations.perpProperty ) == Requisition::FILL_PARENT ) ?
                relations.perpSizeAdjusted :
                child->getRequisition()[ relations.rPerpSize ];

              // flow position - will just be current position of the cursor. However, you need to increment the cursor by the above calculated flow dimension
              childAllocation[ relations.aFlowPos ] = relations.cursor;
              relations.cursor += ( childAllocation[ relations.aFlowSize ] + padding );

              // perp position - this is the one determined by "orientation"
              Orientation orientation = child->getPropertyList().get< Orientation >( relations.orientation );
              if( orientation == Orientation::MIDDLE ) {
                childAllocation[ relations.aPerpPos ] = ( ( relations.perpSizeAdjusted / 2 ) + padding ) - ( childAllocation[ relations.aPerpSize ] / 2 );
              } else if( orientation == Orientation::BOTTOM || orientation == Orientation::RIGHT ) {
                childAllocation[ relations.aPerpPos ] = allocation[ relations.aPerpSize ] - padding - childAllocation[ relations.aPerpSize ];
              } else {
                // left, top, as well as default
                childAllocation[ relations.aPerpPos ] = padding;
              }
            } else {
              // This element breaks the flow - use its settings directly
              int left = child->getPropertyList().get< int >( "left" );
              int top = child->getPropertyList().get< int >( "top" );

              if( !valueIsLiteral( left ) ) { left = 0; }
              if( !valueIsLiteral( top ) ) { top = 0; }

              childAllocation[ 0 ] = left;
              childAllocation[ 1 ] = top;
              childAllocation[ 2 ] = child->getRequisition().x;
              childAllocation[ 3 ] = child->getRequisition().y;
            }

            // That's everything: compute the allocation
            child->setAllocation( childAllocation );
          }
        }

      }
    }
  }
}
