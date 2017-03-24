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

#define VERIFY_USER_DATA( tag, func ) \
  if( !lua_isuserdata( L, -1 ) ) { \
    Log::getInstance().warn( tag, "Argument 1 provided to " func " must be a userdata." ); \
    return 0; \
  }

#endif
