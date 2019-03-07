#ifndef SECTOR_DISCOVERY_NODE
#define SECTOR_DISCOVERY_NODE

#include "tools/vector_hash.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <set>
#include <list>

namespace BlueBear::Tools {

	struct SectorDiscoveryNode {
		glm::ivec2 position;
		bool visited = false;
		struct SectorDiscoveryNode* visitParent = nullptr;
		std::list< struct SectorDiscoveryNode* > links;
	};

	using SectorDiscoveryGraph = std::unordered_map< glm::ivec2, SectorDiscoveryNode >;
	using Sector = std::vector< const SectorDiscoveryNode* >;

	class SectorIdentifier {
		SectorDiscoveryGraph graph;
		std::unordered_map< const void*, std::set< const SectorDiscoveryNode* > > cached;

		const std::set< const SectorDiscoveryNode* >& getGeneratedSet( const Sector& sector );
		void addSectorToList( std::set< Sector >& targetSet, const Sector& newSector );
		std::set< Sector > getSectors( SectorDiscoveryNode* node, const SectorDiscoveryNode* parent, std::list< const SectorDiscoveryNode* > discovered = {} );


	public:
		void addEdge( const glm::ivec2& origin, const glm::ivec2& destination );

		std::set< Sector > getSectors();
	};
}

#endif
