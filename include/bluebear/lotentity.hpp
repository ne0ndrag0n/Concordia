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
			bool ok = false;
			lua_State* L;
		public:
			int luaVMInstance;
			int lotTableRef;
			std::string objType;
			BlueBear::LotEntityType lotEntityType;
			std::queue< BlueBear::ActionDescriptor > actionQueue;
			LotEntity( lua_State* L, json savedTable );
			void execute( unsigned int worldTicks );
			char* save();
			void load( char* pickledObject );
			bool good();

			static int lua_getLotEntityObject( lua_State* L );

	};

}

#endif
