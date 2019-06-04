/**
 * Grid Shader - Display a grid overlay on top of a plane
 */
#include "system/shaders/common/bounding_box.glsl"

struct Grid {
	vec2 origin;
	vec2 dimensions;
	vec4 gridColor;
	float lineSize;
};

uniform Grid grid;

vec4 getGridPixel( vec2 fragment ) {
	vec2 span = vec2( grid.origin.x + grid.dimensions.x, grid.origin.y + grid.dimensions.y );
	float lineSizeHalf = grid.lineSize / 2.0f;

	for( float y = grid.origin.y; y < span.y; y++ ) {
		BoundingBoxVec2 bound = BoundingBoxVec2( vec2( grid.origin.x, grid.origin.y + y - lineSizeHalf ), vec2( span.x, grid.origin.y + y + lineSizeHalf ) );

		if( boundingBoxTest( bound, fragment ) ) {
			return grid.gridColor;
		}
	}

	for( float x = grid.origin.x; x < span.x; x++ ) {
		BoundingBoxVec2 bound = BoundingBoxVec2( vec2( grid.origin.x + x - lineSizeHalf, grid.origin.y ), vec2( grid.origin.x + x + lineSizeHalf, span.y ) );

		if( boundingBoxTest( bound, fragment ) ) {
			return grid.gridColor;
		}
	}

	return vec4( 0.0f, 0.0f, 0.0f, 0.0f );
}