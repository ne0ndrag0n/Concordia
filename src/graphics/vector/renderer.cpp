#include "graphics/vector/renderer.hpp"
#include "device/display/display.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>
#include <nanovg_gl_utils.h>
#include <fstream>
#include <jsoncpp/json/json.h>

namespace BlueBear {
  namespace Graphics {
    namespace Vector {

      Renderer::Renderer( Device::Display::Display& device ) :
        secondaryGLContext( device.getDefaultContextSettings(), device.getDimensions().x, device.getDimensions().y ),
        device( device ) {
        device.executeOnSecondaryContext( secondaryGLContext, [ & ]() {
          context = nvgCreateGL3( NVG_STENCIL_STROKES | NVG_DEBUG );
          if( context == NULL ) {
            Log::getInstance().error( "Renderer::Renderer", "NanoVG failed to init" );
            return;
          }
          loadFonts();
        } );
      }

      Renderer::~Renderer() {
        device.executeOnSecondaryContext( secondaryGLContext, [ & ]() {
          nvgDeleteGL3( context );
        } );
      }

      void Renderer::checkTexture() {
        if( !currentTexture ) {
          throw UnboundTextureException();
        }
      }

      void Renderer::loadFonts() {
        std::ifstream fonts( "system/ui/fonts.json" );
        if( fonts.is_open() && fonts.good() ) {
          Json::Value fontJson;
          Json::Reader reader;
          if( reader.parse( fonts, fontJson ) && fontJson.isObject() ) {
            for( Json::Value::iterator jsonIterator = fontJson.begin(); jsonIterator != fontJson.end(); ++jsonIterator ) {
              std::string key = jsonIterator.key().asString();
              std::string value = ( *jsonIterator ).asString();

              if( nvgCreateFont( context, key.c_str(), value.c_str() ) == -1 ) {
                Log::getInstance().warn( "Renderer::loadFonts", std::string( "Failed to load font face " ) + key + " at path " + value );
              }
            }

            return;
          }
        }

        Log::getInstance().error( "Renderer::loadFonts", "Failed to load font data. Fonts may not display in the UI!" );
      }

      glm::vec4 Renderer::getTextSizeParams( const std::string& fontFace, const std::string& text, double size ) {
        float local[ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
        nvgFontSize( context, size );
        nvgFontFace( context, fontFace.c_str() );
        nvgTextBounds( context, 0, 0, text.c_str(), NULL, local );
        return glm::vec4{ local[ 0 ], local[ 1 ], local[ 2 ], local[ 3 ] };
      }

      void Renderer::drawRect( const glm::uvec4& dimensions, const glm::uvec4& color ) {
        checkTexture();

        nvgBeginPath( context );
        nvgRect( context, dimensions[ 0 ], dimensions[ 1 ], dimensions[ 2 ], dimensions[ 3 ] );
        nvgFillColor( context, nvgRGBA( color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] ) );
        nvgFill( context );
      }

      void Renderer::drawText( const std::string& fontFace, const std::string& text, const glm::uvec2& position, const glm::uvec4& color, double points ) {
        checkTexture();

        nvgFontSize( context, points );
        nvgFontFace( context, fontFace.c_str() );
        nvgFillColor( context, nvgRGBA( color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] ) );

        nvgTextAlign( context, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE );
        nvgText( context, position.x, position.y, text.c_str(), NULL );
      }

      void Renderer::renderCurrentTexture( std::function< void( Renderer& ) > functor ) {
        checkTexture();

        glEnable( GL_STENCIL_TEST );
        glDisable( GL_DEPTH_TEST );

        nvgluBindFramebuffer( currentTexture->framebuffer );
          glViewport( 0, 0, currentTexture->dimensions.x, currentTexture->dimensions.y );
          glClearColor( 0, 0, 0, 0 );
          glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

          nvgBeginFrame( context, currentTexture->dimensions.x, currentTexture->dimensions.y, 1.0f );
            functor( *this );
          nvgEndFrame( context );
        nvgluBindFramebuffer( nullptr );
      }

      std::shared_ptr< Renderer::Texture > Renderer::createTexture( const glm::uvec2& dimensions, std::function< void( Renderer& ) > functor ) {
        device.executeOnSecondaryContext( secondaryGLContext, [ & ]() {
          currentTexture = std::make_shared< Renderer::Texture >( *this, dimensions );
          renderCurrentTexture( functor );
        } );

        auto copy = currentTexture;
        currentTexture = nullptr;
        return copy;
      }

      void Renderer::updateExistingTexture( std::shared_ptr< Renderer::Texture > texture, std::function< void( Renderer& ) > functor ) {
        device.executeOnSecondaryContext( secondaryGLContext, [ & ]() {
          currentTexture = texture;
          renderCurrentTexture( functor );
        } );

        currentTexture = nullptr;
      }

      Renderer::Texture::Texture( Renderer& renderer, const glm::uvec2& dimensions ) : parent( renderer ), dimensions( dimensions ) {
        framebuffer = nvgluCreateFramebuffer( parent.context, dimensions.x, dimensions.y, NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY );
      }

      Renderer::Texture::~Texture() {
        parent.device.executeOnSecondaryContext( parent.secondaryGLContext, [ & ]() {
          nvgluDeleteFramebuffer( framebuffer );
        } );
      }

      GLuint Renderer::Texture::getTextureId() const {
        return framebuffer->texture;
      }

    }
  }
}
