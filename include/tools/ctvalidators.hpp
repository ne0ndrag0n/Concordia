/**
 * (C)ompile (T)ime Validators
 */
#ifndef CTVALIDATORS
#define CTVALIDATORS

#define VERIFY_STRING( tag, func ) \
  if( !lua_isstring( L, -1 ) ) { \
    Log::getInstance().warn( tag, "Argument 1 provided to " func " must be a string." ); \
    return 0; \
  }

#define VERIFY_STRING_N( tag, func, n ) \
  if( !lua_isstring( L, -n ) ) { \
    Log::getInstance().warn( tag, "Argument " #n " provided to " func " must be a string." ); \
    return 0; \
  }

#define VERIFY_USER_DATA( tag, func ) \
  if( !lua_isuserdata( L, -1 ) ) { \
    Log::getInstance().warn( tag, "Argument 1 provided to " func " must be a userdata." ); \
    return 0; \
  }

#define VERIFY_NUMBER_N( tag, func, n ) \
  if( !lua_isnumber( L, -n ) ) { \
    Log::getInstance().warn( tag, "Argument " #n " provided to " func " must be a number." ); \
    return 0; \
  }

#define VERIFY_BOOLEAN_N( tag, func, n ) \
  if( !lua_isboolean( L, -n ) ) { \
    Log::getInstance().warn( tag, "Argument " #n " provided to " func " must be a boolean." ); \
    return 0; \
  }

#define VERIFY_FUNCTION_N( tag, func, n ) \
  if( !lua_isfunction( L, -n ) ) { \
    Log::getInstance().warn( tag, "Argument " #n " provided to " func " must be a function." ); \
    return 0; \
  }

#define VERIFY_TABLE_N( tag, func, n ) \
  if( !lua_istable( L, -n ) ) { \
    Log::getInstance().warn( tag, "Argument " #n " provided to " func " must be a table." ); \
    return 0; \
  }

#define VERIFY_POINTER( ptr, tag ) \
  if( !( ptr ) ) { \
    Log::getInstance().warn( tag, "This pseudo-element must first be associated with an element object." ); \
    return 0; \
  }

#endif
