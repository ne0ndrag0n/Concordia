#ifndef WALLJOINT
#define WALLJOINT

#include "exceptions/genexc.hpp"
#include "tools/cardinal.hpp"
#include <jsoncpp/json/json.h>
#include <vector>

namespace BlueBear::Models {

  struct WallJoint {
    bool north = false;
    bool east = false;
    bool west = false;
    bool south = false;

    bool northeast = false;
    bool southeast = false;
    bool southwest = false;
    bool northwest = false;

    void setByCardinalDirection( const Tools::CardinalDirection& direction );

    bool isElbow() const;

    EXCEPTION_TYPE( InvalidTypeException, "Expected array type" );

    static std::vector< std::vector< WallJoint > > createFromList( unsigned int x, unsigned int y, const Json::Value& array );
  };

}

#endif
