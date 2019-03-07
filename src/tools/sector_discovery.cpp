#include "tools/sector_discovery.hpp"
#include <algorithm>

namespace BlueBear::Tools {

	void SectorIdentifier::addEdge( const glm::ivec2& origin, const glm::ivec2& destination ) {
		graph[ origin ].links.emplace_back( &graph[ destination ] );
		graph[ destination ].links.emplace_back( &graph[ origin ] );

		graph[ origin ].position = origin;
		graph[ destination ].position = destination;
	}

	static bool sectorsEquivalent( const Sector& needle, const Sector& value ) {
		// All nodes in needle must be in value
		for( const auto& node : needle ) {
			if( std::find( value.begin(), value.end(), node ) == value.end() ) {
				return false;
			}
		}

		return needle.size() == value.size();
	}

	static bool isSuperset( const Sector& superset, const Sector& set ) {
		// Every node in set must be in superset
		for( const auto& node : set ) {
			if( std::find( superset.begin(), superset.end(), node ) == superset.end() ) {
				return false;
			}
		}

		return true;
	}

	void SectorIdentifier::addSectorToList( SectorBundle& targetSet, const Sector& newSector ) {
		for( const auto& sector : targetSet ) {
			if( sectorsEquivalent( sector, newSector ) ) {
				return;
			}
		}

		targetSet.push_back( newSector );
	}

	SectorBundle SectorIdentifier::getSectors( SectorDiscoveryNode* node, const SectorDiscoveryNode* parent, std::list< const SectorDiscoveryNode* > discovered ) {
		SectorBundle result;

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
					SectorBundle linkResult = getSectors( link, node, discovered );
					for( const Sector& sector : linkResult ) {
						addSectorToList( result, sector );
					}
				}
			}
		}

		return result;
	}

	SectorBundle SectorIdentifier::getSectors() {
		if( graph.empty() ) {
			return {};
		}

		std::vector< SectorBundle > sectorGroups;
		for( auto& pair : graph ) {
			if( !pair.second.visited ) {
				sectorGroups.emplace_back( getSectors( &pair.second, nullptr ) );
			}
		}

		// Remove sectors that contain other sectors
		SectorBundle finalSet;

		for( const auto& result : sectorGroups ) {
			for( const Sector& needle : result ) {
				bool superset = false;

				// Does needle set contain any of the other sets?
				for( const Sector& value : result ) {
					if( &needle != &value ) {
						if( superset = isSuperset( needle, value ) ) {
							break;
						}
					}
				}

				if( !superset ) {
					addSectorToList( finalSet, needle );
				}
			}
		}

		return finalSet;
	}



}