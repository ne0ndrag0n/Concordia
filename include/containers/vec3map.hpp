#ifndef BB_SPARSE_COLLECTION
#define BB_SPARSE_COLLECTION

#include "tools/utility.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

namespace BlueBear {
  namespace Containers {

    template < typename Class > class Vec3Map {
      struct Hash {
        std::size_t operator()( const glm::vec3& key ) const {
          std::size_t seed = 0;

          Tools::Utility::hashCombine( seed, std::hash< double >( key.x ) );
          Tools::Utility::hashCombine( seed, std::hash< double >( key.y ) );
          Tools::Utility::hashCombine( seed, std::hash< double >( key.z ) );

          return seed;
        }
      };

      std::unordered_map< glm::vec3, Class, Hash > map;

    public:
      Class& get( const glm::vec3& key ) const {
        return map[ key ];
      }

      void set( const glm::vec3& key, const Class& value ) {
        map[ key ] = value;
      }

      void each( std::function< void( const glm::vec3&, Class& ) > predicate ) {
        for( auto& pair : map ) {
          predicate( pair.first, pair.second );
        }
      }

    };

  }
}

#endif
