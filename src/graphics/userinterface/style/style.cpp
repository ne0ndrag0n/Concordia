#include "graphics/userinterface/style/style.hpp"
#include "graphics/userinterface/element.hpp"
#include "configmanager.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        Style::Animation::Animation( Style* parent, std::map< double, Keyframe > keyframes, double fps, double duration, bool suicide, bool sticky )
          : parent( parent ), keyframes( keyframes ), fps( fps ), duration( duration ), current( 0.0 ), suicide( suicide ), sticky( sticky ) {}

        double Style::Animation::getFPS() {
          return fps / ConfigManager::getInstance().getIntValue( "fps_overview" );
        }

        bool Style::Animation::increment() {
          double next = current + getFPS();

          if( next > duration ) {
            if( suicide ) {
              parent->attachAnimation( nullptr );
              return true;
            } else if ( sticky ) {
              return false;
            } else {
              next = 0.0;
            }
          }

          current = next;
          return true;
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

        void Style::setCalculated( const std::unordered_map< std::string, std::any >& map ) {
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

        void Style::updateAnimation() {
          if( attachedAnimation && attachedAnimation->increment() ) {
            parent->reflow();
          }
        }

      }
    }
  }
}
