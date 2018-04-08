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

      private:
        sf::Context secondaryGLContext;
        NVGcontext* context;
        Device::Display::Display& device;
        std::shared_ptr< Texture > currentTexture;

        void checkTexture();
        void loadFonts();
        void renderCurrentTexture( std::function< void( Renderer& ) > functor );

      public:
        Renderer( Device::Display::Display& device );
        ~Renderer();

        glm::vec4 getTextSizeParams( const std::string& fontFace, const std::string& text, double size );

        void drawRect( const glm::uvec4& dimensions, const glm::uvec4& color );
        void drawText( const std::string& fontFace, const std::string& text, const glm::uvec2& position, const glm::uvec4& color, double points );
        void drawLinearGradient( const glm::uvec4& dimensions, const glm::uvec4& begin, const glm::uvec4& end );

        std::shared_ptr< Renderer::Texture > createTexture(
          const glm::uvec2& dimensions,
          std::function< void( Renderer& ) > functor
        );
        void updateExistingTexture(
          std::shared_ptr< Renderer::Texture > texture,
          std::function< void( Renderer& ) > functor
        );
      };

    }
  }
}

#endif
