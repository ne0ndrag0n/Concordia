#include "device/display/adapter/component/guicomponent.hpp"
#include "graphics/gui/luaelement.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <SFGUI/Widget.hpp>
#include <SFGUI/Entry.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {

          GuiComponent::GuiComponent( sf::RenderWindow& renderWindow ) : renderWindow( renderWindow ) {
            Graphics::GUI::LuaElement::masterSignalMap.clear();
            Graphics::GUI::LuaElement::masterAttrMap.clear();

            if( !desktop.LoadThemeFromFile( ConfigManager::getInstance().getValue( "ui_theme" ) ) ) {
              Log::getInstance().warn( "GuiComponent::GuiComponent", "ui_theme unable to load." );
            }

            enumerateSignals();
          }

          void GuiComponent::enumerateSignals() {
            // SFGUI will not set a constant on a signal until it is used
            // Sidestep this irritating behaviour here
            sfg::Widget::OnStateChange = sfg::Signal::GetGUID();
            sfg::Widget::OnGainFocus = sfg::Signal::GetGUID();
            sfg::Widget::OnLostFocus = sfg::Signal::GetGUID();

            sfg::Widget::OnExpose = sfg::Signal::GetGUID();

            sfg::Widget::OnSizeAllocate = sfg::Signal::GetGUID();
            sfg::Widget::OnSizeRequest = sfg::Signal::GetGUID();

            sfg::Widget::OnMouseEnter = sfg::Signal::GetGUID();
            sfg::Widget::OnMouseLeave = sfg::Signal::GetGUID();
            sfg::Widget::OnMouseMove = sfg::Signal::GetGUID();
            sfg::Widget::OnMouseLeftPress = sfg::Signal::GetGUID();
            sfg::Widget::OnMouseRightPress = sfg::Signal::GetGUID();
            sfg::Widget::OnMouseLeftRelease = sfg::Signal::GetGUID();
            sfg::Widget::OnMouseRightRelease = sfg::Signal::GetGUID();

            sfg::Widget::OnLeftClick = sfg::Signal::GetGUID();
            sfg::Widget::OnRightClick = sfg::Signal::GetGUID();

            sfg::Widget::OnKeyPress = sfg::Signal::GetGUID();
            sfg::Widget::OnKeyRelease = sfg::Signal::GetGUID();
            sfg::Widget::OnText = sfg::Signal::GetGUID();
            sfg::Entry::OnTextChanged = sfg::Signal::GetGUID();
          }

          std::queue< sf::Event > GuiComponent::getEvents() {
            std::queue< sf::Event > events;

            sf::Event event;
            while( renderWindow.pollEvent( event ) ) {
              events.push( event );
            }

            return events;
          }

          void GuiComponent::update() {
            glDisable( GL_DEPTH_TEST );
            desktop.Update( clock.restart().asSeconds() );
            sfgui.Display( renderWindow );
            glEnable( GL_DEPTH_TEST );
          }

        }
      }
    }
  }
}
