#ifndef SECTOR_DISCOVERY_NODE
#define SECTOR_DISCOVERY_NODE

#include "tools/vector_hash.hpp"
#include <glm/glm.hpp>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace BlueBear::Tools::SectorDiscovery {

	struct SectorDiscoveryNode {
		enum class VisitStatus { UNVISITED, PARTIALLY_VISITED, COMPLETELY_VISITED };

		glm::ivec2 position;
		VisitStatus visitStatus = VisitStatus::UNVISITED;
		struct SectorDiscoveryNode* visitParent = nullptr;
		std::list< struct SectorDiscoveryNode* > links;
	};

	using SectorDiscoveryGraph = std::unordered_map< glm::ivec2, SectorDiscoveryNode >;
	using Sector = std::vector< glm::ivec2 >;
	using SectorList = std::list< Sector >;

	void addEdge( SectorDiscoveryGraph& graph, const glm::ivec2& origin, const glm::ivec2& destination );
	void findSectors( SectorDiscoveryNode* current, SectorDiscoveryNode* previous, SectorList& list );

}

#endif
