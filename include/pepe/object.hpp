#ifndef BBOBJECT
#define BBOBJECT

#include <string>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

namespace BlueBear {
	
	class Object {
		
		private:
			bool ok;
			lua_State* L;
		public:
			int luaVMInstance;
			int lotTableRef;
			std::string objType;
			Object( lua_State* L, const char* idKey );
			Object( lua_State* L, const char* idKey, char* popPackage, int popSize );
			void execute( unsigned int worldTicks );
			char* save();
			void load( char* pickledObject );
			bool good();
		
	};
	
}

#endif
