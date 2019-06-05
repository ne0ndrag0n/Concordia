/**
 * Grid Shader - Display a grid overlay on top of a plane
 */
#include "system/shaders/common/bounding_box.glsl"
#define  MAX_SELECTABLE_TILES 32

struct Grid {
	vec2 origin;
	vec2 dimensions;
	vec4 gridColor;
	float lineSize;
	float activated;
};

struct SelectedRegion {
	vec2 region;
	vec4 color;
};

uniform Grid grid;

uniform SelectedRegion selectedRegions[ MAX_SELECTABLE_TILES ];
uniform int numSelectedRegions;

vec4 getRegionColor( vec2 fragment ) {
	// Test fragment against a known list of bounding boxes that contain "selected" regions
	// If none match, return grid.gridColor
	for( int i = 0; i != numSelectedRegions; i++ ) {
		BoundingBoxVec2 region = BoundingBoxVec2(
			grid.origin + selectedRegions[ i ].region,
			grid.origin + selectedRegions[ i ].region + 1.0f
		);
		if( boundingBoxTest( region, fragment ) ) {
			return selectedRegions[ i ].color;
		}
	}

	return grid.gridColor;
}

vec4 getGridPixel( vec2 fragment ) {
	vec2 span = vec2( grid.origin.x + grid.dimensions.x, grid.origin.y + grid.dimensions.y );
	float lineSizeHalf = grid.lineSize / 2.0f;

	for( float y = grid.origin.y; y < grid.dimensions.y; y++ ) {
		BoundingBoxVec2 bound = BoundingBoxVec2( vec2( grid.origin.x, grid.origin.y + y - lineSizeHalf ), vec2( span.x, grid.origin.y + y + lineSizeHalf ) );

		if( boundingBoxTest( bound, fragment ) ) {
			return grid.activated * getRegionColor( fragment );
		}
	}

	for( float x = grid.origin.x; x < grid.dimensions.x; x++ ) {
		BoundingBoxVec2 bound = BoundingBoxVec2( vec2( grid.origin.x + x - lineSizeHalf, grid.origin.y ), vec2( grid.origin.x + x + lineSizeHalf, span.y ) );

		if( boundingBoxTest( bound, fragment ) ) {
			return grid.activated * getRegionColor( fragment );
		}
	}

	return vec4( 0.0f, 0.0f, 0.0f, 0.0f );
}