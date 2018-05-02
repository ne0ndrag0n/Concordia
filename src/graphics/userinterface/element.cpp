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

      void Element::walk( std::function< void( Element& ) > predicate ) {
        predicate( *this );

        for( std::shared_ptr< Element > child : children ) {
          child->walk( predicate );
        }
      }

      bool Element::getVisible() {
        return visible;
      }

      void Element::setVisible( bool status ) {
        visible = status;

        for( std::shared_ptr< Element > child : children ) {
          child->setVisible( status );
        }
      }

      glm::ivec2 Element::toRelative( const glm::uvec2& location ) {
        auto absolute = getAbsolutePosition();
        return { location.x - absolute.x, location.y - absolute.y };
      }

      void Element::setShadow( bool status ) {
        shadow = status;
      }

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
        if( visible && drawableDirty() ) {
          // At least one new texture must be re-rendered
          manager->getVectorRenderer().setAntiAlias( localStyle.get< bool >( "antialias" ) );

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

      std::string Element::generateSelectorString() const {
        std::string result;

        result += tag;
        if( id.size() ) {
          result += "#" + id;
        }

        for( const std::string& clss : classes ) {
          result += "." + clss;
        }

        return result;
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

      void Element::sortElements() {
        std::sort( children.begin(), children.end(), []( std::shared_ptr< Element > first, std::shared_ptr< Element > second ) {
          return first->getPropertyList().get< int >( "local-z-order" ) < second->getPropertyList().get< int >( "local-z-order" );
        } );
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

      void Element::addChild( std::shared_ptr< Element > child, bool doReflow ) {
        child->detach( doReflow );
        children.emplace_back( child );
        child->parentWeak = shared_from_this();

        if( doReflow ) {
          reflow();
        }
      }

      void Element::detach( bool doReflow ) {
        if( std::shared_ptr< Element > parent = getParent() ) {
          std::shared_ptr< Element > thisElement = shared_from_this();
          parent->children.erase(
            std::remove( parent->children.begin(), parent->children.end(), thisElement ),
            parent->children.end()
          );

          parentWeak = std::weak_ptr< Element >();

          if( doReflow ) {
            parent->reflow();
          }
        }
      }

      std::vector< std::shared_ptr< Element > > Element::getLeafNodes() {
        std::vector< std::shared_ptr< Element > > result;

        if( children.size() == 0 ) {
          result.push_back( shared_from_this() );
        } else {
          for( std::shared_ptr< Element > child : children ) {
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
          sortElements();

          for( std::shared_ptr< Element > child : children ) {
            child->paint();
          }
        }

        // Reset changed attributes in property list
        localStyle.resetChangedAttributes();
      }

      glm::vec4 Element::getParentScissor() {
        float stackBuffer[ 4 ];
        glGetFloatv( GL_SCISSOR_BOX, stackBuffer );

        return { stackBuffer[ 0 ], stackBuffer[ 1 ], stackBuffer[ 2 ], stackBuffer[ 3 ] };
      }

      glm::vec4 Element::computeScissor( const glm::vec4& parentScissor, const glm::ivec2& absolutePosition ) {
        glm::vec2 parentLowerCorner = { parentScissor[ 0 ], parentScissor[ 1 ] };
        glm::vec2 parentUpperCorner = { parentLowerCorner[ 0 ] + parentScissor[ 2 ], parentLowerCorner[ 1 ] + parentScissor[ 3 ] };
        glm::vec4 scissor = {
          absolutePosition.x,
          ConfigManager::getInstance().getIntValue( "viewport_y" ) - ( absolutePosition.y + allocation[ 3 ] ),
          allocation[ 2 ],
          allocation[ 3 ]
        };

        scissor[ 0 ] = std::max( scissor.x, parentLowerCorner.x );
        scissor[ 1 ] = std::max( scissor.y, parentLowerCorner.y );

        glm::vec2 upperCorner = { scissor[ 0 ] + scissor[ 2 ], scissor[ 1 ] + scissor[ 3 ] };
        if( upperCorner.x > parentUpperCorner.x ) {
          scissor[ 2 ] = parentUpperCorner.x - scissor[ 0 ];
        }

        if( upperCorner.y > parentUpperCorner.y ) {
          scissor[ 3 ] = parentUpperCorner.y - scissor[ 1 ];
        }

        return scissor;
      }

      void Element::setScissor( const glm::vec4& scissor ) {
        glScissor( scissor[ 0 ], scissor[ 1 ], scissor[ 2 ], scissor[ 3 ] );
      }

      void Element::draw() {
        if( !visible ) {
          return;
        }

        glm::ivec2 absolutePosition = getAbsolutePosition();
        if( drawable ) {
          drawable->draw( absolutePosition );
        }

        glm::uvec4 parentScissor = getParentScissor();
        setScissor( computeScissor( parentScissor, absolutePosition ) );
        for( std::shared_ptr< Element > element : children ) {
          element->draw();
        }
        setScissor( parentScissor );
      }

    }
  }
}
