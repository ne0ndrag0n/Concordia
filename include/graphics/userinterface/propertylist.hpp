#ifndef UI_PROPERTY_LIST
#define UI_PROPERTY_LIST

#include "exceptions/genexc.hpp"
#include "log.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include <typeinfo>
#include <any>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      class Element;

      class PropertyList {
        std::unordered_map< std::string, std::any > values;

      public:
        static const PropertyList& rootPropertyList;

        EXCEPTION_TYPE( InvalidValueException, "Property is not a valid property" );

        PropertyList() = default;
        PropertyList( const std::unordered_map< std::string, std::any >& map ) : values( map ) {}

        void clear() {
          values.clear();
        };

        bool keyExists( const std::string& key ) const {
          return values.find( key ) != values.end();
        };

        template < typename VariantType > void set( const std::string& key, VariantType value ) {
          values[ key ] = value;
        };

        template < typename VariantType > const VariantType get( const std::string& key ) const {
          if( keyExists( key ) ) {
            try {
              return std::any_cast< VariantType >( values.find( key )->second );
            } catch ( const std::bad_any_cast& e ) {
              Log::getInstance().error( "PropertyList::get", key + " could not be converted to the requested type." );
              throw e;
            }
          }

          throw InvalidValueException();
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
