#ifndef PER_FRAME_MODIFIER
#define PER_FRAME_MODIFIER

#include <functional>
#include <cstdlib>

namespace BlueBear::Tools {

	template< typename T >
	class FrameModifier {
		T data;
		std::function< void( const T& ) > modifier;
		int current = 0;
		int maxFrames = 0;
		int direction = 1;

	public:
		FrameModifier( const T& data, std::function< void( const T& ) > modifier, int maxFrames )
			: data( data ), modifier( modifier ), maxFrames( maxFrames ) {}

		int getDirection() {
			return direction;
		}

		void setDirection( int direction ) {
			this->direction = direction;
		}

		void reverseDirection() {
			if( direction > 0 ) {
				direction = -direction;
			} else {
				direction = std::abs( direction );
			}
		}

		bool update() {
			if( current < 0 || current > maxFrames ) {
				return false;
			}

			modifier( data );
			current += direction;

			return true;
		}
	};

}


#endif