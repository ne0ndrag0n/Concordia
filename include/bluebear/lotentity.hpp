#ifndef BBOBJECT
#define BBOBJECT

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "json/json.h"
#include "bbtypes.hpp"
#include <string>

namespace BlueBear {

	class LotEntity {

		private:
			lua_State* L;
			static Json::FastWriter writer;
			// Don't you EVER modify this value this outside of Engine! EVER!!
			// Read-only!! Do not TOUCH!
			const Tick& currentTickReference;

		public:
			std::string cid;
			bool ok = false;
			int luaVMInstance;
			std::string classID;
			LotEntity( lua_State* L, const Tick& currentTickReference, const Json::Value& serialEntity );
			LotEntity( lua_State* L, const Tick& currentTickReference, const std::string& classID );

			void createEntityTable();
			void deserializeEntity( const Json::Value& serialEntity );
			void onCreate();
			void execute();
			char* save();
			void load( char* pickledObject );
			bool good();
	};

}

#endif
