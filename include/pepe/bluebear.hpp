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

#define BLUEBEAR_OBJECTS_DIRECTORY "assets/objects/"

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
			void objectLoop();
	};
	
	namespace Utility {
		static void stackDump( lua_State* L );
		
		std::vector< std::string > getSubdirectoryList( const char* rootSubDirectory );
	};
}

#endif
