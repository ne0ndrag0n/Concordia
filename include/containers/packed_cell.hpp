#ifndef PACK_CELL
#define PACK_CELL

#include "containers/bounded_object.hpp"
#include <vector>
#include <algorithm>

namespace BlueBear::Containers {

	template< typename T >
	struct PackedCell {
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
		std::optional< T > data;
	};

	template< typename T >
	static std::optional< PackedCell< T > > getFirstFit( const BoundedObject< T >& object, const std::vector< PackedCell< T > >& available ) {
		// TODO
		return {};
	};

	template< typename T >
	std::vector< PackedCell< T > > packCells( std::vector< BoundedObject< T > > items, int defaultWidth, int defaultHeight ) {
		std::vector< PackedCell< T > > packed;
		std::vector< PackedCell< T > > unpacked{ PackedCell< T >{ 0, 0, defaultWidth, defaultHeight, {} } };

		// Sort items by max( width, height )
		std::sort( items.begin(), items.end(), []( const BoundedObject< T >& boundedObject1, const BoundedObject< T >& boundedObject2 ) {
			return std::max( boundedObject1.width, boundedObject1.height ) < std::max( boundedObject2.width, boundedObject2.height );
		} );

		for( const auto& boundedObject : items ) {
			auto firstFit = getFirstFit( boundedObject, unpacked );
			if( firstFit ) {
				// Use box that first fits
				// TODO
			} else {
				// Must add new box to right of previous region
				// TODO
			}
		}

		return packed;
	};

}

#endif