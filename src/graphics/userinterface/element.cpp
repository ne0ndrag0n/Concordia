#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/style/styleapplier.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"
#include <GL/glew.h>
#include <algorithm>

#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      Device::Display::Adapter::Component::GuiComponent* Element::manager = nullptr;

      Element::Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes )
        : tag( tag ), id( id ), classes( classes ), localStyle( this ), eventBundle( this ) {}

      Element::~Element() {}

      bool Element::reuseDrawableInstance() {
        if( !drawable ) {
          return false;
        }

        const auto& changedAttributes = localStyle.getChangedAttributes();
        return !( changedAttributes.count( "width" ) || changedAttributes.count( "height" ) );
      }

      bool Element::drawableDirty() {
        const auto& changedAttributes = localStyle.getChangedAttributes();

        // Not dirty if no attributes changed
        if( changedAttributes.size() == 0 ) {
          return false;
        }

        // Not dirty if only top and left changed
        if( changedAttributes.size() == 2 && changedAttributes.count( "top" ) && changedAttributes.count( "left" ) ) {
          return false;
        }

        // Not dirty if only local-z-order changed
        if( changedAttributes.size() == 1 && changedAttributes.count( "local-z-order" ) ) {
          return false;
        }

        // Odds are it's dirty
        return true;
      }

      void Element::generateDrawable() {
        if( drawableDirty() ) {
          // At least one new texture must be re-rendered
          // Check if the drawable mesh is reusable
          if( reuseDrawableInstance() ) {
            manager->getVectorRenderer().updateExistingTexture( drawable->getTexture(), [ & ]( Graphics::Vector::Renderer& r ) { render( r ); } );
          } else {
            drawable = std::make_unique< UserInterface::Drawable >(
              manager->getVectorRenderer().createTexture( glm::uvec2{ allocation[ 2 ], allocation[ 3 ] }, [ & ]( Graphics::Vector::Renderer& r ) { render( r ); } ),
              allocation[ 2 ],
              allocation[ 3 ]
            );
          }
        }
      }

      void Element::render( Graphics::Vector::Renderer& renderer ) {
        // abstract !
      }

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

      void Element::setAllocation( const glm::uvec4& allocation, bool doReflow ) {
        this->allocation = allocation;

        if( doReflow ) {
          reflow();
        }
      }

      Style::Style& Element::getPropertyList() {
        return localStyle;
      }

      Event::EventBundle& Element::getEventBundle() {
        return eventBundle;
      }

      glm::uvec2 Element::getRequisition() {
        return requisition;
      }

      glm::ivec4 Element::getAllocation() {
        return allocation;
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

      glm::ivec2 Element::getAbsolutePosition() {
        if( std::shared_ptr< Element > parent = getParent() ) {
          return glm::ivec2{
            allocation.x + parent->getAbsolutePosition().x,
            allocation.y + parent->getAbsolutePosition().y
          };
        }

        return glm::ivec2{ allocation.x, allocation.y };
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

      std::vector< std::shared_ptr< Element > > Element::getLeafNodes() {
        std::vector< std::shared_ptr< Element > > result;
        std::vector< std::shared_ptr< Element > > sortedChildren = getSortedElements();

        if( sortedChildren.size() == 0 ) {
          result.push_back( shared_from_this() );
        } else {
          for( std::shared_ptr< Element > child : sortedChildren ) {
            result = Tools::Utility::concatArrays( result, child->getLeafNodes() );
          }
        }

        return result;
      }

      void Element::reflow() {
        manager->getStyleManager().update( shared_from_this() );
        paint();
      }

      void Element::paint() {
        // Render myself, since I've already been positioned and sized
        generateDrawable();

        if( !children.empty() ) {
          positionAndSizeChildren();

          for( std::shared_ptr< Element > child : children ) {
            child->paint();
          }
        }

        // Reset changed attributes in property list
        localStyle.resetChangedAttributes();
      }

      void Element::draw() {
        glm::ivec2 absolutePosition = getAbsolutePosition();

        if( drawable ) {
          drawable->draw( absolutePosition );
        }

        std::vector< std::shared_ptr< Element > > sortedElements = getSortedElements();
        for( std::shared_ptr< Element > element : sortedElements ) {
          glScissor( absolutePosition.x, ConfigManager::getInstance().getIntValue( "viewport_y" ) - absolutePosition.y - allocation[ 3 ], allocation[ 2 ], allocation[ 3 ] );
          element->draw();
          glScissor( 0, 0, ConfigManager::getInstance().getIntValue( "viewport_x" ), ConfigManager::getInstance().getIntValue( "viewport_y" ) );
        }
      }

    }
  }
}
