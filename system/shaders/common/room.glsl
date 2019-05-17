#include "system/shaders/common/directional_light.glsl"
#include "system/shaders/common/cond.glsl"

#define MAX_ROOMS 16
#define MAP_RESOLUTION 100

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

float fragmentInBox( const Room room, const vec2 fragment ) {
	vec2 stp = step( room.lowerLeft, fragment ) - step( room.upperRight, fragment );
	return stp.x * stp.y;
}

float lookupFragment( const Room room, const vec2 fragment ) {
	ivec2 regionCoords = ( fragment - room.lowerLeft ) * MAP_RESOLUTION;

	return texelFetch( roomData, room.mapLocation + regionCoords, 0 ).r;
}

DirectionalLight getRoomLight( const vec3 fragment ) {
	// Clamp z to all positive values
	float level = clamp( fragment.z, 0.0f, 3.402823466e+38 ) / 4.0f;

	float lightIndex = 0.0f;
	for( int i = 0; i != MAX_ROOMS; i++ ) {
		lightIndex =
			whenEqual( lightIndex, 0.0f ) *							// lightIndex must not have been set before
			fragmentInBox( rooms[ i ], fragment.xy ) *				// fragPos intersects room bounding box
			whenEqual( float( rooms[ i ].level ), level ) *			// room is on the same level as this fragment
			lookupFragment( rooms[ i ], fragment.xy );				// Lookup conducted on fragment returns actual index
	}

	return directionalLights[ lightIndex ];							// If we truly don't hit on a sector, return the outdoor light at position 0
}