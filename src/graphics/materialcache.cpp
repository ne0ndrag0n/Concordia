#include "graphics/materialcache.hpp"
#include "scripting/tile.hpp"
#include "graphics/material.hpp"
#include "graphics/texture.hpp"
#include <memory>
#include <utility>

namespace BlueBear {
  namespace Graphics {

    /**
     * Use the info present in the tile to get a material, or create one.
     */
    std::shared_ptr< Material > MaterialCache::get( Scripting::Tile& tile ) {
      auto materialIterator = materialCache.find( tile.id );

      if( materialIterator != materialCache.end() ) {
        return materialIterator->second;
      }

      // Need to create
      TextureList texList;
      texList.push_back( textureCache.get( tile.id, tile.imagePath ) );

      auto material = std::make_shared< Material >( texList );
      materialCache[ tile.id ] = material;
      return material;
    }

    void MaterialCache::prune() {
      auto iterator = std::begin( materialCache );

      while( iterator != std::end( materialCache ) ) {
        if( iterator->second.unique() ) {
          iterator = materialCache.erase( iterator );
        } else {
          iterator++;
        }
      }
    }

  }
}
