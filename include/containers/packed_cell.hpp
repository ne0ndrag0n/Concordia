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
		std::optional< T > object;
	};

	template< typename T >
	std::vector< PackedCell< T > > packCells( std::vector< BoundedObject< T > > items, int defaultWidth, int defaultHeight ) {
		int totalWidth = defaultWidth;
		int totalHeight = defaultHeight;
		std::vector< PackedCell< T > > packed;
		std::vector< PackedCell< T > > unpacked{ PackedCell< T >{ 0, 0, defaultWidth, defaultHeight, {} } };

		// Sort items by max( width, height )
		std::sort( items.begin(), items.end(), []( const BoundedObject< T >& boundedObject1, const BoundedObject< T >& boundedObject2 ) {
			return std::max( boundedObject1.width, boundedObject1.height ) < std::max( boundedObject2.width, boundedObject2.height );
		} );

		for( const auto& boundedObject : items ) {
			// Get first fit, if it exists
			auto nextFreeBox = unpacked.end();
			for( auto it = unpacked.begin(); it != unpacked.end(); ++it ) {
				if( boundedObject.width <= it->width && boundedObject.height <= it->height ) {
					nextFreeBox = it;
					break;
				}
			}

			if( nextFreeBox != unpacked.end() ) {
				// Use box that first fits
				// Data is placed in top left of next free box
				PackedCell< T > newCell {
					nextFreeBox->x,
					nextFreeBox->y,
					boundedObject.width,
					boundedObject.height,
					boundedObject.object
				};

				// New empty box is created to the left
				PackedCell< T > rightEmpty{
					nextFreeBox->x + boundedObject.width,
					nextFreeBox->y,
					nextFreeBox->width - boundedObject.width,
					boundedObject.height
				};

				// New empty box is created to the bottom
				PackedCell< T > bottomEmpty{
					nextFreeBox->x,
					nextFreeBox->y + boundedObject.height,
					nextFreeBox->width,
					nextFreeBox->height - boundedObject.height
				};

				unpacked.erase( nextFreeBox );

				// Do not place either new box if width/height is entirely used up
				if( rightEmpty.width > 0 && rightEmpty.height > 0 ) { unpacked.emplace_back( std::move( rightEmpty ) ); }
				if( bottomEmpty.width > 0 && bottomEmpty.height > 0 ) { unpacked.emplace_back( std::move( bottomEmpty ) ); }

				packed.emplace_back( std::move( newCell ) );
			} else {
				// Must add new cell to right of previous region
				PackedCell< T > newCell{ totalWidth, 0, boundedObject.width, boundedObject.height, boundedObject.object };

				// New cell goes underneath
				PackedCell< T > bottomEmpty{
					totalWidth,
					boundedObject.height,
					boundedObject.width,
					totalHeight - boundedObject.height
				};

				// This is at risk of continuously growing to the right!
				totalWidth += boundedObject.width;

				unpacked.emplace_back( std::move( bottomEmpty ) );
				packed.emplace_back( std::move( newCell ) );
			}
		}

		return packed;
	};

}

#endif