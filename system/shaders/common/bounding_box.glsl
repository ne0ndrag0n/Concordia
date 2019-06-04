struct BoundingBoxVec2 {
	vec2 lowerLeft;
	vec2 upperRight;
};

bool boundingBoxTest( const BoundingBoxVec2 bound, const vec2 point ) {
	return point.x >= bound.lowerLeft.x && point.y >= bound.lowerLeft.y &&
		point.x <= bound.upperRight.x && point.y <= bound.upperRight.y;
}