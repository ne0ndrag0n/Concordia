#include <iostream>
#include <glm/glm.hpp>

#define   YES    1.0f
#define   NO     0.0f

struct LineSegment {
  glm::vec2 from;
  glm::vec2 to;
  float level;
};


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

float lineIntersect1() {
	LineSegment line1{ { 0.0, 0.0 }, { 0.0, 3.0 } };
	LineSegment line2{ { 1.0, 0.0 }, { 1.0, 3.0 } };

	return segmentsIntersect( line1, line2 );
}

float lineIntersect2() {
	LineSegment line1{ { 1.0, 1.0 }, { 1.0, 3.0 } };
	LineSegment line2{ { 0.0, 2.0 }, { 3.0, 2.0 } };

	return segmentsIntersect( line1, line2 );
}

float lineIntersect3() {
	LineSegment line1{ { 0.0, 3.0 }, { 3.0, 0.0 } };
	LineSegment line2{ { 0.0, 0.0 }, { 3.0, 3.0 } };

	return segmentsIntersect( line1, line2 );
}

int main() {
	std::cout << "Concordia TestSuite v0.0.1" << std::endl;

	// Line segment code as used in fragment.glsl for infrastructure
	std::cout << "Expect line 0,0-0,3 and 1,0-1,3 not to intersect: " << ( lineIntersect1() == NO ? "pass" : "fail" ) << std::endl;
	std::cout << "Expect line 1,1-1,3 and 0,2-3,2 to intersect: " << 	( lineIntersect2() == YES ? "pass" : "fail" ) << std::endl;
	std::cout << "Expect line 0,0-3,3 and 0,3-3,0 to intersect: " << 	( lineIntersect3() == YES ? "pass" : "fail" ) << std::endl;


	return 0;
}