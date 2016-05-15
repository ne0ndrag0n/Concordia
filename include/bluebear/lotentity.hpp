#ifndef BBOBJECT
#define BBOBJECT

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "json/json.h"
#include <string>

namespace BlueBear {

	class LotEntity {

		private:
			lua_State* L;
			static Json::FastWriter writer;
		public:
			std::string cid;
			bool ok = false;
			int luaVMInstance;
			std::string classID;
			LotEntity( lua_State* L, const Json::Value& serialEntity );
			LotEntity( lua_State* L, const std::string& classID );

			void createEntityTable();
			void deserializeEntity( const Json::Value& serialEntity );
			void onCreate();
			void execute( unsigned int currentTick );
			char* save();
			void load( char* pickledObject );
			bool good();
	};

}

#endif
