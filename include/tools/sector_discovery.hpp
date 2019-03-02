#ifndef SECTOR_DISCOVERY_NODE
#define SECTOR_DISCOVERY_NODE

#include "tools/vector_hash.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <set>
#include <list>

namespace BlueBear::Tools::SectorDiscovery {

	struct SectorDiscoveryNode {
		glm::ivec2 position;
		struct SectorDiscoveryNode* visitParent = nullptr;
		std::list< struct SectorDiscoveryNode* > links;
	};

	using SectorDiscoveryGraph = std::unordered_map< glm::ivec2, SectorDiscoveryNode >;
	using Sector = std::set< const SectorDiscoveryNode* >;

	void addEdge( SectorDiscoveryGraph& graph, const glm::ivec2& origin, const glm::ivec2& destination );
	std::set< Sector > getSectors( const SectorDiscoveryNode* node, const SectorDiscoveryNode* parent, std::list< const SectorDiscoveryNode* > discovered = {} );

}

#endif
