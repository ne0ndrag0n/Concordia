#include "graphics/userinterface/widgets/text.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "device/input/input.hpp"
#include "graphics/vector/renderer.hpp"
#include "graphics/userinterface/drawable.hpp"
#include "configmanager.hpp"
#include <cstdlib>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      namespace Widgets {

        Text::Text( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText ) : Element::Element( "Text", id, classes ), innerText( innerText ) {
          eventBundle.registerInputEvent( "mouse-in", std::bind( &Text::onMouseIn, this, std::placeholders::_1 ) );
          eventBundle.registerInputEvent( "mouse-out", std::bind( &Text::onMouseOut, this, std::placeholders::_1 ) );
        }

        void Text::onMouseIn( Device::Input::Metadata event ) {
          if( localStyle.get< bool >( "fade" ) ) {
            double fps = ConfigManager::getInstance().getIntValue( "fps_overview" );

            // Refresh and remove
            localStyle.attachAnimation( nullptr );

            // Attach animation
            localStyle.attachAnimation( std::make_unique< Style::Style::Animation >(
              &localStyle,
              std::map< double, Style::Style::Animation::Keyframe >{
                {
                  fps,
                  {
                    PropertyList( { { "background-color", localStyle.get< glm::uvec4 >( "fade-in-color" ) } } ),
                    true
                  }
                }
              },
              fps * 3.0,
              fps,
              false,
              true
            ) );
          }
        }

        void Text::onMouseOut( Device::Input::Metadata event ) {
          if( localStyle.get< bool >( "fade" ) ) {
            double fps = ConfigManager::getInstance().getIntValue( "fps_overview" );

            // Refresh and remove
            localStyle.attachAnimation( nullptr );

            // Attach animation
            localStyle.attachAnimation( std::make_unique< Style::Style::Animation >(
              &localStyle,
              std::map< double, Style::Style::Animation::Keyframe >{
                {
                  0.0,
                  {
                    PropertyList( { { "background-color", localStyle.get< glm::uvec4 >( "fade-in-color" ) } } ),
                    true
                  }
                },
                {
                  fps,
                  {
                    PropertyList( { { "background-color", localStyle.get< glm::uvec4 >( "background-color" ) } } ),
                    true
                  }
                }
              },
              fps * 3.0,
              fps,
              true,
              false
            ) );
          }
        }

        std::shared_ptr< Text > Text::create( const std::string& id, const std::vector< std::string >& classes, const std::string& innerText ) {
          std::shared_ptr< Text > text( new Text( id, classes, innerText ) );

          return text;
        }

        glm::uvec2 Text::getPosition() const {
          double fontSize = localStyle.get< double >( "font-size" );

          // Assumes
          //  text-orientation-vertical: Orientation::TOP
          //  text-orientation-horizontal: Orientation::MIDDLE;
          glm::uvec2 position{ ( allocation[ 2 ] / 2 ) - ( textSpan / 2 ), fontSize / 2 };

          if( localStyle.get< Orientation >( "text-orientation-vertical" ) == Orientation::MIDDLE ) {
            position.y = ( allocation[ 3 ] / 2 ) - 5;
          }

          return position;
        }

        void Text::render( Graphics::Vector::Renderer& renderer ) {
          double fontSize = localStyle.get< double >( "font-size" );

          renderer.drawRect(
            glm::uvec4{ 0, 0, allocation[ 2 ], allocation[ 3 ] },
            localStyle.get< glm::uvec4 >( "background-color" )
          );

          glm::uvec2 position = getPosition();

          if( localStyle.get< Orientation >( "text-alignment" ) == Orientation::LEFT ) {
            position.x = localStyle.get< int >( "padding" );
          }

          if( localStyle.get< Orientation >( "text-alignment" ) == Orientation::RIGHT ) {
            position.x = allocation[ 2 ] - textSpan - localStyle.get< int >( "padding" );
          }

          renderer.drawText(
            localStyle.get< std::string >( "font" ),
            innerText,
            position,
            localStyle.get< glm::uvec4 >( "color" ),
            fontSize
          );
        }

        void Text::calculate() {
          int padding = localStyle.get< int >( "padding" );
          double fontSize = localStyle.get< double >( "font-size" );
          glm::vec4 size = manager->getVectorRenderer().getTextSizeParams( localStyle.get< std::string >( "font" ), innerText, fontSize );
          textSpan = size[ 2 ];

          requisition = glm::uvec2{
            ( padding * 2 ) + textSpan,
            ( padding * 2 ) + fontSize
          };
        }

        void Text::reflow( bool selectorsInvalidated ) {
          if( auto parent = getParent() ) {
            parent->reflow( selectorsInvalidated );
          } else {
            Element::reflow( selectorsInvalidated );
          }
        }

        void Text::setText( const std::string& text, bool doReflow ) {
          innerText = text;

          if( doReflow ) {
            reflow();
          }
        }

        std::string Text::getText() {
          return innerText;
        }

      }
    }
  }
}
