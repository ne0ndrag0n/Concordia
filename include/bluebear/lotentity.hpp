#ifndef BBOBJECT
#define BBOBJECT

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <string>

namespace BlueBear {

	class LotEntity {

		private:
			lua_State* L;
		public:
			std::string cid;
			bool ok = false;
			int luaVMInstance;
			std::string classID;
			LotEntity( lua_State* L, std::string& classID, std::string& instance );
			void execute( unsigned int currentTick );
			char* save();
			void load( char* pickledObject );
			bool good();
	};

}

#endif
