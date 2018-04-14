#include "graphics/userinterface/style/style.hpp"
#include "graphics/userinterface/element.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        Style::Style( Element* parent ) : parent( parent ) {
          std::vector< std::string > properties = PropertyList::rootPropertyList.getProperties();
          for( auto& property : properties ) {
            changedAttributes.insert( property );
          }
        }

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

      }
    }
  }
}
