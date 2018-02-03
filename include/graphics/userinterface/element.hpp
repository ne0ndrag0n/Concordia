#ifndef NEW_GUI_ELEMENT
#define NEW_GUI_ELEMENT

#include "graphics/userinterface/style.hpp"
#include "graphics/userinterface/drawable.hpp"
#include <sol.hpp>
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <unordered_map>
#include <optional>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      class Element : public std::enable_shared_from_this< Element > {
        std::weak_ptr< Element > parent;
        const std::string& tag;
        std::string id;
        std::vector< std::string > classes;
        Style style;
        std::optional< Drawable > drawable;
        std::vector< std::shared_ptr< Element > > children;
        std::unordered_map< std::string, sol::function > events;

        Element( const std::string& tag, const std::string& id, const std::vector< std::string >& classes );
        Element( const Element& other );

      public:
        static std::shared_ptr< Element > create( const std::string& tag, const std::string& id, const std::vector< std::string >& classes );
        std::shared_ptr< Element > copy();

        const std::string& getTag() const;

        std::string getId() const;
        void setId( const std::string& id );

        std::vector< std::string > getClasses() const;
        bool hasClass() const;
        void addClass( const std::string& clss );
        void removeClass( const std::string& clss );

        void draw();
      };

    }
  }
}



#endif
