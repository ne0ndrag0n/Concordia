#ifndef ENGINE
#define ENGINE

#include "bbtypes.hpp"
#include "lot.hpp"
#include <string>
#include <vector>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#define BLUEBEAR_OBJECTS_DIRECTORY   "assets/objects/"
#define WORLD_TICKS_MAX				 604800000
#define	LUASPHERE_OBJECTS_TABLE		 "objects"
#define LUASPHERE_PLAYERS_TABLE		 "players"

using json = nlohmann::json;

namespace BlueBear {

	class Engine {

		private:
			lua_State* L;
			unsigned int worldTicks;
			Lot* currentLot;
			void createLotTable( Lot* lot );
			void callActionOnObject( const char* playerId, const char* obejctId, const char* method );

		public:
			Engine();
			~Engine();
			bool setupRootEnvironment();
			void objectLoop();
			bool loadLot( const char* lotPath );

	};

}

#endif
