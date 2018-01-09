#ifndef BB_MODEL_TILE
#define BB_MODEL_TILE

#include <memory>

namespace BlueBear {
  namespace Graphics {
    class Texture;
  }

  namespace Models {

    struct Tile {
      const std::string& id;
      const std::string& sound;
      const std::shared_ptr< Graphics::Texture > appearance;
      const double price;
    };

  }
}

#endif
