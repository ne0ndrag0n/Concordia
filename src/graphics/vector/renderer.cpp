#include "graphics/vector/renderer.hpp"
#include "device/display/display.hpp"
#include "configmanager.hpp"
#include "tools/utility.hpp"
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

      void Renderer::setAntiAlias( bool status ) {
        nvgShapeAntiAlias( context, status ? 1 : 0 );
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

      double Renderer::getHorizontalAdvance( const std::string& fontFace, const std::string& text, double size ) {
        float garbage[ 4 ];
        nvgFontSize( context, size );
        nvgFontFace( context, fontFace.c_str() );
        return nvgTextBounds( context, 0, 0, text.c_str(), NULL, garbage );
      }

      void Renderer::drawImage( const Image& image, const glm::uvec2& position ) {
        checkTexture();

        int handle = image.getImageHandle();
        glm::uvec2 dimensions = image.getDimensions();

        auto pattern = nvgImagePattern( context, position.x, position.y, dimensions.x, dimensions.y, 0.0f, handle, 1.0f );

        nvgBeginPath( context );
        nvgRect( context, position.x, position.y, dimensions.x, dimensions.y );
        nvgFillPaint( context, pattern );
        nvgFill( context );
      }

      void Renderer::drawRect( const glm::uvec4& dimensions, const glm::uvec4& color ) {
        checkTexture();

        nvgBeginPath( context );
        nvgRect( context, dimensions[ 0 ], dimensions[ 1 ], dimensions[ 2 ] - dimensions[ 0 ], dimensions[ 3 ] - dimensions[ 1 ] );
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

      void Renderer::drawLinearGradient( const glm::uvec4& dimensions, const glm::uvec4& line, const glm::uvec4& begin, const glm::uvec4& end ) {
        checkTexture();

        auto fill = nvgLinearGradient( context, line[ 0 ], line[ 1 ], line[ 2 ], line[ 3 ],
          nvgRGBA( begin[ 0 ], begin[ 1 ], begin[ 2 ], begin[ 3 ] ),
          nvgRGBA( end[ 0 ], end[ 1 ], end[ 2 ], end[ 3 ] )
        );

        nvgBeginPath( context );
        nvgRect( context, dimensions[ 0 ], dimensions[ 1 ], dimensions[ 2 ] - dimensions[ 0 ], dimensions[ 3 ] - dimensions[ 1 ] );
        nvgFillPaint( context, fill );
        nvgFill( context );
      }

      void Renderer::drawBoxGradient( const glm::uvec4& dimensions, const glm::uvec4& begin, const glm::uvec4& end, float feather, float borderRadius ) {
        checkTexture();

        auto fill = nvgBoxGradient( context, dimensions[ 0 ], dimensions[ 1 ], dimensions[ 2 ] - dimensions[ 0 ], dimensions[ 3 ] - dimensions[ 1 ], borderRadius, feather,
          nvgRGBA( begin[ 0 ], begin[ 1 ], begin[ 2 ], begin[ 3 ] ),
          nvgRGBA( end[ 0 ], end[ 1 ], end[ 2 ], end[ 3 ] )
        );

        nvgBeginPath( context );
        nvgRoundedRect( context, dimensions[ 0 ], dimensions[ 1 ], dimensions[ 2 ] - dimensions[ 0 ], dimensions[ 3 ] - dimensions[ 1 ], borderRadius );
        nvgFillPaint( context, fill );
        nvgFill( context );
      }

      void Renderer::drawRadialGradient( const glm::uvec2& origin, float innerRadius, float outerRadius, const glm::uvec4& innerColor, const glm::uvec4& outerColor ) {
        checkTexture();

        float totalRadius = innerRadius + outerRadius;

        auto fill = nvgRadialGradient( context, origin.x, origin.y, innerRadius, outerRadius,
          nvgRGBA( innerColor[ 0 ], innerColor[ 1 ], innerColor[ 2 ], innerColor[ 3 ] ),
          nvgRGBA( outerColor[ 0 ], outerColor[ 1 ], outerColor[ 2 ], outerColor[ 3 ] )
        );

        nvgBeginPath( context );
        nvgRect( context, origin.x - totalRadius, origin.y - totalRadius, origin.x + totalRadius, origin.y + totalRadius );
        nvgFillPaint( context, fill );
        nvgFill( context );
      }

      void Renderer::drawScissored( const glm::uvec4& scissorRegion, std::function< void() > callback ) {
        nvgScissor( context, scissorRegion[ 0 ], scissorRegion[ 1 ], scissorRegion[ 2 ] - scissorRegion[ 0 ], scissorRegion[ 3 ] - scissorRegion[ 1 ] );
        callback();
        nvgResetScissor( context );
      }

      void Renderer::render( std::function< void( Renderer& ) > frameFunctor, std::function< void() > postFunctor ) {
        checkTexture();

        glEnable( GL_STENCIL_TEST );
        glDisable( GL_DEPTH_TEST );

        nvgluBindFramebuffer( currentTexture->framebuffer );
          glViewport( 0, 0, currentTexture->dimensions.x, currentTexture->dimensions.y );
          glClearColor( 0, 0, 0, 0 );
          glClear( GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

          nvgBeginFrame( context, currentTexture->dimensions.x, currentTexture->dimensions.y, 1.0f );
            frameFunctor( *this );
          nvgEndFrame( context );

          if( postFunctor ) {
            postFunctor();
          }
        nvgluBindFramebuffer( nullptr );
      }

      std::shared_ptr< Renderer::Texture > Renderer::createTexture( const glm::uvec2& dimensions, std::function< void( Renderer& ) > functor ) {
        device.executeOnSecondaryContext( secondaryGLContext, [ & ]() {
          currentTexture = std::make_shared< Renderer::Texture >( *this, dimensions );
          render( functor );
        } );

        auto copy = currentTexture;
        currentTexture = nullptr;
        return copy;
      }

      void Renderer::updateExistingTexture( std::shared_ptr< Renderer::Texture > texture, std::function< void( Renderer& ) > functor ) {
        device.executeOnSecondaryContext( secondaryGLContext, [ & ]() {
          currentTexture = texture;
          render( functor );
        } );

        currentTexture = nullptr;
      }

      std::shared_ptr< unsigned char[] > Renderer::generateBitmap( const glm::uvec2& dimensions, std::function< void( Renderer& ) > functor ) {
        unsigned char* array = new unsigned char[ ( dimensions.x * dimensions.y ) * 4 ];

        device.executeOnSecondaryContext( secondaryGLContext, [ & ]() {
          currentTexture = std::make_shared< Renderer::Texture >( *this, dimensions );
          render( functor, [ & ]() {
            glReadBuffer( GL_COLOR_ATTACHMENT0 );
            glReadPixels( 0, 0, dimensions.x, dimensions.y, GL_RGBA, GL_UNSIGNED_BYTE, array );
          } );
        } );

        return std::shared_ptr< unsigned char[] >( array );
      }

      Renderer::Texture::Texture( Renderer& renderer, const glm::uvec2& dimensions ) : parent( renderer ), dimensions( dimensions ) {
        framebuffer = nvgluCreateFramebuffer( parent.context, dimensions.x, dimensions.y, NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY );
        if( !framebuffer ) {
          Log::getInstance().error( "Renderer::Texture::Texture",
            std::string( "Attempt to create vector renderer framebuffer failed!\n" ) +
            "\tDimensions: { " + std::to_string( dimensions.x ) + ", " + std::to_string( dimensions.y ) +  " }\n" +
            "\tContext: " + Tools::Utility::pointerToString( parent.context )
          );
        }
      }

      Renderer::Texture::~Texture() {
        parent.device.executeOnSecondaryContext( parent.secondaryGLContext, [ & ]() {
          nvgluDeleteFramebuffer( framebuffer );
        } );
      }

      GLuint Renderer::Texture::getTextureId() const {
        return framebuffer->texture;
      }

      Renderer::Image::Image( Renderer& renderer, const std::string& path ) : parent( renderer ) {
        parent.device.executeOnSecondaryContext( parent.secondaryGLContext, [ & ]() {
          imageHandle = nvgCreateImage( parent.context, path.c_str(), 0 );
          if( imageHandle == -1 ) {
            Log::getInstance().error( "Renderer::Image::Image", "Failed to load " + path );
            throw InvalidImageException();
          }

          int width, height;
          nvgImageSize( parent.context, imageHandle, &width, &height );
          dimensions.x = width;
          dimensions.y = height;
        } );
      }

      Renderer::Image::~Image() {
        parent.device.executeOnSecondaryContext( parent.secondaryGLContext, [ & ]() {
          nvgDeleteImage( parent.context, imageHandle );
        } );
      }

      glm::uvec2 Renderer::Image::getDimensions() const {
        return dimensions;
      }

      int Renderer::Image::getImageHandle() const {
        return imageHandle;
      }

    }
  }
}
