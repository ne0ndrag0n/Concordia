#ifndef NEW_GUI_STYLE
#define NEW_GUI_STYLE

#include <variant>
#include <string>
#include <unordered_map>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      class Style {
      public:
        using Variant = std::variant< std::string, int, double >;
      private:
        std::weak_ptr< Element > parent;
        std::unordered_map< std::string, Variant > values;

      public:
        Style( std::weak_ptr< Element > parent );
        Style( const Style& other, std::weak_ptr< Element > newParent );

        Variant getValue( const std::string& id );
        void setValue( const std::string& id, const std::string& value );
        void setValue( const std::string& id, const int value );
        void setValue( const std::string& id, const double value );
      };

    }
  }
}

#endif
