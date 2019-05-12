#include "system/shaders/common/directional_light.glsl"

struct Level {
  vec2 origin;
  vec2 dimensions;
};

const float sectorResolution = 100.0f;
uniform DirectionalLight directionalLight;
uniform DirectionalLight sectorLights[ 16 ];
uniform sampler2D sectorMap0;
uniform Level levels[ 8 ];

/**
 * Turn fragpos into a position in a sector (or lack thereof) and then return its according directional light
 */
DirectionalLight getDirectionalLightBySector( const vec3 fragPos ) {
  int level = int( clamp( fragPos.z, 0, 3.402823466e+38 ) / 4 );
  vec2 lowerRightCorner = vec2(
    levels[ level ].origin.x + levels[ level ].dimensions.x,
    levels[ level ].origin.y - levels[ level ].dimensions.y
  );

  vec2 arrayCoordinates = vec2( fragPos.x, fragPos.y );

  // Convert to 0-n coordinate system
  arrayCoordinates.x = ( arrayCoordinates.x - levels[ level ].origin.x ) / ( lowerRightCorner.x - levels[ level ].origin.x );
  arrayCoordinates.y = ( levels[ level ].origin.y - arrayCoordinates.y ) / ( levels[ level ].origin.y - lowerRightCorner.y );

  int sectorIndex;

  switch( level ) {
    case 0:
      sectorIndex = int( texture( sectorMap0, arrayCoordinates ).r );
      break;
    default:
      sectorIndex = 0;
  }

  if( sectorIndex != 0 ) {
    return sectorLights[ sectorIndex - 1 ];
  } else {
    return directionalLight;
  }
}