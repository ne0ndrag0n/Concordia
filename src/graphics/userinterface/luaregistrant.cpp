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
#include "scripting/luakit/utility.hpp"
#include <unordered_map>
#include <memory>
#include <map>

namespace BlueBear::Graphics::UserInterface {

  void LuaRegistrant::registerWidgets( sol::state& lua ) {
    sol::table gui = lua[ "bluebear" ][ "gui" ];
    sol::table types = lua.create_table();

    // If constructor is required here then all pure virtual functions must be removed
    types.new_usertype< Element >(
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
      },
      "attach_animation", []( Element& self, sol::table options ) {
        std::map< double, Style::Style::Animation::Keyframe > keyframeMap;
        sol::table keyframes = Scripting::LuaKit::Utility::cast< sol::table >( options[ "keyframes" ] );

        for( auto& pair : keyframes ) {
          sol::table keyframe = Scripting::LuaKit::Utility::cast< sol::table >( pair.second );

          sol::table frames = Scripting::LuaKit::Utility::cast< sol::table >( keyframe[ "frames" ] );
          std::unordered_map< std::string, PropertyListType > properties;
          for( auto& frame : frames ) {
            properties[ Scripting::LuaKit::Utility::cast< std::string >( frame.first ) ] = Scripting::LuaKit::Utility::cast< PropertyListType >( frame.second );
          }

          keyframeMap.emplace(
            Scripting::LuaKit::Utility::cast< double >( pair.first ),
            Style::Style::Animation::Keyframe{
              properties,
              ( keyframe[ "interpolate" ] == sol::nil ) ? false : Scripting::LuaKit::Utility::cast< bool >( keyframe[ "interpolate" ] )
            }
          );
        }

        auto& style = self.getPropertyList();
        style.attachAnimation( std::make_unique< Style::Style::Animation >(
          &style,
          keyframeMap,
          Scripting::LuaKit::Utility::cast< double >( options[ "fps" ] ),
          Scripting::LuaKit::Utility::cast< double >( options[ "duration" ] ),
          ( options[ "suicide" ] == sol::nil ) ? true : Scripting::LuaKit::Utility::cast< bool >( options[ "suicide" ] ),
          ( options[ "sticky" ] == sol::nil ) ? false : Scripting::LuaKit::Utility::cast< bool >( options[ "sticky" ] )
        ) );
      },
      "remove_animation", []( Element& self ) {
        self.getPropertyList().attachAnimation( nullptr );
      }
    );

    types.new_usertype< Widgets::Layout >(
      "Layout",
      "new", sol::no_constructor,
      "create", []( const std::string& id, sol::table classes ) {
        return Widgets::Layout::create( id, Scripting::LuaKit::Utility::tableToVector< std::string >( classes ) );
      },
      sol::base_classes, sol::bases< Element >()
    );

    types.new_usertype< Widgets::Text >(
      "Text",
      "new", sol::no_constructor,
      "set_text", &Widgets::Text::setText,
      "get_text", &Widgets::Text::getText,
      "create", []( const std::string& id, sol::table classes, const std::string& innerText ) {
        return Widgets::Text::create( id, Scripting::LuaKit::Utility::tableToVector< std::string >( classes ), innerText );
      },
      sol::base_classes, sol::bases< Element >()
    );

    types.new_usertype< Widgets::Window >(
      "Window",
      "new", sol::no_constructor,
      "create", []( const std::string& id, sol::table classes, const std::string& windowTitle ) {
        return Widgets::Window::create( id, Scripting::LuaKit::Utility::tableToVector< std::string >( classes ), windowTitle );
      },
      sol::base_classes, sol::bases< Element >()
    );

    types.new_usertype< Widgets::WindowDecoration >(
      "WindowDecoration",
      "new", sol::no_constructor,
      sol::base_classes, sol::bases< Element >()
    );

    types.new_usertype< Widgets::Button >(
      "Button",
      "new", sol::no_constructor,
      "create", []( const std::string& id, sol::table classes, const std::string& innerText ) {
        return Widgets::Button::create( id, Scripting::LuaKit::Utility::tableToVector< std::string >( classes ), innerText );
      },
      sol::base_classes, sol::bases< Element >()
    );

    types.new_usertype< Widgets::Input >(
      "Input",
      "new", sol::no_constructor,
      "create", []( const std::string& id, sol::table classes, const std::string& hintText, const std::string& contents ) {
        return Widgets::Input::create( id, Scripting::LuaKit::Utility::tableToVector< std::string >( classes ), hintText, contents );
      },
      sol::base_classes, sol::bases< Element >()
    );

    types.new_usertype< Widgets::TabLayout >(
      "TabLayout",
      "new", sol::no_constructor,
      "set_tab", &Widgets::TabLayout::selectElement,
      "create", []( const std::string& id, sol::table classes ) {
        return Widgets::TabLayout::create( id, Scripting::LuaKit::Utility::tableToVector< std::string >( classes ) );
      },
      sol::base_classes, sol::bases< Element >()
    );

    types.new_usertype< Widgets::Image >(
      "Image",
      "new", sol::no_constructor,
      "set_image", &Widgets::Image::setImage,
      "create", []( const std::string& id, sol::table classes, const std::string& filePath ) {
        return Widgets::Image::create( id, Scripting::LuaKit::Utility::tableToVector< std::string >( classes ), filePath );
      },
      sol::base_classes, sol::bases< Element >()
    );

    types.new_usertype< Widgets::Spacer >(
      "Spacer",
      "new", sol::no_constructor,
      "create", []( const std::string& id, sol::table classes ) {
        return Widgets::Spacer::create( id, Scripting::LuaKit::Utility::tableToVector< std::string >( classes ) );
      },
      sol::base_classes, sol::bases< Element >()
    );

    types.new_usertype< Widgets::Pane >(
      "Pane",
      "new", sol::no_constructor,
      "create", []( const std::string& id, sol::table classes ) {
        return Widgets::Pane::create( id, Scripting::LuaKit::Utility::tableToVector< std::string >( classes ) );
      },
      sol::base_classes, sol::bases< Element >()
    );

    types.new_usertype< Widgets::Scroll >(
      "Scroll",
      "new", sol::no_constructor,
      "create", []( const std::string& id, sol::table classes ) {
        return Widgets::Scroll::create( id, Scripting::LuaKit::Utility::tableToVector< std::string >( classes ) );
      },
      sol::base_classes, sol::bases< Element >()
    );

    gui[ "types" ] = types;
  }

}
