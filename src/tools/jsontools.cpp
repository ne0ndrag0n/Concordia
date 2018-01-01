#include "tools/jsontools.hpp"
#include "tools/utility.hpp"

namespace BlueBear {
  namespace Tools {

    Json::Value JsonTools::getUncompressedRLEArray( const Json::Value& value ) {
      if( !value.isArray() ) {
        throw MalformedRLEObjectException();
      }

      Json::Value result( Json::arrayValue );
      for( auto it = value.begin(); it != value.end(); ++it ) {
        Json::Value key = it.key();
        Json::Value value = *it;

        if( Tools::Utility::isRLEObject( value ) ) {
          unsigned int run = value[ "run" ].asUInt();
          for( unsigned int i = 0; i != run; i++ ) {
            result.append( value[ "value" ] );
          }
        } else {
          result.append( value );
        }
      }

      return result;
    }

  }
}
