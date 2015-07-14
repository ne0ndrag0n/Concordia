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
			bool ok = false;
			lua_State* L;
		public:
			int luaVMInstance;
			int lotTableRef;
			std::string objType;
			BlueBear::LotEntityType lotEntityType;
			std::queue< BlueBear::ActionDescriptor > actionQueue;
			LotEntity( lua_State* L, const char* idKey );
			LotEntity( lua_State* L, const char* idKey, char* popPackage, int popSize, BlueBear::LotEntityType lotEntityType );
			void execute( unsigned int worldTicks );
			char* save();
			void load( char* pickledObject );
			bool good();
			
			static int lua_getLotEntityObject( lua_State* L );
		
	};
	
}

#endif
