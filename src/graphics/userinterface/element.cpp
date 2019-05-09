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

      std::vector< std::shared_ptr< Element > > Element::getElementsByTag( const std::string& tag ) {
        std::vector< std::shared_ptr< Element > > result;

        for( std::shared_ptr< Element > child : children ) {
          if( child->getTag() == tag ) {
            result.push_back( child );
          }

          result = Tools::Utility::concatArrays( result, child->getElementsByTag( tag ) );
        }

        return result;
      }

      std::vector< std::shared_ptr< Element > > Element::getElementsByClass( const std::vector< std::string >& classes ) {
        std::vector< std::shared_ptr< Element > > result;

        for( std::shared_ptr< Element > child : children ) {
          bool add = false;
          for( const std::string& clas : classes ) {
            add = child->hasClass( clas );
            if( !add ) {
              break;
            }
          }

          if( add ) {
            result.push_back( child );
          }

          result = Tools::Utility::concatArrays( result, child->getElementsByClass( classes ) );
        }

        return result;
      }

      std::shared_ptr< Element > Element::getElementById( const std::string& id ) {
        std::shared_ptr< Element > result = nullptr;

        for( std::shared_ptr< Element > child : children ) {
          if( child->getId() == id ) {
            result = child;
          } else {
            result = child->getElementById( id );
          }

          if( result ) {
            break;
          }
        }

        return result;
      }

      void Element::walk( const std::function< void( Element& ) >& predicate ) {
        predicate( *this );

        for( std::shared_ptr< Element >& child : children ) {
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

      int Element::getLocalZOrder() {
        return localZOrder;
      }

      void Element::setLocalZOrder( int zOrder ) {
        localZOrder = zOrder;
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

      void Element::setAllocation( const glm::ivec4& allocation, bool doReflow ) {
        this->allocation = allocation;

        if( doReflow ) {
          reflow();
        }
      }

      Style::Style& Element::getPropertyList() {
        return localStyle;
      }

      const Style::Style& Element::getPropertyList() const {
        return localStyle;
      }

      Event::EventBundle& Element::getEventBundle() {
        return eventBundle;
      }

      glm::uvec2 Element::getRequisition() const {
        return requisition;
      }

      glm::ivec4 Element::getAllocation() const {
        return allocation;
      }

      void Element::positionAndSizeChildren() {
        // abstract !!
      }

      void Element::sortElements() {
        std::stable_sort( children.begin(), children.end(), []( std::shared_ptr< Element > first, std::shared_ptr< Element > second ) {
          return first->getLocalZOrder() < second->getLocalZOrder();
        } );
      }

      bool Element::valueIsLiteral( int r ) const {
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

      void Element::remove( const std::vector< std::shared_ptr< Element > >& elements, bool doReflow ) {
        for( const auto& element : elements ) {
          children.erase(
            std::remove( children.begin(), children.end(), element ),
            children.end()
          );

          element->parentWeak = std::weak_ptr< Element >();
        }

        if( doReflow ) {
          reflow();
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

      void Element::setChildrenZOrder() {
        for( std::shared_ptr< Element > child : children ) {
          child->setLocalZOrder( child->getPropertyList().get< int >( "local-z-order" ) );
        }
      }

      void Element::reflow( bool selectorsInvalidated ) {
        if( selectorsInvalidated ) {
          manager->getStyleManager().update( shared_from_this() );
        }

        paint();
      }

      void Element::paint() {
        // Render myself, since I've already been positioned and sized
        generateDrawable();

        if( !children.empty() ) {
          positionAndSizeChildren();
          setChildrenZOrder();
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
        static int viewportY = ConfigManager::getInstance().getIntValue( "viewport_y" );

        glm::vec2 parentLowerCorner = { parentScissor.x, parentScissor.y };
        glm::vec2 parentUpperCorner = { parentLowerCorner.x + parentScissor.z, parentLowerCorner.y + parentScissor.w };

        int lowerLeftCornerY = absolutePosition.y + allocation.w;
        glm::vec4 scissor = {
          absolutePosition.x,
          viewportY - ( absolutePosition.y + allocation.w ),
          std::max( 0, allocation.z - ( absolutePosition.x < 0 ? std::abs( absolutePosition.x ) : 0 ) ),
          // TODO: Touch up Y case. Scissor Y height cannot be greater than viewportY
          // Hint: starts at the bottom corner as opengl's coordinate system is a pita
          std::max( 0, allocation.w - ( lowerLeftCornerY > viewportY ? ( lowerLeftCornerY - viewportY ) : 0 ) )
        };

        scissor.x = std::max( scissor.x, parentLowerCorner.x );
        scissor.y = std::max( scissor.y, parentLowerCorner.y );

        glm::vec2 upperCorner = { scissor.x + scissor.z, scissor.y + scissor.w };
        if( upperCorner.x > parentUpperCorner.x ) {
          scissor.z = parentUpperCorner.x - scissor.x;
        }

        if( upperCorner.y > parentUpperCorner.y ) {
          scissor.w = parentUpperCorner.y - scissor.y;
        }

        return scissor;
      }

      void Element::setScissor( const glm::vec4& scissor ) {
        glScissor( scissor.x, scissor.y, scissor.z, scissor.w );
      }

      void Element::draw( const Graphics::Shader& guiShader, glm::ivec2 parentAllocation ) {
        if( !visible ) {
          return;
        }

        glm::ivec2 absolutePosition = parentAllocation + glm::ivec2{ allocation.x, allocation.y };
        if( drawable ) {
          drawable->draw( guiShader, absolutePosition );
        }

        glm::vec4 parentScissor = getParentScissor();
        setScissor( computeScissor( parentScissor, absolutePosition ) );
        for( std::shared_ptr< Element > element : children ) {
          element->draw( guiShader, absolutePosition );
        }
        setScissor( parentScissor );
      }

    }
  }
}
