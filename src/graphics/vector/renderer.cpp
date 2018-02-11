#include "graphics/vector/renderer.hpp"
#include "device/display/display.hpp"
#include "configmanager.hpp"
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>
#include <nanovg_gl_utils.h>

namespace BlueBear {
  namespace Graphics {
    namespace Vector {

      Renderer::Renderer( Device::Display::Display& device ) : device( device ) {
        context = nvgCreateGL3( NVG_STENCIL_STROKES | NVG_DEBUG );
        device.getRenderWindow().setActive( true );
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

      void Renderer::drawRect( const glm::uvec4& dimensions, const glm::uvec4& color ) {
        checkTexture();

        nvgBeginPath( context );
        nvgRect( context, dimensions[ 0 ], dimensions[ 1 ], dimensions[ 2 ], dimensions[ 3 ] );
        nvgFillColor( context, nvgRGBA( color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] ) );
        nvgFill( context );
      }

      std::shared_ptr< Renderer::Texture > Renderer::createTexture( const glm::uvec2& dimensions, std::function< void( Renderer& ) > functor ) {
        device.executeOnSecondaryContext( secondaryGLContext, [ & ]() {
          currentTexture = std::make_shared< Renderer::Texture >( *this, dimensions );

          glEnable( GL_STENCIL_TEST );
          glDisable( GL_DEPTH_TEST );

          nvgluBindFramebuffer( currentTexture->framebuffer );
            glViewport( 0, 0, dimensions.x, dimensions.y );
            glClearColor( 0, 0, 0, 0 );
            glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

            nvgBeginFrame( context, ConfigManager::getInstance().getIntValue( "viewport_x" ), ConfigManager::getInstance().getIntValue( "viewport_y" ), 1.0f );
              functor( *this );
            nvgEndFrame( context );
          nvgluBindFramebuffer( nullptr );
        } );

        auto copy = currentTexture;
        currentTexture = nullptr;
        return copy;
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
