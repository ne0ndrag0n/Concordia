#include "system/shaders/common/directional_light.glsl"
#include "system/shaders/common/cond.glsl"

#define MAX_ROOMS 16

struct Room {
	vec2 lowerLeft;
	vec2 upperRight;
	uint level;
	uint samplerId; // object is invalid if this is 0
};

uniform Room rooms[ MAX_ROOMS ];
uniform int availableRooms;

uniform sampler2D sector0;
uniform sampler2D sector1;
uniform sampler2D sector2;
uniform sampler2D sector3;
// TODO

float fragmentInBox( const Room room, const vec2 fragment ) {
	vec2 stp = step( room.lowerLeft, fragment ) - step( room.upperRight, fragment );
	return stp.x * stp.y;
}

float lookupFragment( const Room room, const vec2 fragment ) {
	vec2 textureCoordinates = ( fragment - room.lowerLeft ) / ( room.upperRight - room.lowerLeft );

	float lightIndex = 0.0f;

	// does this break the universe?
	switch( room.samplerId ) {
		default:
		case 0:
			lightIndex = texture( sector0, textureCoordinates ).r;
			break;
		case 1:
			lightIndex = texture( sector1, textureCoordinates ).r;
			break;
		case 2:
			lightIndex = texture( sector2, textureCoordinates ).r;
			break;
		case 3:
			lightIndex = texture( sector3, textureCoordinates ).r;
			break;
		// TODO
	}

	return
		whenGreaterEqual( textureCoordinates.x, 0.0f ) *
		whenGreaterEqual( textureCoordinates.y, 0.0f ) *
		whenLessEqual( textureCoordinates.x, 1.0f ) *
		whenLessEqual( textureCoordinates.y, 1.0f ) *
		lightIndex;
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