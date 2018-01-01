#ifndef BB_JSON_TOOLS
#define BB_JSON_TOOLS

#include "exceptions/genexc.hpp"
#include <jsoncpp/json/json.h>

namespace BlueBear {
  namespace Tools {

    class JsonTools {
    public:
      EXCEPTION_TYPE( MalformedRLEObjectException, "Malformed JSON RLE Object!" );
      static Json::Value getUncompressedRLEArray( const Json::Value& value );
    };

  }
}

#endif
