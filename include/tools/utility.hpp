#ifndef UTILITY
#define UTILITY

#include "exceptions/genexc.hpp"
#include <cstdint>
#include <jsoncpp/json/json.h>
#include <tinyxml2.h>
#include <cstddef>
#include <fstream>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <tbb/task_group.h>
#include <tbb/task.h>

namespace BlueBear {
	namespace Tools {
		class Utility {

			static constexpr const char* UNKNOWN_STRING = "unknown";
			static constexpr const char* DIRECTORY_STRING = "directory";
			static constexpr const char* FILE_STRING = "file";

			public:
				EXCEPTION_TYPE( InvalidJSONException, "Invalid JSON string!" );

				static std::vector< std::string > getSubdirectoryList( const char* rootSubDirectory );

				static std::vector<std::string> split(const std::string &text, char sep);

				static std::string join( const std::vector< std::string >& strings, const std::string& token );

				static bool isRLEObject( Json::Value& value );

				static std::string stringLtrim( std::string& s );

				static std::string stringRtrim( std::string& s );

				static std::string stringTrim( std::string s );

				static std::string pointerToString( const void* pointer );

				static void* stringToPointer( const std::string& str );

				static constexpr unsigned int hash( const char* str, int h = 0 ) {
					if( !str ) {
						str = "";
					}

					return !str[ h ] ? 5381 : ( hash( str, h+1 ) * 33 ) ^ str[ h ];
				};

				static std::string decodeUTF8( const std::string& encoded );

				/**
				 * C++ std::string is too fucking stupid to know what a null string is
				 */
				static inline const char* sanitizeCString( const char* string ) {
					return !string ? "" : string;
				};

				static std::string xmlToString( tinyxml2::XMLElement* element );

				static tinyxml2::XMLElement* getRootNode( tinyxml2::XMLDocument& document, const std::string& xmlString );

				// released under boost licence
				template < class T >
				static inline void hashCombine( std::size_t& seed, const T& v ) {
    			std::hash< T > hasher;
    			seed ^= hasher( v ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
				}

				static std::string generateIndentation( unsigned int amount );

				template< class T >
				static inline std::vector< T > concatArrays( const std::vector< T >& left, const std::vector< T >& right ) {
					std::vector< T > result = left;

					for( auto& r : right ) {
						result.push_back( r );
					}

					return result;
				};

				static constexpr const char* safeString( const char* cString ) {
					return cString ? cString : "";
				};

				static bool intersect( const glm::ivec2& pointer, const glm::ivec4& target ) {
					return pointer.x >= target[ 0 ] &&
								 pointer.y >= target[ 1 ] &&
								 pointer.x <= target[ 2 ] &&
								 pointer.y <= target[ 3 ];
				};

				template < typename Numeric > static constexpr Numeric interpolateLinear( Numeric v0, Numeric v1, double alpha ) {
					return ( 1 - alpha ) * v0 + alpha * v1;
				};

				static std::string sanitizeXML( const std::string& input );

				static Json::Value stringToJson( const std::string& string );

				static std::string jsonToString( const Json::Value& json );

				static std::pair< std::string, std::reference_wrapper< const Json::Value > > jsonIteratorToPair( Json::Value::const_iterator it );

				static Json::Value fileToJson( const std::string& path );

				template < typename GLMType > static inline std::string glmToString( const GLMType& type ) {
					return glm::to_string( type );
				};

				static bool equalEpsilon( float a, float b );
				static bool equalEpsilon( const glm::vec2& a, const glm::vec2& b );
				static bool equalEpsilon( const glm::vec3& a, const glm::vec3& b );
				static bool equalEpsilon( const glm::vec4& a, const glm::vec4& b );

				static glm::ivec2 normalize( const glm::ivec2& candidate );
				static int distance( const glm::ivec2& start, const glm::ivec2& end );
				static float distance( const glm::vec3& start, const glm::vec3& end );
				static float cross( const glm::vec2& a, const glm::vec2& b );

				template < typename T >
				static void runParallel( const std::vector< T >& collection, const std::function< void( const T& ) >& functor ) {
					if( collection.empty() ) {
						return;
					}

					int cores = std::max( (unsigned int) 1, std::thread::hardware_concurrency() );

					// Number of cores used to calculate items per bucket will need to be reduced
					// if there is a partially full bucket at the end
					int itemsPerBucket = collection.size() / cores;
					int bumps = collection.size() % cores;

					tbb::task_group group;
					int upperBound;
					for( int i = 0; i < collection.size(); i = upperBound ) {
						upperBound = std::min( (int) collection.size(), i + itemsPerBucket + ( bumps ? 1 : 0 ) );

						group.run( [ &collection, &functor, i, upperBound ]() {
							for( int index = i; index != upperBound; index++ ) {
								functor( collection[ index ] );
							}
						} );

						if( bumps ) {
							bumps--;
						}
					}

					// Block on bucketised, parallel task
					group.wait();
				};

				template< typename T >
				static inline T& getCircularIndex( std::vector< T >& list, int i ) {
					int size = list.size();

					return list[ ( i + size ) % size ];
				};

				static inline float positiveAngle( float angle ) {
					if( angle < 0.0f ) {
						return angle + 360.0f;
					}

					return ( int ) angle % 360;
				}
		};
	}
}


#endif
