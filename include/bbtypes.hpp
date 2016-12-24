#ifndef BLUEBEAR_TYPES
#define BLUEBEAR_TYPES

#include <chrono>
#include <cstdint>
#include <string>

namespace BlueBear {

	using Tick = unsigned long;
	using LuaReference = int;

	enum TerrainType { TERRAIN_GRASS, TERRAIN_DIRT, TERRAIN_SNOW, TERAIN_SAND, TERRAIN_MOON, TERRAIN_UNDERGROUND };

	enum ModpackStatus { UNINITIALIZED, LOADING, LOAD_SUCCESSFUL, LOAD_FAILED };
}

#endif
