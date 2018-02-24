#include "graphics/userinterface/widgets/layout.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include <algorithm>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Widgets {

        Layout::Layout( const std::string& id, const std::vector< std::string >& classes ) : Element::Element( "Layout", id, classes ) {}

        std::shared_ptr< Layout > Layout::create( const std::string& id, const std::vector< std::string >& classes ) {
          std::shared_ptr< Layout > layout( new Layout( id, classes ) );

          return layout;
        }

        bool Layout::valueIsLiteral( int r ) {
          return ( Requisition ) r != Requisition::AUTO &&
            ( Requisition ) r != Requisition::NONE &&
            ( Requisition ) r != Requisition::FIT_PARENT;
        }

        glm::uvec2 Layout::getRequestedSize() {
          glm::ivec2 total{ localStyle.get< int >( "width" ), localStyle.get< int >( "height" ) };
          int padding = localStyle.get< int >( "padding" );
          Gravity gravity = localStyle.get< Gravity >( "gravity" );
          bool horizontal = ( gravity == Gravity::LEFT || gravity == Gravity::RIGHT );

          std::vector< glm::uvec2 > requisitions;
          for( std::shared_ptr< Element > child : children ) {
            requisitions.push_back( child->getRequestedSize() );
          }

          if( !valueIsLiteral( total.x ) ) {
            if( horizontal ) {
              total.x = padding * ( children.size() + 1 );
              for( glm::uvec2& requisition : requisitions ) {
                total.x += requisition.x;
              }
            } else {
              total.x = 0;
              for( glm::uvec2& requisition : requisitions ) {
                total.x = std::max( ( unsigned int ) total.x, requisition.x );
              }
              total.x += padding * 2;
            }
          }

          if( !valueIsLiteral( total.y ) ) {
            if( horizontal ) {
              total.y = 0;
              for( glm::uvec2& requisition : requisitions ) {
                total.y = std::max( ( unsigned int ) total.y, requisition.y );
              }
              total.y += padding * 2;
            } else {
              total.y = padding * ( children.size() + 1 );
              for( glm::uvec2& requisition : requisitions ) {
                total.y += requisition.y;
              }
            }
          }

          // Bound by min-height and max-height
          glm::ivec2 minima{ localStyle.get< int >( "min-width" ), localStyle.get< int >( "min-height" ) };
          glm::ivec2 maxima{ localStyle.get< int >( "max-width" ), localStyle.get< int >( "max-height" ) };

          if( valueIsLiteral( minima.x ) ) { total.x = std::max( minima.x, total.x ); }
          if( valueIsLiteral( minima.y ) ) { total.y = std::max( minima.y, total.y ); }

          if( valueIsLiteral( maxima.x ) ) { total.x = std::min( total.x, maxima.x ); }
          if( valueIsLiteral( maxima.y ) ) { total.x = std::min( total.y, maxima.y ); }

          return total;
        }

        void Layout::render( Graphics::Vector::Renderer& renderer ) {
          // crazy nanovg shit goes here
        }

        /**
         * Boudnaries already defined by parent element
         */
        void Layout::positionAndSizeChildren() {

        }

      }
    }
  }
}
