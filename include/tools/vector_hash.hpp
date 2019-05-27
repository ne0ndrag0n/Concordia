#ifndef VECTOR_HASH
#define VECTOR_HASH

#include <glm/glm.hpp>
#include <functional>

namespace std {
	template <> struct hash< glm::ivec2 > {
		size_t operator()( const glm::ivec2& vec ) const {
			size_t res = 17;
			res = res * 31 + hash< int >()( vec.x );
			res = res * 31 + hash< int >()( vec.y );
			return res;
		}
	};

	template <> struct hash< glm::vec2 > {
		size_t operator()( const glm::vec2& vec ) const {
			size_t res = 17;
			res = res * 31 + hash< float >()( vec.x );
			res = res * 31 + hash< float >()( vec.y );
			return res;
		}
	};
}

#endif