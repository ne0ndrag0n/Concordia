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

struct Polygon {
  uint numSides;
  uint sides[ 16 ];
};

struct Sector {
  DirectionalLight light;
  Polygon polygon;
};

struct SectorIlluminator {
  LineSegment lineSegments[ 64 ];
  Sector sectors[ 32 ];
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

float getPolygonMaxX( Polygon polygon ) {
  float maxX = 1.175494351e-38;

  for( uint i = uint( 0 ); i != polygon.numSides; i++ ) {
    maxX = max(
      maxX,
      max(
        sectorIlluminator.lineSegments[ polygon.sides[ i ] ].from.x,
        sectorIlluminator.lineSegments[ polygon.sides[ i ] ].to.x
      )
    );
  }

  return maxX;
}

/**
 * Turn fragpos into a position in a sector (or lack thereof) and then return its according directional light
 */
DirectionalLight getDirectionalLightBySector() {
  // Check each sector and determine if fragPos lays within a sector
  for( uint i = uint( 0 ); i != sectorIlluminator.numSectors; i++ ) {
    // For each sector, create a line from fragPos to ( MAX( sectorX ) + 1.0f )
    // Test line against each edge of the sector
    // Point is in polygon if number of intersections is odd - use this polygon's directional light
    Sector sector = sectorIlluminator.sectors[ i ];
    LineSegment needle = LineSegment( fragPos, vec3( getPolygonMaxX( sector.polygon ) + 1.0f, fragPos.y, fragPos.z ) );
    int fragLevel = int( fragPos.z / 4 );

    uint intersectionCount = uint( 0 );
    for( uint j = uint( 0 ); j != sector.polygon.numSides; j++ ) {
      // test intersection along infinite z (x & y only)
      LineSegment value = sectorIlluminator.lineSegments[ sector.polygon.sides[ j ] ];
      if( segmentsIntersect( needle, value ) == YES && fragLevel == int( value.from.z / 4 ) ) {
        // Must intersect within +- 4.0f
        // Two line segments in a polygon side are guaranteed to be at identical levels
        intersectionCount++;
      }
    }

    if( mod( intersectionCount, 2 ) != 0 ) {
      // odd means IN!
      return sector.light;
    }
  }

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
