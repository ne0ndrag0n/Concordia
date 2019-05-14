#include "system/shaders/common/directional_light.glsl"
#include "system/shaders/common/cond.glsl"

#define MAX_ROOMS 16

struct Room {
	vec2 lowerLeft;
	vec2 upperRight;
	uint level;
	uint roomId; // object is invalid if this is 0
};

uniform Room rooms[ MAX_ROOMS ];

uniform int availableRooms;

uniform sampler2D sector0;
uniform sampler2D sector1;
uniform sampler2D sector2;
uniform sampler2D sector3;
uniform sampler2D sector4;
uniform sampler2D sector5;
uniform sampler2D sector6;
uniform sampler2D sector7;
uniform sampler2D sector8;
uniform sampler2D sector9;
uniform sampler2D sector10;
uniform sampler2D sector11;
uniform sampler2D sector12;
uniform sampler2D sector13;
uniform sampler2D sector14;
uniform sampler2D sector15;

float fragmentInBox( const Room room, const vec2 fragment ) {
	vec2 stp = step( room.lowerLeft, fragment ) - step( room.upperRight, fragment );
	return stp.x * stp.y;
}

DirectionalLight getRoomLight( const vec3 fragment ) {
	float level = clamp( fragment.z, 0.0f, 3.402823466e+38 ) / 4.0f;

	// step( needle, value ) - if value >= needle, return 1
	float lightIndex = 0.0f;
	for( int i = 0; i != MAX_ROOMS; i++ ) {
		float prevNonzero = step( 1.0f, lightIndex );
		float roomIndex = fragmentInBox( rooms[ i ], fragment.xy ) * equalSwitch( float( rooms[ i ].level ), level ) * float( rooms[ i ].roomId );
		lightIndex = ( prevNonzero * lightIndex ) + ( ( 1 - prevNonzero ) * roomIndex );
	}

	return directionalLights[ lightIndex ];
}