#ifndef UI_PROPERTY_LIST
#define UI_PROPERTY_LIST

#include "exceptions/genexc.hpp"
#include "log.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
#include <typeinfo>
#include <any>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      class PropertyList {
        unsigned int computedSpecificity = 0;
        std::unordered_map< std::string, std::any > values;

      public:
        EXCEPTION_TYPE( InvalidValueException, "Property is not a valid property" );
        static const PropertyList& rootPropertyList;

        PropertyList() = default;
        PropertyList( unsigned int computedSpecificity ) : computedSpecificity( computedSpecificity ) {}
        PropertyList( const std::unordered_map< std::string, std::any >& map ) : values( map ) {}
        PropertyList( unsigned int computedSpecificity, const std::unordered_map< std::string, std::any >& map ) :
          computedSpecificity( computedSpecificity ), values( map ) {}

        template < typename VariantType > void set( const std::string& key, VariantType value ) {
          values[ key ] = value;
        };

        template < typename VariantType > const VariantType get( const std::string& key ) const {
          auto it = values.find( key );
          if( it != values.end() ) {
            try {
              return std::any_cast< VariantType >( it->second );
            } catch ( const std::bad_any_cast& e ) {
              Log::getInstance().error( "PropertyList::get", key + " could not be converted to the requested type. Please check the type with propertyIsType()." );
              throw e;
            }
          }

          // If we get here, we're about to return a no-value.
          if( this != &rootPropertyList ) {
            // Try rootPropertyList before giving up
            return rootPropertyList.get< VariantType >( key );
          } else {
            // We're now in rootPropertyList and the value still wasn't found. We have no choice now but to return nullptr
            throw InvalidValueException();
          }
        };

        bool propertyIsType( const std::string& key, const std::type_info& type ) const {
          auto it = values.find( key );
          if( it != values.end() ) {
            return it->second.type() == type;
          }

          return false;
        }

        unsigned int getSpecificity() const {
          return computedSpecificity;
        };

      };

      enum class Gravity {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
      };

      enum class Requisition : int {
        AUTO = -1,
        NONE = -2,
        FILL_PARENT = -3
      };

      enum class Placement : int {
        FLOW,
        FREE
      };

      enum class Orientation : int {
        TOP,
        MIDDLE,
        BOTTOM,
        LEFT,
        RIGHT
      };

    }
  }
}


#endif
