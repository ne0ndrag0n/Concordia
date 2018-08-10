#ifndef VG_GRAPHICS_RENDERER
#define VG_GRAPHICS_RENDERER

#include "exceptions/genexc.hpp"
#include <nanovg.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SFML/Window/Context.hpp>
#include <memory>
#include <functional>
#include <optional>
#include <glm/glm.hpp>
#include <string>

struct NVGLUframebuffer;

namespace BlueBear {
  namespace Device {
    namespace Display {
      class Display;
    }
  }

  namespace Graphics {
    namespace Vector {

      class Renderer {
      public:
        EXCEPTION_TYPE( UnboundTextureException, "Attempted vector graphics operation on unbound texture!" );

        // Make sure these are all gone before cleaning up the Renderer!
        class Texture {
          friend class Renderer;
          Renderer& parent;
          struct NVGLUframebuffer* framebuffer;
          glm::uvec2 dimensions;

        public:
          Texture( Renderer& renderer, const glm::uvec2& dimensions );
          ~Texture();

          GLuint getTextureId() const;
        };

        class Image {
          friend class Renderer;
          Renderer& parent;
          int imageHandle = -1;
          glm::uvec2 dimensions;

        public:
          EXCEPTION_TYPE( InvalidImageException, "Invalid image provided to vector renderer" );

          Image( Renderer& renderer, const std::string& path );
          ~Image();

          glm::uvec2 getDimensions() const;
          int getImageHandle() const;
        };

      private:
        sf::Context secondaryGLContext;
        NVGcontext* context;
        Device::Display::Display& device;
        std::shared_ptr< Texture > currentTexture;

        void checkTexture();
        void loadFonts();
        void render( std::function< void( Renderer& ) > frameFunctor, std::function< void() > postFunctor = {} );

      public:
        Renderer( Device::Display::Display& device );
        ~Renderer();

        void setAntiAlias( bool status );

        glm::vec4 getTextSizeParams( const std::string& fontFace, const std::string& text, double size );
        double getHorizontalAdvance( const std::string& fontFace, const std::string& text, double size );

        void drawImage( const Image& image, const glm::uvec2& position );
        void drawRect( const glm::uvec4& dimensions, const glm::uvec4& color );
        void drawText( const std::string& fontFace, const std::string& text, const glm::uvec2& position, const glm::uvec4& color, double points );
        void drawLinearGradient( const glm::uvec4& dimensions, const glm::uvec4& line, const glm::uvec4& begin, const glm::uvec4& end );
        void drawBoxGradient( const glm::uvec4& dimensions, const glm::uvec4& begin, const glm::uvec4& end, float feather, float borderRadius );
        void drawRadialGradient( const glm::uvec2& origin, float innerRadius, float outerRadius, const glm::uvec4& innerColor, const glm::uvec4& outerColor );
        void drawScissored( const glm::uvec4& scissorRegion, std::function< void() > callback );

        std::shared_ptr< Renderer::Texture > createTexture( const glm::uvec2& dimensions, std::function< void( Renderer& ) > functor );
        void updateExistingTexture( std::shared_ptr< Renderer::Texture > texture, std::function< void( Renderer& ) > functor );
        std::shared_ptr< unsigned char[] > generateBitmap( const glm::uvec2& dimensions, std::function< void( Renderer& ) > functor );
      };

    }
  }
}

#endif
