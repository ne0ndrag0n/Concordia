#include "tools/sector_discovery.hpp"

namespace BlueBear::Tools::SectorDiscovery {

	void addEdge( SectorDiscoveryGraph& graph, const glm::ivec2& origin, const glm::ivec2& destination ) {
		graph[ origin ].links.emplace_back( &graph[ destination ] );
		graph[ destination ].links.emplace_back( &graph[ origin ] );

		graph[ origin ].position = origin;
		graph[ destination ].position = destination;
	}

	void findSectors( SectorDiscoveryNode* current, SectorDiscoveryNode* previous, SectorList& list ) {
		if( current->visitStatus == SectorDiscoveryNode::VisitStatus::COMPLETELY_VISITED ) {
			return;
		}

		if( current->visitStatus == SectorDiscoveryNode::VisitStatus::PARTIALLY_VISITED ) {
			Sector cycle;
			cycle.emplace_back( current->position );

			SectorDiscoveryNode* backtrackStep = previous;
			while( backtrackStep != current ) {
				cycle.emplace_back( backtrackStep->position );
				backtrackStep = backtrackStep->visitParent;
			}

			list.emplace_back( std::move( cycle ) );
			return;
		}

		current->visitParent = previous;
		current->visitStatus = SectorDiscoveryNode::VisitStatus::PARTIALLY_VISITED;

		for( SectorDiscoveryNode* linkage : current->links ) {
			if( linkage == previous ) {
				continue;
			}

			findSectors( linkage, current, list );
		}

		current->visitStatus = SectorDiscoveryNode::VisitStatus::COMPLETELY_VISITED;
	}

}