#include "graphics/utilities/textureatlas.hpp"
#include "log.hpp"

namespace BlueBear::Graphics::Utilities {

  std::optional< std::pair< std::string, std::shared_ptr< sf::Image > > > TextureAtlas::getPairById( const std::string& id ) const {
    for( const auto& pair : stored ) {
      if( pair.first == id ) {
        return pair;
      }
    }

    return {};
  }

  glm::uvec2 TextureAtlas::computeTotalDimensions() const {
    glm::uvec2 totalDimensions;

    for( const auto& pair : stored ) {
      auto size = pair.second->getSize();

      totalDimensions = glm::uvec2{ totalDimensions.x + size.x, std::max( size.y, totalDimensions.y ) };
    }

    return totalDimensions;
  }

  void TextureAtlas::addTexture( const std::string& id, std::shared_ptr< sf::Image > image ) {
    if( getPairById( id ) ) {
      Log::getInstance().debug( "TextureAtlas::addTexture", "Texture with id " + id + " already belongs to this texture atlas." );
      return;
    }

    stored.emplace_back( id, image );
  }

  TextureAtlas::TextureData TextureAtlas::getTextureData( const std::string& id ) const {
    if( !getPairById( id ) ) {
      Log::getInstance().warn( "TextureAtlas::getTextureData", "Could not find texture id " + id + " in this atlas!" );
      throw IdNotFoundException();
    }

    glm::uvec2 totalDimensions = computeTotalDimensions();
    unsigned int currentIndex = 0;

    for( const auto& pair : stored ) {
      auto size = pair.second->getSize();

      if( pair.first == id ) {
        TextureAtlas::TextureData textureData;

        // found the lower-left corner...
        textureData.lowerCorner = glm::vec2{ ( ( float ) currentIndex ) / totalDimensions.x, 0.0f };
        textureData.upperCorner = glm::vec2{
          ( ( ( float ) currentIndex ) + size.x ) / totalDimensions.x,
          ( ( float ) size.y / totalDimensions.y )
        };

        return textureData;
      } else {
        currentIndex += size.x;
      }
    }
  }

  std::shared_ptr< sf::Image > TextureAtlas::generateAtlas() {
    std::shared_ptr< sf::Image > result = std::make_shared< sf::Image >();
    glm::uvec2 totalDimensions = computeTotalDimensions();
    result->create( totalDimensions.x, totalDimensions.y );

    unsigned int cursor = 0;
    for( const auto& pair : stored ) {
      auto size = pair.second->getSize();

      result->copy( *pair.second, cursor, totalDimensions.y - size.y );
      cursor += size.x;
    }

    return result;
  }

}
