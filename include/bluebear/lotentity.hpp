#ifndef BBOBJECT
#define BBOBJECT

#include "bbtypes.hpp"
#include <string>
#include <queue>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

namespace BlueBear {

	class LotEntity {

		private:
			lua_State* L;
		public:
			std::string cid;
			bool ok = false;
			int luaVMInstance;
			std::string classID;
			std::queue< BlueBear::ActionDescriptor > actionQueue;
			LotEntity( lua_State* L, const char* classID, const char* instance );
			void execute();
			char* save();
			void load( char* pickledObject );
			bool good();

			static int lua_getLotEntityObject( lua_State* L );

	};

}

#endif
