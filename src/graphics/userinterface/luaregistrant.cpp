#include "graphics/userinterface/luaregistrant.hpp"
#include "graphics/userinterface/widgets/layout.hpp"
#include "graphics/userinterface/widgets/text.hpp"
#include "graphics/userinterface/widgets/window.hpp"
#include "graphics/userinterface/widgets/button.hpp"
#include "graphics/userinterface/widgets/input.hpp"
#include "graphics/userinterface/widgets/tablayout.hpp"
#include "graphics/userinterface/widgets/image.hpp"
#include "graphics/userinterface/widgets/spacer.hpp"
#include "graphics/userinterface/widgets/pane.hpp"
#include "graphics/userinterface/widgets/scroll.hpp"
#include "graphics/userinterface/style/style.hpp"
#include "graphics/userinterface/propertylist.hpp"

namespace BlueBear::Graphics::UserInterface {

  void LuaRegistrant::registerWidgets( sol::state& lua ) {
    sol::table gui = lua[ "bluebear" ][ "gui" ];
    sol::table types = lua.create_table();

    // If constructor is required here then all pure virtual functions must be removed
    gui.new_usertype< Element >(
      "Element",
      "new", sol::no_constructor,
      "get_tag", &Element::getTag,
      "get_id", &Element::getId,
      "has_class", &Element::hasClass,
      "get_selector_string", &Element::generateSelectorString,
      "get_children", &Element::getChildren,
      "get_absolute_position", []( Element& self ) { return glm::vec2{ self.getAbsolutePosition() }; },
      "get_allocation", []( Element& self ) { return glm::vec4{ self.getAllocation() }; },
      "get_style_property", []( Element& self, const std::string& id ) -> PropertyListType {
        PropertyListType type = self.getPropertyList().hierarchy( id );
        if( auto uvec4 = std::get_if< glm::uvec4 >( &type ) ) {
          return glm::vec4( *uvec4 );
        } else {
          return type;
        }
      },
      "set_style_property", []( Element& self, const std::string& id, PropertyListType value ) {
        self.getPropertyList().setDirect( id, value );
      }
    );

    gui.new_usertype< Widgets::Layout >(
      "Layout",
      "new", sol::no_constructor,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Text >(
      "Text",
      "new", sol::no_constructor,
      "set_text", &Widgets::Text::setText,
      "get_text", &Widgets::Text::getText,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Window >(
      "Window",
      "new", sol::no_constructor,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Button >(
      "Button",
      "new", sol::no_constructor,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Input >(
      "Input",
      "new", sol::no_constructor,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::TabLayout >(
      "TabLayout",
      "new", sol::no_constructor,
      "set_tab", &Widgets::TabLayout::selectElement,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Image >(
      "Image",
      "new", sol::no_constructor,
      "set_image", &Widgets::Image::setImage,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Spacer >(
      "Spacer",
      "new", sol::no_constructor,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Pane >(
      "Pane",
      "new", sol::no_constructor,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Scroll >(
      "Scroll",
      "new", sol::no_constructor,
      sol::base_classes, sol::bases< Element >()
    );

    gui[ "types" ] = types;
  }

}
