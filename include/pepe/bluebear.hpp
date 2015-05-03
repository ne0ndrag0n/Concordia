#ifndef BLUEBEAR
#define BLUEBEAR

#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#define INITIAL_SQVM_STACK_SIZE 1024

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
	
	class BBObject {
		private:
			const char* fileName;
			const char* fileContents;
		public:
			BBObject( const char* fileName );
			bool good();
			const char* getFileContents();
	};

	class Engine {
		private:
			lua_State* L;
			std::vector< BlueBear::BBObject > objects;

		public:
			Engine();
			~Engine();
			BlueBear::BBObject getObjectFromFile( const char* fileName );
	};
	
	namespace Utility {
		static void stackDump( lua_State* L );
	};
}

#endif
