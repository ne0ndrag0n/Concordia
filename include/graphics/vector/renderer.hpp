#ifndef VG_GRAPHICS_RENDERER
#define VG_GRAPHICS_RENDERER

#include "exceptions/genexc.hpp"
#include <nanovg.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SFML/Window/Context.hpp>
#include <memory>
#include <functional>

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
          const glm::uvec2& dimensions;

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

      public:
        Renderer( Device::Display::Display& device );
        ~Renderer();

        void drawRect( const glm::uvec4& dimensions, const glm::uvec4& color );
        void drawText( const std::string& fontFace, const std::string& text, const glm::uvec2& position, const glm::uvec4& color, double points );

        std::shared_ptr< Renderer::Texture > createTexture( const glm::uvec2& dimensions, std::function< void( Renderer& ) > functor );
      };

    }
  }
}

#endif
