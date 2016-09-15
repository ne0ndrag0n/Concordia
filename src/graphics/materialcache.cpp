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
      texList.push_back( textureCache.get( tile.imagePath ) );

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

    /**
     * Get a material using the path to a texture atlas JSON definition, and the settings provided to the atlas builder.
     */
    std::shared_ptr< Material > MaterialCache::getAtlas( const AtlasPackage& package ) {
      // The key for a material based on texture caches is formed like so:
      // path.json:val1 val2 val3
      // As order matters in the TextureCache, it also matters in the MaterialCache

      std::string key = package.atlasJSONPath + ":";
      for( auto& kvPair : package.mappings ) {
        key = key + kvPair.second + " ";
      }

      auto iterator = materialCache.find( key );
      if( iterator != materialCache.end() ) {
        // This material was created previously
        return iterator->second;
      } else {
        // This material needs to be created
        TextureList texList;
        texList.push_back( textureCache.getUsingAtlas( package.atlasJSONPath, package.mappings ) );

        return materialCache[ key ] = std::make_shared< Material >( texList );
      }
    }

  }
}
