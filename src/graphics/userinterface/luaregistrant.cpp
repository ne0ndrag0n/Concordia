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

#include "log.hpp"

namespace BlueBear::Graphics::UserInterface {

  void LuaRegistrant::registerWidgets( sol::state& lua ) {
    sol::table gui = lua[ "bluebear" ][ "gui" ];
    sol::table types = lua.create_table();

    // If constructor is required here then all pure virtual functions must be removed
    gui.new_usertype< Element >(
      "Element",
      "get_tag", &Element::getTag,
      "get_id", &Element::getId,
      "has_class", &Element::hasClass,
      "get_selector_string", &Element::generateSelectorString,
      "get_children", &Element::getChildren,
      "get_absolute_position", []( std::shared_ptr< Element > self ) { return glm::vec2{ self->getAbsolutePosition() }; },
      "get_allocation", []( std::shared_ptr< Element > self ) { return glm::vec4{ self->getAllocation() }; }
    );

    gui.new_usertype< Widgets::Layout >(
      "Layout",
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Text >(
      "Text",
      "set_text", &Widgets::Text::setText,
      "get_text", &Widgets::Text::getText,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Window >(
      "Window",
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Button >(
      "Button",
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Input >(
      "Input",
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::TabLayout >(
      "TabLayout",
      "set_tab", &Widgets::TabLayout::selectElement,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Image >(
      "Image",
      "set_image", &Widgets::Image::setImage,
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Spacer >(
      "Spacer",
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Pane >(
      "Pane",
      sol::base_classes, sol::bases< Element >()
    );

    gui.new_usertype< Widgets::Scroll >(
      "Scroll",
      sol::base_classes, sol::bases< Element >()
    );

    gui[ "types" ] = types;
  }

}
