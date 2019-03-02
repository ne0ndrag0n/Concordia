#include "tools/sector_discovery.hpp"

namespace BlueBear::Tools::SectorDiscovery {

	void addEdge( SectorDiscoveryGraph& graph, const glm::ivec2& origin, const glm::ivec2& destination ) {
		graph[ origin ].links.emplace_back( &graph[ destination ] );
		graph[ destination ].links.emplace_back( &graph[ origin ] );

		graph[ origin ].position = origin;
		graph[ destination ].position = destination;
	}

	std::set< Sector > getSectors( const SectorDiscoveryNode* node, const SectorDiscoveryNode* parent, std::list< const SectorDiscoveryNode* > discovered ) {
		std::set< Sector > result;

		discovered.push_back( node );

		for( const SectorDiscoveryNode* link : node->links ) {
			if( link != parent ) {
				Sector potentialSector;
				bool seen = false;

				// Have we already seen this item?
				for( auto iterator = discovered.rbegin(); iterator != discovered.rend(); ++iterator ) {
					potentialSector.insert( *iterator );

					if( seen = ( *iterator == link ) ) {
						break;
					}
				}

				if( seen ) {
					// Item seen
					result.emplace( std::move( potentialSector ) );
				} else {
					// Item not seen and isn't parent. Go through it.
					std::set< Sector > linkResult = getSectors( link, node, discovered );
					result.insert( linkResult.begin(), linkResult.end() );
				}
			}
		}

		return result;
	}

}