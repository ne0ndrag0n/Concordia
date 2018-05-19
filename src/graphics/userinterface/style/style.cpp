#include "graphics/userinterface/style/style.hpp"
#include "graphics/userinterface/element.hpp"
#include "configmanager.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        Style::Animation::Animation( Style* parent, std::map< double, Keyframe > keyframes, double fps, double duration, bool suicide, bool sticky, std::function< void() > callback )
          : parent( parent ), keyframes( keyframes ), fps( fps ), duration( duration ), current( 0.0 ), suicide( suicide ), sticky( sticky ), callback( callback ) {
            for( auto& pair : keyframes ) {
              auto properties = pair.second.properties.getProperties();
              for( const std::string& prop : properties ) {
                frameChangedAttributes.insert( prop );
              }
            }
          }

        double Style::Animation::getFPS() {
          return fps / ConfigManager::getInstance().getIntValue( "fps_overview" );
        }

        bool Style::Animation::increment() {
          double next = current + getFPS();

          if( next > duration ) {
            if( suicide ) {
              if( callback ) { callback(); callback = {}; }
              parent->attachAnimation( nullptr );
              return true;
            } else if ( sticky ) {
              if( callback ) { callback(); callback = {}; }
              return false;
            } else {
              if( callback ) { callback(); }
              next = 0.0;
            }
          }

          current = next;
          return true;
        }

        std::unordered_set< std::string > Style::Animation::getChangedForFrame() {
          return frameChangedAttributes;
        }

        Style::Style( Element* parent ) : parent( parent ) {
          std::vector< std::string > properties = PropertyList::rootPropertyList.getProperties();
          for( auto& property : properties ) {
            changedAttributes.insert( property );
          }
        }

        Style::~Style() = default;

        const std::unordered_set< std::string >& Style::getChangedAttributes() {
          return changedAttributes;
        }

        void Style::resetChangedAttributes() {
          changedAttributes.clear();
        }

        void Style::reflowParent() {
          parent->reflow();
        }

        void Style::setCalculated( const std::unordered_map< std::string, PropertyListType >& map ) {
          calculated = PropertyList( map );

          for( const auto& pair : map ) {
            changedAttributes.insert( pair.first );
          }
        }

        void Style::resetProperty( const std::string& key ) {
          local.removeProperty( key );
        }

        void Style::attachAnimation( std::unique_ptr< Animation > animation ) {
          attachedAnimation = std::move( animation );
        }

        bool Style::animationAttached() {
          return !( attachedAnimation == nullptr );
        }

        void Style::updateAnimation() {
          if( attachedAnimation ) {
            changedAttributes = attachedAnimation->getChangedForFrame();

            if( attachedAnimation->increment() ) {
              reflowParent();
            }
          }
        }

      }
    }
  }
}
