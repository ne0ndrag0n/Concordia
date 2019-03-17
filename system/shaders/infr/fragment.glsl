#version 330 core
in vec2 fragTexture;
in vec3 fragNormal;
in vec3 fragPos;
out vec4 color;

struct Material {
  sampler2D diffuse0;
  float shininess;
  float opacity;
};

struct DirectionalLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Sector {
  vec3 origin;
  vec2 dimensions;
};

uniform vec3 cameraPos;
uniform Material material;
uniform DirectionalLight directionalLight;
uniform float sectorResolution;
uniform DirectionalLight sectorLights[ 16 ];
uniform sampler2D sectorMaps[ 8 ];
uniform Sector sectors[ 8 ];

/**
 * Turn fragpos into a position in a sector (or lack thereof) and then return its according directional light
 */
DirectionalLight getDirectionalLightBySector() {
  int level = int( fragPos.z / 4 );
  vec2 lowerRightCorner = vec2(
    sectors[ level ].origin.x + sectors[ level ].dimensions.x,
    sectors[ level ].origin.y - sectors[ level ].dimensions.y
  );

  vec2 arrayCoordinates = vec2( fragPos.x, fragPos.y );

  // Convert to 0-n coordinate system
  arrayCoordinates.x = ( arrayCoordinates.x - sectors[ level ].origin.x ) / ( lowerRightCorner.x - sectors[ level ].origin.x );
  arrayCoordinates.y = ( sectors[ level ].origin.y - arrayCoordinates.y ) / ( sectors[ level ].origin.y - lowerRightCorner.y );

  int sectorIndex = int( texture( sectorMaps[ level ], arrayCoordinates ).r );

  if( sectorIndex != 0 ) {
    return sectorLights[ sectorIndex - 1 ];
  } else {
    return directionalLight;
  }
}

void main() {
  DirectionalLight light = getDirectionalLightBySector();

  vec3 texResult = texture( material.diffuse0, fragTexture ).rgb;

  vec3 norm = normalize( fragNormal );
  vec3 lightDirection = normalize( -light.direction );
  float diffTheta = max( dot( norm, lightDirection ), 0.0 );

  vec3 ambient = light.ambient * texResult;
  vec3 diffuse = light.diffuse * diffTheta * texResult;

  color = vec4( ambient + diffuse, material.opacity );
}
