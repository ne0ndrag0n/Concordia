#ifndef UI_PROPERTY_LIST
#define UI_PROPERTY_LIST

#include <unordered_map>
#include <string>
#include <variant>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      class PropertyList {
        unsigned int computedSpecificity = 0;
        std::unordered_map< std::string, std::variant< bool, int, double, std::string > > values;

      public:
        PropertyList() = default;
        PropertyList( unsigned int computedSpecificity ) : computedSpecificity( computedSpecificity ) {}
        PropertyList( const std::unordered_map< std::string, std::variant< bool, int, double, std::string > >& map ) : values( map ) {}
        PropertyList( unsigned int computedSpecificity, const std::unordered_map< std::string, std::variant< bool, int, double, std::string > >& map ) :
          computedSpecificity( computedSpecificity ), values( map ) {}

        template < typename VariantType > void set( const std::string& key, VariantType value ) {
          values[ key ] = value;
        };

        template < typename VariantType > VariantType& get( const std::string& key ) {
          return std::get< VariantType >( values.at( key ) );
        };

        unsigned int getSpecificity() const {
          return computedSpecificity;
        };

      };

    }
  }
}


#endif
