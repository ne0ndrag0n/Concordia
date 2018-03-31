#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/style/styleapplier.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include <algorithm>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      Device::Display::Adapter::Component::GuiComponent* Element::manager = nullptr;

      Element::Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes ) : tag( tag ), id( id ), classes( classes ), localStyle( this ) {}

      Element::~Element() {}

      std::vector< std::shared_ptr< Element > > Element::getChildren() const {
        return children;
      }

      const std::string& Element::getTag() const {
        return tag;
      }

      const std::string& Element::getId() const {
        return id;
      }

      bool Element::hasClass( const std::string& clss ) const {
        for( const std::string& c : classes ) {
          if( c == clss ) {
            return true;
          }
        }

        return false;
      }

      void Element::setAllocation( const glm::uvec4& allocation ) {
        this->allocation = allocation;
        reflow();
      }

      Style::Style& Element::getPropertyList() {
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
        if( std::shared_ptr< Element > parent = getParent() ) {
          return glm::uvec2{
            allocation.x + parent->getAbsolutePosition().x,
            allocation.y + parent->getAbsolutePosition().y
          };
        }

        return glm::uvec2{ allocation.x, allocation.y };
      }

      std::shared_ptr< Element > Element::getParent() {
        return parentWeak.lock();
      }

      void Element::addChild( std::shared_ptr< Element > child ) {
        child->detach();
        children.emplace_back( child );
        child->parentWeak = shared_from_this();

        reflow();
      }

      void Element::detach() {
        if( std::shared_ptr< Element > parent = getParent() ) {
          std::shared_ptr< Element > thisElement = shared_from_this();
          parent->children.erase(
            std::remove( parent->children.begin(), parent->children.end(), thisElement ),
            parent->children.end()
          );

          parentWeak = std::weak_ptr< Element >();

          parent->reflow();
        }
      }

      void Element::reflow() {
        manager->getStyleManager().update( shared_from_this() );
        paint();
      }

      void Element::paint() {
        // Render myself, since I've already been positioned and sized
        render();

        if( !children.empty() ) {
          positionAndSizeChildren();

          for( std::shared_ptr< Element > child : children ) {
            child->paint();
          }
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
