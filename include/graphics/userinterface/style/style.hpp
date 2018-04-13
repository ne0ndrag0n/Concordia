#ifndef STYLE_ELEMENT
#define STYLE_ELEMENT

#include "graphics/userinterface/propertylist.hpp"
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Style {

        class Style {
          PropertyList calculated;
          PropertyList local;
          Element* parent;
          std::unordered_set< std::string > changedAttributes;

        public:
          Style( Element* parent );

          const std::unordered_set< std::string >& getChangedAttributes();
          void resetChangedAttributes();
          void reflowParent();
          void setCalculated( const std::unordered_map< std::string, std::any >& map );

          template < typename VariantType > const VariantType get( const std::string& key ) const {
            if( local.keyExists( key ) ) {
              return local.get< VariantType >( key );
            } else if( calculated.keyExists( key ) ) {
              return calculated.get< VariantType >( key );
            } else {
              return PropertyList::rootPropertyList.get< VariantType >( key );
            }
          };

          template < typename VariantType > void set( const std::string& key, VariantType value, bool reflow = true ) {
            local.set< VariantType >( key, value );
            changedAttributes.insert( key );

            if( reflow ) {
              reflowParent();
            }
          };
        };

      }
    }
  }
}

#endif
