#include "system/shaders/common/directional_light.glsl"

#define MAX_ROOMS 16
#define MAP_RESOLUTION 100.0f

struct Room {
	vec2 lowerLeft;
	vec2 upperRight;
	ivec2 mapLocation;
	int level; // -1 for invalid room
};

uniform Room rooms[ MAX_ROOMS ];

// roomData shall be a 2d array flipped vertically for opengl
// Pack these in a manner that is space-efficient
uniform sampler2D roomData;

bool fragmentInBox( const Room room, const vec2 fragment ) {
	return fragment.x >= room.lowerLeft.x &&
		fragment.x <= room.upperRight.x &&
		fragment.y <= room.lowerLeft.y &&
		fragment.y >= room.upperRight.y;
}

float lookupFragment( const Room room, const vec2 fragment ) {
	vec2 multiplier = round( ( fragment - room.lowerLeft ) * MAP_RESOLUTION );
	multiplier.y = 1000.0f - multiplier.y;

	ivec2 regionCoords = ivec2( multiplier );
	return texelFetch( roomData, room.mapLocation + regionCoords, 0 ).r;
}

DirectionalLight getRoomLight( const vec3 fragment ) {
	// Clamp z to all positive values
	float level = clamp( fragment.z, 0.0f, 3.402823466e+38 ) / 4.0f;

	float lightIndex = 0.0f;
	for( int i = 0; i != MAX_ROOMS; i++ ) {
		float lookup = lookupFragment( rooms[ i ], fragment.xy );

		if(
			lightIndex == 0.0f &&
			fragmentInBox( rooms[ i ], fragment.xy ) == true &&
			float( rooms[ i ].level ) == level &&
			lookup != 0.0f
		) {
			lightIndex = lookup;
		}
	}

	return directionalLights[ int( lightIndex ) ];					// If we truly don't hit on a sector, return the outdoor light at position 0
}