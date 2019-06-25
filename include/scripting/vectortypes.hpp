#ifndef COREENGINE_VECTOR_TYPES
#define COREENGINE_VECTOR_TYPES

#include <sol.hpp>

namespace BlueBear::Scripting {

  struct VectorTypes {
    static void submitLuaContributions( sol::table types );
  };

}

#endif
