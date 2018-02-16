#ifndef UI_PROPERTY_LIST
#define UI_PROPERTY_LIST

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
        PropertyList() = default;
        PropertyList( unsigned int computedSpecificity ) : computedSpecificity( computedSpecificity ) {}
        PropertyList( const std::unordered_map< std::string, std::any >& map ) : values( map ) {}
        PropertyList( unsigned int computedSpecificity, const std::unordered_map< std::string, std::any >& map ) :
          computedSpecificity( computedSpecificity ), values( map ) {}

        template < typename VariantType > void set( const std::string& key, VariantType value ) {
          values[ key ] = value;
        };

        template < typename VariantType > const VariantType* get( const std::string& key ) const {
          auto it = values.find( key );
          if( it != values.end() ) {
            try {
              return &std::any_cast< VariantType >( it->second );
            } catch ( const std::bad_any_cast& e ) {
              Log::getInstance().warn( "PropertyList::get", key + " could not be converted to the requested type. Please check the type with propertyIsType()." );
            }
          }

          return nullptr;
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

    }
  }
}


#endif
