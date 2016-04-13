#ifndef BBOBJECT
#define BBOBJECT

#include "bbtypes.hpp"
#include "json.hpp"
#include <string>
#include <queue>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

using json = nlohmann::json;

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
			void execute( unsigned int worldTicks );
			char* save();
			void load( char* pickledObject );
			bool good();

			static int lua_getLotEntityObject( lua_State* L );

	};

}

#endif
