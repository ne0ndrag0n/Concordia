#ifndef BB_SERIALIZABLE
#define BB_SERIALIZABLE

#include <jsoncpp/json/json.h>

namespace BlueBear {

  class Serializable {
    virtual Json::Value save() = 0;
    virtual void load( const Json::Value& data ) = 0;
  };

}

#endif
