#ifndef UI_PROPERTY_LIST
#define UI_PROPERTY_LIST

#include "containers/visitor.hpp"
#include "exceptions/genexc.hpp"
#include "log.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <string>
#include <typeinfo>
#include <vector>
#include <variant>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      class Element;

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

      using PropertyListType = std::variant<
        bool,
        int,
        double,
        Gravity,
        Requisition,
        Placement,
        Orientation,
        std::string,
        glm::uvec4
      >;

      class PropertyList {
        std::unordered_map< std::string, PropertyListType > values;

      public:
        static const PropertyList& rootPropertyList;

        EXCEPTION_TYPE( InvalidValueException, "Property is not a valid property" );

        PropertyList() = default;
        PropertyList( const std::unordered_map< std::string, PropertyListType >& map ) : values( map ) {}

        std::vector< std::string > getProperties() const {
          std::vector< std::string > result;

          for( auto& pair : values ) {
            result.push_back( pair.first );
          }

          return result;
        }

        void clear() {
          values.clear();
        };

        bool keyExists( const std::string& key ) const {
          return values.find( key ) != values.end();
        };

        void removeProperty( const std::string& key ) {
          values.erase( key );
        };

        template < typename VariantType > void set( const std::string& key, VariantType value ) {
          values[ key ] = value;
        };

        template < typename VariantType > static const VariantType discriminate( PropertyListType variant ) {
          if( auto value = std::get_if< VariantType >( &variant ) ) {
            return *value;
          } else {
            throw InvalidValueException();
          }
        }

        template < typename VariantType > const VariantType get( const std::string& key ) const {
          return discriminate< VariantType >( getVariant( key ) );
        };

        PropertyListType getVariant( const std::string& key ) const {
          auto it = values.find( key );
          if( it == values.end() ) {
            throw InvalidValueException();
          }

          return it->second;
        };

        void setVariant( const std::string& key, PropertyListType value ) {
          values[ key ] = value;
        }

      };

    }
  }
}


#endif
