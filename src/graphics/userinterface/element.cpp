#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include <algorithm>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      Element::Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes ) : tag( tag ), id( id ), classes( classes ) {}

      Element::~Element() {}

      void Element::setAllocation( const glm::uvec4& allocation ) {
        this->allocation = allocation;
      }

      PropertyList& Element::getPropertyList() {
        return localStyle;
      }

      glm::uvec2 Element::getRequisition() {
        return requisition;
      }

      void Element::positionAndSizeChildren() {
        // abstract !!
      }

      std::vector< std::shared_ptr< Element > > Element::getSortedElements() {
        std::vector< std::shared_ptr< Element > > sorted;
        std::vector< std::shared_ptr< Element > > freeItems;

        for( std::shared_ptr< Element > child : children ) {
          if( child->getPropertyList().get< Placement >( "placement" ) == Placement::FREE ) {
            freeItems.push_back( child );
          } else {
            sorted.push_back( child );
          }
        }

        std::sort( sorted.begin(), sorted.end(), []( std::shared_ptr< Element > first, std::shared_ptr< Element > second ) {
          return first->getPropertyList().get< int >( "local-z-order" ) < second->getPropertyList().get< int >( "local-z-order" );
        } );

        std::sort( freeItems.begin(), freeItems.end(), []( std::shared_ptr< Element > first, std::shared_ptr< Element > second ) {
          return first->getPropertyList().get< int >( "local-z-order" ) < second->getPropertyList().get< int >( "local-z-order" );
        } );

        for( std::shared_ptr< Element > freeChild : freeItems ) {
          sorted.push_back( freeChild );
        }

        return sorted;
      }

      bool Element::valueIsLiteral( int r ) {
        return ( Requisition ) r != Requisition::AUTO &&
          ( Requisition ) r != Requisition::NONE &&
          ( Requisition ) r != Requisition::FILL_PARENT;
      }

      glm::uvec2 Element::getAbsolutePosition() {
        if( std::shared_ptr< Element > parent = parentWeak.lock() ) {
          return glm::uvec2{
            allocation.x + parent->getAbsolutePosition().x,
            allocation.y + parent->getAbsolutePosition().y
          };
        }

        return glm::uvec2{ allocation.x, allocation.y };
      }

      void Element::reflow( Device::Display::Adapter::Component::GuiComponent& manager ) {
        // Render myself, since I've already been positioned and sized
        render( manager );
        positionAndSizeChildren();

        for( std::shared_ptr< Element > child : children ) {
          child->reflow( manager );
        }
      }

      void Element::draw() {
        if( drawable ) {
          drawable->draw();
        }

        std::vector< std::shared_ptr< Element > > sortedElements = getSortedElements();
        for( std::shared_ptr< Element > element : sortedElements ) {
          element->draw();
        }
      }

    }
  }
}
