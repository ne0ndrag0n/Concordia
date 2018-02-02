#ifndef NEW_GUI_DRAWABLE
#define NEW_GUI_DRAWABLE

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {

      class Drawable {
        unsigned int glHandle;

      public:
        Drawable( unsigned int x, unsigned int y, unsigned int width, unsigned int height );
        ~Drawable();
      };

    }
  }
}

#endif
