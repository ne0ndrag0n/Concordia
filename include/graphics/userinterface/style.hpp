#ifndef NEW_GUI_STYLE
#define NEW_GUI_STYLE

#include <memory>
#include <variant>
#include <string>
#include <unordered_map>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      class Element;

      class Style {
      public:
        using Variant = std::variant< std::string, int, double >;
        struct Value {
          Variant defined;
          Variant computed;
        };

      private:
        std::unordered_map< std::string, Value > values;

      public:
        std::weak_ptr< Element > parent;

        Style() = default;
        Style( const Style& other );

        Value getValue( const std::string& id );
        void setValue( const std::string& id, const std::string& value );
        void setValue( const std::string& id, const int value );
        void setValue( const std::string& id, const double value );
      };

    }
  }
}

#endif
