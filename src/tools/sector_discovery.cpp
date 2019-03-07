#include "tools/sector_discovery.hpp"
#include <algorithm>

namespace BlueBear::Tools {

	void SectorIdentifier::addEdge( const glm::ivec2& origin, const glm::ivec2& destination ) {
		graph[ origin ].links.emplace_back( &graph[ destination ] );
		graph[ destination ].links.emplace_back( &graph[ origin ] );

		graph[ origin ].position = origin;
		graph[ destination ].position = destination;
	}

	const std::set< const SectorDiscoveryNode* >& SectorIdentifier::getGeneratedSet( const Sector& sector ) {
		if( cached.find( &sector ) == cached.end() ) {
			auto& set = cached[ &sector ];
			for( const auto& vertex : sector ) {
				set.insert( vertex );
			}
		}

		return cached[ &sector ];
	}

	void SectorIdentifier::addSectorToList( std::set< Sector >& targetSet, const Sector& newSector ) {
		// Convert sector to set
		std::set< const SectorDiscoveryNode* > needle;
		for( const auto& vertex : newSector ) {
			needle.insert( vertex );
		}

		// If there is no difference between this and other sectors, we already added it
		for( const Sector& sector : targetSet ) {
			// No intersection = this sector was already added, don't add it
			if( needle == getGeneratedSet( sector ) ) {
				// Don't add it
				return;
			}
		}

		targetSet.insert( newSector );
	}

	std::set< Sector > SectorIdentifier::getSectors( SectorDiscoveryNode* node, const SectorDiscoveryNode* parent, std::list< const SectorDiscoveryNode* > discovered ) {
		std::set< Sector > result;

		node->visited = true;
		discovered.push_back( node );

		for( SectorDiscoveryNode* link : node->links ) {
			if( link != parent ) {
				Sector potentialSector;
				bool seen = false;

				// Have we already seen this item?
				for( auto iterator = discovered.rbegin(); iterator != discovered.rend(); ++iterator ) {
					potentialSector.push_back( *iterator );

					if( seen = ( *iterator == link ) ) {
						break;
					}
				}

				if( seen ) {
					// Item seen
					addSectorToList( result, potentialSector );
				} else {
					// Item not seen and isn't parent. Go through it.
					std::set< Sector > linkResult = getSectors( link, node, discovered );
					for( const Sector& sector : linkResult ) {
						addSectorToList( result, sector );
					}
				}
			}
		}

		return result;
	}

	std::set< Sector > SectorIdentifier::getSectors() {
		if( graph.empty() ) {
			return {};
		}

		cached.clear();

		std::vector< std::set< Sector > > sectorGroups;
		for( auto& pair : graph ) {
			if( !pair.second.visited ) {
				sectorGroups.emplace_back( getSectors( &pair.second, nullptr ) );
			}
		}

		// Remove sectors that contain other sectors
		std::set< Sector > finalSet;

		for( const auto& result : sectorGroups ) {
			for( const Sector& needle : result ) {
				auto needleSet = getGeneratedSet( needle );
				bool isSuperset = false;

				// Does needle set contain any of the other sets?
				for( const Sector& value : result ) {
					if( &needle != &value ) {
						auto valueSet = getGeneratedSet( value );
						if( isSuperset = std::includes( needleSet.begin(), needleSet.end(), valueSet.begin(), valueSet.end() ) ) {
							break;
						}
					}
				}

				if( !isSuperset ) {
					finalSet.insert( needle );
				}
			}
		}

		return finalSet;
	}



}