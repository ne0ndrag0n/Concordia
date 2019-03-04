#version 330 core
in vec2 fragTexture;
in vec3 fragNormal;
in vec3 fragPos;
out vec4 color;

#define   YES    1.0f
#define   NO     0.0f

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

struct LineSegment {
  vec3 from;
  vec3 to;
};

struct SectorPolygon {
  int numSides;
  int lineSegmentIndices[ 32 ];
};

struct LightSector {
  DirectionalLight light;
  SectorPolygon polygon;
};

struct SectorIlluminator {
  LineSegment lineSegments[ 128 ];
  LightSector sectors[ 64 ];
  uint numSectors;
};

uniform vec3 cameraPos;
uniform Material material;
uniform DirectionalLight directionalLight;
uniform SectorIlluminator sectorIlluminator;

float segmentsIntersect( LineSegment line1, LineSegment line2 ) {
  // ta = (y3−y4)(x1−x3)+(x4−x3)(y1−y3)
  //      -----------------------------
  //      (x4−x3)(y1−y2)−(x1−x2)(y4−y3)

  // tb = (y1−y2)(x1−x3)+(x2−x1)(y1−y3)
  //      -----------------------------
  //      (x4−x3)(y1−y2)−(x1−x2)(y4−y3)

  // from - odd
  // to - even

  float denominator = ( ( line2.to.x - line2.from.x ) * ( line1.from.y - line1.to.y ) ) -
                      ( ( line1.from.x - line1.to.x ) * ( line2.to.y - line2.from.y ) );

  // collinear
  if( denominator == 0.0f ) {
    return NO;
  }

  // TODO: Check these with equation above
  float ta_numerator = ( ( line2.from.y - line2.to.y ) * ( line1.from.x - line2.from.x ) ) +
                       ( ( line2.to.x - line2.from.x ) * ( line1.from.y - line2.from.y ) );

  float tb_numerator = ( ( line1.from.y - line1.to.y ) * ( line1.from.x - line2.from.x ) ) +
                       ( ( line1.to.x - line1.from.x ) * ( line1.from.y - line2.from.y ) );


  float ta = ta_numerator / denominator;
  float tb = tb_numerator / denominator;

  if( ta >= 0.0f && ta <= 1.0f && tb >= 0.0f && tb <= 1.0f ) {
    return YES;
  } else {
    return NO;
  }
}

float pointInPolygon( vec2 point, SectorPolygon polygon ) {
  //LineSegment needle = LineSegment( point, vec3( 0.0, 0.0 ) );
  return 0.0f;
}

/**
 * Turn fragpos into a position in a sector (or lack thereof) and then return its according directional light
 */
DirectionalLight getDirectionalLightBySector() {
  // Check each sector and determine if fragPos lays within a sector


  return directionalLight;
}

void main() {
  DirectionalLight directionalLight = getDirectionalLightBySector();

  vec3 texResult = texture( material.diffuse0, fragTexture ).rgb;

  vec3 norm = normalize( fragNormal );
  vec3 lightDirection = normalize( -directionalLight.direction );
  float diffTheta = max( dot( norm, lightDirection ), 0.0 );

  vec3 ambient = directionalLight.ambient * texResult;
  vec3 diffuse = directionalLight.diffuse * diffTheta * texResult;

  color = vec4( ambient + diffuse, material.opacity );
}
