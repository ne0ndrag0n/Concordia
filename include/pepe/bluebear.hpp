#ifndef BLUEBEAR
#define BLUEBEAR

#include <string>
#include <vector>
#include <cstdio>
#include <fstream>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

/**
 * BlueBear objects
 *
 *
 */
namespace BlueBear {

	class Engine {
		private:
			lua_State* L;

		public:
			Engine();
			~Engine();
			bool setupRootEnvironment();
	};
	
	namespace Utility {
		static void stackDump( lua_State* L );
	};
}

#endif
