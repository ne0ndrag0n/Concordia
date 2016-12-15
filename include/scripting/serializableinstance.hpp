#ifndef BBOBJECT
#define BBOBJECT

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <jsoncpp/json/json.h>
#include "bbtypes.hpp"
#include <string>
#include <exception>

namespace BlueBear {
	namespace Scripting {
		class SerializableInstance {

			private:
				lua_State* L;
				// Don't you EVER modify this value this outside of Engine! EVER!!
				// Read-only!! Do not TOUCH!
				const Tick& currentTickReference;

			public:
				std::string cid;
				int luaVMInstance;
				SerializableInstance( lua_State* L, const Tick& currentTickReference, int luaVMInstance );

				void execute();

				struct InvalidLuaVMInstanceException : public std::exception {

					const char* what() const throw() {
						return "Invalid Lua VM instance ID given to SerializableInstance";
					}

				};

				struct LuaValueNotTableException : public std::exception {

					const char* what() const throw() {
						return "Invalid table given to SerializableInstance";
					}

				};
		};
	}
}

#endif
